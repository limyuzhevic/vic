#pragma once

#include "Memory.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

// Enhanced Working Memory with advanced mechanisms
// Implements working memory using neural dynamics with additional features:
// - Buffer capacity management
// - Multimodal storage (sensory, motor, internal)
// - Attentional gating
// - Interference management
// - Chunking for complex information

class EnhancedWorkingMemory : public WorkingMemory {
public:
    EnhancedWorkingMemory();
    ~EnhancedWorkingMemory() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Store item with type and metadata
    void store(NeuronId neuron, float value, 
              const std::string& type = "general",
              const std::string& source = "sensory",
              float priority = 1.0f) override;
    
    // Store complex pattern
    void storePattern(const std::vector<NeuronId>& neurons,
                     const std::vector<float>& activations,
                     const std::string& context = "default");
    
    // Retrieve with context
    float retrieve(NeuronId neuron, const std::string& context = "default") const override;
    
    // Get item with metadata
    struct MemoryItem {
        NeuronId neuron;
        float value;
        std::string type;
        std::string source;
        float priority;
        SimulationStep timestamp;
        float decayRate;
        
        MemoryItem() : neuron(), value(0.0f), priority(1.0f), timestamp(0), decayRate(0.01f) {}
    };
    
    MemoryItem getItem(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const override;
    
    // Clear all with option to consolidate
    void clear() override;
    void clearByType(const std::string& type);
    
    // Get capacity
    size_t getCapacity() const override;
    void setCapacity(size_t cap) override;
    
    // Decay management
    void decay(float decayRate) override;
    void setDecayRate(float rate) override;
    void setContextDecayRate(const std::string& context, float rate);
    
    // Chunking: group related items for efficiency
    void createChunk(const std::string& chunkId, const std::vector<NeuronId>& neurons);
    void removeChunk(const std::string& chunkId);
    std::vector<NeuronId> getChunk(const std::string& chunkId) const;
    
    // Attentional gating
    void setAttentionalFocus(const std::string& context, bool enabled);
    bool isInAttentionalFocus(const std::string& context) const;
    
    // Interference management
    void setInterferenceStrength(float strength);
    void increaseInterference(NeuronId interferingNeuron, float strength);
    float getInterferenceLevel(NeuronId neuron) const;
    
    // Update with advanced decay and competition
    void update(TimestepDuration dt, const std::vector<NeuronId>* competitionNeurons = nullptr);
    
    // Get current state
    std::vector<MemoryItem> getAllItems() const;
    std::vector<NeuronId> getActiveNeurons() const;
    float getTotalActivation() const;
    
    // Chunk management
    void setChunkDecayRate(const std::string& chunkId, float rate);
    float getChunkActivity(const std::string& chunkId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Episodic Memory with replay mechanisms
// Implements episodic memory using neural patterns with additional features:
// - Replay during offline periods
// - Scene construction and binding
// - Temporal compression
// - Memory consolidation
// - Autobiographical organization

class AdvancedEpisodicMemory : public EpisodicMemory {
public:
    AdvancedEpisodicMemory();
    ~AdvancedEpisodicMemory() override;
    
    // Store episode with rich context
    void storeEpisode(const EpisodicMemoryItem& episode) override;
    
    // Store episode with scene and temporal information
    void storeEpisode(const std::vector<NeuronId>& neuralPattern,
                     const std::string& scene,
                     SimulationStep timestamp,
                     const std::string& narrative = "");
    
    // Enhanced retrieval with temporal context
    EpisodicMemoryItem retrieveEpisode(size_t index) const override;
    EpisodicMemoryItem retrieveEpisodeByTimestamp(SimulationStep timestamp) const;
    std::vector<EpisodicMemoryItem> retrieveTimeline(SimulationStep start, SimulationStep end) const;
    
    // Get episode count with filtering
    size_t getEpisodeCount() const override;
    size_t getEpisodeCountByContext(const std::string& context) const;
    size_t getEpisodeCountByScene(const std::string& scene) const;
    
    // Get recent episodes with enhanced criteria
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const override;
    std::vector<EpisodicMemoryItem> getRecentEpisodesByContext(const std::string& context, size_t count) const;
    
    // Memory consolidation with multiple mechanisms
    void consolidate(float relevanceThreshold) override;
    void consolidateSceneMemory();
    void consolidateTemporalSequence();
    void consolidateNarrativelyRelatedEpisodes();
    
    // Replay mechanisms
    void initiateReplay(SimulationStep startTime, SimulationStep endTime);
    void updateReplay();  // Continue replay process
    bool isReplaying() const;
    
    // Memory strengthening
    void strengthenEpisode(size_t index, float factor);
    void mergeSimilarEpisodes();  // Merge overlapping temporal episodes
    
    // Autobiographical organization
    void setEpisodeContext(size_t index, const std::string& context);
    std::vector<size_t> getEpisodesByContext(const std::string& context) const;
    
    // Temporal ordering
    void establishTemporalOrder(size_t currentIndex, size_t relatedIndex, bool forward = true);
    std::vector<size_t> getTemporalSequence(size_t anchorIndex) const;
    
    // Clear with options
    void clear() override;
    void clearByContext(const std::string& context);
    void clearByScene(const std::string& scene);
    
    // Statistics and analysis
    float getMemoryDensity() const;  // Neural density of memory storage
    float getTemporalCohesion() const;  // How well temporally related episodes are linked
    std::vector<std::string> getScenes() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic Memory with knowledge graph and inference
// Implements semantic memory using associative networks with additional features:
// - Knowledge graph structure
// - Conceptual hierarchies
// - Semantic associations
// - Inference and reasoning
// - Category learning

class EnhancedSemanticMemory : public SemanticMemory {
public:
    EnhancedSemanticMemory();
    ~EnhancedSemanticMemory() override;
    
    // Store fact with semantic context
    void storeFact(const std::string& key, const std::string& value) override;
    
    // Store concept with relationships
    void storeConcept(const std::string& concept, 
                     const std::string& description,
                     const std::vector<std::string>& relatedConcepts = std::vector<std::string>());
    
    // Enhanced retrieval with inference
    std::string retrieveFact(const std::string& key) const override;
    std::string retrieveConcept(const std::string& concept) const;
    std::string inferFromFacts(const std::vector<std::string>& factKeys) const;
    
    // Semantic network operations
    void establishAssociation(const std::string& source, const std::string& target, float strength);
    float getAssociationStrength(const std::string& source, const std::string& target) const;
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const;
    
    // Hierarchy and categories
    void setConceptCategory(const std::string& concept, const std::string& category);
    std::string getConceptCategory(const std::string& concept) const;
    std::vector<std::string> getConceptsInCategory(const std::string& category) const;
    
    // Knowledge graph operations
    void addKnowledgeNode(const std::string& node, const std::string& type);
    void addKnowledgeEdge(const std::string& source, const std::string& target, float weight);
    float getKnowledgeValue(const std::string& node) const;
    
    // Learning and updating
    void updateConceptProficiency(const std::string& concept, float delta);
    void applySemanticClosure(const std::string& concept, float closureRate);
    void mergeSimilarConcepts();  // Merge closely related concepts
    
    // Inference and reasoning
    std::string reasonAbout(const std::string& premise1, const std::string& premise2) const;
    bool isConsistentWithKnowledge(const std::string& fact) const;
    
    // Clear with options
    void clear() override;
    void clearByCategory(const std::string& category);
    void clearLowProficiencyConcepts(float threshold);
    
    // Statistics
    float getSemanticDensity() const;
    float getKnowledgeCoverage() const;
    std::vector<std::string> getFrequentConcepts(size_t count) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural Memory with skill chaining and habit formation
// Implements procedural memory using neural motor patterns with additional features:
// - Skill chaining and sequences
// - Habit formation and reinforcement
// - Motor schema and subroutines
// - Performance optimization
// - Transfer of learning

class AdvancedProceduralMemory : public ProceduralMemory {
public:
    AdvancedProceduralMemory();
    ~AdvancedProceduralMemory() override;
    
    // Learn skill with neural pattern
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) override;
    
    // Learn skill with additional metadata
    void learnSkill(const std::string& name,
                   const std::vector<NeuronId>& pattern,
                   const std::string& category,
                   float initialProficiency = 0.5f);
    
    // Enhanced skill retrieval with activation patterns
    Skill* getSkill(const std::string& name) override;
    Skill* getSkillByPattern(const std::vector<NeuronId>& pattern) const;
    std::vector<Skin*> getSkillsInCategory(const std::string& category) const;
    
    // Skill chaining
    void createSkillSequence(const std::vector<std::string>& skillNames);
    void removeSkillSequence(const std::string& sequenceId);
    std::vector<std::string> getSkillSequence(const std::string& sequenceId) const;
    float getSequenceEfficiency(const std::string& sequenceId) const;
    
    // Habit formation with reinforcement learning
    void reinforceSkill(const std::string& name, float reward, float predictionError);
    void updateSkillWeights(const std::string& name, const std::vector<NeuronId>& newPattern);
    void applySkillTransfer(const std::string& source, const std::string& target, float transferRate);
    
    // Motor schema and subroutines
    void defineMotorSchema(const std::string& schemaName,
                          const std::vector<NeuronId>& motorPattern,
                          const std::vector<std::string>& subskills);
    void adaptSchema(const std::string& schemaName, float adaptationFactor);
    float getSchemaEfficiency(const std::string& schemaName) const;
    
    // Performance optimization
    float evaluateSkillExecution(const std::string& name) const;
    void optimizeSkillExecution(const std::string& name);
    void applyMotorLearning(const std::string& name, float learningRate);
    
    // Skill state management
    void setSkillProficiency(const std::string& name, float proficiency);
    float getSkillProficiency(const std::string& name) const;
    void setSkillFluency(const std::string& name, float fluency);
    float getSkillFluency(const std::string& name) const;
    
    // Clear with options
    void clear() override;
    void clearByCategory(const std::string& category);
    void clearLowProficiencySkills(float threshold);
    void resetSkillWeights();
    
    // Statistics and analysis
    float getSkillEfficiency(const std::string& name) const;
    std::vector<std::string> getEfficientSkills(float minEfficiency) const;
    float getAverageSkillProficiency() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative Memory with complex relationship mapping
// Implements associative memory using connectionist networks with additional features:
// - Multi-hop associations
// - Temporal associations
// - Bidirectional associations
// - Association chaining
// - Contextual modulation

class EnhancedAssociativeMemory : public AssociativeMemory {
public:
    EnhancedAssociativeMemory();
    ~EnhancedAssociativeMemory() override;
    
    // Create association with rich context
    void associate(NeuronId a, NeuronId b, float strength) override;
    
    // Enhanced association with temporal and contextual information
    void associateWithContext(NeuronId a, NeuronId b, float strength,
                             SimulationStep timestamp,
                             const std::string& context = "default",
                             float temporalDecay = 0.01f);
    
    // Multi-hop association
    std::vector<NeuronId> findPaths(NeuronId start, NeuronId target, size_t maxHops = 3) const;
    float getAssociationPathStrength(const std::vector<NeuronId>& path) const;
    
    // Bidirectional associations
    void establishBidirectionalAssociation(NeuronId a, NeuronId b, float strength);
    float getBidirectionalStrength(NeuronId a, NeuronId b) const;
    void reinforceBidirectionalAssociation(NeuronId a, NeuronId b, float reinforcementFactor);
    
    // Temporal associations
    void associateTemporally(NeuronId a, NeuronId b, SimulationStep timeOffset);
    SimulationStep getTemporalAssociation(NeuronId a, NeuronId b) const;
    std::vector<std::pair<NeuronId, NeuronId>> getTemporalAssociations(SimulationStep timeWindow) const;
    
    // Association chaining
    void createAssociationChain(const std::vector<NeuronId>& chain);
    std::vector<NeuronId> extendAssociationChain(NeuronId anchor, const std::vector<NeuronId>& existingChain) const;
    float getChainCohesion(const std::vector<NeuronId>& chain) const;
    
    // Contextual modulation
    void setAssociationContext(NeuronId a, NeuronId b, const std::string& context);
    float getAssociationContextStrength(NeuronId a, NeuronId b, const std::string& context) const;
    std::vector<std::pair<NeuronId, NeuronId>> getAssociationsInContext(const std::string& context) const;
    
    // Update and learning
    void updateAssociation(NeuronId a, NeuronId b, float delta) override;
    void strengthenWeakAssociations();  // Strengthen associations below threshold
    void weakenStrongAssociations();    // Weaken strong associations for balance
    void applyTemporalForgetting();      // Time-based decay of temporal associations
    
    // Clear with options
    void clear() override;
    void clearByContext(const std::string& context);
    void clearWeakAssociations(float strengthThreshold);
    void clearTemporalAssociations(SimulationStep olderThan);
    
    // Statistics and analysis
    float getAssociationDensity() const;
    float getContextual richness() const;
    float getTemporal coherence() const;
    std::vector<NeuronId> getHighDegreeNodes(float degreeThreshold) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory Integration: Connects all memory systems
// Manages interactions and dependencies between different memory types
// Implements memory consolidation, retrieval, and interference management

class MemoryIntegration {
public:
    MemoryIntegration();
    ~MemoryIntegration();
    
    // Get memory system instances
    EnhancedWorkingMemory* getWorkingMemory();
    AdvancedEpisodicMemory* getEpisodicMemory();
    EnhancedSemanticMemory* getSemanticMemory();
    AdvancedProceduralMemory* getProceduralMemory();
    EnhancedAssociativeMemory* getAssociativeMemory();
    
    // Initialize all memory systems
    void initialize(Brain* brain);
    
    // Update all memory systems
    void updateAll(TimestepDuration dt);
    
    // Cross-memory consolidation
    void consolidateWorkingMemoryToEpisodic();  // Move stable working memory to episodic
    void consolidateEpisodicToSemantic();      // Extract concepts from episodes
    void consolidateSemanticToProcedural();   // Convert knowledge to skills
    void consolidateProceduralToWorking();    // Retrieve skills for active use
    
    // Memory retrieval with context
    std::vector<float> retrieveWorkingMemory(const std::string& context) const;
    EpisodicMemoryItem retrieveEpisodicMemory(const std::string& scene) const;
    std::string retrieveSemanticMemory(const std::string& concept) const;
    std::vector<std::string> retrieveProceduralMemory(const std::string& task) const;
    
    // Interference management
    void reduceWorkingMemoryInterference();    // Reduce interference in working memory
    void resolveEpisodicMemoryConflicts();    // Resolve conflicting episodic memories
    void harmonizeSemanticMemoryConflicts();  // Resolve conflicting semantic knowledge
    void balanceProceduralMemoryCompetition(); // Balance competing procedural skills
    
    // Learning integration
    void integrateWorkingMemoryLearning();   // Learn from working memory usage
    void integrateEpisodicMemoryLearning();   // Learn from episodic memory
    void integrateSemanticMemoryLearning();   // Learn from semantic memory
    void integrateProceduralMemoryLearning(); // Learn from procedural memory
    
    // Statistics and monitoring
    float getMemorySystemHealth() const;
    float getMemoryCohesion() const;
    float getLearningReadiness() const;
    std::string getMemoryStatusReport() const;
    
    // Configuration
    void configureMemorySystems(const std::string& configFile);
    void setIntegrationWeights(float workingEpisodic, float episodicSemantic, float semanticProcedural, float proceduralWorking);
    
    // Reset
    void resetAllMemorySystems();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

