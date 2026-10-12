namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(new Impl(timestep)) {}

SimulationClock::~SimulationClock() = default;

SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Validation helper functions
namespace {
    bool validateTimeValue(Timestamp time) {
        // Time values should be non-negative
        return time >= 0.0;
    }
    
    bool validateTimestep(TimestepDuration dt) {
        // Timestep should be positive and reasonable
        // Typical timestep: 0.000001 to 1.0 seconds (1 microsecond to 1 second)
        const TimestepDuration MIN_TIMESTEP = 0.000001; // 1 microsecond
        const TimestepDuration MAX_TIMESTEP = 1.0;      // 1 second
        return dt > 0.0 && dt >= MIN_TIMESTEP && dt <= MAX_TIMESTEP;
    }
    
    bool validateSimulationStep(SimulationStep step) {
        // Simulation step should be non-negative and reasonable
        // Reasonable upper bound: 1e9 steps (about 11.5 days at 1ms timestep)
        const SimulationStep MAX_STEP = 1000000000;
        return step <= MAX_STEP;
    }
} // namespace anonymous

void SimulationClock::reset() {
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

void SimulationClock::advance() {
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    // Validate step before setting
    if (validateSimulationStep(step)) {
        pImpl->step = step;
        pImpl->time = step * pImpl->timestep;
    }
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    // Validate time before setting
    if (validateTimeValue(time)) {
        pImpl->time = time;
        pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
    }
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    // Validate timestep before setting
    if (validateTimestep(dt)) {
        pImpl->timestep = dt;
        // Adjust step if time exceeds new timestep
        pImpl->step = static_cast<SimulationStep>(pImpl->time / dt);
    }
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    // Validate step before conversion
    if (!validateSimulationStep(step)) {
        return -1.0; // Invalid time
    }
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    // Validate time before conversion
    if (!validateTimeValue(time)) {
        return static_cast<SimulationStep>(-1); // Invalid step
    }
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    // Validate steps before advancing
    if (validateSimulationStep(steps)) {
        pImpl->step += steps;
        pImpl->time += steps * pImpl->timestep;
    }
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    // Validate steps before calculation
    if (!validateSimulationStep(stepsAhead)) {
        return -1.0; // Invalid time
    }
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
