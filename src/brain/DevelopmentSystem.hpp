#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

// ==========================================
// DEVELOPMENT SYSTEM - Development
// ==========================================
// 
// Manages: developmental stages, critical periods
// 
// Contains: Synaptogenesis, Maturation, Pruning
// Handles: developmental plasticity modulation
// Manages: maturation and pruning processes

class DevelopmentSystem {
public:
    DevelopmentSystem(std::shared_ptr<Config> config);
    ~DevelopmentSystem();
    
    // Disable copying, enable moving
    DevelopmentSystem(const DevelopmentSystem&) = delete;
    DevelopmentSystem& operator=(const DevelopmentSystem&) = delete;
    DevelopmentSystem(DevelopmentSystem&&) noexcept;
    DevelopmentSystem& operator=(DevelopmentSystem&&) noexcept;
    
    // Initialize development system with brain reference
    bool initialize(Brain* brain);
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update development system
    void update(SimulationStep step, Timestamp time, TimestepDuration dt);
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Get development progress (0.0 to 1.0)
    float getDevelopmentProgress() const;
    
    // Is critical period active for specific plasticity type?
    bool isCriticalPeriod(PlasticityFlags plasticityFlags) const;
    
    // Get plasticity modulation factor for developmental stage
    float getPlasticityModulationFactor(PlasticityFlags plasticityFlags) const;
    
    // Reset development system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    
    DevelopmentalStage developmentalStage_;
    SimulationStep startStep_;
};

} // namespace nlm