// NLM Configuration Builder
// Provides structured configuration setup for Phase 2 neural computation

#ifndef NLM_CONFIG_BUILDER_HPP
#define NLM_CONFIG_BUILDER_HPP

#include "../core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Configuration parameter documentation
struct ConfigParameterInfo {
    std::string key;
    std::string type;
    std::string description;
    std::string category;
    ConfigValue defaultValue;
};

// Builder class for setting up configuration with defaults and validation
class ConfigBuilder {
public:
    ConfigBuilder(std::shared_ptr<Config> config);
    
    // Set default values for Phase 2 neural computation
    void applyDefaultPhase2Configuration();
    
    // Get parameter information for documentation
    static const std::vector<ConfigParameterInfo>& getParameterInfo();
    
    // Validate configuration values
    bool validate(const Config& config) const;
    
    // Log configuration summary
    static void logConfigurationSummary(const Config& config);
    
    // Get parameter by category
    static std::vector<std::string> getParametersByCategory(const std::string& category);
    
private:
    std::shared_ptr<Config> config_;
    
    // Initialize parameter information
    static void initializeParameterInfo(std::vector<ConfigParameterInfo>& parameters);
    
    // Category constants
    static constexpr const char* PARAM_CATEGORY_CORE = "Core Simulation Parameters";
    static constexpr const char* PARAM_CATEGORY_SYNAPSES = "Synaptic Plasticity Parameters";
    static constexpr const char* PARAM_CATEGORY_STRUCTUREAL = "Structural Plasticity Parameters";
    static constexpr const char* PARAM_CATEGORY_LOGGING = "Logging Configuration";
    static constexpr const char* PARAM_CATEGORY_EXPERIMENTS = "Experiment Configuration";
    static constexpr const char* PARAM_CATEGORY_PERFORMANCE = "Performance Optimizations";
};

// Convenience function to setup configuration
inline void setupPhase2Configuration(std::shared_ptr<Config> config) {
    ConfigBuilder builder(config);
    builder.applyDefaultPhase2Configuration();
    ConfigBuilder::logConfigurationSummary(*config);
}

} // namespace nlm

#endif // NLM_CONFIG_BUILDER_HPP