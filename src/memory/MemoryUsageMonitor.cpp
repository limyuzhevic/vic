#include "memory/MemoryUsageMonitor.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// MemoryUsageMonitor implementation
void MemoryUsageMonitor::monitoringLoop() {
    while (tracking_enabled_) {
        updateMemoryUsage();
        
        // Notify listeners
        std::lock_guard<std::mutex> lock(listener_mutex_);
        for (const auto& listener : listeners_) {
            listener(getCurrentMemoryUsage());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(monitoring_interval_ms_));
    }
}

void MemoryUsageMonitor::updateMemoryUsage() {
    std::lock_guard<std::mutex> lock(history_mutex_);
    
    MemoryUsageSnapshot snapshot = getCurrentMemoryUsage();
    memory_usage_history_.push_back(snapshot);
    
    // Keep only recent history (last 1000 snapshots)
    if (memory_usage_history_.size() > 1000) {
        memory_usage_history_.erase(memory_usage_history_.begin());
    }
}

std::string MemoryUsageMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string MemoryUsageMonitor::getTimeString(std::chrono::system_clock::time_point time_point) const {
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point MemoryUsageMonitor::parseTimeString(const std::string& time_string) const {
    // Parse time string (simplified implementation)
    // In a real implementation, this would use a proper date/time parser
    return std::chrono::system_clock::now();
}

} // namespace nlm