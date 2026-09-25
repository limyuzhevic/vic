#include "DevelopmentalController.hpp"
#include "../core/Constants.hpp"

namespace nlm {

DevelopmentalController::DevelopmentalController()
    : developmentalAge_(0.0), currentStage_(DevelopmentalStage::Initial),
      developmentEnabled_(true), structuralPlasticityEnabled_(true) {}

DevelopmentalController::~DevelopmentalController() {}

void DevelopmentalController::updateDevelopment(double timestep, double& developmentalAge,
                                                float& plasticityModifier,
                                                bool developmentEnabled) {
    if (!developmentEnabled) return;
    
    developmentalAge_ += timestep;
    developmentalAge = developmentalAge_;
    
    // Use developmental constants for stage transitions
    if (developmentalAge_ < DevelopmentConstants::DEVELOPMENT_INITIAL_THRESHOLD) {
        currentStage_ = DevelopmentalStage::Initial;
        plasticityModifier = DevelopmentConstants::DEVELOPMENT_PLASTICITY_INITIAL;
    } else if (developmentalAge_ < DevelopmentConstants::DEVELOPMENT_CRITICAL_PERIOD_THRESHOLD) {
        currentStage_ = DevelopmentalStage::CriticalPeriod;
        plasticityModifier = DevelopmentConstants::DEVELOPMENT_PLASTICITY_CRITICAL;
    } else if (developmentalAge_ < DevelopmentConstants::DEVELOPMENT_MATURATION_THRESHOLD) {
        currentStage_ = DevelopmentalStage::Maturation;
        plasticityModifier = DevelopmentConstants::DEVELOPMENT_PLASTICITY_MATURATION;
    } else {
        currentStage_ = DevelopmentalStage::Adult;
        plasticityModifier = DevelopmentConstants::DEVELOPMENT_PLASTICITY_ADULT;
    }
}

DevelopmentalStage DevelopmentalController::getDevelopmentalStage(double developmentalAge) {
    if (developmentalAge < DevelopmentConstants::DEVELOPMENT_INITIAL_THRESHOLD) {
        return DevelopmentalStage::Initial;
    } else if (developmentalAge < DevelopmentConstants::DEVELOPMENT_CRITICAL_PERIOD_THRESHOLD) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge < DevelopmentConstants::DEVELOPMENT_MATURATION_THRESHOLD) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

const char* DevelopmentalController::developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        default: return "Unknown";
    }
}

bool DevelopmentalController::isDevelopmentActive(double developmentalAge) const {
    return developmentEnabled_ && developmentalAge_ < DevelopmentConstants::DEVELOPMENT_MATURATION_THRESHOLD;
}

float DevelopmentalController::getPlasticityForStage(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return DevelopmentConstants::DEVELOPMENT_PLASTICITY_INITIAL;
        case DevelopmentalStage::CriticalPeriod: return DevelopmentConstants::DEVELOPMENT_PLASTICITY_CRITICAL;
        case DevelopmentalStage::Maturation: return DevelopmentConstants::DEVELOPMENT_PLASTICITY_MATURATION;
        case DevelopmentalStage::Adult: return DevelopmentConstants::DEVELOPMENT_PLASTICITY_ADULT;
        default: return 0.1f;
    }
}

const DevelopmentalController::DevelopmentalStageConfig* DevelopmentalController::getStageConfig(DevelopmentalStage stage) {
    for (const auto& config : stageConfigs_) {
        if (config.stage == stage) {
            return &config;
        }
    }
    return nullptr;
}

// Initialize stage configurations
const DevelopmentalController::DevelopmentalStageConfig DevelopmentalController::stageConfigs_[] = {
    {DevelopmentalStage::Initial, DevelopmentConstants::DEVELOPMENT_INITIAL_THRESHOLD, 
     DevelopmentConstants::DEVELOPMENT_PLASTICITY_INITIAL},
    {DevelopmentalStage::CriticalPeriod, DevelopmentConstants::DEVELOPMENT_CRITICAL_PERIOD_THRESHOLD,
     DevelopmentConstants::DEVELOPMENT_PLASTICITY_CRITICAL},
    {DevelopmentalStage::Maturation, DevelopmentConstants::DEVELOPMENT_MATURATION_THRESHOLD,
     DevelopmentConstants::DEVELOPMENT_PLASTICITY_MATURATION},
    {DevelopmentalStage::Adult, DevelopmentConstants::DEVELOPMENT_ADULT_THRESHOLD,
     DevelopmentConstants::DEVELOPMENT_PLASTICITY_ADULT}
};

} // namespace nlm