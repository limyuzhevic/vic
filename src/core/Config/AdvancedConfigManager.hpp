#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

namespace nlm {

// ============================================================================
// Global Configuration Access
// ============================================================================

/**
 * @brief Get the global ConfigManager instance
 * 
 * @return Reference to the global ConfigManager
 */
inline ConfigManager& getGlobalConfigManager() {
    return ConfigManager::getInstance();
}

/**
 * @brief Initialize the global configuration system
 * 
 * @param configFile Path to configuration file (optional)
 * @param cliArgs Command line arguments (argc, argv)
 */
inline void initializeGlobalConfig(const std::string& configFile = "", int argc = 0, char** argv = nullptr) {
    getGlobalConfigManager().initialize(configFile, argc, argv);
}

/**
 * @brief Get the global configuration
 * 
 * @return Shared pointer to the global configuration
 */
inline std::shared_ptr<Config> getGlobalConfig() {
    return getGlobalConfigManager().getConfig();
}

/**
 * @brief Load default configuration for a specific NLM phase
 * 
 * @param phase Phase number (2-6)
 */
inline void loadPhaseDefaults(int phase) {
    getGlobalConfigManager().loadPhaseDefaults(phase);
}

/**
 * @brief Convenience function to get a configuration value
 * 
 * @tparam T Type of value to retrieve
 * @param key Configuration key
 * @param defaultValue Default value if key not found
 * @return Retrieved value
 */
template<typename T>
inline T getConfigOr(const std::string& key, const T& defaultValue) {
    return getGlobalConfigManager().getOr<T>(key, defaultValue);
}

/**
 * @brief Check if a configuration key exists
 * 
 * @param key Configuration key
 * @return true if key exists, false otherwise
 */
inline bool hasConfig(const std::string& key) {
    return getGlobalConfigManager().has(key);
}

/**
 * @brief Set a configuration value with validation
 * 
 * @param key Configuration key
 * @param value Configuration value
 * @param description Human-readable description
 * @return true if valid and set, false otherwise
 */
inline bool setConfigValidated(const std::string& key, const std::string& value, 
                             const std::string& description = "") {
    return getGlobalConfigManager().setValidated(key, value, description);
}

/**
 * @brief Print configuration to console
 */
inline void printConfig() {
    std::cout << getGlobalConfigManager().getSummary() << std::endl;
}

/**
 * @brief Save configuration to file
 * 
 * @param filepath Path to save configuration
 * @return true if saved successfully, false otherwise
 */
inline bool saveConfig(const std::string& filepath) {
    return getGlobalConfigManager().saveToFile(filepath);
}

/**
 * @brief Reset configuration to defaults
 */
inline void resetConfig() {
    getGlobalConfigManager().reset();
}

// ============================================================================
// Enhanced ConfigManager with Factory Pattern
// ============================================================================

/**
 * @brief Advanced ConfigManager with factory pattern for component creation
 */
class AdvancedConfigManager : public ConfigManager {
public:
    AdvancedConfigManager() = default;
    ~AdvancedConfigManager() = default;
    
    /**
     * @brief Register a factory for creating components from configuration
     * 
     * @tparam ComponentType Type of component to create
     * @param componentType Type identifier for the component
     * @param factory Function that creates a component from configuration
     */
    template<typename ComponentType>
    void registerFactory(const std::string& componentType,
                        std::function<std::shared_ptr<ComponentType>(std::shared_ptr<Config>)> factory) {
        auto factoryAny = std::make_shared<std::function<std::shared_ptr<void>(std::shared_ptr<Config>)>>(
            [factory](std::shared_ptr<Config> config) -> std::shared_ptr<void> {
                return std::shared_ptr<void>(factory(config));
            }
        );
        factories_[componentType] = factoryAny;
    }
    
    /**
     * @brief Create a component using registered factory
     * 
     * @tparam ComponentType Type of component to create
     * @param componentType Type identifier
     * @return Created component or nullptr if factory not found
     */
    template<typename ComponentType>
    std::shared_ptr<ComponentType> createComponent(const std::string& componentType) {
        auto it = factories_.find(componentType);
        if (it == factories_.end()) {
            return nullptr;
        }
        
        // Type-safe factory call (simplified - in real implementation would use type erasure properly)
        return std::dynamic_pointer_cast<ComponentType>(it->second(getConfig()));
    }
    
    /**
     * @brief Get all registered component types
     * 
     * @return Vector of registered component type names
     */
    std::vector<std::string> getRegisteredTypes() const {
        std::vector<std::string> types;
        for (const auto& pair : factories_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    /**
     * @brief Create a complete system configuration for an experiment
     * 
     * @param experimentType Type of experiment
     * @param config Parameters for experiment setup
     * @return Complete configuration for the system
     */
    std::shared_ptr<Config> createExperimentConfig(
        const std::string& experimentType,
        const std::map<std::string, std::string>& config
    ) {
        // Reset and load basic configuration
        reset();
        loadPhaseDefaults(4);  // Default to Phase 4 configuration
        
        // Override with experiment-specific configuration
        for (const auto& pair : config) {
            setValidated(pair.first, pair.second, "Experiment configuration");
        }
        
        return getConfig();
    }
    
    /**
     * @brief Validate configuration for a specific use case
     * 
     * @param useCase Type of use case (e.g., "demo", "experiment", "production")
     * @return true if configuration is valid for the use case, false otherwise
     */
    bool validateForUseCase(const std::string& useCase) const {
        auto config = getConfig();
        
        if (useCase == "demo") {
            // Demo requirements: reasonable resource usage
            if (config->getOr<int64_t>("neuron_count", 1000) > 10000) return false;
            if (config->getOr<int64_t>("region_count", 2) > 10) return false;
            return true;
        } else if (useCase == "experiment") {
            // Experiment requirements: balanced configuration
            if (config->getOr<int64_t>("neuron_count", 1000) == 0) return false;
            if (config->getOr<float>("connection_probability", 0.1) < 0.0f || 
                config->getOr<float>("connection_probability", 0.1) > 1.0f) return false;
            return true;
        } else if (useCase == "production") {
            // Production requirements: optimal performance settings
            if (config->getOr<int64_t>("neuron_count", 1000) < 1000) return false;
            if (config->getOr<double>("simulation_timestep", 0.001) > 0.01) return false;
            return true;
        }
        
        return true; // Unknown use case, assume valid
    }
    
    /**
     * @brief Create configuration from command line arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Configuration object
     */
    static std::shared_ptr<Config> fromCommandLine(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    config->set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        
        return config;
    }
    
    /**
     * @brief Create configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @return Configuration object
     */
    static std::shared_ptr<Config> fromJSON(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open configuration file: " + filepath);
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Simple JSON parsing for key-value pairs (simplified)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Remove quotes, whitespace
                key.erase(0, key.find_first_not_of(" \"""""\t"));
                key.erase(key.find_last_not_of(" \"""""\t") + 1);
                value.erase(0, value.find_first_not_of(" \"""""\t"));
                value.erase(value.find_last_not_of(" \"""""\t") + 1);
                
                config->set(key, value, ConfigSource::File);
            }
        }
        
        return config;
    }
    
    /**
     * @brief Export configuration to JSON format
     * 
     * @param config Configuration to export
     * @return JSON string representation
     */
    static std::string toJSON(const std::shared_ptr<Config>& config) {
        std::ostringstream oss;
        oss << "{\n";
        
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            auto value = config->get<std::string>(key);
            
            oss << "  \"" << key << "\": \"" << (value.has_value() ? value.value() : "") << "\"";
            if (i < keys.size() - 1) oss << ",";
            oss << "\n";
        }
        
        oss << "}\n";
        return oss.str();
    }

private:
    std::map<std::string, std::shared_ptr<void>> factories_;
};

// ============================================================================
// Convenience Functions for Advanced Usage
// ============================================================================

/**
 * @brief Create an advanced configuration manager
 * 
 * @return AdvancedConfigManager instance
 */
inline AdvancedConfigManager& getAdvancedConfigManager() {
    static AdvancedConfigManager manager;
    return manager;
}

/**
 * @brief Get configuration for a specific experiment type
 * 
 * @param experimentType Type of experiment
 * @param customConfig Custom configuration overrides
 * @return Configuration for the experiment
 */
inline std::shared_ptr<Config> getExperimentConfig(
    const std::string& experimentType,
    const std::map<std::string, std::string>& customConfig = {}
) {
    return getAdvancedConfigManager().createExperimentConfig(experimentType, customConfig);
}

/**
 * @brief Validate current configuration for a use case
 * 
 * @param useCase Type of use case
 * @return true if valid, false otherwise
 */
inline bool validateCurrentConfig(const std::string& useCase) {
    return getAdvancedConfigManager().validateForUseCase(useCase);
}

/**
 * @brief Create component using factory pattern
 * 
 * @tparam ComponentType Type of component to create
 * @param componentType Type identifier
 * @return Created component
 */
template<typename ComponentType>
inline std::shared_ptr<ComponentType> createComponent(const std::string& componentType) {
    return getAdvancedConfigManager().template createComponent<ComponentType>(componentType);
}

/**
 * @brief Get list of all registered component types
 * 
 * @return Vector of component type names
 */
inline std::vector<std::string> getRegisteredComponentTypes() {
    return getAdvancedConfigManager().getRegisteredTypes();
}

} // namespace nlm
