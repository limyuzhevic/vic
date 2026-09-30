// Experiment configuration parameters

#pragma once

#include <string>
#include <map>

namespace nlm {

// Experiment configuration structure
struct ExperimentConfig {
    // Experiment type and identification
    std::string experiment_mode;
    std::string experiment_name;
    double experiment_duration;
    std::string experiment_objective;
    
    // Configuration and execution
    std::string experiment_config_file;
    size_t observation_interval;
    std::string reward_mode;
    std::string environment_config_file;
    
    // Runtime parameters
    std::map<std::string, std::string> experiment_parameters;
    
    ExperimentConfig() : experiment_mode("standard"), experiment_duration(1000.0),
                        observation_interval(10), reward_mode("sparse") {}
};

// Experiment configuration manager
class ConfigExperiment {
public:
    ConfigExperiment() = default;
    ~ConfigExperiment() = default;
    
    // Load from Config object
    void loadFromConfig(const Config& config);
    
    // Save to Config object
    void saveToConfig(Config& config) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get parameters
    const ExperimentConfig& get() const { return params; }
    ExperimentConfig& get() { return params; }
    
    // Set parameters
    void set(const ExperimentConfig& e) { params = e; }
    
    // Update individual parameter
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Get parameter
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
private:
    ExperimentConfig params;
};

} // namespace nlm