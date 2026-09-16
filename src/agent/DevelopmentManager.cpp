#include "DevelopmentManager.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

DevelopmentManager::DevelopmentManager(const DevelopmentConfig& config) 
    : developmentalAge_(0.0), currentStage_(DevelopmentalStage::Initial),
      plasticityModifier_(config.initialPlasticity_), stageStartTime_(0.0),
      config_(config) {
    
    NLM_LOG_DEBUG("DevelopmentManager initialized");
}

DevelopmentManager::~DevelopmentManager() {
    // Clean up custom transitions
    customTransitions_.clear();
}

void DevelopmentManager::update(double timestep, bool enableDevelopment) {
    if (!enableDevelopment || !config_.enableDevelopment_) {
        return;
    }
    
    // Update developmental age
    developmentalAge_ += timestep;
    
    // Update time in current stage
    stageStartTime_ += timestep;
    
    // Process stage transitions
    processStageTransition();
    
    // Apply stage-specific effects (for backward compatibility)
    applyStageSpecificEffects(currentStage_);
    
    NLM_LOG_DEBUG("DevelopmentManager updated: age=" + 
                 std::to_string(developmentalAge_) + 
                 "s, stage=" + std::to_string(static_cast<int>(currentStage_)) +
                 ", plasticity=" + std::to_string(plasticityModifier_));
}

void DevelopmentManager::processStageTransition() {
    if (!config_.enableDevelopment_) {
        return;
    }
    
    // Check for custom transitions first
    bool transitioned = false;
    for (const auto& event : customTransitions_) {
        if (developmentalAge_ >= event.timestamp && 
            currentStage_ == event.fromStage &&
            event.toStage != currentStage_) {
            
            addEvent(event.fromStage, event.toStage, event.plasticityModifier,
                     event.description);
            currentStage_ = event.toStage;
            stageStartTime_ = 0.0;
            plasticityModifier_ = event.plasticityModifier;
            transitioned = true;
            break;
        }
    }
    
    if (transitioned) return;
    
    // Check standard developmental stages
    double stageDuration = 0.0;
    float stagePlasticity = 0.0f;
    
    if (currentStage_ == DevelopmentalStage::Initial) {
        if (developmentalAge_ >= config_.initialStageDuration_) {
            stageDuration = config_.initialStageDuration_;
            stagePlasticity = config_.criticalPeriodPlasticity_;
            currentStage_ = DevelopmentalStage::CriticalPeriod;
            addEvent(DevelopmentalStage::Initial, DevelopmentalStage::CriticalPeriod,
                     stagePlasticity, "Transition to Critical Period");
        }
    } else if (currentStage_ == DevelopmentalStage::CriticalPeriod) {
        if (developmentalAge_ >= config_.initialStageDuration_ + 
                           config_.criticalPeriodDuration_) {
            stageDuration = config_.criticalPeriodDuration_;
            stagePlasticity = config_.maturationPlasticity_;
            currentStage_ = DevelopmentalStage::Maturation;
            addEvent(DevelopmentalStage::CriticalPeriod, DevelopmentalStage::Maturation,
                     stagePlasticity, "Transition to Maturation");
        }
    } else if (currentStage_ == DevelopmentalStage::Maturation) {
        if (developmentalAge_ >= config_.initialStageDuration_ + 
                           config_.criticalPeriodDuration_ + 
                           config_.maturationDuration_) {
            stageDuration = config_.maturationDuration_;
            stagePlasticity = config_.adultPlasticity_;
            currentStage_ = DevelopmentalStage::Adult;
            addEvent(DevelopmentalStage::Maturation, DevelopmentalStage::Adult,
                     stagePlasticity, "Transition to Adult");
        }
    }
    
    // If we transitioned stages, reset stage timer
    if (stageDuration > 0.0) {
        stageStartTime_ = 0.0;
        plasticityModifier_ = stagePlasticity;
    }
}

void DevelopmentManager::applyStageSpecificEffects(DevelopmentalStage stage) const {
    // Stage-specific effects for backward compatibility
    // This could be expanded with more complex stage-dependent behaviors
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity, rapid synaptogenesis
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for sensory processing
            break;
        case DevelopmentalStage::Maturation:
            // Refining connections
            break;
        case DevelopmentalStage::Adult:
            // Stable network with limited plasticity
            break;
        case DevelopmentalStage::Aging:
            // Decline in plasticity (for completeness)
            break;
        default:
            break;
    }
}

float DevelopmentManager::calculatePlasticityModifier() const {
    float modifier = plasticityModifier_;
    
    // Apply config-based overrides
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            modifier = config_.initialPlasticity_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            modifier = config_.criticalPeriodPlasticity_;
            break;
        case DevelopmentalStage::Maturation:
            modifier = config_.maturationPlasticity_;
            break;
        case DevelopmentalStage::Adult:
            modifier = config_.adultPlasticity_;
            break;
        case DevelopmentalStage::Aging:
            modifier = 0.1f; // Reduced plasticity in aging
            break;
    }
    
    // Apply activity-dependent plasticity
    if (config_.enableActivityDependentPlasticity_) {
        // This would be based on recent neural activity
        // For now, we just return the base modifier
    }
    
    // Clamp to reasonable range
    modifier = std::clamp(modifier, 0.0f, 2.0f);
    
    return modifier;
}

void DevelopmentManager::applyToSTDP(STDP* stdp) const {
    if (!stdp) {
        NLM_LOG_WARNING("STDP pointer is null");
        return;
    }
    
    float plasticity = calculatePlasticityModifier();
    
    // Scale STDP weights based on developmental stage
    stdp->setLTPWeight(0.01f * plasticity);
    stdp->setLTDWeight(0.012f * plasticity);
    
    NLM_LOG_DEBUG("Applied developmental effects to STDP: plasticity=" +
                 std::to_string(plasticity) +
                 ", LTP weight=" + std::to_string(stdp->getLTPWeight()) +
                 ", LTD weight=" + std::to_string(stdp->getLTDWeight()));
}

void DevelopmentManager::applyToStructuralPlasticity(StructuralPlasticity* sp) const {
    if (!sp || !config_.enableStructuralPlasticity_) {
        return;
    }
    
    float plasticity = calculatePlasticityModifier();
    
    // Calculate stage-specific synaptogenesis and pruning rates
    float synRate = 0.0001f * plasticity;
    float pruneRate = 0.00001f * (2.0f - plasticity);
    
    // Override with stage-specific rates from config if available
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            synRate = config_.initialSynaptogenesis_ * plasticity;
            pruneRate = config_.initialPruning_ * (2.0f - plasticity);
            break;
        case DevelopmentalStage::CriticalPeriod:
            synRate = config_.criticalPeriodSynaptogenesis_ * plasticity;
            pruneRate = config_.criticalPeriodPruning_ * (2.0f - plasticity);
            break;
        case DevelopmentalStage::Maturation:
            synRate = config_.maturationSynaptogenesis_ * plasticity;
            pruneRate = config_.maturationPruning_ * (2.0f - plasticity);
            break;
        case DevelopmentalStage::Adult:
            synRate = config_.adultSynaptogenesis_ * plasticity;
            pruneRate = config_.adultPruning_ * (2.0f - plasticity);
            break;
        default:
            break;
    }
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
    
    NLM_LOG_DEBUG("Applied developmental effects to StructuralPlasticity: synRate=" +
                 std::to_string(synRate) + ", pruneRate=" + std::to_string(pruneRate));
}

void DevelopmentManager::applyToHebbian(Hebbian* hebbian) const {
    if (!hebbian) {
        NLM_LOG_WARNING("Hebbian pointer is null");
        return;
    }
    
    float plasticity = calculatePlasticityModifier();
    
    // Scale Hebbian learning rate based on developmental stage
    hebbian->setLearningRate(0.001f * plasticity);
    
    NLM_LOG_DEBUG("Applied developmental effects to Hebbian: plasticity=" +
                 std::to_string(plasticity) +
                 ", learning rate=" + std::to_string(hebbian->getLearningRate()));
}

double DevelopmentManager::getStageDuration(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return config_.initialStageDuration_;
        case DevelopmentalStage::CriticalPeriod:
            return config_.criticalPeriodDuration_;
        case DevelopmentalStage::Maturation:
            return config_.maturationDuration_;
        case DevelopmentalStage::Adult:
            return std::numeric_limits<double>::max(); // Adult stage continues indefinitely
        case DevelopmentalStage::Aging:
            return std::numeric_limits<double>::max(); // Aging stage continues indefinitely
        default:
            return 0.0;
    }
}

bool DevelopmentManager::isDevelopmentComplete() const {
    return currentStage_ == DevelopmentalStage::Adult;
}

void DevelopmentManager::reset() {
    developmentalAge_ = 0.0;
    currentStage_ = DevelopmentalStage::Initial;
    stageStartTime_ = 0.0;
    plasticityModifier_ = config_.initialPlasticity_;
    
    // Keep events for debugging, but clear them
    events_.clear();
    
    NLM_LOG_DEBUG("DevelopmentManager reset");
}

bool DevelopmentManager::isValid() const {
    // Check for valid developmental state
    if (developmentalAge_ < 0.0) return false;
    if (stageStartTime_ < 0.0) return false;
    if (plasticityModifier_ < 0.0f || plasticityModifier_ > 2.0f) return false;
    if (currentStage_ == DevelopmentalStage::Initial && developmentalAge_ > 0.0) {
        // Only allow transition to other stages after initial duration
        if (developmentalAge_ < config_.initialStageDuration_) {
            return true; // Still in initial stage
        }
    }
    
    return true;
}

void DevelopmentManager::addCustomTransition(double age, DevelopmentalStage fromStage,
                                            DevelopmentalStage toStage, float plasticityModifier,
                                            const std::string& description) {
    DevelopmentEvent event;
    event.timestamp = age;
    event.fromStage = fromStage;
    event.toStage = toStage;
    event.plasticityModifier = plasticityModifier;
    event.description = description;
    
    customTransitions_.push_back(event);
    
    // Sort by age for efficient processing
    std::sort(customTransitions_.begin(), customTransitions_.end(),
              [](const DevelopmentEvent& a, const DevelopmentEvent& b) {
                  return a.timestamp < b.timestamp;
              });
    
    NLM_LOG_DEBUG("Added custom developmental transition: " + description +
                 " at age " + std::to_string(age) + "s");
}

double DevelopmentManager::getTimeInCurrentStage() const {
    return stageStartTime_;
}

void DevelopmentManager::addEvent(DevelopmentalStage from, DevelopmentalStage to,
                                 float plasticity, const std::string& desc) {
    DevelopmentEvent event;
    event.timestamp = developmentalAge_;
    event.fromStage = from;
    event.toStage = to;
    event.plasticityModifier = plasticity;
    event.description = desc;
    
    events_.push_back(event);
    
    // Keep only last N events to prevent memory growth
    const size_t maxEvents = 100;
    if (events_.size() > maxEvents) {
        events_.erase(events_.begin(), events_.begin() + 
                      (events_.size() - maxEvents));
    }
}

} // namespace nlm