#include "BrainBuilder.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace nlm {

struct BrainBuilder::Impl {
    // Brain configuration options
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // System enable flags
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructuralPlasticity = true;
    bool enableMemorySystem = true;
    bool enablePredictionSystem = true;
    bool enableCognition = true;
    bool enableNeuromodulation = true;
    bool enableDevelopment = true;
    bool enableDevelopmentStages = true;
    
    // System parameters
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTimeConstant = 20.0f;
    
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    bool enableCriticalPeriod = true;
    float maturationRate = 0.5f;
    
    // Region information
    struct RegionInfo {
        std::string name;
        size_t neuronCount;
    };
    
    std::vector<RegionInfo> regions;
    
    // Configuration mode
    enum class ConfigurationMode {
        MINIMAL,
        STANDARD,
        LEARNING,
        COGNITION,
        DEVELOPMENT,
        PERFORMANCE,
        RESEARCH
    };
    
    ConfigurationMode configMode = ConfigurationMode::STANDARD;
    
    Impl() = default;
    
    // Create configuration based on current settings
    std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        
        // Set basic parameters
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProbability);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        
        // STDP parameters
        config->set("stdp_ltp_weight", stdpLTPWeight);
        config->set("stdp_ltd_weight", stdpLTDWeight);
        config->set("stdp_tau", stdpTimeConstant);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", synaptogenesisRate);
        config->set("pruning_rate", pruningRate);
        
        // Development parameters
        config->set("development_enabled", enableDevelopmentStages);
        config->set("critical_period_enabled", enableCriticalPeriod);
        config->set("maturation_rate", maturationRate);
        
        // System enable flags
        config->set("enable_stdp", enableSTDP);
        config->set("enable_hebbian", enableHebbian);
        config->set("enable_structural_plasticity", enableStructuralPlasticity);
        config->set("enable_memory", enableMemorySystem);
        config->set("enable_prediction", enablePredictionSystem);
        config->set("enable_cognition", enableCognition);
        config->set("enable_neuromodulation", enableNeuromodulation);
        config->set("enable_development", enableDevelopment);
        
        return config;
    }
};

BrainBuilder::BrainBuilder() : pImpl(std::make_unique<Impl>()) {}

BrainBuilder::~BrainBuilder() = default;

BrainBuilder::BrainBuilder(BrainBuilder&&) noexcept = default;

BrainBuilder& BrainBuilder::operator=(BrainBuilder&&) noexcept = default;

BrainBuilder& BrainBuilder::addRegion(const std::string& name, size_t neuronCount) {
    pImpl->regions.push_back({name, neuronCount});
    return *this;
}

BrainBuilder& BrainBuilder::setNeuronCount(size_t neuronCount) {
    pImpl->neuronCount = neuronCount;
    return *this;
}

BrainBuilder& BrainBuilder::setRegionCount(size_t regionCount) {
    pImpl->regionCount = regionCount;
    return *this;
}

BrainBuilder& BrainBuilder::setConnectionProbability(float probability) {
    pImpl->connectionProbability = std::max(0.0f, std::min(1.0f, probability));
    return *this;
}

BrainBuilder& BrainBuilder::enableSTDP(bool enable) {
    pImpl->enableSTDP = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableHebbian(bool enable) {
    pImpl->enableHebbian = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableStructuralPlasticity(bool enable) {
    pImpl->enableStructuralPlasticity = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableMemorySystem(bool enable) {
    pImpl->enableMemorySystem = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enablePredictionSystem(bool enable) {
    pImpl->enablePredictionSystem = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableCognition(bool enable) {
    pImpl->enableCognition = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableNeuromodulation(bool enable) {
    pImpl->enableNeuromodulation = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableDevelopment(bool enable) {
    pImpl->enableDevelopment = enable;
    return *this;
}

BrainBuilder& BrainBuilder::enableDevelopmentStages(bool enable) {
    pImpl->enableDevelopmentStages = enable;
    return *this;
}

BrainBuilder& BrainBuilder::setSTDPParameters(float ltpWeight, float ltdWeight, float tau) {
    pImpl->stdpLTPWeight = std::max(0.0f, std::min(1.0f, ltpWeight));
    pImpl->stdpLTDWeight = std::max(0.0f, std::min(1.0f, ltdWeight));
    pImpl->stdpTimeConstant = std::max(1.0f, std::min(100.0f, tau));
    return *this;
}

BrainBuilder& BrainBuilder::setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate) {
    pImpl->synaptogenesisRate = std::max(0.0f, synaptogenesisRate);
    pImpl->pruningRate = std::max(0.0f, pruningRate);
    return *this;
}

BrainBuilder& BrainBuilder::setDevelopmentParameters(bool enableCriticalPeriod, float maturationRate) {
    pImpl->enableCriticalPeriod = enableCriticalPeriod;
    pImpl->maturationRate = std::max(0.0f, std::min(1.0f, maturationRate));
    return *this;
}

BrainBuilder& BrainBuilder::configureForLearning() {
    // Configure for rapid adaptation and learning
    pImpl->configMode = Impl::ConfigurationMode::LEARNING;
    pImpl->enableSTDP = true;
    pImpl->enableHebbian = true;
    pImpl->enableStructuralPlasticity = true;
    pImpl->enableMemorySystem = true;
    pImpl->enablePredictionSystem = true;
    pImpl->enableCognition = true;
    pImpl->enableNeuromodulation = true;
    pImpl->enableDevelopment = true;
    pImpl->enableDevelopmentStages = true;
    
    // Optimize for learning
    pImpl->stdpLTPWeight = 0.03f;
    pImpl->stdpLTDWeight = 0.025f;
    pImpl->stdpTimeConstant = 15.0f;
    pImpl->synaptogenesisRate = 0.0002f;
    pImpl->pruningRate = 0.00002f;
    
    return *this;
}

BrainBuilder& BrainBuilder::configureForCognition() {
    // Configure for cognitive tasks and planning
    pImpl->configMode = Impl::ConfigurationMode::COGNITION;
    pImpl->enableSTDP = true;
    pImpl->enableHebbian = true;
    pImpl->enableStructuralPlasticity = true;
    pImpl->enableMemorySystem = true;
    pImpl->enablePredictionSystem = true;
    pImpl->enableCognition = true;
    pImpl->enableNeuromodulation = true;
    pImpl->enableDevelopment = true;
    pImpl->enableDevelopmentStages = true;
    
    // Optimize for cognition
    pImpl->stdpLTPWeight = 0.015f;
    pImpl->stdpLTDWeight = 0.012f;
    pImpl->stdpTimeConstant = 25.0f;
    pImpl->synaptogenesisRate = 0.00005f;
    pImpl->pruningRate = 0.000005f;
    
    return *this;
}

BrainBuilder& BrainBuilder::configureForPerformance() {
    // Configure for speed and efficiency
    pImpl->configMode = Impl::ConfigurationMode::PERFORMANCE;
    pImpl->enableSTDP = false;  // Disable plasticity for speed
    pImpl->enableHebbian = false;
    pImpl->enableStructuralPlasticity = false;
    pImpl->enableMemorySystem = false;
    pImpl->enablePredictionSystem = false;
    pImpl->enableCognition = false;
    pImpl->enableNeuromodulation = false;
    pImpl->enableDevelopment = false;
    pImpl->enableDevelopmentStages = false;
    
    // Minimal parameters for speed
    pImpl->neuronCount = 100;
    pImpl->regionCount = 1;
    
    return *this;
}

BrainBuilder& BrainBuilder::configureForDevelopment() {
    // Configure for development studies
    pImpl->configMode = Impl::ConfigurationMode::DEVELOPMENT;
    pImpl->enableSTDP = true;
    pImpl->enableHebbian = true;
    pImpl->enableStructuralPlasticity = true;
    pImpl->enableMemorySystem = true;
    pImpl->enablePredictionSystem = true;
    pImpl->enableCognition = true;
    pImpl->enableNeuromodulation = true;
    pImpl->enableDevelopment = true;
    pImpl->enableDevelopmentStages = true;
    
    // Optimize for development
    pImpl->stdpLTPWeight = 0.01f;
    pImpl->stdpLTDWeight = 0.008f;
    pImpl->stdpTimeConstant = 30.0f;
    pImpl->synaptogenesisRate = 0.00005f;
    pImpl->pruningRate = 0.00001f;
    pImpl->enableCriticalPeriod = true;
    pImpl->maturationRate = 0.3f;
    
    return *this;
}

BrainBuilder& BrainBuilder::configureForStandardResearch() {
    // Configure for standard research use
    pImpl->configMode = Impl::ConfigurationMode::STANDARD;
    // Keep all settings as defaults for balance
    return *this;
}

BrainBuilder& BrainBuilder::configureForMinimal() {
    // Configure for minimal configuration (testing)
    pImpl->configMode = Impl::ConfigurationMode::MINIMAL;
    pImpl->enableSTDP = false;
    pImpl->enableHebbian = false;
    pImpl->enableStructuralPlasticity = false;
    pImpl->enableMemorySystem = false;
    pImpl->enablePredictionSystem = false;
    pImpl->enableCognition = false;
    pImpl->enableNeuromodulation = false;
    pImpl->enableDevelopment = false;
    pImpl->enableDevelopmentStages = false;
    pImpl->neuronCount = 50;
    pImpl->regionCount = 1;
    
    return *this;
}

std::shared_ptr<Brain> BrainBuilder::buildMinimal() {
    return configureForMinimal().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildStandard() {
    return configureForStandardResearch().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildLearning() {
    return configureForLearning().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildCognition() {
    return configureForCognition().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildDevelopment() {
    return configureForDevelopment().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildPerformance() {
    return configureForPerformance().buildResearch();
}

std::shared_ptr<Brain> BrainBuilder::buildResearch() {
    // Create configuration based on current settings
    auto config = pImpl->createConfig();
    
    // Create brain with the configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return nullptr;
    }
    
    return brain;
}

namespace BrainFactory {
    std::shared_ptr<Brain> createMinimal() {
        return BrainBuilder().buildMinimal();
    }
    
    std::shared_ptr<Brain> createStandard() {
        return BrainBuilder().buildStandard();
    }
    
    std::shared_ptr<Brain> createLearning() {
        return BrainBuilder().buildLearning();
    }
    
    std::shared_ptr<Brain> createCognition() {
        return BrainBuilder().buildCognition();
    }
    
    std::shared_ptr<Brain> createDevelopment() {
        return BrainBuilder().buildDevelopment();
    }
    
    std::shared_ptr<Brain> createPerformance() {
        return BrainBuilder().buildPerformance();
    }
    
    std::shared_ptr<Brain> createResearch() {
        return BrainBuilder().buildResearch();
    }
}

} // namespace nlm