#include "EnhancedDevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>

namespace nlm {

EnhancedDevelopmentSystem::EnhancedDevelopmentSystem() : developmentSpeed_(1.0), adaptiveDevelopmentEnabled_(false),
    developmentTarget_(EnhancedDevelopmentalStage::Adult), criticalPeriodMultiplier_(1.0f),
    maturationFactor_(1.0f), plasticityDecayRate_(0.001f),
    previousStage_(EnhancedDevelopmentalStage::Initial), lastStageTransitionTime_(0.0), stageTransitioning_(false) {
    
    // Add default developmental stages
    addCustomStage(CustomDevelopmentalStage(EnhancedDevelopmentalStage::Initial, 0.0, 60.0, 1.0f, 1.0f, 0.1f));
    addCustomStage(CustomDevelopmentalStage(EnhancedDevelopmentalStage::CriticalPeriod, 60.0, 300.0, 0.8f, 0.9f, 0.2f));
    addCustomStage(CustomDevelopmentalStage(EnhancedDevelopmentalStage::Maturation, 300.0, 900.0, 0.5f, 0.7f, 0.3f));
    addCustomStage(CustomDevelopmentalStage(EnhancedDevelopmentalStage::Adult, 900.0, 2700.0, 0.2f, 0.5f, 0.4f));
    addCustomStage(CustomDevelopmentalStage(EnhancedDevelopmentalStage::Aging, 2700.0, 5400.0, 0.1f, 0.3f, 0.5f));
    
    // Initialize epigenetic factors
    addEpigeneticFactor(EpigeticFactor("stress", 1.0f, 0.01f, 0.002f));
    addEpigeneticFactor(EpigeticFactor("novelty", 0.5f, 0.005f, 0.001f));
    addEpigeneticFactor(EpigeticFactor("social", 0.7f, 0.003f, 0.001f));
    addEpigeneticFactor(EpigeticFactor("curiosity", 0.8f, 0.004f, 0.001f));
}

EnhancedDevelopmentSystem::~EnhancedDevelopmentSystem() = default;

void EnhancedDevelopmentSystem::addCustomStage(const CustomDevelopmentalStage& stage) {
    customStages_.push_back(stage);
    std::sort(customStages_.begin(), customStages_.end(),
              [](const CustomDevelopmentalStage& a, const CustomDevelopmentalStage& b) {
                  return a.startAge < b.startAge;
              });
}

void EnhancedDevelopmentSystem::removeStage(EnhancedDevelopmentalStage stage) {
    customStages_.erase(std::remove_if(customStages_.begin(), customStages_.end(),
                                       [stage](const CustomDevelopmentalStage& s) {
                                           return s.stage == stage;
                                       }),
                        customStages_.end());
}

void EnhancedDevelopmentSystem::setCurrentStage(EnhancedDevelopmentalStage stage) {
    if (getCustomStage(stage) != nullptr) {
        profile_.currentStage = stage;
    }
}

const CustomDevelopmentalStage* EnhancedDevelopmentSystem::getCustomStage(EnhancedDevelopmentalStage stage) const {
    for (const auto& customStage : customStages_) {
        if (customStage.stage == stage) {
            return &customStage;
        }
    }
    return nullptr;
}

bool EnhancedDevelopmentSystem::attemptStageTransition(Brain* brain, EnhancedDevelopmentalStage newStage) {
    if (!shouldTransitionToStage(newStage)) {
        return false;
    }
    
    EnhancedDevelopmentalStage oldStage = profile_.currentStage;
    setCurrentStage(newStage);
    
    // Update profile history
    profile_.stageHistory.push_back(std::make_pair(profile_.currentAge, profile_.currentStage));
    
    // Trigger callbacks
    for (const auto& callback : stageTransitionCallbacks_) {
        try {
            callback(oldStage, profile_.currentStage, brain);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
    
    // Log stage transition
    NLM_LOG_INFO("Development stage transition: " + getStageName(oldStage) + " -> " + getStageName(profile_.currentStage),
                 "DEVELOPMENT");
    
    return true;
}

bool EnhancedDevelopmentSystem::shouldTransitionToStage(EnhancedDevelopmentalStage stage) const {
    const CustomDevelopmentalStage* currentStage = getCustomStage(profile_.currentStage);
    const CustomDevelopmentalStage* targetStage = getCustomStage(stage);
    
    if (!currentStage || !targetStage) {
        return false;
    }
    
    // Check if current age is within target stage range
    if (profile_.currentAge >= targetStage->startAge && profile_.currentAge <= targetStage->endAge) {
        // Check requirements if any
        for (const auto& requirement : targetStage->requirements) {
            if (requirement == "age" && profile_.currentAge < targetStage->startAge) {
                return false;
            } else if (requirement == "experience" && profile_.currentAge < targetStage->startAge) {
                return false;
            }
        }
        return true;
    }
    
    return false;
}

EnhancedDevelopmentalStage EnhancedDevelopmentSystem::getNextStage() const {
    EnhancedDevelopmentalStage current = profile_.currentStage;
    EnhancedDevelopmentalStage next = current;
    
    // Find next stage based on age
    for (const auto& stage : customStages_) {
        if (stage.stage > current && (next == current || stage.startAge < profile_.currentAge)) {
            next = stage.stage;
        }
    }
    
    return next;
}

void EnhancedDevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    double timeDelta = dt * developmentSpeed_;
    profile_.update(timeDelta);
    
    // Update epigenetic factors
    float environmentalStability = std::clamp(1.0f - (profile_.environmentalStability - 0.5f) * 2.0f, 0.1f, 1.0f);
    float socialComplexity = profile_.socialComplexity;
    updateEpigeneticFactors(environmentalStability, socialComplexity);
    
    // Update developmental stage
    updateDevelopmentalStage(brain, profile_.currentAge);
    
    // Process developmental events
    processDevelopmentalEvents(brain, profile_.currentAge);
    
    // Apply external influences
    applyExternalInfluences(brain);
    
    // Update profile metrics
    updateProfileMetrics(brain);
}

void EnhancedDevelopmentSystem::updateDevelopmentalStage(Brain* brain, double currentAge) {
    // Find current stage based on age
    EnhancedDevelopmentalStage newStage = profile_.currentStage;
    
    for (const auto& stage : customStages_) {
        if (currentAge >= stage.startAge && currentAge <= stage.endAge && stage.stage > newStage) {
            newStage = stage.stage;
        }
    }
    
    // Attempt stage transition if needed
    if (newStage != profile_.currentStage && !stageTransitioning_) {
        attemptStageTransition(brain, newStage);
        stageTransitioning_ = true;
    } else {
        stageTransitioning_ = false;
    }
}

void EnhancedDevelopmentSystem::addEpigeneticFactor(const EpigeneticFactor& factor) {
    epigeneticFactors_.push_back(factor);
    currentEpigeneticState_[factor.name] = factor.baselineValue;
}

void EnhancedDevelopmentSystem::updateEpigeneticFactors(float environmentalStability, float socialComplexity) {
    float adaptationInput = environmentalStability * socialComplexity;
    
    for (auto& factor : epigeneticFactors_) {
        float oldValue = currentEpigeneticState_[factor.name];
        float newValue = factor.adaptationFunction(adaptationInput, environmentalStability);
        
        // Apply decay
        newValue = oldValue + (newValue - oldValue) * (1.0f - factor.decayRate);
        
        // Clamp to valid range
        newValue = std::clamp(newValue, 0.0f, 1.0f);
        
        if (std::abs(newValue - oldValue) > 0.001f) {
            currentEpigeneticState_[factor.name] = newValue;
            
            // Trigger epigenetic change callbacks
            for (const auto& callback : epigeneticChangeCallbacks_) {
                try {
                    callback(factor.name, oldValue, newValue);
                } catch (const std::exception& e) {
                    // Ignore callback errors
                }
            }
        }
    }
}

float EnhancedDevelopmentSystem::getEpigeneticFactor(const std::string& name) const {
    auto it = currentEpigeneticState_.find(name);
    if (it != currentEpigeneticState_.end()) {
        return it->second;
    }
    return 0.0f;
}

void EnhancedDevelopmentSystem::setEpigeneticFactor(const std::string& name, float value) {
    currentEpigeneticState_[name] = std::clamp(value, 0.0f, 1.0f);
}

void EnhancedDevelopmentSystem::addDevelopmentalEvent(const DevelopmentalEvent& event) {
    developmentalEvents_.push_back(event);
    std::sort(developmentalEvents_.begin(), developmentalEvents_.end(),
              [](const DevelopmentalEvent& a, const DevelopmentalEvent& b) {
                  return a.triggerAge < b.triggerAge;
              });
}

void EnhancedDevelopmentSystem::removeDevelopmentalEvent(const std::string& description) {
    developmentalEvents_.erase(std::remove_if(developmentalEvents_.begin(), developmentalEvents_.end(),
                                              [description](const DevelopmentalEvent& e) {
                                                  return e.description == description;
                                              }),
                              developmentalEvents_.end());
}

void EnhancedDevelopmentSystem::processDevelopmentalEvents(Brain* brain, double currentAge) {
    for (auto& event : developmentalEvents_) {
        if (!event.enabled) continue;
        
        if (std::abs(currentAge - event.triggerAge) < 1.0) { // Within 1 time unit
            bool conditionsMet = true;
            for (const auto& condition : event.conditions) {
                if (condition.first == "stage" && getEpigeneticFactor(condition.second) < 0.5f) {
                    conditionsMet = false;
                    break;
                }
            }
            
            if (conditionsMet) {
                attemptStageTransition(brain, event.targetStage);
            }
        }
    }
}

void EnhancedDevelopmentSystem::enableEvent(const std::string& description, bool enable) {
    for (auto& event : developmentalEvents_) {
        if (event.description == description) {
            event.enabled = enable;
            break;
        }
    }
}

void EnhancedDevelopmentSystem::addExternalInfluence(const ExternalInfluence& influence) {
    externalInfluences_.push_back(influence);
}

void EnhancedDevelopmentSystem::removeExternalInfluence(const std::string& source) {
    externalInfluences_.erase(std::remove_if(externalInfluences_.begin(), externalInfluences_.end(),
                                             [source](const ExternalInfluence& inf) {
                                                 return inf.source == source;
                                             }),
                            externalInfluences_.end());
}

void EnhancedDevelopmentSystem::updateExternalInfluences(double currentTime) {
    for (auto& influence : externalInfluences_) {
        influence.startTime = currentTime;
    }
}

void EnhancedDevelopmentSystem::applyExternalInfluences(Brain* brain) {
    double currentTime = profile_.currentAge;
    
    for (const auto& influence : externalInfluences_) {
        float effect = influence.getCurrentEffect(currentTime);
        if (effect > 0.001f) {
            // Apply influence to brain
            // This would modify brain parameters based on the influence
            for (const auto& factor : influence.factors) {
                // Modify brain parameters based on influence
            }
            
            // Trigger external influence callbacks
            for (const auto& callback : externalInfluenceCallbacks_) {
                try {
                    callback(influence, brain);
                } catch (const std::exception& e) {
                    // Ignore callback errors
                }
            }
        }
    }
}

void EnhancedDevelopmentSystem::updateProfile(double timeDelta) {
    profile_.update(timeDelta);
    updateProfileMetrics(nullptr); // Can't access brain here
}

std::string EnhancedDevelopmentSystem::getDevelopmentReport() const {
    std::stringstream ss;
    ss << "Development Report" << std::endl;
    ss << "=================" << std::endl;
    ss << "Current Age: " << profile_.currentAge << "s" << std::endl;
    ss << "Current Stage: " << getStageName(profile_.currentStage) << std::endl;
    ss << "Developmental Score: " << profile_.getDevelopmentScore() << std::endl;
    ss << "Total Learning Experience: " << profile_.totalLearningExperience << std::endl;
    ss << "Epigenetic State: " << profile_.epigeneticState << std::endl;
    ss << std::endl;
    
    ss << "Stage History:" << std::endl;
    for (const auto& stage : profile_.stageHistory) {
        ss << "  Age " << stage.first << ": " << getStageName(stage.second) << std::endl;
    }
    
    return ss.str();
}

void EnhancedDevelopmentSystem::optimizeDevelopmentParameters() {
    // Optimization logic would go here
    // This could adjust developmental parameters based on performance metrics
}

std::map<std::string, float> EnhancedDevelopmentSystem::getOptimalDevelopmentParameters() const {
    std::map<std::string, float> params;
    
    // Get current optimal parameters
    params["plasticity_modifier"] = getCurrentStagePlasticity();
    params["learning_rate"] = getCurrentLearningRate();
    params["exploration_rate"] = getCurrentExplorationRate();
    params["development_speed"] = developmentSpeed_;
    
    return params;
}

float EnhancedDevelopmentSystem::getDevelopmentalHealth() const {
    float stageHealth = static_cast<float>(static_cast<int>(profile_.currentStage)) / 6.0f;
    float epigeneticHealth = 1.0f - std::abs(profile_.epigeneticState - 0.7f) * 2.0f; // Target 0.7
    float experienceHealth = std::min(profile_.totalLearningExperience / 1000.0f, 1.0f);
    
    return (stageHealth + epigeneticHealth + experienceHealth) / 3.0f;
}

void EnhancedDevelopmentSystem::addStageTransitionCallback(StageTransitionCallback callback) {
    stageTransitionCallbacks_.push_back(std::move(callback));
}

void EnhancedDevelopmentSystem::addEpigeneticChangeCallback(EpipenicChangeCallback callback) {
    epigeneticChangeCallbacks_.push_back(std::move(callback));
}

void EnhancedDevelopmentSystem::addExternalInfluenceCallback(ExternalInfluenceCallback callback) {
    externalInfluenceCallbacks_.push_back(std::move(callback));
}

void EnhancedDevelopmentSystem::updateProfileMetrics(Brain* brain) {
    profile_.metrics["plasticity"] = getCurrentStagePlasticity();
    profile_.metrics["learning_rate"] = getCurrentLearningRate();
    profile_.metrics["exploration_rate"] = getCurrentExplorationRate();
    profile_.metrics["developmental_health"] = getDevelopmentalHealth();
}

double EnhancedDevelopmentSystem::calculateDevelopmentalAge(double simulationTime) const {
    return simulationTime * developmentSpeed_;
}

float EnhancedDevelopmentSystem::getCurrentStagePlasticity() const {
    const CustomDevelopmentalStage* currentStage = getCustomStage(profile_.currentStage);
    if (currentStage) {
        float environmentalFactors = (profile_.environmentalStability + profile_.socialComplexity) / 2.0f;
        return currentStage->plasticityModifier * calculateStagePlacticity(currentStage->plasticityModifier, environmentalFactors);
    }
    return 1.0f;
}

float EnhancedDevelopmentSystem::getCurrentLearningRate() const {
    const CustomDevelopmentalStage* currentStage = getCustomStage(profile_.currentStage);
    if (currentStage) {
        float environmentalFactors = (profile_.environmentalStability + profile_.socialComplexity) / 2.0f;
        return currentStage->learningRate * environmentalFactors;
    }
    return 1.0f;
}

float EnhancedDevelopmentSystem::getCurrentExplorationRate() const {
    const CustomDevelopmentalStage* currentStage = getCustomStage(profile_.currentStage);
    if (currentStage) {
        float environmentalFactors = (profile_.environmentalStability + profile_.socialComplexity) / 2.0f;
        return currentStage->explorationRate * environmentalFactors;
    }
    return 0.1f;
}

float EnhancedDevelopmentSystem::calculateStagePlacticity(float stagePlasticity, float environmentalFactors) const {
    return stagePlasticity * (1.0f + (environmentalFactors - 1.0f) * 0.5f);
}

std::string EnhancedDevelopmentSystem::getDevelopmentAnalysis() const {
    std::stringstream ss;
    ss << "Development Analysis" << std::endl;
    ss << "==================" << std::endl;
    ss << "Current Stage: " << getStageName(profile_.currentStage) << std::endl;
    ss << "Developmental Health: " << getDevelopmentalHealth() << std::endl;
    ss << "Epigenetic State: " << profile_.epigeneticState << std::endl;
    ss << "Learning Experience: " << profile_.totalLearningExperience << std::endl;
    ss << "Environmental Stability: " << profile_.environmentalStability << std::endl;
    ss << "Social Complexity: " << profile_.socialComplexity << std::endl;
    ss << std::endl;
    
    ss << "Optimization Recommendations:" << std::endl;
    ss << "- Consider increasing environmental complexity for better development" << std::endl;
    ss << "- Monitor epigenetic factors for stability" << std::endl;
    ss << "- Adjust learning rate based on developmental stage" << std::endl;
    
    return ss.str();
}

const char* EnhancedDevelopmentSystem::getStageName(EnhancedDevelopmentalStage stage) const {
    switch (stage) {
        case EnhancedDevelopmentalStage::Initial: return "Initial";
        case EnhancedDevelopmentalStage::CriticalPeriod: return "Critical Period";
        case EnhancedDevelopmentalStage::Maturation: return "Maturation";
        case EnhancedDevelopmentalStage::Adult: return "Adult";
        case EnhancedDevelopmentalStage::Aging: return "Aging";
        case EnhancedDevelopmentalStage::SensoryIntegration: return "Sensory Integration";
        case EnhancedDevelopmentalStage::MotorCoordination: return "Motor Coordination";
        case EnhancedDevelopmentalStage::CognitiveEmergence: return "Cognitive Emergence";
        case EnhancedDevelopmentalStage::SocialLearning: return "Social Learning";
        case EnhancedDevelopmentalStage::AbstractReasoning: return "Abstract Reasoning";
        case EnhancedDevelopmentalStage::SelfAware: return "Self-Aware";
        case EnhancedDevelopmentalStage::Transcendent: return "Transcendent";
        default: return "Unknown";
    }
}

std::map<std::string, float> EnhancedDevelopmentSystem::getCurrentEpigeneticState() const {
    return currentEpigeneticState_;
}

} // namespace nlm