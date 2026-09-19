#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainDevelopment {
public:
    // Constructor
    BrainDevelopment(std::shared_ptr<Config> config);
    ~BrainDevelopment();
    
    // Initialize development system
    void initialize(Brain* brain);
    
    // Update development system
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get development system accessor
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    const DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem.get(); }
    
    // Get developmental stage
    DevelopmentalStage getCurrentStage() const { return developmentalStage; }
    void setStage(DevelopmentalStage stage) { developmentalStage = stage; }
    
    // Update development effects on brain systems
    void applyDevelopmentEffects(Brain* brain);
    
    // Check if development should be triggered
    bool shouldUpdateDevelopment(SimulationStep currentStep) const;
    
    // Reset development state
    void reset();
    
    // Log development system status
    void logStatus() const;
    
    // Backward compatibility getters
    DevelopmentSystem* getDevelopmentSystemPtr() { return developmentSystem.get(); }
    const DevelopmentSystem* getDevelopmentSystemPtr() const { return developmentSystem.get(); }
    
private:
    // Development system component
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Developmental stage
    DevelopmentalStage developmentalStage;
    
    // Development timing
    size_t stepsSinceLastDevelopmentUpdate;
    
    // Helper methods
    void initializeFromConfig();
    void applySynaptogenesisEffects(Brain* brain);
    void applyPruningEffects(Brain* brain);
    void applyMaturationEffects(Brain* brain);
    float calculateDevelopmentProgress(SimulationStep currentStep) const;
};

} // namespace nlm