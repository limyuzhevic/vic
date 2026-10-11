#include "SimulationClock.hpp"
#include <chrono>

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;                    // Current simulation step number
    Timestamp time;                          // Current simulation time (seconds)
    TimestepDuration timestep;              // Time increment per step (seconds)
    std::chrono::steady_clock::time_point realStart;  // Wall-clock start time for real-time mode
    bool realStartSet;                       // Flag indicating if real-time start is set
    
    // Constructor initializes with given timestep
    // Parameters: dt - Initial timestep for the simulation clock
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

// Constructor implementation
// Parameters: timestep - Time increment per simulation step
SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(new Impl(timestep)) {}

// Default destructor
SimulationClock::~SimulationClock() = default;

// Move constructor implementation
// Safely transfers ownership of internal implementation
SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;  // Prevent double deletion
}

// Move assignment operator implementation
// Transfers ownership from other to this object
SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        delete pImpl;               // Clean up current implementation
        pImpl = other.pImpl;        // Take ownership
        other.pImpl = nullptr;      // Prevent double deletion
    }
    return *this;
}

// Reset implementation
// Restores clock to initial state
void SimulationClock::reset() {
    pImpl->step = 0;                // Reset step counter to 0
    pImpl->time = 0.0;              // Reset simulation time to 0
    pImpl->realStartSet = false;    // Clear real-time start flag
}

// Advance implementation
// Progress simulation by one timestep
void SimulationClock::advance() {
    ++pImpl->step;                          // Increment step counter
    pImpl->time += pImpl->timestep;        // Advance simulation time
}

// Get step implementation
// Returns current step number without modification
SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

// Set step implementation
// Sets step number and calculates corresponding time
// Parameters: step - New step number to set
void SimulationClock::setStep(SimulationStep step) {
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;  // Calculate time from step
}

// Get time implementation
// Returns current simulation time without modification
Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

// Set time implementation
// Sets simulation time and calculates corresponding step number
// Parameters: time - New simulation time (seconds)
void SimulationClock::setTime(Timestamp time) {
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

// Get timestep implementation
// Returns current timestep without modification
TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

// Set timestep implementation
// Changes timestep for future operations
// Parameters: dt - New timestep (seconds)
void SimulationClock::setTimestep(TimestepDuration dt) {
    pImpl->timestep = dt;
}

// Is first step implementation
// Checks if current step is the initial step (step 0)
bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

// Is last step implementation
// Checks if current step has reached or passed total steps
// Parameters: totalSteps - Total number of steps in simulation
bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

// Step to time implementation
// Converts simulation step to corresponding simulation time
// Parameters: step - Step number to convert
Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return step * pImpl->timestep;
}

// Time to step implementation
// Converts simulation time to nearest step number
// Parameters: time - Simulation time to convert
SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

// Advance by multiple steps implementation
// Efficiently advances time by multiple timesteps
// Parameters: steps - Number of steps to advance
void SimulationClock::advanceBy(SimulationStep steps) {
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

// Time at future step implementation
// Calculates time at a future step without modifying current state
// Parameters: stepsAhead - Number of steps to look ahead
Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

// Get real time elapsed implementation
// Returns wall-clock time elapsed since real-time mode started
// Returns 0.0 if real-time mode has not been started
// Used for controlling simulation speed in real-time visualization
// Performance considerations: System clock may have limited resolution
// Note: Uses steady_clock for monotonic time, avoiding system clock adjustments
double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realStartSet) {
        return 0.0;  // Not in real-time mode
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

// Set real time start implementation
// Initializes wall-clock timer for real-time mode
// Should be called once at the beginning of real-time simulation
// Note: Uses steady_clock for system-independent time measurements
// Steady clock is immune to system clock adjustments (NTP, DST, etc.)
void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
