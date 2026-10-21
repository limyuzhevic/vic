#include "BrainFactory.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace nlm {

std::unordered_map<std::string, std::function<std::shared_ptr<Brain>()>> BrainFactory::customCreators_;
std::unordered_map<std::string, Config> BrainFactory::presetConfigs_;

std::shared_ptr<Brain> BrainFactory::createStandardBrain(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability,
    double timestep,
    uint64_t seed) {
    
    auto config = createStandardConfig(neuronCount, regionCount, connectionProbability, timestep, seed);
    return createBrainFromConfig(config);
}

std::shared_ptr<Brain> BrainFactory::createCustomBrain(const std::string& configFile) {
    Config config;
    if (!config.loadFromFile(configFile)) {
        throw std::runtime_error("Failed to load configuration from file: " + configFile);
    }
    
    if (!validateBrainConfiguration(config, std::string())) {
        throw std::runtime_error("Invalid configuration in file: " + configFile);
    }
    
    return createBrainFromConfig(config);
}

std::shared_ptr<Brain> BrainFactory::createDevelopmentBrain() {
    auto config = createDevelopmentConfig();
    return createBrainFromConfig(config);
}

std::shared_ptr<Brain> BrainFactory::createMinimalBrain() {
    auto config = createMinimalConfig();
    return createBrainFromConfig(config);
}

std::shared_ptr<Brain> BrainFactory::createBrainFromConfig(const Config& config) {
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain from configuration");
    }
    
    return brain;
}

std::shared_ptr<Brain> BrainFactory::createOptimizedBrain(const std::string& optimizationProfile) {
    auto config = createOptimizedConfig(optimizationProfile);
    return createBrainFromConfig(config);
}

bool BrainFactory::validateBrainConfiguration(const Config& config, std::string& errorMessage) {
    try {
        ConfigValidator validator;
        return validator.validate(config, errorMessage);
    } catch (const std::exception& e) {
        errorMessage = e.what();
        return false;
    }
}

void BrainFactory::registerCustomBrainCreator(const std::string& name, 
                                             std::function<std::shared_ptr<Brain>()> creator) {
    customCreators_[name] = creator;
}

bool BrainFactory::createCustomBrain(const std::string& name, std::shared_ptr<Brain>& result) {
    auto it = customCreators_.find(name);
    if (it == customCreators_.end()) {
        return false;
    }
    
    try {
        result = it->second();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

size_t BrainFactory::getRecommendedNeuronCount(const std::string& taskType) {
    static const std::unordered_map<std::string, size_t> recommendations = {
        {"simple", 100},
        {"medium", 1000},
        {"complex", 10000},
        {"research", 50000},
        {"default", 1000}
    };
    
    auto it = recommendations.find(taskType);
    return (it != recommendations.end()) ? it->second : recommendations.at("default");
}

size_t BrainFactory::getRecommendedRegionCount(const std::string& taskType) {
    static const std::unordered_map<std::string, size_t> recommendations = {
        {"simple", 1},
        {"medium", 2},
        {"complex", 4},
        {"research", 8},
        {"default", 2}
    };
    
    auto it = recommendations.find(taskType);
    return (it != recommendations.end()) ? it->second : recommendations.at("default");
}

float BrainFactory::getRecommendedConnectionProbability(const std::string& taskType) {
    static const std::unordered_map<std::string, float> recommendations = {
        {"simple", 0.05f},
        {"medium", 0.1f},
        {"complex", 0.15f},
        {"research", 0.2f},
        {"default", 0.1f}
    };
    
    auto it = recommendations.find(taskType);
    return (it != recommendations.end()) ? it->second : recommendations.at("default");
}

Config BrainFactory::createStandardConfig(size_t neuronCount, size_t regionCount,
                                        float connectionProbability, double timestep, uint64_t seed) {
    Config config;
    config.setNeuronCount(neuronCount);
    config.setRegionCount(regionCount);
    config.setConnectionProbability(connectionProbability);
    config.setSimulationTimestep(timestep);
    config.setRandomSeed(seed);
    
    // Standard STDP and plasticity parameters
    config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return config;
}

Config BrainFactory::createDevelopmentConfig() {
    Config config;
    config.setNeuronCount(500);
    config.setRegionCount(2);
    config.setConnectionProbability(0.15f);
    config.setSimulationTimestep(0.001);
    config.setRandomSeed(123);
    
    // Development-focused parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config.set("pruning_rate", 0.00002f, ConfigSource::Default);
    config.set("development_enabled", true, ConfigSource::Default);
    config.set("curiosity_enabled", true, ConfigSource::Default);
    config.set("novelty_threshold", 0.1f, ConfigSource::Default);
    
    return config;
}

Config BrainFactory::createMinimalConfig() {
    Config config;
    config.setNeuronCount(100);
    config.setRegionCount(1);
    config.setConnectionProbability(0.05f);
    config.setSimulationTimestep(0.001);
    config.setRandomSeed(42);
    
    return config;
}

Config BrainFactory::createOptimizedConfig(const std::string& profile) {
    std::string profileLower = profile;
    std::transform(profileLower.begin(), profileLower.end(), profileLower.begin(), ::tolower);
    
    if (profileLower == "fast") {
        Config config;
        config.setNeuronCount(500);
        config.setRegionCount(1);
        config.setConnectionProbability(0.05f);
        config.setSimulationTimestep(0.001);
        config.setRandomSeed(42);
        config.set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.006f, ConfigSource::Default);
        return config;
    } else if (profileLower == "accurate") {
        Config config;
        config.setNeuronCount(5000);
        config.setRegionCount(4);
        config.setConnectionProbability(0.2f);
        config.setSimulationTimestep(0.0005);
        config.setRandomSeed(12345);
        config.set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.014f, ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0003f, ConfigSource::Default);
        config.set("pruning_rate", 0.00003f, ConfigSource::Default);
        return config;
    } else {
        // Default optimized configuration
        return createStandardConfig(2000, 3, 0.12f, 0.001, 999);
    }
}

}} // namespace nlm