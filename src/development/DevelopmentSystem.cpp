#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct DevelopmentSystem::Impl {
    EnhancedDevelopmentalStage enhancedStage;
    DevelopmentalStage traditionalStage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double age;      // Total developmental age
    
    // Developmental stage parameters
    struct StageParameters {
        double startAge;
        double duration;
        float plasticityBaseline;
        float memoryCapacityMultiplier;
        float learningRateMultiplier;
        float attentionalCapacityMultiplier;
        float predictionAccuracyMultiplier;
        float conceptStabilityMultiplier;
        float curiosityMultiplier;
        float noveltyProcessing;
        float socialLearningRate;
        float stressResilience;
        float rewardSensitivity;
    };
    
    std::unordered_map<EnhancedDevelopmentalStage, StageParameters> stageParameters;
    
    Impl() : enhancedStage(EnhancedDevelopmentalStage::Embryonic)
           , traditionalStage(DevelopmentalStage::Initial)
           , stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), age(0.0) {
        
        // Initialize stage parameters with biologically realistic values
        // Based on developmental neuroscience research on human brain development
        
        // Embryonic stage: rapid growth, high plasticity, no functional systems yet
        stageParameters[EnhancedDevelopmentalStage::Embryonic] = {
            0.0, 60.0,      // start age, duration
            1.0f, 0.1f,     // plasticity, memory capacity
            0.5f, 0.2f,     // learning rate, attentional capacity
            0.1f, 0.2f,     // prediction accuracy, concept stability
            0.0f, 1.0f,     // curiosity, novelty processing
            0.0f, 0.5f,     // social learning, stress resilience
            0.5f            // reward sensitivity
        };
        
        // Neonatal stage: sensory systems online, basic reflexes
        stageParameters[EnhancedDevelopmentalStage::Neonatal] = {
            60.0, 120.0,
            0.9f, 0.2f,
            0.7f, 0.3f,
            0.2f, 0.3f,
            0.5f, 1.5f,
            0.1f, 1.0f,
            1.0f
        };
        
        // Infancy stage: rapid synaptic growth, basic learning
        stageParameters[EnhancedDevelopmentalStage::Infancy] = {
            180.0, 180.0,
            0.8f, 0.3f,
            0.9f, 0.4f,
            0.3f, 0.4f,
            0.4f, 1.8f,
            0.3f, 1.2f,
            1.2f
        };
        
        // Early childhood stage: language acquisition, improved memory
        stageParameters[EnhancedDevelopmentalStage::EarlyChildhood] = {
            360.0, 360.0,
            0.7f, 0.5f,
            0.8f, 0.5f,
            0.4f, 0.6f,
            0.6f, 2.0f,
            0.5f, 1.5f,
            1.5f
        };
        
        // Middle childhood stage: concrete operational thinking
        stageParameters[EnhancedDevelopmentalStage::MiddleChildhood] = {
            720.0, 240.0,
            0.6f, 0.6f,
            0.7f, 0.6f,
            0.5f, 0.7f,
            0.8f, 2.5f,
            0.6f, 1.8f,
            1.8f
        };
        
        // Puberty stage: adolescent brain reorganization, risk taking
        stageParameters[EnhancedDevelopmentalStage::Puberty] = {
            960.0, 480.0,
            0.5f, 0.7f,
            0.6f, 0.7f,
            0.6f, 0.8f,
            0.9f, 3.0f,
            0.8f, 2.0f,
            2.0f
        };
        
        // Late adolescence stage: prefrontal cortex maturation
        stageParameters[EnhancedDevelopmentalStage::LateAdolescence] = {
            1440.0, 720.0,
            0.4f, 0.8f,
            0.5f, 0.8f,
            0.7f, 0.9f,
            1.0f, 3.5f,
            0.9f, 2.5f,
            2.5f
        };
        
        // Young adulthood stage: peak cognitive performance
        stageParameters[EnhancedDevelopmentalStage::YoungAdult] = {
            2160.0, 720.0,
            0.3f, 0.9f,
            0.4f, 0.9f,
            0.8f, 1.0f,
            1.2f, 4.0f,
            1.0f, 2.8f,
            2.8f
        };
        
        // Middle age stage: maintenance, some decline
        stageParameters[EnhancedDevelopmentalStage::MiddleAge] = {
            2880.0, 720.0,
            0.25f, 0.85f,
            0.35f, 0.85f,
            0.75f, 1.0f,
            1.0f, 3.5f,
            0.85f, 2.5f,
            2.5f
        };
        
        // Senior stage: slower processing, wisdom accumulation
        stageParameters[EnhancedDevelopmentalStage::Senior] = {
            3600.0, 720.0,
            0.2f, 0.8f,
            0.3f, 0.8f,
            0.65f, 1.0f,
            0.8f, 3.0f,
            0.7f, 2.2f,
            2.2f
        };
    }
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0), enhancedStage_(EnhancedDevelopmentalStage::Embryonic) {
    updateMetrics();
}

DevelopmentSystem::~DevelopmentSystem() = default;

EnhancedDevelopmentalStage DevelopmentSystem::getEnhancedStage() const {
    return pImpl->enhancedStage;
}

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->traditionalStage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->traditionalStage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stageStartStep = 0;
    updateMetrics();
}

void DevelopmentSystem::advanceStage() {
    // Advance enhanced stage
    switch (pImpl->enhancedStage) {
        case EnhancedDevelopmentalStage::Embryonic:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::Neonatal;
            break;
        case EnhancedDevelopmentalStage::Neonatal:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::Infancy;
            break;
        case EnhancedDevelopmentalStage::Infancy:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::EarlyChildhood;
            break;
        case EnhancedDevelopmentalStage::EarlyChildhood:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::MiddleChildhood;
            break;
        case EnhancedDevelopmentalStage::MiddleChildhood:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::Puberty;
            break;
        case EnhancedDevelopmentalStage::Puberty:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::LateAdolescence;
            break;
        case EnhancedDevelopmentalStage::LateAdolescence:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::YoungAdult;
            break;
        case EnhancedDevelopmentalStage::YoungAdult:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::MiddleAge;
            break;
        case EnhancedDevelopmentalStage::MiddleAge:
            pImpl->enhancedStage = EnhancedDevelopmentalStage::Senior;
            break;
        case EnhancedDevelopmentalStage::Senior:
            // Already at final stage
            break;
        default:
            break;
    }
    
    // Map enhanced stage to traditional stage for compatibility
    switch (pImpl->enhancedStage) {
        case EnhancedDevelopmentalStage::Senior:
        case EnhancedDevelopmentalStage::MiddleAge:
        case EnhancedDevelopmentalStage::YoungAdult:
            pImpl->traditionalStage = DevelopmentalStage::Adult;
            break;
        case EnhancedDevelopmentalStage::LateAdolescence:
        case EnhancedDevelopmentalStage::Puberty:
            pImpl->traditionalStage = DevelopmentalStage::Maturation;
            break;
        case EnhancedDevelopmentalStage::MiddleChildhood:
        case EnhancedDevelopmentalStage::EarlyChildhood:
        case EnhancedDevelopmentalStage::Infancy:
            pImpl->traditionalStage = DevelopmentalStage::CriticalPeriod;
            break;
        default:
            pImpl->traditionalStage = DevelopmentalStage::Initial;
            break;
    }
    
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getEnhancedStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->traditionalStage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

const char* DevelopmentSystem::getEnhancedStageName() const {
    switch (pImpl->enhancedStage) {
        case EnhancedDevelopmentalStage::Embryonic: return "Embryonic";
        case EnhancedDevelopmentalStage::Neonatal: return "Neonatal";
        case EnhancedDevelopmentalStage::Infancy: return "Infancy";
        case EnhancedDevelopmentalStage::EarlyChildhood: return "EarlyChildhood";
        case EnhancedDevelopmentalStage::MiddleChildhood: return "MiddleChildhood";
        case EnhancedDevelopmentalStage::Puberty: return "Puberty";
        case EnhancedDevelopmentalStage::LateAdolescence: return "LateAdolescence";
        case EnhancedDevelopmentalStage::YoungAdult: return "YoungAdult";
        case EnhancedDevelopmentalStage::MiddleAge: return "MiddleAge";
        case EnhancedDevelopmentalStage::Senior: return "Senior";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    pImpl->age = age_;
    
    updateMetrics();
    
    // Auto-advance stage based on developmental age (in simulation seconds)
    if (age_ > 60.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::Embryonic) {
        advanceStage();
    } else if (age_ > 360.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::Neonatal) {
        advanceStage();
    } else if (age_ > 720.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::Infancy) {
        advanceStage();
    } else if (age_ > 960.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::EarlyChildhood) {
        advanceStage();
    } else if (age_ > 1440.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::MiddleChildhood) {
        advanceStage();
    } else if (age_ > 2160.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::Puberty) {
        advanceStage();
    } else if (age_ > 3600.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::LateAdolescence) {
        advanceStage();
    } else if (age_ > 5400.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::YoungAdult) {
        advanceStage();
    } else if (age_ > 7200.0 && pImpl->enhancedStage == EnhancedDevelopmentalStage::MiddleAge) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    pImpl->age = age_;
    
    updateMetrics();
    
    // Auto-advance stage based on developmental age (in simulation seconds)
    if (pImpl->enhancedStage == EnhancedDevelopmentalStage::Embryonic && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::Neonatal && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::Infancy && age_ > 720.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::EarlyChildhood && age_ > 960.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::MiddleChildhood && age_ > 1440.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::Puberty && age_ > 2160.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::LateAdolescence && age_ > 3600.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::YoungAdult && age_ > 5400.0) {
        advanceStage();
    } else if (pImpl->enhancedStage == EnhancedDevelopmentalStage::MiddleAge && age_ > 7200.0) {
        advanceStage();
    }
    
    // Apply developmental effects to brain systems if available
    if (brain) {
        if (brain->getAttention()) {
            applyDevelopmentToAttention(brain->getAttention());
        }
        if (brain->getConceptFormation()) {
            applyDevelopmentToConceptFormation(brain->getConceptFormation());
        }
        if (brain->getWorkingMemory()) {
            applyDevelopmentToWorkingMemory(brain->getWorkingMemory());
        }
        if (brain->getEpisodicMemory()) {
            applyDevelopmentToEpisodicMemory(brain->getEpisodicMemory());
        }
        if (brain->getPredictionSystem()) {
            applyDevelopmentToPredictionSystem(brain->getPredictionSystem());
        }
        if (brain->getDopamine()) {
            applyDevelopmentToNeuromodulation(brain->getDopamine());
        }
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    const auto& params = pImpl->stageParameters.at(pImpl->enhancedStage);
    return params.plasticityBaseline;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->traditionalStage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->traditionalStage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

EnhancedDevelopmentalStage DevelopmentSystem::getStageFromAge(double age) {
    if (age < 60.0) return EnhancedDevelopmentalStage::Embryonic;
    if (age < 180.0) return EnhancedDevelopmentalStage::Neonatal;
    if (age < 360.0) return EnhancedDevelopmentalStage::Infancy;
    if (age < 720.0) return EnhancedDevelopmentalStage::EarlyChildhood;
    if (age < 960.0) return EnhancedDevelopmentalStage::MiddleChildhood;
    if (age < 1440.0) return EnhancedDevelopmentalStage::Puberty;
    if (age < 2160.0) return EnhancedDevelopmentalStage::LateAdolescence;
    if (age < 2880.0) return EnhancedDevelopmentalStage::YoungAdult;
    if (age < 3600.0) return EnhancedDevelopmentalStage::MiddleAge;
    return EnhancedDevelopmentalStage::Senior;
}

float DevelopmentSystem::getAgeMultiplier(EnhancedDevelopmentalStage stage) {
    const auto& params = DevelopmentSystem::Impl().stageParameters.at(stage);
    return params.plasticityBaseline;
}

size_t DevelopmentSystem::getMemoryCapacityForStage(EnhancedDevelopmentalStage stage) {
    const auto& params = DevelopmentSystem::Impl().stageParameters.at(stage);
    // Base capacity of 100 neurons, scaled by multiplier
    return static_cast<size_t>(100 * params.memoryCapacityMultiplier);
}

float DevelopmentSystem::getLearningRateModifier(EnhancedDevelopmentalStage stage) {
    const auto& params = DevelopmentSystem::Impl().stageParameters.at(stage);
    return params.learningRateMultiplier;
}

float DevelopmentSystem::getAttentionalCapacityForStage(EnhancedDevelopmentalStage stage) {
    const auto& params = DevelopmentSystem::Impl().stageParameters.at(stage);
    return params.attentionalCapacityMultiplier;
}

void DevelopmentSystem::updateMetrics() {
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Initialize metrics with stage-specific values
    metrics_.age = age_;
    metrics_.stage = stage;
    
    // Attention development
    metrics_.attentionalCapacity = 2.0f * params.attentionalCapacityMultiplier;  // Base 2 foci
    metrics_.attentionalStability = 0.5f + 0.5f * params.attentionalCapacityMultiplier;  // Improves with age
    metrics_.attentionalPrecision = 0.3f + 0.7f * params.attentionalCapacityMultiplier;  // Better precision with maturity
    
    // Memory development
    metrics_.workingMemoryCapacity = getMemoryCapacityForStage(stage);
    metrics_.episodicMemoryConsolidation = 0.1f + 0.9f * params.memoryCapacityMultiplier;  // Slower consolidation in youth, faster in maturity
    metrics_.memoryRetrievalSpeed = 0.2f + 0.8f * params.memoryCapacityMultiplier;  // Faster retrieval with capacity
    
    // Prediction development
    metrics_.predictionAccuracy = 0.1f + 0.9f * params.predictionAccuracyMultiplier;  // Improves with age
    metrics_.predictionConfidence = 0.2f + 0.8f * params.predictionAccuracyMultiplier;  // More confident with experience
    metrics_.predictionLatency = 0.5f - 0.4f * params.predictionAccuracyMultiplier;  // Faster predictions with maturity
    
    // Concept formation development
    metrics_.conceptStabilityThreshold = 0.3f + 0.7f * params.conceptStabilityMultiplier;  // Stricter threshold early, looser later
    metrics_.conceptGeneralizationAbility = 0.1f + 0.9f * params.conceptStabilityMultiplier;  // Better generalization with age
    metrics_.conceptFormationRate = 0.5f + 0.5f * params.conceptStabilityMultiplier;  // Slower concept formation early
    
    // Learning rate development
    metrics_.learningRate = 0.5f * params.learningRateMultiplier;  // Modulated by stage
    metrics_.exploratoryDrive = 1.0f * params.curiosityMultiplier;  // High in youth, regulated with age
    metrics_.habituationRate = 0.1f + 0.9f * (2.0f - params.curiosityMultiplier);  // Faster habituation with maturity
    
    // Cognitive skill development
    metrics_.planningHorizon = 2.0f + 8.0f * params.learningRateMultiplier;  // Longer planning horizon with maturity
    metrics_.problemSolvingAbility = 0.3f + 0.7f * params.learningRateMultiplier;  // Better problem solving with experience
    metrics_.workingMemoryEfficiency = 0.4f + 0.6f * params.memoryCapacityMultiplier;  // More efficient memory operations
    
    // Social development
    metrics_.socialLearningCapacity = 0.2f + 0.8f * params.socialLearningRate;  // Better social learning with age
    metrics_.theoryOfMind = 0.1f + 0.9f * params.socialLearningRate;  // Understanding others improves with age
    metrics_.empathyLevel = 0.2f + 0.8f * params.socialLearningRate;  // Empathy develops with maturity
    
    // Motivational development
    metrics_.curiosityLevel = 1.5f * params.curiosityMultiplier;  // High curiosity in youth, regulated
    metrics_.noveltySeeking = 1.0f * params.noveltyProcessing;  // Novelty seeking varies by stage
    metrics_.intrinsicMotivation = 0.5f + 0.5f * params.curiosityMultiplier;  // Develops with age
    
    // Sensory development
    metrics_.sensoryReceptorCount = 10.0f * params.memoryCapacityMultiplier;  // More receptors with maturity
    metrics_.sensoryProcessingSpeed = 0.5f + 0.5f * params.memoryCapacityMultiplier;  // Faster processing with age
    metrics_.sensoryDiscrimination = 0.3f + 0.7f * params.memoryCapacityMultiplier;  // Better discrimination with experience
    
    // Neuromodulation development
    metrics_.neuromodulationMaturity = params.stressResilience;  // Maturity level
    metrics_.stressResilience = params.stressResilience;  // Better stress handling with age
    metrics_.rewardSensitivity = params.rewardSensitivity;  // Responsiveness to rewards
}

void DevelopmentSystem::applyDevelopmentToAttention(AttentionalSelection* attention) const {
    if (!attention) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to attentional selection
    // Attention becomes more selective and stable with age
    
    // Adjust attentional resource allocation based on developmental stage
    float attentionalResourceAdjustment = 0.5f + 0.5f * params.attentionalCapacityMultiplier;
    
    // In early stages, attention is more diffuse and novelty-driven
    if (stage == EnhancedDevelopmentalStage::Embryonic || 
        stage == EnhancedDevelopmentalStage::Neonatal ||
        stage == EnhancedDevelopmentalStage::Infancy) {
        // Increase novelty sensitivity in early stages
        // Note: This would modify attention's internal state
        // In a real implementation, this would involve setting neuromodulatory signals
    }
    
    // In later stages, attention becomes more focused and efficient
    if (stage >= EnhancedDevelopmentalStage::LateAdolescence) {
        // Improve attentional stability and reduce distractibility
        // This would involve enhancing neuromodulatory control (e.g., acetylcholine)
    }
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied attentional development: capacity = " + std::to_string(metrics_.attentionalCapacity));
}

void DevelopmentSystem::applyDevelopmentToConceptFormation(ConceptFormation* conceptFormation) const {
    if (!conceptFormation) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to concept formation
    
    // Adjust formation threshold based on developmental stage
    float formationThreshold = 0.5f - 0.3f * params.conceptStabilityMultiplier;
    
    // Younger stages have higher formation thresholds (form concepts more slowly)
    if (stage <= EnhancedDevelopmentalStage::EarlyChildhood) {
        // Be more conservative in forming concepts early
        // Set formation threshold higher to avoid false concepts
    } else {
        // Later stages can form concepts more readily
        // Lower threshold to allow faster concept acquisition
    }
    
    // Adjust stability threshold based on developmental stage
    float stabilityThreshold = 0.6f - 0.4f * params.conceptStabilityMultiplier;
    
    // Concept stability improves with age - older concepts are more stable
    // In early stages, concepts need more observations to be considered stable
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied concept formation development: stability threshold = " + std::to_string(stabilityThreshold));
}

void DevelopmentSystem::applyDevelopmentToWorkingMemory(NeuralWorkingMemory* workingMemory) const {
    if (!workingMemory) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to working memory
    
    // Adjust working memory capacity based on developmental stage
    size_t newCapacity = getMemoryCapacityForStage(stage);
    if (newCapacity != workingMemory->getCapacity()) {
        workingMemory->setCapacity(newCapacity);
        NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                    " increased working memory capacity to " + std::to_string(newCapacity));
    }
    
    // Adjust decay rate based on developmental stage
    float decayRate = 0.01f + 0.02f * (2.0f - params.memoryCapacityMultiplier);
    workingMemory->setDecayRate(decayRate);
    
    // Younger stages have faster decay (less stable memory traces)
    if (stage <= EnhancedDevelopmentalStage::EarlyChildhood) {
        workingMemory->setDecayRate(0.05f);  // Faster decay in early stages
    }
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied working memory development: capacity = " + std::to_string(metrics_.workingMemoryCapacity) +
                ", decay rate = " + std::to_string(workingMemory->getDecayRate()));
}

void DevelopmentSystem::applyDevelopmentToEpisodicMemory(NeuralEpisodicMemory* episodicMemory) const {
    if (!episodicMemory) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to episodic memory
    
    // Adjust consolidation rate based on developmental stage
    float consolidationRate = metrics_.episodicMemoryConsolidation;
    
    // In early stages, consolidation is slower (less efficient memory integration)
    if (stage <= EnhancedDevelopmentalStage::EarlyChildhood) {
        consolidationRate = 0.3f;  // Slower consolidation
    } else if (stage >= EnhancedDevelopmentalStage::LateAdolescence) {
        consolidationRate = 0.8f;  // Faster consolidation in maturity
    }
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied episodic memory development: consolidation rate = " + std::to_string(consolidationRate));
}

void DevelopmentSystem::applyDevelopmentToPredictionSystem(PredictionSystem* predictionSystem) const {
    if (!predictionSystem) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to prediction system
    
    // Note: In a real implementation, this would modify prediction system parameters
    // For now, we log the developmental effects
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied prediction system development: accuracy = " + std::to_string(metrics_.predictionAccuracy) +
                ", confidence = " + std::to_string(metrics_.predictionConfidence) +
                ", latency = " + std::to_string(metrics_.predictionLatency));
}

void DevelopmentSystem::applyDevelopmentToNeuromodulation(Neuromodulator* neuromodulator) const {
    if (!neuromodulator) return;
    
    EnhancedDevelopmentalStage stage = pImpl->enhancedStage;
    const auto& params = pImpl->stageParameters.at(stage);
    
    // Apply age-based modulation to neuromodulatory systems
    
    // Neuromodulation maturity increases with age
    float neuromodulationMaturity = metrics_.neuromodulationMaturity;
    
    // Dopamine system development
    float dopamineMaturity = neuromodulationMaturity;
    
    // Stress response system development
    float stressResilience = metrics_.stressResilience;
    
    // Reward sensitivity development
    float rewardSensitivity = metrics_.rewardSensitivity;
    
    // Log developmental effect
    NLM_LOG_INFO(std::string("DevelopmentStage::") + getEnhancedStageName() +
                " applied neuromodulation development: maturity = " + std::to_string(neuromodulationMaturity) +
                ", stress resilience = " + std::to_string(stressResilience) +
                ", reward sensitivity = " + std::to_string(rewardSensitivity));
}

} // namespace nlm