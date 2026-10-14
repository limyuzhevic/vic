#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>

namespace nlm {

// Forward declarations for memory system components
class WorkingMemory;
class EpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;

// Memory system configuration
struct MemoryConfig {
    // Working memory configuration
    size_t workingMemoryCapacity = 200;
    float workingMemoryDecayRate = 0.95f;
    
    // Episodic memory configuration
    size_t episodicMaxEpisodes = 5000;
    bool episodicReplayEnabled = true;
    float episodicConsolidationThreshold = 0.7f;
    
    // Semantic memory configuration
    size_t semanticMaxFacts = 10000;
    bool semanticAutoCategorize = true;
    
    // Procedural memory configuration
    size_t proceduralMaxSkills = 1000;
    float skillLearningRate = 0.01f;
    
    // Associative memory configuration
    size_t associativeMaxAssociations = 10000;
    bool associativeHebbianEnabled = true;
    float associativeDecayRate = 0.99f;
    
    // Integration settings
    bool enableMemoryReplay = true;
    bool enableCrossMemoryAssociations = true;
    float memoryIntegrationStrength = 1.0f;
    
    // Development settings
    size_t criticalMemoryGrowthPeriod = 100;
    float developmentMemoryPlasticity = 1.0f;
};

// Core memory interface
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    
    virtual void store(const std::string& key, const std::vector<float>& value,
                      const std::unordered_map<std::string, std::string>& metadata = {}) = 0;
    
    virtual std::vector<float> retrieve(const std::string& key) const = 0;
    
    virtual bool exists(const std::string& key) const = 0;
    
    virtual void clear() = 0;
    
    virtual size_t size() const = 0;
    
    virtual void update(float deltaTime) = 0;
    
    virtual std::string getTypeName() const = 0;
};

// Working memory: temporary active storage with neural integration
class WorkingMemory : public IMemorySystem {
public:
    WorkingMemory(const MemoryConfig& config);
    ~WorkingMemory() override;
    
    // IMemorySystem interface
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata) override;
    
    std::vector<float> retrieve(const std::string& key) const override;
    
    bool exists(const std::string& key) const override;
    
    void clear() override;
    
    size_t size() const override;
    
    void update(float deltaTime) override;
    
    std::string getTypeName() const override { return "WorkingMemory"; }
    
    // Working memory specific methods
    void storeActivation(const NeuronId& neuron, float activation);
    float getActivation(const NeuronId& neuron) const;
    bool hasNeuron(const NeuronId& neuron) const;
    void clearNeuron(const NeuronId& neuron);
    
    // Competition and normalization
    void applyCompetition();
    void applyNormalization();
    
    // Capacity management
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    float getLoadPercentage() const;
    
    // Decay and plasticity
    void decay(float decayRate);
    void applyPlasticity(const std::vector<float>& inputPattern, float strength);
    
    // Integration with episodic memory
    std::string getEpisodicMemoryKey(const NeuronId& neuron) const;
    void storeToEpisodicMemory(const NeuronId& neuron, float value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory: event storage and replay with neural patterns
struct MemoryEpisode {
    SimulationStep timestamp;
    SimulationStep replayTime;
    std::vector<NeuronId> neuronPattern;
    std::vector<float> activationPattern;
    std::vector<std::string> sensoryInputs;
    std::vector<std::string> actionsTaken;
    std::vector<float> rewardsReceived;
    float relevanceScore;
    std::string contextSignature;
    
    MemoryEpisode()
        : timestamp(0), replayTime(0), relevanceScore(0.0f) {}
    
    MemoryEpisode(SimulationStep ts, const std::vector<NeuronId>& neurons,
                  const std::vector<float>& activations,
                  const std::vector<std::string>& inputs,
                  const std::vector<std::string>& actions,
                  const std::vector<float>& rewards,
                  const std::string& context)
        : timestamp(ts), replayTime(ts), neuronPattern(neurons),
          activationPattern(activations), sensoryInputs(inputs),
          actionsTaken(actions), rewardsReceived(rewards),
          relevanceScore(calculateRelevance(inputs, actions)),
          contextSignature(context) {}
    
    static float calculateRelevance(const std::vector<std::string>& inputs,
                                   const std::vector<std::string>& actions) {
        float inputScore = inputs.empty() ? 0.0f : 1.0f;
        float actionScore = actions.empty() ? 0.0f : 1.0f;
        return (inputScore + actionScore) * 0.5f;
    }
};

class EpisodicMemory : public IMemorySystem {
public:
    EpisodicMemory(const MemoryConfig& config);
    ~EpisodicMemory() override;
    
    // IMemorySystem interface
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata) override;
    
    std::vector<float> retrieve(const std::string& key) const override;
    
    bool exists(const std::string& key) const override;
    
    void clear() override;
    
    size_t size() const override;
    
    void update(float deltaTime) override;
    
    std::string getTypeName() const override { return "EpisodicMemory"; }
    
    // Episodic memory specific methods
    void storeEpisode(const MemoryEpisode& episode);
    MemoryEpisode retrieveEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    
    // Replay functionality
    std::vector<MemoryEpisode> getRecentEpisodes(size_t count) const;
    std::vector<MemoryEpisode> getReplayEpisodes(size_t count, float minRelevance = 0.5f);
    void replayEpisode(size_t index, std::vector<float>& outPattern) const;
    
    // Integration with working memory
    void integrateWithWorkingMemory(const WorkingMemory& workingMem);
    std::vector<NeuronId> getReplicatedNeurons() const;
    
    // Consolidation and pruning
    void consolidateEpisodes(float threshold);
    void pruneOldEpisodes(size_t maxCount);
    
    // Pattern completion
    std::vector<float> completePattern(const std::vector<float>& partialPattern) const;
    float getPatternSimilarity(const std::vector<float>& pattern1,
                              const std::vector<float>& pattern2) const;
    
    // Relevance and strength calculation
    float getEpisodeRelevance(size_t index) const;
    void updateEpisodeRelevance(size_t index, float delta);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Semantic memory: abstract knowledge representation
class SemanticMemory : public IMemorySystem {
public:
    SemanticMemory(const MemoryConfig& config);
    ~SemanticMemory() override;
    
    // IMemorySystem interface
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata) override;
    
    std::vector<float> retrieve(const std::string& key) const override;
    
    bool exists(const std::string& key) const override;
    
    void clear() override;
    
    size_t size() const override;
    
    void update(float deltaTime) override;
    
    std::string getTypeName() const override { return "SemanticMemory"; }
    
    // Semantic memory specific methods
    void storeConcept(const std::string& concept, const std::vector<std::string>& instances,
                     const std::vector<float>& activationPattern);
    
    std::vector<std::string> retrieveConcept(const std::string& concept) const;
    std::vector<float> getConceptActivation(const std::string& concept) const;
    
    // Category and hierarchy management
    void createCategory(const std::string& parent, const std::string& child);
    std::vector<std::string> getSubcategories(const std::string& parent) const;
    std::vector<std::string> getSupercategories(const std::string& child) const;
    
    // Concept relationships
    void establishRelationship(const std::string& concept1, const std::string& concept2,
                              const std::string& relationshipType, float strength);
    
    std::vector<std::pair<std::string, float>> getRelatedConcepts(const std::string& concept) const;
    
    // Activation spreading
    void spreadActivation(const std::string& concept, float amount);
    std::vector<std::pair<std::string, float>> getAllActiveConcepts() const;
    
    // Knowledge inference
    std::vector<std::string> inferFromConcepts(const std::vector<std::string>& inputConcepts) const;
    std::string getConceptExplanation(const std::string& concept) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Procedural memory: skills and behavioral patterns
struct LearnedSkill {
    std::string skillName;
    std::vector<NeuronId> neuralPattern;
    std::vector<std::string> requiredStates;
    float proficiency;
    float executionThreshold;
    SimulationStep lastExecuted;
    size_t executionCount;
    float successRate;
    
    LearnedSkill(const std::string& name, const std::vector<NeuronId>& pattern,
                 const std::vector<std::string>& states,
                 float threshold = 0.5f)
        : skillName(name), neuralPattern(pattern), requiredStates(states),
          proficiency(0.0f), executionThreshold(threshold),
          lastExecuted(0), executionCount(0), successRate(0.0f) {}
    
    bool canExecute(const std::vector<std::string>& currentStates) const;
    bool tryExecute(const std::vector<std::string>& currentStates, bool success);
    float getExecutionProbability() const;
};

class ProceduralMemory : public IMemorySystem {
public:
    ProceduralMemory(const MemoryConfig& config);
    ~ProceduralMemory() override;
    
    // IMemorySystem interface
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata) override;
    
    std::vector<float> retrieve(const std::string& key) const override;
    
    bool exists(const std::string& key) const override;
    
    void clear() override;
    
    size_t size() const override;
    
    void update(float deltaTime) override;
    
    std::string getTypeName() const override { return "ProceduralMemory"; }
    
    // Procedural memory specific methods
    void learnSkill(const std::string& skillName,
                   const std::vector<NeuronId>& neuralPattern,
                   const std::vector<std::string>& requiredStates);
    
    LearnedSkill* getSkill(const std::string& skillName);
    std::vector<LearnedSkill*> getAllSkills() const;
    
    // Skill execution
    bool executeSkill(const std::string& skillName, const std::vector<std::string>& currentStates,
                     std::vector<std::string>& resultingStates);
    
    // Proficiency and learning
    void updateProficiency(const std::string& skillName, float delta, bool success);
    void practiceSkill(const std::string& skillName, const std::vector<std::string>& currentStates);
    
    // Skill transfer
    void transferSkill(const std::string& sourceSkill, const std::string& targetSkill,
                      float transferRate);
    
    // Pattern matching
    std::vector<std::string> findMatchingSkills(const std::vector<std::string>& currentStates) const;
    float getSkillReadiness(const std::string& skillName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Associative memory: neural pattern associations
struct NeuralAssociation {
    NeuronId sourceNeuron;
    NeuronId targetNeuron;
    float strength;
    SimulationStep lastActivation;
    std::vector<float> contextVector;
    
    NeuralAssociation(NeuronId src, NeuronId tgt, float str = 1.0f)
        : sourceNeuron(src), targetNeuron(tgt), strength(str),
          lastActivation(0), contextVector(5, 0.0f) {}
};

class AssociativeMemory : public IMemorySystem {
public:
    AssociativeMemory(const MemoryConfig& config);
    ~AssociativeMemory() override;
    
    // IMemorySystem interface
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata) override;
    
    std::vector<float> retrieve(const std::string& key) const override;
    
    bool exists(const std::string& key) const override;
    
    void clear() override;
    
    size_t size() const override;
    
    void update(float deltaTime) override;
    
    std::string getTypeName() const override { return "AssociativeMemory"; }
    
    // Associative memory specific methods
    void createAssociation(NeuronId source, NeuronId target, float strength = 1.0f);
    void strengthenAssociation(NeuronId source, NeuronId target, float delta);
    void weakenAssociation(NeuronId source, NeuronId target, float delta);
    
    std::vector<NeuronId> getAssociatedNeurons(NeuronId neuron) const;
    float getAssociationStrength(NeuronId source, NeuronId target) const;
    
    // Hebbian learning
    void hebbianLearning(const std::vector<NeuronId>& activeNeurons, float learningRate = 0.01f);
    void antiHebbianLearning(const std::vector<NeuronId>& activeNeurons, float learningRate = 0.01f);
    
    // Context-dependent associations
    void setContextForAssociation(NeuronId source, NeuronId target,
                                  const std::vector<float>& context);
    
    std::vector<NeuronId> getAssociatedNeuronsInContext(NeuronId neuron,
                                                      const std::vector<float>& context) const;
    
    // Association management
    void decayAssociations(float decayRate);
    void pruneWeakAssociations(float threshold);
    size_t getAssociationCount() const;
    
    // Integration with working memory
    void updateFromWorkingMemory(const WorkingMemory& workingMem);
    std::vector<std::pair<NeuronId, NeuronId>> getStrongAssociations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Unified memory system manager
class UnifiedMemorySystem {
public:
    UnifiedMemorySystem(const MemoryConfig& config);
    ~UnifiedMemorySystem();
    
    // Memory system access
    WorkingMemory* getWorkingMemory();
    EpisodicMemory* getEpisodicMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    AssociativeMemory* getAssociativeMemory();
    
    // IMemorySystem interface methods
    void store(const std::string& key, const std::vector<float>& value,
              const std::unordered_map<std::string, std::string>& metadata = {});
    
    std::vector<float> retrieve(const std::string& key) const;
    
    bool exists(const std::string& key) const;
    
    void clearAll();
    
    size_t getTotalMemorySize() const;
    std::vector<std::string> getAllMemoryTypes() const;
    
    // Integration and coordination
    void integrateAllSystems();
    void updateAll(float deltaTime);
    
    // Replay and consolidation
    void initiateSystemReplay();
    void consolidateAll(float threshold);
    
    // Configuration
    void updateConfig(const MemoryConfig& newConfig);
    MemoryConfig getCurrentConfig() const;
    
    // Statistics and monitoring
    std::string getMemoryStatusReport() const;
    float getSystemIntegrationScore() const;
    
    // Development and plasticity
    void applyDevelopmentChanges(float developmentalStage);
    void adjustPlasticityRates(float plasticityFactor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
