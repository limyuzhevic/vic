#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <string>

namespace nlm {

/**
 * @struct EpisodicMemoryItem
 * @brief Represents a single episodic memory (event/experience)
 * @details Stores information about a specific experience including when it occurred,
 * which neurons were active, their activation levels, and associated metadata.
 * Episodic memories form the basis of autobiographical memory and enable
 * the recall of specific past events.
 */
struct EpisodicMemoryItem {
    SimulationStep timestamp;           ///< Time when the episode occurred
    std::vector<NeuronId> neurons;       ///< Neurons involved in the episode
    std::vector<float> values;           ///< Activation levels for each neuron
    std::string metadata;               ///< Additional descriptive information
    
    /**
     * @brief Default constructor
     * @details Creates an empty episode with zero timestamp
     */
    EpisodicMemoryItem() : timestamp(0) {}
    
    /**
     * @brief Parameterized constructor
     * @param ts Time when the episode occurred
     * @param n Vector of neuron IDs active during the episode
     * @param v Vector of activation values for each neuron
     * @param meta Additional metadata about the episode
     */
    EpisodicMemoryItem(SimulationStep ts, const std::vector<NeuronId>& n, 
                      const std::vector<float>& v, const std::string& meta = "")
        : timestamp(ts), neurons(n), values(v), metadata(meta) {}
};

/**
 * @class WorkingMemory
 * @brief Temporary active storage of information for cognitive processing
 * @details Working memory maintains a limited-capacity representation of
 * current information, allowing for manipulation and integration with new
 * inputs. Unlike long-term memory, working memory is transient and capacity-limited,
 * serving as the mental workspace for ongoing cognitive tasks.
 * 
 * Key mechanisms:
 * - Capacity-limited storage with eviction policies
 * - Neural pattern matching for item retrieval
 * - Decay dynamics for information fading
 * - Integration with attention and learning systems
 * 
 * @note This class is implemented with a pImpl pattern for performance
 * and encapsulation of implementation details.
 */
class WorkingMemory {
public:
    /**
     * @brief Constructs an empty working memory
     * @details Creates working memory with default capacity (100 items)
     * @post Working memory is initialized but empty
     */
    WorkingMemory();
    
    /**
     * @brief Destroys the working memory
     * @details Cleans up all stored items and memory management structures
     */
    ~WorkingMemory();
    
    /**
     * @brief Initializes working memory with specified capacity
     * @details Sets the maximum number of items that can be stored and evicts
     * items if the current count exceeds the new capacity.
     * 
     * @param capacity Maximum number of items to store (default: 100)
     * @post Capacity is set and items are evicted if necessary
     */
    void initialize(size_t capacity = 100);
    
    /**
     * @brief Stores information in working memory
     * @details Encodes information as a neural activation pattern, storing it
     * for temporary access during cognitive processing. If the item already exists,
     * its value is updated. If capacity is reached, the oldest item is evicted.
     * 
     * @param neuron Neuron ID representing the information to store
     * @param value Activation value (typically 0.0-1.0)
     * @post Item is stored in working memory or updated if already present
     */
    void store(NeuronId neuron, float value);
    
    /**
     * @brief Retrieves information from working memory
     * @details Looks up stored information by neuron ID and returns its activation level.
     * Returns zero if the information is not present.
     * 
     * @param neuron Neuron ID of the information to retrieve
     * @return float Activation value of the stored information (0.0 if not found)
     * @post Working memory state is unchanged
     */
    float retrieve(NeuronId neuron) const;
    
    /**
     * @brief Checks if information is stored in working memory
     * @details Determines whether a specific neuron ID is currently stored
     * in working memory.
     * 
     * @param neuron Neuron ID to check
     * @return bool true if information is stored, false otherwise
     * @post Working memory state is unchanged
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Clears all items from working memory
     * @details Removes all stored information, effectively resetting working memory
     * to an empty state.
     * 
     * @post Working memory is empty
     */
    void clear();
    
    /**
     * @brief Gets the capacity of working memory
     * @return size_t Maximum number of items that can be stored
     */
    size_t getCapacity() const;
    
    /**
     * @brief Gets the current number of items in working memory
     * @return size_t Current count of stored items
     */
    size_t getCurrentSize() const;
    
    /**
     * @brief Sets the capacity of working memory
     * @details Changes the maximum number of items and evicts items if necessary
     * to fit the new capacity.
     * 
     * @param capacity New maximum number of items
     * @post Capacity is updated and items are evicted if necessary
     */
    void setCapacity(size_t capacity);
    
    /**
     * @brief Applies decay to all items in working memory
     * @details Reduces the activation values of all stored items according to
     * the decay rate, simulating the natural fading of working memory traces.
     * 
     * @param decayRate Rate of decay (typically 0.0-1.0, where 0.0 means no decay)
     * @post All item values are multiplied by (1.0 - decayRate)
     */
    void decay(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class EpisodicMemory
 * @brief Long-term storage of experiences and events
 * @details Episodic memory encodes and stores detailed representations of
 * specific events and experiences, including temporal information, sensory
 * details, and contextual metadata. This memory system supports replay and
 * consolidation processes that strengthen important memories.
 * 
 * Key mechanisms:
 * - Bounded storage with FIFO eviction policy
 * - Neural pattern encoding of experiences
 * - Replay mechanisms for memory consolidation
 * - Integration with working memory and prediction systems
 */
class EpisodicMemory {
public:
    /**
     * @brief Constructs an empty episodic memory
     * @details Creates episodic memory with default capacity (1000 episodes)
     * @post Episodic memory is initialized but empty
     */
    EpisodicMemory();
    
    /**
     * @brief Destroys the episodic memory
     * @details Cleans up all stored episodes and memory management structures
     */
    ~EpisodicMemory();
    
    /**
     * @brief Stores an episode in episodic memory
     * @details Encodes a new experience as an episodic memory item. If the memory
     * is full, the oldest episode is evicted to make room for the new one.
     * 
     * @param episode Episodic memory item to store
     * @post Episode is added to memory (evicting oldest if necessary)
     */
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    /**
     * @brief Retrieves an episode from episodic memory
     * @details Accesses a stored episode by index for replay or analysis.
     * Returns an empty episode if the index is invalid.
     * 
     * @param index Index of the episode to retrieve
     * @return EpisodicMemoryItem Retrieved episode (empty if index out of range)
     * @post Memory state is unchanged
     */
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    /**
     * @brief Gets the number of stored episodes
     * @return size_t Count of stored episodes
     */
    size_t getEpisodeCount() const;
    
    /**
     * @brief Gets recent episodes
     * @details Retrieves a specified number of the most recent episodes for
     * analysis, replay, or consolidation processes.
     * 
     * @param count Number of recent episodes to retrieve
     * @return std::vector<EpisodicMemoryItem> Vector of recent episodes
     * @post Memory state is unchanged
     */
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    /**
     * @brief Clears all episodes from episodic memory
     * @details Removes all stored experiences, effectively resetting episodic memory
     * to an empty state.
     * 
     * @post Episodic memory is empty
     */
    void clear();
    
    /**
     * @brief Consolidates episodes into long-term memory
     * @details Moves important episodes from episodic to semantic memory through
     * a process of abstraction and generalization, reducing memory load while
     * preserving essential knowledge.
     * 
     * @param relevanceThreshold Threshold for determining episode importance
     * @post Relevant episodes may be consolidated into semantic memory
     */
    void consolidate(float relevanceThreshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class SemanticMemory
 * @brief Long-term storage of facts and knowledge
 * @details Semantic memory stores abstract knowledge and facts acquired through
 * experience, independent of the specific contexts in which they were learned.
 * This memory system supports concept formation, categorization, and general
 * knowledge representation.
 */
class SemanticMemory {
public:
    /**
     * @brief Constructs an empty semantic memory
     * @post Semantic memory is initialized but empty
     */
    SemanticMemory();
    
    /**
     * @brief Destroys the semantic memory
     * @details Cleans up all stored facts and memory management structures
     */
    ~SemanticMemory();
    
    /**
     * @brief Stores a fact in semantic memory
     * @details Encodes a key-value pair representing a piece of knowledge. If the
     * fact already exists, its value is updated.
     * 
     * @param key Unique identifier for the fact
     * @param value Value/content of the fact
     * @post Fact is stored or updated
     */
    void storeFact(const std::string& key, const std::string& value);
    
    /**
     * @brief Retrieves a fact from semantic memory
     * @details Looks up a fact by its key and returns its value. Returns empty
     * string if the fact is not found.
     * 
     * @param key Unique identifier of the fact to retrieve
     * @return std::string Value of the fact (empty if not found)
     * @post Memory state is unchanged
     */
    std::string retrieveFact(const std::string& key) const;
    
    /**
     * @brief Checks if a fact exists in semantic memory
     * @details Determines whether a specific fact key is stored in semantic memory.
     * 
     * @param key Unique identifier to check
     * @return bool true if fact exists, false otherwise
     * @post Memory state is unchanged
     */
    bool hasFact(const std::string& key) const;
    
    /**
     * @brief Gets all facts from semantic memory
     * @return std::vector<std::pair<std::string, std::string>> Vector of all facts
     * @post Memory state is unchanged
     */
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    /**
     * @brief Clears all facts from semantic memory
     * @details Removes all stored knowledge, effectively resetting semantic memory
     * to an empty state.
     * 
     * @post Semantic memory is empty
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @struct Skill
 * @brief Represents a learned procedural skill or behavior
 * @details Skills consist of neural patterns associated with specific names and
 * proficiency levels. They represent procedural knowledge that can be executed
 * automatically once learned.
 */
struct Skill {
    std::string name;                  ///< Name/identifier of the skill
    std::vector<NeuronId> neuralPattern; ///< Neural pattern that activates the skill
    float proficiency;                 ///< Current proficiency level (0.0-1.0)
    
    /**
     * @brief Default constructor
     * @details Creates an empty skill with zero proficiency
     */
    Skill() : proficiency(0.0f) {}
    
    /**
     * @brief Parameterized constructor
     * @param n Skill name
     * @param pattern Neural pattern for the skill
     * @param proficiency Initial proficiency level
     */
    Skill(const std::string& n, const std::vector<NeuronId>& pattern, float proficiency = 0.0f)
        : name(n), neuralPattern(pattern), proficiency(proficiency) {}
};

/**
 * @class ProceduralMemory
 * @brief Long-term storage of skills and habits
 * @details Procedural memory stores learned behaviors and skills through
 * repeated practice and reinforcement. These memories become automatic and
 * require less cognitive control once established.
 */
class ProceduralMemory {
public:
    /**
     * @brief Constructs an empty procedural memory
     * @post Procedural memory is initialized but empty
     */
    ProceduralMemory();
    
    /**
     * @brief Destroys the procedural memory
     * @details Cleans up all stored skills and memory management structures
     */
    ~ProceduralMemory();
    
    /**
     * @brief Learns a new skill
     * @details Stores a new skill in procedural memory. If the skill already exists,
     * its neural pattern and proficiency are updated.
     * 
     * @param name Name of the skill to learn
     * @param pattern Neural pattern that activates the skill
     * @post Skill is learned or updated
     */
    void learnSkill(const std::string& name, const std::vector<NeuronId>& pattern);
    
    /**
     * @brief Retrieves a skill from procedural memory
     * @details Looks up a skill by name and returns a pointer to it. Returns null
     * if the skill is not found.
     * 
     * @param name Name of the skill to retrieve
     * @return Skill* Pointer to the skill (null if not found)
     * @post Memory state is unchanged
     */
    Skill* getSkill(const std::string& name);
    
    /**
     * @brief Gets all skills from procedural memory
     * @return std::vector<Skill*> Vector of pointers to all skills
     * @post Memory state is unchanged
     */
    std::vector<Skill*> getAllSkills();
    
    /**
     * @brief Updates a skill's proficiency level
     * @details Adjusts the proficiency of a learned skill by the specified delta,
     * ensuring the value stays within the valid range (0.0-1.0).
     * 
     * @param name Name of the skill to update
     * @param delta Change in proficiency (positive for improvement, negative for decline)
     * @post Skill proficiency is updated
     */
    void updateProficiency(const std::string& name, float delta);
    
    /**
     * @brief Clears all skills from procedural memory
     * @details Removes all learned skills, effectively resetting procedural memory
     * to an empty state.
     * 
     * @post Procedural memory is empty
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class AssociativeMemory
 * @brief Long-term storage of relationships between representations
 * @details Associative memory stores connections between different neural patterns,
 * enabling inference, generalization, and the transfer of knowledge across domains.
 * This memory system supports Hebbian learning and dynamic adjustment of associations.
 */
class AssociativeMemory {
public:
    /**
     * @brief Constructs an empty associative memory
     * @post Associative memory is initialized but empty
     */
    AssociativeMemory();
    
    /**
     * @brief Destroys the associative memory
     * @details Cleans up all stored associations and memory management structures
     */
    ~AssociativeMemory();
    
    /**
     * @brief Creates an association between two neural patterns
     * @details Establishes a connection between two neuron IDs with a specified
     * strength. If the association already exists, its strength is updated.
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @param strength Initial association strength (typically 0.0-1.0)
     * @post Association is created or updated
     */
    void associate(NeuronId a, NeuronId b, float strength);
    
    /**
     * @brief Gets neurons associated with a given neuron
     * @details Retrieves all neurons that have associations with the specified neuron.
     * 
     * @param neuron Neuron ID to find associations for
     * @return std::vector<NeuronId> Vector of associated neuron IDs
     * @post Memory state is unchanged
     */
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    /**
     * @brief Gets the strength of an association between two neurons
     * @details Looks up the connection strength between two neuron IDs. Returns
     * zero if the association doesn't exist.
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @return float Association strength (0.0 if no association)
     * @post Memory state is unchanged
     */
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    /**
     * @brief Updates an association strength
     * @details Modifies the strength of an existing association by adding the specified
     * delta, ensuring the value stays within the valid range (0.0-1.0).
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @param delta Change in association strength
     * @post Association strength is updated
     */
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    /**
     * @brief Clears all associations from associative memory
     * @details Removes all stored connections, effectively resetting associative memory
     * to an empty state.
     * 
     * @post Associative memory is empty
     */
    void clear();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm