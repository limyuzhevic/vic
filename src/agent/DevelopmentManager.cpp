#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

DevelopmentManager::DevelopmentManager(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , developmentEnabled_(true)
    , structuralPlasticityEnabled_(true)
{
}

void DevelopmentManager::update(double timestep, bool developmentEnabled, bool structuralPlasticityEnabled) {
    developmentEnabled_ = developmentEnabled;
    structuralPlasticityEnabled_ = structuralPlasticityEnabled;
    
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage DevelopmentManager::getStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

} // namespace nlm
