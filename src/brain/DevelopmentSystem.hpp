#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// DevelopmentSystem: Developmental processes
// Manages maturation, critical periods, and plasticity changes
class DevelopmentSystem {
public:
    DevelopmentSystem(std::shared_ptr<Config> config);
    ~DevelopmentSystem();
    
    // Initialize development system
    bool initialize(Brain* brain);
    
    // Development update
    void update(Brain* brain, RandomGenerator& rng, double timestep);
    
    // Developmental stage management
    DevelopmentalStage getDevelopmentalStage() const { return currentStage_; }
    void setDevelopmentalStage(DevelopmentalStage stage) { currentStage_ = stage; }
    
    // Developmental age and plasticity
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Development state
    double developmentalAge_;
    DevelopmentalStage currentStage_;
    float plasticityModifier_;
};

} // namespace nlm
