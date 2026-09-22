#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cmath>

namespace nlm {

// PatternAssociation: Neural representation of association between two patterns
struct PatternAssociation {
    // Source and target pattern neurons
    std::vector<NeuronId> sourceNeurons;
    std::vector<NeuronId> targetNeurons;
    
    // Association strength (0-1)
    float strength;
    
    // Hebbian eligibility trace for plasticity
    float eligibilityTrace;
    
    // Last time this association was reinforced
    SimulationStep lastUpdated;
    
    // Pattern similarity scores
    float sourcePatternScore;
    float targetPatternScore;
    
    PatternAssociation()
        : strength(0.0f), eligibilityTrace(0.0f), lastUpdated(0), 
          sourcePatternScore(0.0f), targetPatternScore(0.0f) {}
};

// PatternSet: Collection of similar neural patterns for efficient storage
struct PatternSet {
    // Representative neuron ("pattern neuron") for this set
    NeuronId patternNeuron;
    
    // All neurons in this pattern set
    std::vector<NeuronId> neurons;
    
    // Pattern characteristics
    std::vector<float> patternPrototype;
    
    // Activation/decay state
    float currentActivation;
    float decayRate;
    
    // Similarity metrics for pattern completion
    std::unordered_map<NeuronId, float> activationMap;
    
    PatternSet()
        : patternNeuron(INVALID_NEURON_ID), currentActivation(0.0f), decayRate(0.01f) {}
};

// NeuralAssociativeMemory: Neural pattern association system
// Implements Hebbian learning, spreading activation, and pattern completion
// NOT just data storage - creates distributed neural representations

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference for neural integration
    void initialize(Brain* brain);
    
    // Convert sensory pattern to neural representation and store
    void learnPattern(const std::vector<float>& pattern,
                      SimulationStep currentTime,
                      float reward = 1.0f);
    
    // Convert experience episode to neural associations
    void learnFromEpisode(const EpisodicMemoryItem& episode,
                         SimulationStep currentTime);
    
    // Complete partially activated pattern through association network
    std::vector<float> completePattern(const std::vector<float>& partialPattern,
                                       float similarityThreshold = 0.5f) const;
    
    // Get all associations for a given pattern neuron
    std::vector<NeuronId> getAssociations(NeuronId patternNeuron) const;
    
    // Get activation level of a pattern
    float getPatternActivation(NeuronId patternNeuron) const;
    
    // Get strength of association between two patterns
    float getAssociationStrength(NeuronId sourceNeuron, NeuronId targetNeuron) const;
    
    // Activate pattern set from pattern neuron
    void activatePatternSet(NeuronId patternNeuron, float activationStrength = 1.0f);
    
    // Get current active patterns
    std::vector<NeuronId> getActivePatterns() const;
    
    // Update neural associative memory (Hebbian plasticity, decay)
    void update(TimestepDuration dt);
    
    // Clear all pattern associations and neural patterns
    void clear();
    
    // Get pattern for debugging
    const PatternSet* getPatternSet(NeuronId patternNeuron) const;
    
    // Enable/disable association learning
    void setLearningEnabled(bool enabled) { learningEnabled_ = enabled; }
    
    // Set Hebbian learning parameters
    void setHebbianLearningRate(float rate) { hebbianLearningRate_ = rate; }
    void setSTDPLearningRate(float rate) { stdpLearningRate_ = rate; }
    void setActivationDecayRate(float rate) { activationDecayRate_ = rate; }
    
    // Get current learning rate
    float getHebbianLearningRate() const { return hebbianLearningRate_; }
    float getSTDPLearningRate() const { return stdpLearningRate_; }
    
    // Enable spreading activation
    void enableSpreadingActivation(bool enabled) { spreadingEnabled_ = enabled; }
    void setMaxSpreadSteps(size_t steps) { maxSpreadSteps_ = steps; }
    
    // Get pattern set statistics
    size_t getPatternCount() const { return patternSets_.size(); }
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Get average pattern activation
    float getAveragePatternActivation() const;
    
    // Convert pattern to neural representation (integrates with NeuralWorkingMemory)
    void patternToNeuralRepresentation(const std::vector<float>& pattern,
                                      SimulationStep currentTime);
    
    // Retrieve patterns using spreading activation through association network
    std::vector<std::vector<float>> retrieveWithSpreading(const std::vector<float>& cuePattern,
                                                         size_t steps = 2) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Neural pattern storage
    std::unordered_map<NeuronId, PatternSet> patternSets_;
    
    // Pattern associations (Hebbian links)
    std::vector<PatternAssociation> associations_;
    
    // Pattern completion cache
    mutable std::unordered_map<NeuronId, std::vector<float>> completionCache_;
    
    // Configuration parameters
    bool learningEnabled_;
    bool spreadingEnabled_;
    float hebbianLearningRate_;
    float stdpLearningRate_;
    float activationDecayRate_;
    size_t maxSpreadSteps_;
    
    // Pattern-to-neuron mapping utilities
    NeuronId getOrCreatePatternNeuron(const std::vector<float>& pattern,
                                      SimulationStep currentTime);
    float computePatternSimilarity(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;
    
    // Neural association mechanisms
    void createNeuralAssociation(NeuronId sourceNeuron, NeuronId targetNeuron,
                                float strength, SimulationStep currentTime);
    void updateHebbianPlasticity(PatternAssociation& assoc,
                               SimulationStep currentTime,
                               float reward = 1.0f);
    void updateSTDPPlasticity(PatternAssociation& assoc,
                            SimulationStep currentTime,
                            const std::vector<float>& preSpikeHistory,
                            const std::vector<float>& postSpikeHistory);
    
    // Spreading activation
    void propagateActivation(NeuronId startNeuron,
                           std::vector<float>& accumulatedActivation,
                           size_t maxSteps = 3) const;
    float computePatternCompletionScore(NeuronId patternNeuron,
                                       const std::vector<float>& queryPattern) const;
    
    // Pattern maintenance
    void decayPatternActivations(TimestepDuration dt);
    void removeWeakPatterns(float activationThreshold);
    
    // Cache management
    void updateCompletionCache() const;
    void clearCompletionCache();
    
    // Debug and monitoring
    void logPatternStats() const;
};

} // namespace nlm