#include <stdexcept>

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

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(std::make_unique<Impl>(timestep)) {}

SimulationClock::~SimulationClock() = default;

// Add safety check in move constructor
SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(std::move(other.pImpl)) {
    // Note: other.pImpl is now nullptr, no need for extra cleanup
}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        // std::unique_ptr handles self-assignment and cleanup
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Safety check macro
#define ASSERT_PIMPL_READY() do { \
    if (!pImpl) { throw std::runtime_error("SimulationClock is not properly initialized"); } \
} while(0)

void SimulationClock::reset() {
    ASSERT_PIMPL_READY();
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

// Add helper function to check pImpl
inline void assertPImplReady() {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock is not properly initialized");
    }
}

void SimulationClock::advance() {
    assertPImplReady();
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

SimulationStep SimulationClock::getStep() const {
    assertPImplReady();
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    assertPImplReady();
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    assertPImplReady();
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    assertPImplReady();
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    assertPImplReady();
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    assertPImplReady();
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    assertPImplReady();
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    assertPImplReady();
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    assertPImplReady();
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    assertPImplReady();
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    assertPImplReady();
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    assertPImplReady();
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed() const {
    assertPImplReady();
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

void SimulationClock::setRealTimeStart() {
    assertPImplReady();
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
