#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information with advanced features
// Implements real storage with capacity limits, LRU eviction, decay mechanisms
// Includes memory compression, consolidation mechanisms, and competition

class AdvancedWorkingMemory {
public:
    AdvancedWorkingMemory();
    ~AdvancedWorkingMemory();

    // Initialize with brain reference and performance optimizations
    void initialize(Brain* brain);

    // Store item with capacity management and LRU eviction
    void store(NeuronId neuron, float value, SimulationStep timestamp);

    // Retrieve item
    float retrieve(NeuronId neuron) const;

    // Check if item exists
    bool contains(NeuronId neuron) const;

    // Clear all
    void clear();

    // Capacity management
    void setCapacity(size_t cap) { capacity_ = cap; }
    size_t getCapacity() const { return capacity_; }
    size_t getCurrentSize() const { return memoryEntries_.size(); }

    // Decay management
    void setDecayRate(float rate) { decayRate_ = rate; }
    float getDecayRate() const { return decayRate_; }

    // Memory consolidation
    void consolidate(float threshold); // Remove weak traces

    // Competition mechanisms
    void runCompetition();
    bool isWinning(NeuronId neuron) const;
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Memory statistics
    float getMemoryActivity() const;
    float getTotalMemoryLoad() const;
    SimulationStep getOldestTraceAge() const;

    // Neural integration
    Brain* getBrain() const { return brain_; }

private:
    // Memory entry structure for advanced features
    struct MemoryEntry {
        NeuronId neuronId;
        float value;
        SimulationStep timestamp;
        SimulationStep lastAccessAge;
        float accessFrequency;
        float importanceScore;
        float decayFactor;

        MemoryEntry(NeuronId id, float val, SimulationStep ts)
            : neuronId(id), value(val), timestamp(ts), lastAccessAge(0),
              accessFrequency(1.0f), importanceScore(1.0f), decayFactor(1.0f) {}
    };

    // LRU management
    struct MemoryEntryCompare {
        bool operator()(const MemoryEntry* a, const MemoryEntry* b) {
            return a->lastAccessAge > b->lastAccessAge ||
                   (a->lastAccessAge == b->lastAccessAge && a->importanceScore < b->importanceScore);
        }
    };

    std::priority_queue<MemoryEntry*, std::vector<MemoryEntry*>, MemoryEntryCompare> lruQueue_;
    std::vector<MemoryEntry*> memoryEntries_;
    std::unordered_map<uint64_t, size_t> neuronToIndexMap_;

    // Brain integration
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    float totalMemoryLoad_;
    SimulationStep lastConsolidationStep_;
    std::vector<NeuronId> winners_;
    std::mutex memoryMutex_;
    bool compressionEnabled_;
};

// Episodic memory: storage of events and experiences with advanced features
// Includes temporal gap detection, memory aging/forgetting, relevance computation
// Supports indexing optimization and memory replay mechanisms

struct EnhancedEpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<float> sensoryState;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    ActionType action;
    float reward;
    float energy;
    float novelty;
    SimulationStep age;
    float relevanceScore;
    SimulationStep lastAccess;
    float consolidationPriority;
    std::vector<size_t> relatedEpisodes;
    std::vector<size_t> temporalGaps;

    EnhancedEpisodicMemoryItem()
        : timestamp(0), reward(0), energy(0), novelty(0), age(0),
          relevanceScore(0.5f), lastAccess(0), consolidationPriority(0.0f) {}
};

class AdvancedEpisodicMemory {
public:
    AdvancedEpisodicMemory();
    ~AdvancedEpisodicMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store experience with temporal gap detection
    void storeExperience(const std::vector<float>& sensoryState,
                        const std::vector<NeuronId>& activeNeurons,
                        const std::vector<float>& neuronActivations,
                        ActionType action,
                        float reward,
                        float novelty);

    // Retrieve episodes based on similarity with relevance weighting
    std::vector<const EnhancedEpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        float minRelevance = 0.3f,
        size_t maxResults = 10) const;

    // Temporal gap detection for compression
    void analyzeTemporalGaps();
    void compressTemporalGaps();

    // Memory aging and forgetting mechanisms
    void applyForgetting(float forgettingRate);
    void ageAllEpisodes();

    // Relevance computation based on prediction error
    void updateRelevanceBasedOnPredictionError(float predictionError,
                                              float confidence);
    void markImportantForConsolidation(const std::vector<size_t>& episodeIndices);

    // Memory retrieval optimization with indexing
    void buildTemporalIndex();
    void buildSensoryIndex();
    void buildActionIndex();

    // Replay mechanisms for consolidation
    void scheduleReplay(SimulationStep delaySteps);
    void replaySequence(const std::vector<size_t>& episodeIndices);
    void performRehearsal(size_t episodeIndex);

    // Consolidated episodic memory
    void consolidate(float relevanceThreshold);

    // Clear old episodes
    void clear();

    // Statistics
    size_t getEpisodeCount() const { return episodes_.size(); }
    float getAverageRelevance() const { return averageRelevance_; }
    float getCompressionRatio() const { return compressionRatio_; }

    // Neural integration
    Brain* getBrain() const { return brain_; }

private:
    std::deque<EnhancedEpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    float compressionThreshold_;
    Brain* brain_;
    std::vector<TemporalGap> temporalGaps_;
    std::vector<SensoryIndexEntry> sensoryIndex_;
    std::unordered_map<ActionType, std::vector<size_t>> actionIndex_;
    float averageRelevance_;
    float compressionRatio_;
    SimulationStep nextReplayStep_;
    SimulationStep lastConsolidationStep_;
    std::vector<size_t> replayQueue_;
    std::mutex replayMutex_;

    void buildIndexes();
    void detectTemporalGaps();
    void computeRelevanceScores(float predictionError, float confidence);
    void applyMemoryForgetting(float forgettingRate);
    void performConsolidation(float relevanceThreshold);
    void scheduleReplayEvents();
};

// Semantic memory: gradually acquired knowledge with semantic networks
// Includes semantic relevance, semantic similarity, pattern completion

class AdvancedSemanticMemory {
public:
    AdvancedSemanticMemory();
    ~AdvancedSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store fact with semantic relevance computation
    void storeFact(const std::string& concept,
                  const std::string& attribute,
                  const std::string& value,
                  float baseRelevance = 1.0f);

    // Retrieve fact with semantic similarity
    std::string retrieveFact(const std::string& concept,
                             const std::string& attribute) const;

    // Semantic network connection establishment
    void establishConnection(const std::string& conceptA,
                            const std::string& conceptB,
                            float connectionStrength = 1.0f);

    // Semantic similarity computation for fact retrieval
    float computeSemanticSimilarity(const std::string& conceptA,
                                   const std::string& conceptB) const;

    // Semantic pattern completion for missing information
    std::string completePattern(const std::string& partialConcept,
                               const std::string& partialAttribute) const;

    // Semantic abstraction and generalization
    void generalizePattern(const std::string& specificConcept);
    void createAbstraction(const std::vector<std::string>& relatedConcepts);

    // Get all facts
    std::vector<std::tuple<std::string, std::string, std::string>> getAllFacts() const;

    // Clear all
    void clear();

    // Statistics
    size_t getConceptCount() const { return concepts_.size(); }
    size_t getConnectionCount() const { return semanticConnections_.size(); }

    // Neural integration
    Brain* getBrain() const { return brain_; }

private:
    struct SemanticConcept {
        std::string id;
        std::unordered_map<std::string, std::string> attributes;
        std::vector<std::string> relatedConcepts;
        float activationLevel;
        float stability;
        SimulationStep lastAccessed;

        SemanticConcept(const std::string& conceptId)
            : id(conceptId), activationLevel(0.0f), stability(1.0f), lastAccessed(0) {}
    };

    struct SemanticConnection {
        std::string sourceConcept;
        std::string targetConcept;
        float strength;
        float asymmetry;
        SimulationStep formedTime;
        float decayRate;

        SemanticConnection(const std::string& src, const std::string& tgt, float str)
            : sourceConcept(src), targetConcept(tgt), strength(str),
              asymmetry(1.0f), formedTime(0), decayRate(0.001f) {}
    };

    struct CompletionRequest {
        std::string partialConcept;
        std::string partialAttribute;
        float confidence;
        std::vector<std::string> candidateConcepts;
    };

    std::unordered_map<std::string, SemanticConcept> concepts_;
    std::vector<SemanticConnection> semanticConnections_;
    std::vector<CompletionRequest> completionRequests_;
    std::unordered_map<std::string, std::vector<std::string>> reverseLookupIndex_;
    Brain* brain_;

    void updateConceptActivation(const std::string& conceptId, float delta);
    void applySemanticDecay();
    void completePatternRequests();
    void generalizeFromSimilarity(const std::string& conceptA, const std::string& conceptB);
};

// Procedural memory: learned skills and habits with adaptation
// Includes skill learning, hybridization, adaptation, decay, transfer

class AdvancedProceduralMemory {
public:
    AdvancedProceduralMemory();
    ~AdvancedProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn skill with proficiency tracking
    void learnSkill(const std::string& skillName,
                   const std::vector<NeuronId>& neuralPattern,
                   float initialProficiency = 0.1f);

    // Retrieve skill
    std::vector<float> retrieveSkill(const std::string& skillName) const;

    // Get all skills
    std::vector<std::string> getAllSkills() const;

    // Update proficiency based on performance feedback
    void updateProficiency(const std::string& skillName, float delta,
                         const std::vector<float>& performanceContext);

    // Skill combination and hybridization
    void combineSkills(const std::string& skillA, const std::string& skillB,
                      const std::string& hybridName);
    void hybridizeSkill(const std::string& sourceSkill,
                       const std::vector<float>& adaptationParameters);

    // Skill adaptation based on performance feedback
    void adaptSkill(const std::string& skillName,
                   const std::vector<float>& performanceMetrics,
                   float learningRate = 0.1f);

    // Skill decay and forgetting mechanisms
    void applySkillDecay(float decayRate);
    void forgetUnusedSkills(float threshold);

    // Skill transfer between similar tasks
    void transferSkill(const std::string& sourceSkill,
                      const std::string& targetSkill,
                      float transferStrength = 1.0f);
    void generalizeSkill(const std::string& sourceSkill,
                       const std::vector<std::string>& relatedSkills);

    // Clear all
    void clear();

    // Statistics
    size_t getSkillCount() const { return skills_.size(); }
    float getAverageProficiency() const { return averageProficiency_; }

    // Neural integration
    Brain* getBrain() const { return brain_; }

private:
    struct ProceduralSkill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        std::unordered_map<std::string, float> adaptationParameters;
        std::vector<std::string> relatedSkills;
        SimulationStep lastPracticed;
        float stability;
        float transferPotential;

        ProceduralSkill(const std::string& skillName,
                       const std::vector<NeuronId>& pattern,
                       float proficiency)
            : name(skillName), neuralPattern(pattern), proficiency(proficiency),
              stability(1.0f), transferPotential(0.5f), lastPracticed(0) {}
    };

    struct SkillHybrid {
        std::string hybridName;
        std::vector<std::string> parentSkills;
        std::vector<NeuronId> combinedPattern;
        float baseProficiency;
        SimulationStep createdTime;
    };

    struct AdaptationMetrics {
        std::vector<float> performanceHistory;
        std::vector<float> adaptationResponses;
        float learningRate;
        SimulationStep lastUpdate;
    };

    std::unordered_map<std::string, ProceduralSkill> skills_;
    std::vector<SkillHybrid> skillHybrids_;
    std::unordered_map<std::string, AdaptationMetrics> adaptationMetrics_;
    std::unordered_map<NeuronId, std::string> neuralToSkillMap_;
    Brain* brain_;
    float averageProficiency_;

    void updateSkillPattern(const std::string& skillName,
                           const std::vector<NeuronId>& newPattern);
    void computeSkillTransferability(const std::string& sourceSkill,
                                    const std::string& targetSkill);
    void applySkillAdaptation(const std::string& skillName,
                            const std::vector<float>& metrics,
                            float learningRate);
};

// Associative memory: relationships between representations with robust strength computation
// Includes spreading activation, forward/backward retrieval, chaining

class AdvancedAssociativeMemory {
public:
    AdvancedAssociativeMemory();
    ~AdvancedAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association with robust strength computation
    void associate(const std::vector<float>& patternA,
                  const std::vector<float>& patternB,
                  float baseStrength = 1.0f);

    // Get associated patterns with forward and backward retrieval
    std::vector<std::vector<float>> retrieveForward(
        const std::vector<float>& patternA,
        size_t maxResults = 5) const;

    std::vector<std::vector<float>> retrieveBackward(
        const std::vector<float>& patternB,
        size_t maxResults = 5) const;

    // Get association strength
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Spread activation mechanisms
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 3) const;

    // Association chaining for complex relationships
    void establishChain(const std::vector<std::vector<float>>& patternSequence);
    std::vector<std::vector<float>> retrieveViaChain(
        const std::vector<float>& startingPattern,
        size_t maxChainLength = 3) const;

    // Association decay and strengthening mechanisms
    void applyAssociationDecay(float decayRate);
    void strengthenAssociations(const std::vector<float>& contextPattern);

    // Clear associations
    void clear();

    // Statistics
    size_t getAssociationCount() const { return associations_.size(); }
    size_t getPatternCount() const { return patternNeurons_.size(); }

    // Neural integration
    Brain* getBrain() const { return brain_; }

private:
    struct Association {
        std::vector<float> patternA;
        std::vector<float> patternB;
        float strength;
        float asymmetry;
        SimulationStep formedTime;
        float decayRate;
        float forwardSpread;
        float backwardSpread;
        std::vector<size_t> chainIndices;

        Association(const std::vector<float>& a, const std::vector<float>& b, float strength)
            : patternA(a), patternB(b), strength(strength), asymmetry(1.0f),
              formedTime(0), decayRate(0.001f), forwardSpread(0.0f),
              backwardSpread(0.0f) {}
    };

    struct PatternNeuron {
        std::vector<float> pattern;
        NeuronId neuronId;
        float activationLevel;
        std::vector<size_t> associatedAssociations;

        PatternNeuron(const std::vector<float>& pat, NeuronId id)
            : pattern(pat), neuronId(id), activationLevel(0.0f) {}
    };

    struct AssociationChainLink {
        size_t associationIndex;
        float activationFlow;
        SimulationStep activationTime;
    };

    std::vector<Association> associations_;
    std::vector<PatternNeuron> patternNeurons_;
    std::vector<std::vector<AssociationChainLink>> associationChains_;
    std::unordered_map<size_t, std::vector<float>> patternCache_;
    std::mutex associationMutex_;
    Brain* brain_;

    void updateAssociationStrengths();
    void computeSpreadActivation();
    void maintainAssociationChains();
    void applyAssociationDecay(float decayRate);
    void establishAssociationChain(const std::vector<size_t>& associationIndices);
};

// Integration hooks with Brain
void integrateMemorySystems(Brain* brain);
void registerMemoryCallbacks(Brain* brain);

} // namespace nlm
