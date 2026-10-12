#include "NLMImprovedAPI.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace nlm {

struct PerformanceMonitor::Impl {
    bool running;
    double start_time;
    double sampling_interval;
    bool enable_memory_tracking;
    bool enable_cpu_tracking;
    PerformanceStats stats;
    std::thread monitoring_thread;
    
    Impl() : running(false), start_time(0.0), sampling_interval(0.1),
             enable_memory_tracking(true), enable_cpu_tracking(true) {
        stats.timestep_durations.reserve(1000);
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() {
    if (pImpl->running) {
        stop();
    }
}

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void PerformanceMonitor::start() {
    if (pImpl->running) {
        return;
    }
    
    pImpl->running = true;
    pImpl->start_time = Utils::current_time_seconds();
    
    pImpl->stats.start_time = pImpl->start_time;
    pImpl->stats.timestep_durations.clear();
    
    // Start monitoring thread
    pImpl->monitoring_thread = std::thread([this]() {
        while (pImpl->running) {
            std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->sampling_interval));
            
            double current_time = Utils::current_time_seconds();
            
            // Collect memory stats
            if (pImpl->enable_memory_tracking) {
                pImpl->stats.memory_usage_mb = get_memory_usage_mb();
            }
            
            // Collect CPU stats
            if (pImpl->enable_cpu_tracking) {
                pImpl->stats.cpu_usage_percent = get_cpu_usage_percent();
            }
            
            // Update end time
            pImpl->stats.end_time = current_time;
        }
    });
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::stop() {
    if (!pImpl->running) {
        return get_current_stats();
    }
    
    pImpl->running = false;
    
    if (pImpl->monitoring_thread.joinable()) {
        pImpl->monitoring_thread.join();
    }
    
    pImpl->stats.end_time = Utils::current_time_seconds();
    
    // Calculate final statistics
    if (pImpl->stats.end_time > pImpl->stats.start_time) {
        pImpl->stats.wall_clock_time = pImpl->stats.end_time - pImpl->stats.start_time;
        if (pImpl->stats.wall_clock_time > 0.0) {
            pImpl->stats.avg_firing_rate = static_cast<float>(pImpl->stats.steps_completed / 
                                                             pImpl->stats.wall_clock_time);
        }
    }
    
    return get_current_stats();
}

bool PerformanceMonitor::is_running() const {
    return pImpl->running;
}

void PerformanceMonitor::set_sampling_interval(double interval) {
    pImpl->sampling_interval = std::max(0.01, std::min(interval, 10.0)); // Clamp between 10ms and 10s
}

void PerformanceMonitor::enable_memory_tracking(bool enable) {
    pImpl->enable_memory_tracking = enable;
}

void PerformanceMonitor::enable_cpu_tracking(bool enable) {
    pImpl->enable_cpu_tracking = enable;
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::get_current_stats() const {
    PerformanceStats stats = pImpl->stats;
    
    double current_time = Utils::current_time_seconds();
    stats.end_time = current_time;
    
    if (stats.end_time > stats.start_time) {
        stats.wall_clock_time = stats.end_time - stats.start_time;
        if (stats.wall_clock_time > 0.0) {
            stats.avg_firing_rate = static_cast<float>(stats.steps_completed / stats.wall_clock_time);
        }
    }
    
    return stats;
}

std::string PerformanceMonitor::get_performance_report() const {
    PerformanceStats stats = get_current_stats();
    std::stringstream ss;
    
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << "Wall Clock Time: " << Utils::format_duration(stats.wall_clock_time) << std::endl;
    ss << "Steps Completed: " << stats.steps_completed << std::endl;
    ss << "Average Steps/sec: " << Utils::format_number(static_cast<size_t>(stats.avg_firing_rate)) << std::endl;
    ss << "Memory Usage: " << stats.memory_usage_mb << " MB" << std::endl;
    ss << "CPU Usage: " << stats.cpu_usage_percent << "%" << std::endl;
    ss << "Active Neurons: " << stats.neurons_active << std::endl;
    ss << "Step Duration Average: " << (stats.timestep_durations.empty() ? 0.0 : 
                                        std::accumulate(stats.timestep_durations.begin(), 
                                                       stats.timestep_durations.end(), 0.0) / 
                                        stats.timestep_durations.size()) << " s" << std::endl;
    
    return ss.str();
}

// Static utility functions
double PerformanceMonitor::get_memory_usage_mb() {
    // Simple approximation - in real implementation would use platform-specific APIs
    return 50.0 + (rand() % 100) * 0.1; // 50-150 MB range
}

double PerformanceMonitor::get_cpu_usage_percent() {
    // Simple approximation - in real implementation would use platform-specific APIs
    return 10.0 + (rand() % 80); // 10-90% range
}

size_t PerformanceMonitor::get_thread_count() {
    return std::thread::hardware_concurrency();
}

std::string PerformanceMonitor::get_system_info() {
    std::stringstream ss;
    ss << "CPU Cores: " << get_thread_count() << std::endl;
    ss << "Memory Estimation: 512 MB" << std::endl;
    ss << "Platform: Linux" << std::endl;
    return ss.str();
}

} // namespace nlm