#include "Developer.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

Developer::Developer(std::shared_ptr<Config> config)
    : config_(config)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , developmentEnabled_(true)
    , structuralPlasticityEnabled_(true)
{
}

Developer::~Developer() = default;

void Developer::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void Developer::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
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

DevelopmentalStage Developer::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

void Developer::reset() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

void Developer::updateStructuralPlasticity(double timestep) {
    if (!brain_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

} // namespace nlm
