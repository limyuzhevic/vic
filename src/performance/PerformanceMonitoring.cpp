#include "PerformanceMonitoring.hpp"
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>

namespace nlm {

PerformanceMonitor::PerformanceMonitor(const PerformanceConfig& config) : config(config) {
    last_sampling_time = std::chrono::steady_clock::now();
    registerThread(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

PerformanceMonitor::~PerformanceMonitor() {
    // Ensure all threads are unregistered
    std::lock_guard<std::mutex> lock(thread_mutex);
    thread_metrics.clear();
}

void PerformanceMonitor::configure(const PerformanceConfig& new_config) {
    std::lock_guard<std::mutex> lock1(metrics_mutex);
    std::lock_guard<std::mutex> lock2(profile_mutex);
    
    config = new_config;
    
    // Clear old data if history exceeds limit
    if (config.max_history_samples > 0 && metrics_history.size() > config.max_history_samples) {
        size_t start_index = metrics_history.size() - config.max_history_samples;
        metrics_history.erase(metrics_history.begin(), metrics_history.begin() + start_index);
    }
}

void PerformanceMonitor::recordMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    
    metrics_history.push_back(metrics);
    total_simulation_steps += metrics.simulation_step;
    total_spikes += metrics.total_spikes;
    
    // Trim history if it exceeds limit
    if (config.max_history_samples > 0 && metrics_history.size() > config.max_history_samples) {
        size_t start_index = metrics_history.size() - config.max_history_samples;
        metrics_history.erase(metrics_history.begin(), metrics_history.begin() + start_index);
    }
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    
    PerformanceMetrics current;
    if (!metrics_history.empty()) {
        current = metrics_history.back();
    }
    
    return current;
}

std::vector<PerformanceMetrics> PerformanceMonitor::getMetricsHistory() const {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    return metrics_history;
}

void PerformanceMonitor::recordProfileEvent(const ProfileEvent& event) {
    std::lock_guard<std::mutex> lock(profile_mutex);
    
    profile_events.push_back(event);
}

void PerformanceMonitor::startProfile(const std::string& function_name, const std::string& file_name, uint32_t line_number) {
    if (config.enable_profiling) {
        ProfileEvent event;
        event.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
        event.function_name = function_name;
        event.file_name = file_name;
        event.line_number = line_number;
        event.thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
        
        recordProfileEvent(event);
    }
}

void PerformanceMonitor::stopProfile(const std::string& function_name) {
    // Implementation will be handled by PerformanceProfiler
}

std::vector<ProfileEvent> PerformanceMonitor::getProfileEvents() const {
    std::lock_guard<std::mutex> lock(profile_mutex);
    return profile_events;
}

void PerformanceMonitor::addAlertCondition(const PerformanceAlert& alert) {
    // This is handled by PerformanceAlerting class
}

std::vector<PerformanceAlert> PerformanceMonitor::getActiveAlerts() const {
    return std::vector<PerformanceAlert>(); // Handled by PerformanceAlerting
}

void PerformanceMonitor::clearAlerts() {
    // Handled by PerformanceAlerting
}

void PerformanceMonitor::registerThread(uint32_t thread_id) {
    std::lock_guard<std::mutex> lock(thread_mutex);
    
    bool exists = false;
    for (const auto& tm : thread_metrics) {
        if (tm.thread_id == thread_id) {
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        ThreadMetrics tm;
        tm.thread_id = thread_id;
        thread_metrics.push_back(tm);
    }
}

void PerformanceMonitor::unregisterThread(uint32_t thread_id) {
    std::lock_guard<std::mutex> lock(thread_mutex);
    
    auto it = std::remove_if(thread_metrics.begin(), thread_metrics.end(),
        [&thread_id](const ThreadMetrics& tm) {
            return tm.thread_id == thread_id;
        });
    
    thread_metrics.erase(it, thread_metrics.end());
}

void PerformanceMonitor::saveMetricsToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "Performance Metrics" << std::endl;
    file << "====================" << std::endl << std::endl;
    
    for (const auto& metric : metrics_history) {
        file << "Step: " << metric.simulation_step << std::endl;
        file << "Timestamp: " << metric.timestamp_ns << " ns" << std::endl;
        file << "CPU Usage: " << std::fixed << std::setprecision(2) << metric.cpu_usage_percent << "%" << std::endl;
        file << "Memory Usage: " << std::fixed << std::setprecision(2) << metric.memory_usage_mb << " MB" << std::endl;
        file << "Active Neurons: " << metric.active_neurons << std::endl;
        file << "Total Spikes: " << metric.total_spikes << std::endl;
        file << "Average Firing Rate: " << std::fixed << std::setprecision(3) << metric.average_firing_rate << std::endl;
        file << "Simulation Time: " << std::fixed << std::setprecision(3) << metric.simulation_time_ms << " ms" << std::endl;
        file << "Thread Count: " << metric.thread_count << std::endl;
        file << "Memory Allocated: " << metric.memory_allocated << " bytes" << std::endl;
        file << "Memory Pool Allocated: " << metric.memory_pool_allocated << " bytes" << std::endl;
        file << "Memory Pool Used: " << metric.memory_pool_used << " bytes" << std::endl;
        file << std::endl;
    }
    
    file.close();
}

void PerformanceMonitor::saveProfileToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(profile_mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "Profile Events" << std::endl;
    file << "===============" << std::endl << std::endl;
    
    for (const auto& event : profile_events) {
        file << "Function: " << event.function_name << std::endl;
        file << "Timestamp: " << event.timestamp_ns << " ns" << std::endl;
        file << "Duration: " << event.duration_ns << " ns" << std::endl;
        file << "Thread ID: " << event.thread_id << std::endl;
        if (!event.file_name.empty()) {
            file << "File: " << event.file_name;
            if (event.line_number > 0) {
                file << ":" << event.line_number;
            }
            file << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
}

void PerformanceMonitor::saveAlertsToFile(const std::string& filename) const {
    // Alerts are handled by PerformanceAlerting
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock1(metrics_mutex);
    std::lock_guard<std::mutex> lock2(profile_mutex);
    
    metrics_history.clear();
    profile_events.clear();
    total_simulation_steps = 0;
    total_spikes = 0;
}

void PerformanceMonitor::resetProfileData() {
    std::lock_guard<std::mutex> lock(profile_mutex);
    profile_events.clear();
}

double PerformanceMonitor::getAverageFiringRate() const {
    return PerformanceAnalyzer::calculateAverageFiringRate(metrics_history);
}

double PerformanceMonitor::getCurrentFiringRate() const {
    const auto& metrics = getCurrentMetrics();
    return metrics.average_firing_rate;
}

} // namespace nlm