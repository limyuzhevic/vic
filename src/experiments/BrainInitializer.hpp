#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

/**
 * Brain initialization and setup
 * Handles brain creation, initialization, and basic configuration
 */
class BrainInitializer {
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;
    
    /**
     * Create and initialize a brain with given configuration
     */
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    /**
     * Create and initialize a brain with default settings
     */
    std::shared_ptr<Brain> createBrain(size_t neuronCount = 1000);
    
    /**
     * Validate that brain was initialized correctly
     */
    bool validateBrain(std::shared_ptr<Brain> brain) const;
    
    /**
     * Get brain initialization status
     */
    bool isBrainInitialized(std::shared_ptr<Brain> brain) const;
    
    /**
     * Create config for minimal brain testing
     */
    std::shared_ptr<Config> createMinimalBrainConfig(size_t neuronCount = 100);
    
    /**
     * Create config for full simulation brain
     */
    std::shared_ptr<Config> createSimulationBrainConfig(const Phase6Config& phaseConfig);
};

} // namespace nlm
