#pragma once

#include <string>
#include <memory>

namespace nlm {

class Brain;
class ConfigurationManager;

class BrainInitializer {
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;
    
    // Create and initialize brain with given configuration
    std::shared_ptr<Brain> createBrain(std::shared_ptr<ConfigurationManager> config);
    
    // Reset brain to initial state
    bool resetBrain(std::shared_ptr<Brain> brain);
    
    // Log brain status and statistics
    void logBrainStatus(std::shared_ptr<Brain> brain);
    
    // Validate brain state
    bool validateBrainState(std::shared_ptr<Brain> brain) const;
    
    // Get initialization statistics
    std::string getInitializationStats() const;
    
private:
    int initializationCount_ = 0;
    int totalBrainSize_ = 0;
    
    // Brain setup helpers
    void setupNetworkTopology(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void setupSynapticConnections(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void enablePlasticity(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
};

} // namespace nlm