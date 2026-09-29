#ifndef NLM_BRAIN_CONFIG_HPP
#define NLM_BRAIN_CONFIG_HPP

#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

/**
 * Configuration manager for NLM brain systems
 * Provides centralized configuration with validation and defaults
 */
class BrainConfig {
public:
    struct RegionConfig {
        std::string name;
        size_t neuronCount;
        float connectionProbability;
        float weightRange;
    };
    
    struct MemoryConfig {
        size_t workingMemoryCapacity;
        size_t maxEpisodicEpisodes;
        float consolidationThreshold;
        size_t replayInterval;
        size_t consolidationInterval;
    };
    
    struct PlasticityConfig {
        float stdpLTPWeight;
        float stdpLTDWeight;
        float stdpTimeConstant;
        float synaptogenesisRate;
        float pruningRate;
    };
    
    struct DevelopmentConfig {
        float plasticityModulation;
        float excitabilityChangeRate;
        float structuralPlasticityRate;
    };
    
    struct NeuromodulationConfig {
        float curiosityThreshold;
        float noveltyDecayRate;
        float dopamineInfluence;
    };

    BrainConfig();
    explicit BrainConfig(std::shared_ptr<Config> config);
    
    // Validation
    bool validate() const;
    std::string getValidationError() const { return validationError_; }
    
    // Region configuration
    void addRegionConfig(const std::string& name, size_t neuronCount,
                        float connectionProbability = 0.1f, float weightRange = 0.2f);
    const std::vector<RegionConfig>& getRegionConfigs() const { return regionConfigs_; }
    
    // Memory system configuration
    void setMemoryCapacity(size_t capacity) { memoryConfig_.workingMemoryCapacity = capacity; }
    void setMaxEpisodicEpisodes(size_t max) { memoryConfig_.maxEpisodicEpisodes = max; }
    const MemoryConfig& getMemoryConfig() const { return memoryConfig_; }
    
    // Plasticity configuration
    const PlasticityConfig& getPlasticityConfig() const { return plasticityConfig_; }
    void setPlasticityConfig(const PlasticityConfig& config) { plasticityConfig_ = config; }
    
    // Development configuration
    const DevelopmentConfig& getDevelopmentConfig() const { return developmentConfig_; }
    void setDevelopmentConfig(const DevelopmentConfig& config) { developmentConfig_ = config; }
    
    // Neuromodulation configuration
    const NeuromodulationConfig& getNeuromodulationConfig() const { return neuromodulationConfig_; }
    void setNeuromodulationConfig(const NeuromodulationConfig& config) { neuromodulationConfig_ = config; }
    
    // Global configuration parameters
    float getSimulationTimestep() const { return timestep_; }
    void setSimulationTimestep(float timestep) { timestep_ = timestep; }
    
    size_t getReplayInterval() const { return memoryConfig_.replayInterval; }
    void setReplayInterval(size_t interval) { memoryConfig_.replayInterval = interval; }
    
    size_t getConsolidationInterval() const { return memoryConfig_.consolidationInterval; }
    void setConsolidationInterval(size_t interval) { memoryConfig_.consolidationInterval = interval; }
    
    std::string getCheckpointDir() const { return checkpointDir_; }
    void setCheckpointDir(const std::string& dir) { checkpointDir_ = dir; }
    
    // Random seed for deterministic behavior
    uint64_t getRandomSeed() const { return randomSeed_; }
    void setRandomSeed(uint64_t seed) { randomSeed_ = seed; }
    
    // Apply configuration to brain
    void applyToBrain(Brain& brain) const;
    
private:
    std::vector<RegionConfig> regionConfigs_;
    MemoryConfig memoryConfig_;
    PlasticityConfig plasticityConfig_;
    DevelopmentConfig developmentConfig_;
    NeuromodulationConfig neuromodulationConfig_;
    
    float timestep_;
    std::string checkpointDir_;
    uint64_t randomSeed_;
    
    std::string validationError_;
    
    void initializeDefaults();
    bool validateRegionConfigs() const;
    bool validateMemoryConfig() const;
    bool validatePlasticityConfig() const;
    bool validateDevelopmentConfig() const;
    bool validateNeuromodulationConfig() const;
};

} // namespace nlm

#endif // NLM_BRAIN_CONFIG_HPP
