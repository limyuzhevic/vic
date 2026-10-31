#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

// Core brain functionality: constructor, destructor, and basic management
class BrainCore {
public:
    explicit BrainCore(std::shared_ptr<Config> config);
    ~BrainCore();
    
    // Disable copying
    BrainCore(const BrainCore&) = delete;
    BrainCore& operator=(const BrainCore&) = delete;
    
    // Enable moving
    BrainCore(BrainCore&&) noexcept;
    BrainCore& operator=(BrainCore&&) noexcept;
    
    // Basic accessors
    const std::shared_ptr<Config>& getConfig() const { return pImpl->config; }
    void setConfig(std::shared_ptr<Config> config);
    
    // Simulation control
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Configuration access
    double getTimestep() const { return pImpl->timestep; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm