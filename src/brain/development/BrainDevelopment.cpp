#include "brain/development/BrainDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>

namespace nlm {

BrainDevelopment::BrainDevelopment(std::shared_ptr<Config> config)
    : config_(config)
    , currentStage_(DevelopmentalStage::Initial)
    , developmentalAge_(0.0)
    , criticalPeriodEnd_(60.0)  // 60.0 seconds (approximately 1 minute)
    , maturationEnd_(300.0)  // 300.0 seconds (approximately 5 minutes)
    , synaptogenesisRate_(0.0001f)
    , pruningRate_(0.00001f)
    , developmentEvents_(0)
    , plasticityMaturation_(0.0f)
    , developmentStartTime_(std::chrono::steady_clock::now())
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainDevelopment constructor");
    }
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config_->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng_ = std::make_unique<RandomGenerator>(seed);
    
    // Initialize developmental milestones
    initializeDevelopmentalMilestones();
    
    NLM_LOG_INFO("BrainDevelopment initialized");
}

BrainDevelopment::~BrainDevelopment() = default;

bool BrainDevelopment::initialize() {
    NLM_LOG_INFO("Initializing brain development system...");
    
    // Initialize development system
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
    developmentSystem_->initialize(nullptr);  // Would pass brain reference
    
    // Initialize structural plasticity
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
    
    // Configure structural plasticity rates
    structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate_);
    structuralPlasticity_->setPruningRate(pruningRate_);
    
    // Initialize dopamine for reward-based development
    dopamine_ = std::make_unique<Dopamine>();
    dopamine_->initialize(nullptr);  // Would pass brain reference
    
    // Validate configuration
    if (!validateDevelopmentConfig(config_)) {
        NLM_LOG_ERROR("Development system configuration validation failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain development system initialized");
    return true;
}

void BrainDevelopment::reset() {
    NLM_LOG_INFO("Resetting brain development system...");
    
    developmentalAge_ = 0.0;
    currentStage_ = DevelopmentalStage::Initial;
    developmentEvents_ = 0;
    plasticityMaturation_ = 0.0f;
    
    // Reset developmental systems
    if (developmentSystem_) {
        // Reset development system if available
    }
    
    if (structuralPlasticity_) {
        structuralPlasticity_->reset();
    }
    
    // Reset dopamine
    if (dopamine_) {
        dopamine_->setLevel(0.0f);
    }
    
    // Reset developmental milestones
    for (auto& milestone : developmentalMilestones_) {
        milestone.achieved = false;
    }
    
    NLM_LOG_INFO("Brain development system reset");
}

void BrainDevelopment::updateDevelopment(double timestep) {
    NLM_LOG_INFO("Updating brain development system...");
    
    // Update developmental age
    developmentalAge_ += timestep;
    
    // Update developmental stage based on age
    updateDevelopmentalStage();
    
    // Apply development effects
    applyDevelopmentEffects(timestep);
    
    // Update structural plasticity
    updateStructuralPlasticity(timestep);
    
    // Update plasticity rates based on development
    updatePlasticityRates(timestep);
    
    // Check developmental milestones
    checkDevelopmentalMilestones(timestep);
    
    developmentEvents_++;
}

void BrainDevelopment::applyDevelopmentEffects(double timestep) {
    if (!developmentSystem_) {
        return;
    }
    
    // Apply developmental system effects
    developmentSystem_->update(nullptr, *rng_, timestep);  // Would pass brain reference
    
    // Update dopamine level based on developmental stage
    float dopamineLevel = calculateDopamineLevel();
    if (dopamine_) {
        dopamine_->setLevel(dopamineLevel);
    }
    
    // Apply maturation effects on neural properties
    plasticityMaturation_ = calculatePlasticityMaturation();
}

void BrainDevelopment::updateStructuralPlasticity(double timestep) {
    if (!structuralPlasticity_) {
        return;
    }
    
    // Update structural plasticity based on development
    structuralPlasticity_->update(nullptr, *rng_);  // Would pass brain reference
    
    // Adjust plasticity rates based on developmental stage
    float stageModulation = getStageModulationFactor(currentStage_);
    
    float adjustedSynaptogenesisRate = synaptogenesisRate_ * stageModulation;
    float adjustedPruningRate = pruningRate_ * (2.0f - stageModulation);
    
    structuralPlasticity_->setSynaptogenesisRate(adjustedSynaptogenesisRate);
    structuralPlasticity_->setPruningRate(adjustedPruningRate);
}

void BrainDevelopment::updatePlasticityRates(double timestep) {
    // Update plasticity rates based on developmental stage
    float stageModulation = getStageModulationFactor(currentStage_);
    
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            synaptogenesisRate_ = 0.0001f * stageModulation;
            pruningRate_ = 0.00001f * (2.0f - stageModulation);
            break;
        case DevelopmentalStage::CriticalPeriod:
            synaptogenesisRate_ = 0.00008f * stageModulation;
            pruningRate_ = 0.00002f * (2.0f - stageModulation);
            break;
        case DevelopmentalStage::Maturation:
            synaptogenesisRate_ = 0.00005f * stageModulation;
            pruningRate_ = 0.00003f * (2.0f - stageModulation);
            break;
        case DevelopmentalStage::Adult:
            synaptogenesisRate_ = 0.00002f * stageModulation;
            pruningRate_ = 0.00004f * (2.0f - stageModulation);
            break;
    }
    
    // Update structural plasticity with new rates
    if (structuralPlasticity_) {
        structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate_);
        structuralPlasticity_->setPruningRate(pruningRate_);
    }
}

float BrainDevelopment::calculateDopamineLevel() const {
    // Calculate dopamine level based on developmental stage
    // This is a simplified model
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            return 0.0f;  // Low reward prediction
        case DevelopmentalStage::CriticalPeriod:
            return 0.3f;  // Moderate exploration
        case DevelopmentalStage::Maturation:
            return 0.5f;  // Balanced exploration/exploitation
        case DevelopmentalStage::Adult:
            return 0.2f;  // Low novelty seeking
        default:
            return 0.0f;
    }
}

float BrainDevelopment::calculatePlasticityMaturation() const {
    // Calculate plasticity maturation based on developmental age
    float ageRatio = std::clamp(static_cast<float>(developmentalAge_) / maturationEnd_, 0.0f, 1.0f);
    
    // Plasticity decreases with maturation
    return 1.0f - (ageRatio * 0.8f);
}

float BrainDevelopment::getStageModulationFactor(DevelopmentalStage stage) const {
    // Get modulation factor for each developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.2f;  // High plasticity
        case DevelopmentalStage::CriticalPeriod:
            return 1.0f;  // Moderate plasticity
        case DevelopmentalStage::Maturation:
            return 0.8f;  // Decreasing plasticity
        case DevelopmentalStage::Adult:
            return 0.5f;  // Low plasticity
        default:
            return 1.0f;
    }
}

void BrainDevelopment::updateDevelopmentalStage() {
    DevelopmentalStage oldStage = currentStage_;
    
    if (developmentalAge_ < 60.0) {
        currentStage_ = DevelopmentalStage::Initial;
    } else if (developmentalAge_ < 300.0) {
        currentStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < 900.0) {
        currentStage_ = DevelopmentalStage::Maturation;
    } else {
        currentStage_ = DevelopmentalStage::Adult;
    }
    
    // Log stage change
    if (oldStage != currentStage_) {
        NLM_LOG_INFO("Developmental stage changed from " + std::to_string(static_cast<int>(oldStage)) + 
                     " to " + std::to_string(static_cast<int>(currentStage_)) +
                     " at age " + std::to_string(developmentalAge_) + " seconds");
    }
}

void BrainDevelopment::checkDevelopmentalMilestones(double timestep) {
    for (auto& milestone : developmentalMilestones_) {
        if (!milestone.achieved) {
            bool achieved = checkMilestoneCondition(milestone.stage, timestep);
            if (achieved) {
                milestone.achieved = true;
                NLM_LOG_INFO("Developmental milestone achieved: " + milestone.description);
            }
        }
    }
}

bool BrainDevelopment::checkMilestoneCondition(DevelopmentalStage stage, double timestep) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return developmentalAge_ >= 10.0;  // Initial stage ends at 10 seconds
        case DevelopmentalStage::CriticalPeriod:
            return developmentalAge_ >= 300.0;  // Critical period ends at 5 minutes
        case DevelopmentalStage::Maturation:
            return developmentalAge_ >= 900.0;  // Maturation ends at 15 minutes
        case DevelopmentalStage::Adult:\n            return developmentalAge_ >= 1800.0;  // Adult stage at 30 minutes
        default:
            return true;
    }
}

void BrainDevelopment::initializeDevelopmentalMilestones() {
    developmentalMilestones_ = {
        {DevelopmentalStage::Initial, 10.0, "Initial neural network establishment", false},
        {DevelopmentalStage::CriticalPeriod, 300.0, "Critical period for sensory refinement", false},
        {DevelopmentalStage::Maturation, 900.0, "Concept formation and abstract reasoning", false},
        {DevelopmentalStage::Adult, 1800.0, "Stable adult cognitive architecture", false}
    };
}

void BrainDevelopment::triggerDevelopmentEvent(const std::string& eventType, double timestep) {
    developmentEvents_++;
    
    NLM_LOG_INFO("Development event triggered: " + eventType + " at age " + std::to_string(developmentalAge_));
    
    // Handle specific event types
    if (eventType == "synaptogenesis") {
        // Trigger synaptogenesis
        if (structuralPlasticity_) {
            structuralPlasticity_->triggerSynaptogenesis();
        }
    } else if (eventType == "pruning") {
        // Trigger pruning
        if (structuralPlasticity_) {
            structuralPlasticity_->triggerPruning();
        }
    } else if (eventType == "maturation") {
        // Trigger maturation
        if (developmentSystem_) {
            // Trigger maturation effects
        }
    }
}

bool BrainDevelopment::validateDevelopmentConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate development-related configuration
    auto criticalPeriodDuration = config->getOr<double>("critical_period_duration", 60.0);
    if (criticalPeriodDuration <= 0.0 || criticalPeriodDuration > 3600.0) {
        NLM_LOG_ERROR("Invalid critical_period_duration: " + std::to_string(criticalPeriodDuration));
        return false;
    }
    
    auto maturationDuration = config->getOr<double>("maturation_duration", 240.0);
    if (maturationDuration <= 0.0 || maturationDuration > 7200.0) {
        NLM_LOG_ERROR("Invalid maturation_duration: " + std::to_string(maturationDuration));
        return false;
    }
    
    auto synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.001f) {
        NLM_LOG_ERROR("Invalid synaptogenesis_rate: " + std::to_string(synaptogenesisRate));
        return false;
    }
    
    return true;
}

void BrainDevelopment::applyDefaultDevelopmentSettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default development settings
    config->set("critical_period_duration", 60.0);
    config->set("maturation_duration", 240.0);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("development_enabled", true);
}

} // namespace nlm
