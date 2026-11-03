#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class BrainStepManager {
public:
    BrainStepManager(Brain* brain, std::shared_ptr<Config> config);
    ~BrainStepManager();
    
    void executeIntegrationStep(SimulationStep step, Timestamp time);
    
private:
    struct Impl;
    Impl* pImpl;
};

class BrainMemorySystem {
public:
    BrainMemorySystem(Brain* brain, std::shared_ptr<Config> config);
    ~BrainMemorySystem();
    
    void initialize();
    void update(SimulationStep step);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

class BrainNeuromodulation {
public:
    BrainNeuromodulation(Brain* brain);
    ~BrainNeuromodulation();
    
    void applyAllNeuromodulation(SimulationStep step, Timestamp time);
    
private:
    struct Impl;
    Impl* pImpl;
};

class BrainDevelopment {
public:
    BrainDevelopment(Brain* brain, std::shared_ptr<Config> config);
    ~BrainDevelopment();
    
    void updateDevelopmentEffects(SimulationStep step);
    void applyDevelopmentToPlasticity();
    
private:
    struct Impl;
    Impl* pImpl;
};

class BrainCheckpointManager {
public:
    BrainCheckpointManager(Brain* brain);
    ~BrainCheckpointManager();
    
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

class BrainInitializer {
public:
    BrainInitializer(Brain* brain, std::shared_ptr<Config> config);
    ~BrainInitializer();
    
    bool initialize();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
