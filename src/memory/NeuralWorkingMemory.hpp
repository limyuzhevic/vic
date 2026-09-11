#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <array>
#include <string>

namespace nlm {

// ActionType enum for working memory
// This is now available through Action class
using ActionType = nlm::Action::ActionType;

// Sensory pattern representation for working memory
struct SensoryPattern {
    SimulationStep timestamp;
    std::vector<float> features;
    std::array<float, 2> position;      // x, y position
    float orientation;                    // in radians
    std::vector<float> rawSignals;        // raw sensory data
    
    SensoryPattern() : timestamp(0), orientation(0.0f) {
        position.fill(0.0f);
    }
};

// Action context for working memory
struct ActionContext {
    ActionType type;
    std::vector<float> parameters;
    SimulationStep timestamp;
    float outcomeReward;
    float outcomeEnergy;
    std::vector<float> outcomeSensoryState;
    bool succeeded;
    
    ActionContext() : type(ActionType::Wait), outcomeReward(0.0f), outcomeEnergy(0.0f), succeeded(false) {}
};

// Complete working memory trace with temporal context
struct MemoryTrace {
    SimulationStep creationTime;
    SimulationStep lastUpdateTime;
    std::vector<float> neuralPattern;
    float activationLevel;
    float predictionConfidence;
    
    // Associated sensory and action context
    SensoryPattern sensoryPattern;
    ActionContext actionContext;
    
    // Predictive encoding
    std::vector<float> predictedNextState;
    std::vector<float> predictionError;
    
    // For pattern completion
    std::vector<float> patternSignature;    // for similarity matching
    std::vector<size_t> associatedTraceIds;  // temporal sequence links
    
    MemoryTrace() : creationTime(0), lastUpdateTime(0), activationLevel(0.0f), predictionConfidence(0.0f) {}
};

// NeuralWorkingMemory: Implements working memory using neural dynamics
// Enhanced version with full cognitive support:
// - Stores actual sensory patterns and action contexts
// - Maintains temporal sequences of experiences
// - Provides pattern completion for related memories
// - Integrates with episodic memory and prediction systems
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention
// - Temporal sequence maintenance
// - Pattern completion and prediction
// - Sensory-action integration

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Enhanced storage methods
    
    // Store information in working memory
    // The information is encoded as neural activity pattern with sensory/action context
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation with full context
    void storeToNeuron(NeuronId neuron, float activation, 
                      const SensoryPattern& sensory = SensoryPattern(),
                      const ActionContext& action = ActionContext());

    // Store complete experience
    void storeExperience(const SensoryPattern& sensory, 
                        const ActionContext& action,
                        const std::vector<float>& resultingState,
                        float reward,
                        float energy);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Enhanced retrieve with pattern completion
    // Find related memories based on partial patterns
    std::vector<float> retrieveSimilar(const std::vector<float>& pattern, 
                                       size_t maxResults = 5) const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Get full memory trace for a neuron
    const MemoryTrace* getMemoryTrace(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return traces_.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

    // Pattern encoding/decoding for transformation
    std::vector<float> encodePattern(const SensoryPattern& sensory, 
                                     const ActionContext& action) const;
    SensoryPattern decodeSensory(const std::vector<float>& encoded) const;
    ActionContext decodeAction(const std::vector<float>& encoded) const;

    // Temporal sequence management
    void updateTemporalSequences();
    void linkTemporalSequences();

    // Integration with episodic memory
    void createEpisodicMemoryItem(const MemoryTrace& trace);
    bool hasEpisodicMemory() const { return episodicMemoryCreated_; }

    // Integration with prediction system
    void updatePredictions();
    void incorporatePredictionError(const std::vector<float>& error);

    // Competition mechanism improvements
    void advancedCompetition();
    void applyPredictionGating();

    // Pattern completion
    void completePattern(const std::vector<float>& partialPattern);

    // Get all memory traces for inspection
    const std::vector<MemoryTrace>& getMemoryTraces() const { return traces_; }

    // Get recent traces for temporal window queries
    std::vector<const MemoryTrace*> getRecentTraces(size_t count) const;

    // Get traces by similarity
    std::vector<const MemoryTrace*> getTracesSimilar(const std::vector<float>& pattern,
                                                     size_t maxResults) const;

    // Get traces by time range
    std::vector<const MemoryTrace*> getTracesInTimeRange(SimulationStep startTime,
                                                         SimulationStep endTime,
                                                         size_t maxResults) const;

private:
    // Internal memory management
    void addTrace(const MemoryTrace& trace);
    void removeWeakTraces();
    void consolidateTraces();

    // Pattern similarity computation
    float computePatternSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b) const;

    // Neural encoding/decoding helpers
    void encodeToNeurons(const std::vector<float>& pattern, float strength);
    std::vector<float> decodeFromNeurons() const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content (legacy support)
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Enhanced memory system
    std::vector<MemoryTrace> traces_;
    std::vector<NeuronId> activeNeurons_;
    
    // Temporal sequence management
    std::deque<size_t> temporalSequence_;  // IDs of traces in temporal order
    SimulationStep lastSequenceUpdate_;
    
    // Pattern completion buffer
    std::vector<float> completionPattern_;
    
    // Integration flags
    bool episodicMemoryCreated_;
    bool predictionSystemIntegrated_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
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