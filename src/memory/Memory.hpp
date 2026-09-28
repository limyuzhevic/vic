#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Working memory: temporary active storage of information
// PLACEHOLDER - Phase 2 will implement real working memory

/**
 * @class WorkingMemory
 * @brief Implements working memory using neural dynamics
 * 
 * The WorkingMemory class provides temporary storage for active information during
 * neural computation. Unlike simple buffer storage, it leverages persistent neural
 * activity and recurrent connections to maintain information across timesteps.
 * 
 * **Memory Mechanisms:**
 * - Persistent neural activity patterns for information maintenance
 * - Recurrent synaptic connections that reinforce stored representations
 * - Activity-dependent short-term plasticity for trace dynamics
 * - Neural competition for selective memory retention and interference
 * 
 * **Key Features:**
 * - Capacity limits based on neural population size
 * - Natural decay mechanisms for information forgetting
 * - Competitive interactions between memory traces
 * - Integration with neural plasticity systems
 * 
 * **Applications:**
 * - Maintaining task goals and rules during execution
 * - Holding intermediate results in reasoning chains
 * - Temporary storage of sensory predictions
 * - Working memory in cognitive architectures
 * 
 * @note This is a placeholder implementation for Phase 2
 * @see NeuralWorkingMemory for neural-based working memory implementation
 * @see EpisodicMemory for long-term episodic memory
 * @see SemanticMemory for knowledge-based memory
 * 
 * @warning Thread safety must be ensured by the caller when accessing memory concurrently
 * @tparam NeuronId Type for neuron identification
 * @tparam SimulationStep Type for time tracking
 */

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store item
    /**
     * @brief Store information in working memory
     * 
     * @param neuron Identifier of the neuron representing this memory item
     * @param value Activation level or content value of the memory item
     * @post Memory item stored with specified neuron and value
     * @pre Neuron must not already have a stored item (single-slot per neuron)
     * @pre Memory capacity must not be exceeded
     * 
     * @note In neural implementations, this would strengthen recurrent connections
     * @see retrieve() to access stored information
     * @see contains() to check if item exists
     */
    void store(NeuronId neuron, float value);
    
    // Retrieve item
    /**
     * @brief Retrieve information from working memory
     * 
     * @param neuron Identifier of the neuron whose memory item to retrieve
     * @return float Value of the stored memory item
     * @throws std::out_of_range if neuron has no stored item
     * @pre Neuron must have stored item in working memory
     * 
     * @note In neural implementations, this would reactivate persistent activity
     * @see store() to store new items
     * @see contains() to check if item exists
     */
    float retrieve(NeuronId neuron) const;
    
    // Check if item exists
    /**
     * @brief Check if working memory contains item for specified neuron
     * 
     * @param neuron Identifier of the neuron to check
     * @return bool true if neuron has a stored item, false otherwise
     * @post Does not modify memory state
     * 
     * @note Enables efficient query before retrieval operations
     * @see store() to add items
     * @see retrieve() to get item values
     */
    bool contains(NeuronId neuron) const;
    
    // Clear all
    /**
     * @brief Clear all items from working memory
     * 
     * @post All memory items removed, working memory becomes empty
     * @see store() to add items back
     * @note Typically called at start of new task or episode
     */
    void clear();
    
    // Get capacity
    /**
     * @brief Get the maximum capacity of working memory
     * 
     * @return size_t Maximum number of items working memory can hold
     * @see getCurrentSize() to get current number of items
     * @see store() to add items
     * @note In neural implementations, this corresponds to available neuron count
     */
    size_t getCapacity() const;
    
    /**
     * @brief Get the current number of items in working memory
     * 
     * @return size_t Current count of stored memory items
     * @see getCapacity() to get maximum capacity
     * @see store() to add items
     * @note Equals number of neurons with stored items
     */
    size_t getCurrentSize() const;
    
    // Decay all items
    /**
     * @brief Apply decay to all items in working memory
     * 
     * @param decayRate Rate of decay per timestep (0.0 to 1.0)
     * @post All item values reduced by decay factor
     * @pre Decay rate must be in valid range [0.0, 1.0]
     * @note Implements natural forgetting mechanisms
     * @see store() to add new items
     * @see getCurrentSize() to check remaining items
     */
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Episodic memory: storage of events and experiences
// PLACEHOLDER - Phase 2 will implement real episodic memory

/**
 * @struct EpisodicMemoryItem
 * @brief Represents a single episodic memory event or experience
 * 
 * An episodic memory item captures a specific moment in time with associated
 * neural activity patterns and contextual information. It enables the system
 * to recall past experiences with temporal context.
 * 
 * **Components:**
 * - Timestamp: When the event occurred
 * - Neural pattern: Which neurons were active
 * - Values: Activation levels of those neurons
 * - Metadata: Additional contextual information
 * 
 * **Use Cases:**
 * - Recording sensory inputs and processing
 * - Storing action outcomes and rewards
 * - Capturing behavioral sequences
 * - Maintaining temporal context for decision making
 * 
 * @note Each item represents a unique moment in time
 * @see EpisodicMemory for managing collections of episodic items
 * @see SimulationStep for timestamp type
 */

struct EpisodicMemoryItem {
    SimulationStep timestamp;               ///< Time when the event occurred
    std::vector<NeuronId> neurons;           ///< Neurons active during the event
    std::vector<float> values;              ///< Activation levels of neurons
    std::string metadata;                   ///< Additional contextual information
    
    /**
     * @brief Default constructor initializes empty episodic memory item
     * 
     * @post timestamp set to 0 (invalid/unset)
     * @note Use storeEpisode() to create meaningful episodic items
     */
    EpisodicMemoryItem() : timestamp(0) {}
};

/**
 * @class EpisodicMemory
 * @brief Implements storage of events and experiences with temporal context
 * 
 * The EpisodicMemory class provides a temporal database of experiences and events,
 * enabling the system to learn from past experiences and maintain a temporal
 * understanding of the environment.
 * 
 * **Memory Organization:**
 * - Linear timeline of events
 * - Each event includes neural activity patterns
 * - Metadata for rich contextual information
 * - Consolidation mechanisms for long-term storage
 * 
 * **Key Features:**
 * - Temporal ordering of events
 * - Replay and reactivation capabilities
 * - Memory consolidation (transfer to long-term)
 * - Integration with semantic memory
 * 
 * **Neural Integration:**
 * - Stores neural activity patterns as memory traces
 * - Learns temporal correlations between events
 * - Supports experience replay during sleep/wake
 * 
 * **Applications:**
 * - Skill learning through practice repetition
 * - Causal reasoning based on event sequences
 * - Predictive coding based on past experience
 * - adaptive behavior based on history
 * 
 * @note This is a placeholder implementation for Phase 2
 * @see SemanticMemory for knowledge-based memory
 * @see WorkingMemory for temporary information storage
 * @see EpisodicMemoryItem for individual memory items
 * 
 * @warning Memory growth must be managed through consolidation
 */

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode
    /**
     * @brief Store a new episodic memory item
     * 
     * @param episode EpisodicMemoryItem to store, including timestamp and neural pattern
     * @post Episode added to memory timeline
     * @pre Episode must have valid timestamp (> 0)
     * @pre Episode must have neural activity data
     * 
     * @note Creates a permanent record of the experience
     * @see retrieveEpisode() to access stored episodes
     * @see getRecentEpisodes() to get recent memories
     */
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    /**
     * @brief Retrieve an episodic memory by index
     * 
     * @param index Index of episode to retrieve (0-based, chronological order)
     * @return EpisodicMemoryItem Copy of the requested episode
     * @throws std::out_of_range if index >= getEpisodeCount()
     * @pre Memory must contain episodes
     * 
     * @note Index 0 corresponds to oldest episode
     * @see storeEpisode() to add episodes
     * @see getRecentEpisodes() to get recent memories
     */
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    /**
     * @brief Get the total number of episodic memories stored
     * 
     * @return size_t Number of episodic memories in the timeline
     * @see storeEpisode() to add memories
     * @see getRecentEpisodes() to get recent memories
     * @see consolidate() to manage memory growth
     */
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    /**
     * @brief Get the most recent episodic memories
     * 
     * @param count Number of recent episodes to retrieve
     * @return std::vector<EpisodicMemoryItem> Vector of recent episodes (newest first)
     * @pre count must be > 0 and <= getEpisodeCount()
     * @post Returned vector contains recent episodes in reverse chronological order
     * 
     * @note Useful for experience replay and recent memory consolidation
     * @see storeEpisode() to add memories
     * @see getEpisodeCount() for total count
     */
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all
    /**
     * @brief Clear all episodic memories
     * 
     * @post Timeline becomes empty, all memories removed
     * @see storeEpisode() to add memories
     * @note Useful for task-specific memory management
     */
    void clear();
    
    // Memory consolidation (move to long-term)
    /**
     * @brief Consolidate episodic memories into long-term storage
     * 
     * @param relevanceThreshold Minimum relevance score for consolidation (0.0 to 1.0)
     * @post Relevant episodes moved to long-term storage, recent timeline updated
     * @pre relevanceThreshold must be in valid range [0.0, 1.0]
     * 
     * @note Reduces memory footprint while preserving important experiences
     * @see getRecentEpisodes() to check before consolidation
     * @see SemanticMemory for knowledge extraction
     */
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Semantic memory: gradually acquired knowledge
// PLACEHOLDER - Phase 2 will implement real semantic memory

/**
 * @class SemanticMemory
 * @brief Implements knowledge-based memory for facts and concepts
 * 
 * The SemanticMemory class stores structured knowledge about the world in the form
 * of facts, concepts, and relationships. It enables symbolic reasoning and
 * knowledge-based inference alongside neural pattern recognition.
 * 
 * **Knowledge Organization:**
 * - Key-value pairs for factual information
 * - Concept hierarchies for organized knowledge
 * - Relationship networks for associative thinking
 * - Inference rules for logical reasoning
 * 
 * **Key Features:**
 * - Persistent knowledge storage across sessions
 * - Efficient retrieval by key or concept
 * - Knowledge integration from episodic memories
 * - Compression and abstraction of patterns
 * 
 * **Knowledge Types:**
 * - Facts: Direct statements about the world
 * - Rules: Logical relationships and constraints
 * - Concepts: Abstract categories and classifications
 * - Procedures: Step-by-step knowledge for skills
 * 
 * **Neural-Symbolic Integration:**
 * - Maps neural patterns to symbolic concepts
 * - Extracts knowledge from neural activity
 * - Provides symbolic guidance for neural processing
 * 
 * @note This is a placeholder implementation for Phase 2
 * @see EpisodicMemory for experience-based knowledge
 * @see WorkingMemory for temporary knowledge use
 * @see AssociativeMemory for relationship storage
 */

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact
    /**
     * @brief Store a factual knowledge entry
     * 
     * @param key Unique identifier for the fact
     * @param value The factual content or value
     * @post Fact stored in knowledge base
     * @pre Key must be unique (overwrites existing if duplicate)
     * @pre Key and value should be meaningful and well-formed
     * 
     * @note Keys can be concepts, fact identifiers, or property names
     * @see retrieveFact() to access stored facts
     * @see hasFact() to check if fact exists
     */
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    /**
     * @brief Retrieve a factual knowledge entry
     * 
     * @param key Unique identifier of the fact to retrieve
     * @return std::string Value of the stored fact
     * @throws std::out_of_range if key not found
     * @pre Key must exist in knowledge base
     * 
     * @note Returns empty string if key not found
     * @see storeFact() to add facts
     * @see hasFact() to check existence
     */
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    /**
     * @brief Check if semantic memory contains a specific fact
     * 
     * @param key Unique identifier of the fact to check
     * @return bool true if fact exists, false otherwise
     * @post Does not modify memory state
     * 
     * @note Efficient lookup for decision making
     * @see storeFact() to add facts
     * @see retrieveFact() to get fact values
     */
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    /**
     * @brief Get all knowledge facts as key-value pairs
     * 
     * @return std::vector<std::pair<std::string, std::string>> All stored facts
     * @post Returns copy of entire knowledge base
     * @see storeFact() to add facts
     * @see hasFact() for individual fact checks
     */
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    /**
     * @brief Clear all semantic knowledge
     * 
     * @post Knowledge base becomes empty
     * @see storeFact() to add knowledge
     * @note Useful for task-specific knowledge management
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// PLACEHOLDER - Phase 2 will implement real procedural memory

/**
 * @struct Skill
 * @brief Represents a learned skill or behavioral pattern
 * 
 * A skill is a collection of coordinated neural patterns that produce a specific
 * behavior or sequence of actions. Skills are stored as procedural knowledge
 * that can be executed automatically.
 * 
 * **Skill Components:**
 * - Name: Unique identifier for the skill
 * - Neural pattern: Network activity that produces the skill
 * - Proficiency: How well the skill is executed (0.0 to 1.0)
 * - History: Past execution performance and outcomes
 * 
 * **Skill Characteristics:**
 * - Strength: Proficiency level for automaticity
 * - Flexibility: Ability to adapt to different contexts
 * - Speed: Time to execution completion
 * - Accuracy: Success rate of execution
 * 
 * **Learning Process:**
 * - Repetition of neural patterns
 * - Reinforcement based on outcomes
 * - Refinement of coordination
 * - Transfer to different contexts
 * 
 * @note Skills represent learned behaviors that become automated
 * @see ProceduralMemory for skill management
 * @see Skill::neuralPattern for neural implementation
 */

struct Skill {
    std::string name;                    ///< Unique name/identifier for the skill
    std::vector<NeuronId> neuralPattern;  ///< Neural activity pattern that produces the skill
    float proficiency;                    ///< Current skill proficiency (0.0 to 1.0)
    
    /**
     * @brief Default constructor initializes empty skill
     * 
     * @post proficiency set to 0.0 (not learned)
     * @note Use learnSkill() to create meaningful skills
     */
    Skill() : proficiency(0.0f) {}
};

/**
 * @class ProceduralMemory
 * @brief Implements storage of learned skills and behavioral patterns
 * 
 * The ProceduralMemory class stores skills and habits as procedural knowledge,
 * enabling automatic execution of learned behaviors. It bridges the gap between
 * symbolic planning and reactive action execution.
 * 
 * **Skill Management:**
 * - Skill acquisition through repetition
 * - Performance monitoring and feedback
 * - Skill transfer and generalization
 * - Competition between skills for selection
 * 
 * **Execution Support:**
 * - Skill selection based on context
 * - Parameter modulation for varying execution
 * * Error correction and refinement
 * - Contextual adaptation
 * 
 * **Learning Mechanisms:**
 * - Reinforcement learning from outcomes
 * - Hebbian learning for neural pattern strengthening
 * - Memory consolidation for skill retention
 * - Adaptability for new contexts
 * 
 * **Neural Integration:**
 * - Maps procedural skills to neural patterns
 * - Supports skill acquisition through plasticity
 * - Enables automatic skill execution
 * 
 * @note This is a placeholder implementation for Phase 2
 * @see Skill for individual skill structure
 * @see SemanticMemory for declarative knowledge
 * @see WorkingMemory for skill planning
 */

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill
    /**
     * @brief Learn a new procedural skill
     * 
     * @param name Unique name for the skill
     * @param pattern Neural activity pattern that produces the skill
     * @post Skill created and added to procedural memory
     * @pre Name must be unique (overwrites existing if duplicate)
     * @pre Pattern must be a valid neural activity pattern
     * 
     * @note Initial proficiency is 0.0 (not yet mastered)
     * @see ProceduralMemory::getSkill() to retrieve skills
     * @see updateProficiency() to improve skill
     */
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    /**
     * @brief Get a procedural skill by name
     * 
     * @param name Name of the skill to retrieve
     * @return Skill* Pointer to skill object (caller owns lifetime)
     * @throws std::out_of_range if skill name not found
     * @pre Skill must have been learned
     * 
     * @note Returns pointer to skill in internal storage
     * @see learnSkill() to add skills
     * @see getAllSkills() to get all skills
     */
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    /**
     * @brief Get all procedural skills
     * 
     * @return std::vector<Skill*> Vector of skill pointers (caller owns lifetime)
     * @post Returns pointers to skills in internal storage
     * @see learnSkill() to add skills
     * @see getSkill() for individual skill retrieval
     */
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    /**
     * @brief Update the proficiency level of a learned skill
     * 
     * @param name Name of the skill to update
     * @param delta Change in proficiency (can be positive or negative)
     * @post Skill proficiency updated
     * @pre Skill must exist in procedural memory
     * @pre Resulting proficiency should be clamped to [0.0, 1.0]
     * 
     * @note Proficiency reflects skill mastery and automaticity
     * @see learnSkill() to create skills
     * @see getSkill() to retrieve skill proficiency
     */
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    /**
     * @brief Clear all procedural skills
     * 
     * @post All skills removed, procedural memory becomes empty
     * @see learnSkill() to add skills
     * @note Useful for skill-specific memory management
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

/**
 * @class AssociativeMemory
 * @brief Implements storage of relationships and associations between concepts
 * 
 * The AssociativeMemory class stores connections between different neural representations,
 * enabling the system to make inferences and predictions based on learned relationships.
 * It implements the core mechanism for pattern completion and analogical reasoning.
 * 
 * **Association Types:**
 * - Direct associations: Neuron A → Neuron B with strength
 * - Bidirectional associations: Mutual relationships
 * - Conditional associations: A → B if C
 * - Negative associations: A inhibits B
 * 
 * **Computational Benefits:**
 * - Pattern completion from partial cues
 * - Categorical inference and generalization
 * - Memory spreading activation
 * - Knowledge structuring and organization
 * 
 * **Neural Implementation:**
 * - Synaptic weights as association strengths
 * - Recurrent connections for pattern completion
 * - Competitive dynamics for association selection
 * - Plasticity rules for association learning
 * 
 * **Applications:**
 * - Language understanding and semantic relations
 * - Concept formation and categorization
 * - Problem solving through associative reasoning
 * - Creativity through novel associations
 * 
 * @note This is a placeholder implementation for Phase 2
 * @see SemanticMemory for categorical knowledge
 * @see EpisodicMemory for temporal associations
 * @see WorkingMemory for active associations
 */

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association
    /**
     * @brief Create an association between two neural representations
     * 
     * @param a First neuron identifier (source or cue)
     * @param b Second neuron identifier (target or response)
     * @param strength Initial association strength (-1.0 to 1.0)
     * @post Association created and stored
     * @pre Neurons a and b must be valid neuron IDs
     * @pre Strength should be within valid range
     * 
     * @note Negative strength indicates inhibitory relationship
     * @see getAssociationStrength() to retrieve strength
     * @see getAssociations() to get all associations for a neuron
     */
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    /**
     * @brief Get all neurons associated with a given neuron
     * 
     * @param neuron Neuron identifier to query
     * @return std::vector<NeuronId> Vector of associated neuron IDs
     * @post Returns copy of association list
     * @pre Neuron must have associations
     * 
     * @note Enables pattern completion from partial cues
     * @see associate() to create associations
     * @see getAssociationStrength() for specific relationships
     */
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    /**
     * @brief Get the strength of association between two neurons
     * 
     * @param a First neuron identifier
     * @param b Second neuron identifier
     * @return float Association strength (-1.0 to 1.0)
     * @throws std::out_of_range if association not found
     * @pre Both neurons must have an association
     * 
     * @note Positive values indicate excitatory relationships
     * @note Negative values indicate inhibitory relationships
     * @see associate() to create associations
     * @see getAssociations() to get all associations
     */
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    /**
     * @brief Update the strength of an existing association
     * 
     * @param a First neuron identifier
     * @param b Second neuron identifier
     * @param delta Change in association strength
     * @post Association strength updated
     * @pre Association must exist
     * @pre Resulting strength should be clamped to [-1.0, 1.0]
     * 
     * @note Enables learning and adaptation of relationships
     * @see associate() to create new associations
     * @see getAssociationStrength() to check current strength
     */
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    /**
     * @brief Clear all associations
     * 
     * @post All associations removed, associative memory becomes empty
     * @see associate() to add associations
     * @note Useful for association-specific memory management
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

// Episodic memory: storage of events and experiences
// PLACEHOLDER - Phase 2 will implement real episodic memory

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> values;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
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
// PLACEHOLDER - Phase 2 will implement real semantic memory

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Store fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Procedural memory: learned skills and habits
// PLACEHOLDER - Phase 2 will implement real procedural memory

struct Skill {
    std::string name;
    std::vector<NeuronId> neuralPattern;
    float proficiency;
    
    Skill() : proficiency(0.0f) {}
};

class ProceduralMemory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
    // Learn skill
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    // Retrieve skill
    Skill* getSkill(const std::string& name);
    
    // Get all skills
    std::vector<Skill*> getAllSkills();
    
    // Update proficiency
    void updateProficiency(const std::string& name, float delta);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Associative memory: relationships between representations
// PLACEHOLDER - Phase 2 will implement real associative memory

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    // Create association
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
