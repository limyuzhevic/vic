#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class DevelopmentSystemWrapper {
public:
    DevelopmentSystemWrapper();
    ~DevelopmentSystemWrapper();
    
    // Disable copying, enable moving
    DevelopmentSystemWrapper(const DevelopmentSystemWrapper&) = delete;
    DevelopmentSystemWrapper& operator=(const DevelopmentSystemWrapper&) = delete;
    DevelopmentSystemWrapper(DevelopmentSystemWrapper&&) noexcept;
    DevelopmentSystemWrapper& operator=(DevelopmentSystemWrapper&&) noexcept;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    // Development updates
    void updateDevelopment(SimulationStep currentStep, float timestep, const class RandomGenerator& rng);
    
    // Development stages
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Development effects
    void applyDevelopmentalEffects(Brain* brain);
    void updatePlasticityBasedOnDevelopment(Brain* brain);
    void updateNeuralExcitability(Brain* brain);
    void updateSynapticConnectivity(Brain* brain);
    
    // Configuration
    void setDevelopmentSpeed(float speed);
    void setMaturationRate(float rate);
    void setCriticalPeriodDuration(size_t duration);
    void setDevelopmentalStageDuration(DevelopmentalStage stage, size_t duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm