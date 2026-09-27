#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * @file SpikeSystem.hpp
 * @brief Header file for the SpikeSystem class implementing spike event management
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the SpikeSystem class definition, which provides an
 * event-driven neural computation system for handling spike event processing
 * and delayed synaptic transmission. The SpikeSystem manages spike queues,
 * delayed spike delivery, and statistics collection for neural activity.
 */

/**
 * @struct DetailedSpikeEvent
 * @brief Represents a spike event with additional metadata
 * @since Version 1.0 (2024)
 *
 * Contains comprehensive information about spike events including source neuron,
 * timing, simulation step, and regional context. Used for spike history tracking
 * and statistical analysis.
 */
struct DetailedSpikeEvent {
    NeuronId source;              ///< Source neuron identifier
    Timestamp timestamp;         ///< Time when spike occurred (seconds)
    SimulationStep step;         ///< Simulation step when spike occurred
    RegionId regionId;            ///< Region containing the source neuron
    PopulationId populationId;    ///< Population containing the source neuron
    
    /**
     * @brief Default constructor
     * @post Initializes all fields to default values
     */
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    
    /**
     * @brief Parameterized constructor
     * @param nid Source neuron identifier
     * @param ts Timestamp of spike event
     * @param s Simulation step
     * @param rid Region identifier
     * @param pid Population identifier
     */
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

/**
 * @class SpikeSystem
 * @brief Event-driven neural computation system for spike processing
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The SpikeSystem provides a high-performance event-driven architecture for
 * managing neural spike events, supporting real-time spike processing, delayed
 * transmission, and comprehensive statistics collection. It serves as the
 * central spike processing unit in the neural framework, coordinating spike
 * generation and delivery across the neural network.
 *
 * Key features:
 * - Efficient spike event queuing and processing
 * - Delayed spike transmission for synaptic timing
 * - Event-driven architecture for high performance
 * - Comprehensive spike statistics and metrics
 * - Callback-based event handling
 * - Integration with neural regions and populations
 *
 * @note This class uses the Pimpl idiom for implementation hiding and
 * efficient compilation. Spike processing is handled through private
 * implementation details.
 *
 * @warning Spike system must be properly initialized before use.
 * Spike processing depends on correct neural network connectivity.
 *
 * @see Brain, Neuron, Synapse, NeuralRegion
 */
class SpikeSystem {
public:
    /**
     * @brief Constructor for the SpikeSystem class
     * @post Creates empty spike system with default configuration
     * @note Spike system is ready for spike processing after construction
     */
    SpikeSystem();
    
    ~SpikeSystem();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Spike systems cannot be copied due to pointer-based internal representation
     * @note Memory management handled through pImpl pattern
     */
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    
    /**
     * @brief Move constructor
     * @param other SpikeSystem object to move from
     * @post This spike system owns other's resources; other is in valid but unspecified state
     * @note Enables efficient transfer of spike system ownership
     */
    SpikeSystem(SpikeSystem&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other SpikeSystem object to move from
     * @return Reference to this spike system
     * @post Current spike system's resources are transferred from other
     * @note Enables efficient transfer of spike system ownership
     */
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event
    /**
     * @brief Queue a spike event for processing
     * @param event Spike event to queue
     * @post Event is added to processing queue
     * @note Immediate processing queue for current step spikes
     */
    void queueSpike(const SpikeEvent& event);
    
    /**
     * @brief Queue a spike event with individual parameters
     * @param neuron Neuron identifier that spiked
     * @param timestamp Time when spike occurred
     * @param step Simulation step when spike occurred
     * @post Spike event is queued for immediate processing
     */
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission)
    /**
     * @brief Queue a delayed spike event for future delivery
     * @param event Delayed spike event to queue
     * @post Event is added to delayed processing queue
     * @note Delayed spikes are processed at specified future times
     */
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current step
    /**
     * @brief Process all queued spikes for the current simulation step
     * @param currentStep Current simulation step number
     * @post All queued spikes are processed and delivered
     * @note Processes immediate spike events for current step
     * @see processDelayedSpikes()
     */
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    /**
     * @brief Process delayed spikes that should be delivered at current time
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (seconds)
     * @post Delayed spikes are processed and delivered according to timing
     * @note Handles synaptic transmission timing and delays
     * @see queueDelayedSpike()
     */
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    /**
     * @brief Register callback function for spike event handling
     * @param handler Callback function to be called when spikes occur
     * @post Handler is registered for spike events
     * @note Callback receives detailed spike event information
     * @see SpikeHandler, registerDelayedHandler()
     */
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    /**
     * @brief Register callback function for delayed spike event handling
     * @param handler Callback function to be called when delayed spikes occur
     * @post Handler is registered for delayed spike events
     * @note Callback receives delayed spike event information
     */
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    /**
     * @brief Get spike event history
     * @return const std::vector<DetailedSpikeEvent>& Vector of spike events
     * @post Returns complete history of all spike events processed
     */
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike event history
     * @post All spike events are removed from history
     * @note Useful for resetting statistics and clearing memory
     */
    void clearHistory();
    
    // Get spike count
    /**
     * @brief Get total number of spikes processed
     * @return size_t Total spike count
     * @post Returns cumulative number of spikes processed
     */
    size_t getSpikeCount() const;
    
    /**
     * @brief Get number of spikes queued for immediate processing
     * @return size_t Number of pending spikes
     * @post Returns count of spikes waiting to be processed
     */
    size_t getPendingSpikeCount() const;
    
    /**
     * @brief Get number of delayed spikes queued for future delivery
     * @return size_t Number of pending delayed spikes
     * @post Returns count of delayed spikes in queue
     */
    size_t getPendingDelayedCount() const;
    
    // Statistics
    /**
     * @brief Get average firing rate across all spikes
     * @return float Average spike rate (spikes per second)
     * @post Returns system-wide average firing rate
     */
    float getAverageSpikeRate() const;
    
    /**
     * @brief Get most active neurons by spike count
     * @param count Number of neurons to return
     * @return std::vector<NeuronId> Vector of neuron identifiers
     * @post Returns list of neurons sorted by spike activity (most active first)
     */
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset
    /**
     * @brief Reset spike system to initial state
     * @post All spike queues and statistics are cleared
     * @note Resets spike counting and history tracking
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm