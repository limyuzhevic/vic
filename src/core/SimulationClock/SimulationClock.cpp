#include "SimulationClock.hpp"
#include <chrono>

namespace nlm {

struct SimulationClock::Impl {
    // Implementation details
    SimulationStep step;                    ///< Current simulation step number
    Timestamp time;                        ///< Current simulation time (seconds)
    TimestepDuration timestep;             ///< Duration of each simulation step
    std::chrono::steady_clock::time_point realStart; ///< Start time for real-time mode
    bool realStartSet;                     ///< Whether real-time tracking is initialized
    
    /**
     * Implementation constructor
     * 
     * @param dt Simulation timestep
     */
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

/**
 * Constructor implementation
 * 
 * Creates the internal implementation structure with the specified timestep.
 * This uses the Pimpl idiom to hide implementation details from the header.
 * 
 * @param timestep Simulation timestep (stored in implementation)
 */
SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(new Impl(timestep)) {}

/**
 * Destructor implementation
 * 
 * Cleans up the internal implementation structure.
 * The default destructor is sufficient since the implementation
 * uses standard memory management.
 */
SimulationClock::~SimulationClock() = default;

/**
 * Move constructor implementation
 * 
 * Transfers ownership of the internal implementation from another clock.
 * The source clock is left in a valid but empty state.
 * 
 * @param other Clock to transfer ownership from
 */
SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

/**
 * Move assignment operator implementation
 * 
 * Transfers ownership of the internal implementation from another clock.
 * Properly cleans up current implementation before transfer.
 * 
 * @param other Clock to transfer ownership from
 * @return Reference to this clock
 */
SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

/**
 * Reset implementation
 * 
 * Resets the simulation clock to the initial state:
 * - Step counter set to 0
 * - Simulation time set to 0.0
 * - Real-time tracking reset (if enabled)
 * 
 * @note This is equivalent to resetting the simulation entirely
 */
void SimulationClock::reset() {
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

/**
 * Advance one timestep implementation
 * 
 * Increments the simulation step and updates simulation time by the timestep.
 * This is the primary advancement operation called in simulation loops.
 * 
 * @note Time advances automatically based on the timestep
 * @see getStep()
 * @see getTime()
 */
void SimulationClock::advance() {
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

/**
 * Get step implementation
 * 
 * @return Current simulation step number
 * @note Simple direct access to internal state
 */
SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

/**
 * Set step implementation
 * 
 * Sets both step and time to maintain consistency:
 * - Step set directly
 * - Time calculated as step * timestep
 * 
 * @param step New simulation step number
 * @note This maintains the invariant: time = step * timestep
 */
void SimulationClock::setStep(SimulationStep step) {
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

/**
 * Get time implementation
 * 
 * @return Current simulation time in seconds
 * @note Simple direct access to internal state
 */
Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

/**
 * Set time implementation
 * 
 * Sets both time and step to maintain consistency:
 * - Time set directly
 * - Step calculated as time / timestep (integer division)
 * 
 * @param time New simulation time in seconds
 * @note This maintains the invariant: step = time / timestep
 */
void SimulationClock::setTime(Timestamp time) {
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

/**
 * Get timestep implementation
 * 
 * @return Simulation timestep in seconds
 * @note Simple direct access to internal state
 */
TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

/**
 * Set timestep implementation
 * 
 * Updates the timestep and maintains consistency:
 * - Timestep set directly
 * - Time and step remain unchanged (could be recalculated)
 * 
 * @param dt New simulation timestep
 * @note Changing timestep affects time conversion operations
 */
void SimulationClock::setTimestep(TimestepDuration dt) {
    pImpl->timestep = dt;
}

/**
 * Check first step implementation
 * 
 * @return true if current step is 0
 * @note Step 0 represents the initial state
 */
bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

/**
 * Check last step implementation
 * 
 * @param totalSteps Total number of steps in simulation
 * @return true if current step >= totalSteps
 * @note Used to detect simulation completion
 */
bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

/**
 * Convert step to time implementation
 * 
 * @param step Simulation step number
 * @return Simulation time = step * timestep
 * @note Fast computation: simple multiplication
 */
Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return step * pImpl->timestep;
}

/**
 * Convert time to step implementation
 * 
 * @param time Simulation time in seconds
 * @return Nearest simulation step (integer division)
 * @note Fast computation: simple division and cast
 */
SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

/**
 * Advance by multiple steps implementation
 * 
 * Efficiently advances the clock by multiple steps in one operation.
 * More efficient than repeated advance() calls.
 * 
 * @param steps Number of steps to advance (must be >= 0)
 * @note Maintains consistency: step += steps, time += steps * timestep
 */
void SimulationClock::advanceBy(SimulationStep steps) {
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

/**
 * Get time at future step implementation
 * 
 * @param stepsAhead Number of steps ahead (can be negative)
 * @return Simulation time at future step
 * @note Fast computation: direct arithmetic
 */
Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

/**
 * Get real-time elapsed implementation
 * 
 * Returns the wall-clock time elapsed since real-time mode started.
 * Returns 0.0 if real-time mode is not enabled.
 * 
 * @return Wall-clock time elapsed in seconds (0.0 if not enabled)
 * @note Uses high-resolution steady clock for accurate timing
 */
double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

/**
 * Set real-time start implementation
 * 
 * Initializes real-time tracking using a high-resolution monotonic clock.
 * This enables synchronization with real-time execution for performance monitoring
 * or real-time simulations.
 * 
 * @note Real-time mode is optional and disabled by default
 * @see getRealTimeElapsed()
 */
void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
