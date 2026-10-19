#pragma once

/**
 * Performance Profiler Configuration Manager
 * 
 * Manages configuration for the NLM performance profiling system,
 * including loading configuration from files and integrating with
 * the existing NLM configuration system.
 */

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "ProfilerConfig.hpp"

namespace nlm {
namespace profiling {

class ProfilerConfigManager {
private:
    static ProfilerConfigManager instance_;
    ProfilerConfig config_;
    bool loaded_;
    
    ProfilerConfigManager() : loaded_(false) {}
    
public:
    static ProfilerConfigManager& instance() {
        return instance_;
    }
    
    /**
     * Load configuration from a file
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse the configuration file
        loadFromStream(file);
        loaded_ = true;
        return true;
    }
    
    /**
     * Load configuration from a stream
     */
    bool loadFromStream(std::istream& stream) {
        config_ = ProfilerConfig();  // Reset to defaults
        
        std::string line;
        while (std::getline(stream, line)) {
            // Remove comments and trim whitespace
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            size_t end = line.find_last_not_of(" \t\r\n");
            if (start == std::string::npos || end == std::string::npos) {
                continue;
            }
            line = line.substr(start, end - start + 1);
            
            // Parse key-value pair
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace from key and value
            start = key.find_first_not_of(" \t\r\n");
            end = key.find_last_not_of(" \t\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                key = key.substr(start, end - start + 1);
            }
            
            start = value.find_first_not_of(" \t\r\n");
            end = value.find_last_not_of(" \t\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                value = value.substr(start, end - start + 1);
            }
            
            // Parse based on key
            parseKeyValue(key, value);
        }
        
        return true;
    }
    
    /**
     * Parse a key-value pair
     */
    void parseKeyValue(const std::string& key, const std::string& value) {
        // Remove section prefix if present (e.g., "performance." -> "performance")
        size_t dotPos = key.find('.');
        std::string section = (dotPos != std::string::npos) ? key.substr(0, dotPos) : "";
        std::string field = (dotPos != std::string::npos) ? key.substr(dotPos + 1) : key;
        
        // Parse based on field name
        if (field == "profiling_enabled") {
            config_.enabled = (value == "true" || value == "1" || value == "yes");
        } else if (field == "profiling_output_file") {
            config_.outputFile = value;
        } else if (field == "profiling_sample_interval_ms") {
            config_.sampleIntervalMs = std::stoul(value);
        } else if (field == "profiling_report_interval_s") {
            config_.reportIntervalSeconds = std::stoul(value);
        } else if (field == "memory_threshold_mb") {
            config_.memoryThresholdMB = std::stod(value);
        } else if (field == "cpu_threshold_percent") {
            config_.cpuThresholdPercent = std::stod(value);
        } else if (field == "operation_threshold_ns") {
            config_.operationThresholdNs = std::stod(value);
        } else if (field == "spike_rate_threshold_per_s") {
            config_.spikeRateThresholdPerSec = std::stod(value);
        } else if (field == "profile_brain") {
            config_.profileBrain = (value == "true" || value == "1" || value == "yes");
        } else if (field == "profile_motor") {
            config_.profileMotor = (value == "true" || value == "1" || value == "yes");
        } else if (field == "profile_sensor") {
            config_.profileSensor = (value == "true" || value == "1" || value == "yes");
        } else if (field == "profile_neuromodulation") {
            config_.profileNeuromodulation = (value == "true" || value == "1" || value == "yes");
        } else if (field == "profile_plasticity") {
            config_.profilePlasticity = (value == "true" || value == "1" || value == "yes");
        } else if (field == "output_text_report") {
            config_.outputTextReport = (value == "true" || value == "1" || value == "yes");
        } else if (field == "output_csv_report") {
            config_.outputCSVReport = (value == "true" || value == "1" || value == "yes");
        } else if (field == "output_json_report") {
            config_.outputJSONReport = (value == "true" || value == "1" || value == "yes");
        } else if (field == "console_output") {
            config_.consoleOutput = (value == "true" || value == "1" || value == "yes");
        } else if (field == "real_time_monitoring") {
            config_.realTimeMonitoring = (value == "true" || value == "1" || value == "yes");
        } else if (field == "thread_local_profiling") {
            config_.threadLocalProfiling = (value == "true" || value == "1" || value == "yes");
        } else if (field == "global_aggregation") {
            config_.globalAggregation = (value == "true" || value == "1" || value == "yes");
        } else if (field == "max_threads") {
            config_.maxThreads = std::stoul(value);
        } else if (field == "detect_bottlenecks") {
            config_.detectBottlenecks = (value == "true" || value == "1" || value == "yes");
        } else if (field == "detect_regressions") {
            config_.detectRegressions = (value == "true" || value == "1" || value == "yes");
        } else if (field == "comparative_profiling") {
            config_.comparativeProfiling = (value == "true" || value == "1" || value == "yes");
        } else if (field == "baseline_file") {
            config_.baselineFile = value;
        } else if (field == "alert_on_threshold") {
            config_.alertOnThreshold = (value == "true" || value == "1" || value == "yes");
        } else if (field == "alert_level") {
            config_.alertLevel = value;
        } else if (field == "alert_callback") {
            config_.alertCallback = value;
        } else if (field == "enable_memory_tracking") {
            config_.enableMemoryTracking = (value == "true" || value == "1" || value == "yes");
        } else if (field == "enable_cpu_tracking") {
            config_.enableCPUTracking = (value == "true" || value == "1" || value == "yes");
        } else if (field == "enable_io_tracking") {
            config_.enableIOTracking = (value == "true" || value == "1" || value == "yes");
        } else if (field == "enable_network_tracking") {
            config_.enableNetworkTracking = (value == "true" || value == "1" || value == "yes");
        } else if (field == "compressed_output") {
            config_.compressedOutput = (value == "true" || value == "1" || value == "yes");
        } else if (field == "max_history_entries") {
            config_.maxHistoryEntries = std::stoul(value);
        } else if (field == "include_operations") {
            config_.includeOperations = value;
        } else if (field == "exclude_operations") {
            config_.excludeOperations = value;
        } else if (field == "include_contexts") {
            config_.includeContexts = value;
        } else if (field == "exclude_contexts") {
            config_.excludeContexts = value;
        }
    }
    
    /**
     * Get the current configuration
     */
    const ProfilerConfig& getConfig() const {
        return config_;
    }
    
    /**
     * Update configuration with another config
     */
    void updateConfig(const ProfilerConfig& other) {
        config_.merge(other);
        loaded_ = true;
    }
    
    /**
     * Check if configuration has been loaded
     */
    bool isLoaded() const {
        return loaded_;
    }
    
    /**
     * Save configuration to a file
     */
    bool saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration
        saveToStream(file);
        return true;
    }
    
    /**
     * Save configuration to a stream
     */
    void saveToStream(std::ostream& stream) const {
        stream << "# NLM Performance Profiler Configuration" << std::endl;
        stream << "# Configuration for Phase 8: Performance Profiling System" << std::endl;
        stream << std::endl;
        
        // Basic configuration
        stream << "performance.profiling_enabled = " << (config_.enabled ? "true" : "false") << std::endl;
        stream << "performance.profiling_output_file = " << config_.outputFile << std::endl;
        stream << "performance.profiling_sample_interval_ms = " << config_.sampleIntervalMs << std::endl;
        stream << "performance.profiling_report_interval_s = " << config_.reportIntervalSeconds << std::endl;
        stream << std::endl;
        
        // Thresholds
        stream << "performance.memory_threshold_mb = " << config_.memoryThresholdMB << std::endl;
        stream << "performance.cpu_threshold_percent = " << config_.cpuThresholdPercent << std::endl;
        stream << "performance.operation_threshold_ns = " << config_.operationThresholdNs << std::endl;
        stream << "performance.spike_rate_threshold_per_s = " << config_.spikeRateThresholdPerSec << std::endl;
        stream << std::endl;
        
        // Profiling targets
        stream << "performance.profile_brain = " << (config_.profileBrain ? "true" : "false") << std::endl;
        stream << "performance.profile_motor = " << (config_.profileMotor ? "true" : "false") << std::endl;
        stream << "performance.profile_sensor = " << (config_.profileSensor ? "true" : "false") << std::endl;
        stream << "performance.profile_neuromodulation = " << (config_.profileNeuromodulation ? "true" : "false") << std::endl;
        stream << "performance.profile_plasticity = " << (config_.profilePlasticity ? "true" : "false") << std::endl;
        stream << std::endl;
        
        // Output configuration
        stream << "performance.output_text_report = " << (config_.outputTextReport ? "true" : "false") << std::endl;
        stream << "performance.output_csv_report = " << (config_.outputCSVReport ? "true" : "false") << std::endl;
        stream << "performance.output_json_report = " << (config_.outputJSONReport ? "true" : "false") << std::endl;
        stream << "performance.console_output = " << (config_.consoleOutput ? "true" : "false") << std::endl;
        stream << "performance.real_time_monitoring = " << (config_.realTimeMonitoring ? "true" : "false") << std::endl;
        stream << std::endl;
        
        // Multi-threading configuration
        stream << "performance.thread_local_profiling = " << (config_.threadLocalProfiling ? "true" : "false") << std::endl;
        stream << "performance.global_aggregation = " << (config_.globalAggregation ? "true" : "false") << std::endl;
        stream << "performance.max_threads = " << config_.maxThreads << std::endl;
        stream << std::endl;
        
        // Analysis and detection
        stream << "performance.detect_bottlenecks = " << (config_.detectBottlenecks ? "true" : "false") << std::endl;
        stream << "performance.detect_regressions = " << (config_.detectRegressions ? "true" : "false") << std::endl;
        stream << "performance.comparative_profiling = " << (config_.comparativeProfiling ? "true" : "false") << std::endl;
        stream << "performance.baseline_file = " << config_.baselineFile << std::endl;
        stream << std::endl;
        
        // Alerting configuration
        stream << "performance.alert_on_threshold = " << (config_.alertOnThreshold ? "true" : "false") << std::endl;
        stream << "performance.alert_level = " << config_.alertLevel << std::endl;
        stream << "performance.alert_callback = " << config_.alertCallback << std::endl;
        stream << std::endl;
        
        // Advanced features
        stream << "performance.enable_memory_tracking = " << (config_.enableMemoryTracking ? "true" : "false") << std::endl;
        stream << "performance.enable_cpu_tracking = " << (config_.enableCPUTracking ? "true" : "false") << std::endl;
        stream << "performance.enable_io_tracking = " << (config_.enableIOTracking ? "true" : "false") << std::endl;
        stream << "performance.enable_network_tracking = " << (config_.enableNetworkTracking ? "true" : "false") << std::endl;
        stream << "performance.compressed_output = " << (config_.compressedOutput ? "true" : "false") << std::endl;
        stream << "performance.max_history_entries = " << config_.maxHistoryEntries << std::endl;
        stream << std::endl;
        
        // Filter configuration
        if (!config_.includeOperations.empty()) {
            stream << "performance.include_operations = " << config_.includeOperations << std::endl;
        }
        if (!config_.excludeOperations.empty()) {
            stream << "performance.exclude_operations = " << config_.excludeOperations << std::endl;
        }
        if (!config_.includeContexts.empty()) {
            stream << "performance.include_contexts = " << config_.includeContexts << std::endl;
        }
        if (!config_.excludeContexts.empty()) {
            stream << "performance.exclude_contexts = " << config_.excludeContexts << std::endl;
        }
    }
};

// Global configuration manager instance
inline ProfilerConfigManager ProfilerConfigManager::instance_;

// Convenience functions
inline bool initializeProfiler(const std::string& configFile = "") {
    if (!configFile.empty()) {
        return ProfilerConfigManager::instance().loadFromFile(configFile);
    }
    return true;
}

inline const ProfilerConfig& getProfilerConfig() {
    return ProfilerConfigManager::instance().getConfig();
}

inline void updateProfilerConfig(const ProfilerConfig& config) {
    ProfilerConfigManager::instance().updateConfig(config);
}

inline void startProfiler() {
    nlm::profiling::GlobalProfiler::startProfiling();
}

inline void stopProfiler() {
    nlm::profiling::GlobalProfiler::stopProfiling();
}

inline void updateProfiler() {
    nlm::profiling::GlobalProfiler::updateProfiling();
}

inline void generateProfilerReport(const std::string& filename, 
                                 nlm::profiling::ProfileReporter::OutputFormat format = nlm::profiling::ProfileReporter::OutputFormat::TEXT) {
    nlm::profiling::GlobalProfiler::generateReport(filename, format);
}

inline void recordBrainOperation(const std::string& operation, double durationNs = 0.0) {
    nlm::profiling::GlobalProfiler::instance().recordOperation("BRAIN_" + operation, durationNs, "BRAIN");
}

inline void recordMotorOperation(const std::string& operation, double durationNs = 0.0) {
    nlm::profiling::GlobalProfiler::instance().recordOperation("MOTOR_" + operation, durationNs, "MOTOR");
}

inline void recordSensorOperation(const std::string& operation, double durationNs = 0.0) {
    nlm::profiling::GlobalProfiler::instance().recordOperation("SENSOR_" + operation, durationNs, "SENSOR");
}

inline void recordNeuromodulationOperation(const std::string& operation, double durationNs = 0.0) {
    nlm::profiling::GlobalProfiler::instance().recordOperation("NEUROMODULATION_" + operation, durationNs, "NEUROMODULATION");
}

inline void recordPlasticityOperation(const std::string& operation, double durationNs = 0.0) {
    nlm::profiling::GlobalProfiler::instance().recordOperation("PLASTICITY_" + operation, durationNs, "PLASTICITY");
}

} // namespace profiling
} // namespace nlm
