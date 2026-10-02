// Brain Spike Manager - Handles spike system operations
// Separates spike system coordination from the main Brain class

#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

/**
 * @brief Manages spike system operations and event-driven dynamics
 * 
 * This class separates the spike system coordination logic from the main Brain class,
 * making it more modular and testable. It handles:
 * - Spike processing for immediate effects
 * - Delayed spike event processing
 * - Statistics collection and monitoring
 * - Spike system reset and initialization
 * 
 * @note This is a key part of the refactoring effort to reduce Brain.cpp size
 * and improve code organization by using the manager pattern. It encapsulates
 * all spike system-specific logic in a dedicated class.
 */
class BrainSpikeManager {
public:
    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the Brain instance to manage
     */
    explicit BrainSpikeManager(std::shared_ptr<Brain> brain)
        : brain_(brain) {}
    
    /**
     * @brief Destructor
     */
    ~BrainSpikeManager() = default;
    
    /**
     * @brief Process spikes for the current simulation step
     * 
     * Processes immediate spike events and applies their effects on neural dynamics.
     * 
     * @param step Current simulation step
     */
    void processSpikes(SimulationStep step);
    
    /**
     * @brief Process delayed spike events
     * 
     * Processes spike events that are scheduled for delivery at future time steps,
     * delivering synaptic input with appropriate delays.
     * 
     * @param step Current simulation step
     * @param time Current simulation time
     */
    void processDelayedSpikes(SimulationStep step, Timestamp time);
    
    /**
     * @brief Get spike system pointer
     * 
     * Returns a pointer to the spike system managed by this class.
     * 
     * @return SpikeSystem* Pointer to the spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system pointer (const version)
     * 
     * Returns a const pointer to the spike system managed by this class.
     * 
     * @return const SpikeSystem* Pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get spike statistics for the current step
     * 
     * Returns the number of spikes generated in the current simulation step.
     * 
     * @return size_t Number of spikes in current step
     */
    size_t getTotalSpikesThisStep() const;
    
    /**
     * @brief Get total spike count across all simulation steps
     * 
     * Returns the cumulative number of spikes generated throughout the simulation.
     * 
     * @return size_t Total spike count
     */
    size_t getTotalSpikesTotal() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events that are pending delivery (both immediate
     * and delayed).
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Reset the spike system
     * 
     * Resets the spike system to its initial state, clearing all pending events
     * and statistics.
     */
    void reset();
    
private:
    /**
     * @brief Shared pointer to the Brain instance being managed
     */
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
