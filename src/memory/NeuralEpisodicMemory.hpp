#pragma once

/**
 * @file NeuralEpisodicMemory.hpp
 * @brief Episodic memory system using neural dynamics
 * 
 * This header defines the NeuralEpisodicMemory class for the brain's episodic memory system.
 * The neural episodic memory implements episodic memory using neural dynamics and
 * pattern completion, rather than simple storage vectors. It provides the foundation
 * for storing and retrieving autobiographical experiences and event sequences.
 * 
 * Episodic Memory Theory:
 * - Episodic memory stores specific events with temporal and spatial context
 * - Pattern completion enables recall from partial cues
 * - Neural dynamics support temporal sequence representation
 * - Integration with working memory enables memory updating and rehearsal
 * - Context representation supports event differentiation and retrieval
 * 
 * Episodic Memory Functions:
 * - Event storage: Records specific events with rich context
 * - Pattern completion: Retrieves events from partial cues
 * - Temporal sequencing: Maintains event order and timing
 * - Context integration: Links events to environmental context
 * - Consolidation: Transfers information to long-term storage
 * 
 * Integration Points:
 * - Connected to working memory for rehearsal and updating
 * - Interacts with attention system for encoding and retrieval
 * - Modulated by neuromodulation systems (acetylcholine, dopamine)
 * - Used by planning system for context-aware decision making
 * - Coordinated with development system for memory capacity development
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Forward declarations
class NeuralEpisodicMemory;
class AttentionalSelection;
class DevelopmentSystem;

/**
 * @brief Episodic memory event representation
 * 
 * Represents a specific episodic memory event containing the neural pattern
 * that encodes this particular experience or event. Episodic memories are
 * specific in time and context, representing what happened, where, when,
 * and how it felt.
 * 
 * EpisodicEvent Structure:
 * - pattern: Neural activity pattern representing this event
 * - time: Simulation step when event occurred
 * - context: Environmental context of the event
 * - reward: Reward associated with the event
 * - significance: Importance of the event (0.0-1.0)
 * 
 * Event Properties:
 * - Temporal specificity: Events are anchored in time
 * - Contextual richness: Events include environmental context
 * - Perceptual detail: Events encode sensory and perceptual details
 * - Emotional tagging: Events include affective components
 * - Causal linking: Events are linked through temporal and spatial relationships
 */
struct EpisodicEvent {
    /**
     * @brief Neural activity pattern representing this event
     * 
     * This vector contains the neural activation levels that encode this
     * specific episodic memory event. It represents the complete experience
     * including all sensory, contextual, and affective components.
     */
    std::vector<float> pattern;
    
    /**
     * @brief Time when event occurred
     * 
     * Simulation step when this episodic event occurred. Provides temporal
     * anchoring for event ordering and temporal context.
     */
    SimulationStep time;
    
    /**
     * @brief Environmental context of the event
     * 
     * Vector representing the environmental context in which this event
     * occurred. Includes spatial, sensory, and other contextual information.
     */
    std::vector<float> context;
    
    /**
     * @brief Reward associated with the event
     * 
     * Reward value associated with this episodic event. Encodes the affective
     * or motivational significance of the event.
     */
    float reward;
    
    /**
     * @brief Event significance (0.0-1.0)
     * 
     * Significance measure of the event (0.0-1.0). Higher values indicate
     * more important or salient events that are more likely to be retained
     * and recalled.
     */
    float significance;
};

/**
 * @brief Neural episodic memory system for event storage and retrieval
 * 
 * Implements the brain's episodic memory system using neural dynamics and
 * pattern completion. The neural episodic memory stores specific events with
 * rich contextual information and enables retrieval through pattern completion
 * from partial cues. This provides a biologically realistic model of episodic
 * memory that supports autobiographical knowledge and contextual learning.
 * 
 * Neural Episodic Memory Architecture:
 * 1. Encoding: Events are transformed into neural activity patterns
 * 2. Storage: Events are stored with rich contextual information
 * 3. Pattern Completion: Events can be retrieved from partial cues
 * 4. Temporal Integration: Events are linked through temporal sequences
 * 5. Contextual Binding: Events are associated with environmental context
 * 
 * Episodic Memory Dynamics:
 * - Pattern completion: Recall from partial or degraded cues
 * - Temporal sequencing: Maintain and retrieve event sequences
 * - Contextual binding: Link events to environmental context
 * - Memory updating: Update memories based on new information
 * - Consolidation: Strengthen memory traces over time
 * 
 * Thread Safety: NeuralEpisodicMemory is thread-safe for concurrent access
 * from different brain components. Memory operations are protected
 * with mutexes.
 * 
 * Memory Management: NeuralEpisodicMemory uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note NeuralEpisodicMemory is essential for autobiographical memory and
 *       contextual learning. It provides the foundation for remembering
 *       what, where, and when events occurred.
 * @see WorkingMemory for rehearsal and updating
 * @see DevelopmentSystem for developmental aspects of episodic memory
 * @see Attention for encoding and retrieval control
 */
class NeuralEpisodicMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a neural episodic memory system with initial state. The neural
     * episodic memory implementation uses the pImpl pattern for efficient memory usage
     * and to reduce compilation dependencies.
     */
    NeuralEpisodicMemory();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of NeuralEpisodicMemory's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~NeuralEpisodicMemory();
    
    /**
     * @brief Initialize neural episodic memory with brain reference
     * 
     * Initializes the neural episodic memory with reference to the brain,
     * enabling access to brain's attentional system, working memory,
     * and other components for context-aware episodic memory operation.
     * 
     * @param brain Pointer to the brain that contains this episodic memory
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store episodic event
     * 
     * Stores an episodic event with rich contextual information. This records
     * a specific experience or event in the brain's episodic memory system.
     * 
     * Storage Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Transform event into neural activity pattern
     * 3. Store event with context, time, and reward information
     * 4. Apply pattern completion for cue-based retrieval
     * 5. Update temporal sequences and context links
     * 
     * @param event Episodic event to store
     * 
     * @note Thread-safe with mutex protection.
     *       Events are stored with rich contextual information for
     *       later retrieval and pattern completion.
     */
    void storeEvent(const EpisodicEvent& event);
    
    /**
     * @brief Retrieve episodic event by time
     * 
     * Retrieves an episodic event by its occurrence time. This provides
     * access to specific events in the temporal sequence of experiences.
     * 
     * @param time Simulation step time for event retrieval
     * @return Episodic event at specified time, or empty event if not found
     * 
     * @note Thread-safe with mutex protection.
     *       Returns a default-constructed event if no event matches
     *       the specified time.
     */
    EpisodicEvent retrieveEvent(SimulationStep time) const;
    
    /**
     * @brief Retrieve episodic event by pattern (pattern completion)
     * 
     * Retrieves an episodic event through pattern completion from a partial
     * or similar neural activity pattern. This simulates human memory recall
     * from cues and associations.
     * 
     * Pattern Completion Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Find most similar stored event pattern
     * 3. Return stored event (full pattern, not the query)
     * 4. Apply confidence weighting based on similarity
     * 5. Return completed memory
     * 
     * @param pattern Neural activity pattern (partial or complete)
     * @param similarityThreshold Similarity threshold for completion (0.0-1.0)
     * @return Retrieved episodic event, or empty event if no match
     * 
     * @note Thread-safe with mutex protection.
     *       This simulates human memory recall through pattern completion,
     *       where partial cues can trigger full memory retrieval.
     */
    EpisodicEvent retrieveEventByPattern(const std::vector<float>& pattern,
                                        float similarityThreshold = 0.7f) const;
    
    /**
     * @brief Get all episodic events
     * 
     * Returns the complete list of stored episodic events. Order is preserved
     * as event occurrence time.
     * 
     * @return Vector of all stored episodic events
     */
    const std::vector<EpisodicEvent>& getAllEvents() const { return events_; }
    
    /**
     * @brief Clear all episodic events
     * 
     * Removes all episodic events and resets to empty state. This is used to
     * clear episodic memory or when memory should be completely reset.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the entire episodic memory system.
     */
    void clearEvents();
    
    /**
     * @brief Get number of stored events
     * 
     * Returns the total number of episodic events currently stored.
     * 
     * @return Number of stored episodic events
     */
    size_t getEventCount() const { return events_.size(); }
    
    /**
     * @brief Get events within time range
     * 
     * Returns all episodic events that occurred within a specified time range.
     * This is useful for retrieving event sequences or contextual blocks.
     * 
     * @param startTime Start time of range
     * @param endTime End time of range
     * @return Vector of events within time range
     */
    std::vector<EpisodicEvent> getEventsInRange(SimulationStep startTime,
                                               SimulationStep endTime) const;
    
    /**
     * @brief Get current developmental stage multiplier
     * 
     * Returns how the current developmental stage affects episodic memory capacity
     * and processing. Different developmental stages have different episodic memory
     * characteristics and capacities.
     * 
     * @return Developmental stage multiplier for episodic memory
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Get current episodic memory parameters
     * 
     * Returns current episodic memory parameters.
     * 
     * @return Vector containing current parameters [patternCompletionThreshold, significanceThreshold]
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Set episodic memory parameters
     * 
     * Sets internal parameters for episodic memory processing.
     * 
     * @param patternCompletionThreshold Threshold for pattern completion
     * @param significanceThreshold Threshold for event significance
     */
    void setParameters(float patternCompletionThreshold, float significanceThreshold);
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Reference to brain for episodic memory context
     */
    Brain* brain_;
    
    /**
     * @brief Stored episodic events
     */
    std::vector<EpisodicEvent> events_;
    
    /**
     * @brief Pattern completion threshold
     */
    float patternCompletionThreshold_;
    
    /**
     * @brief Event significance threshold
     */
    float significanceThreshold_;
};

} // namespace nlm