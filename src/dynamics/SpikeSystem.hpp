#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <stdexcept>

namespace nlm {

/**
 * @brief Detailed spike event containing metadata
 * 
 * Contains comprehensive information about a neural spike event including
 * source neuron, timing, region and population information for detailed
 * spike analysis and visualization.
 * 
 * This structure extends the basic SpikeEvent with additional metadata
 * useful for spike analysis, visualization, and detailed event processing.
 * 
 * @sa SpikeEvent
 * @sa SpikeSystem
 * 
 * @note This structure is designed for use in event-driven neural simulations
 *       where detailed spike information is required for analysis or visualization.
 */
struct DetailedSpikeEvent {
    NeuronId source;              ///< Source neuron ID that generated the spike
    Timestamp timestamp;          ///< Spike timestamp in simulation time
    SimulationStep step;          ///< Simulation step when spike occurred
    RegionId regionId;            ///< Region where spike originated
    PopulationId populationId;    ///< Population within region
    
    /**
     * @brief Default constructor
     * 
     * Creates a zero-initialized spike event with default values.
     * Useful for creating placeholder events or for zero-initialization patterns.
     */
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param nid Neuron ID that generated the spike
     * @param ts Timestamp when spike occurred
     * @param s Simulation step when spike occurred
     * @param rid Region ID
     * @param pid Population ID
     * 
     * @note This constructor initializes all fields and provides a clear
     *       interface for creating spike events with specific metadata.
     */
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

/**
 * @brief Event-driven neural computation system for spike processing
 * 
 * This class implements a thread-safe, event-driven system for processing
 * neural spikes and delayed synaptic events. It provides a central hub for
 * spike event management, statistics tracking, and handler registration.
 * 
 * Key features:
 * - Thread-safe spike event processing with mutex protection
 * - Efficient spike history management with configurable limits
 * - Statistics tracking per neuron for activity analysis
 * - Handler system for custom spike processing
 * - Delayed spike event support for synaptic transmission
 * - Reset and cleanup functionality
 * 
 * This class is a fundamental component of the NLM brain architecture,
 * providing the core event-driven computation engine for neural simulations.
 * It implements the spike processing pipeline that enables real-time
 * neural computation with event-driven dynamics.
 * 
 * @note This class uses the Pimpl idiom for implementation details hiding
 *       and provides thread-safe operations through mutex protection.
 * 
 * @sa SpikeEvent, DetailedSpikeEvent, DelayedSpikeEvent, SpikeSystem::Impl
 * @sa https://en.wikipedia.org/wiki/Event-driven_simulation
 * @sa Event-driven neural networks
 * 
 * @par Design Principles
 * - Thread Safety: All public methods are thread-safe using mutex protection
 * - RAII: Proper resource management through constructors and destructors
 * - Extensibility: Handler system allows custom spike processing
 * - Efficiency: Optimized for real-time neural simulation performance
 * - Isolation: Pimpl idiom hides implementation details
 * 
 * @par Mathematical Background
 * This system implements event-driven simulation for neural networks,
 * where events (spikes) trigger state updates and synaptic interactions.
 * The time-stepped nature allows for efficient computation compared to
 * traditional time-stepped simulations.
 */
class SpikeSystem {
public:
    /**
     * @brief Default constructor
     * 
     * Initializes the spike system with default configuration
     * and sets up internal synchronization primitives.
     * 
     * @note The constructor initializes the internal implementation
     *       structure and sets up thread synchronization primitives.
     * 
     * @post The spike system is in a valid initialized state
     * 
     * @see SpikeSystem::Impl
     */
    SpikeSystem();
    
    /**
     * @brief Destructor
     * 
     * Cleans up spike system resources and memory management structures.
     * Properly releases all allocated resources and cleans up internal
     * implementation details.
     * 
     * @note This destructor ensures proper cleanup of resources held by
     *       the spike system, including mutexes and allocated memory.
     */
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;                    ///< Copy constructor deleted
    SpikeSystem& operator=(const SpikeSystem&) = delete;       ///< Copy assignment deleted
    SpikeSystem(SpikeSystem&&) noexcept;                       ///< Move constructor
    SpikeSystem& operator=(SpikeSystem&&) noexcept;            ///< Move assignment operator
    
    /**
     * @brief Queue an immediate spike event
     * 
     * Adds a spike event to be processed in the current simulation step.
     * This is the primary interface for spike event generation from neurons.
     * 
     * @param event Spike event to be queued
     * 
     * @throws std::runtime_error if spike system is not properly initialized
     * 
     * @note This method is thread-safe and uses mutex protection for concurrent
     *       access to internal data structures.
     * 
     * @sa queueDelayedSpike()
     * @sa processSpikes()
     * 
     * @par Time Complexity
     * O(1) - Constant time queue insertion
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Example
     * @code
     * SpikeEvent event(neuronId, currentTime, currentStep);
     * spikeSystem->queueSpike(event);
     * @endcode
     */
    void queueSpike(const SpikeEvent& event);
    
    /**
     * @brief Queue an immediate spike event with separate parameters
     * 
     * Convenience overload that constructs a spike event from individual
     * parameters and queues it for processing. This provides a more
     * convenient interface for spike event creation.
     * 
     * @param neuron Source neuron ID
     * @param timestamp Spike timestamp
     * @param step Simulation step
     * 
     * @throws std::runtime_error if spike system is not properly initialized
     * 
     * @note This method is thread-safe and uses mutex protection for concurrent
     *       access to internal data structures.
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Example
     * @code
     * spikeSystem->queueSpike(neuronId, 1.234, 5000);
     * @endcode
     */
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    /**
     * @brief Queue a delayed spike event for synaptic transmission
     * 
     * Schedules a spike event to be delivered at a specific future time
     * for synaptic input delivery to postsynaptic neurons. This is essential
     * for implementing synaptic transmission delays in neural networks.
     * 
     * @param event Delayed spike event with delivery timing information
     * 
     * @throws std::runtime_error if spike system is not properly initialized
     * 
     * @note This method is thread-safe and uses mutex protection for concurrent
     *       access to internal data structures. Delayed spike events are
     *       processed by the processDelayedSpikes() method.
     * 
     * @sa processDelayedSpikes()
     * @sa DelayedSpikeEvent
     * @sa registerDelayedHandler()
     * 
     * @par Time Complexity
     * O(log N) where N is number of delivery steps (using std::unordered_map)
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     */
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    /**
     * @brief Process all queued immediate spike events
     * 
     * Processes all pending spike events scheduled for the current simulation
     * step, including updating statistics, calling handlers, and managing
     * the spike history. This is the core method of the spike processing
     * pipeline.
     * 
     * @param currentStep Current simulation step being processed
     * 
     * @throws std::runtime_error if spike system is not properly initialized
     * 
     * @note This method is thread-safe and uses mutex protection for
     *       concurrent access to internal data structures. It processes
     *       all pending spikes in FIFO order.
     * 
     * @sa queueSpike()
     * @sa registerHandler()
     * @sa processSpikes()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection for all internal operations
     * 
     * @par Algorithm
     * 1. Acquire mutex lock
     * 2. Create local copy of pending spikes
     * 3. Clear pending spikes queue
     * 4. Release mutex lock
     * 5. Process each spike event (outside mutex)
     * 6. Call registered handlers for each spike
     * 7. Update statistics and history
     * 
     * @par Time Complexity
     * O(N + M) where N is number of spikes, M is number of handlers
     */
    void processSpikes(SimulationStep currentStep);
    
    /**
     * @brief Process pending delayed spike events that should be delivered now
     * 
     * Processes all delayed spike events scheduled for the current simulation
     * step, delivering synaptic input to target neurons through registered
     * delayed spike handlers. This implements synaptic transmission with
     * configurable delays.
     * 
     * @param currentStep Current simulation step being processed
     * @param currentTime Current simulation time for precise timing
     * 
     * @throws std::runtime_error if spike system is not properly initialized
     * 
     * @note This method is thread-safe and uses mutex protection for
     *       concurrent access to internal data structures. It processes
     *       all delayed spikes scheduled for the current step and removes
     *       them from the delayed spikes queue.
     * 
     * @sa queueDelayedSpike()
     * @sa registerDelayedHandler()
     * @sa DelayedSpikeEvent
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection for all internal operations
     * 
     * @par Algorithm
     * 1. Acquire mutex lock
     * 2. Find delayed spikes for current step
     * 3. Create local copy of delayed spikes
     * 4. Remove from delayed spikes map
     * 5. Release mutex lock
     * 6. Process each delayed spike (outside mutex)
     * 7. Call registered delayed handlers
     * 
     * @par Time Complexity
     * O(M) where M is number of delayed spikes for current step
     */
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Register a callback handler for spike events
     * 
     * Registers a function to be called when spike events are processed.
     * Multiple handlers can be registered, and they will all be called
     * for each processed spike event. This provides extensibility for
     * custom spike processing and analysis.
     * 
     * @param handler Function to be called for each spike event
     * 
     * @throws std::invalid_argument if handler is null
     * 
     * @note Handlers are called outside the main mutex lock to prevent
     *       blocking of other operations during handler execution.
     * 
     * @sa SpikeHandler
     * @sa processSpikes()
     * @sa registerDelayedHandler()
     * 
     * @par Handler Interface
     * The handler function should have the signature:
     * @code
     * using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
     * @endcode
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection for handler registration
     * 
     * @par Example
     * @code
     * auto myHandler = [](const DetailedSpikeEvent& event) {
     *     // Custom spike processing
     *     std::cout << "Spike from neuron " << event.source.value << std::endl;
     * };
     * spikeSystem->registerHandler(myHandler);
     * @endcode
     */
    void registerHandler(SpikeHandler handler);
    
    /**
     * @brief Register a callback handler for delayed spike events
     * 
     * Registers a function to be called when delayed spike events are
     * delivered for synaptic input. This is used for implementing
     * synaptic transmission mechanisms and delayed neural interactions.
     * 
     * @param handler Function to be called for each delayed spike event
     * 
     * @throws std::invalid_argument if handler is null
     * 
     * @note Handlers are called outside the main mutex lock to prevent
     *       blocking of other operations during handler execution.
     * 
     * @sa DelayedSpikeHandler
     * @sa processDelayedSpikes()
     * @sa registerHandler()
     * 
     * @par Handler Interface
     * The handler function should have the signature:
     * @code
     * using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
     * @endcode
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection for handler registration
     * 
     * @par Example
     * @code
     * auto delayedHandler = [](const DelayedSpikeEvent& event) {
     *     // Implement synaptic transmission
     *     std::cout << "Delayed spike delivery from neuron " 
     *               << event.source_neuron.value << " to " 
     *               << event.destination_neuron.value << std::endl;
     * };
     * spikeSystem->registerDelayedHandler(delayedHandler);
     * @endcode
     */
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    /**
     * @brief Get the complete spike history
     * 
     * Returns a reference to the vector containing all recorded spike events
     * processed since initialization or since the last clearHistory() call.
     * This provides access to the complete history of neural activity for
     * analysis and visualization purposes.
     * 
     * @return Reference to spike history vector
     * 
     * @note This method is thread-safe and returns a copy of the internal
     *       history vector to prevent external modification of the spike
     *       system's internal data structures.
     * 
     * @sa clearHistory()
     * @sa getSpikeCount()
     * @sa getAverageSpikeRate()
     * @sa getMostActiveNeurons()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Memory Considerations
     * The returned vector is a copy to ensure thread safety. For performance-
     * critical applications, consider using iterators or references with
     * proper synchronization.
     */
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history and statistics
     * 
     * Removes all spike events from history and resets spike count
     * statistics. This is useful for experiment reset or memory management
     * in long-running simulations.
     * 
     * @note This method is thread-safe and provides atomic clearing of
     *       both history and statistics. It resets the spike system to a
     *       clean state for the next simulation.
     * 
     * @sa getSpikeHistory()
     * @sa reset()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Complexity
     * O(N) where N is current history size for vector clearing
     */
    void clearHistory();
    
    /**
     * @brief Get total number of spikes processed
     * 
     * @return Total count of all spike events processed since initialization
     * 
     * @note This method is thread-safe and returns an atomic count that
     *       provides cumulative spike statistics across the entire
     *       simulation lifetime.
     * 
     * @sa getSpikeHistory()
     * @sa clearHistory()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Example
     * @code
     * size_t totalSpikes = spikeSystem->getSpikeCount();
     * std::cout << "Total spikes: " << totalSpikes << std::endl;
     * @endcode
     */
    size_t getSpikeCount() const;
    
    /**
     * @brief Get number of pending immediate spike events
     * 
     * @return Count of spike events currently queued for immediate processing
     * 
     * @note This method is thread-safe and returns the current queue size,
     *       which is useful for monitoring system load and performance.
     * 
     * @sa getPendingDelayedCount()
     * @sa getSpikeCount()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Example
     * @code
     * size_t pendingSpikes = spikeSystem->getPendingSpikeCount();
     * if (pendingSpikes > 1000) {
     *     std::cout << "Warning: High spike queue size" << std::endl;
     * }
     * @endcode
     */
    size_t getPendingSpikeCount() const;
    
    /**
     * @brief Get number of pending delayed spike events
     * 
     * @return Count of delayed spike events scheduled for future delivery
     * 
     * @note This method is thread-safe and returns an aggregated count of
     *       all delayed spike events across all delivery times, which is
     *       useful for monitoring synaptic transmission load.
     * 
     * @sa getPendingSpikeCount()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Example
     * @code
     * size_t pendingDelayed = spikeSystem->getPendingDelayedCount();
     * std::cout << "Delayed spikes pending: " << pendingDelayed << std::endl;
     * @endcode
     */
    size_t getPendingDelayedCount() const;
    
    /**
     * @brief Calculate average spike rate from history
     * 
     * Computes the average spike rate over the entire spike history,
     * taking into account the time span between the first and last spike.
     * This provides a measure of overall neural activity in the system.
     * 
     * @return Average spike rate in spikes per unit time, or 0 if insufficient data
     * 
     * @note This method is thread-safe and handles edge cases like
     *       insufficient history data or near-zero duration gracefully.
     * 
     * @sa getSpikeHistory()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Edge Cases
     * Returns 0.0 if:
     * - Less than 2 spikes in history (insufficient data for rate calculation)
     * - Time duration is zero or negative (edge case handling)
     * - History is empty (no spikes recorded)
     * 
     * @par Example
     * @code
     * float averageRate = spikeSystem->getAverageSpikeRate();
     * if (averageRate > 0.0f) {
     *     std::cout << "Average firing rate: " << averageRate << " Hz" << std::endl;
     * }
     * @endcode
     */
    float getAverageSpikeRate() const;
    
    /**
     * @brief Get the most active neurons by spike count
     * 
     * Returns a list of neuron IDs sorted by their spike activity in
     * descending order (most active first). This provides a ranking of
     * neural activity and can be used to identify key neurons in the network.
     * 
     * @param count Maximum number of neurons to return
     * @return Vector of neuron IDs ordered by activity level (most active first)
     * 
     * @note This method is thread-safe and returns a copy of the results
     *       to prevent external modification. The returned vector is sorted
     *       by spike count in descending order, with ties broken by neuron ID.
     * 
     * @sa getSpikeCount()
     * @sa getSpikeHistory()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Complexity
     * O(N log N) where N is number of neurons with recorded activity
     * 
     * @par Example
     * @code
     * size_t topN = 10;
     * auto activeNeurons = spikeSystem->getMostActiveNeurons(topN);
     * std::cout << "Top " << topN << " most active neurons:" << std::endl;
     * for (size_t i = 0; i < activeNeurons.size(); ++i) {
     *     std::cout << "  " << i+1 << ". Neuron " << activeNeurons[i].value << std::endl;
     * }
     * @endcode
     */
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    /**
     * @brief Reset spike system to initial state
     * 
     * Clears all spike events, history, and statistics, returning the
     * system to its initial uninitialized state. This is useful for
     * experiment reset or simulation restart scenarios.
     * 
     * @note This method is thread-safe and provides complete system reset.
     *       It ensures all internal data structures are cleared and the
     *       system can be safely re-initialized.
     * 
     * @sa queueSpike()
     * @sa clearHistory()
     * @sa getSpikeHistory()
     * 
     * @par Thread Safety
     * Thread-safe with mutex protection
     * 
     * @par Reset Behavior
     * Clears:
     * - Pending spike queue
     * - Spike history vector
     * - Per-neuron spike counts
     * - Delayed spike events
     * - Registered handlers (re-creation needed)
     * 
     * @par Example
     * @code
     * // Reset before starting a new experiment
     * spikeSystem->reset();
     * spikeSystem->initialize(); // Re-initialize if needed
     * @endcode
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm