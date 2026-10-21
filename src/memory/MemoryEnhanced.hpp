#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <queue>
#include <random>
#include <chrono>

namespace nlm {

// Forward declarations for brain integration
class Brain;

// ============================================================================
// ENHANCED WORKING MEMORY
// ============================================================================

// WorkingMemoryTrace: Represents a single memory trace with advanced features
struct WorkingMemoryTrace {
    SimulationStep timestamp;
    std::vector<float> pattern;
    float activation;
    float strength;
    SimulationStep age;
    SimulationStep lastRehearsal;
    float interferenceWeight;
    std::unordered_set<NeuronId> interferingNeurons;
    std::unordered_set<NeuronId> supportingNeurons;
    
    // Temporal decay parameters
    float decayTimeConstant;
    float retrievalThreshold;
    
    WorkingMemoryTrace()
        : timestamp(0), activation(0.0f), strength(1.0f), age(0)
        , lastRehearsal(0), interferenceWeight(0.0f)
        , decayTimeConstant(1000.0f), retrievalThreshold(0.1f) {}
    
    WorkingMemoryTrace(const std::vector<float>& pat, float act = 1.0f)
        : timestamp(0), pattern(pat), activation(act), strength(1.0f)
        , age(0), lastRehearsal(0), interferenceWeight(0.0f)
        , decayTimeConstant(1000.0f), retrievalThreshold(0.1f) {}
};

// Capacity management strategies
enum class CapacityStrategy {
    FIFO,           // First-In-First-Out
    LRU,            // Least-Recently-Used
    WEIGHTED,       // Based on trace strength
    DYNAMIC,        // Adaptive based on competition
    PRIORITY       // Based on task relevance
};

// Replacement strategy for working memory
enum class ReplacementStrategy {
    RANDOM,         // Random replacement
    WEAKEST,        // Replace weakest activation
    OLDEST,         // Replace oldest trace
    LEAST_SUPPORT,  // Replace with least support
    COMPETITIVE     // Replace via competition
};

// ============================================================================
// NeuralWorkingMemoryEnhanced: Enhanced working memory with advanced features
// ============================================================================
class NeuralWorkingMemoryEnhanced {
public:
    NeuralWorkingMemoryEnhanced();
    ~NeuralWorkingMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhanced storage with pattern completion
    void store(const std::vector<float>& pattern, float strength = 1.0f,
               const std::vector<float>* partialCue = nullptr);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve with pattern completion
    std::vector<float> retrieve(const std::vector<float>* cue = nullptr) const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;
    
    // Enhanced update with temporal dynamics
    void update(TimestepDuration dt, bool isResting = false);
    
    // Clear working memory
    void clear();
    
    // Capacity management
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Decay parameters
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    float getDecayTimeConstant() const { return decayTimeConstant_; }
    void setDecayTimeConstant(float constant) { decayTimeConstant_ = constant; }
    
    // Competition parameters
    void setInterferenceThreshold(float threshold) { interferenceThreshold_ = threshold; }
    void setCompetitionIntensity(float intensity) { competitionIntensity_ = intensity; }
    
    // Temporal decay mechanisms
    void applyTemporalDecay(float dt);
    void applyRehearsalBoost(const std::vector<NeuronId>& rehearsingNeurons);
    
    // Pattern completion with partial cues
    std::vector<float> completePattern(const std::vector<float>& cue) const;
    float getPatternSimilarity(const std::vector<float>& pattern1,
                              const std::vector<float>& pattern2) const;
    
    // Interference management
    void registerInterference(NeuronId sourceNeuron, NeuronId targetNeuron);
    void resolveInterference(const std::vector<float>& currentActivity);
    
    // Replacement strategies
    void setCapacityStrategy(CapacityStrategy strategy) { capacityStrategy_ = strategy; }
    void setReplacementStrategy(ReplacementStrategy strategy) { replacementStrategy_ = strategy; }
    void enforceCapacity();
    
    // Advanced retrieval mechanisms
    size_t getActiveTraces() const { return traces_.size(); }
    float getMemoryActivity() const;
    float getPatternStability(const std::vector<float>& pattern) const;
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);
    
    // Competition between memory traces
    void runAdvancedCompetition();
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;
    
    // Temporal sequence encoding
    void storeTemporalSequence(const std::vector<std::vector<float>>& sequence,
                               float temporalSpacing = 1.0f);
    
    // Chunking and compression
    void chunkMemoryTraces();
    std::vector<std::vector<float>> getCompressedMemory() const;
    
    // Memory-guided attention
    void updateAttentionalFocus(const std::vector<float>& taskRequirements);
    std::vector<NeuronId> getAttendedMemoryNeurons() const { return attendedNeurons_; }
    
    // Sleep/rest processing
    void processSleepRest(bool isResting, float sleepQuality);
    void consolidateWorkingMemory(float consolidationStrength);
    
    // Get recent activity
    std::vector<std::pair<SimulationStep, std::vector<float>>> getRecentActivity(
        size_t maxCount = 10) const;
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    float decayTimeConstant_;
    float interferenceThreshold_;
    float competitionIntensity_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Advanced traces
    std::vector<WorkingMemoryTrace> traces_;
    std::vector<size_t> traceIndices_;  // Map from neuron to trace index
    
    // Attention and focus
    std::vector<NeuronId> attendedNeurons_;
    std::vector<float> attentionalWeights_;
    
    // Interference tracking
    std::vector<std::pair<NeuronId, NeuronId>> interferencePairs_;
    
    // Competition and selection
    std::vector<NeuronId> winners_;
    std::vector<float> competitionScores_;
    
    // Strategies
    CapacityStrategy capacityStrategy_;
    ReplacementStrategy replacementStrategy_;
    
    // Temporal processing
    SimulationStep lastSequenceStep_;
    std::vector<std::vector<float>> recentSequences_;
    
    // Helper methods
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    void updateRecurrentConnections();
    void decayWeakTraces();
    void applyInterferenceEffects();
    void updateAttentionalWeights();
    WorkingMemoryTrace* findOrCreateTrace(NeuronId neuron);
    void updateTraceActivity(const WorkingMemoryTrace& trace);
    std::vector<float> computePatternCompletion(const std::vector<float>& cue) const;
};

// ============================================================================
// ENHANCED EPISODIC MEMORY
// ============================================================================

// CausalRelationship: Represents causal connections between events
struct CausalRelationship {
    SimulationStep causeTime;
    SimulationStep effectTime;
    std::vector<NeuronId> causeNeurons;
    std::vector<NeuronId> effectNeurons;
    float causalStrength;
    std::string causalDescription;
    
    CausalRelationship(SimulationStep cause, SimulationStep effect,
                       const std::vector<NeuronId>& causeN,
                       const std::vector<NeuronId>& effectN,
                       float strength = 1.0f)
        : causeTime(cause), effectTime(effect), causeNeurons(causeN),
          effectNeurons(effectN), causalStrength(strength) {}
};

// TemporalSequence: Represents a sequence of related events
struct TemporalSequence {
    std::vector<size_t> episodeIndices;
    float temporalCohesion;
    float predictivePower;
    std::vector<CausalRelationship> causalLinks;
    
    TemporalSequence() : temporalCohesion(0.0f), predictivePower(0.0f) {}
};

// ============================================================================
// NeuralEpisodicMemoryEnhanced: Enhanced episodic memory with temporal and causal features
// ============================================================================
class NeuralEpisodicMemoryEnhanced {
public:
    NeuralEpisodicMemoryEnhanced();
    ~NeuralEpisodicMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhanced storage with temporal sequence encoding
    void storeEpisode(const EpisodicMemoryItem& episode,
                     const std::vector<size_t>* relatedEpisodeIndices = nullptr);
    
    // Enhanced retrieval with temporal windows and causal context
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5,
        const std::vector<float>* temporalContext = nullptr) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    std::vector<const EpisodicMemoryItem*> retrieveCausal(
        const std::vector<NeuronId>& causeNeurons,
        SimulationStep timeWindow = 100,
        size_t maxResults = 5) const;
    
    // Temporal sequence analysis and retrieval
    void analyzeTemporalSequences();
    std::vector<TemporalSequence> getTemporalSequences() const { return sequences_; }
    
    // Causal relationship storage and retrieval
    void storeCausalRelationship(const CausalRelationship& causalRel);
    std::vector<const CausalRelationship*> getCausalRelationships(
        const std::vector<NeuronId>& neurons) const;
    
    // Enhanced replay with consolidation
    void replayEpisode(const EpisodicMemoryItem* episode, bool withStrengthening = true);
    void replayTemporalWindow(SimulationStep startTime, SimulationStep endTime);
    void replaySequence(const std::vector<size_t>& episodeIndices);
    
    // Sleep/rest-based consolidation
    void consolidateDuringSleep(float sleepQuality, SimulationStep sleepStartTime);
    void strengthenRecentEpisodes(float strengthFactor);
    void weakenUnusedEpisodes(float threshold);
    
    // Memory strengthening through rehearsal
    void rehearseEpisode(size_t episodeIndex, float rehearsalStrength = 1.0f);
    
    // Predictive coding and anticipation
    void updatePredictions(const std::vector<float>& currentPattern);
    std::vector<float> predictNextEpisode() const;
    
    // Cross-memory integration
    void integrateWithSemanticMemory(const SemanticMemory& semanticMem);
    void integrateWithWorkingMemory(NeuralWorkingMemoryEnhanced& workingMem);
    
    // Temporal pattern completion
    std::vector<const EpisodicMemoryItem*> retrieveTemporalPattern(
        SimulationStep startTime,
        size_t patternLength,
        float similarityThreshold = 0.7f) const;
    
    // Statistical properties of memory
    float getMemoryCohesion(const std::vector<size_t>& episodeIndices) const;
    float getTemporalCoherence(SimulationStep startTime, SimulationStep endTime) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Memory statistics
    float getAverageEpisodeStrength() const;
    float getAverageTemporalCohesion() const;
    float getMemoryLoad() const;  // Fraction of capacity used
    
    // Clear old episodes
    void consolidate(float relevanceThreshold) override;
    void clearOldEpisodes(SimulationStep beforeTime);
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const override;
    
    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const { return &episodes_[index]; }
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const override;
    
    // Get average reward from episodes
    float getAverageReward() const { return averageReward_; }
    
    // Clear all episodes
    void clear() override;
    
    // Update episode relevance based on recent use
    void updateRelevance(SimulationStep episodeId, float relevanceDelta) override;
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Enhanced storage
    std::vector<std::vector<size_t>> temporalSequences_;
    std::vector<CausalRelationship> causalRelationships_;
    std::vector<TemporalSequence> sequences_;
    
    // Predictive coding
    std::vector<float> predictionPattern_;
    std::vector<float> predictionError_;
    
    // Sleep/rest processing
    SimulationStep lastSleepTime_;
    float sleepQuality_;
    
    // Statistics
    float averageReward_;
    SimulationStep lastConsolidationStep_;
    
    // Helper methods
    void detectTemporalSequences();
    void extractCausalRelationships();
    void updateTemporalCohesion();
    std::vector<const EpisodicMemoryItem*> filterByTemporalWindow(
        const std::vector<const EpisodicMemoryItem*>& candidates,
        SimulationStep startTime, SimulationStep endTime) const;
    float computeTemporalSimilarity(const EpisodicMemoryItem& episode1,
                                   const EpisodicMemoryItem& episode2) const;
    void updatePredictiveModel(const std::vector<float>& currentPattern);
    std::vector<size_t> findRelatedEpisodes(size_t episodeIndex) const;
};

// ============================================================================
// ENHANCED ASSOCIATIVE MEMORY
// ============================================================================

// HigherOrderAssociation: Represents third-order and higher relationships
struct HigherOrderAssociation {
    std::vector<NeuronId> constituentNeurons;  // Neurons involved in the association
    std::vector<float> patternWeights;         // Strength of each neuron
    float associationStrength;                 // Overall association strength
    std::string associationType;               // Type of association (e.g., "temporal", "causal")
    
    HigherOrderAssociation(const std::vector<NeuronId>& neurons,
                          const std::vector<float>& weights,
                          float strength = 1.0f,
                          const std::string& type = "")
        : constituentNeurons(neurons), patternWeights(weights),
          associationStrength(strength), associationType(type) {}
};

// ============================================================================
// NeuralAssociativeMemoryEnhanced: Enhanced associative memory with sparse representations
// ============================================================================
class NeuralAssociativeMemoryEnhanced {
public:
    NeuralAssociativeMemoryEnhanced();
    ~NeuralAssociativeMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhanced storage with sparse distributed representations
    void store(const std::vector<float>& patternA,
               const std::vector<float>& patternB,
               float strength = 1.0f,
               bool useSparseRepresentation = true);
    
    // Enhanced retrieval with pattern completion
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 5,
        float completionThreshold = 0.7f) const;
    
    // Pattern separation for similar patterns
    void separateSimilarPatterns(const std::vector<float>& pattern1,
                                const std::vector<float>& pattern2);
    
    // Higher-order associations
    void createHigherOrderAssociation(
        const std::vector<std::vector<float>>& patterns,
        const std::string& associationType = "");
    
    // Spread activation with multiple cues
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 3,
        const std::vector<float>* additionalCues = nullptr) const;
    
    // Enhanced pattern completion
    std::vector<float> completePattern(const std::vector<float>& partialPattern,
                                      size_t maxCompletionAttempts = 5) const;
    
    // Cross-pattern similarity
    float getCrossPatternSimilarity(const std::vector<float>& pattern1,
                                   const std::vector<float>& pattern2) const;
    
    // Multiple cue associative recall
    std::vector<std::vector<float>> retrieveWithMultipleCues(
        const std::vector<std::vector<float>>& cuePatterns,
        size_t maxResults = 5) const;
    
    // Integration with episodic memory
    void integrateWithEpisodicMemory(const NeuralEpisodicMemoryEnhanced& episodicMem);
    
    // Integration with working memory
    void integrateWithWorkingMemory(const NeuralWorkingMemoryEnhanced& workingMem);
    
    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta,
                          bool reinforce = true);
    
    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    // Clear associations
    void clear() { patternNeurons_.clear(); associations_.clear(); }
    
    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Statistical properties
    float getAverageAssociationStrength() const;
    float getPatternDensity() const;  // Fraction of possible connections
    std::vector<float> getRepresentationSparseness() const;
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Enhanced storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    std::vector<std::vector<size_t>> higherOrderAssociations_;
    
    // Sparse representation
    std::vector<std::vector<float>> sparsePatternCache_;
    
    // Multiple cue processing
    std::vector<std::vector<float>> cuePatternCache_;
    
    // Helper methods
    NeuronId findOrCreatePatternNeuron(const std::vector<float>& pattern,
                                      bool useSparse = true);
    void createSparseRepresentation(const std::vector<float>& pattern,
                                    NeuronId neuronId);
    float computeSparseSimilarity(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;
    std::vector<float> performPatternCompletion(const std::vector<float>& partialPattern,
                                                size_t maxAttempts) const;
    void updateHigherOrderAssociations(const std::vector<float>& pattern1,
                                      const std::vector<float>& pattern2);
};

// ============================================================================
// PROCEDURAL MEMORY
// ============================================================================

// ProceduralMemoryItem: Represents a learned skill or habit
struct ProceduralMemoryItem {
    std::string skillName;
    std::vector<NeuronId> neuralPattern;
    std::vector<std::vector<NeuronId>> sequencePattern;
    float proficiency;
    float executionEfficiency;
    SimulationStep lastPractice;
    SimulationStep acquisitionTime;
    std::vector<float> successHistory;
    
    ProceduralMemoryItem(const std::string& name,
                        const std::vector<NeuronId>& pattern,
                        const std::vector<std::vector<NeuronId>>& sequence)
        : skillName(name), neuralPattern(pattern), sequencePattern(sequence),
          proficiency(0.0f), executionEfficiency(0.0f), lastPractice(0),
          acquisitionTime(0), successHistory() {}
};

// ============================================================================
// ProceduralMemoryEnhanced: Skills and habits with reinforcement learning
// ============================================================================
class ProceduralMemoryEnhanced {
public:
    ProceduralMemoryEnhanced();
    ~ProceduralMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn new skill
    void learnSkill(const std::string& name,
                   const std::vector<NeuronId>& neuralPattern,
                   const std::vector<std::vector<NeuronId>>& sequencePattern = {});
    
    // Retrieve skill for execution
    ProceduralMemoryItem* getSkill(const std::string& name);
    ProceduralMemoryItem* getMostSimilarSkill(const std::vector<NeuronId>& neuralPattern);
    
    // Get all skills
    std::vector<ProceduralMemoryItem*> getAllSkills();
    
    // Update proficiency through practice
    void updateProficiency(const std::string& name, float delta,
                          bool successfulExecution = true);
    void practiceSkill(const std::string& name, bool successful = true);
    
    // Execute skill (neural pattern activation)
    std::vector<NeuronId> executeSkill(const std::string& name,
                                      float executionStrength = 1.0f);
    
    // Skill adaptation and refinement
    void adaptSkill(const std::string& name,
                   const std::vector<NeuronId>& refinedPattern,
                   const std::vector<std::vector<NeuronId>>& refinedSequence);
    
    // Transfer learning between skills
    void transferLearning(const std::string& sourceSkill,
                         const std::string& targetSkill,
                         float transferStrength = 0.5f);
    
    // Integration with episodic memory
    void learnFromExperience(const EpisodicMemoryItem& episode);
    
    // Integration with working memory
    void updateFromWorkingMemory(const NeuralWorkingMemoryEnhanced& workingMem);
    
    // Skill statistics and analytics
    float getSkillProficiency(const std::string& name) const;
    float getAverageExecutionEfficiency() const;
    std::vector<std::string> getMostPracticedSkills(size_t count = 5) const;
    
    // Clear all skills
    void clear();
    
    // Decay skills that haven't been used
    void decayUnusedSkills(SimulationStep currentStep, float decayThreshold = 1000);
    
    // Get skills within certain proficiency range
    std::vector<ProceduralMemoryItem*> getSkillsByProficiency(float minProficiency,
                                                            float maxProficiency = 1.0f) const;
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Skill storage
    std::vector<ProceduralMemoryItem> skills_;
    std::unordered_map<std::string, size_t> skillNameToIndex_;
    
    // Learning statistics
    SimulationStep lastExperienceLearning_;
    float totalPracticeTime_;
    float overallProficiency_;
};

// ============================================================================
// SEMANTIC MEMORY
// ============================================================================

// SemanticMemoryNode: Represents a concept or knowledge unit
struct SemanticMemoryNode {
    std::string conceptName;
    std::vector<float> conceptVector;  // Distributed representation
    std::vector<std::string> relatedConcepts;
    std::vector<std::pair<std::string, float>> propertyWeights;
    float activationLevel;
    float stability;  // Resistance to change
    SimulationStep lastUpdated;
    SimulationStep acquisitionTime;
    std::vector<float> contextualActivation;
    
    SemanticMemoryNode(const std::string& name,
                      const std::vector<float>& vector,
                      const std::vector<std::pair<std::string, float>>& properties)
        : conceptName(name), conceptVector(vector), propertyWeights(properties),
          activationLevel(0.0f), stability(1.0f), lastUpdated(0),
          acquisitionTime(0), contextualActivation() {}
};

// SemanticMemoryLink: Represents relationship between concepts
struct SemanticMemoryLink {
    std::string sourceConcept;
    std::string targetConcept;
    std::string relationshipType;  // "is_a", "has_property", "causes", etc.
    float connectionStrength;
    float bidirectionalWeight;
    SimulationStep lastReinforcement;
    
    SemanticMemoryLink(const std::string& source, const std::string& target,
                      const std::string& type, float strength = 1.0f)
        : sourceConcept(source), targetConcept(target), relationshipType(type),
          connectionStrength(strength), bidirectionalWeight(strength),
          lastReinforcement(0) {}
};

// ============================================================================
// SemanticMemoryEnhanced: Knowledge representation with conceptual coherence
// ============================================================================
class SemanticMemoryEnhanced {
public:
    SemanticMemoryEnhanced();
    ~SemanticMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store new fact/concept
    void storeFact(const std::string& key, const std::string& value,
                  const std::vector<float>& conceptVector = {});
    void storeConcept(const std::string& name,
                     const std::vector<std::pair<std::string, float>>& properties,
                     const std::vector<float>& vector = {});
    
    // Retrieve facts/concepts with pattern completion
    std::string retrieveFact(const std::string& key) const;
    SemanticMemoryNode* retrieveConcept(const std::string& name);
    std::vector<SemanticMemoryNode*> retrieveSimilarConcepts(
        const std::vector<float>& queryVector,
        size_t maxResults = 5,
        float similarityThreshold = 0.6f) const;
    
    // Check if fact/concept exists
    bool hasFact(const std::string& key) const;
    bool hasConcept(const std::string& name) const;
    
    // Get all facts/concepts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    std::vector<SemanticMemoryNode*> getAllConcepts() const;
    
    // Store relationship between concepts
    void storeRelationship(const std::string& source,
                          const std::string& target,
                          const std::string& type,
                          float strength = 1.0f);
    
    // Retrieve related concepts
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const;
    float getConceptSimilarity(const std::string& concept1,
                              const std::string& concept2) const;
    
    // Concept inference and generalization
    std::vector<std::string> inferRelatedConcepts(const std::string& concept) const;
    std::string generalizeConcept(const std::string& concept) const;
    
    // Integration with episodic memory
    void learnFromExperience(const EpisodicMemoryItem& episode);
    void updateConceptFromEpisode(const std::string& concept,
                                 const EpisodicMemoryItem& episode);
    
    // Integration with working memory
    void updateFromWorkingMemory(const NeuralWorkingMemoryEnhanced& workingMem,
                                const std::vector<float>& contextVector);
    
    // Knowledge coherence and consistency
    float getKnowledgeCoherence() const;
    std::vector<std::string> getIncoherentConcepts() const;
    void resolveInconsistencies();
    
    // Clear all knowledge
    void clear();
    
    // Concept decay and updating
    void decayUnusedConcepts(SimulationStep currentStep, float decayThreshold = 1000);
    void updateConceptStability();
    
    // Statistical properties
    float getAverageConceptActivation() const;
    float getAverageConceptStability() const;
    size_t getConceptCount() const { return concepts_.size(); }
    
    // Context-dependent retrieval
    void setCurrentContext(const std::vector<float>& context);
    std::vector<float> getCurrentContext() const { return currentContext_; }
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Knowledge storage
    std::vector<std::pair<std::string, std::string>> facts_;
    std::vector<SemanticMemoryNode> concepts_;
    std::vector<SemanticMemoryLink> conceptLinks_;
    
    // Context and state
    std::vector<float> currentContext_;
    SimulationStep lastUpdateTime_;
    
    // Statistics
    float totalKnowledgeScore_;
    float coherenceScore_;
};

// ============================================================================
// EMOTIONAL MEMORY
// ============================================================================

// EmotionalMemoryItem: Represents emotional associations and affect
struct EmotionalMemoryItem {
    std::string eventDescription;
    SimulationStep timestamp;
    float emotionalValence;  // -1.0 (negative) to +1.0 (positive)
    float emotionalArousal;  // 0.0 (calm) to 1.0 (excited)
    std::vector<NeuronId> associatedNeurons;
    std::vector<float> intensityHistory;
    float forgettingRate;  // How quickly the emotion fades
    float reinforcementValue;  // Value for reinforcement learning
    
    EmotionalMemoryItem(const std::string& description, SimulationStep time,
                       float valence, float arousal)
        : eventDescription(description), timestamp(time),
          emotionalValence(valence), emotionalArousal(arousal),
          forgettingRate(0.001f), reinforcementValue(0.0f) {}
};

// ============================================================================
// EmotionalMemoryEnhanced: Affect associations and emotional learning
// ============================================================================
class EmotionalMemoryEnhanced {
public:
    EmotionalMemoryEnhanced();
    ~EmotionalMemoryEnhanced();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store emotional experience
    void storeEmotionalEvent(const std::string& description,
                            SimulationStep timestamp,
                            float valence,
                            float arousal,
                            const std::vector<NeuronId>& associatedNeurons = {});
    
    // Retrieve emotional associations
    EmotionalMemoryItem* getEmotionalEvent(SimulationStep timestamp);
    std::vector<EmotionalMemoryItem*> getEventsWithSimilarValence(float valence,
                                                                 float threshold = 0.3f) const;
    float getCurrentEmotionalState() const { return currentEmotionalState_; }
    std::pair<float, float> getCurrentEmotion() const { return currentEmotion_; }
    
    // Emotional modulation of memory processes
    float getEmotionalModulationFactor(const std::string& memoryType) const;
    void applyEmotionalModulation(float valence, float arousal);
    
    // Integration with episodic memory
    void integrateWithEpisodicMemory(const EpisodicMemoryItem& episode);
    void updateEmotionalAssociations(const EpisodicMemoryItem& episode);
    
    // Integration with working memory
    void updateFromWorkingMemory(const NeuralWorkingMemoryEnhanced& workingMem,
                                float emotionalContext = 0.0f);
    
    // Emotional learning and adaptation
    void updateEmotionalResponse(const std::string& event,
                                 float actualValence,
                                 float actualArousal,
                                 float learningRate = 0.1f);
    void reinforceEmotionalMemory(const std::string& event,
                                 float reinforcementStrength = 1.0f);
    
    // Memory consolidation with emotional significance
    void consolidateEmotionallySignificantMemories(float emotionalThreshold);
    void strengthenPositiveMemories(float strengthFactor);
    void weakenNegativeMemories(float decayFactor);
    
    // Statistical properties
    float getAverageEmotionalValence() const;
    float getAverageEmotionalArousal() const;
    float getEmotionalMemoryLoad() const;  // Fraction of capacity used
    
    // Clear old emotional memories
    void clearOldEmotionalMemories(SimulationStep beforeTime);
    
    // Current emotional state management
    void updateEmotionalState(float valenceDelta, float arousalDelta);
    void resetEmotionalState();
    
    // Get all emotional events
    std::vector<EmotionalMemoryItem*> getAllEvents() { return emotionalEvents_; }
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Emotional storage
    std::vector<EmotionalMemoryItem> emotionalEvents_;
    
    // Current emotional state
    float currentEmotionalState_;
    std::pair<float, float> currentEmotion_;  // (valence, arousal)
    
    // Emotional modulation
    float emotionalModulationWorkingMemory_;
    float emotionalModulationEpisodic_;
    
    // Learning statistics
    float totalEmotionalLearning_;
    SimulationStep lastEmotionalUpdate_;
};

// ============================================================================
// MEMORY INTEGRATION IMPROVEMENTS
// ============================================================================

// MemoryGateway: Cross-memory connectivity and coordination
class MemoryGateway {
public:
    MemoryGateway();
    ~MemoryGateway();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Cross-memory connectivity
    void connectWorkingEpisodic(NeuralWorkingMemoryEnhanced& workingMem,
                               NeuralEpisodicMemoryEnhanced& episodicMem);
    void connectEpisodicSemantic(NeuralEpisodicMemoryEnhanced& episodicMem,
                                 SemanticMemoryEnhanced& semanticMem);
    void connectSemanticAssociative(SemanticMemoryEnhanced& semanticMem,
                                    NeuralAssociativeMemoryEnhanced& associativeMem);
    void connectAssociativeProcedural(NeuralAssociativeMemoryEnhanced& associativeMem,
                                      ProceduralMemoryEnhanced& proceduralMem);
    
    // Memory-guided attention
    void processAttentionalRequests(const std::vector<float>& taskRequirements,
                                    NeuralWorkingMemoryEnhanced& workingMem,
                                    NeuralEpisodicMemoryEnhanced& episodicMem);
    
    // Memory-based prediction
    std::vector<float> generatePrediction(
        NeuralWorkingMemoryEnhanced& workingMem,
        NeuralEpisodicMemoryEnhanced& episodicMem,
        const std::vector<float>& currentState);
    
    // Memory consolidation algorithms
    void consolidateAcrossSystems(float consolidationStrength);
    void optimizeMemoryAllocation();
    void balanceMemoryLoad();
    
    // Integration statistics
    float getCrossMemoryCohesion() const { return crossMemoryCohesion_; }
    std::vector<float> getMemorySystemActivation() const { return systemActivations_; }
    
    // Reset gateway
    void reset() {
        crossMemoryCohesion_ = 0.0f;
        systemActivations_.clear();
    }
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Integration state
    float crossMemoryCohesion_;
    std::vector<float> systemActivations_;
    
    // Connection management
    std::vector<std::pair<std::string, std::string>> memoryConnections_;
};

} // namespace nlm
