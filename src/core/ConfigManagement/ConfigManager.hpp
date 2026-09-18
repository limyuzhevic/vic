#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace nlm {

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Initialize with defaults
    void initializeDefaults();
    
    // Load configuration from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& jsonString);
    bool loadFromYaml(const std::string& yamlString);
    bool loadFromEnv(const std::unordered_map<std::string, std::string>& envVars);
    bool loadFromCommandLine(int argc, char** argv);
    
    // Save configuration
    bool saveToFile(const std::string& filepath) const;
    std::string exportToJson() const;
    std::string exportToYaml() const;
    
    // Configuration validation
    bool validate() const;
    bool validateSchema(const nlohmann::json& schema) const;
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const { return config_; }
    template<typename T>
    std::optional<T> get(const std::string& key) const { 
        return config_->getOr<T>(key, T{}); 
    }
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const { 
        return config_->getOr<T>(key, defaultValue); 
    }
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    void clear() { if (config_) config_->clear(); }
    std::vector<std::string> getKeys() const { 
        return config_ ? config_->getKeys() : std::vector<std::string>(); 
    }
    bool has(const std::string& key) const { 
        return config_ ? config_->has(key) : false; 
    }
    
    // Configuration file utilities
    static std::string detectFormat(const std::string& filepath);
    static bool isFormatSupported(const std::string& format);
    static std::string getFormatExtension(const std::string& format);
    
    // Default values for different phases
    void setPhase2Defaults();
    void setPhase3Defaults();
    void setPhase4Defaults();
    void setPhase6Defaults();
    
    // Checkpoint management
    bool saveCheckpoint(const std::string& prefix = "checkpoint");
    bool loadCheckpoint(const std::string& filepath = "checkpoint.json");
    
    // Performance monitoring
    struct PerformanceStats {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        double totalTimeMs = 0.0;
        uint64_t memoryUsage = 0;
        size_t stepCount = 0;
        
        double getElapsedTimeMs() const {
            if (endTime.time_since_epoch().count() == 0) {
                return std::chrono::duration<double, std::milli>(
                    std::chrono::steady_clock::now() - startTime
                ).count();
            }
            return std::chrono::duration<double, std::milli>(endTime - startTime).count();
        }
    };
    
    PerformanceStats& getPerformanceStats() { return perfStats_; }
    void resetPerformanceStats() { perfStats_ = PerformanceStats(); perfStats_.startTime = std::chrono::steady_clock::now(); }
    void endPerformanceMeasurement() { perfStats_.endTime = std::chrono::steady_clock::now(); }
    
    // Configuration statistics
    std::string getStatsSummary() const;
    
private:
    std::shared_ptr<Config> config_;
    PerformanceStats perfStats_;
    std::string currentPhase_ = "Phase2";
    
    // Internal helper functions
    bool loadFromFileInternal(const std::string& filepath);
    bool saveToFileInternal(const std::string& filepath) const;
    std::string generateConfigFileName(const std::string& prefix, const std::string& format) const;
    std::unordered_map<std::string, std::string> parseEnvironmentVariables() const;
};

} // namespace nlm
