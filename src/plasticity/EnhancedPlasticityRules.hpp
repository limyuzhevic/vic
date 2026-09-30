// Decay Plasticity Rule
// Implements synaptic decay over time, a biologically realistic mechanism
// where synapses naturally weaken with prolonged inactivity
//
// This rule complements STDP and Hebbian by providing a homeostatic mechanism
// that prevents runaway excitation and maintains network stability

#pragma once

#include "../plasticity/PlasticityRule.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class DecayPlasticity : public PlasticityRule {
public:
    DecayPlasticity() : PlasticityRule(PlasticityType::Decay) {}
    
    ~DecayPlasticity() override = default;
    
    // Decay rule has no pre/post spike requirements
    bool requiresSpikes() const override {
        return false;
    }
    
    // Update synaptic weight based on decay mechanism
    // Weight decays exponentially toward a baseline over time
    void update(Synapse* synapse, const std::vector<Timestamp>& /* preSpikes */, 
                const std::vector<Timestamp>& /* postSpikes */, 
                Timestamp currentTime) override {
        if (!synapse) return;
        
        // Get current synaptic weight
        float currentWeight = synapse->getWeight();
        float baselineWeight = 0.1f;  // Minimum weight for synapse existence
        float decayRate = 0.0005f;   // Rate of decay per time unit
        
        // Exponential decay toward baseline
        float decayFactor = std::exp(-decayRate * currentTime);
        float targetWeight = baselineWeight + (currentWeight - baselineWeight) * decayFactor;
        
        // Apply decay, but ensure weight doesn't go below baseline
        float newWeight = std::max(targetWeight, baselineWeight);
        
        // Only update if change is significant
        if (std::abs(newWeight - currentWeight) > 0.001f) {
            synapse->setWeight(newWeight);
        }
    }
    
    // Get rule-specific parameters
    float getDecayRate() const { return 0.0005f; }
    float getBaselineWeight() const { return 0.1f; }
    
    // Set rule parameters for tuning
    void setDecayRate(float rate) {
        decayRate_ = rate;
    }
    
    void setBaselineWeight(float baseline) {
        baselineWeight_ = baseline;
    }
    
private:
    float decayRate_ = 0.0005f;
    float baselineWeight_ = 0.1f;
};

// Enhanced Episodic Memory with Pattern Completion
// Extends episodic memory with pattern completion capabilities
//
// This enhancement allows the memory system to reconstruct partial
// episodes based on similarity, improving recall robustness and
// enabling more sophisticated memory utilization

#pragma once

#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

class EnhancedEpisodicMemory : public NeuralEpisodicMemory {
public:
    EnhancedEpisodicMemory() : NeuralEpisodicMemory() {}
    
    ~EnhancedEpisodicMemory() override = default;
    
    // Enhanced episode storage with pattern indexing
    // Creates multiple representations of episodes for faster retrieval
    void storeEpisode(const EpisodicMemoryItem& episode) override {
        // Call parent implementation
        NeuralEpisodicMemory::storeEpisode(episode);
        
        // Create pattern index for fast lookup based on key features
        createPatternIndex(episode);
    }
    
    // Enhanced retrieval with pattern completion
    // Can retrieve episodes based on partial patterns or similarity
    EpisodicMemoryItem* retrieveEpisode(const std::vector<NeuronId>& partialPattern,
                                       float similarityThreshold = 0.7f) {
        // First try exact match
        auto* episode = NeuralEpisodicMemory::retrieveEpisode(partialPattern);
        if (episode) return episode;
        
        // If no exact match, try pattern completion
        return retrieveByPatternCompletion(partialPattern, similarityThreshold);
    }
    
    // Get all episodes with similarity above threshold
    std::vector<const EpisodicMemoryItem*> retrieveSimilarEpisodes(
        const std::vector<NeuronId>& pattern,
        float similarityThreshold = 0.6f) {
        std::vector<const EpisodicMemoryItem*> similarEpisodes;
        
        // Get all episodes
        auto allEpisodes = NeuralEpisodicMemory::getEpisodes();
        
        for (const auto* episode : allEpisodes) {
            float similarity = calculatePatternSimilarity(pattern, episode->activeNeurons);
            if (similarity >= similarityThreshold) {
                similarEpisodes.push_back(episode);
            }
        }
        
        return similarEpisodes;
    }
    
    // Get pattern statistics for analytics
    struct PatternStats {
        size_t episodeCount = 0;
        float averageActivation = 0.0f;
        float activationVariance = 0.0f;
        std::vector<float> commonNeurons;
    };
    
    PatternStats getPatternStatistics() {
        PatternStats stats;
        auto episodes = NeuralEpisodicMemory::getEpisodes();
        stats.episodeCount = episodes.size();
        
        if (stats.episodeCount == 0) return stats;
        
        // Calculate statistics
        float totalActivation = 0.0f;
        for (const auto* episode : episodes) {
            totalActivation += episode->averageActivation();
        }
        stats.averageActivation = totalActivation / stats.episodeCount;
        
        // Find common neurons across episodes
        std::unordered_map<NeuronId, size_t> neuronCounts;
        for (const auto* episode : episodes) {
            for (NeuronId neuronId : episode->activeNeurons) {
                neuronCounts[neuronId]++;
            }
        }
        
        // Keep top 10 most common neurons
        std::vector<std::pair<NeuronId, size_t>> sortedNeurons(
            neuronCounts.begin(), neuronCounts.end());
        std::sort(sortedNeurons.begin(), sortedNeurons.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        stats.commonNeurons.reserve(std::min(size_t(10), sortedNeurons.size()));
        for (size_t i = 0; i < std::min(size_t(10), sortedNeurons.size()); ++i) {
            stats.commonNeurons.push_back(static_cast<float>(sortedNeurons[i].first));
        }
        
        return stats;
    }
    
protected:
    // Pattern indexing for fast retrieval
    void createPatternIndex(const EpisodicMemoryItem& episode) {
        // Create a compact pattern representation
        // This could be based on neuron types, activation levels, etc.
        
        // For now, just store the raw pattern - could be enhanced
        // to include normalized activation levels, neuron types, etc.
        patternIndices_.push_back(episode.activeNeurons);
    }
    
    // Pattern completion via similarity matching
    EpisodicMemoryItem* retrieveByPatternCompletion(
        const std::vector<NeuronId>& partialPattern,
        float similarityThreshold) {
        
        // Search through all patterns for similarity
        for (size_t i = 0; i < patternIndices_.size(); ++i) {
            float similarity = calculatePatternSimilarity(partialPattern, patternIndices_[i]);
            if (similarity >= similarityThreshold) {
                // Return a copy of the episode for modification
                return NeuralEpisodicMemory::getEpisode(static_cast<int>(i));
            }
        }
        
        return nullptr;
    }
    
    // Calculate pattern similarity
    float calculatePatternSimilarity(const std::vector<NeuronId>& pattern1,
                                     const std::vector<NeuronId>& pattern2) {
        if (pattern1.empty() || pattern2.empty()) return 0.0f;
        
        // Jaccard similarity: intersection / union
        std::unordered_set<NeuronId> set1(pattern1.begin(), pattern1.end());
        std::unordered_set<NeuronId> set2(pattern2.begin(), pattern2.end());
        
        size_t intersection = 0;
        for (NeuronId id : set1) {
            if (set2.find(id) != set2.end()) {
                intersection++;
            }
        }
        
        size_t union = set1.size() + set2.size() - intersection;
        
        return static_cast<float>(intersection) / static_cast<float>(union);
    }
    
private:
    // Store indexed patterns for fast lookup
    std::vector<std::vector<NeuronId>> patternIndices_;
};

// Adaptive Neuromodulation System
// Enhanced neuromodulation with adaptive parameters
//
// This system extends the basic neuromodulation framework with:
// 1. Adaptive gain control
// 2. State-dependent modulation
// 3. Cross-modality interactions
// 4. Learning of optimal modulation parameters

#pragma once

#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace nlm {

class AdaptiveNeuromodulation {
public:
    struct ModulatoryState {
        float dopamine = 0.0f;
        float serotonin = 0.0f;
        float acetylcholine = 0.0f;
        float noradrenaline = 0.0f;
        
        // State-dependent gains
        float learningRateGain = 1.0f;
        float explorationGain = 1.0f;
        float attentionGain = 1.0f;
        
        // Temporal derivatives for adaptive control
        float dopamineDelta = 0.0f;
        float learningRateDelta = 0.0f;
        float explorationDelta = 0.0f;
    };
    
    AdaptiveNeuromodulation() : state_() {}
    
    ~AdaptiveNeuromodulation() = default;
    
    // Update neuromodulatory state with adaptive control
    void update(double timestep, float predictionError, float reward) {
        // Update basic neuromodulators (simplified)
        state_.dopamine = predictionError;
        state_.serotonin = 0.5f * (1.0f - std::abs(predictionError));
        state_.acetylcholine = reward * 0.5f + 0.2f;
        state_.noradrenaline = std::abs(predictionError) * 0.8f;
        
        // Adaptive control based on task demands
        adaptGains(predictionError, reward, timestep);
        
        // Apply temporal smoothing
        smoothState(timestep);
    }
    
    // Get current neuromodulatory state
    const ModulatoryState& getState() const { return state_; }
    
    // Get specific neuromodulator level
    float getDopamine() const { return state_.dopamine; }
    float getSerotonin() const { return state_.serotonin; }
    float getAcetylcholine() const { return state_.acetylcholine; }
    float getNoradrenaline() const { return state_.noradrenaline; }
    
    // Get adaptive gains
    float getLearningRateGain() const { return state_.learningRateGain; }
    float getExplorationGain() const { return state_.explorationGain; }
    float getAttentionGain() const { return state_.attentionGain; }
    
    // Check if neuromodulation should be active
    bool isActive() const {
        return (std::abs(state_.dopamine) > 0.01f ||
                std::abs(state_.serotonin) > 0.01f ||
                std::abs(state_.acetylcholine) > 0.01f ||
                std::abs(state_.noradrenaline) > 0.01f);
    }
    
    // Get neuromodulatory influence on plasticity
    float getPlasticityInfluence() const {
        return state_.learningRateGain * 0.01f + 
               state_.explorationGain * 0.005f + 
               state_.attentionGain * 0.002f;
    }
    
    // Reset state for new episode
    void reset() {
        state_.dopamine = 0.0f;
        state_.serotonin = 0.0f;
        state_.acetylcholine = 0.0f;
        state_.noradrenaline = 0.0f;
        state_.learningRateGain = 1.0f;
        state_.explorationGain = 1.0f;
        state_.attentionGain = 1.0f;
    }
    
protected:
    // Adaptive gain control based on task demands
    void adaptGains(float predictionError, float reward, double timestep) {
        // Learning rate should be higher when prediction error is high
        // but not too high when reward is stable
        float learningRateTarget = std::min(2.0f, 1.0f + std::abs(predictionError) * 0.5f);
        if (reward > 0.5f) {
            learningRateTarget *= 0.8f;  // Reduce exploration when reward is high
        }
        state_.learningRateGain += (learningRateTarget - state_.learningRateGain) * 0.1f;
        
        // Exploration should be higher when novelty is high
        float noveltyFactor = std::abs(predictionError);
        state_.explorationGain = 1.0f + noveltyFactor * 0.5f;
        
        // Attention should be higher when uncertainty is high
        float uncertainty = std::min(1.0f, std::abs(predictionError) + std::abs(0.5f - reward));
        state_.attentionGain = 1.0f + uncertainty * 0.3f;
        
        // Store deltas for analysis
        state_.learningRateDelta = learningRateTarget - state_.learningRateGain;
        state_.explorationDelta = (noveltyFactor * 0.5f) - state_.explorationGain;
    }
    
    // Temporal smoothing to prevent rapid changes
    void smoothState(double timestep) {
        float smoothingFactor = std::exp(-timestep * 0.1f);
        
        state_.dopamineDelta = state_.dopamine * (1.0f - smoothingFactor);
        state_.learningRateDelta *= smoothingFactor;
        state_.explorationDelta *= smoothingFactor;
    }
    
private:
    ModulatoryState state_;
};

} // namespace nlm
