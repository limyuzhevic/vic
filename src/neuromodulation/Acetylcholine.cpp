#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    // Core acetylcholine dynamics
    float level;
    float baseline;              // Tonic ACh baseline
    float peak;                  // Maximum achievable level
    
    // Attention and memory modulation
    float attentionStrength;      // Current attentional focus level
    float memoryConsolidation;   // Memory consolidation bias
    float workingMemoryBoost;    // Working memory enhancement factor
    float sensoryGain;           // Sensory processing gain
    float noiseReduction;        // Noise suppression factor
    
    // Temporal dynamics
    float attentionDecay;        // Attention decay rate
    float consolidationDecay;    // Memory consolidation decay
    float recoveryRate;           // Recovery towards baseline
    
    // State variables
    float attentionTime;         // Duration of current attentional focus
    float memoryTrace;            // Memory eligibility trace
    float sensoryMemory;          // Integrated sensory memory
    float cholinergicTone;        // Overall cholinergic tone
    
    // Learning and plasticity
    float learningModulation;     // Modulation of learning processes
    float encodingEfficiency;     // Efficiency of memory encoding
    float interferenceSuppression; // Suppression of memory interference
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Working memory integration
    NeuralWorkingMemory* associatedWorkingMemory;
    float memoryCapacityBoost;    // Boost to working memory capacity
    float retentionTimeBoost;     // Extended retention time
    
    // Attention control
    bool focusedAttention;        // Whether currently in focused attention mode
    float focusStartTime;         // Time when focus began
    float attentionSpan;          // Duration of attentional focus
    float distractionThreshold;   // Threshold for distraction
    
    Impl() : level(0.05f), baseline(0.05f), peak(1.0f)
           , attentionStrength(0.0f), memoryConsolidation(0.0f)
           , workingMemoryBoost(0.0f), sensoryGain(1.0f), noiseReduction(0.0f)
           , attentionDecay(0.1f), consolidationDecay(0.05f), recoveryRate(0.02f)
           , attentionTime(0.0f), memoryTrace(0.0f), sensoryMemory(0.0f)
           , cholinergicTone(0.0f), learningModulation(0.0f), encodingEfficiency(0.0f)
           , interferenceSuppression(0.0f), associatedWorkingMemory(nullptr)
           , memoryCapacityBoost(0.0f), retentionTimeBoost(0.0f)
           , focusedAttention(false), focusStartTime(0.0f), attentionSpan(0.0f)
           , distractionThreshold(0.3f) {
        rng.seed(std::random_device{}());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Biologically plausible ACh-modulated plasticity factor
    // ACh enhances plasticity for attended/relevant information
    float attentionComponent = pImpl->attentionStrength * 0.7f;
    float memoryComponent = pImpl->memoryConsolidation * 0.3f;
    
    // Combined effect with saturation
    float plasticityFactor = attentionComponent + memoryComponent;
    
    // Apply sigmoid function for bounded response
    plasticityFactor = 1.0f / (1.0f + std::exp(-10.0f * (plasticityFactor - 0.5f)));
    
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Update acetylcholine dynamics with biologically plausible model
    float dtFloat = static_cast<float>(dt);
    
    // Decay of attention and memory consolidation
    if (pImpl->attentionStrength > 0.0f) {
        pImpl->attentionStrength *= (1.0f - pImpl->attentionDecay * dtFloat);
    }
    
    if (pImpl->memoryConsolidation > 0.0f) {
        pImpl->memoryConsolidation *= (1.0f - pImpl->consolidationDecay * dtFloat);
    }
    
    // Homeostatic recovery towards baseline
    float recoveryForce = (pImpl->baseline - pImpl->level) * pImpl->recoveryRate;
    pImpl->level += recoveryForce * dtFloat;
    
    // Update attention duration
    if (pImpl->focusedAttention) {
        pImpl->attentionTime += dtFloat;
        
        // End focused attention after attention span time
        if (pImpl->attentionTime >= pImpl->attentionSpan) {
            pImpl->focusedAttention = false;
            pImpl->attentionStrength *= 0.5f;  // Gradual decrease
        }
    }
    
    // Update memory traces
    if (pImpl->memoryTrace > 0.0f) {
        pImpl->memoryTrace *= (1.0f - pImpl->consolidationDecay * dtFloat);
    }
    
    // Update integrated sensory memory
    if (pImpl->sensoryMemory > 0.0f) {
        pImpl->sensoryMemory *= (1.0f - pImpl->attentionDecay * dtFloat);
    }
    
    // Calculate overall cholinergic tone
    pImpl->cholinergicTone = pImpl->level + pImpl->attentionStrength * 0.3f;
    pImpl->cholinergicTone = std::min(pImpl->cholinergicTone, 1.0f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Phasic acetylcholine release for attentional focus
    // Based on behavioral relevance and novelty detection
    
    // Apply noise to attention signal for biological variability
    float noisyInput = attentionStrength + normalDist(pImpl->rng) * 0.1f;
    noisyInput = std::max(0.0f, std::min(1.0f, noisyInput));
    
    // Update attention strength with smoothing
    if (noisyInput > pImpl->attentionStrength) {
        // Rapid increase for novel/important stimuli
        pImpl->attentionStrength = std::min(1.0f, noisyInput * 1.5f);
        pImpl->focusedAttention = true;
        pImpl->focusStartTime = 0.0f;
        pImpl->attentionTime = 0.0f;
    } else {
        // Gradual decrease
        pImpl->attentionStrength *= 0.95f;
    }
    
    // Increase overall level for strong attention signals
    if (pImpl->attentionStrength > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->attentionStrength * 0.3f);
    }
}

void Acetylcholine::enableMemoryConsolidation(bool enable) {
    if (enable) {
        // Enable memory consolidation mode
        pImpl->memoryConsolidation = 1.0f;
        
        // If associated working memory exists, enhance its consolidation
        if (pImpl->associatedWorkingMemory) {
            pImpl->workingMemoryBoost = 0.5f;
            pImpl->retentionTimeBoost = 2.0f;
            
            // Extend memory retention time
            pImpl->associatedWorkingMemory->setConsolidationBias(1.0f);
        }
    } else {
        // Gradually disable consolidation
        pImpl->memoryConsolidation *= 0.5f;
        pImpl->workingMemoryBoost *= 0.5f;
        pImpl->retentionTimeBoost *= 0.5f;
        
        if (pImpl->associatedWorkingMemory) {
            pImpl->associatedWorkingMemory->setConsolidationBias(0.3f);
        }
    }
}

void Acetylcholine::enhanceWorkingMemory(NeuralWorkingMemory* workingMemory) {
    if (workingMemory) {
        pImpl->associatedWorkingMemory = workingMemory;
        
        // Immediate boost to working memory performance
        float currentAChLevel = getLevel();
        
        // Calculate enhancement based on current ACh level
        pImpl->workingMemoryBoost = currentAChLevel * 0.8f;
        pImpl->retentionTimeBoost = 1.0f + currentAChLevel * 2.0f;
        
        // Apply enhancement to working memory
        workingMemory->setConsolidationBias(pImpl->workingMemoryBoost);
        workingMemory->setDecayRate(workingMemory->getDecayRate() * (1.0f - pImpl->workingMemoryBoost * 0.5f));
    }
}

void Acetylcholine::modulateSensoryProcessing(std::vector<float>& sensoryInput, float gainFactor) {
    // Modulate sensory input gain based on ACh level
    // ACh enhances signal-to-noise ratio in sensory processing
    
    float currentAChLevel = getLevel();
    
    // Calculate gain enhancement
    float sensoryGain = 1.0f + currentAChLevel * 2.0f * gainFactor;
    
    // Apply gain modulation to sensory input
    for (float& value : sensoryInput) {
        // Enhance signal components
        value *= sensoryGain;
        
        // Apply noise reduction (ACh reduces neural noise)
        float noiseReduction = 1.0f - currentAChLevel * 0.3f;
        // In a real implementation, we would add controlled noise here
        
        // Clip to reasonable range
        value = std::max(-10.0f, std::min(10.0f, value));
    }
}

void Acetylcholine::enhanceMemoryEncoding(const std::vector<float>& neuralActivity, float learningRate) {
    // Enhance memory encoding of neural activity patterns
    // ACh prioritizes salient patterns for consolidation
    
    float currentAChLevel = getLevel();
    
    // Calculate encoding enhancement based on ACh
    float encodingBoost = 1.0f + currentAChLevel * pImpl->encodingEfficiency;
    
    // Enhance memory trace formation
    for (float activity : neuralActivity) {
        // Amplify salient activities
        if (std::abs(activity) > 0.5f) {
            pImpl->memoryTrace += activity * encodingBoost * learningRate;
        }
    }
    
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 1.0f);
}

void Acetylcholine::suppressIrrelevantActivity(std::vector<float>& neuralActivity, float threshold) {
    // Suppress irrelevant neural activity (inattention)
    // ACh helps filter out irrelevant information
    
    float currentAChLevel = getLevel();
    float suppressionStrength = currentAChLevel * pImpl->interferenceSuppression;
    
    // Apply suppression to low-amplitude activities
    for (float& activity : neuralActivity) {
        if (std::abs(activity) < threshold) {
            // Suppress irrelevant activity
            activity *= (1.0f - suppressionStrength);
        }
    }
}

float Acetylcholine::getCholinergicTone() const {
    return pImpl->cholinergicTone;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getSensoryGain() const {
    return pImpl->sensoryGain;
}

float Acetylcholine::getNoiseReduction() const {
    return pImpl->noiseReduction;
}

} // namespace nlm