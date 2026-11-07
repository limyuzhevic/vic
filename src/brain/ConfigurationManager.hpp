#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager(Brain* brain);
    ~ConfigurationManager();
    
    bool initialize();
    void step(double timestep);
    void reset();
    
    // Configuration operations
    float getSimulationTimestep() const;
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    float getConnectionProbability() const;
    
    // Random generator access
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    
    // Logger access
    Logger* getLogger() { return logger.get(); }
    
private:
    Brain* brain_;
    std::unique_ptr<RandomGenerator> rng;
    std::unique_ptr<Logger> logger;
};

} // namespace nlm
