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
    
    // Apply stage-specific effects on brain systems
    if (brain_) {
        // 1. Neural excitability changes with development
        applyExcitabilityEffects(brain, dt);
        
        // 2. Neuromodulation level adjustments
        applyNeuromodulationEffects(brain, dt);
        
        // 3. Attention system modulation
        applyAttentionEffects(brain, dt);
        
        // 4. Memory consolidation enhancement
        applyMemoryEffects(brain, dt);
        
        // 5. Plasticity rule modulation
        applyPlasticityEffects(brain, dt);
        
        // 6. Structural plasticity changes
        applyStructuralEffects(brain, rng, dt);
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

// Additional development system methods

void DevelopmentSystem::applyExcitabilityEffects(Brain* brain, TimestepDuration dt) {
    // Development stages affect neural excitability
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High excitability in initial stage
            brain->injectCurrentToNeurons(NeuronType::Internal, 20.0f * static_cast<float>(dt));
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Moderate excitability, but tuned
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f * static_cast<float>(dt));
            break;
        case DevelopmentalStage::Maturation:
            // Lower excitability, more stable
            brain->injectCurrentToNeurons(NeuronType::Internal, 5.0f * static_cast<float>(dt));
            break;
        case DevelopmentalStage::Adult:
            // Low excitability in adult stage
            brain->injectCurrentToNeurons(NeuronType::Internal, 2.0f * static_cast<float>(dt));
            break;
        case DevelopmentalStage::Aging:
            // Very low excitability, reduced responsiveness
            brain->injectCurrentToNeurons(NeuronType::Internal, 1.0f * static_cast<float>(dt));
            break;
        default:
            break;
    }
}

void DevelopmentSystem::applyNeuromodulationEffects(Brain* brain, TimestepDuration dt) {
    // Development affects neuromodulation levels
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High dopamine for exploration and learning
            if (brain->getDopamine()) {
                brain->getDopamine()->setLevel(std::min(1.0f, brain->getDopamine()->getLevel() + 0.05f * static_cast<float>(dt)));
            }
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Moderate norepinephrine for attention and focus
            if (brain->getNorepinephrine()) {
                brain->getNorepinephrine()->setLevel(std::min(1.0f, brain->getNorepinephrine()->getLevel() + 0.03f * static_cast<float>(dt)));
            }
            break;
        case DevelopmentalStage::Maturation:
            // Increasing acetylcholine for memory consolidation
            if (brain->getAcetylcholine()) {
                brain->getAcetylcholine()->setLevel(std::min(1.0f, brain->getAcetylcholine()->getLevel() + 0.02f * static_cast<float>(dt)));
            }
            break;
        case DevelopmentalStage::Adult:
            // Balanced neuromodulation
            if (brain->getSerotonin()) {
                brain->getSerotonin()->setLevel(std::min(1.0f, brain->getSerotonin()->getLevel() + 0.01f * static_cast<float>(dt)));
            }
            break;
        default:
            break;
    }
}

void DevelopmentSystem::applyAttentionEffects(Brain* brain, TimestepDuration dt) {
    // Development affects attentional selection
    if (brain->getAttention()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // Broad attention, less selective
                brain->getAttention()->setInhibitionStrength(0.3f);
                brain->getAttention()->setExcitationStrength(1.0f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Emerging selective attention
                brain->getAttention()->setInhibitionStrength(0.5f);
                brain->getAttention()->setExcitationStrength(1.5f);
                break;
            case DevelopmentalStage::Maturation:
                // Mature, selective attention
                brain->getAttention()->setInhibitionStrength(0.7f);
                brain->getAttention()->setExcitationStrength(2.0f);
                break;
            case DevelopmentalStage::Adult:
                // Focused, selective attention
                brain->getAttention()->setInhibitionStrength(0.8f);
                brain->getAttention()->setExcitationStrength(2.5f);
                break;
            default:
                break;
        }
    }
}

void DevelopmentSystem::applyMemoryEffects(Brain* brain, TimestepDuration dt) {
    // Development enhances memory consolidation
    if (brain->getWorkingMemory()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // Basic working memory
                brain->getWorkingMemory()->setDecayRate(0.1f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Improving working memory
                brain->getWorkingMemory()->setDecayRate(0.05f);
                break;
            case DevelopmentalStage::Maturation:
                // Strong working memory
                brain->getWorkingMemory()->setDecayRate(0.02f);
                break;
            case DevelopmentalStage::Adult:
                // Well-developed working memory
                brain->getWorkingMemory()->setDecayRate(0.01f);
                break;
            default:
                break;
        }
    }
    
    if (brain->getEpisodicMemory()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // Early episodic memory formation
                // (Implementation depends on EpisodicMemory interface)
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Consolidating episodic memories
                break;
            case DevelopmentalStage::Maturation:
                // Rich episodic memory
                break;
            case DevelopmentalStage::Adult:
                // Well-developed episodic memory
                break;
            default:
                break;
        }
    }
}

void DevelopmentSystem::applyPlasticityEffects(Brain* brain, TimestepDuration dt) {
    // Development modulates plasticity rules
    if (brain->getSTDP()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High STDP plasticity
                brain->getSTDP()->setLTPWeight(0.05f);
                brain->getSTDP()->setLTDWeight(0.06f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Tuning STDP parameters
                brain->getSTDP()->setLTPWeight(0.04f);
                brain->getSTDP()->setLTDWeight(0.05f);
                break;
            case DevelopmentalStage::Maturation:
                // Stabilizing STDP
                brain->getSTDP()->setLTPWeight(0.03f);
                brain->getSTDP()->setLTDWeight(0.04f);
                break;
            case DevelopmentalStage::Adult:
                // Low STDP plasticity
                brain->getSTDP()->setLTPWeight(0.02f);
                brain->getSTDP()->setLTDWeight(0.03f);
                break;
            default:
                break;
        }
    }
}

void DevelopmentSystem::applyStructuralEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Development affects structural plasticity
    if (brain->getStructuralPlasticity()) {
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High synaptogenesis, low pruning
                brain->getStructuralPlasticity()->setSynaptogenesisRate(0.001f);
                brain->getStructuralPlasticity()->setPruningRate(0.0001f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Moderate synaptogenesis, increasing pruning
                brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0005f);
                brain->getStructuralPlasticity()->setPruningRate(0.0002f);
                break;
            case DevelopmentalStage::Maturation:
                // Low synaptogenesis, moderate pruning
                brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f);
                brain->getStructuralPlasticity()->setPruningRate(0.0001f);
                break;
            case DevelopmentalStage::Adult:
                // Very low synaptogenesis, high pruning
                brain->getStructuralPlasticity()->setSynaptogenesisRate(0.00001f);
                brain->getStructuralPlasticity()->setPruningRate(0.00005f);
                break;
            default:
                break;
        }
    }
}

} // namespace nlm
