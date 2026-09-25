#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <memory>
#include <mutex>

/**
 * @file SpikeSystem.hpp
 * @brief Event-driven neural computation system for spike processing and delayed transmission
 *
 * This file contains the definition of the SpikeSystem class, which implements an
 * event-driven neural computation system for real-time spike processing and delayed
 * synaptic transmission. The spike system provides essential functionality for
 * implementing realistic neural dynamics with precise spike timing and temporal
 * processing capabilities.
 *
 * The SpikeSystem class provides:
 * - Event-driven spike event queuing and processing
 * - Delayed synaptic transmission for temporal dynamics
 * - Spike history tracking for analysis and computation
 * - Handler callback system for spike event processing
 * - Statistical analysis of neural activity
 * - Integration with neuron and synapse systems
 *
 * This implementation is designed for:
 * - Real-time neural simulation with event-driven dynamics
 * - Precise spike timing and temporal processing
 * - Large-scale neural network computation
 * - Integration with plasticity and learning systems
 * - Analysis and monitoring of neural activity
 *
 * @note The SpikeSystem uses an efficient event-driven architecture with:
 * - Priority queues for spike event ordering
 * - Delayed spike event handling for synaptic transmission
 * - Callback-based event processing for extensibility
 * - Thread-safe operations for concurrent access
 * - Memory-efficient storage for spike history
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission

namespace nlm {

/**
 * @struct DetailedSpikeEvent
 * @brief Complete spike event with metadata for neural computation
 *
 * A DetailedSpikeEvent contains comprehensive information about a spike event,
 * including the generating neuron, timing, and contextual metadata for use
 * in neural computation and analysis. This structure provides complete spike
 * event information for advanced neural simulation and processing.
 *
 * DetailedSpikeEvent includes:
 * - Source neuron identifier for event identification
 * - Precise spike timestamp for timing analysis
 * - Simulation step for temporal synchronization
 * - Region and population context for distributed processing
 * - Complete event metadata for advanced processing
 *
 * @note DetailedSpikeEvent is used for:
 * - Comprehensive spike event analysis
 * - STDP and spike-timing dependent plasticity
 * - Neural activity statistics and monitoring
 * - Event-driven neural computation
 * - Integration with higher-level neural systems
 *
 * @see SpikeSystem
 * @see SpikeEvent
 * @see DetailedSpikeEvent
 */
struct DetailedSpikeEvent {
    /** Unique identifier of the neuron that generated the spike */
    NeuronId source;
    /** Precise timestamp of the spike event (simulation time) */
    Timestamp timestamp;
    /** Simulation step when the spike occurred */
    SimulationStep step;
    /** Neural region identifier for distributed processing */
    RegionId regionId;
    /** Neural population identifier for population-specific processing */
    PopulationId populationId;
    
    /**
     * @brief Default constructor for DetailedSpikeEvent
     * @note Creates an empty spike event with default values
     */
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    
    /**
     * @brief Parameterized constructor for DetailedSpikeEvent
     *
     * @param nid Neuron identifier that generated the spike
     * @param ts Precise timestamp of the spike event
     * @param s Simulation step when the spike occurred
     * @param rid Neural region identifier
     * @param pid Neural population identifier
     *
     * @note The constructor initializes all event metadata for:
     * - Event identification (neuron source)
     * - Temporal precision (timestamp and step)
     * - Contextual information (region and population)
     *
     * @see SpikeSystem::queueSpike
     * @see SpikeSystem::processSpikes
     */
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

/**
 * @class SpikeSystem
 * @brief Event-driven neural computation system for spike processing and delayed transmission
 *
 * The SpikeSystem class serves as the central coordinator for neural spike events,
 * implementing real-time spike event processing with precise temporal dynamics.
 * It provides essential functionality for neural simulation including spike event
 * queuing, delayed transmission, and comprehensive spike history tracking.
 *
 * The spike system is designed for:
 * - Event-driven neural computation with precise timing
 * - Delayed synaptic transmission for temporal dynamics
 * - Efficient spike event processing for large-scale networks
 * - Integration with neuron and synapse systems
 * - Comprehensive neural activity analysis and monitoring
 *
 * Key features:
 * - Real-time spike event queuing and processing
 * - Priority-based event ordering for temporal accuracy
 * - Delayed spike event handling for synaptic transmission
 * - Callback-based event processing for extensibility
 * - Thread-safe operations for concurrent access
 * - Memory-efficient spike history storage
 * - Statistical analysis of neural activity
 *
 * @note The SpikeSystem uses an efficient architecture with:
 * - Internal implementation structure (PImpl pattern)
 * - Thread-safe operations for concurrent access
 * - Memory-efficient storage for large-scale networks
 * - Extensible callback system for custom processing
 * - Comprehensive error handling and recovery
 *
 * @warning SpikeSystem operations require careful parameter validation to prevent
 *          numerical instability or unrealistic spike timing. Always validate
 *          spike event parameters before use. See NLMErrors for available error codes.
 *
 * @see Neuron
 * @see Synapse
 * @see DetailedSpikeEvent
 * @see NLMErrors
 *
 * @todo Add support for spike event filtering based on neuron type
 * @todo Implement spike event priority system
 * @todo Add support for spike event replication for distributed processing
 *
 * Examples:
 * @code
 * // Create spike system
 * SpikeSystem spikeSystem;
 *
 * // Queue a spike event
 * spikeSystem.queueSpike(NeuronId(1), 1000.0, 0);
 *
 * // Register spike handler
 * spikeSystem.registerHandler([](const DetailedSpikeEvent& event) {
 *     // Process spike event
 *     std::cout << "Spike from neuron " << event.source.value << " at time " << event.timestamp << std::endl;
 * });
 *
 * // Process spikes for current step
 * spikeSystem.processSpikes(0);
 *
 * // Get spike statistics
 * size_t spikeCount = spikeSystem.getSpikeCount();
 * float spikeRate = spikeSystem.getAverageSpikeRate();
 * @endcode
 */
class SpikeSystem {
public:
    /**
     * @brief Constructor implementation for the SpikeSystem class
     *
     * Creates a new spike system with default settings. The constructor
     * initializes the internal implementation structure and sets up spike
     * event processing infrastructure.
     *
     * @throws NLMErrors::MemoryAllocation if memory allocation fails
     *
     * @note The constructor implements:
     * - Internal implementation structure initialization
     * - Spike event queue setup
     * - Delayed spike event queue initialization
     * - Handler registration system setup
     * - Event processing infrastructure initialization
     *
     * @see queueSpike
     * @see processSpikes
     * @see registerHandler
     */
    SpikeSystem();
    
    /**
     * @brief Destructor implementation for the SpikeSystem class
     *
     * Cleans up the spike system by destroying the internal implementation
     * structure. The destructor ensures proper cleanup of all spike event
     * processing resources while maintaining exception safety.
     *
     * @note The destructor is defaulted to avoid unnecessary code and ensure that
     *       all unique_ptr smart pointers properly clean up their implementation
     *       structures when the SpikeSystem object is destroyed.
     */
    ~SpikeSystem();
    
    /**
     * @brief Move constructor for the SpikeSystem class
     *
     * Transfers ownership of the internal implementation structure from the source
     * SpikeSystem object to this new SpikeSystem object. The source object's
     * implementation pointers are set to nullptr to prevent double deletion.
     *
     * @param other The source SpikeSystem object to move from
     * @note This operation provides exception safety and efficient transfer of
     *       ownership of all spike system components and resources.
     */
    SpikeSystem(SpikeSystem&& other) noexcept;
    
    /**
     * @brief Move assignment operator for the SpikeSystem class
     *
     * Transfers ownership of the internal implementation structure from the source
     * SpikeSystem object to this SpikeSystem object. Properly handles self-assignment
     * and ensures safe resource transfer.
     *
     * @param other The source SpikeSystem object to move from
     * @return Reference to this SpikeSystem object after assignment
     * @note This operation provides strong exception safety and efficient transfer
     *       of ownership while properly handling self-assignment cases.
     */
    SpikeSystem& operator=(SpikeSystem&& other) noexcept;
    
    /**
     * @brief Queue a simple spike event
     *
     * Queues a spike event for processing in the next simulation step. This method
     * creates a basic spike event with minimal metadata for standard spike processing.
     *
     * @param event Spike event to queue (contains neuron ID, timestamp, and step)
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if event parameters are invalid
     *
     * @note Spike event queuing includes:
     * - Event validation and parameter checking
     * - Queue insertion with priority ordering
     * - Event metadata preservation
     * - Statistics updating
     *
     * @see queueSpike(NeuronId, Timestamp, SimulationStep)
     * @see processSpikes
     * @see getPendingSpikeCount
     */
    void queueSpike(const SpikeEvent& event);
    
    /**
     * @brief Queue a spike event with separate parameters
     *
     * Queues a spike event for processing using separate parameters. This method
     * provides an alternative interface for spike event queuing with individual
     * parameters for convenience and flexibility.
     *
     * @param neuron Neuron that generated the spike
     * @param timestamp Precise spike timestamp
     * @param step Simulation step when the spike occurred
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if parameters are invalid
     *
     * @note Separate parameter queuing includes:
     * - Individual parameter validation
     * - Event creation and metadata assembly
     * - Queue insertion with priority ordering
     * - Statistics updating
     *
     * @see queueSpike(const SpikeEvent&)
     * @see processSpikes
     */
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    /**
     * @brief Queue a delayed spike event for synaptic transmission
     *
     * Queues a spike event that should be delivered after a delay, simulating
     * synaptic transmission delays in neural circuits. This method is essential
     * for implementing realistic temporal dynamics in neural computation.
     *
     * @param event Delayed spike event to queue (includes delay and target neuron)
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if event parameters are invalid
     *
     * @note Delayed spike event queuing includes:
     * - Delay validation and parameter checking
     * - Event creation with timing metadata
     * - Delayed event queue insertion
     * - Statistics updating
     *
     * @see processDelayedSpikes
     * @see queueSpike
     * @param event Delayed spike event to queue (includes delay and target neuron)
     */
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    /**
     * @brief Process all queued spikes for the current simulation step
     *
     * Processes all spike events queued for the current simulation step, delivering
     * them to registered handlers and updating spike system state. This is the
     * main spike processing method for event-driven neural computation.
     *
     * @param currentStep The current simulation step
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::IntegrationFailed if spike processing fails
     *
     * @note Spike processing includes:
     * - Event retrieval from spike event queue
     * - Detailed spike event creation with metadata
     * - Handler callback invocation
     * - Event statistics updating
     * - State management for spike system
     *
     * @see queueSpike
     * @see getSpikeHandlerCount
     * @see getSpikeCount
     */
    void processSpikes(SimulationStep currentStep);
    
    /**
     * @brief Process pending delayed spike events due for delivery
     *
     * Processes delayed spike events that should be delivered at the current simulation
     * time, simulating synaptic transmission delays and temporal dynamics in neural computation.
     *
     * @param currentStep The current simulation step
     * @param currentTime The current simulation time (in milliseconds)
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::IntegrationFailed if delayed spike processing fails
     *
     * @note Delayed spike processing includes:
     * - Event retrieval from delayed spike event queue
     * - Time-based event delivery validation
     * - Detailed spike event creation
     * - Handler callback invocation
     * - Temporal dynamics implementation
     *
     * @see queueDelayedSpike
     * @param currentStep The current simulation step
     * @param currentTime The current simulation time (in milliseconds)
     */
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Register a spike handler callback
     *
     * Registers a callback function that will be invoked when spike events are
     * processed. This method enables custom spike event processing and integration
     * with higher-level neural systems.
     *
     * @param handler Function to call when spike events are processed
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if handler is invalid
     *
     * @note Handler registration includes:
     * - Callback validation and registration
     * - Handler count updating
     * - Event processing integration
     * - Error handling and recovery
     *
     * @see registerDelayedHandler
     * @see removeHandler
     * @see getSpikeHandlerCount
     */
    void registerHandler(SpikeHandler handler);
    
    /**
     * @brief Register a delayed spike handler callback
     *
     * Registers a callback function that will be invoked when delayed spike events
     * are delivered. This method enables custom delayed spike event processing and
     * integration with temporal dynamics systems.
     *
     * @param handler Function to call when delayed spike events are delivered
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if handler is invalid
     *
     * @note Handler registration includes:
     * - Callback validation and registration
     * - Handler count updating
     * @param handler Function to call when delayed spike events are delivered
     */
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    /**
     * @brief Remove a spike handler by index
     *
     * Removes a spike handler from the handler list using its zero-based index.
     * This method is used for dynamic handler management and cleanup.
     *
     * @param index Zero-based index of the handler to remove
     * @return true if the handler was successfully removed, false if index was invalid
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Handler removal includes:
     * - Handler validation and removal
     * - Handler list updating
     * - Handler count decrementing
     * - Error reporting for invalid operations
     *
     * @see removeDelayedHandler
     * @param index Zero-based index of the handler to remove
     */
    bool removeHandler(size_t index);
    
    /**
     * @brief Remove a delayed spike handler by index
     *
     * Removes a delayed spike handler from the handler list using its zero-based index.
     * This method is used for dynamic handler management and cleanup of temporal processing.
     *
     * @param index Zero-based index of the delayed handler to remove
     * @return true if the handler was successfully removed, false if index was invalid
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Handler removal includes:
     * - Handler validation and removal
     * - Handler list updating
     * - Handler count decrementing
     * - Error reporting for invalid operations
     *
     * @see removeHandler
     * @param index Zero-based index of the delayed handler to remove
     */
    bool removeDelayedHandler(size_t index);
    
    /**
     * @brief Get the number of registered spike handlers
     *
     * Returns the number of spike handlers currently registered for spike event processing.
     * This method is useful for monitoring handler registration and managing system resources.
     *
     * @return size_t Number of registered spike handlers
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Handler counting includes:
     * - Current handler list size retrieval
     * - Resource management planning
     * - Performance monitoring
     * - System health assessment
     *
     * @see registerHandler
     * @see removeHandler
     */
    size_t getSpikeHandlerCount() const;
    
    /**
     * @brief Get the number of registered delayed spike handlers
     *
     * Returns the number of delayed spike handlers currently registered for delayed spike event processing.
     * This method is useful for monitoring temporal processing resources and managing system resources.
     *
     * @return size_t Number of registered delayed spike handlers
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Handler counting includes:
     * - Current delayed handler list size retrieval
     * - Resource management planning
     * - Performance monitoring
     * - Temporal processing assessment
     *
     * @see registerDelayedHandler
     * @see removeDelayedHandler
     */
    size_t getDelayedSpikeHandlerCount() const;
    
    /**
     * @brief Get the complete spike history
     *
     * Returns a constant reference to the spike history vector containing all spike
     * events that have been processed by the spike system. This provides complete
     * access to spike event history for analysis and computation.
     *
     * @return const std::vector<DetailedSpikeEvent>& Constant reference to spike history
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Spike history includes:
     * - All spike events processed by the system
     * - Complete event metadata for each spike
     * - Temporal ordering of spike events
     * - Event statistics and metrics
     *
     * @see clearHistory
     * @see getSpikeCount
     * @see getAverageSpikeRate
     */
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     *
     * Removes all spike events from the spike history, resetting the history to an
     * empty state. This method is used for history management, debugging, and
     * system reset operations.
     *
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note History clearing includes:
     * - Complete removal of all spike events
     * - History vector reset
     * - Statistics reset
     * - Memory management
     *
     * @see getSpikeHistory
     * @see getSpikeCount
     */
    void clearHistory();
    
    /**
     * @brief Get the total number of spikes processed
     *
     * Returns the cumulative count of spikes that have been processed by the spike system
     * since the last reset or initialization. This is a fundamental metric for
     * quantifying neural activity and computational output.
     *
     * @return size_t Total number of spikes processed
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Total spike count includes:
     * - All spikes processed by the system
     * - Both regular and delayed spikes
     * - Spikes from all registered handlers
     * - Statistical accumulation
     *
     * @see getSpikeHistory
     * @see getPendingSpikeCount
     * @see getAverageSpikeRate
     */
    size_t getSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     *
     * Returns the number of spike events currently queued for processing but not yet
     * processed. This indicates the current processing load and queue size.
     *
     * @return size_t Number of pending spike events
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Pending spike count is important for:
     * - Assessing computational load
     * - Monitoring system performance
     * - Queue management
     * - Resource allocation
     *
     * @see getSpikeCount
     * @see processSpikes
     */
    size_t getPendingSpikeCount() const;
    
    /**
     * @brief Get the number of pending delayed spike events
     *
     * Returns the number of delayed spike events currently queued for delivery but not yet
     * processed. This indicates the current temporal processing load.
     *
     * @return size_t Number of pending delayed spike events
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Pending delayed spike count is important for:
     * - Assessing temporal processing load
     * - Monitoring system performance
     * - Temporal queue management
     * - Resource allocation for delayed processing
     *
     * @see getPendingSpikeCount
     * @see processDelayedSpikes
     */
    size_t getPendingDelayedCount() const;
    
    /**
     * @brief Get the average spike rate
     *
     * Computes and returns the average firing rate across all spikes in the spike history.
     * This is a fundamental metric for understanding neural computation and activity.
     *
     * @return float Average spike rate (Hz)
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Average spike rate is calculated as:
     * total spikes / total time span of spike history
     *
     * @note Average spike rate is used for:
     * - Understanding neural computation
     * - Assessing brain activity
     * - Evaluating learning progress
     * - Performance analysis
     *
     * @see getSpikeCount
     * @see getSpikeHistory
     * @see getMostActiveNeurons
     */
    float getAverageSpikeRate() const;
    
    /**
     * @brief Get the most active neurons
     *
     * Returns a vector of neuron identifiers for the most active neurons based on spike
     * history. This is useful for identifying key neurons and analyzing neural activity patterns.
     *
     * @param count Number of most active neurons to return
     * @return std::vector<NeuronId> Vector of neuron identifiers for most active neurons
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     * @throws NLMErrors::InvalidParameter if count is zero or invalid
     *
     * @note Most active neuron identification includes:
     * - Spike count analysis
     * - Ranking neurons by activity level
     * - Returning top N neurons by spike count
     * - Performance bottleneck identification
     *
     * @see getSpikeHistory
     * @see getSpikeCount
     * @see getAverageSpikeRate
     */
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    /**
     * @brief Reset the spike system
     *
     * Resets the spike system to its initial state, clearing all spike events, history,
     * and processing state. This is used for system reset, clearing state, or preparing
     * for new computational tasks.
     *
     * @throws NLMErrors::SystemNotInitialized if the SpikeSystem object is not properly initialized
     *
     * @note Reset includes:
     * - Clearing spike event queues
     * - Clearing spike history
     * - Resetting processing state
     * - Clearing handler registrations
     * - Logging for reset tracking
     *
     * @see queueSpike
     * @see processSpikes
     * @see clearHistory
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};
