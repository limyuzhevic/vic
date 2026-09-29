#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Thread-local storage for environment factors
static thread_local struct EnvironmentFactors {
    double temperature = 20.0;          // Temperature (Celsius)
    double nutrition = 1.0;              // Nutrition level (0-1)
    double socialSupport = 0.0;         // Social support (0-1)
    double stress = 0.0;                // Stress level (0-1)
    double enrichment = 0.0;            // Environmental enrichment (0-1)
    double safety = 0.0;                // Safety level (0-1)
} environment;

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double age;       // Overall developmental age
    
    // Environmental state
    double temperature;
    double nutrition;
    double socialSupport;
    double stress;
    double enrichment;
    double safety;
    
    // Individual variation
    double geneticBaseAge;
    double ageNoise;
    double environmentalSensitivity;
    
    // Experience tracking
    std::map<std::string, double> experienceLevels;
    std::map<std::string, double> experienceHistory;
    
    // Adaptive plasticity
    std::map<DevelopmentalStage, float> plasticityHistory;
    float currentPlasticityLearningRate;
    
    // Disorders and impairments
    std::string disorderType;
    float disorderSeverity;
    bool hasDisorder;
    
    // Research mode
    bool researchMode;
    std::map<std::string, double> researchParameters;
    
    // Development statistics
    std::map<std::string, double> developmentMetrics;
    std::vector<std::string> completedMilestones;
    
    Impl() : stage(DevelopmentalStage::Embryonic), stageStartStep(0), stepsInCurrentStage(0), 
             stageAge(0.0), age(0.0), temperature(20.0), nutrition(1.0), socialSupport(0.0),
             stress(0.0), enrichment(0.0), safety(0.0), geneticBaseAge(0.0), ageNoise(0.0),
             environmentalSensitivity(1.0), currentPlasticityLearningRate(0.01f),
             disorderType(""), disorderSeverity(0.0f), hasDisorder(false),
             researchMode(false) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(std::make_unique<Impl>()), age_(0.0) {
    // Initialize configuration with defaults
    config_.stageDurations = {
        {DevelopmentalStage::Embryonic, 30.0},
        {DevelopmentalStage::Larval, 120.0},
        {DevelopmentalStage::Juvenile, 600.0},
        {DevelopmentalStage::Adolescent, 900.0},
        {DevelopmentalStage::YoungAdult, 300.0},
        {DevelopmentalStage::PrimeAdult, 400.0},
        {DevelopmentalStage::MatureAdult, 500.0},
        {DevelopmentalStage::Elderly, 600.0}
    };
    
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO("DevelopmentSystem initialized with enhanced configuration");
    }
}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO(std::string("Development stage changed from ") + std::string(getStageName()) + 
                    " to " + std::string(getStageName((stage))));
    }
    
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stepsInCurrentStage = 0;
    
    // Apply stage-specific effects
    updatePlasticityModifiers();
}

void DevelopmentSystem::advanceStage() {
    std::vector<DevelopmentalStage> stageOrder = {
        DevelopmentalStage::Embryonic, DevelopmentalStage::Larval, 
        DevelopmentalStage::Juvenile, DevelopmentalStage::Adolescent,
        DevelopmentalStage::YoungAdult, DevelopmentalStage::PrimeAdult,
        DevelopmentalStage::MatureAdult, DevelopmentalStage::Elderly,
        DevelopmentalStage::CriticalPeriod, DevelopmentalStage::SensitivePeriod,
        DevelopmentalStage::Adaptation, DevelopmentalStage::Recovery
    };
    
    auto it = std::find(stageOrder.begin(), stageOrder.end(), pImpl->stage);
    if (it != stageOrder.end() && std::next(it) != stageOrder.end()) {
        DevelopmentalStage nextStage = *(std::next(it));
        
        // Check if next stage is enabled
        if (config_.stageDurations.find(nextStage) != config_.stageDurations.end()) {
            setStage(nextStage);
            NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName(pImpl->stage)));
        }
    }
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Embryonic: return "Embryonic";
        case DevelopmentalStage::Larval: return "Larval";
        case DevelopmentalStage::Juvenile: return "Juvenile";
        case DevelopmentalStage::Adolescent: return "Adolescent";
        case DevelopmentalStage::YoungAdult: return "YoungAdult";
        case DevelopmentalStage::PrimeAdult: return "PrimeAdult";
        case DevelopmentalStage::MatureAdult: return "MatureAdult";
        case DevelopmentalStage::Elderly: return "Elderly";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::SensitivePeriod: return "SensitivePeriod";
        case DevelopmentalStage::Adaptation: return "Adaptation";
        case DevelopmentalStage::Recovery: return "Recovery";
        case DevelopmentalStage::ExperimentalStage1: return "ExperimentalStage1";
        case DevelopmentalStage::ExperimentalStage2: return "ExperimentalStage2";
        case DevelopmentalStage::ExperimentalStage3: return "ExperimentalStage3";
        default: return "Unknown";
    }
}

const char* DevelopmentSystem::getStageName(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Embryonic: return "Embryonic";
        case DevelopmentalStage::Larval: return "Larval";
        case DevelopmentalStage::Juvenile: return "Juvenile";
        case DevelopmentalStage::Adolescent: return "Adolescent";
        case DevelopmentalStage::YoungAdult: return "YoungAdult";
        case DevelopmentalStage::PrimeAdult: return "PrimeAdult";
        case DevelopmentalStage::MatureAdult: return "MatureAdult";
        case DevelopmentalStage::Elderly: return "Elderly";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::SensitivePeriod: return "SensitivePeriod";
        case DevelopmentalStage::Adaptation: return "Adaptation";
        case DevelopmentalStage::Recovery: return "Recovery";
        case DevelopmentalStage::ExperimentalStage1: return "ExperimentalStage1";
        case DevelopmentalStage::ExperimentalStage2: return "ExperimentalStage2";
        case DevelopmentalStage::ExperimentalStage3: return "ExperimentalStage3";
        default: return "Unknown";
    }
}

void DevelopmentSystem::updatePlasticityModifiers() {
    auto it = config_.plasticityRates.find(pImpl->stage);
    if (it != config_.plasticityRates.end()) {
        pImpl->plasticityHistory[pImpl->stage] = it->second;
    }
}

void DevelopmentSystem::advanceToNextAppropriateStage() {
    if (pImpl->stage == DevelopmentalStage::ExperimentalStage1 && 
        pImpl->stageAge > config_.stageDurations[DevelopmentalStage::ExperimentalStage1]) {
        setStage(DevelopmentalStage::ExperimentalStage2);
    } else if (pImpl->stage == DevelopmentalStage::ExperimentalStage2 && 
               pImpl->stageAge > config_.stageDurations[DevelopmentalStage::ExperimentalStage2]) {
        setStage(DevelopmentalStage::ExperimentalStage3);
    } else if (pImpl->stage == DevelopmentalStage::ExperimentalStage3 && 
               pImpl->stageAge > config_.stageDurations[DevelopmentalStage::ExperimentalStage3]) {
        // Return to normal stages
        setStage(DevelopmentalStage::PrimeAdult);
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    pImpl->stepsInCurrentStage++;
    
    // Apply environmental influences
    applyEnvironmentalInfluence(0.001);
    
    // Apply individual variation
    applyIndividualVariation(0.001);
    
    // Record experience if enabled
    if (config_.enableExperienceDependence) {
        applyExperienceDependence(0.001);
    }
    
    // Apply adaptive plasticity
    if (config_.enableAdaptivePlasticity) {
        updateAdaptivePlasticity(0.001);
    }
    
    // Apply developmental disorders if present
    if (pImpl->hasDisorder) {
        applyDevelopmentalDisorders(0.001);
    }
    
    // Handle stage transitions
    handleStageTransitions();
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    pImpl->stepsInCurrentStage++;
    
    // Apply environmental influences
    applyEnvironmentalInfluence(dt);
    
    // Apply individual variation
    applyIndividualVariation(dt);
    
    // Record experience if enabled
    if (config_.enableExperienceDependence) {
        applyExperienceDependence(dt);
    }
    
    // Apply adaptive plasticity
    if (config_.enableAdaptivePlasticity) {
        updateAdaptivePlasticity(dt);
    }
    
    // Apply developmental disorders if present
    if (pImpl->hasDisorder) {
        applyDevelopmentalDisorders(dt);
    }
    
    // Handle stage transitions
    handleStageTransitions();
}

void DevelopmentSystem::applyEnvironmentalInfluence(double timestep) {
    if (!config_.enableEnvironmentalInfluence) return;
    
    // Apply temperature effects
    if (config_.enableTemperatureInfluence) {
        double temperatureEffect = std::abs(pImpl->temperature - 20.0) / 20.0;  // Optimal at 20C
        pImpl->stageAge *= (1.0 + temperatureEffect * 0.1);  // Slower development in extreme temps
    }
    
    // Apply nutrition effects
    if (config_.enableNutritionalInfluence) {
        double nutritionMultiplier = 1.0 + (pImpl->nutrition - 0.5) * 0.2;  // Better nutrition speeds development
        pImpl->stageAge *= nutritionMultiplier;
    }
    
    // Apply social support effects
    if (config_.enableSocialInfluence) {
        double socialMultiplier = 1.0 + pImpl->socialSupport * 0.15;  // Social support accelerates development
        pImpl->stageAge *= socialMultiplier;
    }
    
    // Apply stress effects
    if (config_.enableStressInfluence) {
        double stressMultiplier = 1.0 - pImpl->stress * 0.2;  // Stress slows development
        pImpl->stageAge *= stressMultiplier;
    }
    
    // Apply enrichment effects
    double enrichmentMultiplier = 1.0 + pImpl->enrichment * 0.1;  // Enrichment accelerates development
    pImpl->stageAge *= enrichmentMultiplier;
    
    // Apply safety effects
    double safetyMultiplier = 1.0 + pImpl->safety * 0.05;  // Safety provides better development conditions
    pImpl->stageAge *= safetyMultiplier;
}

void DevelopmentSystem::applyNutritionalInfluence(double timestep) {
    if (!config_.enableNutritionalInfluence) return;
    
    double nutritionalEfficiency = std::min(1.0, pImpl->nutrition * 2.0);  // 0.5+ nutrition is good
    age_ *= (1.0 + (nutritionalEfficiency - 0.5) * 0.1);
    
    // Record nutrition experience
    if (config_.enableExperienceDependence) {
        recordExperience("nutrition", pImpl->nutrition);
    }
}

void DevelopmentSystem::applySocialInfluence(double timestep) {
    if (!config_.enableSocialInfluence) return;
    
    double socialMultiplier = 1.0 + pImpl->socialSupport * 0.15;
    age_ *= socialMultiplier;
    
    // Record social experience
    if (config_.enableExperienceDependence) {
        recordExperience("social", pImpl->socialSupport);
    }
}

void DevelopmentSystem::applyStressInfluence(double timestep) {
    if (!config_.enableStressInfluence) return;
    
    double stressImpact = -pImpl->stress * 0.1;  // Stress is negative for development
    age_ *= (1.0 + stressImpact);
    
    // Record stress experience
    if (config_.enableExperienceDependence) {
        recordExperience("stress", pImpl->stress);
    }
}

void DevelopmentSystem::applyTemperatureInfluence(double timestep) {
    if (!config_.enableTemperatureInfluence) return;
    
    double temperatureDeviation = std::abs(pImpl->temperature - 20.0) / 20.0;
    double temperatureImpact = -temperatureDeviation * 0.05;  // Temperature extremes slow development
    age_ *= (1.0 + temperatureImpact);
    
    // Record temperature experience
    if (config_.enableExperienceDependence) {
        recordExperience("temperature", pImpl->temperature);
    }
}

void DevelopmentSystem::applyIndividualVariation(double timestep) {
    if (!config_.enableGeneticInfluence) return;
    
    // Apply genetic base age
    double geneticAgeMultiplier = 1.0 + (pImpl->geneticBaseAge - 0.5) * 0.1;
    age_ *= geneticAgeMultiplier;
    
    // Apply age noise (stochastic variation)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> noiseDist(0.0, pImpl->ageNoise);
    
    double noiseEffect = noiseDist(gen) * 0.05;  // Small random variation
    age_ *= (1.0 + noiseEffect);
}

void DevelopmentSystem::recordExperience(const std::string& experienceType, double value) {
    pImpl->experienceHistory[experienceType] = value;
    
    // Update running average
    double current = pImpl->experienceLevels[experienceType];
    double newValue = (current * 9.0 + value) / 10.0;  // Weighted average
    pImpl->experienceLevels[experienceType] = newValue;
}

float DevelopmentSystem::getExperienceLevel(const std::string& experienceType) const {
    auto it = pImpl->experienceLevels.find(experienceType);
    return it != pImpl->experienceLevels.end() ? it->second : 0.0f;
}

void DevelopmentSystem::updateAdaptivePlasticity(double timestep) {
    if (!config_.enableAdaptivePlasticity) return;
    
    // Adjust plasticity based on environmental conditions
    double environmentalMatch = 1.0 - std::abs(getPlasticityModifier() - 0.5);  // Target plasticity of 0.5
    double plasticityAdjustment = environmentalMatch * config_.plasticityLearningRate * timestep;
    
    // Find current stage plasticity
    auto stageIt = config_.plasticityRates.find(pImpl->stage);
    if (stageIt != config_.plasticityRates.end()) {
        float currentPlasticity = stageIt->second;
        float newPlasticity = currentPlasticity + plasticityAdjustment;
        
        // Clamp to reasonable range
        newPlasticity = std::max(0.1f, std::min(2.0f, newPlasticity));
        
        config_.plasticityRates[pImpl->stage] = newPlasticity;
    }
}

void DevelopmentSystem::adjustPlasticityBasedOnEnvironment() {
    if (!config_.enableAdaptivePlasticity) return;
    
    // Adjust plasticity based on current environment
    double stressLevel = pImpl->stress;
    double enrichmentLevel = pImpl->enrichment;
    
    // Higher stress = more plasticity needed for adaptation
    // Higher enrichment = more plasticity for exploration
    double plasticityNeed = stressLevel * 0.5 + enrichmentLevel * 0.3;
    
    // Adjust current stage plasticity
    auto stageIt = config_.plasticityRates.find(pImpl->stage);
    if (stageIt != config_.plasticityRates.end()) {
        float currentPlasticity = stageIt->second;
        float targetPlasticity = std::min(1.0f, 0.5f + plasticityNeed * 0.3f);
        
        // Gradual adjustment toward target
        float adjustment = (targetPlasticity - currentPlasticity) * config_.plasticityLearningRate;
        float newPlasticity = currentPlasticity + adjustment;
        
        config_.plasticityRates[pImpl->stage] = std::max(0.1f, std::min(2.0f, newPlasticity));
    }
}

void DevelopmentSystem::applyExperienceDependence(double timestep) {
    if (!config_.enableExperienceDependence) return;
    
    // Apply experience-dependent development
    for (const auto& pair : pImpl->experienceLevels) {
        const std::string& experienceType = pair.first;
        double experienceLevel = pair.second;
        
        // Different experience types affect development differently
        double experienceEffect = 0.0;
        if (experienceType == "nutrition") {
            experienceEffect = (experienceLevel - 0.5) * 0.15;
        } else if (experienceType == "social") {
            experienceEffect = experienceLevel * 0.1;
        } else if (experienceType == "stress") {
            experienceEffect = -experienceLevel * 0.08;
        } else if (experienceType == "temperature") {
            experienceEffect = -std::abs(experienceLevel - 20.0) / 100.0 * 0.05;
        }
        
        age_ *= (1.0 + experienceEffect * timestep);
    }
}

void DevelopmentSystem::applyDevelopmentalDisorders(double timestep) {
    if (!pImpl->hasDisorder) return;
    
    double disorderImpact = -pImpl->disorderSeverity * 0.3;  // Disorder severity slows development
    age_ *= (1.0 + disorderImpact * timestep);
}

void DevelopmentSystem::setDisorder(const std::string& disorderType, float severity) {
    pImpl->disorderType = disorderType;
    pImpl->disorderSeverity = severity;
    pImpl->hasDisorder = (severity > 0.0f);
    
    if (pImpl->hasDisorder && config_.enableDebugLogging) {
        NLM_LOG_WARNING(std::string("Developmental disorder set: ") + disorderType + 
                        std::string(" with severity ") + std::to_string(severity));
    }
}

bool DevelopmentSystem::hasDisorder() const {
    return pImpl->hasDisorder;
}

std::string DevelopmentSystem::getDisorderType() const {
    return pImpl->disorderType;
}

float DevelopmentSystem::getDisorderSeverity() const {
    return pImpl->disorderSeverity;
}

void DevelopmentSystem::enableResearchMode(bool enable, const std::map<std::string, double>& parameters) {
    pImpl->researchMode = enable;
    if (enable) {
        pImpl->researchParameters = parameters;
        NLM_LOG_INFO("Research mode enabled with parameters");
    }
}

bool DevelopmentSystem::isResearchModeEnabled() const {
    return pImpl->researchMode;
}

double DevelopmentSystem::getResearchParameter(const std::string& name) const {
    auto it = pImpl->researchParameters.find(name);
    return it != pImpl->researchParameters.end() ? it->second : 0.0;
}

void DevelopmentSystem::setStageDuration(DevelopmentalStage stage, double duration) {
    config_.stageDurations[stage] = duration;
}

double DevelopmentSystem::getStageDuration(DevelopmentalStage stage) const {
    auto it = config_.stageDurations.find(stage);
    return it != config_.stageDurations.end() ? it->second : 0.0;
}

void DevelopmentSystem::addStage(DevelopmentalStage stage, double duration, float plasticityRate) {
    config_.stageDurations[stage] = duration;
    config_.plasticityRates[stage] = plasticityRate;
    config_.learningRates[stage] = plasticityRate * 0.5f;  // Default learning rate
}

void DevelopmentSystem::removeStage(DevelopmentalStage stage) {
    config_.stageDurations.erase(stage);
    config_.plasticityRates.erase(stage);
    config_.learningRates.erase(stage);
}

std::vector<DevelopmentalStage> DevelopmentSystem::getAvailableStages() const {
    std::vector<DevelopmentalStage> stages;
    for (const auto& pair : config_.stageDurations) {
        stages.push_back(pair.first);
    }
    return stages;
}

double DevelopmentSystem::getStageProgress() const {
    auto it = config_.stageDurations.find(pImpl->stage);
    if (it == config_.stageDurations.end()) return 0.0;
    
    double stageDuration = it->second;
    return std::min(1.0, pImpl->stageAge / stageDuration);
}

double DevelopmentSystem::getOverallProgress() const {
    double totalDuration = 0.0;
    double elapsedDuration = 0.0;
    
    for (const auto& pair : config_.stageDurations) {
        totalDuration += pair.second;
        if (pair.first == pImpl->stage) {
            elapsedDuration += pImpl->stageAge;
        }
    }
    
    return totalDuration > 0.0 ? elapsedDuration / totalDuration : 0.0;
}

std::map<DevelopmentalStage, double> DevelopmentSystem::getStageProgressMap() const {
    std::map<DevelopmentalStage, double> progressMap;
    
    for (const auto& pair : config_.stageDurations) {
        DevelopmentalStage stage = pair.first;
        double stageDuration = pair.second;
        
        double stageProgress = 0.0;
        if (stage == pImpl->stage) {
            stageProgress = std::min(1.0, pImpl->stageAge / stageDuration);
        }
        
        progressMap[stage] = stageProgress;
    }
    
    return progressMap;
}

bool DevelopmentSystem::hasReachedMilestone(const std::string& milestoneName) const {
    return pImpl->completedMilestones.end() != std::find(pImpl->completedMilestones.begin(), 
                                                         pImpl->completedMilestones.end(), milestoneName);
}

void DevelopmentSystem::setMilestoneCompleted(const std::string& milestoneName, bool completed) {
    if (completed) {
        if (std::find(pImpl->completedMilestones.begin(), pImpl->completedMilestones.end(), 
                     milestoneName) == pImpl->completedMilestones.end()) {
            pImpl->completedMilestones.push_back(milestoneName);
            NLM_LOG_INFO(std::string("Milestone completed: ") + milestoneName);
        }
    } else {
        auto it = std::find(pImpl->completedMilestones.begin(), pImpl->completedMilestones.end(), milestoneName);
        if (it != pImpl->completedMilestones.end()) {
            pImpl->completedMilestones.erase(it);
        }
    }
}

std::vector<std::string> DevelopmentSystem::getCompletedMilestones() const {
    return pImpl->completedMilestones;
}

void DevelopmentSystem::handleStageTransitions() {
    // Auto-advance stage based on developmental age
    for (const auto& pair : config_.stageDurations) {
        DevelopmentalStage stage = pair.first;
        double stageDuration = pair.second;
        
        if (stage == pImpl->stage) {
            // Check if current stage should advance
            if (pImpl->stageAge > stageDuration * 0.9) {  // Advance at 90% completion
                if (stage == DevelopmentalStage::ExperimentalStage3) {
                    // Return to normal development
                    setStage(DevelopmentalStage::PrimeAdult);
                } else {
                    advanceToNextAppropriateStage();
                }
                break;
            }
        }
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    auto it = config_.plasticityRates.find(pImpl->stage);
    if (it != config_.plasticityRates.end()) {
        return it->second;
    }
    return 0.5f;  // Default plasticity
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

void DevelopmentSystem::validateConfiguration() const {
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO(std::string("Development system configuration validated. Stages: ") + 
                    std::to_string(config_.stageDurations.size()));
    }
}

} // namespace nlm
