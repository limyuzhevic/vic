#pragma once

#include <memory>
#include <string>

namespace nlm {

class Config;
class Brain;
class Neuromodulator;

class BrainFactory {
public:
    BrainFactory();
    ~BrainFactory();
    
    // Disable copying and moving
    BrainFactory(const BrainFactory&) = delete;
    BrainFactory& operator=(const BrainFactory&) = delete;
    BrainFactory(BrainFactory&&) = delete;
    BrainFactory& operator=(BrainFactory&&) = delete;
    
    // Create a brain with configuration
    static std::unique_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    // Validate configuration parameters
    static void validateConfig(std::shared_ptr<Config> config);
    
    // Initialize brain with configuration parameters
    static bool initializeBrain(std::unique_ptr<Brain>& brain, std::shared_ptr<Config> config);
    
private:
    static bool validateNeuromodulationParameters(std::shared_ptr<Config> config, std::unique_ptr<Brain>& brain);
};

} // namespace nlm
