#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <chrono>

namespace nlm {

// Research configuration modes
enum class ResearchMode {
    Minimal,
    Standard,
    LargeScale,
    Developmental,
    Custom
};

// Research experiment configuration parameters
struct ResearchConfig {
    std::string name;
    std::string description;
    ResearchMode mode;
    
    // Core simulation parameters
    size_t neuron_count;
    size_t region_count;
    double simulation_timestep;
    uint64_t random_seed;
    
    // Plasticity settings
    double plasticity_learning_rate;
    double stdp_ltp_weight;
    double stdp_ltd_weight;
    double stdp_time_constant;
    
    // Development settings
    double development_synaptogenesis_rate;
    double development_pruning_rate;
    
    // Neuromodulation settings
    double dopamine_baseline;
    double dopamine_release_rate;
    
    // Reward settings
    double reward_discount_factor;
    
    // Environment settings
    std::string environment_name;
    size_t environment_width;
    size_t environment_height;
    
    // Logging settings
    std::string log_level;
    bool log_to_file;
    std::string log_filename;
    
    // Simulation settings
    size_t max_simulation_steps;
    double simulation_time_limit;
    
    // Visualization settings
    bool visualization_enabled;
    double visualization_update_rate;
    
    // Research-specific parameters
    size_t num_trials;
    size_t num_seeds;
    size_t max_concurrent_experiments;
    std::string output_format;
    bool save_checkpoints;
    size_t checkpoint_frequency;
    
    // Batch processing
    bool enable_batch_processing;
    size_t batch_size;
    bool enable_parallel_processing;
    size_t parallel_workers;
    
    // Performance optimization
    bool enable_performance_monitoring;
    bool enable_hardware_acceleration;
    bool optimize_for_scale;
    
    ResearchConfig();
    explicit ResearchConfig(const std::string& config_name);
};

// Advanced research configuration manager
class ResearchConfigManager {
public:
    ResearchConfigManager();
    ~ResearchConfigManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Load configuration from JSON/YAML (future)
    bool loadFromJSON(const std::string& filepath);
    
    // Convert to nlm::Config for compatibility
    nlm::Config toCoreConfig() const;
    
    // Create predefined research configurations
    static ResearchConfig createMinimalConfig();
    static ResearchConfig createStandardConfig();
    static ResearchConfig createLargeScaleConfig();
    static ResearchConfig createDevelopmentalConfig();
    static ResearchConfig createCustomConfig(const std::string& name);
    
    // Validate configuration
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Generate configuration report
    std::string generateReport() const;
    std::string generateValidationReport() const;
    
    // Configuration modification
    void setParameter(const std::string& key, const std::string& value);
    void setParameter(const std::string& key, size_t value);
    void setParameter(const std::string& key, double value);
    void setParameter(const std::string& key, bool value);
    
    // Parameter access
    std::string getParameter(const std::string& key) const;
    size_t getSizeParameter(const std::string& key) const;
    double getDoubleParameter(const std::string& key) const;
    bool getBoolParameter(const std::string& key) const;
    
    // Configuration manipulation
    void setMode(ResearchMode mode);
    ResearchMode getMode() const;
    void setName(const std::string& name);
    const std::string& getName() const;
    void setDescription(const std::string& desc);
    const std::string& getDescription() const;
    
    // Configuration comparison
    bool matches(const ResearchConfig& other) const;
    std::string hash() const;
    
    // Utility functions
    static std::string generateConfigName(const std::string& prefix = "config");
    static std::chrono::system_clock::time_point getTimestamp();
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    
    // Preset management
    static std::vector<std::string> getPresetNames();
    static bool isPreset(const std::string& name);
    static ResearchConfig getPresetConfig(const std::string& name);
    
    // Configuration cloning
    std::shared_ptr<ResearchConfigManager> clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void initializeDefaults();
    static std::string sanitizeKey(const std::string& key);
    static bool isValidLogLevel(const std::string& level);
    static bool isValidEnvironment(const std::string& env);
};

} // namespace nlm