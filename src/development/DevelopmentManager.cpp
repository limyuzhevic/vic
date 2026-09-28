#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct DevelopmentManager::Impl {
    Brain* brain = nullptr;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Configuration
    float criticalPeriodDuration = 1000.0f; // Steps
    float maturationRate = 0.01f;          // Rate per step
    float adultStability = 0.2f;           // Plasticity floor
    float plasticityDecayRate = 0.0001f;   // Per step
    
    // State
    DevelopmentalStage currentStage = DevelopmentalStage::Initial;
    double stageStartTime = 0.0;
    double criticalPeriodEndTime = 0.0;
    double maturationEndTime = 0.0;
    double developmentalAge = 0.0;
    
    float stageProgress = 0.0f;
    float criticalPeriodProgress = 0.0f;
    float maturationProgress = 0.0f;
    
    // Statistics
    double totalDevelopmentTime = 0.0;
    uint32_t developmentSteps = 0;
    double lastDevelopmentUpdate = 0.0;
    
    // Control
    float developmentSpeed = 1.0f;
    double nextDevelopmentStep = 0.0;
    
    // Milestones
    std::vector<std::string> milestonesReached;
};

DevelopmentManager::DevelopmentManager() : pImpl(std::make_unique<Impl>()) {}

DevelopmentManager::~DevelopmentManager() = default;

void DevelopmentManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Configure initial state
    pImpl->currentStage = DevelopmentalStage::Initial;
    pImpl->stageStartTime = 0.0;
    pImpl->criticalPeriodEndTime = pImpl->criticalPeriodDuration;
    pImpl->maturationEndTime = pImpl->criticalPeriodEndTime * 2.0;
    
    pImpl->lastDevelopmentUpdate = 0.0;
    pImpl->nextDevelopmentStep = 1.0; // First development step at step 1
    
    NLM_LOG_INFO("DevelopmentManager initialized");
}

void DevelopmentManager::update(float timestep) {
    if (!pImpl->developmentSystem) return;
    
    double currentStep = pImpl->brain->getCurrentStep();
    
    // Calculate developmental age
    pImpl->developmentalAge = currentStep / 10.0; // Assume 0.1 time units per step
    pImpl->totalDevelopmentTime += timestep;
    
    // Check if development should advance
    if (shouldAdvanceDevelopment(currentStep)) {
        pImpl->developmentSteps++;
        
        // Update development system
        pImpl->developmentSystem->update(pImpl->brain, currentStep);
        
        // Update stage
        advanceStage();
        
        // Update progress calculations
        double timeSinceStart = currentStep - pImpl->stageStartTime;
        pImpl->stageProgress = static_cast<float>(std::min(timeSinceStart / 1000.0, 1.0));
        
        // Calculate critical period progress
        if (currentStage < DevelopmentalStage::CriticalPeriod) {
            pImpl->criticalPeriodProgress = static_cast<float>(timeSinceStart / pImpl->criticalPeriodDuration);
        } else {
            pImpl->criticalPeriodProgress = 1.0f;
        }
        
        // Calculate maturation progress
        if (currentStage < DevelopmentalStage::Maturation) {
            pImpl->maturationProgress = static_cast<float>(timeSinceStart / (pImpl->criticalPeriodDuration * 2.0));
        } else if (currentStage < DevelopmentalStage::Adult) {
            pImpl->maturationProgress = static_cast<float>((currentStep - pImpl->criticalPeriodDuration * 2.0) / 10000.0);
        } else {
            pImpl->maturationProgress = 1.0f;
        }
        
        // Schedule next development step
        pImpl->nextDevelopmentStep = currentStep + 1.0 / pImpl->developmentSpeed;
        
        NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
    }
    
    NLM_LOG_DEBUG("DevelopmentManager updated");
}

void DevelopmentManager::advanceStage() {
    pImpl->stageStartTime = pImpl->brain->getCurrentStep();
    
    DevelopmentalStage oldStage = pImpl->currentStage;
    
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial:
            pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->currentStage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->currentStage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            // Stay in adult stage
            break;
    }
    
    // Record milestone if changed
    if (oldStage != pImpl->currentStage) {
        triggerMilestone(getStageName());
    }
    
    NLM_LOG_INFO("Developmental stage advanced to: " + std::string(getStageName()));
}

void DevelopmentManager::configure(float criticalPeriodDuration, float maturationRate,
                                   float adultStability, float plasticityDecayRate) {
    pImpl->criticalPeriodDuration = criticalPeriodDuration;
    pImpl->maturationRate = maturationRate;
    pImpl->adultStability = adultStability;
    pImpl->plasticityDecayRate = plasticityDecayRate;
    
    // Recalculate critical period end time
    pImpl->criticalPeriodEndTime = pImpl->criticalPeriodDuration;
    pImpl->maturationEndTime = pImpl->criticalPeriodEndTime * 2.0;
    
    NLM_LOG_INFO("DevelopmentManager configured");
}

void DevelopmentManager::applyPlasticityModifiers(STDP* stdp, Hebbian* hebbian,
                                                 StructuralPlasticity* structuralPlasticity) {
    if (!stdp || !hebbian || !structuralPlasticity) return;
    
    float plasticityMod = getPlasticityModifier();
    
    // Calculate stage-specific modifiers
    float stageMod = 1.0f;
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial:
            stageMod = 1.0f; // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            stageMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            stageMod = pImpl->adultStability;
            break;
    }
    
    float totalMod = plasticityMod * stageMod;
    
    // Apply modifiers to plasticity systems
    float synapticRate = 0.0001f * totalMod;
    float pruningRate = 0.00001f * (2.0f - totalMod);
    
    structuralPlasticity->setSynaptogenesisRate(synapticRate);
    structuralPlasticity->setPruningRate(pruningRate);
    
    // Modify STDP weights based on development stage
    float stdpLTP = 0.01f * totalMod;
    float stdpLTD = 0.012f * totalMod;
    float stdpTau = 20.0f / totalMod; // Longer time constant with lower plasticity
    
    stdp->configure(stdpLTP, stdpLTD, stdpTau);
    
    // Modify Hebbian learning rate
    float hebbianRate = 0.01f * totalMod;
    hebbian->setLearningRate(hebbianRate);
    
    NLM_LOG_DEBUG("DevelopmentManager applied plasticity modifiers");
}

bool DevelopmentManager::shouldAdvanceDevelopment(double currentStep) const {
    return pImpl->developmentSpeed > 0.0f && currentStep >= pImpl->nextDevelopmentStep;
}

void DevelopmentManager::reset() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setStage(DevelopmentalStage::Initial);
    }
    
    pImpl->currentStage = DevelopmentalStage::Initial;
    pImpl->stageStartTime = 0.0;
    pImpl->criticalPeriodEndTime = pImpl->criticalPeriodDuration;
    pImpl->maturationEndTime = pImpl->criticalPeriodEndTime * 2.0;
    pImpl->developmentalAge = 0.0;
    
    pImpl->stageProgress = 0.0f;
    pImpl->criticalPeriodProgress = 0.0f;
    pImpl->maturationProgress = 0.0f;
    
    pImpl->developmentSteps = 0;
    pImpl->totalDevelopmentTime = 0.0;
    pImpl->lastDevelopmentUpdate = 0.0;
    pImpl->nextDevelopmentStep = 1.0;
    
    pImpl->milestonesReached.clear();
    
    NLM_LOG_INFO("DevelopmentManager reset");
}

void DevelopmentManager::triggerMilestone(const char* milestoneName) {
    pImpl->milestonesReached.push_back(std::string(milestoneName));
    NLM_LOG_INFO("Development milestone reached: " + std::string(milestoneName));
}

const char* DevelopmentManager::getStageName() const {
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        default: return "Unknown";
    }
}

DevelopmentalStage DevelopmentManager::getStage() const {
    return pImpl->currentStage;
}

double DevelopmentManager::getDevelopmentalAge() const {
    return pImpl->developmentalAge;
}

void DevelopmentManager::setDevelopmentalAge(double age) {
    pImpl->developmentalAge = age;
    // Calculate current stage based on age (simplified)
    if (age < pImpl->criticalPeriodDuration * 0.25) {
        pImpl->currentStage = DevelopmentalStage::Initial;
    } else if (age < pImpl->criticalPeriodDuration * 0.75) {
        pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (age < pImpl->maturationEndTime) {
        pImpl->currentStage = DevelopmentalStage::Maturation;
    } else {
        pImpl->currentStage = DevelopmentalStage::Adult;
    }
}

float DevelopmentManager::getCriticalPeriodProgress() const {
    return pImpl->criticalPeriodProgress;
}

float DevelopmentManager::getMaturationProgress() const {
    return pImpl->maturationProgress;
}

float DevelopmentManager::getAdultStabilityLevel() const {
    return pImpl->adultStability;
}

void DevelopmentManager::setDevelopmentSpeed(float speed) {
    pImpl->developmentSpeed = speed;
    // Recalculate next development step
    if (speed > 0.0f) {
        pImpl->nextDevelopmentStep = pImpl->brain->getCurrentStep() + 1.0 / speed;
    }
}

bool DevelopmentManager::isCriticalPeriod() const {
    return pImpl->currentStage == DevelopmentalStage::CriticalPeriod;
}

double DevelopmentManager::getTotalDevelopmentTime() const {
    return pImpl->totalDevelopmentTime;
}

uint32_t DevelopmentManager::getDevelopmentSteps() const {
    return pImpl->developmentSteps;
}

float DevelopmentManager::getPlasticityModifier() const {
    // Base plasticity modifier based on developmental age
    float ageFactor = std::min(static_cast<float>(pImpl->developmentalAge) / 1000.0f, 1.0f);
    
    // Decrease plasticity with age
    return 1.0f - pImpl->plasticityDecayRate * pImpl->developmentalAge;
}

} // namespace nlm
