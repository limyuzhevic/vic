#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
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
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
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
    
    // Update sleep/rest cycle - simple version
    pImpl->stageAge += 0.001;
    if (pImpl->stage == DevelopmentalStage::Adult) {
        // Can enter sleep/rest
        double sleepThreshold = 1200.0;  // After 20 minutes of adult stage
        if (age_ > sleepThreshold && age_ < sleepThreshold + 10.0) {
            // Enter rest phase
            NLM_LOG_INFO("Development: Entering rest/sleep phase for consolidation");
        }
        
        if (age_ > sleepThreshold + 10.0) {
            // Exit rest phase and consolidate memory
            NLM_LOG_INFO("Development: Exiting rest phase, consolidating memory");
            if (brain) {
                auto* episodicMemory = brain->getEpisodicMemory();
                if (episodicMemory) {
                    // Simple consolidation - remove less important memories
                    episodicMemory->consolidate(0.3f);
                }
            }
        }
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
    
    // Update sleep/rest cycle with continuous time
    if (pImpl->stage == DevelopmentalStage::Adult) {
        // Can enter sleep/rest at specific times
        double sleepStart = 1200.0;  // After 20 minutes of adult stage
        double sleepDuration = 300.0;  // 5 minutes sleep
        
        if (age_ > sleepStart && age_ < sleepStart + sleepDuration) {
            // Enter rest phase
            NLM_LOG_INFO("Development: Entering rest/sleep phase for consolidation");
            
            // During rest, replay and consolidate memories
            if (brain) {
                auto* episodicMemory = brain->getEpisodicMemory();
                if (episodicMemory && episodicMemory->isReplayEnabled()) {
                    // Select episodes for replay based on importance
                    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
                    for (const auto* episode : episodesToReplay) {
                        episodicMemory->replayEpisode(episode);
                    }
                }
            }
        }
        
        if (age_ > sleepStart + sleepDuration) {
            // Memory consolidation after rest
            if (brain) {
                auto* episodicMemory = brain->getEpisodicMemory();
                if (episodicMemory) {
                    episodicMemory->consolidate(0.5f);  // Higher threshold after sleep
                }
            }
            NLM_LOG_INFO("Development: Memory consolidation complete after rest");
        }
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
