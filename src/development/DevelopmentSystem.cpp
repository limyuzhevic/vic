#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double developmentalAge;
    
    // Development mechanisms
    Synaptogenesis synaptogenesis;
    Pruning pruning;
    Maturation maturation;
    
    // Neuromodulation integration
    Dopamine* dopamine;
    Reward* reward;
    Curiosity* curiosity;
    Novelty* novelty;
    
    // Memory integration
    NeuralEpisodicMemory* episodicMemory;
    NeuralWorkingMemory* workingMemory;
    
    // Plasticity integration
    STDP* stdp;
    Hebbian* hebbian;
    StructuralPlasticity* structuralPlasticity;
    
    // Developmental parameters
    float targetExcitationInhibitionRatio;
    float homeostaticSetpoint;
    float targetSynapseDensity;
    float competitionFactor;
    
    // Molecular mechanisms
    std::vector<float> stabilizationFactors;
    std::vector<float> decayRates;
    std::vector<float> matureExcitability;
    std::vector<float> agingFactors;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), 
             stepsInCurrentStage(0), stageAge(0.0), developmentalAge(0.0),
             dopamine(nullptr), reward(nullptr), curiosity(nullptr), novelty(nullptr),
             episodicMemory(nullptr), workingMemory(nullptr), stdp(nullptr),
             hebbian(nullptr), structuralPlasticity(nullptr),
             targetExcitationInhibitionRatio(1.0f), homeostaticSetpoint(1.0f),
             targetSynapseDensity(0.1f), competitionFactor(0.5f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {
    // Initialize developmental molecular mechanisms
    pImpl->stabilizationFactors.resize(1000, 1.0f);  // Initialize stabilization factors
    pImpl->decayRates.resize(1000, 0.001f);         // Initialize decay rates
    pImpl->matureExcitability.resize(1000, 1.0f);   // Initialize mature excitability
    pImpl->agingFactors.resize(1000, 0.0f);        // Initialize aging factors
}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stageStartStep = 0; // Would need access to current step
    
    NLM_LOG_INFO("DevelopmentSystem: Advanced to stage " + std::string(getStageName()));
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
    pImpl->developmentalAge += 0.001;
    
    // Update development mechanisms
    RandomGenerator& rng = *brain->getRandomGenerator();
    pImpl->synaptogenesis.update(brain, rng, age_);
    pImpl->pruning.update(brain, rng, age_);
    pImpl->maturation.update(brain, currentStep);
    
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
    pImpl->developmentalAge += dt;
    
    // Update development mechanisms
    pImpl->synaptogenesis.update(brain, rng, dt);
    pImpl->pruning.update(brain, rng, dt);
    pImpl->maturation.update(brain, static_cast<SimulationStep>(age_));
    
    // Apply developmental stage effects
    applyStageEffects(brain, rng, dt);
    
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

void DevelopmentSystem::applyStageEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Get current stage parameters
    float plasticityModifier = getPlasticityModifier();
    bool isCritical = isCriticalPeriod();
    
    // Apply stage-specific effects to plasticity systems
    if (pImpl->stdp) {
        pImpl->stdp->setLearningRate(pImpl->stdp->getLearningRate() * plasticityModifier);
        pImpl->stdp->configure(
            pImpl->stdp->getLTPWeight() * plasticityModifier,
            pImpl->stdp->getLTDWeight() * plasticityModifier,
            pImpl->stdp->getTimeConstant() * (1.0f - plasticityModifier)
        );
    }
    
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(pImpl->hebbian->getLearningRate() * plasticityModifier);
        pImpl->hebbian->setMaxWeight(pImpl->hebbian->getMaxWeight() * plasticityModifier);
    }
    
    if (pImpl->structuralPlasticity) {
        // Adjust structural plasticity rates based on development stage
        if (isCritical) {
            // Critical period: high structural plasticity for rapid connection formation
            pImpl->structuralPlasticity->setSynaptogenesisRate(0.01f * plasticityModifier);
            pImpl->structuralPlasticity->setPruningRate(0.001f * (2.0f - plasticityModifier));
        } else if (pImpl->stage == DevelopmentalStage::Maturation) {
            // Maturation: balanced formation and elimination
            pImpl->structuralPlasticity->setSynaptogenesisRate(0.001f * plasticityModifier);
            pImpl->structuralPlasticity->setPruningRate(0.0001f * (2.0f - plasticityModifier));
        } else if (pImpl->stage == DevelopmentalStage::Adult) {
            // Adult: low structural plasticity for maintenance
            pImpl->structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityModifier);
            pImpl->structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityModifier));
        } else {
            // Initial and Aging: moderate structural changes
            pImpl->structuralPlasticity->setSynaptogenesisRate(0.0005f * plasticityModifier);
            pImpl->structuralPlasticity->setPruningRate(0.00005f * (2.0f - plasticityModifier));
        }
    }
    
    // Apply neuromodulation effects on development
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Dopamine enhances synaptogenesis during critical period
        if (isCritical) {
            pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                                   (1.0f + dopamineLevel * 0.5f));
        }
        
        // Dopamine modulates pruning
        pImpl->pruning.setPruningRate(pImpl->pruning.getPruningRate() * 
                                      (1.0f + dopamineLevel * 0.3f));
        
        // Dopamine affects maturation progress
        if (pImpl->stage == DevelopmentalStage::Maturation) {
            pImpl->maturation.setProgress(pImpl->maturation.getProgress() + 
                                         dopamineLevel * 0.001f * dt);
        }
    }
    
    if (pImpl->reward) {
        float rewardValue = pImpl->reward->getValue();
        
        // Reward signals enhance synaptogenesis through novelty
        pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                               (1.0f + rewardValue * 0.2f));
        
        // Reward reduces pruning (conservation of successful connections)
        pImpl->pruning.setPruningRate(pImpl->pruning.getPruningRate() * 
                                      std::max(0.1f, 1.0f - rewardValue * 0.1f));
    }
    
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        
        // Curiosity drives exploration and synaptogenesis
        pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                               (1.0f + curiosityLevel * 0.3f));
    }
    
    if (pImpl->novelty) {
        float noveltyLevel = pImpl->novelty->getLevel();
        
        // Novelty enhances synaptogenesis and plasticity
        pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                               (1.0f + noveltyLevel * 0.4f));
    }
    
    // Apply memory-based development influences
    if (pImpl->episodicMemory && pImpl->workingMemory) {
        // Integrate with episodic memory for experience-dependent development
        integrateWithEpisodicMemory(brain, rng, dt);
        
        // Integrate with working memory for active experience processing
        integrateWithWorkingMemory(brain, rng, dt);
    }
}

void DevelopmentSystem::integrateWithEpisodicMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Get recent episodes for development influences
    auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
    
    for (const auto* episode : recentEpisodes) {
        // Calculate episode relevance based on reward, novelty, and prediction error
        float episodeRelevance = episode->reward * 0.4f + 
                                episode->novelty * 0.3f + 
                                std::abs(episode->reward - 0.5f) * 0.3f;  // prediction error proxy
        
        // Stronger episodes have greater influence on development
        if (episodeRelevance > 0.5f) {
            // Enhance synaptogenesis for high-relevance episodes
            pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                                   (1.0f + episodeRelevance * 0.2f));
            
            // Promote retention of episode-related connections
            pImpl->pruning.setPruningRate(pImpl->pruning.getPruningRate() * 
                                          std::max(0.2f, 1.0f - episodeRelevance * 0.3f));
        }
        
        // Replay episode neural patterns to strengthen connections
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void DevelopmentSystem::integrateWithWorkingMemory(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Get current working memory content
    auto workingMemoryContent = pImpl->workingMemory->getContent();
    
    // Working memory activation patterns influence development
    float workingMemoryActivation = 0.0f;
    for (const auto& item : workingMemoryContent) {
        workingMemoryActivation += item.activation;
    }
    workingMemoryActivation /= std::max(1.0f, static_cast<float>(workingMemoryContent.size()));
    
    // High working memory activation drives synaptogenesis
    pImpl->synaptogenesis.setFormationRate(pImpl->synaptogenesis.getFormationRate() * 
                                           (1.0f + workingMemoryActivation * 0.3f));
    
    // Working memory maintenance reduces pruning
    pImpl->pruning.setPruningRate(pImpl->pruning.getPruningRate() * 
                                  std::max(0.3f, 1.0f - workingMemoryActivation * 0.4f));
}

void DevelopmentSystem::initializeDevelopment(Brain* brain) {
    // Initialize development system with brain references
    pImpl->dopamine = brain->getDopamine();
    pImpl->reward = brain->getReward();
    pImpl->curiosity = brain->getCuriosity();
    pImpl->novelty = brain->getNovelty();
    pImpl->episodicMemory = brain->getAdvancedEpisodicMemory();
    pImpl->workingMemory = brain->getAdvancedWorkingMemory();
    pImpl->stdp = brain->getSTDP();
    pImpl->hebbian = brain->getHebbian();
    pImpl->structuralPlasticity = brain->getStructuralPlasticity();
    
    // Initialize development parameters based on brain configuration
    auto config = brain->getConfig();
    if (config) {
        if (config->has("target_e_i_ratio")) {
            pImpl->targetExcitationInhibitionRatio = 
                static_cast<float>(config->getDouble("target_e_i_ratio"));
        }
        if (config->has("homeostatic_setpoint")) {
            pImpl->homeostaticSetpoint = 
                static_cast<float>(config->getDouble("homeostatic_setpoint"));
        }
        if (config->has("target_synapse_density")) {
            pImpl->targetSynapseDensity = 
                static_cast<float>(config->getDouble("target_synapse_density"));
        }
    }
    
    NLM_LOG_INFO("DevelopmentSystem initialized with stage: " + std::string(getStageName()));
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

DevelopmentSystem* DevelopmentSystem::getDevelopmentSystem() {
    return nullptr; // Would need to be implemented properly
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setDevelopmentalStage(DevelopmentalStage stage) {
    setStage(stage);
}

} // namespace nlm