#pragma once

#include "Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class ConfigBuilder {
public:
    ConfigBuilder();
    ~ConfigBuilder();
    
    // Disable copying, enable moving
    ConfigBuilder(const ConfigBuilder&) = delete;
    ConfigBuilder& operator=(const ConfigBuilder&) = delete;
    ConfigBuilder(ConfigBuilder&&) noexcept;
    ConfigBuilder& operator=(ConfigBuilder&&) noexcept;
    
    // Set configuration values
    ConfigBuilder& set(const std::string& key, const std::string& value);
    ConfigBuilder& set(const std::string& key, int value);
    ConfigBuilder& set(const std::string& key, int64_t value);
    ConfigBuilder& set(const std::string& key, double value);
    ConfigBuilder& set(const std::string& key, bool value);
    ConfigBuilder& set(const std::string& key, const std::vector<int>& value);
    ConfigBuilder& set(const std::string& key, const std::vector<double>& value);
    ConfigBuilder& set(const std::string& key, const std::vector<std::string>& value);
    
    // Set configuration value with source
    ConfigBuilder& set(const std::string& key, const std::string& value, ConfigSource source);
    ConfigBuilder& set(const std::string& key, int value, ConfigSource source);
    ConfigBuilder& set(const std::string& key, double value, ConfigSource source);
    ConfigBuilder& set(const std::string& key, bool value, ConfigSource source);
    
    // Set default values (more efficient for multiple sets)
    ConfigBuilder& setDefault(const std::string& key, const std::string& value);
    ConfigBuilder& setDefault(const std::string& key, int value);
    ConfigBuilder& setDefault(const std::string& key, double value);
    ConfigBuilder& setDefault(const std::string& key, bool value);
    
    // Set STDP-specific parameters
    ConfigBuilder& setStdpParameters(float ltpWeight, float ltdWeight, float tau);
    
    // Set structural plasticity parameters
    ConfigBuilder& setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate);
    
    // Set simulation parameters
    ConfigBuilder& setSimulationParameters(int neuronCount, int regionCount, float connectionProbability, double timestep);
    
    // Build the configuration
    std::shared_ptr<Config> build();
    
    // Build and load from file
    std::shared_ptr<Config> buildAndLoad(const std::string& filepath);
    
    // Reset builder
    void clear();
    
    // Get current configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for common configurations
namespace ConfigPresets {
    std::shared_ptr<Config> createDefaultConfig();
    std::shared_ptr<Config> createMinimalConfig();
    std::shared_ptr<Config> createHighPlasticityConfig();
    std::shared_ptr<Config> createLowPlasticityConfig();
    std::shared_ptr<Config> createResearchConfig();
}

} // namespace nlm