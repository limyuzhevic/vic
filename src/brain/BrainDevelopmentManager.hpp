#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

class BrainDevelopmentManager {
public:
    // Create development manager with configuration
    explicit BrainDevelopmentManager(std::shared_ptr<Config> config);
    
    ~BrainDevelopmentManager();
    
    // Initialize development manager
    bool initialize();
    
    // Update development system
    void update(BrainNeuralCore* neuralCore,
                std::unique_ptr<RandomGenerator>& rng,
                TimestepDuration timestep);
    
    // Developmental stage management
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    const char* getStageName() const;
    void advanceStage();
    
    // Development effects
    void applyEffectsToPlasticity(BrainPlasticityManager* plasticityManager);
    void applyEffectsToNeuralCore(BrainNeuralCore* neuralCore);
    
    // Get development parameters
    double getDevelopmentalAge() const;
    float getPlasticityModifier() const;
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Configuration
    void setDevelopmentRate(float rate);
    void setCriticalPeriodStart(float step);
    void setCriticalPeriodEnd(float step);
    
    // Reset development state
    void reset();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Log development manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm