#pragma once

#include "AgentBrainCore.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class AgentBrainDevelopment {
public:
    AgentBrainDevelopment(AgentBrainCore* parent) : parent_(parent) {}
    
    // Update development based on age
    void updateDevelopment(double timestep);
    
    // Check if development is enabled
    bool isDevelopmentEnabled() const { return parent_->developmentEnabled_; }
    
    // Enable/disable development
    void enableDevelopment(bool enable) { parent_->developmentEnabled_ = enable; }
    
    // Get developmental age
    double getDevelopmentalAge() const { return parent_->developmentalAge_; }
    
    // Get plasticity modifier
    float getPlasticityModifier() const { return parent_->plasticityModifier_; }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Check if structural plasticity is enabled
    bool isStructuralPlasticityEnabled() const { return parent_->structuralPlasticityEnabled_; }
    
    // Enable/disable structural plasticity
    void enableStructuralPlasticity(bool enable) { parent_->structuralPlasticityEnabled_ = enable; }
    
    // Reset development state
    void reset();
    
private:
    AgentBrainCore* parent_;
};

inline void AgentBrainDevelopment::updateDevelopment(double timestep) {
    if (!parent_->brain_ || !parent_->developmentEnabled_) return;
    
    parent_->developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (parent_->developmentalAge_ < 60.0) {  // ~1 minute
        parent_->plasticityModifier_ = 1.0f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (parent_->developmentalAge_ < 300.0) {  // ~5 minutes
        parent_->plasticityModifier_ = 0.8f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (parent_->developmentalAge_ < 900.0) {  // ~15 minutes
        parent_->plasticityModifier_ = 0.5f;
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        parent_->plasticityModifier_ = 0.2f;  // Adult - more stable
        parent_->brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

inline DevelopmentalStage AgentBrainDevelopment::getDevelopmentalStage() const {
    if (!parent_->brain_) return DevelopmentalStage::Initial;
    return parent_->brain_->getDevelopmentalStage();
}

inline void AgentBrainDevelopment::reset() {
    parent_->developmentalAge_ = 0.0;
    parent_->plasticityModifier_ = 1.0f;
}

} // namespace nlm