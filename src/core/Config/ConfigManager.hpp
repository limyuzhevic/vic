// Configuration Manager
#pragma once

#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept = default;
    ConfigManager& operator=(ConfigManager&&) noexcept = default;
    
    // Singleton pattern for global access
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    // Configuration management
    void setConfig(const std::shared_ptr<Config>& config) { 
        config_ = config; 
    }
    
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Type-safe configuration getters with validation
    template<typename T>
    T getValidated(const std::string& key, T defaultValue, 
                   const std::string& errorMessage = "Configuration error") {
        if (!config_) {
            throw std::runtime_error(errorMessage + ": No configuration loaded");
        }
        
        auto value = config_->getOr<T>(key, defaultValue);
        if (!value) {
            throw std::runtime_error(errorMessage + ": Missing required configuration");
        }
        
        return *value;
    }
    
    // Convenience methods for common configuration patterns
    void loadFromFile(const std::string& filepath) {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
        
        if (!config_->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
    }
    
    void loadFromArgs(int argc, char** argv) {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
        
        if (!config_->loadFromArgs(argc, argv)) {
            throw std::runtime_error("Failed to load configuration from command line arguments");
        }
    }
    
    // Configuration validation
    bool validate() const {
        if (!config_) {
            return false; // No config loaded
        }
        
        // Check for required keys
        std::vector<std::string> required_keys = getRequiredKeys();
        for (const auto& key : required_keys) {
            if (!config_->has(key)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Configuration summary
    std::string getSummary() const {
        if (!config_) {
            return "No configuration loaded";
        }
        return config_->summary();
    }
    
    // Reset configuration
    void reset() {
        config_.reset();
    }
    
    // Application-specific configuration presets
    struct AppConfig {
        int neuronCount = 100;
        int regionCount = 1;
        double connectionProbability = 0.15;
        double stdpLtpWeight = 0.02;
        double stdpLtdWeight = 0.015;
        double synaptogenesisRate = 0.0001;
        double pruningRate = 0.00001;
        int maxSteps = 1000;
        double simulationTimestep = 0.001;
        std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
        bool enableCheckpointing = false;
        bool enableDevelopment = false;
        bool enableReplay = false;
    };
    
    AppConfig getAppConfig() const {
        AppConfig appConfig;
        if (config_) {
            appConfig.neuronCount = getValidated<int>("neuron_count", appConfig.neuronCount);
            appConfig.regionCount = getValidated<int>("region_count", appConfig.regionCount);
            appConfig.connectionProbability = getValidated<double>("connection_probability", appConfig.connectionProbability);
            appConfig.stdpLtpWeight = getValidated<double>("stdp_ltp_weight", appConfig.stdpLtpWeight);
            appConfig.stdpLtdWeight = getValidated<double>("stdp_ltd_weight", appConfig.stdpLtdWeight);
            appConfig.synaptogenesisRate = getValidated<double>("synaptogenesis_rate", appConfig.synaptogenesisRate);
            appConfig.pruningRate = getValidated<double>("pruning_rate", appConfig.pruningRate);
            appConfig.maxSteps = getValidated<int>("max_steps", appConfig.maxSteps);
            appConfig.simulationTimestep = getValidated<double>("simulation_timestep", appConfig.simulationTimestep);
            appConfig.enableCheckpointing = getValidated<bool>("enable_checkpointing", appConfig.enableCheckpointing);
            appConfig.enableDevelopment = getValidated<bool>("enable_development", appConfig.enableDevelopment);
            appConfig.enableReplay = getValidated<bool>("enable_replay", appConfig.enableReplay);
        }
        return appConfig;
    }
    
    // Set required keys for validation
    void setRequiredKeys(const std::vector<std::string>& keys) {
        required_keys_ = keys;
    }
    
    // Get current required keys
    const std::vector<std::string>& getRequiredKeys() const {
        return required_keys_;
    }
    
    // Configuration source management
    enum class SourcePriority {
        File = 1,
        CommandLine = 2,
        Default = 3,
        Runtime = 4
    };
    
    void setSourcePriority(SourcePriority priority) {
        source_priority_ = priority;
    }
    
protected:
    std::shared_ptr<Config> config_;
    std::vector<std::string> required_keys_;
    SourcePriority source_priority_ = SourcePriority::Default;
};

} // namespace nlm
