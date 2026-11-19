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
    
    // Apply developmental effects to the brain
    if (brain) {
        applyToBrain(brain);
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
    
    // Apply developmental effects to the brain
    if (brain) {
        applyToBrain(brain);
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

void DevelopmentSystem::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply stage-specific developmental effects to the brain
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: high excitability, rapid learning
            // Apply high baseline excitability to all neurons
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Increase neural excitability for faster initial learning
                        neuron->injectCurrent(1.0f);
                    }
                }
            }
            // Enable strong memory consolidation
            if (auto* episodicMemory = brain->getEpisodicMemory()) {
                // Note: episodicMemory doesn't have consolidation strength method,
                // so we'll focus on enabling replay
                episodicMemory->enableReplay(true);
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: heightened plasticity and sensory refinement
            // Modify sensory neuron responsiveness based on experience
            if (auto* episodicMemory = brain->getEpisodicMemory()) {
                // Enhance consolidation of sensory experiences
                // Note: episodicMemory doesn't have consolidation strength, so we'll
                // focus on replay and updating relevance
                episodicMemory->enableReplay(true);
                // Get recent episodes and increase their relevance
                auto recentEpisodes = episodicMemory->getRecentEpisodes(10);
                for (const auto* episode : recentEpisodes) {
                    episodicMemory->updateRelevance(episode->timestamp, 0.1f);
                }
            }
            // Enhance working memory capacity for learning
            if (auto* workingMemory = brain->getWorkingMemory()) {
                workingMemory->setCapacity(workingMemory->getCapacity() * 1.5f);
                workingMemory->setDecayRate(0.5f * workingMemory->getDecayRate());  // Slower decay
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Maturation: balance stability and flexibility
            // Optimize synaptic efficiency
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Fine-tune neural thresholds for stable performance
                        neuron->setThreshold(neuron->getThreshold() * 0.9f);
                    }
                }
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Adult: stable, efficient operation with some adaptability
            // Maintain optimal performance with subtle learning capacity
            if (auto* workingMemory = brain->getWorkingMemory()) {
                workingMemory->setDecayRate(workingMemory->getDecayRate() * 1.2f);  // Slightly faster decay
            }
            break;
            
        case DevelopmentalStage::Aging:
            // Aging: reduced plasticity, preserved knowledge
            // Strengthen existing memory traces
            if (auto* episodicMemory = brain->getEpisodicMemory()) {
                // Note: episodicMemory doesn't have consolidation strength method,
                // so we'll focus on reducing replay for stability
                episodicMemory->enableReplay(false);
            }
            // Increase working memory decay for flexibility
            if (auto* workingMemory = brain->getWorkingMemory()) {
                workingMemory->setDecayRate(workingMemory->getDecayRate() * 2.0f);
            }
            break;
    }
    
    // Stage-independent effects
    // Modulate neuromodulator sensitivity based on developmental stage
    if (auto* dopamine = brain->getDopamine()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                dopamine->setLevel(std::min(1.0f, dopamine->getLevel() * 1.5f));  // Boost reward sensitivity
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Maintain moderate dopamine for exploration
                break;
            case DevelopmentalStage::Adult:
                // Fine-tune dopamine for optimal performance
                dopamine->setLevel(std::max(0.3f, dopamine->getLevel() * 0.9f));
                break;
            default:
                break;
        }
    }
    
    // Update developmental age
    age_ = pImpl->stageAge;
}

} // namespace nlm
