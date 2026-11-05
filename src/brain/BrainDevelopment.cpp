#include "BrainDevelopment.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void BrainDevelopment::configureFromConfig(const Config* config) {
    if (!config) return;
    
    developmentRate_ = config->getOr<float>("development.rate", 0.01f);
    criticalPeriodThreshold_ = config->getOr<float>("development.critical_period.threshold", 0.5f);
    maturationSpeed_ = config->getOr<float>("development.maturation.speed", 0.1f);
    
    synaptogenesisRate_ = config->getOr<float>("development.synaptogenesis.rate", 0.0001f);
    pruningRate_ = config->getOr<float>("development.pruning.rate", 0.00001f);
    maturationRate_ = config->getOr<float>("development.maturation.rate", 0.001f);
    
    criticalPeriodStart_ = static_cast<size_t>(config->getOr<int64_t>("development.critical_period.start", 1000));
    criticalPeriodEnd_ = static_cast<size_t>(config->getOr<int64_t>("development.critical_period.end", 5000));
    
    // Initialize development systems
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
    synaptogenesis_ = std::make_unique<Synaptogenesis>();
    pruning_ = std::make_unique<Pruning>();
    maturation_ = std::make_unique<Maturation>();
    
    NLM_LOG_INFO("BrainDevelopment configured from config:");
    NLM_LOG_INFO("  Development rate: " + std::to_string(developmentRate_));
    NLM_LOG_INFO("  Critical period threshold: " + std::to_string(criticalPeriodThreshold_));
    NLM_LOG_INFO("  Maturation speed: " + std::to_string(maturationSpeed_));
    NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(synaptogenesisRate_));
    NLM_LOG_INFO("  Pruning rate: " + std::to_string(pruningRate_));
    NLM_LOG_INFO("  Maturation rate: " + std::to_string(maturationRate_));
}

void BrainDevelopment::setDevelopmentParameters(float developmentRate, float criticalPeriodThreshold, float maturationSpeed) {
    developmentRate_ = std::clamp(developmentRate, 0.0f, 0.1f);
    criticalPeriodThreshold_ = std::clamp(criticalPeriodThreshold, 0.0f, 1.0f);
    maturationSpeed_ = std::clamp(maturationSpeed, 0.0f, 0.2f);
}

void BrainDevelopment::updateAllDevelopment(Brain* brain, TimestepDuration timestep, Timestamp currentTime) {
    if (!brain) return;
    
    // Update developmental stage
    updateDevelopmentalStage(brain, currentTime);
    
    // Update development systems based on current stage
    developmentSystem_->update(brain, developmentRate_, timestep);
    
    // Update structural plasticity based on development
    if (currentStage_ == DevelopmentalStage::CriticalPeriod) {
        // Enhanced synaptogenesis during critical period
        synaptogenesis_->setSynaptogenesisRate(synaptogenesisRate_ * 2.0f);
        pruning_->setPruningRate(pruningRate_ * 0.5f);
    } else if (currentStage_ == DevelopmentalStage::Maturation) {
        // Reduced synaptogenesis, increased pruning during maturation
        synaptogenesis_->setSynaptogenesisRate(synaptogenesisRate_ * 0.5f);
        pruning_->setPruningRate(pruningRate_ * 2.0f);
    } else {
        // Normal rates in other stages
        synaptogenesis_->setSynaptogenesisRate(synaptogenesisRate_);
        pruning_->setPruningRate(pruningRate_);
    }
    
    // Update structural plasticity
    synaptogenesis_->update(brain, *brain->getRandomGenerator());
    pruning_->update(brain, *brain->getRandomGenerator());
    
    // Update maturation
    maturation_->update(brain, maturationRate_, currentTime);
    
    // Collect statistics
    collectDevelopmentStats(brain);
}

void BrainDevelopment::updateDevelopmentalStage(Brain* brain, Timestamp currentTime) {
    currentStage_ = calculateDevelopmentalStage(currentTime);
    
    // Update brain developmental stage
    brain->setDevelopmentalStage(currentStage_);
    
    // Adjust plasticity based on developmental stage
    float plasticityModulation = 1.0f;
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            plasticityModulation = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityModulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityModulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityModulation = 0.2f;  // Stable
            break;
        case DevelopmentalStage::Aging:
            plasticityModulation = 0.1f;  // Declining plasticity
            break;
    }
    
    // Update development parameters based on stage
    updateDevelopmentParameters(brain, plasticityModulation);
    
    NLM_LOG_INFO("Developmental stage changed to: " + std::to_string(static_cast<int>(currentStage_)) + 
                 " (plasticity modulation: " + std::to_string(plasticityModulation) + ")");
}

void BrainDevelopment::updateSynaptogenesis(Brain* brain, RandomGenerator& rng) {
    if (!synaptogenesis_) return;
    
    synaptogenesis_->update(brain, rng);
}

void BrainDevelopment::updatePruning(Brain* brain, RandomGenerator& rng) {
    if (!pruning_) return;
    
    pruning_->update(brain, rng);
}

void BrainDevelopment::updateMaturation(Brain* brain, Timestamp currentTime) {
    if (!maturation_) return;
    
    maturation_->update(brain, maturationRate_, currentTime);
}

void BrainDevelopment::resetDevelopment(Brain* brain) {
    if (developmentSystem_) developmentSystem_->reset();
    if (synaptogenesis_) synaptogenesis_->reset();
    if (pruning_) pruning_->reset();
    if (maturation_) maturation_->reset();
    
    currentStage_ = DevelopmentalStage::Initial;
}

void BrainDevelopment::enableDevelopment(Brain* brain, bool enable) {
    if (enable) {
        developmentSystem_->enable(true);
        synaptogenesis_->enable(true);
        pruning_->enable(true);
        maturation_->enable(true);
    } else {
        developmentSystem_->enable(false);
        synaptogenesis_->enable(false);
        pruning_->enable(false);
        maturation_->enable(false);
    }
}

void BrainDevelopment::setDevelopmentalStage(Brain* brain, DevelopmentalStage stage) {
    currentStage_ = stage;
    brain->setDevelopmentalStage(stage);
    
    NLM_LOG_INFO("Developmental stage manually set to: " + std::to_string(static_cast<int>(stage)));
}

void BrainDevelopment::updateDevelopmentParameters(Brain* brain, float plasticityModulation) {
    if (!brain) return;
    
    // Get structural plasticity system
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        sp->setSynaptogenesisRate(synaptogenesisRate_ * plasticityModulation);
        sp->setPruningRate(pruningRate_ * (2.0f - plasticityModulation));
    }
}

void BrainDevelopment::processDevelopmentalMaturation(Brain* brain, Timestamp currentTime) {
    if (!maturation_) return;
    
    maturation_->processMaturationEffects(brain, currentTime);
}

void BrainDevelopment::processCriticalPeriod(Brain* brain) {
    // Process critical period effects
    // This would include enhanced learning and synaptic strengthening
    if (currentStage_ == DevelopmentalStage::CriticalPeriod) {
        NLM_LOG_INFO("Critical period effects active - enhanced learning and plasticity");
        
        // This would typically include:
        // - Enhanced Hebbian learning
        // - Increased synaptic competition
        // - Accelerated concept formation
        // - Optimized attention mechanisms
    }
}

void BrainDevelopment::processSynapticCompetition(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Process synaptic competition for limited resources
    // This would involve:
    // - Pruning weak synapses
    // - Strengthening strong synapses
    // - Establishing competitive balance
    
    // For now, this is handled by the development system
}

void BrainDevelopment::processExperienceIndependentDevelopment(Brain* brain, Timestamp currentTime) {
    if (!brain) return;
    
    // Process development that occurs independently of experience
    // This includes:
    // - Intrinsic maturation programs
    // - Genetic developmental programs
    // - Innate developmental trajectories
    
    // This would typically be handled by the development system
}

void BrainDevelopment::initializeDevelopmentSystems(Brain* brain) {
    if (!brain) return;
    
    // Establish connections between development systems
    if (developmentSystem_ && synaptogenesis_) {
        developmentSystem_->setSynaptogenesisReference(synaptogenesis_.get());
    }
    
    if (synaptogenesis_ && pruning_) {
        synaptogenesis_->setPruningReference(pruning_.get());
    }
    
    if (pruning_ && maturation_) {
        pruning_->setMaturationReference(maturation_.get());
    }
    
    if (maturation_ && developmentSystem_) {
        maturation_->setDevelopmentSystemReference(developmentSystem_.get());
    }
}

void BrainDevelopment::validateDevelopmentState(Brain* brain) const {
    // Validate development system state
    if (developmentSystem_ && !developmentSystem_->isEnabled()) {
        NLM_LOG_WARNING("Development system is disabled");
    }
    
    if (synaptogenesis_ && !synaptogenesis_->isEnabled()) {
        NLM_LOG_WARNING("Synaptogenesis system is disabled");
    }
    
    if (pruning_ && !pruning_->isEnabled()) {
        NLM_LOG_WARNING("Pruning system is disabled");
    }
    
    if (maturation_ && !maturation_->isEnabled()) {
        NLM_LOG_WARNING("Maturation system is disabled");
    }
}

void BrainDevelopment::updateDevelopmentConnections(Brain* brain) {
    if (!brain) return;
    
    // Update connections between development systems
    // This would maintain consistency across developmental processes
}

void BrainDevelopment::processDevelopmentLearning(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Process learning in development systems
    // This would adapt developmental parameters based on experience
}

DevelopmentalStage BrainDevelopment::calculateDevelopmentalStage(Timestamp currentTime) const {
    size_t currentStep = static_cast<size_t>(currentTime / 0.001f);  // Assuming 1ms timestep
    
    if (currentStep < criticalPeriodStart_) {
        return DevelopmentalStage::Initial;
    } else if (currentStep < criticalPeriodEnd_) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (currentStep < criticalPeriodEnd_ + 10000) {
        return DevelopmentalStage::Maturation;
    } else if (currentStep < criticalPeriodEnd_ + 20000) {
        return DevelopmentalStage::Adult;
    } else {
        return DevelopmentalStage::Aging;
    }
}

} // namespace nlm