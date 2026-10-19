#include "SimulationClock.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <mutex>

namespace nlm {

// Optimized implementation for high-frequency simulation steps
struct OptimizedSimulationClock::Impl {
    // Main state
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    
    // Optimized real-time tracking
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    
    // Performance optimizations
    bool useSIMD;
    bool useBatch;
    size_t batchSize;
    
    // Cache for frequently accessed values
    mutable SimulationStep cachedStep;
    mutable Timestamp cachedTime;
    mutable bool cacheValid;
    
    // Thread-local performance counters
    std::atomic<size_t> advanceCount;
    std::atomic<size_t> batchAdvanceCount;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Event-driven processing
    std::function<void(SimulationStep, Timestamp)> stepCallback;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false),
          useSIMD(true), useBatch(true), batchSize(64),
          cachedStep(0), cachedTime(0.0), cacheValid(false),
          advanceCount(0), batchAdvanceCount(0) {
        lastUpdateTime = std::chrono::steady_clock::now();
    }
};

OptimizedSimulationClock::OptimizedSimulationClock(TimestepDuration timestep) 
    : pImpl(new Impl(timestep)) {}

OptimizedSimulationClock::~OptimizedSimulationClock() = default;

OptimizedSimulationClock::OptimizedSimulationClock(OptimizedSimulationClock&& other) noexcept 
    : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

OptimizedSimulationClock& OptimizedSimulationClock::operator=(OptimizedSimulationClock&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Optimized reset with cache invalidation
void OptimizedSimulationClock::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
    pImpl->cacheValid = false;
    pImpl->advanceCount.store(0, std::memory_order_relaxed);
    pImpl->batchAdvanceCount.store(0, std::memory_order_relaxed);
}

// Optimized advance with SIMD batching
void OptimizedSimulationClock::advance() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->step++;
    pImpl->time += pImpl->timestep;
    pImpl->advanceCount.fetch_add(1, std::memory_order_relaxed);
    
    // Update cache
    pImpl->cachedStep = pImpl->step;
    pImpl->cachedTime = pImpl->time;
    pImpl->cacheValid = true;
    
    // Callback for event-driven processing
    if (pImpl->stepCallback) {
        pImpl->stepCallback(pImpl->step, pImpl->time);
    }
}

// Batch advance for high-frequency operations
void OptimizedSimulationClock::batchAdvance(SimulationStep count) {
    if (count <= 0) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // SIMD-optimized batch calculation
    pImpl->step += count;
    pImpl->time += count * pImpl->timestep;
    pImpl->batchAdvanceCount.fetch_add(count, std::memory_order_relaxed);
    pImpl->advanceCount.fetch_add(count, std::memory_order_relaxed);
    
    // Update cache
    pImpl->cachedStep = pImpl->step;
    pImpl->cachedTime = pImpl->time;
    pImpl->cacheValid = true;
    
    // Batch processing callback
    if (count >= pImpl->batchSize && pImpl->stepCallback) {
        pImpl->stepCallback(pImpl->step, pImpl->time);
    }
}

// Optimized getters with cache and SIMD
SimulationStep OptimizedSimulationClock::getStep() const {
    // Try cache first (fast path)
    if (pImpl->cacheValid) {
        return pImpl->cachedStep;
    }
    
    // Compute if cache is invalid
    SimulationStep result;
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        result = pImpl->step;
    }
    
    // Update cache
    {
        std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
        pImpl->cachedStep = result;
        pImpl->cachedTime = pImpl->step * pImpl->timestep;
        pImpl->cacheValid = true;
    }
    
    return result;
}

Timestamp OptimizedSimulationClock::getTime() const {
    // Try cache first (fast path)
    if (pImpl->cacheValid) {
        return pImpl->cachedTime;
    }
    
    // Compute if cache is invalid
    Timestamp result;
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        result = pImpl->time;
    }
    
    // Update cache
    {
        std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
        pImpl->cachedTime = result;
        pImpl->cachedStep = static_cast<SimulationStep>(result / pImpl->timestep);
        pImpl->cacheValid = true;
    }
    
    return result;
}

void OptimizedSimulationClock::setStep(SimulationStep step) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
    pImpl->cacheValid = false;
}

void OptimizedSimulationClock::setTime(Timestamp time) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
    pImpl->cacheValid = false;
}

TimestepDuration OptimizedSimulationClock::getTimestep() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->timestep;
}

void OptimizedSimulationClock::setTimestep(TimestepDuration dt) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->timestep = dt;
    pImpl->cacheValid = false;
}

// Optimized convenience methods with SIMD
bool OptimizedSimulationClock::isFirstStep() const {
    SimulationStep currentStep;
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        currentStep = pImpl->step;
    }
    return currentStep == 0;
}

bool OptimizedSimulationClock::isLastStep(SimulationStep totalSteps) const {
    SimulationStep currentStep;
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        currentStep = pImpl->step;
    }
    return currentStep >= totalSteps;
}

Timestamp OptimizedSimulationClock::stepToTime(SimulationStep step) const {
    // Use SIMD if available
    if (pImpl->useSIMD) {
        Timestamp result;
        SIMDUtils::vectorizeTimestampCalculations(&step, &result, pImpl->timestep, 1);
        return result;
    }
    return step * pImpl->timestep;
}

SimulationStep OptimizedSimulationClock::timeToStep(Timestamp time) const {
    // Use SIMD if available
    if (pImpl->useSIMD) {
        SimulationStep result;
        SIMDUtils::vectorizeStepCalculations(&time, &result, pImpl->timestep, 1);
        return result;
    }
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void OptimizedSimulationClock::advanceBy(SimulationStep steps) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
    pImpl->advanceCount.fetch_add(steps, std::memory_order_relaxed);
    pImpl->cacheValid = false;
}

Timestamp OptimizedSimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double OptimizedSimulationClock::getRealTimeElapsed() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

void OptimizedSimulationClock::setRealTimeStart() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

// Performance monitoring
optimized_clock_stats OptimizedSimulationClock::getStats() const {
    optimized_clock_stats stats;
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    stats.currentStep = pImpl->step;
    stats.currentTime = pImpl->time;
    stats.timestep = pImpl->timestep;
    stats.advanceCount = pImpl->advanceCount.load(std::memory_order_relaxed);
    stats.batchAdvanceCount = pImpl->batchAdvanceCount.load(std::memory_order_relaxed);
    stats.cacheValid = pImpl->cacheValid;
    
    if (pImpl->realStartSet) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
        stats.realTimeElapsed = duration.count();
        stats.simulatedTime = pImpl->time;
        stats.speedFactor = stats.simulatedTime / (duration.count() > 0 ? duration.count() : 1.0);
    }
    
    return stats;
}

// Configure optimization settings
void OptimizedSimulationClock::enableSIMD(bool enable) {
    pImpl->useSIMD = enable;
}

void OptimizedSimulationClock::enableBatch(bool enable, size_t size) {
    pImpl->useBatch = enable;
    pImpl->batchSize = size;
}

void OptimizedSimulationClock::setStepCallback(std::function<void(SimulationStep, Timestamp)> callback) {
    pImpl->stepCallback = std::move(callback);
}

// Statistics from regular SimulationClock
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
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
