#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/// Implementation details for DevelopmentSystem
struct DevelopmentSystem::Impl {
    /// Current developmental stage the brain is in
    DevelopmentalStage stage;
    
    /// Simulation step when current stage began
    SimulationStep stageStartStep;
    
    /// Number of steps spent in current stage
    SimulationStep stepsInCurrentStage;
    
    /// Age of current developmental stage (in simulation time units)
    double stageAge;  
    
    /// Initialize with default developmental stage
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

/// Constructor for DevelopmentSystem
DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {
    NLM_LOG_INFO("Development system initialized with initial stage");
}

/// Destructor for DevelopmentSystem
DevelopmentSystem::~DevelopmentSystem() = default;

/// Get the current developmental stage
/// @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, or Aging)
DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

/// Set the developmental stage to a specific stage
/// @param stage The developmental stage to set
void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    if (pImpl->stage != stage) {
        auto oldStage = pImpl->stage;
        pImpl->stage = stage;
        pImpl->stageAge = 0.0;
        pImpl->stepsInCurrentStage = 0;
        NLM_LOG_INFO("Development system advanced from " + std::string(getStageName(oldStage)) + 
                    " to " + std::string(getStageName(stage)));
    }
}

/// Advance to the next developmental stage in sequence
/// Automatically progresses from Initial -> CriticalPeriod -> Maturation -> Adult -> Aging
void DevelopmentSystem::advanceStage() {
    auto oldStage = pImpl->stage;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage, no change
            break;
        default:
            NLM_LOG_WARNING("Unknown developmental stage, cannot advance");
            return;
    }
    
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development system advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

} // namespace nlm
