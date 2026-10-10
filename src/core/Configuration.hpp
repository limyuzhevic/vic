#pragma once

#include <string>
#include <memory>

namespace nlm {

// Forward declaration for the enhanced Config class
class Config;

// Function to create a new configuration
std::unique_ptr<Config> createDefaultConfig();

// Function to create a configuration from file
std::unique_ptr<Config> createConfigFromFile(const std::string& filepath);

// Function to create a configuration from command line arguments
std::unique_ptr<Config> createConfigFromArgs(int argc, char** argv);

// Main entry point for configuration management
class ConfigurationManager {
public:
    static ConfigurationManager& getInstance() {
        static ConfigurationManager instance;
        return instance;
    }
    
    // Create default configuration
    std::unique_ptr<Config> createDefault() {
        return createDefaultConfig();
    }
    
    // Create configuration from file
    std::unique_ptr<Config> createFromFile(const std::string& filepath) {
        auto config = createConfigFromFile(filepath);
        if (!config || !config->isHealthy()) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
        return config;
    }
    
    // Create configuration from command line arguments
    std::unique_ptr<Config> createFromArgs(int argc, char** argv) {
        auto config = createConfigFromArgs(argc, argv);
        if (!config || !config->isHealthy()) {
            throw ConfigError("Failed to create configuration from command line arguments");
        }
        return config;
    }
    
    // Validate configuration
    bool validate(const Config& config) {
        return config.isHealthy();
    }
    
    // Merge configurations
    std::unique_ptr<Config> mergeConfigs(const Config& base, const Config& override) {
        auto merged = std::make_unique<Config>();
        
        // Start with base configuration
        for (const auto& entry : base.getKeys()) {
            auto val = base.get<std::string>(entry);
            if (val) {
                merged->set(entry, *val, ConfigSource::Runtime);
            }
        }
        
        // Override with override configuration
        for (const auto& entry : override.getKeys()) {
            auto val = override.get<std::string>(entry);
            if (val) {
                merged->set(entry, *val, ConfigSource::CommandLine);
            }
        }
        
        return merged;
    }
    
    // Save configuration
    bool saveConfig(const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }
    
private:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
};

// Convenience functions for common configuration scenarios

// Create a configuration for simulation
std::unique_ptr<Config> createSimulationConfig(
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1,
    float simulationTimestep = 0.001,
    uint64_t seed = 42
) {
    auto config = createDefaultConfig();
    config->set("neuron_count", neuronCount);
    config->set("region_count", regionCount);
    config->set("connection_probability", connectionProbability);
    config->set("simulation_timestep", simulationTimestep);
    config->set("random_seed", seed);
    return config;
}

// Create a configuration for visualization
std::unique_ptr<Config> createVisualizationConfig(
    bool enabled = false,
    float updateRate = 30.0,
    const std::string& outputPath = "nlm_visualization"
) {
    auto config = createDefaultConfig();
    config->set("visualization_enabled", enabled);
    config->set("visualization_update_rate", updateRate);
    config->set("visualization_output_path", outputPath);
    return config;
}

// Create a configuration for logging
std::unique_ptr<Config> createLoggingConfig(
    LogLevel logLevel = LogLevel::Info,
    bool logToFile = false,
    const std::string& logFilename = "nlm.log",
    size_t maxLogSize = 10000000
) {
    auto config = createDefaultConfig();
    config->set("log_level", static_cast<int>(logLevel));
    config->set("log_to_file", logToFile);
    config->set("log_filename", logFilename);
    config->set("max_log_size", maxLogSize);
    return config;
}

// Configuration template for common scenarios
struct ConfigTemplates {
    // Standard simulation template
    static std::unique_ptr<Config> standardSimulation() {
        return createSimulationConfig(
            1000,      // neuron_count
            1,         // region_count
            0.1,       // connection_probability
            0.001,     // simulation_timestep
            42         // random_seed
        );
    }
    
    // Small network template
    static std::unique_ptr<Config> smallNetwork() {
        return createSimulationConfig(
            100,       // neuron_count
            1,         // region_count
            0.05,      // connection_probability
            0.001,     // simulation_timestep
            123        // random_seed
        );
    }
    
    // Large network template
    static std::unique_ptr<Config> largeNetwork() {
        return createSimulationConfig(
            10000,    // neuron_count
            4,        // region_count
            0.15,     // connection_probability
            0.001,    // simulation_timestep
            456        // random_seed
        );
    }
    
    // Development template
    static std::unique_ptr<Config> development() {
        auto config = createSimulationConfig(100, 1, 0.05, 0.001, 789);
        config->set("plasticity_learning_rate", 0.1);
        config->set("stdp_ltp_weight", 0.1);
        config->set("stdp_ltd_weight", 0.12);
        config->set("stdp_time_constant", 50.0);
        config->set("development_synaptogenesis_rate", 0.01);
        config->set("development_pruning_rate", 0.001);
        return config;
    }
    
    // Production template
    static std::unique_ptr<Config> production() {
        auto config = createSimulationConfig(1000, 2, 0.1, 0.001, 42);
        config->set("plasticity_learning_rate", 0.01);
        config->set("stdp_ltp_weight", 0.01);
        config->set("stdp_ltd_weight", 0.012);
        config->set("stdp_time_constant", 20.0);
        config->set("development_synaptogenesis_rate", 0.001);
        config->set("development_pruning_rate", 0.0001);
        config->set("visualization_enabled", false);
        config->set("log_level", static_cast<int>(LogLevel::Warning));
        return config;
    }
};

} // namespace nlm
