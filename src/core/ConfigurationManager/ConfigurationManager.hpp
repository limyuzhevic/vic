#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <iostream>

namespace nlm {

// Configuration parameter definitions
struct ParameterInfo {
    std::string name;
    ConfigValue defaultValue;
    std::string description;
    ConfigSource source;
    double minValue;
    double maxValue;
    bool isRequired;
};

// Parameter types for validation
enum class ParameterType {
    Integer,
    Float,
    Boolean,
    String,
    Integer64,
    Double,
    VectorInt,
    VectorDouble,
    VectorString
};

// ConfigurationManager: Enhanced configuration management with validation
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept;
    
    // Configuration file loading
    bool loadFromFile(const std::string& filepath);
    
    // Command-line argument loading
    bool loadFromCommandLine(int argc, char** argv);
    
    // Get parameter with default value
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Parameter validation
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Print validation errors
    void printValidationErrors() const;
    
    // Print configuration summary
    void printSummary() const;
    
    // Print help documentation
    static void printHelp();
    
    // Add parameter definition
    void addParameter(const std::string& name, const ConfigValue& defaultValue, 
                     const std::string& description, ConfigSource source = ConfigSource::Default,
                     double minValue = std::numeric_limits<double>::lowest(),
                     double maxValue = std::numeric_limits<double>::max(),
                     bool isRequired = false);
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const;
    
    // Get parameter type
    ParameterType getParameterType(const std::string& name) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
    
// Get underlying config
    std::shared_ptr<Config> getConfig() const;
    
    // Get parameter type
    ParameterType getParameterType(const std::string& name) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& name) const;
    
private:
    struct Impl {
        std::shared_ptr<Config> config;
        std::unordered_map<std::string, ParameterInfo> parameterDefinitions;
        std::vector<std::string> validationErrors;
        std::string configFile;
        bool helpRequested;
        bool versionRequested;
        
        Impl() : config(std::make_shared<Config>()), helpRequested(false), versionRequested(false) {
            // Add default parameters with descriptions
            addDefaultParameters();
        }
        
        void addDefaultParameters() {
            // Core simulation parameters
            addParameter("random_seed", ConfigValue(static_cast<int64_t>(42)), 
                        "Random seed for reproducible experiments", ConfigSource::Default);
            
            addParameter("simulation_timestep", ConfigValue(0.001), 
                        "Simulation timestep in seconds", ConfigSource::Default, 1e-6, 0.1);
            
            addParameter("neuron_count", ConfigValue(static_cast<int64_t>(500)), 
                        "Number of neurons in the brain (smaller for faster testing)", ConfigSource::Default, 100, 10000);
            
            addParameter("region_count", ConfigValue(static_cast<int64_t>(1)), 
                        "Number of brain regions", ConfigSource::Default, 1, 10);
            
            addParameter("connection_probability", ConfigValue(0.15f), 
                        "Connection probability for random connectivity", ConfigSource::Default, 0.01f, 1.0f);
            
            // Plasticity parameters
            addParameter("stdp_ltp_weight", ConfigValue(0.02f), 
                        "STDP long-term potentiation weight", ConfigSource::Default, 0.001f, 0.1f);
            
            addParameter("stdp_ltd_weight", ConfigValue(0.015f), 
                        "STDP long-term depression weight", ConfigSource::Default, 0.001f, 0.1f);
            
            addParameter("stdp_tau", ConfigValue(20.0f), 
                        "STDP time constant", ConfigSource::Default, 1.0f, 100.0f);
            
            // Structural plasticity parameters
            addParameter("synaptogenesis_rate", ConfigValue(0.0001f), 
                        "Structural synaptogenesis rate", ConfigSource::Default, 1e-6f, 0.001f);
            
            addParameter("pruning_rate", ConfigValue(0.00001f), 
                        "Structural pruning rate", ConfigSource::Default, 1e-7f, 0.001f);
            
            // Experiment-specific parameters
            addParameter("max_simulation_steps", ConfigValue(1000), 
                        "Maximum simulation steps per experiment", ConfigSource::Default, 10, 100000);
            
            addParameter("learning_threshold", ConfigValue(0.01f), 
                        "Weight change threshold for learning detection", ConfigSource::Default, 1e-4f, 0.1f);
            
            addParameter("spike_threshold", ConfigValue(1.0f), 
                        "Neuron membrane potential threshold for spiking", ConfigSource::Default, 0.0f, 10.0f);
            
            // Input/output parameters
            addParameter("input_strength", ConfigValue(50.0f), 
                        "Strength of injected current for input stimulation", ConfigSource::Default, 0.0f, 100.0f);
            
            addParameter("output_neurons", ConfigValue(10), 
                        "Number of output neurons to stimulate", ConfigSource::Default, 1, 100);
            
            // Logging parameters
            addParameter("log_level", ConfigValue(static_cast<int>(2)), 
                        "Log level (0=Error, 1=Warning, 2=Info, 3=Debug)", ConfigSource::Default, 0, 3);
            
            addParameter("verbose", ConfigValue(false), 
                        "Enable verbose logging", ConfigSource::Default);
            
            addParameter("log_filename", ConfigValue("nlm.log"), 
                        "Filename for log output (empty for console only)", ConfigSource::Default);
        }
        
        void addParameter(const std::string& name, const ConfigValue& defaultValue,
                         const std::string& description, ConfigSource source,
                         double minValue, double maxValue, bool isRequired);
    };
    
    std::vector<std::string> getValidationErrors() const;
    
    void printValidationErrors() const;

} // namespace nlm