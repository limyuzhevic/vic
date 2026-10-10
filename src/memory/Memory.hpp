#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

/**
 * @file Memory.hpp
 * 
 * Memory systems implementation for NLM
 * 
 * This file provides the complete memory architecture for the NLM system,
 * implementing multiple memory systems that work together to store, retrieve,
 * and process information in the brain. The memory systems are essential for
 * learning, perception, decision-making, and behavioral consistency.
 * 
 * The memory architecture in NLM consists of:
 * - Working Memory: Temporary active storage of information
 * - Episodic Memory: Storage of events and experiences
 * - Semantic Memory: Gradually acquired knowledge and facts
 * - Procedural Memory: Learned skills and habits
 * - Associative Memory: Relationships between representations
 * 
 * Key features:
 * - Multiple memory systems with distinct functions
 * - Integration with neural activity and computation
 * - Memory consolidation and retrieval mechanisms
 * - Pattern completion and association capabilities
 * - Learning and adaptation through memory
 * - Real-time access to stored information
 * - Memory capacity and usage management
 * 
 * @note Critical for learning, behavior, and cognitive function
 * @note Integrates with all neural systems
 * @note Essential for memory-based computation
 * @see Config.hpp - Configuration options for memory systems
 * @see Brain.hpp - Integration with brain architecture
 * @see DevelopmentSystem.hpp - Memory development
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class WorkingMemory
 * 
 * Working memory: temporary active storage of information.
 * 
 * Working memory is a core component of the NLM architecture that provides
 * temporary storage of information for immediate use in neural computations.
 * It is the most active and accessible memory system, holding information
 * that is currently being processed, used, or relevant to current tasks.
 * 
 * Working memory serves several critical functions:
 * - Maintaining information for short-term use
 * - Supporting ongoing cognitive processes
 * - Facilitating decision-making and action planning
 * - Buffering sensory and internal representations
 * - Enabling working memory updates and modifications
 * 
 * Key characteristics of working memory in NLM:
 * - **Temporary**: Information is stored for limited time periods
 * - **Active**: Information is actively maintained and updated
 * - **Limited capacity**: Finite storage capacity
 * - **Dynamic**: Content can change rapidly based on input
 * - **Integrated**: Tightly coupled with neural activity
 * 
 * Working memory mechanisms include:
 * - Storage and retrieval of neural representations
 * - Decay and interference processes
 * - Competitive dynamics between memory traces
 * - Rehearsal and strengthening mechanisms
 * - Attentional selection and filtering
 * 
 * @note Essential for real-time cognitive processing
 * @note Used extensively by all brain regions
 * @note Critical for decision-making and action
 * @see Brain.hpp - For working memory access
 * @see NeuralWorkingMemory.hpp - For neural implementation
 * @see EpisodicMemory - For long-term storage
 * 
 * @author NLM Development Team
 */
class WorkingMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new working memory instance with default parameters.
     * The working memory is initialized with default capacity and
     * decay characteristics.
     * 
     * @post Working memory created with default settings
     * @post Ready for storage and retrieval operations
     * 
     * @see WorkingMemory(size_t capacity) - Alternative constructor
     * @see decay() - For setting decay characteristics
     */
    WorkingMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up working memory resources and performs any necessary
     * cleanup operations. The working memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Working memory cannot be used after destruction
     */
    ~WorkingMemory();
    
    // Disable copying - PImpl idiom
    WorkingMemory(const WorkingMemory&) = delete;
    WorkingMemory& operator=(const WorkingMemory&) = delete;
    WorkingMemory(WorkingMemory&&) noexcept;
    WorkingMemory& operator=(WorkingMemory&&) noexcept;
    
    /**
     * @brief Store information in working memory
     * 
     * Stores information in working memory, associating it with a specific
     * neuron. This enables persistent activity and maintenance of neural
     * representations. Working memory information is used for:
     * - Maintaining task-relevant information
     * - Supporting decision-making
     * - Enabling cognitive operations
     * - Facilitating working memory updates
     * 
     * @param neuron Neuron ID to associate with stored information
     * @param value Information value to store (e.g., activation level)
     * 
     * @pre neuron must be a valid neuron ID
     * @post Information stored in working memory
     * @post Neuron representation maintained
     * @post Working memory capacity respected
     * 
     * @note Information can be retrieved later via retrieve()
     * @see retrieve() - For retrieving stored information
     * @see contains() - For checking stored information
     * @see clear() - For clearing all information
     */
    void store(NeuronId neuron, float value);
    
    /**
     * @brief Retrieve information from working memory
     * 
     * Retrieves information stored in working memory for the specified
     * neuron. This is essential for:
     * - Accessing current neural representations
     * - Supporting ongoing cognitive processes
     * - Implementing decision-making
     * - Maintaining working memory contents
     * 
     * @param neuron Neuron ID to retrieve information for
     * @return Stored information value for the neuron
     * 
     * @pre neuron must be a valid neuron ID
     * @pre neuron must contain stored information
     * @post Information retrieved from working memory
     * @post Original value preserved
     * 
     * @return Retrieved information value
     * @retval value Stored information for neuron
     * @retval undefined If neuron has no stored information
     * 
     * @note Returns uninitialized value if neuron not found
     * @see store() - For storing information
     * @see contains() - For checking stored information
     */
    float retrieve(NeuronId neuron) const;
    
    /**
     * @brief Check if information is stored in working memory
     * 
     * Determines whether information is currently stored in working memory
     * for the specified neuron. This is useful for:
     * - Checking memory contents
     * - Implementing memory-dependent operations
     * - Managing working memory access
     * - Error handling for missing information
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron has stored information, false otherwise
     * 
     * @pre neuron must be a valid neuron ID
     * @post Return value indicates memory status
     * @complexity O(1) for lookup
     * 
     * @see store() - For storing information
     * @see retrieve() - For retrieving information
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Clear all information from working memory
     * 
     * Clears all information from working memory, removing all stored
     * representations. This is useful for:
     * - Resetting working memory
     * - Implementing memory-dependent tasks
     * - Managing working memory state
     * - Error recovery
     * 
     * @post All working memory cleared
     * @post Working memory ready for new operations
     * @post All neurons cleared of stored information
     * 
     * @see store() - For storing information
     * @see retrieve() - For retrieving information
     */
    void clear();
    
    /**
     * @brief Get working memory capacity
     * 
     * Returns the maximum capacity of working memory in terms of number
     * of stored items. This is a key statistic for understanding memory
     * limitations and constraints.
     * 
     * @return Maximum working memory capacity
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @note Capacity affects working memory dynamics
     * @see getCurrentSize() - For current usage
     */
    size_t getCapacity() const;
    
    /**
     * @brief Get current working memory usage
     * 
     * Returns the current number of items stored in working memory.
     * This is an important statistic for understanding memory load and
     * utilization.
     * 
     * @return Current number of stored items
     * 
     * @post Return value fluctuates during operation
     * @complexity O(1)
     * 
     * @note Current usage vs. capacity affects dynamics
     * @see getCapacity() - For maximum capacity
     */
    size_t getCurrentSize() const;
    
    /**
     * @brief Decay all information in working memory
     * 
     * Applies decay to all information stored in working memory, reducing
     * the strength of all stored representations. This is essential for:
     * - Implementing forgetting mechanisms
     * - Maintaining memory dynamics
     * - Preventing memory overload
     * - Modeling real-world memory decay
     * 
     * @param decayRate Decay factor (0.0 to 1.0)
     * 
     * @pre 0.0 <= decayRate <= 1.0
     * @post All stored information decayed
     * @post Memory traces weakened
     * 
     * @note Decay affects memory stability and dynamics
     * @see store() - For storing information
     * @see decayRate - For decay rate parameters
     */
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @struct EpisodicMemoryItem
 * 
 * Complete episodic memory item containing event and experience information.
 * 
 * This structure encapsulates a complete episodic memory item, representing
 * a single event or experience in the NLM system. Episodic memories are
 * essential for:
 * - Storing personal experiences and events
 * - Maintaining temporal order of events
 * - Supporting memory-based reasoning
 * - Enabling episodic recall and reconstruction
 * - Modeling temporal sequences of events
 * 
 * The episodic memory item includes:
 * - Temporal information (when the event occurred)
 * - Neural representation (which neurons were involved)
 * - Activation levels (how active each neuron was)
 * - Contextual metadata (additional information about the event)
 * 
 * @note Used by EpisodicMemory for event storage
 * @see EpisodicMemory - For episodic memory management
 * @see NeuralEpisodicMemory.hpp - For neural implementation
 * @see SemanticMemory - For knowledge storage
 * 
 * @author NLM Development Team
 */
struct EpisodicMemoryItem {
    /** Timestamp when the event occurred */
    SimulationStep timestamp;
    
    /** Neurons involved in the event */
    std::vector<NeuronId> neurons;
    
    /** Activation levels of involved neurons */
    std::vector<float> values;
    
    /** Additional metadata about the event */
    std::string metadata;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty episodic memory item with default values.
     * This represents an uninitialized or empty memory entry.
     * 
     * @post Item created with empty data
     * @post Timestamp set to 0
     * @post All other fields empty
     */
    EpisodicMemoryItem() : timestamp(0) {}
};

/**
 * @class EpisodicMemory
 * 
 * Episodic memory: storage of events and experiences.
 * 
 * Episodic memory is a core memory system in NLM that provides long-term
 * storage of events, experiences, and personal history. It is essential for:
 * - Storing what happened and when
 * - Maintaining temporal sequences of events
 * - Supporting memory-based reasoning
 * - Enabling episodic recall and reconstruction
 * - Modeling personal history and experiences
 * 
 * Episodic memory mechanisms include:
 * - Event storage and retrieval
 * - Temporal ordering and sequencing
 * - Memory consolidation and integration
 * - Pattern completion and completion
 * - Temporal navigation and reconstruction
 * 
 * Key characteristics of episodic memory in NLM:
 * - **Chronological**: Stores events in temporal order
 * - **Detailed**: Contains rich contextual information
 * - **Personal**: Represents individual experiences
 * - **Dynamic**: Can be updated and modified
 * - **Integrated**: Connects with other memory systems
 * 
 * @note Essential for temporal processing and experience
 * @note Used by Brain for episodic memory access
 * @note Critical for memory-based learning
 * @see WorkingMemory - For temporary storage
 * @see SemanticMemory - For knowledge storage
 * @see NeuralEpisodicMemory.hpp - For neural implementation
 * 
 * @author NLM Development Team
 */
class EpisodicMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new episodic memory instance with default parameters.
     * The episodic memory is initialized with empty storage and ready
     * for storing and retrieving events.
     * 
     * @post Episodic memory created with default settings
     * @post Ready for event storage and retrieval
     * 
     * @see EpisodicMemory(size_t capacity) - Alternative constructor
     * @see storeEpisode() - For storing episodes
     */
    EpisodicMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up episodic memory resources and performs any necessary
     * cleanup operations. The episodic memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Episodic memory cannot be used after destruction
     */
    ~EpisodicMemory();
    
    // Disable copying - PImpl idiom
    EpisodicMemory(const EpisodicMemory&) = delete;
    EpisodicMemory& operator=(const EpisodicMemory&) = delete;
    EpisodicMemory(EpisodicMemory&&) noexcept;
    EpisodicMemory& operator=(EpisodicMemory&&) noexcept;
    
    /**
     * @brief Store an episode
     * 
     * Stores an episode in episodic memory, associating it with a specific
     * timestamp and neural activity pattern. This is essential for:
     * - Recording personal experiences and events
     * - Maintaining temporal sequences of events
     * - Supporting episodic memory
     * - Enabling temporal navigation
     * - Storing contextual information
     * 
     * @param episode Complete episodic memory item to store
     * 
     * @pre episode must be a valid EpisodicMemoryItem
     * @post Episode stored in episodic memory
     * @post Temporal order maintained
     * @post Neural activity recorded
     * 
     * @note Episodes are stored in chronological order
     * @see retrieveEpisode() - For retrieving episodes
     * @see getEpisodeCount() - For counting episodes
     * @see getRecentEpisodes() - For retrieving recent episodes
     */
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    /**
     * @brief Retrieve an episode
     * 
     * Retrieves an episode from episodic memory by index. This enables
     * access to stored events and experiences for:
     * - Episodic recall and reconstruction
     * - Temporal navigation
     * - Memory-based reasoning
     * - Analysis of past events
     * 
     * @param index Index of episode to retrieve (0 to episode count-1)
     * @return Retrieved episodic memory item
     * 
     * @pre 0 <= index < episode count
     * @post Episode retrieved from episodic memory
     * @post Original episode preserved
     * 
     * @return Retrieved episode
     * @retval episode Retrieved episodic memory item
     * @retval undefined If index out of range
     * 
     * @note Episodes are accessed by chronological index
     * @see storeEpisode() - For storing episodes
     * @see getEpisodeCount() - For counting episodes
     */
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    /**
     * @brief Get episode count
     * 
     * Returns the number of episodes stored in episodic memory.
     * This is an important statistic for understanding memory capacity
     * and load.
     * 
     * @return Number of episodes in memory
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @see retrieveEpisode() - For retrieving episodes
     * @see getRecentEpisodes() - For retrieving recent episodes
     */
    size_t getEpisodeCount() const;
    
    /**
     * @brief Get recent episodes
     * 
     * Retrieves recent episodes from episodic memory by count. This is
     * useful for:
     * - Accessing recent history
     * - Analyzing recent events
     * - Maintaining working memory context
     * - Temporal navigation
     * 
     * @param count Number of recent episodes to retrieve
     * @return Vector of recent episodic memory items
     * 
     * @pre count <= episode count
     * @post Recent episodes retrieved
     * @post Episodes in chronological order
     * 
     * @return Recent episodes
     * @retval episodes Vector of retrieved episodes
     * @retval empty If no episodes
     * 
     * @note Episodes ordered by recency
     * @see retrieveEpisode() - For retrieving specific episode
     * @see getEpisodeCount() - For counting episodes
     */
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    /**
     * @brief Clear all episodes
     * 
     * Clears all episodes from episodic memory, removing all stored events
     * and experiences. This is useful for:
     * - Resetting episodic memory
     * - Managing memory capacity
     * - Memory consolidation
     * - Error recovery
     * 
     * @post All episodes cleared from episodic memory
     * @post Episodic memory ready for new operations
     * @post All temporal data removed
     * 
     * @see storeEpisode() - For storing episodes
     * @see retrieveEpisode() - For retrieving episodes
     */
    void clear();
    
    /**
     * @brief Consolidate memories
     * 
     * Performs memory consolidation, moving important episodes from
     * episodic memory to long-term storage. This is essential for:
     * - Managing memory capacity
     * - Prioritizing important events
     * - Maintaining memory efficiency
     * - Supporting learning
     * 
     * @param relevanceThreshold Threshold for consolidation (0.0 to 1.0)
     * 
     * @pre 0.0 <= relevanceThreshold <= 1.0
     * @post Important episodes consolidated
     * @post Memory capacity optimized
     * 
     * @note Implements memory consolidation strategies
     * @see storeEpisode() - For storing episodes
     * @see getEpisodeCount() - For counting episodes
     */
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class SemanticMemory
 * 
 * Semantic memory: gradually acquired knowledge.
 * 
 * Semantic memory is a core memory system in NLM that provides long-term
 * storage of facts, concepts, and general knowledge. It is essential for:
 * - Storing what we know and understand
 * - Maintaining general knowledge
 * - Supporting reasoning and problem-solving
 * - Enabling learning and inference
 * - Modeling cognitive knowledge
 * 
 * Semantic memory mechanisms include:
 * - Fact storage and retrieval
 * - Knowledge organization and categorization
 * - Concept formation and abstraction
 * - Memory association and connection
 * - Learning and adaptation
 * 
 * Key characteristics of semantic memory in NLM:
 * - **General**: Stores general knowledge and facts
 * - **Structured**: Organized and categorized information
 * - **Persistent**: Long-term storage of knowledge
 * - **Abstract**: Represents concepts and relationships
 * - **Adaptive**: Can be updated and modified
 * 
 * @note Essential for knowledge and reasoning
 * @note Used by Brain for semantic memory access
 * @note Critical for cognitive function
 * @see WorkingMemory - For temporary storage
 * @see EpisodicMemory - For experience storage
 * @see ConceptFormation.hpp - For concept formation
 * 
 * @author NLM Development Team
 */
class SemanticMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new semantic memory instance with default parameters.
     * The semantic memory is initialized with empty storage and ready
     * for storing and retrieving facts.
     * 
     * @post Semantic memory created with default settings
     * @post Ready for fact storage and retrieval
     */
    SemanticMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up semantic memory resources and performs any necessary
     * cleanup operations. The semantic memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Semantic memory cannot be used after destruction
     */
    ~SemanticMemory();
    
    // Disable copying - PImpl idiom
    SemanticMemory(const SemanticMemory&) = delete;
    SemanticMemory& operator=(const SemanticMemory&) = delete;
    SemanticMemory(SemanticMemory&&) noexcept;
    SemanticMemory& operator=(SemanticMemory&&) noexcept;
    
    /**
     * @brief Store a fact
     * 
     * Stores a fact in semantic memory, associating it with a key for
     * later retrieval. This is essential for:
     * - Maintaining general knowledge
     * - Supporting reasoning and problem-solving
     * - Enabling learning and inference
     * - Organizing knowledge
     * 
     * @param key Unique key identifying the fact
     * @param value Fact value to store
     * 
     * @pre key must be unique
     * @post Fact stored in semantic memory
     * @post Key-value pair recorded
     * 
     * @note Keys must be unique
     * @see retrieveFact() - For retrieving facts
     * @see hasFact() - For checking fact existence
     * @see getAllFacts() - For retrieving all facts
     */
    void storeFact(const std::string& key, const std::string& value);
    
    /**
     * @brief Retrieve a fact
     * 
     * Retrieves a fact from semantic memory using its key. This is
     * essential for:
     * - Accessing stored knowledge
     * - Supporting reasoning and problem-solving
     * - Retrieving learned information
     * - Knowledge-based operations
     * 
     * @param key Key of fact to retrieve
     * @return Fact value associated with key
     * 
     * @pre key must be valid and stored
     * @post Fact retrieved from semantic memory
     * @post Original fact preserved
     * 
     * @return Retrieved fact
     * @retval value Fact value associated with key
     * @retval empty If key not found
     * 
     * @note Returns empty string if key not found
     * @see storeFact() - For storing facts
     * @see hasFact() - For checking fact existence
     */
    std::string retrieveFact(const std::string& key) const;
    
    /**
     * @brief Check if fact exists
     * 
     * Determines whether a fact exists in semantic memory for the
     * specified key. This is useful for:
     * - Checking knowledge existence
     * - Implementing error handling
     * - Managing fact retrieval
     * - Knowledge validation
     * 
     * @param key Key to check
     * @return true if fact exists, false otherwise
     * 
     * @pre key must be valid
     * @post Return value indicates fact existence
     * @complexity O(1) for lookup
     * 
     * @see storeFact() - For storing facts
     * @see retrieveFact() - For retrieving facts
     */
    bool hasFact(const std::string& key) const;
    
    /**
     * @brief Get all facts
     * 
     * Returns all facts stored in semantic memory. This is useful for:
     * - Retrieving complete knowledge base
     * - Analyzing stored information
     * - Knowledge export
     * - Memory management
     * 
     * @return Vector of all fact pairs (key, value)
     * 
     * @post Return value stable during operation
     * @complexity O(n) where n is number of facts
     * 
     * @note Returns pairs in insertion order
     * @see storeFact() - For storing facts
     * @see hasFact() - For checking fact existence
     */
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    /**
     * @brief Clear all facts
     * 
     * Clears all facts from semantic memory, removing all stored
     * knowledge. This is useful for:
     * - Resetting semantic memory
     * - Managing memory capacity
     * - Knowledge management
     * - Error recovery
     * 
     * @post All facts cleared from semantic memory
     * @post Semantic memory ready for new operations
     * @post All knowledge removed
     * 
     * @see storeFact() - For storing facts
     * @see retrieveFact() - For retrieving facts
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class ProceduralMemory
 * 
 * Procedural memory: learned skills and habits.
 * 
 * Procedural memory is a core memory system in NLM that provides long-term
 * storage of skills, habits, and procedures. It is essential for:
 * - Storing learned skills and routines
 * - Maintaining behavioral patterns
 * - Supporting action execution
 * - Enabling habit formation
 * - Modeling motor memory
 * 
 * Procedural memory mechanisms include:
 * - Skill storage and retrieval
 * - Habit formation and reinforcement
 * - Pattern completion and execution
 * - Skill proficiency tracking
 * - Procedural learning
 * 
 * Key characteristics of procedural memory in NLM:
 * - **Automatic**: Skills are executed automatically
 * - **Implicit**: Knowledge is not consciously accessible
 * - **Behavioral**: Influences behavior and action
 * - **Performance**: Tracks skill proficiency
 * - **Adaptive**: Can be improved through practice
 * 
 * @note Essential for behavior and skill
 * @note Used by Brain for procedural memory access
 * @note Critical for motor function
 * @see WorkingMemory - For temporary storage
 * @see EpisodicMemory - For experience storage
 * @see MotorSystem.hpp - For action execution
 * 
 * @author NLM Development Team
 */
class ProceduralMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new procedural memory instance with default parameters.
     * The procedural memory is initialized with empty storage and ready
     * for storing and retrieving skills.
     * 
     * @post Procedural memory created with default settings
     * @post Ready for skill storage and retrieval
     */
    ProceduralMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up procedural memory resources and performs any necessary
     * cleanup operations. The procedural memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Procedural memory cannot be used after destruction
     */
    ~ProceduralMemory();
    
    // Disable copying - PImpl idiom
    ProceduralMemory(const ProceduralMemory&) = delete;
    ProceduralMemory& operator=(const ProceduralMemory&) = delete;
    ProceduralMemory(ProceduralMemory&&) noexcept;
    ProceduralMemory& operator=(ProceduralMemory&&) noexcept;
    
    /**
     * @brief Learn a skill
     * 
     * Learns a new skill in procedural memory, storing the neural pattern
     * and initial proficiency. This is essential for:
     * - Building skill repertoire
     * - Maintaining behavioral capabilities
     * - Supporting action execution
     * - Enabling habit formation
     * 
     * @param name Unique name identifying the skill
     * @param pattern Neural pattern representing the skill
     * 
     * @pre name must be unique
     * @pre pattern must be valid neural pattern
     * @post Skill learned and stored
     * @post Proficiency initialized
     * 
     * @note Skills are stored by name
     * @see getSkill() - For retrieving skills
     * @see getAllSkills() - For retrieving all skills
     * @see updateProficiency() - For updating skill proficiency
     */
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    /**
     * @brief Get a skill
     * 
     * Retrieves a skill from procedural memory using its name. This is
     * essential for:
     * - Accessing learned skills
     * - Implementing action execution
     * - Retrieving behavioral patterns
     * - Skill-based operations
     * 
     * @param name Name of skill to retrieve
     * @return Pointer to skill (nullptr if not found)
     * 
     * @pre name must be valid
     * @post Skill retrieved from procedural memory
     * @post Original skill preserved
     * 
     * @return Retrieved skill pointer
     * @retval skill Pointer to retrieved skill
     * @retval nullptr If skill not found
     * 
     * @note Returns nullptr if skill not found
     * @see learnSkill() - For learning skills
     * @see getAllSkills() - For retrieving all skills
     */
    Skill* getSkill(const std::string& name);
    
    /**
     * @brief Get all skills
     * 
     * Returns all skills stored in procedural memory. This is useful for:
     * - Retrieving complete skill repertoire
     * - Analyzing stored capabilities
     * - Skill management
     * - Capability assessment
     * 
     * @return Vector of skill pointers
     * 
     * @post Return value stable during operation
     * @complexity O(n) where n is number of skills
     * 
     * @note Returns pointers to stored skills
     * @see learnSkill() - For learning skills
     * @see getSkill() - For retrieving specific skill
     */
    std::vector<Skill*> getAllSkills();
    
    /**
     * @brief Update skill proficiency
     * 
     * Updates the proficiency of a skill based on practice and experience.
     * This is essential for:
     * - Improving skill performance
     * - Tracking skill development
     * - Managing skill adaptation
     * - Supporting skill learning
     * 
     * @param name Name of skill to update
     * @param delta Proficiency change (positive for improvement)
     * 
     * @pre name must be valid
     * @post Skill proficiency updated
     * @post Skill performance affected
     * 
     * @note Proficiency tracks skill performance
     * @see learnSkill() - For learning skills
     * @see getSkill() - For retrieving skills
     */
    void updateProficiency(const std::string& name, float delta);
    
    /**
     * @brief Clear all skills
     * 
     * Clears all skills from procedural memory, removing all stored
     * habits and behaviors. This is useful for:
     * - Resetting procedural memory
     * - Managing memory capacity
     * - Behavior management
     * - Error recovery
     * 
     * @post All skills cleared from procedural memory
     * @post Procedural memory ready for new operations
     * @post All skills removed
     * 
     * @see learnSkill() - For learning skills
     * @see getSkill() - For retrieving skills
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class AssociativeMemory
 * 
 * Associative memory: relationships between representations.
 * 
 * Associative memory is a core memory system in NLM that provides storage
 * of relationships between neural representations. It is essential for:
 * - Storing connections between representations
 * - Pattern completion and association
 * - Content-addressable memory
 * - Memory-based reasoning
 * - Connectionist processing
 * 
 * Associative memory mechanisms include:
 * - Association storage and retrieval
 * - Pattern completion and completion
 * - Connection strength modulation
 * - Memory association and linking
 * - Network-based processing
 * 
 * Key characteristics of associative memory in NLM:
 * - **Relational**: Stores relationships between items
 * - **Content-addressable**: Retrieves by content
 * - **Distributed**: Information spread across network
 * - **Dynamic**: Connections can be modified
 * - **Adaptive**: Can learn new associations
 * 
 * @note Essential for relationship storage
 * @note Used by Brain for associative memory access
 * @note Critical for pattern completion
 * @see WorkingMemory - For temporary storage
 * @see EpisodicMemory - For experience storage
 * @see ConceptFormation.hpp - For pattern discovery
 * 
 * @author NLM Development Team
 */
class AssociativeMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new associative memory instance with default parameters.
     * The associative memory is initialized with empty storage and ready
     * for storing and retrieving associations.
     * 
     * @post Associative memory created with default settings
     * @post Ready for association storage and retrieval
     */
    AssociativeMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up associative memory resources and performs any necessary
     * cleanup operations. The associative memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Associative memory cannot be used after destruction
     */
    ~AssociativeMemory();
    
    // Disable copying - PImpl idiom
    AssociativeMemory(const AssociativeMemory&) = delete;
    AssociativeMemory& operator=(const AssociativeMemory&) = delete;
    AssociativeMemory(AssociativeMemory&&) noexcept;
    AssociativeMemory& operator=(AssociativeMemory&&) noexcept;
    
    /**
     * @brief Create association
     * 
     * Creates an association between two neural representations.
     * This is essential for:
     * - Building associative networks
     * - Storing relationships between items
     * - Pattern completion and association
     * - Memory-based reasoning
     * - Connectionist processing
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @param strength Association strength (default: 0.0f)
     * 
     * @pre a and b must be valid neuron IDs
     * @pre a != b (no self-associations)
     * @post Association created
     * @post Strength recorded
     * 
     * @note Associations are bidirectional
     * @see getAssociations() - For retrieving associations
     * @see getAssociationStrength() - For association strength
     * @see updateAssociation() - For updating associations
     */
    void associate(NeuronId a, NeuronId b, float strength = 0.0f);
    
    /**
     * @brief Get associated neurons
     * 
     * Returns a vector of neurons associated with the specified neuron.
     * This is essential for:
     * - Pattern completion
     * - Retrieval of related items
     * - Memory-based reasoning
     * - Association-based processing
     * 
     * @param neuron Neuron ID to query
     * @return Vector of associated neuron IDs
     * 
     * @pre neuron must be a valid neuron ID
     * @post Return value stable during operation
     * @complexity O(n) where n is number of associations
     * 
     * @note Returns all neurons directly associated with neuron
     * @see associate() - For creating associations
     */
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    /**
     * @brief Get association strength
     * 
     * Returns the strength of the association between two neurons.
     * This is essential for:
     * - Measuring association strength
     * - Controlling associative influence
     * - Managing association dynamics
     * - Association-based processing
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @return Association strength
     * 
     * @pre a and b must be valid neuron IDs
     * @post Return value stable during operation
     * @complexity O(1) for lookup
     * 
     * @note Returns 0.0f if no association exists
     * @see associate() - For creating associations
     * @see updateAssociation() - For updating associations
     */
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    /**
     * @brief Update association
     * 
     * Updates the strength of an existing association between two
     * neurons. This is essential for:
     * - Modifying association strength
     * - Learning new associations
     * - Adjusting associative influence
     * - Managing association dynamics
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @param delta Strength change (positive for strengthening)
     * 
     * @pre a and b must be valid neuron IDs
     * @post Association strength updated
     * @post Association influence changed
     * 
     * @note Can be used for learning new associations
     * @see associate() - For creating associations
     * @see getAssociationStrength() - For getting strength
     */
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    /**
     * @brief Clear all associations
     * 
     * Clears all associations from associative memory, removing all
     * stored relationships. This is useful for:
     * - Resetting associative memory
     * - Managing memory capacity
     * - Association management
     * - Error recovery
     * 
     * @post All associations cleared from associative memory
     * @post Associative memory ready for new operations
     * @post All relationships removed
     * 
     * @see associate() - For creating associations
     * @see getAssociations() - For retrieving associations
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
