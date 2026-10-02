#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <functional>

namespace nlm {

// Brain configuration builder for creating custom brain configurations
class BrainConfigBuilder {
public:
    BrainConfigBuilder();
    ~BrainConfigBuilder();
    
    // Setters for configuration parameters with validation
    BrainConfigBuilder& setNeuronCount(size_t count);
    BrainConfigBuilder& setRegionCount(size_t count);
    BrainConfigBuilder& setConnectionProbability(float prob);
    BrainConfigBuilder& setTimestep(double timestep);
    BrainConfigBuilder& setRandomSeed(uint64_t seed);
    BrainConfigBuilder& enableNeuromodulation(bool enable);
    BrainConfigBuilder& enableDevelopment(bool enable);
    BrainConfigBuilder& enableCuriosity(bool enable);
    BrainConfigBuilder& enableStructuralPlasticity(bool enable);
    BrainConfigBuilder& enableMemory(bool enable);
    BrainConfigBuilder& enablePrediction(bool enable);
    BrainConfigBuilder& enableCognition(bool enable);
    
    // Custom configuration callback
    BrainConfigBuilder& setCustomConfig(const std::function<void(std::shared_ptr<Config>)>& callback);
    
    // Build the final configuration
    std::shared_ptr<Config> build() const;
    
    // Apply to an existing config
    void applyTo(std::shared_ptr<Config> config) const;
    
    // Default configurations
    static std::shared_ptr<Config> defaultConfig();
    static std::shared_ptr<Config> minimalConfig();
    static std::shared_ptr<Config> experimentalConfig();
    static std::shared_ptr<Config> researchConfig();
    
    // Python bindings registration
    static void registerPythonBindings(py::module_& m);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
