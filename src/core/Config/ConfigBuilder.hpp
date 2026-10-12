// ConfigBuilder.hpp - Centralized configuration setup for Phase 6
#pragma once

#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <string>
#include <memory>

namespace nlm {

class ConfigBuilder {
public:
    // Default constructor
    ConfigBuilder();
    
    // Copy constructor and assignment operator are disabled
    ConfigBuilder(const ConfigBuilder&) = delete;
    ConfigBuilder& operator=(const ConfigBuilder&) = delete;
    
    // Move constructor and assignment operator
    ConfigBuilder(ConfigBuilder&&) noexcept = default;
    ConfigBuilder& operator=(ConfigBuilder&&) noexcept = default;
    
    // Destructor
    ~ConfigBuilder() = default;
    
    // Builder methods for fluent interface
    ConfigBuilder& setRandomSeed(uint64_t seed) {
        seed_ = seed;
        return *this;
    }
    
    ConfigBuilder& setTimestep(double timestep) {
        timestep_ = timestep;
        return *this;
    }
    
    ConfigBuilder& setNeuronCount(size_t count) {
        neuronCount_ = count;
        return *this;
    }
    
    ConfigBuilder& setRegionCount(size_t count) {
        regionCount_ = count;
        return *this;
    }
    
    ConfigBuilder& setConnectionProbability(float prob) {
        connectionProbability_ = prob;
        return *this;
    }
    
    ConfigBuilder& setStdpLtpWeight(float weight) {
        stdpLtpWeight_ = weight;
        return *this;
    }
    
    ConfigBuilder& setStdpLtdWeight(float weight) {
        stdpLtdWeight_ = weight;
        return *this;
    }
    
    ConfigBuilder& setStdpTau(float tau) {
        stdpTau_ = tau;
        return *this;
    }
    
    ConfigBuilder& setSynaptogenesisRate(float rate) {
        synaptogenesisRate_ = rate;
        return *this;
    }
    
    ConfigBuilder& setPruningRate(float rate) {
        pruningRate_ = rate;
        return *this;
    }
    
    // Phase 6 specific config
    ConfigBuilder& setPhase6NeuronCount(size_t count) {
        phase6Config_.neuronCount = count;
        return *this;
    }
    
    ConfigBuilder& setPhase6MaxSteps(uint64_t steps) {
        phase6Config_.maxSteps = steps;
        return *this;
    }
    
    ConfigBuilder& setPhase6EnableCheckpointing(bool enable) {
        phase6Config_.enableCheckpointing = enable;
        return *this;
    }
    
    ConfigBuilder& setPhase6EnableReplay(bool enable) {
        phase6Config_.enableReplay = enable;
        return *this;
    }
    
    ConfigBuilder& setPhase6EnableDevelopment(bool enable) {
        phase6Config_.enableDevelopment = enable;
        return *this;
    }
    
    ConfigBuilder& setPhase6CheckpointPath(const std::string& path) {
        phase6Config_.checkpointPath = path;
        return *this;
    }
    
    // Profile configuration methods
    ConfigBuilder& phase2() {
        return defaultPhase2();
    }
    
    ConfigBuilder& phase6() {
        return defaultPhase6();
    }
    
    ConfigBuilder& minimal() {
        return minimalConfig();
    }
    
    ConfigBuilder& maximal() {
        return maximalConfig();
    }
    
    // Build methods
    std::shared_ptr<Config> build() {
        auto config = std::make_shared<Config>();
        buildConfig(config);
        return config;
    }
    
    Phase6Config buildPhase6Config() {
        buildPhase6ConfigInternal(phase6Config_);
        return phase6Config_;
    }
    
    // Predefined profiles
    static ConfigBuilder defaultPhase2() {
        ConfigBuilder builder;
        builder.setRandomSeed(42)
               .setTimestep(0.001)
               .setNeuronCount(500)
               .setRegionCount(1)
               .setConnectionProbability(0.15f)
               .setStdpLtpWeight(0.02f)
               .setStdpLtdWeight(0.015f)
               .setStdpTau(20.0f)
               .setSynaptogenesisRate(0.0001f)
               .setPruningRate(0.00001f);
        return builder;
    }
    
    static ConfigBuilder defaultPhase6() {
        ConfigBuilder builder;
        builder.setRandomSeed(123)
               .setTimestep(0.001)
               .setNeuronCount(1000)
               .setRegionCount(1)
               .setConnectionProbability(0.1f)
               .setStdpLtpWeight(0.01f)
               .setStdpLtdWeight(0.012f)
               .setStdpTau(15.0f)
               .setSynaptogenesisRate(0.0001f)
               .setPruningRate(0.00001f)
               .setPhase6MaxSteps(10000)
               .setPhase6EnableCheckpointing(true)
               .setPhase6EnableReplay(true)
               .setPhase6EnableDevelopment(true);
        return builder;
    }
    
    static ConfigBuilder minimalConfig() {
        ConfigBuilder builder;
        builder.setRandomSeed(42)
               .setTimestep(0.001)
               .setNeuronCount(100)
               .setRegionCount(1)
               .setConnectionProbability(0.05f);
        return builder;
    }
    
    static ConfigBuilder maximalConfig() {
        ConfigBuilder builder;
        builder.setRandomSeed(123)
               .setTimestep(0.001)
               .setNeuronCount(5000)
               .setRegionCount(2)
               .setConnectionProbability(0.2f)
               .setStdpLtpWeight(0.05f)
               .setStdpLtdWeight(0.04f)
               .setStdpTau(30.0f)
               .setSynaptogenesisRate(0.001f)
               .setPruningRate(0.0001f)
               .setPhase6MaxSteps(50000)
               .setPhase6EnableCheckpointing(true)
               .setPhase6EnableReplay(true)
               .setPhase6EnableDevelopment(true);
        return builder;
    }
    
    // Validation methods
    bool validate() const {
        if (neuronCount_ == 0) {
            return false;
        }
        if (regionCount_ == 0) {
            return false;
        }
        if (connectionProbability_ < 0.0f || connectionProbability_ > 1.0f) {
            return false;
        }
        if (timestep_ <= 0.0) {
            return false;
        }
        return true;
    }
    
    std::string validateAndGetError() const {
        if (neuronCount_ == 0) return "neuronCount cannot be zero";
        if (regionCount_ == 0) return "regionCount cannot be zero";
        if (connectionProbability_ < 0.0f || connectionProbability_ > 1.0f) 
            return "connectionProbability must be between 0.0 and 1.0";
        if (timestep_ <= 0.0) return "timestep must be positive";
        return "";
    }
    
    // Get the current Phase6Config
    Phase6Config getPhase6Config() const {
        return phase6Config_;
    }
    
private:
    // Internal build helpers
    void buildConfig(std::shared_ptr<Config> config) {
        config->set("random_seed", seed_, ConfigSource::Default);
        config->set("simulation_timestep", timestep_, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount_), ConfigSource::Default);
        config->set("connection_probability", connectionProbability_, ConfigSource::Default);
        config->set("stdp_ltp_weight", stdpLtpWeight_, ConfigSource::Default);
        config->set("stdp_ltd_weight", stdpLtdWeight_, ConfigSource::Default);
        config->set("stdp_tau", stdpTau_, ConfigSource::Default);
        config->set("synaptogenesis_rate", synaptogenesisRate_, ConfigSource::Default);
        config->set("pruning_rate", pruningRate_, ConfigSource::Default);
    }
    
    void buildPhase6ConfigInternal(Phase6Config& config) {
        config.neuronCount = neuronCount_;
        config.regionCount = regionCount_;
        config.connectionProbability = connectionProbability_;
        config.maxSteps = phase6Config_.maxSteps;
        config.enableCheckpointing = phase6Config_.enableCheckpointing;
        config.enableReplay = phase6Config_.enableReplay;
        config.enableDevelopment = phase6Config_.enableDevelopment;
        config.checkpointPath = phase6Config_.checkpointPath;
    }
    
    // Configuration values
    uint64_t seed_ = 42;
    double timestep_ = 0.001;
    size_t neuronCount_ = 500;
    size_t regionCount_ = 1;
    float connectionProbability_ = 0.15f;
    float stdpLtpWeight_ = 0.02f;
    float stdpLtdWeight_ = 0.015f;
    float stdpTau_ = 20.0f;
    float synaptogenesisRate_ = 0.0001f;
    float pruningRate_ = 0.00001f;
    
    // Phase 6 config
    Phase6Config phase6Config_;
};

} // namespace nlm
