#include "DevelopmentalStageManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct DevelopmentalStageManager::Impl {
    // Stage characteristics
    struct StageCharacteristics {
        const char* name;
        const char* description;
        double duration;           // Duration in simulation time
        float plasticity;          // Plasticity multiplier
        float learningRate;        // Learning rate multiplier
        float synaptogenesisRate;  // Synapse formation rate
        float pruningRate;         // Synapse pruning rate
        float curiosityDrive;      // Exploration motivation
        float memoryConsolidation;  // Memory consolidation strength
        float myelinationRate;     // Myelin development rate
        float connectivityGrowth;   // Network connectivity growth
        bool criticalPeriod;       // Whether this is a critical period
    };
    
    static const StageCharacteristics STAGE_DATA[5];
    
    DevelopmentalStage currentStage;
    double stageStartTime;
    double stageAge;
    
    Impl() : currentStage(DevelopmentalStage::Initial), stageStartTime(0.0), stageAge(0.0) {}
};

const DevelopmentalStageManager::Impl::StageCharacteristics DevelopmentalStageManager::Impl::STAGE_DATA[5] = {
    {"Initial", "Early development with rapid synaptogenesis and high plasticity", 60.0, 1.0f, 1.0f, 0.002f, 0.0001f, 0.3f, 0.2f, 0.1f, 0.8f, false},
    {"CriticalPeriod", "Sensory processing specialization with heightened plasticity", 180.0, 0.8f, 0.7f, 0.001f, 0.001f, 0.6f, 0.5f, 0.3f, 0.6f, true},
    {"Maturation", "Refinement of neural circuits and development of complex functions", 240.0, 0.5f, 0.4f, 0.0005f, 0.0005f, 0.4f, 0.7f, 0.5f, 0.4f, false},
    {"Adult", "Optimized performance with maintenance and selective changes", 360.0, 0.2f, 0.3f, 0.0001f, 0.0001f, 0.2f, 0.9f, 0.7f, 0.2f, false},
    {"Aging", "Reduced plasticity and maintenance with gradual decline", 180.0, 0.1f, 0.1f, 0.00005f, 0.0002f, 0.1f, 0.8f, 0.3f, 0.1f, false}
};

DevelopmentalStageManager::DevelopmentalStageManager() : pImpl(new Impl) {}

DevelopmentalStageManager::~DevelopmentalStageManager() = default;

void DevelopmentalStageManager::update(Brain* brain, double age, SimulationStep currentStep) {
    // Auto-advance stage based on age
    double currentStageDuration = Impl::STAGE_DATA[static_cast<int>(pImpl->currentStage)].duration;
    
    if (age - pImpl->stageStartTime >= currentStageDuration) {
        // Advance to next stage
        advanceStage();
        if (pImpl->currentStage != DevelopmentalStage::Aging) {
            pImpl->stageStartTime = age;
            pImpl->stageAge = 0.0;
        }
    } else {
        pImpl->stageAge = age - pImpl->stageStartTime;
    }
}

float DevelopmentalStageManager::getStagePlasticity() const {
    return Impl::STAGE_DATA[static_cast<int>(pImpl->currentStage)].plasticity;
}

float DevelopmentalStageManager::getStageSynapticDensity() const {
    return Impl::STAGE_DATA[static_cast<int>(pImpl->currentStage)].synaptogenesisRate;
}

float DevelopmentalStageManager::getStageLearningRate() const {
    return Impl::STAGE_DATA[static_cast<int>(pImpl->currentStage)].learningRate;
}

double DevelopmentalStageManager::getStageDuration(DevelopmentalStage stage) {
    return Impl::STAGE_DATA[static_cast<int>(stage)].duration;
}

const char* DevelopmentalStageManager::getStageDescription(DevelopmentalStage stage) const {
    return Impl::STAGE_DATA[static_cast<int>(stage)].description;
}

} // namespace nlm