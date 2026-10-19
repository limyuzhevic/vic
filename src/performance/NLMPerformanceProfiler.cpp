// Implementation of NLMPerformanceProfiler.hpp

#include "NLMPerformanceProfiler.hpp"
#include "ProfilerConfig.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>

// Thread-local storage definitions
thread_local nlm::profiling::ThreadLocalStorage::ThreadData nlm::profiling::ThreadLocalStorage::threadData_;
std::shared_mutex nlm::profiling::ThreadLocalStorage::mutex_;

// Initialize global thread-local profiler
inline void nlm::profiling::GlobalProfiler::initThreadProfilers() {
    GlobalProfiler::instance().addThreadProfiler("main");
    GlobalProfiler::instance().addThreadProfiler("worker");
}

// Start global profiling
inline void nlm::profiling::GlobalProfiler::startProfiling() {
    GlobalProfiler::instance().start();
}

// Stop global profiling
inline void nlm::profiling::GlobalProfiler::stopProfiling() {
    GlobalProfiler::instance().stop();
}

// Update global profiling
inline void nlm::profiling::GlobalProfiler::updateProfiling() {
    GlobalProfiler::instance().update();
    GlobalProfiler::instance().checkForBottlenecks();
}

// Generate performance report
inline void nlm::profiling::GlobalProfiler::generateReport(const std::string& filename,
                                                       ProfileReporter::OutputFormat format) {
    GlobalProfiler::instance().generateReport(filename, format);
}

// Memory profiler implementation
inline uint64_t nlm::profiling::MemoryProfiler::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// CPU profiler implementation
inline uint64_t nlm::profiling::CPUProfiler::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Performance metrics implementation
inline uint64_t nlm::profiling::PerformanceMetrics::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Performance alert system implementation
inline uint64_t nlm::profiling::PerformanceAlertSystem::getTimestampNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Timer implementation
inline Timer::Timer(const std::string& name, const std::string& context, const std::string& tag)
    : measurement_(new Measurement()) {
    measurement_->name = name;
    measurement_->context = context;
    measurement_->tag = tag;
    measurement_->threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
}

inline double Timer::duration_ns() const {
    if (!measurement_) return 0.0;
    return measurement_->duration_ns();
}

inline double Timer::duration_us() const {
    if (!measurement_) return 0.0;
    return measurement_->duration_us();
}

inline double Timer::duration_ms() const {
    if (!measurement_) return 0.0;
    return measurement_->duration_ms();
}
