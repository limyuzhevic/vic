#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlm {

class Config;

class ExperimentConfig {
public:
    ExperimentConfig();
    explicit ExperimentConfig(const std::string& filename);
    ~ExperimentConfig();
    
    // Configuration loading and saving
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Configuration value access
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    void set(const std::string& key, const T& value);
    
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    uint64_t getUint64(const std::string& key) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration sections
    struct {
        // Environment configuration
        std::string environment_name;
        float environment_width;
        float environment_height;
        
        // Brain configuration
        size_t brain_neuron_count;
        size_t brain_region_count;
        float brain_connection_probability;
        uint64_t brain_random_seed;
        float brain_simulation_timestep;
        float brain_stdp_ltp_weight;
        float brain_stdp_ltd_weight;
        float brain_stdp_tau;
        float brain_synaptogenesis_rate;
        float brain_pruning_rate;
        
        // Experiment configuration
        size_t experiment_num_episodes;
        size_t experiment_steps_per_episode;
        size_t experiment_num_trials;
        std::string experiment_name;
        std::string experiment_description;
        
        // Logging configuration
        std::string log_filename;
        bool log_to_console;
        bool log_detailed;
        
        // Output configuration
        std::string output_directory;
        std::string output_format;
        bool save_metrics;
        bool save_brain_state;
        
        // Performance configuration
        float performance_threshold;
        size_t max_steps;
        float timeout_seconds;
        
    } settings;
    
    // Environment/brain configuration factories
    static std::shared_ptr<Config> createDefaultEnvironmentConfig();
    static std::shared_ptr<Config> createDefaultBrainConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    
    // Experiment configuration factories
    static std::shared_ptr<ExperimentConfig> createPhase3Config(int episodes = 5, int steps = 200);
    static std::shared_ptr<ExperimentConfig> createPhase4Config(size_t trials = 50);
    static std::shared_ptr<ExperimentConfig> createAblationConfig(const std::string& mechanism);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeDefaults();
    bool validateInternal() const;
};

} // namespace nlm
