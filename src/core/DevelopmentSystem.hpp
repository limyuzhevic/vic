// DevelopmentSystem - Handles developmental stages and age-related plasticity
// Extracted from Brain::step() method
#pragma once

#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem(std::shared_ptr<Brain> brain);
    ~DevelopmentSystem();
    
    // Initialize the development system
    bool initialize();
    
    // Process one timestep of development effects
    void step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Get development system pointer
    ::DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem_.get(); }
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage_; }
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage_ = stage; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<::DevelopmentSystem> developmentSystem_;
    DevelopmentalStage developmentalStage_;
    
    // State tracking
    double developmentalAge_;
    
    // Helper methods
    void updateDevelopmentalStage(SimulationStep currentStep);
    void applyDevelopmentEffectsToPlasticity();
};

} // namespace nlm
