#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <string>

namespace nlm {

// LongTermMemoryTrace: Persistent storage of memories across sessions
struct LongTermMemoryTrace {
    SimulationStep creationTime;
    SimulationStep lastAccessTime;
    SimulationStep lastConsolidationTime;
    float stabilityScore;
    float strength;
    float decayRate;
    std::vector<NeuronId> associatedNeurons;
    std::vector<float> activationPattern;
    std::string semanticContent;
    std::vector<SimulationStep> relatedEpisodes;
    float predictionError;
    float noveltyScore;
    float rewardAssociation;
    
    LongTermMemoryTrace()
        : creationTime(0)
        , lastAccessTime(0)
        , lastConsolidationTime(0)
        , stabilityScore(0.0f)
        , strength(0.0f)
        , decayRate(0.001f)  // Very slow decay for long-term storage
        , predictionError(0.0f)
        , noveltyScore(0.0f)
        , rewardAssociation(0.0f) {}
};

// MemoryConsolidationEvent: Represents a memory consolidation event
struct MemoryConsolidationEvent {
    SimulationStep timestamp;
    SimulationStep sourceEpisodeId;
    float sourceStrength;
    float targetTraceStrength;
    float integrationStrength;
    float temporalCompression;
    std::vector<NeuronId> sourceNeurons;
    std::vector<NeuronId> targetNeurons;
    float weightChange;
    
    MemoryConsolidationEvent()
        : timestamp(0)
        , sourceEpisodeId(0)
        , sourceStrength(0.0f)
        , targetTraceStrength(0.0f)
        , integrationStrength(0.0f)
        , temporalCompression(1.0f)
        , weightChange(0.0f) {}
};

// MemoryPrioritizationInfo: Information used for memory prioritization
struct MemoryPrioritizationInfo {
    SimulationStep episodeId;
    float rehearsalCount;
    float replayFrequency;
    float noveltyValue;
    float rewardValue;
    float predictionError;
    float temporalRecency;
    float emotionalValence;
    float contextualRelevance;
    float integrationPotential;
    
    MemoryPrioritizationInfo()
        : episodeId(0)
        , rehearsalCount(0.0f)
        , replayFrequency(0.0f)
        , noveltyValue(0.0f)
        , rewardValue(0.0f)
        , predictionError(0.0f)
        , temporalRecency(0.0f)
        , emotionalValence(0.0f)
        , contextualRelevance(0.0f)
        , integrationPotential(0.0f) {}
};

// MemoryDecayMechanism: Describes how memories decay over time
struct MemoryDecayMechanism {
    enum class DecayType {
        Exponential,
        PowerLaw,
        Logistic,
        Adaptive
    };
    
    DecayType type;
    float baseDecayRate;
    float adaptationFactor;
    float consolidationProtection;
    float strengthDependentFactor;
    
    MemoryDecayMechanism()
        : type(DecayType::Exponential)
        , baseDecayRate(0.001f)
        , adaptationFactor(1.0f)
        , consolidationProtection(0.5f)
        , strengthDependentFactor(0.5f) {}
};

// MemoryIntegrationWeights: Weights for integrating memories across systems
struct MemoryIntegrationWeights {
    float workingEpisodicIntegration;    // Weight from working to episodic
    float episodicEpisodicIntegration;  // Weight between episodic memories
    float episodicLongTermIntegration;  // Weight from episodic to long-term
    float semanticEpisodicIntegration;  // Weight from semantic to episodic
    float proceduralEpisodicIntegration; // Weight from procedural to episodic
    float crossSystemConsolidation;      // General cross-system integration
    
    MemoryIntegrationWeights()
        : workingEpisodicIntegration(0.3f)
        , episodicEpisodicIntegration(0.4f)
        , episodicLongTermIntegration(0.3f)
        , semanticEpisodicIntegration(0.2f)
        , proceduralEpisodicIntegration(0.2f)
        , crossSystemConsolidation(0.5f) {}
};

class PersistentMemorySystem {
public:
    PersistentMemorySystem();
    ~PersistentMemorySystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update system
    void update(TimestepDuration dt);
    
    // Create a new long-term memory trace
    void createMemoryTrace(const std::vector<float>& activationPattern, 
                          const std::string& semanticContent = "");
    
    // Retrieve long-term memory trace
    LongTermMemoryTrace* retrieveMemoryTrace(SimulationStep episodeId);
    
    // Update memory trace based on recent usage
    void updateMemoryTrace(SimulationStep episodeId, const std::vector<float>& newActivationPattern);
    
    // Consolidate memories from episodic to long-term storage
    void consolidateMemories(const std::vector<MemoryPrioritizationInfo>& priorities);
    
    // Integrate memories across systems
    void integrateMemories(const MemoryIntegrationWeights& weights);
    
    // Apply memory decay mechanisms
    void applyDecay(float timeDelta);
    
    // Prioritize memories for consolidation
    std::vector<MemoryPrioritizationInfo> prioritizeMemories() const;
    
    // Get consolidation events
    const std::vector<MemoryConsolidationEvent>& getConsolidationEvents() const { return consolidationEvents_; }
    
    // Get long-term memory traces
    const std::vector<LongTermMemoryTrace>& getMemoryTraces() const { return memoryTraces_; }
    
    // Get memory statistics
    size_t getTraceCount() const { return memoryTraces_.size(); }
    float getAverageMemoryStrength() const;
    float getAverageMemoryStability() const;
    
    // Enable/disable memory persistence across sessions
    void enablePersistence(bool enable) { persistenceEnabled_ = enable; }
    bool isPersistenceEnabled() const { return persistenceEnabled_; }
    
    // Set memory decay parameters
    void setDecayParameters(const MemoryDecayMechanism& params) { decayParams_ = params; }
    const MemoryDecayMechanism& getDecayParameters() const { return decayParams_; }
    
    // Set integration weights
    void setIntegrationWeights(const MemoryIntegrationWeights& weights) { integrationWeights_ = weights; }
    const MemoryIntegrationWeights& getIntegrationWeights() const { return integrationWeights_; }
    
    // Save memory traces to persistent storage
    bool saveMemoryTraces(const std::string& filepath) const;
    
    // Load memory traces from persistent storage
    bool loadMemoryTraces(const std::string& filepath);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<LongTermMemoryTrace> memoryTraces_;
    std::vector<MemoryConsolidationEvent> consolidationEvents_;
    
    // Memory management parameters
    bool persistenceEnabled_;
    float consolidationInterval_;
    float maxMemoryTraces_;
    float integrationStrength_;
    
    // Statistics
    size_t totalConsolidations_;
    size_t successfulIntegrations_;
    size_t memoryEvictions_;
    float totalMemoryStrength_;
    
    // Internal methods
    void generateMemoryTraceFromEpisode(const EpisodicMemoryItem& episode);
    void updateTraceStrength(LongTermMemoryTrace& trace, float timeDelta);
    void performTracePruning();
    void scheduleConsolidationEvents();
    void updateTraceIntegration(LongTermMemoryTrace& sourceTrace, LongTermMemoryTrace& targetTrace);
    void calculateIntegrationWeights(MemoryIntegrationWeights& weights) const;
    void applyAdaptiveDecay(float timeDelta);
    void consolidateToSemanticMemory(const std::vector<LongTermMemoryTrace>& traces);
    LongTermMemoryTrace* findSimilarTrace(const std::vector<float>& pattern) const;
};

} // namespace nlm
