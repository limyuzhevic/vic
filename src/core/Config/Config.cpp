#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>
#include <unordered_map>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <utility>
#include <fstream>
#include <sys/types.h>
#include <sys/sysinfo.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<spdlog::logger> logger;
    
    Impl() {
        logger = spdlog::stdout_color_mt("nlm_config");
        logger->set_level(spdlog::level::info);
    }
};

// Helper function to validate and normalize config keys
std::string validateAndNormalizeKey(const std::string& key) {
    std::string normalized = key;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    // Remove any non-alphanumeric characters except underscores
    std::string result;
    for (char c : normalized) {
        if (std::isalnum(c) || c == '_') {
            result += c;
        }
    }
    
    return result;
}

// Configuration validation schema
struct ConfigValidation {
    static bool validate(const std::string& key, const ConfigValue& value) {
        // Define valid configuration keys and their expected types
        static const std::unordered_map<std::string, std::vector<std::type_index>> validConfigs = {
            {"neuron_count", {typeid(int), typeid(int64_t)}},
            {"region_count", {typeid(int), typeid(int64_t)}},
            {"connection_probability", {typeid(double)}},
            {"simulation_timestep", {typeid(double)}},
            {"random_seed", {typeid(int), typeid(int64_t)}},
            {"stdp_ltp_weight", {typeid(double)}},
            {"stdp_ltd_weight", {typeid(double)}},
            {"synaptogenesis_rate", {typeid(double)}},
            {"pruning_rate", {typeid(double)}},
            {"enable_reward_modulation", {typeid(bool)}},
            {"enable_curiosity", {typeid(bool)}},
            {"enable_structural_plasticity", {typeid(bool)}},
            {"enable_development", {typeid(bool)}}
        };
        
        std::string normalizedKey = validateAndNormalizeKey(key);
        
        // Check if key is in validation schema
        auto it = validConfigs.find(normalizedKey);
        if (it == validConfigs.end()) {
            return true; // Allow unknown keys for forward compatibility
        }
        
        // Check if value type matches expected types
        std::type_index valueType = typeid(ConfigValue);
        for (const auto& valType : it->second) {
            if (valType == typeid(int) && std::holds_alternative<int>(value)) {
                return true;
            } else if (valType == typeid(int64_t) && std::holds_alternative<int64_t>(value)) {
                return true;
            } else if (valType == typeid(double) && std::holds_alternative<double>(value)) {
                return true;
            } else if (valType == typeid(bool) && std::holds_alternative<bool>(value)) {
                return true;
            } else if (valType == typeid(std::string) && std::holds_alternative<std::string>(value)) {
                return true;
            } else if (valType == typeid(std::vector<int>) && std::holds_alternative<std::vector<int>>(value)) {
                return true;
            } else if (valType == typeid(std::vector<double>) && std::holds_alternative<std::vector<double>>(value)) {
                return true;
            } else if (valType == typeid(std::vector<std::string>) && std::holds_alternative<std::vector<std::string>>(value)) {
                return true;
            }
        }
        
        return false;
    }
};

// Conversion helpers for different value types
namespace {
    std::string configValueToString(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
                return oss.str();
            } else {
                throw std::runtime_error("Unsupported ConfigValue type");
            }
        }, value);
    }
    
    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& json) {
        if (json.is_number_integer()) {
            return json.get<int>();
        } else if (json.is_number_unsigned()) {
            return json.get<int64_t>();
        } else if (json.is_number_float()) {
            return json.get<double>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_array()) {
            // Try to infer array type
            if (!json.empty()) {
                if (json[0].is_number_integer()) {
                    return json.get<std::vector<int>>();
                } else if (json[0].is_number_float()) {
                    return json.get<std::vector<double>>();
                } else {
                    return json.get<std::vector<std::string>>();
                }
            }
            return std::vector<std::string>();
        } else {
            throw std::runtime_error("Unsupported JSON type");
        }
    }
    
    // Validate configuration entry
    bool validateConfigEntry(const ConfigEntry& entry) {
        if (!isValidKey(entry.key)) {
            return false;
        }
        
        if (!validateValueRange(entry.key, entry.value)) {
            return false;
        }
        
        return true;
    }
    
    // Validate configuration key format
    bool isValidKey(const std::string& key) {
        if (key.empty()) {
            return false;
        }
        
        // Check for valid characters (alphanumeric and underscores)
        for (char c : key) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        return true;
    }
    
    // Validate configuration value ranges
    bool validateValueRange(const std::string& key, const ConfigValue& value) {
        std::string normalizedKey = key;
        std::transform(normalizedKey.begin(), normalizedKey.end(), normalizedKey.begin(), ::tolower);
        
        // Neuron count validation
        if (normalizedKey == "neuron_count") {
            if (std::holds_alternative<int>(value)) {
                int count = std::get<int>(value);
                return count > 0 && count < std::numeric_limits<int>::max() / 1000;
            } else if (std::holds_alternative<int64_t>(value)) {
                int64_t count = std::get<int64_t>(value);
                return count > 0 && count < std::numeric_limits<int64_t>::max() / 1000;
            }
        }
        
        // Connection probability validation
        if (normalizedKey == "connection_probability") {
            if (std::holds_alternative<double>(value)) {
                double prob = std::get<double>(value);
                return prob >= 0.0 && prob <= 1.0;
            }
        }
        
        // Simulation timestep validation
        if (normalizedKey == "simulation_timestep") {
            if (std::holds_alternative<double>(value)) {
                double timestep = std::get<double>(value);
                return timestep > 0.0 && timestep < 1.0;
            }
        }
        
        // Plasticity weights validation
        if (normalizedKey == "stdp_ltp_weight" || normalizedKey == "stdp_ltd_weight") {
            if (std::holds_alternative<double>(value)) {
                double weight = std::get<double>(value);
                return weight >= 0.0 && weight <= 1.0;
            }
        }
        
        return true; // Default to valid for unknown keys
    }
    
    // Advanced configuration validation with detailed error reporting
    struct DetailedValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::unordered_map<std::string, std::string> suggestions;
    };
    
    DetailedValidationResult validateConfigDetailed(const std::vector<ConfigEntry>& entries) {
        DetailedValidationResult result;
        result.valid = true;
        
        // Check for duplicate keys
        std::unordered_set<std::string> seenKeys;
        for (const auto& entry : entries) {
            std::string normalizedKey = validateAndNormalizeKey(entry.key);
            if (seenKeys.find(normalizedKey) != seenKeys.end()) {
                result.errors.push_back("Duplicate configuration key: " + entry.key);
                result.valid = false;
            }
            seenKeys.insert(normalizedKey);
        }
        
        // Validate value ranges for specific keys
        for (const auto& entry : entries) {
            std::string normalizedKey = validateAndNormalizeKey(entry.key);
            
            // Neuron count validation
            if (normalizedKey == "neuron_count") {
                if (std::holds_alternative<int>(entry.value)) {
                    int count = std::get<int>(entry.value);
                    if (count <= 0) {
                        result.errors.push_back("Neuron count must be positive: " + std::to_string(count));
                        result.valid = false;
                    } else if (count > 1000000) {
                        result.warnings.push_back("Large neuron count may impact performance: " + std::to_string(count));
                    }
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    int64_t count = std::get<int64_t>(entry.value);
                    if (count <= 0) {
                        result.errors.push_back("Neuron count must be positive: " + std::to_string(count));
                        result.valid = false;
                    } else if (count > 1000000) {
                        result.warnings.push_back("Large neuron count may impact performance: " + std::to_string(count));
                    }
                }
            }
            
            // Connection probability validation
            if (normalizedKey == "connection_probability") {
                if (std::holds_alternative<double>(entry.value)) {
                    double prob = std::get<double>(entry.value);
                    if (prob < 0.0 || prob > 1.0) {
                        result.errors.push_back("Connection probability must be between 0 and 1: " + std::to_string(prob));
                        result.valid = false;
                    } else if (prob < 0.01) {
                        result.suggestions["low_connectivity"] = "Very low connection probability may result in poor connectivity. Consider increasing to at least 0.05.";
                    } else if (prob > 0.5) {
                        result.warnings.push_back("High connection probability may create overly dense network.");
                    }
                }
            }
            
            // Timestep validation
            if (normalizedKey == "simulation_timestep") {
                if (std::holds_alternative<double>(entry.value)) {
                    double timestep = std::get<double>(entry.value);
                    if (timestep <= 0.0 || timestep > 1.0) {
                        result.errors.push_back("Simulation timestep must be positive and less than 1.0s: " + std::to_string(timestep));
                        result.valid = false;
                    } else if (timestep < 0.001) {
                        result.warnings.push_back("Very small timestep may impact performance. Consider increasing to at least 0.001s.");
                    }
                }
            }
            
            // Plasticity weights validation
            if (normalizedKey == "stdp_ltp_weight" || normalizedKey == "stdp_ltd_weight") {
                if (std::holds_alternative<double>(entry.value)) {
                    double weight = std::get<double>(entry.value);
                    if (weight < 0.0 || weight > 1.0) {
                        result.errors.push_back("Plasticity weight must be between 0 and 1: " + std::to_string(weight));
                        result.valid = false;
                    } else if (weight < 0.001) {
                        result.suggestions["low_plasticity"] = "Very low plasticity may result in slow learning. Consider increasing to at least 0.01.";
                    } else if (weight > 0.1) {
                        result.warnings.push_back("High plasticity may cause unstable learning dynamics.");
                    }
                }
            }
            
            // Performance-related validations
            if (normalizedKey == "neuron_count" && std::holds_alternative<int>(entry.value)) {
                int count = std::get<int>(entry.value);
                if (count > 10000) {
                    result.suggestions["performance"] = "For " + std::to_string(count) + " neurons, consider using performance optimizations or distributed computing.";
                }
            }
        }
        
        return result;
    }
    
    // Configuration optimization suggestions
    std::vector<std::string> getOptimizationSuggestions(const std::vector<ConfigEntry>& entries) {
        std::vector<std::string> suggestions;
        
        bool hasHighNeuronCount = false;
        bool hasHighConnectivity = false;
        bool hasHighPlasticity = false;
        bool hasLowTimestep = false;
        
        for (const auto& entry : entries) {
            std::string normalizedKey = validateAndNormalizeKey(entry.key);
            
            if (normalizedKey == "neuron_count") {
                if (std::holds_alternative<int>(entry.value) && std::get<int>(entry.value) > 10000) {
                    hasHighNeuronCount = true;
                }
            } else if (normalizedKey == "connection_probability") {
                if (std::holds_alternative<double>(entry.value) && std::get<double>(entry.value) > 0.5) {
                    hasHighConnectivity = true;
                }
            } else if (normalizedKey == "stdp_ltp_weight" || normalizedKey == "stdp_ltd_weight") {
                if (std::holds_alternative<double>(entry.value) && std::get<double>(entry.value) > 0.1) {
                    hasHighPlasticity = true;
                }
            } else if (normalizedKey == "simulation_timestep") {
                if (std::holds_alternative<double>(entry.value) && std::get<double>(entry.value) < 0.001) {
                    hasLowTimestep = true;
                }
            }
        }
        
        if (hasHighNeuronCount) {
            suggestions.push_back("For large networks (>10,000 neurons), consider enabling performance optimizations like sparse connectivity or memory pools.");
        }
        
        if (hasHighConnectivity) {
            suggestions.push_back("High connection probability may create computational bottlenecks. Consider using smaller regions or sparse connectivity.");
        }
        
        if (hasHighPlasticity) {
            suggestions.push_back("High plasticity may cause unstable dynamics. Consider using adaptive plasticity or regularization.");
        }
        
        if (hasLowTimestep) {
            suggestions.push_back("Small timestep may impact performance. Consider increasing timestep or using adaptive time-stepping.");
        }
        
        if (entries.empty()) {
            suggestions.push_back("No configuration found. Using default values. Consider creating a configuration file for optimal performance.");
        }
        
        return suggestions;
    }
    
    // Configuration export/import utilities
    std::string exportConfigToJSON(const std::vector<ConfigEntry>& entries) {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        return jsonConfig.dump(2);
    }
    
    bool importConfigFromJSON(const std::string& json, std::vector<ConfigEntry>& entries) {
        try {
            auto jsonDoc = nlohmann::json::parse(json);
            entries.clear();
            
            for (auto& [key, value] : jsonDoc.items()) {
                try {
                    ConfigEntry entry;
                    entry.key = key;
                    entry.value = jsonToConfigValue(value);
                    entry.source = ConfigSource::File;
                    entries.push_back(entry);
                } catch (const std::exception& e) {
                    // Skip invalid entries but continue processing
                    continue;
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Configuration comparison utilities
    struct ConfigDifference {
        std::string key;
        std::string oldValue;
        std::string newValue;
        bool isAddition;
        bool isRemoval;
    };
    
    std::vector<ConfigDifference> compareConfigs(
        const std::vector<ConfigEntry>& config1,
        const std::vector<ConfigEntry>& config2) {
        
        std::vector<ConfigDifference> differences;
        
        std::unordered_map<std::string, ConfigValue> config1Map;
        for (const auto& entry : config1) {
            config1Map[entry.key] = entry.value;
        }
        
        std::unordered_map<std::string, ConfigValue> config2Map;
        for (const auto& entry : config2) {
            config2Map[entry.key] = entry.value;
        }
        
        // Check for additions and modifications
        for (const auto& [key, value] : config2Map) {
            auto it = config1Map.find(key);
            if (it == config1Map.end()) {
                // New entry
                differences.push_back({
                    key,
                    "",
                    configValueToString(value),
                    true,
                    false
                });
            } else if (it->second != value) {
                // Modified entry
                differences.push_back({
                    key,
                    configValueToString(it->second),
                    configValueToString(value),
                    false,
                    false
                });
            }
        }
        
        // Check for removals
        for (const auto& [key, value] : config1Map) {
            if (config2Map.find(key) == config2Map.end()) {
                differences.push_back({
                    key,
                    configValueToString(value),
                    "",
                    false,
                    true
                });
            }
        }
        
        return differences;
    }

    // Performance monitoring integration
    class PerformanceProfiler {
    private:
        std::chrono::steady_clock::time_point startTime;
        std::unordered_map<std::string, std::chrono::duration<double>> operationTimes;
        std::unordered_map<std::string, size_t> operationCounts;
        size_t totalMemoryUsage;
        
    public:
        PerformanceProfiler() : startTime(std::chrono::steady_clock::now()), totalMemoryUsage(0) {}
        
        void startOperation(const std::string& name) {
            // This would track individual operation times
            operationTimes[name] = std::chrono::steady_clock::now() - startTime;
            operationCounts[name]++;
        }
        
        void endOperation(const std::string& name) {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = endTime - startTime;
            operationTimes[name] += duration;
        }
        
        double getOperationTime(const std::string& name) const {
            auto it = operationTimes.find(name);
            return (it != operationTimes.end()) ? 
                std::chrono::duration<double>(it->second).count() : 0.0;
        }
        
        size_t getOperationCount(const std::string& name) const {
            auto it = operationCounts.find(name);
            return (it != operationCounts.end()) ? it->second : 0;
        }
        
        double getAverageTime(const std::string& name) const {
            size_t count = getOperationCount(name);
            if (count == 0) return 0.0;
            return getOperationTime(name) / count;
        }
        
        void setMemoryUsage(size_t bytes) {
            totalMemoryUsage = bytes;
        }
        
        size_t getMemoryUsage() const {
            return totalMemoryUsage;
        }
        
        double getTotalTime() const {
            return std::chrono::duration<double>(
                std::chrono::steady_clock::now() - startTime
            ).count();
        }
        
        std::string getPerformanceReport() const {
            std::stringstream report;
            report << "=== Performance Profile ===\n";
            report << "Total time: " << getTotalTime() << "s\n";
            report << "Memory usage: " << getMemoryUsage() << " bytes\n\n";
            
            report << "Operations:\n";
            for (const auto& [name, time] : operationTimes) {
                double avgTime = getAverageTime(name);
                size_t count = getOperationCount(name);
                report << "  " << name << ": " << count << " calls, "
                       << avgTime << "s average, " << time.count() << "s total\n";
            }
            
            return report.str();
        }
        
        nlohmann::json getPerformanceMetrics() const {
            nlohmann::json metrics;
            
            metrics["total_time"] = getTotalTime();
            metrics["memory_usage"] = getMemoryUsage();
            metrics["operations"] = nlohmann::json::object();
            
            for (const auto& [name, time] : operationTimes) {
                nlohmann::json opMetrics;
                opMetrics["count"] = getOperationCount(name);
                opMetrics["total_time"] = time.count();
                opMetrics["average_time"] = getAverageTime(name);
                opMetrics["calls_per_second"] = getOperationCount(name) / getTotalTime();
                
                metrics["operations"][name] = opMetrics;
            }
            
            return metrics;
        }
    };

    // Memory usage monitoring
    class MemoryMonitor {
    public:
        struct MemoryStats {
            size_t totalBytes;
            size_t usedBytes;
            size_t availableBytes;
            double usagePercentage;
        };
        
        static MemoryStats getSystemMemoryStats() {
            MemoryStats stats;
            
            struct sysinfo memInfo;
            if (sysinfo(&memInfo) == 0) {
                stats.totalBytes = memInfo.totalram;
                stats.usedBytes = memInfo.totalram - memInfo.freeram;
                stats.availableBytes = memInfo.freeram;
                stats.usagePercentage = (static_cast<double>(stats.usedBytes) / stats.totalBytes) * 100.0;
            } else {
                stats.totalBytes = 0;
                stats.usedBytes = 0;
                stats.availableBytes = 0;
                stats.usagePercentage = 0.0;
            }
            
            return stats;
        }
        
        static std::string getMemoryReport() {
            auto stats = getSystemMemoryStats();
            
            std::stringstream report;
            report << "=== Memory Usage ===\n";
            report << "Total memory: " << (stats.totalBytes / (1024 * 1024)) << " MB\n";
            report << "Used memory: " << (stats.usedBytes / (1024 * 1024)) << " MB\n";
            report << "Available memory: " << (stats.availableBytes / (1024 * 1024)) << " MB\n";
            report << "Usage percentage: " << stats.usagePercentage << "%\n";
            
            return report.str();
        }
        
        static nlohmann::json getMemoryMetrics() {
            auto stats = getSystemMemoryStats();
            
            nlohmann::json metrics;
            metrics["total_bytes"] = stats.totalBytes;
            metrics["used_bytes"] = stats.usedBytes;
            metrics["available_bytes"] = stats.availableBytes;
            metrics["usage_percentage"] = stats.usagePercentage;
            
            return metrics;
        }
    };

    // CPU performance monitoring
    class CPUMonitor {
    public:
        struct CPUStats {
            double utilizationPercentage;
            size_t totalCores;
            std::chrono::duration<double> idleTime;
            std::chrono::duration<double> totalTime;
        };
        
        static CPUStats getCPUStats() {
            CPUStats stats;
            stats.utilizationPercentage = 0.0;
            stats.totalCores = std::thread::hardware_concurrency();
            stats.idleTime = std::chrono::duration<double>::zero();
            stats.totalTime = std::chrono::duration<double>::zero();
            
            // Note: This is a simplified implementation
            // Real CPU monitoring would require platform-specific APIs
            return stats;
        }
        
        static std::string getCPUReport() {
            auto stats = getCPUStats();
            
            std::stringstream report;
            report << "=== CPU Usage ===\n";
            report << "Cores: " << stats.totalCores << "\n";
            report << "Utilization: " << stats.utilizationPercentage << "%\n";
            
            return report.str();
        }
        
        static nlohmann::json getCPUMetrics() {
            auto stats = getCPUStats();
            
            nlohmann::json metrics;
            metrics["cores"] = stats.totalCores;
            metrics["utilization_percentage"] = stats.utilizationPercentage;
            
            return metrics;
        }
    };
}} // namespace