#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "AgentBrainConfig.hpp"

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem(std::shared_ptr<Brain> brain,
                     double& developmentalAge,
                     float& plasticityModifier,
                     bool structuralPlasticityEnabled)
        : brain_(brain)
        , developmentalAge_(developmentalAge)
        , plasticityModifier_(plasticityModifier)
        , structuralPlasticityEnabled_(structuralPlasticityEnabled) {
        validateSetup();
    }
    
    // Update development system based on simulation timestep
    void updateDevelopment(double timestep, bool enabled) {
        if (!enabled || !brain_) {
            resetDevelopmentState();
            return;
        }
        
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        // This is a biologically inspired approximation
        if (developmentalAge_ < AgentBrainConfig::CRITICAL_PERIOD_THRESHOLD) {  // ~1 minute
            plasticityModifier_ = AgentBrainConfig::HIGH_PLASTICITY;  // High plasticity
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < AgentBrainConfig::MATURATION_THRESHOLD) {  // ~5 minutes
            plasticityModifier_ = AgentBrainConfig::MODERATE_PLASTICITY;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < AgentBrainConfig::ADULT_THRESHOLD) {  // ~15 minutes
            plasticityModifier_ = AgentBrainConfig::LOW_PLASTICITY;
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier_ = AgentBrainConfig::ADULT_PLASTICITY;  // Adult - more stable
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development
                float synRate = AgentBrainConfig::SYNAPTOGENESIS_RATE_BASE * plasticityModifier_;
                float pruneRate = AgentBrainConfig::PRUNING_RATE_BASE * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
    
    void resetDevelopmentState() {
        developmentalAge_ = AgentBrainConfig::INITIAL_DEVELOPMENT_TIME;
        plasticityModifier_ = AgentBrainConfig::HIGH_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    }
    
    // Reset method for testing and state clearing
    void reset() {
        resetDevelopmentState();
    }
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    // Get current developmental age
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    // Get current plasticity modifier
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    bool validateSetup() {
        if (!brain_) {
            throw AgentBrainException::nullPointerError("Brain reference");
        }
        
        return true;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    double& developmentalAge_;
    float& plasticityModifier_;
    bool structuralPlasticityEnabled_;
};

} // namespace nlm
