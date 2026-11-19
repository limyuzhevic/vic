#include "MathUtils.hpp"
#include <random>
#include <chrono>
#include <iostream>

namespace nlm {

struct PerformanceTimer::Impl {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isRunning;
};

PerformanceTimer::PerformanceTimer() : pImpl(std::make_unique<Impl>()) {
    reset();
}

PerformanceTimer::~PerformanceTimer() = default;

void PerformanceTimer::start() {
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->isRunning = true;
}

double PerformanceTimer::stop() {
    if (!pImpl->isRunning) return 0.0;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
    pImpl->isRunning = false;
    return duration.count() / 1000000.0;
}

double PerformanceTimer::elapsed() const {
    if (!pImpl->isRunning) return 0.0;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
    return duration.count() / 1000000.0;
}

bool PerformanceTimer::isRunning() const {
    return pImpl->isRunning;
}

void PerformanceTimer::reset() {
    pImpl->isRunning = false;
}

} // namespace nlm
