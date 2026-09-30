#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhanced initialization with episodic memory support
    void initializeWithEpisodicMemory(Brain* brain, NeuralEpisodicMemory* episodicMem);
    
    // Store information in working memory with episodic integration
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    void storeWithEpisodicSupport(const std::vector<float>& pattern, 
                                   const EnhancedEpisodicMemoryItem* episode,
                                   float strength = 1.0f);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    void storeToNeuronWithContext(NeuronId neuron, float activation, 
                                   const EnhancedEpisodicMemoryItem* episode);
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;
    std::vector<float> retrieveWithEpisodicSupport() const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    bool containsWithEpisodicSupport(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;
    float getNeuronActivationWithEpisodicSupport(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);
    void updateWithEpisodicRehearsal(TimestepDuration dt);
    
    // Enhanced rehearsal mechanisms
    void rehearse(EpisodeSupportType type = EpisodeSupportType::ActiveRehearsal);
    void rehearseWithPatternCompletion();
    void strengthenMemory(float factor);
    void enhanceRehearsal(float factor);
    
    // Novelty-based attentional capture from episodic memory
    void detectNoveltyFromEpisodic(NeuralEpisodicMemory* episodicMem);
    void applyNoveltyBoost(float noveltyLevel);
    void updateNoveltySignals();
    
    // Memory-guided prediction and planning
    void predictFromEpisodic(NeuralEpisodicMemory* episodicMem);
    void planBasedOnPastExperience();
    std::vector<float> predictNextAction() const;
    
    // Feedback loops from working memory to episodic storage
    void updateEpisodicRelevance(NeuralEpisodicMemory* episodicMem);
    void storeToEpisodicMemory(NeuralEpisodicMemory* episodicMem);
    void strengthenEpisodicTrace(size_t episodeId, float factor);
    
    // Temporal sequencing and pattern completion
    void maintainTemporalSequence();
    void completePatternsFromEpisodic();
    void updateTemporalContext();
    
    // Competition and selection mechanisms
    void runCompetition();
    void selectWinner();
    void applyInhibition();
    void enhanceWinners(float factor);
    void applyEpisodicInfluence();
    
    // Novelty detection and attentional capture
    void detectNovelty();
    void applyTopDownBiases();
    void captureAttentionFromNovelty();
    
    // Capacity management and memory compression
    void enforceCapacity();
    void evictWeakTraces();
    void compressWorkingMemory();
    void optimizeMemoryDensity();
    
    // Clear working memory
    void clear();
    void clearWithEpisodicCleanup();
    
    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Enhanced metrics and statistics
    float getMemoryActivity() const;
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getRehearsalStrength() const { return rehearsalStrength_; }
    float getEpisodicSupportLevel() const { return episodicSupportLevel_; }
    size_t getSupportingEpisodicCount() const { return supportingEpisodicIds_.size(); }
    
    // State management
    void pauseRehearsal() { rehearsalPaused_ = true; }
    void resumeRehearsal() { rehearsalPaused_ = false; }
    bool isRehearsalPaused() const { return rehearsalPaused_; }
    
    void enablePatternCompletion() { patternCompletionEnabled_ = true; }
    void disablePatternCompletion() { patternCompletionEnabled_ = false; }
    bool isPatternCompletionEnabled() const { return patternCompletionEnabled_; }
    
    void enableNoveltyBoost() { noveltyBoostEnabled_ = true; }
    void disableNoveltyBoost() { noveltyBoostEnabled_ = false; }
    bool isNoveltyBoostEnabled() const { return noveltyBoostEnabled_; }
    
private:
    // Enhanced structure for Phase 6 implementation
    struct Impl {
        Brain* brain;
        NeuralEpisodicMemory* episodicMem;
        
        // Memory maintenance connections
        std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
        std::vector<std::pair<size_t, float>> rehearsalSchedules;
        
        // Episodic integration
        std::unordered_map<size_t, float> episodicSupportMap;
        std::vector<size_t> activeEpisodicIds;
        std::vector<float> episodicSupportStrengths;
        
        // Novelty and attention
        std::vector<float> noveltySignals;
        std::unordered_map<NeuronId, float> attentionalWeights;
        std::vector<NeuronId> attendedNeurons;
        
        // Temporal sequencing
        std::vector<size_t> temporalSequence;
        std::vector<float> patternCompletionScores;
        std::vector<size_t> sequenceStartTimes;
        
        // Competition state
        std::vector<NeuronId> winnerHistory;
        SimulationStep lastCompetitionStep;
        
        Impl() : brain(nullptr), episodicMem(nullptr), lastCompetitionStep(0) {}
    };
    
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    NeuralEpisodicMemory* episodicMemory_;
    
    size_t capacity_;
    float decayRate_;
    float noveltyDecayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<SimulationStep> rehearsalTimestamps_;
    std::vector<float> rehearsalStrengths_;
    
    // Competition and attention
    std::vector<NeuronId> winners_;
    std::vector<float> noveltySignals_;
    float noveltyLevel_;
    float rehearsalStrength_;
    float episodicSupportLevel_;
    
    // Temporal sequencing
    std::vector<size_t> temporalSequence_;
    std::vector<float> patternCompletionScores_;
    
    // Episodic integration
    std::vector<size_t> supportingEpisodicIds_;
    std::vector<float> episodicSupportStrengths_;
    
    // Control mechanisms
    bool rehearsalPaused_;
    bool noveltyBoostEnabled_;
    bool patternCompletionEnabled_;
    float patternCompletionThreshold_;
    
    // Internal state
    SimulationStep lastUpdateStep_;
    size_t totalPatternCompletions_;
    
    // Helper methods
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    void updateRecurrentConnections();
    void decayWeakTraces();
    void buildTemporalSequence();
    void applyTopDownInfluence();
    void updateNoveltyBasedOnEpisodic();
    void reinforceEpisodicConnections();
    void updatePatternCompletionScores();
    void scheduleRehearsal(size_t episodeId, EpisodeSupportType type);
};

// AttentionalSelection: Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation
//
// Mechanisms:
// - Competition between neural populations
// - Salience-based selection
// - Recurrent amplification of winners
// - Inhibitory surround suppression

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    void releaseAttention();

    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm