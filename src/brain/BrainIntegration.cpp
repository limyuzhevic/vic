#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainIntegration::applyNeuromodulation(Brain* parent, const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (auto* stdp = parent->getSTDP()) {
        stdp->setLTPWeight(0.01f * modulation);
        stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainIntegration::updatePlasticity(Brain* parent) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainIntegration::develop(Brain* parent) {
    // Development updates structural plasticity
    if (auto* sp = parent->getStructuralPlasticity()) {
        sp->update(parent, *parent->getRandomGenerator());
    }
}

void BrainIntegration::applyNeuromodulationToPlasticity(Brain* parent, const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (auto* stdp = parent->getSTDP()) {
        float ltpWeight = stdp->getLTPWeight();
        float ltdWeight = stdp->getLTDWeight();
        stdp->setLTPWeight(ltpWeight * modulation);
        stdp->setLTDWeight(ltdWeight * modulation);
    }
}

void BrainIntegration::updatePlasticityRules(Brain* parent) {
    // Plasticity rules are applied during simulation steps
    // This is handled in BrainSimulation
}

void BrainIntegration::applyDevelopmentEffects(Brain* parent) {
    // Development affects plasticity rates
    auto* sp = parent->getStructuralPlasticity();
    if (!sp) return;
    
    DevelopmentalStage stage = parent->getDevelopmentalStage();
    float plasticityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            break;
    }
    
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

void BrainIntegration::updateDevelopmentState(Brain* parent) {
    // Update developmental stage based on time
    auto* core = parent->getCore();
    if (!core) return;
    
    // Simple developmental stage progression based on steps
    if (core->currentStep >= 10000) {
        if (core->developmentalStage == DevelopmentalStage::Initial) {
            parent->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
    } else if (core->currentStep >= 20000) {
        if (core->developmentalStage == DevelopmentalStage::CriticalPeriod) {
            parent->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    } else if (core->currentStep >= 40000) {
        if (core->developmentalStage == DevelopmentalStage::Maturation) {
            parent->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
}

void BrainIntegration::updatePlasticityRates(Brain* parent) {
    // Update plasticity rates based on development
    if (auto* sp = parent->getStructuralPlasticity()) {
        DevelopmentalStage stage = parent->getDevelopmentalStage();
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;
                break;
        }
        
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

} // namespace nlm