#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

/**
 * @brief Working memory: temporary active storage of information
 * 
 * Working memory provides short-term storage for currently active information
 * during neural processing. It maintains representations of recent events and
 * active neural patterns, enabling immediate cognitive operations and decision making.
 * 
 * Key characteristics:
 * - Transient storage: Items decay over time if not actively used
 * - Activity-dependent: Stores information related to currently active neurons
 * - Fast access: Optimized for rapid retrieval of active representations
 * - Limited capacity: Maintains only the most relevant current information
 * 
 * Phase 2 TODO: Implement real working memory with:
 * - Dynamic capacity management
 * - Competitive inhibition between stored items
 * - Integration with neural activity patterns
 * - Active decay and consolidation mechanisms
 */
class WorkingMemory {
public:
    /**
     * @brief Default constructor creates an empty working memory
     * 
     * @note Initializes with default capacity and settings. The actual working
     *       memory implementation will be completed in Phase 2.
     */
    WorkingMemory();
    
    /**
     * @brief Destroy the WorkingMemory instance
     * 
     * @note Ensures proper cleanup of working memory contents and resources.
     */
    ~WorkingMemory();
    
    // Disable copying, enable moving for efficient resource management
    WorkingMemory(const WorkingMemory&) = delete;
    WorkingMemory& operator=(const WorkingMemory&) = delete;
    WorkingMemory(WorkingMemory&&) noexcept;
    WorkingMemory& operator=(WorkingMemory&&) noexcept;
    
    /**
     * @brief Store an item in working memory
     * 
     * @param neuron Neuron identifier associated with the stored information
     * @param value The information value to store (neuron activation level, pattern strength, etc.)
     * 
     * @note Phase 2: Implement real storage with:
     * - Activity-dependent storage rules
     * - Competitive storage mechanisms
     * - Integration with neural firing patterns
     * - Decay and consolidation controls
     */
    void store(NeuronId neuron, float value);
    
    /**
     * @brief Retrieve an item from working memory
     * 
     * @param neuron Neuron identifier of the item to retrieve
     * @return float The stored value associated with the neuron
     * 
     * @note Phase 2: Implement real retrieval with:
     * - Error handling for missing items
     * - Integration with active neural state
     * - Competition with other stored items
     */
    float retrieve(NeuronId neuron) const;
    
    /**
     * @brief Check if an item exists in working memory
     * 
     * @param neuron Neuron identifier to check
     * @return bool true if item exists, false otherwise
     * 
     * @note Used for working memory queries and validation.
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Clear all items from working memory
     * 
     * @note Resets working memory to empty state. This may be called periodically
     *       to prevent memory overload or during simulation resets.
     */
    void clear();
    
    /**
     * @brief Get the maximum capacity of working memory
     * 
     * @return size_t Maximum number of items that can be stored simultaneously
     * 
     * @note Phase 2: Implement capacity management with:
     * - Dynamic capacity adjustment based on cognitive load
     * - Eviction policies for full memory states
     */
    size_t getCapacity() const;
    
    /**
     * @brief Get the current number of items in working memory
     * 
     * @return size_t Current number of stored items
     * 
     * @note Used for monitoring working memory load and state assessment.
     */
    size_t getCurrentSize() const;
    
    /**
     * @brief Apply decay to all items in working memory
     * 
     * @param decayRate Decay factor (0.0 to 1.0, where 1.0 = complete decay)
     * 
     * @note Implements the natural forgetting process in working memory.
     *       Items lose value over time if not actively maintained or used.
     *       
     * @note Phase 2: Implement controlled decay with:
     * - Time-dependent decay rates
     * - Item-specific decay factors
     * - Integration with consolidation processes
     */
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

/**
 * @brief Episodic memory: storage of events and experiences
 * 
 * Episodic memory stores discrete events, experiences, and episodes with temporal
 * ordering and contextual information. It enables the brain to recall past events,
 * recognize patterns, and learn from experience through replay and consolidation.
 * 
 * Key characteristics:
 * - Temporal ordering: Events stored in chronological order
 * - Rich context: Each episode includes metadata and associated neural patterns
 * - Replay capability: Episodes can be reactivated for consolidation
 * - Consolidation: Transfer of important episodes to long-term storage
 * - Integration with neural activity: Episodes linked to specific neural patterns
 * 
 * Phase 2 TODO: Implement real episodic memory with:
 * - Detailed event recording with neural correlates
 * - Replay mechanisms during offline periods
 * - Consolidation algorithms for important episodes
 * - Integration with working memory and semantic memory
 */
struct EpisodicMemoryItem {
    SimulationStep timestamp;          ///< Time when the episode occurred
    std::vector<NeuronId> neurons;      ///< Neural pattern representing the episode
    std::vector<float> values;         ///< Associated neural activity levels
    std::string metadata;              ///< Additional contextual information
    
    /**
     * @brief Default constructor creates an empty episode
     * 
     * @note For valid episodic memory items, use the parameterized constructor
     *       with appropriate timestamp and neural pattern information.
     */
    EpisodicMemoryItem() : timestamp(0) {}
};

/**
 * @brief Episodic memory: storage of events and experiences
 * 
 * Episodic memory provides long-term storage for experienced events and episodes.
 * It enables the brain to learn from experience, recognize patterns, and form
 * expectations based on past events through storage, retrieval, and consolidation.
 * 
 * Key features:
 * - Event recording: Stores complete episodes with temporal and neural information
 * - Replay consolidation: Reactivates episodes during offline processing
 * - Pattern completion: Retrieves similar episodes based on context
 * - Integration: Connects with working memory and semantic memory systems
 * - Temporal coherence: Maintains chronological ordering of experiences
 * 
 * Phase 2 TODO: Implement real episodic memory with:
 * - Efficient storage of large episode collections
 * - Fast retrieval based on temporal or pattern similarity
 * - Consolidation mechanisms for important experiences
 * - Integration with neural prediction systems
 */
class EpisodicMemory {
public:
    /**
     * @brief Default constructor creates an empty episodic memory
     * 
     * @note Initializes with empty episode storage. Memory capacity and
     *       storage algorithms will be implemented in Phase 2.
     */
    EpisodicMemory();
    
    /**
     * @brief Destroy the EpisodicMemory instance
     * 
     * @note Ensures proper cleanup of stored episodes and allocated memory.
     */
    ~EpisodicMemory();
    
    // Disable copying, enable moving for efficient resource management
    EpisodicMemory(const EpisodicMemory&) = delete;
    EpisodicMemory& operator=(const EpisodicMemory&) = delete;
    EpisodicMemory(EpisodicMemory&&) noexcept;
    EpisodicMemory& operator=(EpisodicMemory&&) noexcept;
    
    /**
     * @brief Store a complete episode in episodic memory
     * 
     * @param episode The episodic memory item to store
     * 
     * @note Phase 2: Implement storage with:
     * - Validation of episode data
     * - Efficient storage algorithms
     * - Integration with neural activity patterns
     * - Capacity management and eviction policies
     */
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    /**
     * @brief Retrieve a specific episode from memory
     * 
     * @param index Index of the episode to retrieve (0..episode_count-1)
     * @return EpisodicMemoryItem The requested episode
     * 
     * @note Phase 2: Implement retrieval with:
     * - Error handling for invalid indices
     * - Efficient access patterns for large episode collections
     * - Reconstruction of neural patterns and context
     */
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    /**
     * @brief Get the number of stored episodes
     * 
     * @return size_t Total number of episodes in memory
     */
    size_t getEpisodeCount() const;
    
    /**
     * @brief Get recent episodes in temporal order
     * 
     * @param count Maximum number of recent episodes to retrieve
     * @return std::vector<EpisodicMemoryItem> Vector of recent episodes
     * 
     * @note Returns episodes in chronological order (oldest to newest).
     *       Used for temporal analysis and recent experience processing.
     */
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    /**
     * @brief Clear all episodes from memory
     * 
     * @note Resets episodic memory to empty state. May be called periodically
     *       to prevent memory overload or during simulation resets.
     */
    void clear();
    
    /**
     * @brief Consolidate important episodes into long-term storage
     * 
     * @param relevanceThreshold Threshold for episode relevance (0.0 to 1.0)
     * 
     * @note Moves highly relevant episodes from episodic to semantic memory
     *       or permanently stores them. Consolidation extracts patterns and
     *       generalizes across similar episodes.
     *       
     * @note Phase 2: Implement consolidation with:
     * - Relevance scoring based on neural impact and frequency
     * - Pattern extraction and generalization algorithms
     * - Integration with semantic memory formation
     */
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

/**
 * @brief Semantic memory: gradually acquired knowledge
 * 
 * Semantic memory stores abstract facts, concepts, and knowledge acquired through
 * experience. It enables general understanding, categorization, and reasoning
 * based on accumulated knowledge rather than specific episodes.
 * 
 * Key characteristics:
 * - Abstract representation: Stores facts as key-value pairs
 * - Generalization: Extracts common patterns from multiple episodes
 * - Schema-based: Organized around concepts and categories
 * - Cumulative learning: Builds upon previously learned knowledge
 * - Cross-episode integration: Combines information from multiple experiences
 * 
 * Phase 2 TODO: Implement real semantic memory with:
 * - Concept formation from episodic patterns
 * - Hierarchical knowledge organization
 * - Cross-modal integration
 * - Schema-based reasoning capabilities
 */
class SemanticMemory {
public:
    /**
     * @brief Default constructor creates an empty semantic memory
     * 
     * @note Initializes with empty knowledge base. Semantic memory will be
     *       populated through consolidation from episodic memory in Phase 2.
     */
    SemanticMemory();
    
    /**
     * @brief Destroy the SemanticMemory instance
     * 
     * @note Ensures proper cleanup of stored knowledge and allocated memory.
     */
    ~SemanticMemory();
    
    // Disable copying, enable moving for efficient resource management
    SemanticMemory(const SemanticMemory&) = delete;
    SemanticMemory& operator=(const SemanticMemory&) = delete;
    SemanticMemory(SemanticMemory&&) noexcept;
    SemanticMemory& operator=(SemanticMemory&&) noexcept;
    
    /**
     * @brief Store a fact in semantic memory
     * 
     * @param key Unique identifier for the fact
     * @param value The factual information to store
     * 
     * @note Phase 2: Implement storage with:
     * - Key validation and conflict resolution
     * - Efficient retrieval based on keys
     * - Integration with concept formation algorithms
     * - Knowledge base consistency maintenance
     */
    void storeFact(const std::string& key, const std::string& value);
    
    /**
     * @brief Retrieve a fact from semantic memory
     * 
     * @param key Unique identifier of the fact to retrieve
     * @return std::string The stored factual information
     * 
     * @note Phase 2: Implement retrieval with:
     * - Error handling for unknown keys
     * - Value validation and type checking
     * - Integration with reasoning algorithms
     */
    std::string retrieveFact(const std::string& key) const;
    
    /**
     * @brief Check if a fact exists in semantic memory
     * 
     * @param key Unique identifier to check
     * @return bool true if fact exists, false otherwise
     * 
     * @note Used for semantic memory queries and validation.
     */
    bool hasFact(const std::string& key) const;
    
    /**
     * @brief Get all facts currently stored in semantic memory
     * 
     * @return std::vector<std::pair<std::string, std::string>> Vector of all fact pairs
     * 
     * @note Provides access to the complete knowledge base for analysis and
     *       reasoning operations.
     */
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    /**
     * @brief Clear all facts from semantic memory
     * 
     * @note Resets semantic memory to empty state. May be called periodically
     *       to prevent knowledge base bloat or during simulation resets.
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

/**
 * @brief Procedural memory: learned skills and habits
 * 
 * Procedural memory stores skills, habits, and motor patterns learned through
 * repeated practice and experience. It enables automatic execution of behaviors
 * without conscious control, supporting efficient action selection and execution.
 * 
 * Key characteristics:
 * - Skill representation: Stores skills with neural patterns and proficiency levels
 * - Incremental learning: Skills improve with practice and experience
 * - Automatic execution: Skills become automated with high proficiency
 * - Pattern storage: Links skills to neural activation patterns
 * - Motor integration: Connects skills with motor system outputs
 * 
 * Phase 2 TODO: Implement real procedural memory with:
 * - Skill acquisition algorithms
 * - Practice-based learning mechanisms
 * - Motor skill integration
 * - Habit formation and consolidation
 */
struct Skill {
    std::string name;               ///< Human-readable skill name
    std::vector<NeuronId> neuralPattern; ///< Neural pattern representing the skill
    float proficiency;              ///< Skill proficiency level (0.0 to 1.0)
    
    /**
     * @brief Default constructor creates an uninitialized skill
     * 
     * @note Skills should be properly initialized with name and pattern
     *       before being used in procedural memory operations.
     */
    Skill() : proficiency(0.0f) {}
};

/**
 * @brief Procedural memory: learned skills and habits
 * 
 * Procedural memory stores skills, habits, and learned behaviors that have become
 * automatic through practice and experience. It enables efficient, unconscious
 * execution of familiar actions and supports motor learning and habit formation.
 * 
 * Key features:
 * - Skill storage: Maintains skills with neural patterns and proficiency levels
 * - Incremental learning: Skills improve through practice and experience
 * - Automatic execution: High-proficiency skills execute automatically
 * - Pattern-based learning: Links skills to specific neural activation patterns
 * - Integration: Connects with motor system and cognitive planning
 * 
 * Phase 2 TODO: Implement real procedural memory with:
 * - Skill acquisition and refinement algorithms
 * - Practice-based learning curves
 * - Motor skill integration and refinement
 * - Habit formation and strength maintenance
 */
class ProceduralMemory {
public:
    /**
     * @brief Default constructor creates an empty procedural memory
     * 
     * @note Initializes with empty skill storage. Skills will be learned
     *       through experience and practice in Phase 2.
     */
    ProceduralMemory();
    
    /**
     * @brief Destroy the ProceduralMemory instance
     * 
     * @note Ensures proper cleanup of stored skills and allocated memory.
     */
    ~ProceduralMemory();
    
    // Disable copying, enable moving for efficient resource management
    ProceduralMemory(const ProceduralMemory&) = delete;
    ProceduralMemory& operator=(const ProceduralMemory&) = delete;
    ProceduralMemory(ProceduralMemory&&) noexcept;
    ProceduralMemory& operator=(ProceduralMemory&&) noexcept;
    
    /**
     * @brief Learn a new skill
     * 
     * @param name Human-readable skill name
     * @param pattern Neural pattern associated with the skill
     * 
     * @note Phase 2: Implement skill learning with:
     * - Initial skill acquisition
     * - Neural pattern extraction from experience
     * - Integration with motor and cognitive systems
     * - Practice-based refinement
     */
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    /**
     * @brief Retrieve a skill from memory
     * 
     * @param name Skill name to retrieve
     * @return Skill* Pointer to the retrieved skill (nullptr if not found)
     * 
     * @note Phase 2: Implement retrieval with:
     * - Error handling for unknown skills
     * - Skill proficiency assessment
     * - Pattern matching and completion
     */
    Skill* getSkill(const std::string& name);
    
    /**
     * @brief Get all skills currently stored in memory
     * 
     * @return std::vector<Skill*> Vector of all skill pointers
     * 
     * @note Provides access to the complete skill repertoire for action selection
     *       and learning analysis.
     */
    std::vector<Skill*> getAllSkills();
    
    /**
     * @brief Update a skill's proficiency level
     * 
     * @param name Skill name to update
     * @param delta Change in proficiency (positive for improvement, negative for decline)
     * 
     * @note Phase 2: Implement proficiency updates with:
     * - Skill-specific learning curves
     * - Practice-based improvement
     * - Forgetting mechanisms for unused skills
     * - Integration with reward systems
     */
    void updateProficiency(const std::string& name, float delta);
    
    /**
     * @brief Clear all skills from memory
     * 
     * @note Resets procedural memory to empty state. May be called periodically
     *       or during simulation resets to clear learned behaviors.
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

/**
 * @brief Associative memory: relationships between representations
 * 
 * Associative memory stores relationships and connections between different neural
 * representations and patterns. It enables pattern completion, similarity-based
 * retrieval, and the formation of abstract associations between concepts and experiences.
 * 
 * Key characteristics:
 * - Pattern association: Links between neural patterns and their relationships
 * - Strength-based storage: Associations have activation strengths
 * - Pattern completion: Can retrieve related patterns based on partial input
 * - Cross-representation connections: Links different types of representations
 * - Integration: Connects with all memory system types
 * 
 * Phase 2 TODO: Implement real associative memory with:
 * - Connection formation algorithms
 * - Pattern completion mechanisms
 * - Strength dynamics and decay
 * - Integration with prediction systems
 */
class AssociativeMemory {
public:
    /**
     * @brief Default constructor creates an empty associative memory
     * 
     * @note Initializes with empty association storage. Associations will be
     *       formed through experience and learning in Phase 2.
     */
    AssociativeMemory();
    
    /**
     * @brief Destroy the AssociativeMemory instance
     * 
     * @note Ensures proper cleanup of stored associations and allocated memory.
     */
    ~AssociativeMemory();
    
    // Disable copying, enable moving for efficient resource management
    AssociativeMemory(const AssociativeMemory&) = delete;
    AssociativeMemory& operator=(const AssociativeMemory&) = delete;
    AssociativeMemory(AssociativeMemory&&) noexcept;
    AssociativeMemory& operator=(AssociativeMemory&&) noexcept;
    
    /**
     * @brief Create an association between two neural representations
     * 
     * @param a First neuron identifier
     * @param b Second neuron identifier  
     * @param strength Initial association strength (-1.0 to 1.0)
     * 
     * @note Phase 2: Implement association formation with:
     * - Co-activation learning
     * - Strength dynamics and normalization
     * - Integration with synaptic plasticity
     * - Competition and interference management
     */
    void associate(NeuronId a, NeuronId b, float strength);
    
    /**
     * @brief Get all neurons associated with a given neuron
     * 
     * @param neuron Neuron identifier to query
     * @return std::vector<NeuronId> Vector of associated neuron identifiers
     * 
     * @note Pattern completion: Given a partial pattern, retrieve all related
     *       representations through associative connections.
     */
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    /**
     * @brief Get the strength of an association between two neurons
     * 
     * @param a First neuron identifier
     * @param b Second neuron identifier
     * @return float Association strength (-1.0 to 1.0)
     * 
     * @note Association strength determines the strength of influence between
     *       the two neural representations.
     */
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    /**
     * @brief Update the strength of an association
     * 
     * @param a First neuron identifier
     * @param b Second neuron identifier
     * @param delta Change in association strength
     * 
     * @note Phase 2: Implement association updates with:
     * - Co-activity based learning
     * - Competitive normalization
     * - Integration with memory consolidation
     */
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    /**
     * @brief Clear all associations from memory
     * 
     * @note Resets associative memory to empty state. May be called periodically
     *       or during simulation resets to clear learned relationships.
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm