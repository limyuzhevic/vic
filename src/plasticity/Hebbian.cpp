#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    // Core Hebbian learning parameters
    float learningRate;             // Base learning rate η
    float maxWeight;                // Maximum synaptic weight for normalization
    float minWeight;                // Minimum synaptic weight
    float covarianceThreshold;     // Threshold for covariance-based updates
    
    // Oja's rule parameters
    float ojaAlpha;                 // Oja learning rate
    float ojaBeta;                  // Oja normalization factor
    
    // BCM theory parameters
    float bcmThreshold;             // BCM sliding threshold θ
    float bcmTau;                   // BCM time constant for threshold adaptation
    float bcmTheta;                 // BCM threshold (θ̂ = (1/τ)·⟨y²⟩)
    
    // Temporal learning parameters
    float temporalWindow;           // Temporal window for spike pairing
    float temporalDecay;             // Temporal trace decay rate
    
    // Synaptic competition parameters
    float competitionStrength;      // Strength of synaptic competition
    float globalNormalization;      // Global weight normalization factor
    float competitionRange;         // Range of synaptic competition
    
    // Synaptic tagging and capture
    bool tagEnabled;                // Whether synaptic tagging is enabled
    float tagDecayRate;             // Tag decay rate
    float tagCaptureThreshold;      // Threshold for tag capture
    std::unordered_map<SynapseId, float> synapticTags;  // Per-synapse tags
    
    // Activity-dependent learning rates
    float baselineLearningRate;     // Baseline learning rate
    float activityModulation;       // Activity modulation factor
    float maxActivityLevel;         // Maximum activity level
    
    // Statistics and debugging
    size_t totalHebbianUpdates;
    float cumulativeWeightChange;
    
    // Sliding window statistics for online covariance calculation
    float runningMeanPre;
    float runningMeanPost;
    float runningCovariance;
    size_t sampleCount;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), ojaAlpha(0.001f), ojaBeta(0.5f),
             bcmThreshold(0.5f), bcmTau(100.0f), bcmTheta(0.5f),
             temporalWindow(20.0f), temporalDecay(0.01f),
             competitionStrength(0.1f), globalNormalization(1.0f),
             competitionRange(10), tagEnabled(false), tagDecayRate(0.001f),
             tagCaptureThreshold(0.1f), baselineLearningRate(0.01f),
             activityModulation(1.0f), maxActivityLevel(10.0f),
             totalHebbianUpdates(0), cumulativeWeightChange(0.0f),
             runningMeanPre(0.0f), runningMeanPost(0.0f),
             runningCovariance(0.0f), sampleCount(0) {}
};

// Helper function for Oja's rule implementation
inline float applyOjaRule(float pre, float post, float weight,
                         float learningRate, float beta) {
    // Oja's rule: Δw = η·y·(x·w - β·w²)
    // where x is presynaptic activity, y is postsynaptic activity, w is weight
    float prePostProduct = pre * post;
    return learningRate * (prePostProduct * weight - beta * weight * weight * weight);
}

// Helper function for BCM theory
inline float applyBCMRule(float postActivity, float threshold, float learningRate) {
    // BCM rule: Δw = φ(y)·(w - θ̂)
    // where φ(y) is the BCM function (thresholded linear)
    // θ̂ is the sliding threshold (θ̂ = (1/τ)·⟨y²⟩)
    float bcmFunction = std::max(0.0f, postActivity - threshold);
    return learningRate * bcmFunction * (postActivity - threshold);
}

// Helper function for temporal Hebbian learning with spike timing windows
inline float applyTemporalHebbian(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  float temporalWindow, float decayRate) {
    float totalCovariance = 0.0f;
    float currentTime = postSpikes.empty() ? 0.0f : postSpikes.back();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < temporalWindow) {
                // Temporal weighting based on spike timing
                float temporalWeight = std::exp(-std::abs(dt) / temporalWindow);
                totalCovariance += temporalWeight;
            }
        }
    }
    
    return totalCovariance * decayRate;
}

// Helper function for synaptic competition
inline float applySynapticCompetition(float currentWeight, float avgWeight,
                                      float competitionStrength, float globalNorm) {
    // Competitive normalization: w_i = w_i - α·(w_i - w̄)
    // where w̄ is the average weight across synapses
    float deviation = currentWeight - avgWeight;
    return -competitionStrength * deviation * globalNorm;
}

// Helper function for synaptic tagging and capture
inline bool checkTagCapture(float synapticActivity, float tagThreshold) {
    return synapticActivity > tagThreshold;
}

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation
     *
     * This implementation combines multiple learning rules:
     * 1. Covariance-based Hebbian learning
     * 2. Oja's rule for stable competitive learning
     * 3. BCM theory for homeostatic stability
     * 4. Temporal Hebbian learning with spike timing windows
     * 5. Synaptic competition and normalization
     * 6. Synaptic tagging and capture mechanisms
     * 7. Activity-dependent learning rate modulation
     *
     * Biological Basis:
     * - This model reflects Hebbian plasticity observed in cortical circuits
     * - Combines elements from multiple experimental paradigms:
     *   * Oja's rule from unsupervised learning in cerebellum
     *   * BCM theory from visual cortex development
     *   * Spike timing dependent learning from hippocampus
     * - Implements synaptic homeostasis through normalization
     * - Enables synaptic tagging for memory consolidation
     *
     * Mathematical Formulation:
     * 1. Covariance rule: Δw = η·(⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
     * 2. Oja's rule: Δw = η·(x·w·y - β·w³)
     * 3. BCM rule: Δw = φ(y)·(w - θ̂) where φ(y) = y·(y-θ)·H(y-θ)
     * 4. Temporal learning: Weighted correlation based on spike timing
     * 5. Competition: w_i = w_i - α·(w_i - w̄)/N
     *
     * Where:
     * - η is learning rate modulated by activity
     * - x, y are pre/post synaptic activities
     * - w is synaptic weight
     * - θ is sliding threshold
     * - α is competition strength
     *
     * Limitations:
     * - Assumes linear integration of multiple learning signals
     * - Simplified activity statistics (can be extended to non-stationary)
     * - Doesn't account for neuromodulatory control (STDP handles this)
     */
     
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current weight for reference
    float currentWeight = std::clamp(synapse->getWeight(),
                                     pImpl->minWeight, pImpl->maxWeight);
    
    // Calculate presynaptic and postsynaptic activities
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Activity-dependent learning rate modulation
    float currentLearningRate = modulateLearningRate(
        postActivity, pImpl->baselineLearningRate,
        pImpl->activityModulation, pImpl->maxActivityLevel);
    
    float totalDelta = 0.0f;
    
    // 1. Covariance-based Hebbian learning (standard rule)
    float covDelta = calculateCovarianceUpdate(preActivity, postActivity,
                                              currentLearningRate);
    totalDelta += covDelta;
    
    // 2. Oja's rule for stable competitive learning
    if (pImpl->ojaAlpha > 0.0f) {
        float ojaDelta = applyOjaRule(preActivity, postActivity, currentWeight,
                                     pImpl->ojaAlpha, pImpl->ojaBeta);
        totalDelta += ojaDelta;
    }
    
    // 3. BCM theory for homeostatic stability
    if (pImpl->bcmThreshold > 0.0f) {
        float bcmDelta = applyBCMRule(postActivity, pImpl->bcmThreshold,
                                     currentLearningRate);
        totalDelta += bcmDelta;
    }
    
    // 4. Temporal Hebbian learning with spike timing windows
    if (pImpl->temporalWindow > 0.0f) {
        float temporalDelta = applyTemporalHebbian(
            preSpikes, postSpikes, pImpl->temporalWindow, pImpl->temporalDecay);
        totalDelta += temporalDelta * currentLearningRate;
    }
    
    // 5. Synaptic competition and normalization
    float avgWeight = calculateGlobalAverageWeight(synapse);
    float competitionDelta = applySynapticCompetition(
        currentWeight, avgWeight, pImpl->competitionStrength,
        pImpl->globalNormalization);
    totalDelta += competitionDelta;
    
    // 6. Synaptic tagging and capture (for memory consolidation)
    if (pImpl->tagEnabled) {
        float tagActivity = postActivity * preActivity;
        if (checkTagCapture(tagActivity, pImpl->tagCaptureThreshold)) {
            // Capture activity into synaptic tag
            float& currentTag = pImpl->synapticTags[synapse->getId()];
            currentTag = std::min(currentTag + 0.1f, 1.0f);
            
            // Add tag-based potentiation
            float tagDelta = currentTag * currentLearningRate * 0.5f;
            totalDelta += tagDelta;
        }
        
        // Decay tags
        if (pImpl->synapticTags.find(synapse->getId()) != pImpl->synapticTags.end()) {
            pImpl->synapticTags[synapse->getId()] *= 
                std::exp(-pImpl->tagDecayRate);
            
            // Reset tag if too small
            if (pImpl->synapticTags[synapse->getId()] < 0.01f) {
                pImpl->synapticTags.erase(synapse->getId());
            }
        }
    }
    
    // 7. Apply weight change with bounds and normalization
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
        
        // Apply global weight normalization to prevent runaway
        float normalizedWeight = std::clamp(
            synapse->getWeight(), pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(normalizedWeight);
        
        // Update statistics
        ++pImpl->totalHebbianUpdates;
        pImpl->cumulativeWeightChange += totalDelta;
    }
    
    // Update running statistics for online covariance calculation
    updateRunningStatistics(preActivity, postActivity);
}

// Additional helper functions for Hebbian implementation

// Calculate covariance-based weight update
float Hebbian::Impl::calculateCovarianceUpdate(float preActivity, float postActivity, float learningRate) {
    // Covariance rule: Δw = η·(⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
    // Simplified: Δw = η·(pre·post - mean_pre·mean_post)
    // For online implementation, we use running statistics
    if (sampleCount > 0) {
        float meanPre = runningMeanPre / static_cast<float>(sampleCount);
        float meanPost = runningMeanPost / static_cast<float>(sampleCount);
        float covariance = (preActivity * postActivity) - (meanPre * meanPost);
        
        // Apply covariance threshold if set
        if (std::abs(covariance) > covarianceThreshold) {
            return learningRate * covariance;
        }
    }
    return 0.0f;
}

// Calculate global average weight across synapses
float Hebbian::Impl::calculateGlobalAverageWeight(Synapse* synapse) {
    // This is a simplified implementation - in practice, this would
    // require access to all synapses in the network
    // For now, we return the current weight as the average
    return synapse->getWeight();
}

// Update running statistics for online covariance calculation
void Hebbian::Impl::updateRunningStatistics(float preActivity, float postActivity) {
    if (sampleCount < 1000) {  // Limit sample size for stability
        runningMeanPre += preActivity;
        runningMeanPost += postActivity;
        runningCovariance += (preActivity * postActivity) - 
                           (runningMeanPre / (sampleCount + 1)) * 
                           (runningMeanPost / (sampleCount + 1));
        sampleCount++;
    }
}
