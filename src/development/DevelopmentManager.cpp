#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentManager::Impl {
    Brain* brain;
    StructuralPlasticity* structuralPlasticity;
    DevelopmentSystem* developmentSystem;
    SimulationStep lastDevelopmentUpdate;
    SimulationStep lastPlasticityUpdate;
    
    // Development tracking
    std::unordered_map<DevelopmentalStage, SimulationStep> stageStartTimes;
    SimulationStep previousStage;
    
    // Plasticity modifiers by stage
    std::unordered_map<DevelopmentalStage, float> stagePlasticityModifiers;
    std::unordered_map<DevelopmentalStage, float> stageSynaptogenesisRates;
    std::unordered_map<DevelopmentalStage, float> stagePruningRates;
    
    Impl(Brain* br, StructuralPlasticity* sp, DevelopmentSystem* ds)
        : brain(br), structuralPlasticity(sp), developmentSystem(ds)
        , lastDevelopmentUpdate(0), lastPlasticityUpdate(0)
        , previousStage(DevelopmentalStage::Initial) {
        
        // Initialize stage plasticity modifiers
        stagePlasticityModifiers[DevelopmentalStage::Initial] = 1.0f;    // High plasticity
        stagePlasticityModifiers[DevelopmentalStage::CriticalPeriod] = 0.8f;
        stagePlasticityModifiers[DevelopmentalStage::Maturation] = 0.5f;
        stagePlasticityModifiers[DevelopmentalStage::Adult] = 0.2f;     // Stable
        stagePlasticityModifiers[DevelopmentalStage::Aging] = 0.1f;      // Decreasing
        
        // Initialize synaptogenesis rates
        stageSynaptogenesisRates[DevelopmentalStage::Initial] = 0.0001f;
        stageSynaptogenesisRates[DevelopmentalStage::CriticalPeriod] = 0.00008f;
        stageSynaptogenesisRates[DevelopmentalStage::Maturation] = 0.00005f;
        stageSynaptogenesisRates[DevelopmentalStage::Adult] = 0.00002f;
        stageSynaptogenesisRates[DevelopmentalStage::Aging] = 0.00001f;
        
        // Initialize pruning rates
        stagePruningRates[DevelopmentalStage::Initial] = 0.00001f;
        stagePruningRates[DevelopmentalStage::CriticalPeriod] = 0.000015f;
        stagePruningRates[DevelopmentalStage::Maturation] = 0.00002f;
        stagePruningRates[DevelopmentalStage::Adult] = 0.00001f;
        stagePruningRates[DevelopmentalStage::Aging] = 0.000005f;
        
        stageStartTimes[DevelopmentalStage::Initial] = 0;
    }
};

DevelopmentManager::DevelopmentManager(Brain* brain, StructuralPlasticity* structuralPlasticity,
                                     DevelopmentSystem* developmentSystem)
    : pImpl(new Impl(brain, structuralPlasticity, developmentSystem))
    , updateInterval_(1000)
    , developmentalAge_(0.0) {
    
    // Initialize stage thresholds
    setStageThresholds(60.0, 360.0, 960.0, 1440.0, 2160.0);
    
    NLM_LOG_INFO("DevelopmentManager initialized");
}

DevelopmentManager::~DevelopmentManager() {
    delete pImpl;
}

void DevelopmentManager::update(SimulationStep currentStep, TimestepDuration dt,
                               RandomGenerator& rng, DevelopmentalStage currentStage) {
    ++stats_.totalUpdates;
    developmentalAge_ += dt;
    
    // Update development system periodically
    if (currentStep % updateInterval_ == 0 && pImpl->brain && pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->brain, currentStep);
        ++stats_.plasticityUpdates;
    }
    
    // Apply development effects on plasticity
    if (pImpl->structuralPlasticity && pImpl->brain) {
        applyPlasticityEffects(currentStage, pImpl->structuralPlasticity);
    }
    
    // Manage developmental stage transitions
    manageStageTransition(currentStage, currentStep);
    
    // Update statistics
    if (pImpl->developmentSystem) {
        stats_.criticalPeriodProgress = pImpl->developmentSystem->getCriticalPeriodProgress();
        stats_.averagePlasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
    }
    
    // Track stage duration
    if (pImpl->stageStartTimes.find(currentStage) != pImpl->stageStartTimes.end()) {
        SimulationStep stageStart = pImpl->stageStartTimes[currentStage];
        stats_.stageDurations[currentStage] += (currentStep - stageStart);
    }
    
    pImpl->stageStartTimes[currentStage] = currentStep;
}

void DevelopmentManager::applyPlasticityEffects(DevelopmentalStage stage,
                                               StructuralPlasticity* structuralPlasticity) {
    if (!structuralPlasticity) return;
    
    // Apply stage-specific plasticity modifiers
    float plasticityMod = pImpl->stagePlasticityModifiers[stage];
    float synaptogenesisRate = pImpl->stageSynaptogenesisRates[stage];
    float pruningRate = pImpl->stagePruningRates[stage];
    
    // Update structural plasticity parameters
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
    
    // Update cumulative statistics
    stats_.averagePlasticityModifier = (stats_.averagePlasticityModifier + plasticityMod) / 2.0f;
    stats_.cumulativeMaturationRate += (1.0f - plasticityMod);
    
    // Track synaptic events
    stats_.synapticEventsCreated += static_cast<SimulationStep>(synaptogenesisRate * 1000);
    stats_.synapsesPruned += static_cast<SimulationStep>(pruningRate * 1000);
}

void DevelopmentManager::manageStageTransition(DevelopmentalStage currentStage,
                                               SimulationStep currentStep) {
    if (currentStage != pImpl->previousStage) {
        // Track stage transition
        ++stats_.stageTransitions;
        
        // Record previous stage duration
        if (pImpl->stageStartTimes.find(pImpl->previousStage) != pImpl->stageStartTimes.end()) {
            SimulationStep stageStart = pImpl->stageStartTimes[pImpl->previousStage];
            stats_.stageDurations[pImpl->previousStage] += (currentStep - stageStart);
        }
        
        pImpl->previousStage = currentStage;
        pImpl->stageStartTimes[currentStage] = currentStep;
        
        // Log stage transition
        const char* stageName = "";
        switch (currentStage) {
            case DevelopmentalStage::Initial: stageName = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: stageName = "CriticalPeriod"; break;
            case DevelopmentalStage::Maturation: stageName = "Maturation"; break;
            case DevelopmentalStage::Adult: stageName = "Adult"; break;
            case DevelopmentalStage::Aging: stageName = "Aging"; break;
        }
        
        NLM_LOG_INFO("DevelopmentManager: Advanced to " + std::string(stageName) + 
                    " stage at step " + std::to_string(currentStep));
    }
}

void DevelopmentManager::setStageThresholds(double initial, double criticalPeriod,
                                           double maturation, double adult, double aging) {
    stageThresholds_[0] = initial;
    stageThresholds_[1] = criticalPeriod;
    stageThresholds_[2] = maturation;
    stageThresholds_[3] = adult;
    stageThresholds_[4] = aging;
}

} // namespace nlm