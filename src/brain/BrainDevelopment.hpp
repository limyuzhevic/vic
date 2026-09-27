// BrainDevelopment.hpp - Development system integration interface
#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "../brain/Types/DevelopmentalStage.hpp"
#include "../brain/Types/SimulationStep.hpp"
#include "../brain/Types/Timestamp.hpp"

namespace nlm {

class Brain;

class BrainDevelopment {
public:
    BrainDevelopment();
    ~BrainDevelopment();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Get development statistics
    double getDevelopmentalAge() const;
    float getPlasticityModifier() const;
    
    // Access to underlying development system
    DevelopmentSystem* getDevelopmentSystem();
    
private:
    Brain* brain_;
    
    // Reference to development system
    DevelopmentSystem* developmentSystem_;
};

} // namespace nlm