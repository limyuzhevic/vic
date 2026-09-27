#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// PLACEHOLDER - Phase 2 will implement real working memory

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
    // TODO PHASE 2: Implement real storage
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    bool contains(NeuronId neuron) const;
    
    // Clear all
    void clear();
    
    // Get capacity
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Decay all items
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// Advanced episodic memory with indexing, replay, and consolidation

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    // Enhanced episodic memory item for biological plausibility
    float novelty;           // How novel this episode is
    float strength;          // Memory strength (0.0-1.0)
    SimulationStep age;     // Age in simulation steps
    float reward;           // Associated reward
    float predictionError;  // Surprise/prediction error
    
    // Episode context
    float positionX;        // World position
    float positionY;        // World position
    float orientation;       // Facing direction
    ActionType action;       // Action taken
    std::vector<float> sensoryState;    // Sensory input pattern
    std::vector<float> resultingSensoryState; // Result of action
    
    // Hebbian trace (synaptic modifications)
    std::vector<std::pair<NeuronId, float>> synapticChanges;
    
    EpisodicMemoryItem() 
        : timestamp(0), novelty(0.0f), strength(0.0f), age(0), reward(0.0f), 
          predictionError(0.0f), positionX(0), positionY(0), orientation(0), 
          action(ActionType::Wait), age(0) {}
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation (move to long-term)
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// Advanced semantic memory with inference, networks, and generalization

struct SemanticNode {
    std::string concept;                // The concept/fact
    std::string representation;         // Vector representation
    std::vector<std::string> relatedConcepts;  // Associated concepts
    std::vector<float> activationPattern;       // Neural activation pattern
    float activation;                    // Current activation level (0.0-1.0)
    float strength;                      // Connection strength (0.0-1.0)
    float decayRate;                     // Decay rate parameter
    size_t accessCount;                  // How often accessed
    SimulationStep lastAccessed;         // Last access time
    SimulationStep lastUpdated;          // Last update time
    
    SemanticNode() : activation(0.0f), strength(0.0f), decayRate(0.01f), 
                     accessCount(0), lastAccessed(0), lastUpdated(0) {}
};

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact with vector representation
    void storeConcept(const std::string& concept, const std::vector<float>& representation);
    
    // Store association between concepts
    void associateConcepts(const std::string& conceptA, const std::string& conceptB, float strength);
    
    // Retrieve concept by its vector representation (approximate search)
    std::string retrieveConcept(const std::vector<float>& query, float threshold = 0.7f) const;
    
    // Retrieve concept by name
    std::string retrieveConcept(const std::string& concept) const;
    
    // Check if concept exists
    bool hasConcept(const std::string& concept) const;
    
    // Get all concepts
    std::vector<SemanticNode> getAllConcepts() const;
    
    // Infer new concepts from existing ones
    std::vector<std::string> inferConcepts(const std::string& concept, size_t maxResults = 5) const;
    
    // Get related concepts
    std::vector<std::string> getRelatedConcepts(const std::string& concept, size_t maxResults = 5) const;
    
    // Update concept activation based on usage
    void activateConcept(const std::string& concept, float boost = 0.1f);
    
    // Decay all concepts based on time
    void decayAllConcepts(float globalDecayRate);
    
    // Consolidate related concepts
    void consolidateConcepts(float similarityThreshold);
    
    // Clear all
    void clear();
    
    // Get semantic network density
    float getSemanticDensity() const;
    
    // Get average concept activation
    float getAverageActivation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// Advanced procedural memory with skill transfer, decomposition, and mastery

struct SkillNode {
    std::string name;                       // Skill name
    std::vector<NeuronId> neuralPattern;     // Base neural pattern
    float proficiency;                       // Current proficiency (0.0-1.0)
    float mastery;                           // Deep mastery level
    float adaptationRate;                    // How quickly skill adapts
    float transferability;                   // How well skill transfers to other domains
    SimulationStep lastPracticed;            // Last practice time
    std::vector<std::string> prerequisiteSkills; // Skills needed to learn this
    std::vector<std::string> derivedSkills;  // Skills that can be derived from this
    float efficiency;                        // How efficiently the skill is executed
    float errorRate;                         // Current error rate
    
    SkillNode() : proficiency(0.0f), mastery(0.0f), adaptationRate(0.05f), 
                  transferability(0.0f), lastPracticed(0), efficiency(0.0f), errorRate(0.0f) {}
};

struct SkillDecomposition {
    std::vector<std::string> subskills;      // Component subskills
    std::vector<std::vector<NeuronId>> subskillPatterns; // Neural patterns for subskills
    float complexity;                        // Overall skill complexity
    float learningCurve;                    // Learning difficulty
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn primitive skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Learn composite skill from components
    void learnCompositeSkill(const std::string& name, const std::vector<std::string>& subskillNames, 
                           const std::vector<NeuronId>& pattern);
    
    // Get skill by name
    SkillNode* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<SkillNode*> getAllSkills();
    
    // Update proficiency through practice
    void practiceSkill(const std::string& name, float delta = 0.1f);
    
    // Transfer skill to new context
    void transferSkill(const std::string& skillName, const std::vector<NeuronId>& newPattern, 
                      float transferStrength = 0.8f);
    
    // Decompose skill into components
    SkillDecomposition decomposeSkill(const std::string& skillName) const;
    
    // Get skills that can be derived from this skill
    std::vector<std::string> getDerivedSkills(const std::string& skillName) const;
    
    // Check if skill is mastered
    bool isMastered(const std::string& skillName, float threshold = 0.9f) const;
    
    // Get average proficiency across all skills
    float getAverageProficiency() const;
    
    // Get total skill complexity
    float getTotalComplexity() const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// Advanced associative memory with chaining, path completion, and rewiring

struct AssociationLink {
    NeuronId source;           // Source neuron
    NeuronId destination;      // Destination neuron
    float strength;            // Connection strength (0.0-1.0)
    float eligibilityTrace;    // TD error eligibility trace
    float lastUpdated;         // Last update time
    float decayRate;           // Hebbian decay rate
    float rewardPrediction;    // Expected reward
    float predictionError;     // Prediction error (TD error)
    
    AssociationLink() : strength(0.0f), eligibilityTrace(0.0f), lastUpdated(0), 
                        decayRate(0.01f), rewardPrediction(0.0f), predictionError(0.0f) {}
};

struct AssociationChain {
    std::vector<NeuronId> chain;      // Ordered sequence of neurons
    float chainStrength;              // Overall chain strength
    float completionProbability;      // How complete the chain is
    float temporalCohesion;           // How temporally bound the chain is
    SimulationStep creationTime;      // When chain was created
    std::vector<float> gapRewards;    // Rewards for gaps in chain
};

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association with Hebbian learning
    void associate(NeuronId a, NeuronId b, float strength = 1.0f);
    
    // Create association from experience (TD learning)
    void associateFromExperience(NeuronId a, NeuronId b, float reward, float predictionError);
    
    // Get direct associations for a neuron
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association with delta rule
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Complete partial cue to form predictions
    std::vector<NeuronId> completePath(const std::vector<NeuronId>& cue) const;
    
    // Spread activation through network
    std::vector<NeuronId> spreadActivation(NeuronId source, size_t steps = 3) const;
    
    // Find association chains containing a neuron
    std::vector<AssociationChain> findChains(NeuronId neuron) const;
    
    // Prune weak associations
    void pruneWeakAssociations(float threshold);
    
    // Rewire based on recent usage patterns
    void rewireBasedOnUsage(float learningRate);
    
    // Update eligibility traces (TD learning)
    void updateEligibilityTraces(float reward, float discountFactor);
    
    // Get number of associations
    size_t getAssociationCount() const;
    
    // Get association statistics
    float getAverageStrength() const;
    float getMaxStrength() const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
