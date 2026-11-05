// NLM Performance Monitoring and Debugging Demonstration

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <map>

#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"

namespace nlm {

/**
 * @class PerformanceMonitor
 * @brief Simple performance monitoring for NLM systems
 * 
 * This class provides basic performance monitoring capabilities for NLM components,
 * including timing, memory usage, and system state monitoring.
 */
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpu_time_ms = 0.0;
        double wall_time_ms = 0.0;
        size_t memory_usage_kb = 0;
        size_t spike_count = 0;
        size_t neuron_count = 0;
        size_t synapse_count = 0;
        float average_firing_rate = 0.0;
        size_t memory_trace_count = 0;
        size_t episode_count = 0;
        float curiosity_level = 0.0;
        float novelty_level = 0.0;
        float prediction_error = 0.0;
        float reward_value = 0.0;
        std::map<std::string, double> component_times_ms;
    };
    
    PerformanceMonitor() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void startTimer() {
        timer_start_ = std::chrono::high_resolution_clock::now();
    }
    
    double stopTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - timer_start_);
        return duration.count() / 1000.0;  // Convert to milliseconds
    }
    
    double getUptimeMs() const {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time_);
        return duration.count() / 1000.0;
    }
    
    void logPerformance(const std::string& component, const PerformanceMetrics& metrics) {
        std::cout << "[PERFORMANCE] " << component << ":" << std::endl;
        std::cout << "  CPU Time: " << std::fixed << std::setprecision(2) << metrics.cpu_time_ms << " ms" << std::endl;
        std::cout << "  Wall Time: " << metrics.wall_time_ms << " ms" << std::endl;
        std::cout << "  Memory Usage: " << metrics.memory_usage_kb << " KB" << std::endl;
        std::cout << "  Spikes Generated: " << metrics.spike_count << std::endl;
        std::cout << "  Neurons: " << metrics.neuron_count << std::endl;
        std::cout << "  Synapses: " << metrics.synapse_count << std::endl;
        std::cout << "  Average Firing Rate: " << std::setprecision(3) << metrics.average_firing_rate << " Hz" << std::endl;
        std::cout << "  Working Memory Traces: " << metrics.memory_trace_count << std::endl;
        std::cout << "  Episodes: " << metrics.episode_count << std::endl;
        std::cout << "  Curiosity Level: " << std::setprecision(3) << metrics.curiosity_level << std::endl;
        std::cout << "  Novelty Level: " << std::setprecision(3) << metrics.novelty_level << std::endl;
        std::cout << "  Prediction Error: " << std::setprecision(3) << metrics.prediction_error << std::endl;
        std::cout << "  Reward Value: " << std::setprecision(3) << metrics.reward_value << std::endl;
        
        if (!metrics.component_times_ms.empty()) {
            std::cout << "  Component Times:" << std::endl;
            for (const auto& pair : metrics.component_times_ms) {
                std::cout << "    " << pair.first << ": " << std::fixed << std::setprecision(2) 
                         << pair.second << " ms" << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    static std::string formatTime(double ms) {
        if (ms < 1.0) {
            return std::to_string(ms * 1000.0) + " μs";
        } else if (ms < 1000.0) {
            return std::to_string(ms) + " ms";
        } else {
            return std::to_string(ms / 1000.0) + " s";
        }
    }
    
    static std::string formatMemory(size_t kb) {
        if (kb < 1024) {
            return std::to_string(kb) + " KB";
        } else if (kb < 1024 * 1024) {
            return std::to_string(kb / 1024.0) + " MB";
        } else {
            return std::to_string(kb / (1024.0 * 1024.0)) + " GB";
        }
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point timer_start_;
};

/**
 * @class DebugLogger
 * @brief Simple debugging and logging for NLM systems
 * 
 * This class provides basic debugging capabilities for NLM components,
 * including log levels, message formatting, and filtering.
 */
class DebugLogger {
public:
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        std::chrono::high_resolution_clock::time_point timestamp;
        LogLevel level;
        std::string component;
        std::string message;
    };
    
    DebugLogger() : min_level_(LogLevel::INFO) {}
    
    void setMinLevel(LogLevel level) {
        min_level_ = level;
    }
    
    void log(LogLevel level, const std::string& component, const std::string& message) {
        if (level < min_level_) {
            return;
        }
        
        LogEntry entry;
        entry.timestamp = std::chrono::high_resolution_clock::now();
        entry.level = level;
        entry.component = component;
        entry.message = message;
        
        logEntries_.push_back(entry);
        
        // Format and output log message
        std::string level_str;
        switch (level) {
            case LogLevel::TRACE: level_str = "TRACE"; break;
            case LogLevel::DEBUG: level_str = "DEBUG"; break;
            case LogLevel::INFO: level_str = "INFO"; break;
            case LogLevel::WARNING: level_str = "WARNING"; break;
            case LogLevel::ERROR: level_str = "ERROR"; break;
            case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
        }
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - entry.timestamp);
        
        std::cout << "[" << duration.count() << "ms] " << level_str << " [" 
                 << component << "] " << message << std::endl;
    }
    
    void trace(const std::string& component, const std::string& message) {
        log(LogLevel::TRACE, component, message);
    }
    
    void debug(const std::string& component, const std::string& message) {
        log(LogLevel::DEBUG, component, message);
    }
    
    void info(const std::string& component, const std::string& message) {
        log(LogLevel::INFO, component, message);
    }
    
    void warning(const std::string& component, const std::string& message) {
        log(LogLevel::WARNING, component, message);
    }
    
    void error(const std::string& component, const std::string& message) {
        log(LogLevel::ERROR, component, message);
    }
    
    void critical(const std::string& component, const std::string& message) {
        log(LogLevel::CRITICAL, component, message);
    }
    
    void showSummary() const {
        std::cout << "=== Debug Log Summary ===" << std::endl;
        std::cout << "Total entries: " << logEntries_.size() << std::endl;
        
        std::map<LogLevel, size_t> level_counts;
        for (const auto& entry : logEntries_) {
            level_counts[entry.level]++;
        }
        
        std::cout << "Level breakdown:" << std::endl;
        for (const auto& pair : level_counts) {
            std::string level_str;
            switch (pair.first) {
                case LogLevel::TRACE: level_str = "TRACE"; break;
                case LogLevel::DEBUG: level_str = "DEBUG"; break;
                case LogLevel::INFO: level_str = "INFO"; break;
                case LogLevel::WARNING: level_str = "WARNING"; break;
                case LogLevel::ERROR: level_str = "ERROR"; break;
                case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
            }
            std::cout << "  " << level_str << ": " << pair.second << std::endl;
        }
        
        std::cout << "Last 10 entries:" << std::endl;
        size_t start_idx = std::max<size_t>(0, logEntries_.size() - 10);
        for (size_t i = start_idx; i < logEntries_.size(); ++i) {
            const auto& entry = logEntries_[i];
            std::string level_str;
            switch (entry.level) {
                case LogLevel::TRACE: level_str = "TRACE"; break;
                case LogLevel::DEBUG: level_str = "DEBUG"; break;
                case LogLevel::INFO: level_str = "INFO"; break;
                case LogLevel::WARNING: level_str = "WARNING"; break;
                case LogLevel::ERROR: level_str = "ERROR"; break;
                case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
            }
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - entry.timestamp);
            
            std::cout << "  [" << duration.count() << "ms] " << level_str << " [" 
                     << entry.component << "] " << entry.message << std::endl;
        }
    }
    
    void clear() {
        logEntries_.clear();
    }
    
private:
    std::vector<LogEntry> logEntries_;
    LogLevel min_level_;
};

/**
 * @brief Demonstration of NLM performance monitoring and debugging features
 * 
 * This function demonstrates the performance monitoring and debugging capabilities
 * that have been added to the NLM brain system. It shows:
 * - Performance metrics collection and reporting
 * - Debug logging with different levels
 * - Configuration validation and monitoring
 * - Basic system health checks
 */
void demonstratePerformanceMonitoring() {
    std::cout << "=== NLM Performance Monitoring and Debugging Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create performance monitor and debug logger
    PerformanceMonitor perfMonitor;
    DebugLogger debugLogger;
    
    // Set debug logger to INFO level
    debugLogger.setMinLevel(DebugLogger::LogLevel::INFO);
    
    // Simulate system initialization
    debugLogger.info("System", "Initializing NLM brain system...");
    perfMonitor.startTimer();
    
    // Simulate configuration loading
    debugLogger.info("Config", "Loading configuration...");
    Config config;
    ConfigManager::initializeDefaults(config);
    
    // Validate configuration
    std::string errorMsg;
    if (ConfigManager::validate(config, errorMsg)) {
        debugLogger.info("Config", "Configuration validation passed");
    } else {
        debugLogger.error("Config", "Configuration validation failed: " + errorMsg);
    }
    
    // Simulate reward computation
    debugLogger.info("Reward", "Computing reward based on observation...");
    
    // Create a mock observation for reward computation
    // (In a real system, this would come from the actual environment)
    Reward reward;
    
    perfMonitor.startTimer();
    // In a real system, we would have actual sensory data
    // For this demo, we'll simulate reward computation
    double computed_reward = 2.5;  // Simulated reward value
    debugLogger.info("Reward", "Reward computed: " + std::to_string(computed_reward));
    
    double computation_time = perfMonitor.stopTimer();
    debugLogger.info("Performance", "Reward computation completed in " + std::to_string(computation_time) + " ms");
    
    // Simulate curiosity system
    debugLogger.info("Curiosity", "Updating curiosity system...");
    
    perfMonitor.startTimer();
    double curiosity_level = 0.6;  // Simulated curiosity
    double novelty = 0.3;
    double prediction_error = 0.1;
    debugLogger.info("Curiosity", "Curiosity level: " + std::to_string(curiosity_level) + ", Novelty: " + 
                     std::to_string(novelty) + ", Prediction Error: " + std::to_string(prediction_error));
    
    double curiosity_time = perfMonitor.stopTimer();
    debugLogger.info("Performance", "Curiosity update completed in " + std::to_string(curiosity_time) + " ms");
    
    // Simulate system metrics collection
    debugLogger.info("Performance", "Collecting system metrics...");
    
    PerformanceMonitor::PerformanceMetrics metrics;
    metrics.cpu_time_ms = computation_time + curiosity_time;
    metrics.wall_time_ms = perfMonitor.getUptimeMs();
    metrics.memory_usage_kb = 2048;  // Simulated memory usage
    metrics.spike_count = 15420;     // Simulated spike count
    metrics.neuron_count = 1000;     // Simulated neuron count
    metrics.synapse_count = 37800;   // Simulated synapse count
    metrics.average_firing_rate = 23.4;  // Simulated firing rate
    metrics.memory_trace_count = 45;    // Simulated memory traces
    metrics.episode_count = 12;          // Simulated episodes
    metrics.curiosity_level = curiosity_level;
    metrics.novelty_level = novelty;
    metrics.prediction_error = prediction_error;
    metrics.reward_value = computed_reward;
    
    // Log performance metrics
    perfMonitor.logPerformance("Main Loop", metrics);
    
    // Show debug log summary
    debugLogger.showSummary();
    
    std::cout << "=== Performance Monitoring Demo Complete ===" << std::endl;
}

} // namespace nlm

// Main function for standalone execution
int main() {
    // Set up logging
    std::cout << "NLM Performance Monitoring and Debugging Tools" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    // Run demonstration
    nlm::demonstratePerformanceMonitoring();
    
    return 0;
}
