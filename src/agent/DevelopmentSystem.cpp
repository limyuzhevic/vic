#include "DevelopmentSystem.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

void DevelopmentSystem::initialize() {
    developmentalAge_ = AgentBrainConstants::DEVELOPMENT_INITIAL;
    plasticityModifier_ = AgentBrainConstants::PLASTICITY_INITIAL;
}

void DevelopmentSystem::updateDevelopment(
    double timestep,
    double& developmentalAge,
    float& plasticityModifier,
    class Brain* brain,
    bool structuralPlasticityEnabled) {
    
    if (!validateDevelopmentParams(timestep, developmentalAge)) {
        return;
    }
    
    developmentalAge += timestep;
    
    // Get developmental stage and plasticity modifier
    DevelopmentalStage stage = getDevelopmentalStageFromAge(developmentalAge);
    plasticityModifier = calculatePlasticityModifier(developmentalAge);
    
    if (brain) {
        brain->setDevelopmentalStage(stage);
    }
    
    // Update structural plasticity if enabled
    if (structuralPlasticityEnabled) {
        auto* sp = brain ? brain->getStructuralPlasticity() : nullptr;
        if (sp) {
            calculateStructuralPlasticityRates(plasticityModifier, 
                                             sp->synaptogenesisRate, 
                                             sp->pruningRate);
            sp->setSynaptogenesisRate(sp->synaptogenesisRate);
            sp->setPruningRate(sp->pruningRate);
        }
    }
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStageFromAge(double developmentalAge) {
    if (developmentalAge < AgentBrainConstants::DEVELOPMENT_CRITICAL) {
        return DevelopmentalStage::Initial;
    } else if (developmentalAge < AgentBrainConstants::DEVELOPMENT_MATURATION) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge < AgentBrainConstants::DEVELOPMENT_ADULT) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

float DevelopmentSystem::calculatePlasticityModifier(double developmentalAge) {
    if (developmentalAge < AgentBrainConstants::DEVELOPMENT_CRITICAL) {
        return AgentBrainConstants::PLASTICITY_INITIAL;
    } else if (developmentalAge < AgentBrainConstants::DEVELOPMENT_MATURATION) {
        return AgentBrainConstants::PLASTICITY_CRITICAL_PERIOD;
    } else if (developmentalAge < AgentBrainConstants::DEVELOPMENT_ADULT) {
        return AgentBrainConstants::PLASTICITY_MMATURATION;
    } else {
        return AgentBrainConstants::PLASTICITY_ADULT;
    }
}

void DevelopmentSystem::calculateStructuralPlasticityRates(
    float plasticityModifier,
    float& synaptogenesisRate,
    float& pruningRate) {
    synaptogenesisRate = AgentBrainConstants::SYNAPTOGENESIS_RATE * plasticityModifier;
    pruningRate = AgentBrainConstants::PRUNING_RATE * (2.0f - plasticityModifier);
}

bool DevelopmentSystem::validateDevelopmentParams(double timestep, double developmentalAge) {
    // Check for valid timestep
    if (timestep < 0.0) {
        return false;
    }
    
    // Check for NaN or infinite age
    if (std::isnan(developmentalAge) || std::isinf(developmentalAge)) {
        return false;
    }
    
    return true;
}

} // namespace nlm