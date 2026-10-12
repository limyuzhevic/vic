#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Developer: Handles developmental stage updates
// Manages developmental stages, plasticity, and structural changes
class Developer {
public:
    Developer(std::shared_ptr<Config> config);
    ~Developer();
    
    // Configure with brain reference
    void initialize(std::shared_ptr<Brain> brain);
    
    // Update development system with current timestep
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Enable/disable development
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Reset development state
    void reset();
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool developmentEnabled_;
    bool structuralPlasticityEnabled_;
    
    // Update structural plasticity rates based on developmental stage
    void updateStructuralPlasticity(double timestep);
};

} // namespace nlm
