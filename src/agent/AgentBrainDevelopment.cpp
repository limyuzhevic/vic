#include "AgentBrainDevelopment.hpp"
#include "AgentBrain.hpp"

namespace nlm {
namespace development {

void AgentBrainDevelopment::updateDevelopment(AgentBrain& agentBrain, double timestep) {
    if (!agentBrain.brain_ || !agentBrain.developmentEnabled_) return;
    
    agentBrain.developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (agentBrain.developmentalAge_ < 60.0) {  // ~1 minute
        agentBrain.plasticityModifier_ = 1.0f;  // High plasticity
        agentBrain.brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (agentBrain.developmentalAge_ < 300.0) {  // ~5 minutes
        agentBrain.plasticityModifier_ = 0.8f;
        agentBrain.brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (agentBrain.developmentalAge_ < 900.0) {  // ~15 minutes
        agentBrain.plasticityModifier_ = 0.5f;
        agentBrain.brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        agentBrain.plasticityModifier_ = 0.2f;  // Adult - more stable
        agentBrain.brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (agentBrain.structuralPlasticityEnabled_) {
        auto* sp = agentBrain.brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * agentBrain.plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - agentBrain.plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

} // namespace development
} // namespace nlm
