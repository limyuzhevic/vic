#pragma once

#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {

class BrainDevelopment {
public:
    BrainDevelopment();
    ~BrainDevelopment();
    
    // Initialize with core brain
    bool initialize(BrainCore* core);
    
    // Update development system
    void update(BrainCore* core, Timestamp elapsedTime);
    
    // Apply development effects
    void applyDevelopmentEffects(BrainCore* core);
    
    // Get development system
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Development stage management
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage = stage; }
    
    // Configuration
    void setDevelopmentRate(float rate) { developmentRate = rate; }
    void setPlasticityModulation(float mod) { plasticityModulation = mod; }
    
private:
    // Core brain reference
    BrainCore* core = nullptr;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Development state
    DevelopmentalStage developmentalStage;
    float developmentRate;
    float plasticityModulation;
    
    // Helper methods
    void updateSynaptogenesis(BrainCore* core, Timestamp elapsedTime);
    void updatePruning(BrainCore* core, Timestamp elapsedTime);
    void updateMaturation(BrainCore* core, Timestamp elapsedTime);
    void applyPlasticityModulation(BrainCore* core);
    
    // Friend class
    friend class Brain;
};

} // namespace nlm
