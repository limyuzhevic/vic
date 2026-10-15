#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Enhanced development effects
    bool attentionModulationEnabled;
    bool memoryConsolidationEnabled;
    bool neuromodulationModulationEnabled;
    bool neuralExcitabilityModulationEnabled;
    bool structuralPlasticityEnhanced;
    
    // Development state tracking
    double currentAttentionLevel;
    double currentMemoryConsolidationRate;
    double currentNeuromodulationLevel;
    double currentNeuralExcitability;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
           , attentionModulationEnabled(true), memoryConsolidationEnabled(true), neuromodulationModulationEnabled(true)
           , neuralExcitabilityModulationEnabled(true), structuralPlasticityEnhanced(true)
           , currentAttentionLevel(0.5), currentMemoryConsolidationRate(0.5)
           , currentNeuromodulationLevel(0.5), currentNeuralExcitability(0.5) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    
    // Recalculate development effects when stage changes
    applyDevelopmentalEffects(nullptr);
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
    
    // Apply enhanced developmental effects for new stage
    applyDevelopmentalEffects(nullptr);
    
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
    
    // Apply enhanced developmental effects
    applyDevelopmentalEffects(brain);
    
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
    
    // Apply enhanced developmental effects
    applyDevelopmentalEffects(brain);
    
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

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain) {
    if (!brain || !pImpl) return;
    
    float stageModifier = getPlasticityModifier();
    
    // Enhance attention modulation
    if (pImpl->attentionModulationEnabled && brain_->getAttention()) {
        // Different developmental stages have different effects on attention
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High attention selection in early development
                pImpl->currentAttentionLevel = std::min(1.0f, pImpl->currentAttentionLevel + 0.1f);
                brain_->getAttention()->setInhibitionStrength(0.3f);  // Lower inhibition = more exploration
                brain_->getAttention()->setExcitationStrength(2.0f);   // Higher excitation = more selective
                break;
            
            case DevelopmentalStage::CriticalPeriod:
                // Peak attention mechanism in critical period
                pImpl->currentAttentionLevel = 1.0f;
                brain_->getAttention()->setInhibitionStrength(0.5f);  // Balanced inhibition
                brain_->getAttention()->setExcitationStrength(1.5f);   // Moderately high excitation
                break;
            
            case DevelopmentalStage::Maturation:
                // Fine-tuning of attention
                pImpl->currentAttentionLevel = 0.8f;
                brain_->getAttention()->setInhibitionStrength(0.7f);  // Higher inhibition = more focused
                brain_->getAttention()->setExcitationStrength(1.2f);   // Reduced excitation
                break;
            
            case DevelopmentalStage::Adult:
                // Stable, efficient attention
                pImpl->currentAttentionLevel = 0.6f;
                brain_->getAttention()->setInhibitionStrength(0.8f);  // High inhibition = efficient filtering
                brain_->getAttention()->setExcitationStrength(1.0f);   // Baseline excitation
                break;
            
            case DevelopmentalStage::Aging:
                // Reduced attentional flexibility
                pImpl->currentAttentionLevel = 0.4f;
                brain_->getAttention()->setInhibitionStrength(0.9f);  // Very high inhibition
                brain_->getAttention()->setExcitationStrength(0.8f);   // Reduced excitation
                break;
        }
    }
    
    // Enhance memory consolidation
    if (pImpl->memoryConsolidationEnabled && brain_->getWorkingMemory()) {
        // Different developmental stages affect memory consolidation
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High memory consolidation in early development
                pImpl->currentMemoryConsolidationRate = 0.8f;
                brain_->getWorkingMemory()->setDecayRate(0.005f);  // Slower decay = better consolidation
                break;
            
            case DevelopmentalStage::CriticalPeriod:
                // Peak memory consolidation in critical period
                pImpl->currentMemoryConsolidationRate = 1.0f;
                brain_->getWorkingMemory()->setDecayRate(0.002f);  // Very slow decay
                break;
            
            case DevelopmentalStage::Maturation:
                // Balanced consolidation and forgetting
                pImpl->currentMemoryConsolidationRate = 0.6f;
                brain_->getWorkingMemory()->setDecayRate(0.01f);   // Moderate decay
                break;
            
            case DevelopmentalStage::Adult:
                // Efficient memory management
                pImpl->currentMemoryConsolidationRate = 0.4f;
                brain_->getWorkingMemory()->setDecayRate(0.02f);   // Faster decay
                break;
            
            case DevelopmentalStage::Aging:
                // Reduced memory consolidation efficiency
                pImpl->currentMemoryConsolidationRate = 0.2f;
                brain_->getWorkingMemory()->setDecayRate(0.05f);   // Fast decay
                break;
        }
    }
    
    // Enhance neuromodulation effects
    if (pImpl->neuromodulationModulationEnabled) {
        float neuromodulationStrength = stageModifier * 0.8f;
        
        // Modulate different neuromodulators based on developmental stage
        if (brain_->getDopamine()) {
            // Dopamine affects reward processing and learning
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial:
                    // High dopamine in early development = exploration
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.8f);
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    // Optimal dopamine for learning
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 1.0f);
                    break;
                case DevelopmentalStage::Maturation:
                    // Modulated dopamine
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.6f);
                    break;
                case DevelopmentalStage::Adult:
                    // Baseline dopamine
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.4f);
                    break;
                case DevelopmentalStage::Aging:
                    // Reduced dopamine signaling
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.2f);
                    break;
            }
        }
        
        if (brain_->getCuriosity()) {
            // Curiosity drives exploration based on developmental stage
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial:
                    // High curiosity in early development = lots of exploration
                    brain_->getCuriosity()->setLevel(neuromodulationStrength * 0.9f);
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    // Optimal curiosity for discovery
                    brain_->getCuriosity()->setLevel(neuromodulationStrength * 1.0f);
                    break;
                case DevelopmentalStage::Maturation:
                    // Curated curiosity
                    brain_->getCuriosity()->setLevel(neuromodulationStrength * 0.7f);
                    break;
                case DevelopmentalStage::Adult:
                    // Goal-directed curiosity
                    brain_->getCuriosity()->setLevel(neuromodulationStrength * 0.5f);
                    break;
                case DevelopmentalStage::Aging:
                    // Reduced curiosity
                    brain_->getCuriosity()->setLevel(neuromodulationStrength * 0.3f);
                    break;
            }
        }
        
        if (brain_->getNovelty()) {
            // Novelty detection sensitivity based on developmental stage
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial:
                    // High novelty detection = lots of new experiences
                    brain_->getNovelty()->setLevel(neuromodulationStrength * 0.7f);
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    // Peak novelty sensitivity
                    brain_->getNovelty()->setLevel(neuromodulationStrength * 1.0f);
                    break;
                case DevelopmentalStage::Maturation:
                    // Filter novel events
                    brain_->getNovelty()->setLevel(neuromodulationStrength * 0.5f);
                    break;
                case DevelopmentalStage::Adult:
                    // Efficient novelty detection
                    brain_->getNovelty()->setLevel(neuromodulationStrength * 0.3f);
                    break;
                case DevelopmentalStage::Aging:
                    // Reduced novelty detection
                    brain_->getNovelty()->setLevel(neuromodulationStrength * 0.1f);
                    break;
            }
        }
    }
    
    // Enhance neural excitability
    if (pImpl->neuralExcitabilityModulationEnabled) {
        // Different developmental stages affect neural excitability
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High neural excitability = rapid learning
                pImpl->currentNeuralExcitability = 1.0f;
                if (brain_->getDopamine()) {
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.9f);
                }
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Optimal neural excitability
                pImpl->currentNeuralExcitability = 1.0f;
                if (brain_->getDopamine()) {
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 1.0f);
                }
                break;
            case DevelopmentalStage::Maturation:
                // Balanced excitability
                pImpl->currentNeuralExcitability = 0.7f;
                if (brain_->getDopamine()) {
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.7f);
                }
                break;
            case DevelopmentalStage::Adult:
                // Efficient, stable excitability
                pImpl->currentNeuralExcitability = 0.5f;
                if (brain_->getDopamine()) {
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.5f);
                }
                break;
            case DevelopmentalStage::Aging:
                // Reduced neural excitability
                pImpl->currentNeuralExcitability = 0.3f;
                if (brain_->getDopamine()) {
                    brain_->getDopamine()->setLevel(neuromodulationStrength * 0.3f);
                }
                break;
        }
    }
    
    // Enhanced structural plasticity
    if (pImpl->structuralPlasticityEnhanced) {
        // Structural plasticity also varies by developmental stage
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High structural plasticity = rapid wiring changes
                if (brain_->getStructuralPlasticity()) {
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0002f);
                    brain_->getStructuralPlasticity()->setPruningRate(0.00002f);
                }
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Peak structural plasticity
                if (brain_->getStructuralPlasticity()) {
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00015f);
                    brain_->getStructuralPlasticity()->setPruningRate(0.00001f);
                }
                break;
            case DevelopmentalStage::Maturation:
                // Refined structural plasticity
                if (brain_->getStructuralPlasticity()) {
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00008f);
                    brain_->getStructuralPlasticity()->setPruningRate(0.000008f);
                }
                break;
            case DevelopmentalStage::Adult:
                // Minimal structural plasticity for stability
                if (brain_->getStructuralPlasticity()) {
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00002f);
                    brain_->getStructuralPlasticity()->setPruningRate(0.000002f);
                }
                break;
            case DevelopmentalStage::Aging:
                // Reduced structural plasticity
                if (brain_->getStructuralPlasticity()) {
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00001f);
                    brain_->getStructuralPlasticity()->setPruningRate(0.000001f);
                }
                break;
        }
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    // Now also includes effects on excitability, attention, and memory
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;
        case DevelopmentalStage::Aging:
            return 0.1f;
        default:
            return 0.5f;
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
