#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>

namespace nlm {

class PlasticityManager {
public:
    PlasticityManager(std::shared_ptr<Config> config);
    ~PlasticityManager();
    
    // Initialize plasticity systems
    bool initialize(Brain* brain);
    
    // Update plasticity rules
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset plasticity systems
    void reset();
    
    // Get component access
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Plasticity statistics
    float getPlasticityRate() const;
    size_t getActiveConnections() const;
    
    // Configuration methods
    void setSTDPParameters(float ltp, float ltd, float tau);
    void setHebbianLearningRate(float rate) { hebbianLearningRate = rate; }
    void setStructuralPlasticityRate(float rate) { structuralPlasticityRate = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float hebbianLearningRate;
    float structuralPlasticityRate;
};

} // namespace nlm