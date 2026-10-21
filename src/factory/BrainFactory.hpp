#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class BrainFactory {
public:
    static std::shared_ptr<Brain> createStandardBrain(
        size_t neuronCount = 1000,
        size_t regionCount = 1,
        float connectionProbability = 0.1f,
        double timestep = 0.001,
        uint64_t seed = 42);
    
    static std::shared_ptr<Brain> createCustomBrain(const std::string& configFile);
    static std::shared_ptr<Brain> createDevelopmentBrain();
    static std::shared_ptr<Brain> createMinimalBrain();
    
    static std::shared_ptr<Brain> createBrainFromConfig(const Config& config);
    static std::shared_ptr<Brain> createOptimizedBrain(const std::string& optimizationProfile);
    
    static bool validateBrainConfiguration(const Config& config, std::string& errorMessage);
    
    static void registerCustomBrainCreator(const std::string& name, 
                                           std::function<std::shared_ptr<Brain>()> creator);
    static bool createCustomBrain(const std::string& name, std::shared_ptr<Brain>& result);
    
    static size_t getRecommendedNeuronCount(const std::string& taskType);
    static size_t getRecommendedRegionCount(const std::string& taskType);
    static float getRecommendedConnectionProbability(const std::string& taskType);
    
private:
    static std::unordered_map<std::string, std::function<std::shared_ptr<Brain>()>> customCreators_;
    static std::unordered_map<std::string, Config> presetConfigs_;
    
    static Config createStandardConfig(size_t neuronCount, size_t regionCount, 
                                       float connectionProbability, double timestep, uint64_t seed);
    static Config createDevelopmentConfig();
    static Config createMinimalConfig();
    static Config createOptimizedConfig(const std::string& profile);
};

}} // namespace nlm