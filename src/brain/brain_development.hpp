#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class DevelopmentSystem;

// Development system integration
class BrainDevelopment {
public:
    BrainDevelopment() = default;
    ~BrainDevelopment() = default;
    
    // Initialize development system
    void initialize(std::shared_ptr<class DevelopmentSystem> developmentSystem);
    
    // Development updates (called from main step)
    void update(SimulationStep currentStep,
               std::shared_ptr<Brain> brain,
               std::shared_ptr<DevelopmentSystem> developmentSystem,
               std::shared_ptr<class StructuralPlasticity> structuralPlasticity,
               std::shared_ptr<class RandomGenerator> rng);
    
    // Get development system accessor
    DevelopmentSystem* getDevelopmentSystem() const;
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
