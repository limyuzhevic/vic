#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager(std::shared_ptr<Config> config)
        : config(config), verboseLevel(1) {
    }
    
    // Set configuration values with validation
    void setBool(const std::string& key, bool value, const std::string& description = "") {
        config->set(key, value, ConfigSource::Runtime);
        logSetting(key, value, description);
    }
    
    void setInt(const std::string& key, int64_t value, const std::string& description = "") {
        config->set(key, value, ConfigSource::Runtime);
        logSetting(key, value, description);
    }
    
    void setFloat(const std::string& key, float value, const std::string& description = "") {
        config->set(key, value, ConfigSource::Runtime);
        logSetting(key, value, description);
    }
    
    void setString(const std::string& key, const std::string& value, const std::string& description = "") {
        config->set(key, value, ConfigSource::Runtime);
        logSetting(key, value, description);
    }
    
    // Get configuration values with type safety
    bool getBool(const std::string& key, bool defaultValue = false) const {
        auto value = config->get<bool>(key);
        return value.has_value() ? *value : defaultValue;
    }
    
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const {
        auto value = config->get<int64_t>(key);
        return value.has_value() ? *value : defaultValue;
    }
    
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        auto value = config->get<float>(key);
        return value.has_value() ? *value : defaultValue;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto value = config->get<std::string>(key);
        return value.has_value() ? *value : defaultValue;
    }
    
    // Convenience methods for common configuration patterns
    void configureForPerformance(bool enableSimd = true, bool enableMultithreading = true) {
        setBool("performance.simd", enableSimd, "Enable SIMD instructions");
        setBool("performance.multithreading", enableMultithreading, "Enable multithreading");
        setBool("performance.optimization", true, "Enable all optimizations");
    }
    
    void configureForAccuracy(bool enableHighPrecision = true) {
        setBool("accuracy.high_precision", enableHighPrecision, "Enable high precision calculations");
        setFloat("accuracy.tolerance", 1e-6f, "Precision tolerance");
    }
    
    void configureForSpeed(bool enableAllOptimizations = true, int threadCount = 0) {
        configureForPerformance(true, threadCount > 0);
        setBool("speed.enable_all_optimizations", enableAllOptimizations, "Enable all speed optimizations");
        if (threadCount > 0) {
            setInt("speed.thread_count", threadCount, "Number of threads for parallel processing");
        }
    }
    
    void configureForDebug(bool enableDebugOutput = true, bool enableProfiling = true) {
        setBool("debug.enable_output", enableDebugOutput, "Enable debug output");
        setBool("debug.enable_profiling", enableProfiling, "Enable performance profiling");
        setInt("debug.log_level", verboseLevel, "Debug log level");
    }
    
    // Validation methods
    bool validate() const {
        bool isValid = true;
        
        // Check required parameters
        if (!config->has("random_seed")) {
            NLM_LOG_WARNING("Missing required parameter: random_seed");
            isValid = false;
        }
        
        if (!config->has("simulation_timestep")) {
            NLM_LOG_WARNING("Missing required parameter: simulation_timestep");
            isValid = false;
        }
        
        if (!config->has("neuron_count")) {
            NLM_LOG_WARNING("Missing required parameter: neuron_count");
            isValid = false;
        }
        
        // Validate parameter ranges
        auto timestep = getFloat("simulation_timestep");
        if (timestep <= 0.0f || timestep > 1.0f) {
            NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be between 0 and 1.");
            isValid = false;
        }
        
        auto neurons = getInt("neuron_count");
        if (neurons <= 0) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neurons) + ". Must be positive.");
            isValid = false;
        }
        
        if (neurons > 1000000) {
            NLM_LOG_WARNING("Very large neuron count may impact performance. Consider using --performance flag.");
        }
        
        return isValid;
    }
    
    // Configuration profiles
    enum class Profile {
        Default,
        Performance,
        Accuracy,
        Debug,
        Minimal
    };
    
    void applyProfile(Profile profile) {
        // Clear runtime settings
        clearRuntimeSettings();
        
        switch (profile) {
            case Profile::Default:
                applyDefaultProfile();
                break;
            case Profile::Performance:
                applyPerformanceProfile();
                break;
            case Profile::Accuracy:
                applyAccuracyProfile();
                break;
            case Profile::Debug:
                applyDebugProfile();
                break;
            case Profile::Minimal:
                applyMinimalProfile();
                break;
        }
    }
    
    // Configuration import/export
    bool saveToFile(const std::string& filename) const {
        return config->saveToFile(filename);
    }
    
    bool loadFromFile(const std::string& filename) {
        bool success = config->loadFromFile(filename);
        if (success) {
            NLM_LOG_INFO("Configuration loaded from: " + filename);
        }
        return success;
    }
    
    // Configuration comparison and diffing
    std::string diffWith(const ConfigurationManager& other) const {
        std::stringstream ss;
        
        auto myKeys = config->getKeys();
        auto otherKeys = other.config->getKeys();
        
        ss << "Configuration differences:\n";
        
        for (const auto& key : myKeys) {
            if (other.config->has(key)) {
                auto myValue = config->get<std::string>(key);
                auto otherValue = other.config->get<std::string>(key);
                if (myValue != otherValue) {
                    ss << "  " << key << ": " << *myValue << " vs " << *otherValue << "\n";
                }
            } else {
                ss << "  Added key: " << key << "\n";
            }
        }
        
        for (const auto& key : otherKeys) {
            if (!config->has(key)) {
                ss << "  Removed key: " << key << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Statistics and reporting
    std::string getSummary() const {
        std::stringstream ss;
        ss << "=== Configuration Summary ===\n";
        ss << "Loaded configuration with " << config->getKeys().size() << " parameters.\n\n";
        
        for (const auto& key : config->getKeys()) {
            ss << key << ": ";
            auto value = config->get<std::string>(key);
            if (value) {
                ss << *value << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Convenience getters for common configurations
    std::string getConfigFile() const {
        return getString("config_file", "configs/default.cfg");
    }
    
    bool isVerbose() const {
        return verboseLevel > 1;
    }
    
    bool isDebug() const {
        return verboseLevel > 2;
    }
    
    int getThreadCount() const {
        int threads = getInt("thread_count", 0);
        return threads > 0 ? threads : std::thread::hardware_concurrency();
    }
    
private:
    void applyDefaultProfile() {
        setInt("random_seed", 42, "Random seed for reproducible results");
        setFloat("simulation_timestep", 0.001f, "Simulation timestep in seconds");
        setInt("neuron_count", 1000, "Number of neurons");
        setInt("region_count", 1, "Number of neural regions");
        setFloat("connection_probability", 0.15f, "Connection probability");
        setBool("performance.enabled", true, "Enable performance optimizations");
    }
    
    void applyPerformanceProfile() {
        applyDefaultProfile();
        setBool("performance.simd", true, "Enable SIMD instructions");
        setBool("performance.multithreading", true, "Enable multithreading");
        setBool("performance.optimization", true, "Enable all optimizations");
        setInt("performance.thread_count", -1, "Automatic thread detection");
    }
    
    void applyAccuracyProfile() {
        applyDefaultProfile();
        setFloat("accuracy.tolerance", 1e-6f, "High precision tolerance");
        setBool("accuracy.high_precision", true, "Enable high precision calculations");
        setInt("accuracy.iterations", 100, "Number of precision iterations");
    }
    
    void applyDebugProfile() {
        applyDefaultProfile();
        setBool("debug.enabled", true, "Enable debug output");
        setBool("debug.profiling", true, "Enable performance profiling");
        setBool("debug.logging", true, "Enable detailed logging");
        setInt("debug.log_level", 3, "Debug log level (3=trace)");
    }
    
    void applyMinimalProfile() {
        setInt("neuron_count", 100, "Minimal neuron count for quick testing");
        setFloat("simulation_timestep", 0.01f, "Coarse timestep for speed");
        setBool("performance.enabled", false, "Disable performance optimizations");
    }
    
    void clearRuntimeSettings() {
        // Remove runtime-only settings
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // In a real implementation, we would track which keys are runtime-only
            // For now, we'll just clear the config and let the defaults reapply
        }
        config->clear();
    }
    
    void logSetting(const std::string& key, const ConfigValue& value, const std::string& description) {
        if (verboseLevel > 2) {
            NLM_LOG_DEBUG("Set configuration: " + key + " = " + std::to_string(value) + 
                         (description.empty() ? "" : " (" + description + ")"));
        }
    }
    
    std::shared_ptr<Config> config;
    int verboseLevel;
};

} // namespace nlm