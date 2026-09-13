// Improve AgentBrain functionality and organization

// This file enhances the AgentBrain class with:
// 1. Better memory management and cleanup
// 2. More sophisticated motor control algorithms
// 3. Enhanced neuromodulation systems
// 4. Better integration with brain systems
// 5. Advanced curiosity and exploration mechanisms
// 6. Improved reward prediction and learning
// 7. Better developmental progression
// 8. More comprehensive error handling

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>

namespace nlm {

// Forward declarations for nested types
class SensoryPercept;
class SimpleWorld;

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , lastActionTime_(0.0)
    , actionSequenceLength_(0)
    , explorationRate_(0.2f)
    , rewardPredictionHorizon_(5)
    , adaptationFactor_(0.05f)
    , habituationThreshold_(0.3f)
    , advancedFeaturesEnabled_(false)
    , personalizedDevelopment_(false)
    , environmentalAdaptation_(false)
    , socialLearning_(false)
    , emotionalIntelligence_(false)
    , moralDevelopment_(false)
    , consciousnessDevelopment_(false)
    , autonomyDevelopment_(false)
    , selfReflectionEnabled_(false)
    , introspectionLevel_(0.0f)
    , metaAwarenessLevel_(0.0f)
    , workingMemoryCapacity_(1000)
    , episodicMemoryCapacity_(1000)
    , memoryConsolidationRate_(0.1f)
    , learningRate_(0.01f)
    , retentionRate_(0.9f)
    , empathyLevel_(0.5f)
    , theoryOfMindLevel_(0.5f)
    , emotionalIntelligenceLevel_(0.5f)
    , moralReasoningLevel_(0.5f)
    , adaptabilityLevel_(0.5f)
    , innovationRate_(0.1f)
    , conformityLevel_(0.1f)
    , temperatureSensitivity_(1.0f)
    , lightSensitivity_(1.0f)
    , humiditySensitivity_(1.0f)
{
    // Initialize motor and sensory neuron groups with better distribution
    if (brain_) {
        initializeNeuronGroups();
        logInitializationStats();
    }
}

AgentBrain::~AgentBrain() {
    // Clean up neuron group pointers to prevent double deletion
    clearNeuronGroups();
}

void AgentBrain::initializeNeuronGroups() {
    // More sophisticated neuron distribution based on brain configuration
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            auto neurons = pop->getNeurons();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(neurons);
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(neurons);
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    // Enhanced motor neuron distribution with role specialization
    if (neurons.empty()) return;
    
    // Calculate ideal distribution based on neuron count
    size_t motorGroups = 6;  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
    size_t neuronsPerGroup = neurons.size() / motorGroups;
    
    size_t startIdx = 0;
    for (size_t group = 0; group < motorGroups && startIdx < neurons.size(); ++group) {
        size_t groupSize = neuronsPerGroup;
        if (group == motorGroups - 1) {  // Last group gets remainder
            groupSize = neurons.size() - startIdx;
        }
        
        std::vector<Neuron*> groupNeurons(neurons.begin() + startIdx, 
                                        neurons.begin() + startIdx + groupSize);
        
        // Assign neurons to appropriate groups based on group type
        switch (group) {
            case 0: motorForward_ = groupNeurons; break;
            case 1: motorBackward_ = groupNeurons; break;
            case 2: motorTurnLeft_ = groupNeurons; break;
            case 3: motorTurnRight_ = groupNeurons; break;
            case 4: motorInteract_ = groupNeurons; break;
            case 5: motorWait_ = groupNeurons; break;
        }
        
        startIdx += groupSize;
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Enhanced sensory neuron distribution with modality specialization
    if (neurons.empty()) return;
    
    size_t sensoryGroups = 4;  // Vision, Touch, Internal, Proprioception
    size_t neuronsPerGroup = neurons.size() / sensoryGroups;
    
    size_t startIdx = 0;
    for (size_t group = 0; group < sensoryGroups && startIdx < neurons.size(); ++group) {
        size_t groupSize = neuronsPerGroup;
        if (group == sensoryGroups - 1) {  // Last group gets remainder
            groupSize = neurons.size() - startIdx;
        }
        
        std::vector<Neuron*> groupNeurons(neurons.begin() + startIdx, 
                                        neurons.begin() + startIdx + groupSize);
        
        switch (group) {
            case 0: sensoryVision_ = groupNeurons; break;
            case 1: sensoryTouch_ = groupNeurons; break;
            case 2: sensoryInternal_ = groupNeurons; break;
            case 3: sensoryProprioception_ = groupNeurons; break;
        }
        
        startIdx += groupSize;
    }
}

void AgentBrain::clearNeuronGroups() {
    // Clear all neuron group pointers
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
}

void AgentBrain::logInitializationStats() const {
    // Log detailed initialization statistics
    NLM_LOG_INFO("AgentBrain initialization complete:");
    NLM_LOG_INFO("  Motor neuron groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                                                          motorTurnLeft_.size() + motorTurnRight_.size() + 
                                                          motorInteract_.size() + motorWait_.size()));
    NLM_LOG_INFO("  Vision sensory neurons: " + std::to_string(sensoryVision_.size()));
    NLM_LOG_INFO("  Touch sensory neurons: " + std::to_string(sensoryTouch_.size()));
    NLM_LOG_INFO("  Internal sensory neurons: " + std::to_string(sensoryInternal_.size()));
    NLM_LOG_INFO("  Proprioception sensory neurons: " + std::to_string(sensoryProprioception_.size()));
    NLM_LOG_INFO("  Default neuromodulation levels: Dopamine=" + std::to_string(dopamineLevel_) + 
                 ", Novelty=" + std::to_string(noveltyLevel_) + 
                 ", Curiosity=" + std::to_string(curiosityLevel_));
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    actionSequenceLength_ = 0;
    explorationRate_ = 0.2f; // Default exploration rate
    
    // Initialize development state with enhanced metrics
    developmentalMetrics_.clear();
    
    NLM_LOG_INFO("AgentBrain initialized with enhanced features");
}

void AgentBrain::enableAdvancedFeatures() {
    // Enable advanced neuromodulation and learning features
    enableRewardModulation(true);
    enableStructuralPlasticity(true);
    enableDevelopment(true);
    enableCuriosity(true);
    
    // Initialize advanced metrics
    rewardPrediction_.resize(rewardPredictionHorizon_, 0.0f);
    curiosityHistory_.clear();
    adaptationHistory_.clear();
    
    NLM_LOG_INFO("Advanced features enabled");
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Exploration rate set to: " + std::to_string(explorationRate_));
}

void AgentBrain::setRewardPredictionHorizon(size_t horizon) {
    rewardPredictionHorizon_ = horizon;
    rewardPrediction_.resize(horizon, 0.0f);
    NLM_LOG_INFO("Reward prediction horizon set to: " + std::to_string(horizon));
}

void AgentBrain::updateAdvancedNeuromodulation() {
    // Enhanced neuromodulation with more sophisticated calculations
    if (!brain_) return;
    
    // Calculate novelty based on sensory input changes
    float novelty = calculateNoveltyLevel();
    noveltyLevel_ = novelty * sensoryNoveltyDecay_;
    
    // Calculate curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = novelty * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update dopamine level based on prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply adaptation factor to neuromodulation levels
    adaptNeuromodulationLevels();
}

float AgentBrain::calculateNoveltyLevel() {
    if (previousVision_.empty() || vision_.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision_.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision_[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize by number of vision elements
    float novelty = totalDiff / std::max(static_cast<size_t>(1), vision_.size());
    
    // Apply sensory novelty decay
    novelty *= sensoryNoveltyDecay_;
    
    return novelty;
}

void AgentBrain::adaptNeuromodulationLevels() {
    // Adaptive neuromodulation based on experience
    float adaptation = calculateAdaptation();
    
    // Adjust neuromodulation levels based on adaptation
    dopamineLevel_ *= (1.0f + adaptationFactor_ * adaptation);
    curiosityLevel_ *= (1.0f + adaptationFactor_ * adaptation);
    noveltyLevel_ *= (1.0f - adaptationFactor_ * adaptation);
    
    // Clamp to valid ranges
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
}

float AgentBrain::calculateAdaptation() {
    // Calculate adaptation factor based on recent performance
    if (adaptationHistory_.size() < 10) return 0.0f;
    
    // Calculate average reward over recent history
    float totalReward = std::accumulate(adaptationHistory_.begin(), 
                                        adaptationHistory_.end(), 0.0f);
    float avgReward = totalReward / adaptationHistory_.size();
    
    // Higher adaptation if rewards are low (need to learn more)
    float adaptation = std::max(0.0f, (1.0f - std::abs(avgReward)) * 0.5f);
    
    return adaptation;
}

void AgentBrain::recordReward(float reward) {
    // Record reward for adaptation calculations
    adaptationHistory_.push_back(reward);
    
    // Keep only recent history
    if (adaptationHistory_.size() > 100) {
        adaptationHistory_.erase(adaptationHistory_.begin());
    }
    
    // Update reward prediction
    updateRewardPrediction(reward);
}

void AgentBrain::updateRewardPrediction(float actualReward) {
    // Shift reward prediction array
    for (size_t i = rewardPredictionHorizon_ - 1; i > 0; --i) {
        rewardPrediction_[i] = rewardPrediction_[i - 1];
    }
    rewardPrediction_[0] = actualReward;
}

float AgentBrain::predictNextReward() const {
    // Calculate predicted next reward based on recent rewards
    if (rewardPrediction_.empty()) return 0.0f;
    
    float totalPrediction = std::accumulate(rewardPrediction_.begin(), 
                                           rewardPrediction_.end(), 0.0f);
    return totalPrediction / rewardPrediction_.size();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages with more granular progression
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else if (developmentalAge_ < 1800.0) {  // ~30 minutes
        plasticityModifier_ = 0.3f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        plasticityModifier_ = 0.2f;  // Mature adult - stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Record developmental metrics
    developmentalMetrics_.push_back(plasticityModifier_);
    if (developmentalMetrics_.size() > 50) {
        developmentalMetrics_.erase(developmentalMetrics_.begin());
    }
    
    // Enhanced structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // More sophisticated developmental regulation
            float synRate = calculateSynaptogenesisRate();
            float pruneRate = calculatePruningRate();
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

float AgentBrain::calculateSynaptogenesisRate() {
    // Calculate synaptogenesis rate based on developmental stage
    float baseRate = 0.0001f;
    float developmentalFactor = 1.0f;
    
    switch (brain_->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            developmentalFactor = 1.5f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            developmentalFactor = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            developmentalFactor = 1.0f;
            break;
        case DevelopmentalStage::Adult:
            developmentalFactor = 0.8f;
            break;
        default:
            developmentalFactor = 1.0f;
            break;
    }
    
    return baseRate * developmentalFactor * plasticityModifier_;
}

float AgentBrain::calculatePruningRate() {
    // Calculate pruning rate based on developmental stage and synaptic activity
    float baseRate = 0.00001f;
    float activityFactor = calculateSynapticActivityLevel();
    
    return baseRate * activityFactor * (2.0f - plasticityModifier_);
}

float AgentBrain::calculateSynapticActivityLevel() {
    // Calculate overall synaptic activity level
    float totalActivity = 0.0f;
    float maxPossibleActivity = 0.0f;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            totalActivity += std::abs(syn->getWeight());
            maxPossibleActivity += 1.0f;  // Normalized
        }
    }
    
    return maxPossibleActivity > 0.0f ? totalActivity / maxPossibleActivity : 0.0f;
}

void AgentBrain::recordAction(MotorCommand action) {
    actionSequenceLength_++;
    
    // Update curiosity based on action variety
    curiosityHistory_.push_back(action);
    if (curiosityHistory_.size() > 20) {
        curiosityHistory_.erase(curiosityHistory_.begin());
    }
    
    // Check for action repetition patterns
    checkActionHabituation(action);
}

void AgentBrain::checkActionHabituation(MotorCommand action) {
    // Check if the agent is becoming too habitual in its actions
    if (curiosityHistory_.size() < 5) return;
    
    // Count occurrences of this action in recent history
    size_t actionCount = std::count(curiosityHistory_.begin(), curiosityHistory_.end(), action);
    float repetitionRate = static_cast<float>(actionCount) / curiosityHistory_.size();
    
    // Increase exploration rate if action repetition is too high
    if (repetitionRate > habituationThreshold_) {
        explorationRate_ = std::min(explorationRate_ * 1.1f, 0.5f);
        NLM_LOG_INFO("Action habituation detected. Increasing exploration rate to: " + 
                     std::to_string(explorationRate_));
    }
}

float AgentBrain::calculateActionNovelty(MotorCommand currentAction) const {
    // Calculate how novel the current action is based on history
    if (curiosityHistory_.size() < 2) return 1.0f;
    
    // Count different actions in history
    std::set<MotorCommand> uniqueActions(curiosityHistory_.begin(), curiosityHistory_.end());
    float actionVariety = static_cast<float>(uniqueActions.size()) / curiosityHistory_.size();
    
    // Calculate novelty - lower variety means higher novelty
    float novelty = 1.0f - actionVariety;
    
    return novelty;
}

void AgentBrain::enableAdvancedLearning() {
    // Enable advanced learning mechanisms
    enableAdvancedFeatures();
    
    // Initialize advanced learning metrics
    learningMetrics_.clear();
    successHistory_.clear();
    failureHistory_.clear();
    
    NLM_LOG_INFO("Advanced learning mechanisms enabled");
}

void AgentBrain::recordLearningOutcome(bool success, float reward) {
    // Record learning outcomes for analysis
    learningMetrics_.push_back(success ? 1.0f : 0.0f);
    if (success) {
        successHistory_.push_back(reward);
    } else {
        failureHistory_.push_back(reward);
    }
    
    // Keep only recent history
    if (learningMetrics_.size() > 100) {
        learningMetrics_.erase(learningMetrics_.begin());
        if (!successHistory_.empty()) {
            successHistory_.erase(successHistory_.begin());
        }
        if (!failureHistory_.empty()) {
            failureHistory_.erase(failureHistory_.begin());
        }
    }
}

float AgentBrain::calculateLearningEfficiency() const {
    // Calculate learning efficiency based on success rates and rewards
    if (learningMetrics_.empty()) return 0.0f;
    
    float totalSuccess = std::accumulate(learningMetrics_.begin(), learningMetrics_.end(), 0.0f);
    float successRate = totalSuccess / learningMetrics_.size();
    
    float avgSuccessReward = successHistory_.empty() ? 0.0f : 
        std::accumulate(successHistory_.begin(), successHistory_.end(), 0.0f) / successHistory_.size();
    
    float avgFailureReward = failureHistory_.empty() ? 0.0f : 
        std::accumulate(failureHistory_.begin(), failureHistory_.end(), 0.0f) / failureHistory_.size();
    
    // Combine metrics for efficiency score
    float efficiency = (successRate * 0.6f) + (avgSuccessReward * 0.3f) - (std::abs(avgFailureReward) * 0.1f);
    
    return std::clamp(efficiency, 0.0f, 1.0f);
}

void AgentBrain::enablePersonalizedDevelopment() {
    // Enable personalized development based on agent's unique characteristics
    personalizedDevelopment_ = true;
    
    // Initialize personalized metrics
    developmentStyle_ = calculateDevelopmentStyle();
    learningStyle_ = calculateLearningStyle();
    
    NLM_LOG_INFO("Personalized development enabled");
}

AgentBrain::DevelopmentStyle AgentBrain::calculateDevelopmentStyle() const {
    // Calculate development style based on current metrics
    float plasticityVariance = calculatePlasticityVariance();
    float curiosityLevel = getCuriosityLevel();
    
    // Determine development style based on metrics
    if (plasticityVariance > 0.3f && curiosityLevel > 0.5f) {
        return DevelopmentStyle::Adaptive;
    } else if (plasticityVariance < 0.1f) {
        return DevelopmentStyle::Stable;
    } else {
        return DevelopmentStyle::Balanced;
    }
}

AgentBrain::LearningStyle AgentBrain::calculateLearningStyle() const {
    // Calculate learning style based on experience
    float successRate = calculateSuccessRate();
    float rewardConsistency = calculateRewardConsistency();
    
    if (successRate > 0.7f && rewardConsistency > 0.5f) {
        return LearningStyle::Exploratory;
    } else if (successRate < 0.4f) {
        return LearningStyle::TrialAndError;
    } else {
        return LearningStyle::Balanced;
    }
}

float AgentBrain::calculatePlasticityVariance() const {
    // Calculate variance in plasticity modifier over time
    if (developmentalMetrics_.size() < 2) return 0.0f;
    
    float mean = std::accumulate(developmentalMetrics_.begin(), 
                                 developmentalMetrics_.end(), 0.0f) / developmentalMetrics_.size();
    
    float sumSquaredDiff = 0.0f;
    for (float val : developmentalMetrics_) {
        float diff = val - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return sumSquaredDiff / developmentalMetrics_.size();
}

float AgentBrain::calculateSuccessRate() const {
    // Calculate success rate based on learning metrics
    if (learningMetrics_.empty()) return 0.0f;
    
    float totalSuccess = std::accumulate(learningMetrics_.begin(), learningMetrics_.end(), 0.0f);
    return totalSuccess / learningMetrics_.size();
}

float AgentBrain::calculateRewardConsistency() const {
    // Calculate reward consistency over time
    if (successHistory_.empty()) return 0.0f;
    
    float meanReward = std::accumulate(successHistory_.begin(), 
                                      successHistory_.end(), 0.0f) / successHistory_.size();
    
    float sumSquaredDiff = 0.0f;
    for (float reward : successHistory_) {
        float diff = reward - meanReward;
        sumSquaredDiff += diff * diff;
    }
    
    float variance = sumSquaredDiff / successHistory_.size();
    
    // Higher consistency means lower variance
    return 1.0f - std::sqrt(variance);
}

void AgentBrain::setDevelopmentStyle(DevelopmentStyle style) {
    developmentStyle_ = style;
    NLM_LOG_INFO("Development style set to: " + std::to_string(static_cast<int>(style)));
}

void AgentBrain::setLearningStyle(LearningStyle style) {
    learningStyle_ = style;
    NLM_LOG_INFO("Learning style set to: " + std::to_string(static_cast<int>(style)));
}

void AgentBrain::enableEnvironmentalAdaptation() {
    // Enable adaptation to environmental conditions
    environmentalAdaptation_ = true;
    
    // Initialize environmental metrics
    environmentalHistory_.clear();
    
    NLM_LOG_INFO("Environmental adaptation enabled");
}

void AgentBrain::recordEnvironmentalCondition(float temperature, float humidity, float lightLevel) {
    // Record environmental conditions for adaptation
    EnvironmentalCondition envCond;
    envCond.temperature = temperature;
    envCond.humidity = humidity;
    envCond.lightLevel = lightLevel;
    envCond.timestamp = developmentalAge_;
    
    environmentalHistory_.push_back(envCond);
    
    // Keep only recent history
    if (environmentalHistory_.size() > 50) {
        environmentalHistory_.erase(environmentalHistory_.begin());
    }
}

void AgentBrain::adaptToEnvironment() {
    // Adapt behavior based on environmental conditions
    if (!environmentalAdaptation_ || environmentalHistory_.empty()) return;
    
    // Calculate average environmental conditions
    float avgTemperature = 0.0f, avgHumidity = 0.0f, avgLightLevel = 0.0f;
    size_t count = environmentalHistory_.size();
    
    for (const auto& env : environmentalHistory_) {
        avgTemperature += env.temperature;
        avgHumidity += env.humidity;
        avgLightLevel += env.lightLevel;
    }
    
    avgTemperature /= count;
    avgHumidity /= count;
    avgLightLevel /= count;
    
    // Adjust behavior based on environmental conditions
    adjustBehaviorForEnvironment(avgTemperature, avgHumidity, avgLightLevel);
}

void AgentBrain::adjustBehaviorForEnvironment(float temperature, float humidity, float lightLevel) {
    // Adjust behavior based on environmental conditions
    // Higher temperature may increase need for cooling behaviors
    // Lower light levels may increase exploration for food/resources
    // Higher humidity may affect movement patterns
    
    if (temperature > 30.0f) {
        // Hot environment - reduce activity, seek shelter
        explorationRate_ *= 0.8f;
        NLM_LOG_INFO("Hot environment detected. Reducing exploration rate.");
    }
    
    if (lightLevel < 100.0f) {
        // Dark environment - increase exploration and curiosity
        explorationRate_ *= 1.2f;
        NLM_LOG_INFO("Dark environment detected. Increasing exploration rate.");
    }
    
    if (humidity > 70.0f) {
        // High humidity - slower movement, more careful exploration
        explorationRate_ *= 0.9f;
        NLM_LOG_INFO("High humidity detected. Slower movement patterns.");
    }
    
    // Clamp exploration rate
    explorationRate_ = std::clamp(explorationRate_, 0.1f, 0.5f);
}

void AgentBrain::enableSocialLearning() {
    // Enable social learning capabilities
    socialLearning_ = true;
    
    // Initialize social metrics
    socialInteractions_.clear();
    socialSuccessRate_.clear();
    
    NLM_LOG_INFO("Social learning enabled");
}

void AgentBrain::recordSocialInteraction(const std::string& interactionType, bool success, float outcomeValue) {
    // Record social interaction for learning
    SocialInteraction interaction;
    interaction.type = interactionType;
    interaction.success = success;
    interaction.outcomeValue = outcomeValue;
    interaction.timestamp = developmentalAge_;
    
    socialInteractions_.push_back(interaction);
    
    // Update social success rate
    float successRate = calculateSocialSuccessRate();
    socialSuccessRate_.push_back(successRate);
    
    // Keep only recent history
    if (socialInteractions_.size() > 100) {
        socialInteractions_.erase(socialInteractions_.begin());
        if (!socialSuccessRate_.empty()) {
            socialSuccessRate_.erase(socialSuccessRate_.begin());
        }
    }
}

void AgentBrain::applySocialLearning() {
    // Apply social learning principles to behavior
    if (!socialLearning_ || socialInteractions_.empty()) return;
    
    // Analyze successful social interactions
    float socialSuccessRate = calculateSocialSuccessRate();
    
    // Apply successful strategies to individual behavior
    if (socialSuccessRate > 0.7f) {
        // High social success - adopt conservative strategies
        explorationRate_ *= 0.9f;
        NLM_LOG_INFO("High social success rate. Adopting conservative strategies.");
    } else if (socialSuccessRate < 0.3f) {
        // Low social success - increase exploration and try new approaches
        explorationRate_ *= 1.1f;
        NLM_LOG_INFO("Low social success rate. Increasing exploration.");
    }
}

float AgentBrain::calculateSocialSuccessRate() const {
    // Calculate social success rate
    if (socialInteractions_.empty()) return 0.0f;
    
    size_t successfulInteractions = 0;
    for (const auto& interaction : socialInteractions_) {
        if (interaction.success) {
            successfulInteractions++;
        }
    }
    
    return static_cast<float>(successfulInteractions) / socialInteractions_.size();
}

void AgentBrain::enableEmotionalIntelligence() {
    // Enable emotional intelligence capabilities
    emotionalIntelligence_ = true;
    
    // Initialize emotional metrics
    emotionHistory_.clear();
    emotionalState_.reset();
    
    NLM_LOG_INFO("Emotional intelligence enabled");
}

void AgentBrain::setEmotionalState(float fear, float pleasure, float surprise, float anger) {
    // Set emotional state
    emotionalState_.fear = fear;
    emotionalState_.pleasure = pleasure;
    emotionalState_.surprise = surprise;
    emotionalState_.anger = anger;
    emotionalState_.timestamp = developmentalAge_;
    
    emotionHistory_.push_back(emotionalState_);
    
    // Keep only recent history
    if (emotionHistory_.size() > 50) {
        emotionHistory_.erase(emotionHistory_.begin());
    }
    
    // Apply emotional state to behavior
    applyEmotionalStateToBehavior();
}

void AgentBrain::applyEmotionalStateToBehavior() {
    // Apply emotional state to influence behavior
    // High fear reduces exploration, increases caution
    // High pleasure increases reward-seeking behavior
    // High surprise increases curiosity and exploration
    // High anger increases aggression and persistence
    
    if (emotionalState_.fear > 0.7f) {
        // High fear - reduce exploration, increase caution
        explorationRate_ *= 0.7f;
        NLM_LOG_INFO("High fear detected. Reducing exploration and increasing caution.");
    }
    
    if (emotionalState_.pleasure > 0.7f) {
        // High pleasure - increase reward-seeking
        explorationRate_ *= 1.3f;
        NLM_LOG_INFO("High pleasure detected. Increasing reward-seeking behavior.");
    }
    
    if (emotionalState_.surprise > 0.7f) {
        // High surprise - increase curiosity and exploration
        explorationRate_ *= 1.5f;
        NLM_LOG_INFO("High surprise detected. Increasing exploration and curiosity.");
    }
    
    if (emotionalState_.anger > 0.7f) {
        // High anger - increase persistence and goal-directed behavior
        explorationRate_ *= 1.1f;
        NLM_LOG_INFO("High anger detected. Increasing persistence.");
    }
    
    // Clamp exploration rate
    explorationRate_ = std::clamp(explorationRate_, 0.1f, 0.5f);
}

float AgentBrain::calculateEmotionalStability() const {
    // Calculate emotional stability based on emotion history
    if (emotionHistory_.size() < 2) return 1.0f;
    
    // Calculate variance in emotional states
    float meanFear = 0.0f, meanPleasure = 0.0f, meanSurprise = 0.0f, meanAnger = 0.0f;
    size_t count = emotionHistory_.size();
    
    for (const auto& emotion : emotionHistory_) {
        meanFear += emotion.fear;
        meanPleasure += emotion.pleasure;
        meanSurprise += emotion.surprise;
        meanAnger += emotion.anger;
    }
    
    meanFear /= count;
    meanPleasure /= count;
    meanSurprise /= count;
    meanAnger /= count;
    
    // Calculate variance
    float fearVariance = 0.0f, pleasureVariance = 0.0f, surpriseVariance = 0.0f, angerVariance = 0.0f;
    
    for (const auto& emotion : emotionHistory_) {
        float fearDiff = emotion.fear - meanFear;
        float pleasureDiff = emotion.pleasure - meanPleasure;
        float surpriseDiff = emotion.surprise - meanSurprise;
        float angerDiff = emotion.anger - meanAnger;
        
        fearVariance += fearDiff * fearDiff;
        pleasureVariance += pleasureDiff * pleasureDiff;
        surpriseVariance += surpriseDiff * surpriseDiff;
        angerVariance += angerDiff * angerDiff;
    }
    
    fearVariance /= count;
    pleasureVariance /= count;
    surpriseVariance /= count;
    angerVariance /= count;
    
    // Calculate overall emotional stability (lower variance = higher stability)
    float totalVariance = fearVariance + pleasureVariance + surpriseVariance + angerVariance;
    float stability = 1.0f - std::sqrt(totalVariance);
    
    return std::clamp(stability, 0.0f, 1.0f);
}

void AgentBrain::enableMoralDevelopment() {
    // Enable moral development capabilities
    moralDevelopment_ = true;
    
    // Initialize moral metrics
    moralValues_.clear();
    ethicalDecisions_.clear();
    
    NLM_LOG_INFO("Moral development enabled");
}

void AgentBrain::recordMoralDecision(const std::string& decision, const std::string& reasoning, bool ethical) {
    // Record moral decision for development
    MoralDecision moralDecision;
    moralDecision.decision = decision;
    moralDecision.reasoning = reasoning;
    moralDecision.ethical = ethical;
    moralDecision.timestamp = developmentalAge_;
    
    moralValues_.push_back(ethical ? 1.0f : 0.0f);
    ethicalDecisions_.push_back(moralDecision);
    
    // Keep only recent history
    if (moralValues_.size() > 50) {
        moralValues_.erase(moralValues_.begin());
        if (!ethicalDecisions_.empty()) {
            ethicalDecisions_.erase(ethicalDecisions_.begin());
        }
    }
}

void AgentBrain::applyMoralDevelopment() {
    // Apply moral development principles to behavior
    if (!moralDevelopment_ || moralValues_.empty()) return;
    
    // Calculate ethical consistency
    float ethicalConsistency = calculateEthicalConsistency();
    
    // Apply moral principles to behavior
    if (ethicalConsistency > 0.7f) {
        // High ethical consistency - follow moral rules
        explorationRate_ *= 0.9f;
        NLM_LOG_INFO("High ethical consistency. Following moral principles.");
    } else if (ethicalConsistency < 0.3f) {
        // Low ethical consistency - more flexible behavior
        explorationRate_ *= 1.1f;
        NLM_LOG_INFO("Low ethical consistency. More flexible behavior.");
    }
}

float AgentBrain::calculateEthicalConsistency() const {
    // Calculate ethical consistency over time
    if (moralValues_.empty()) return 0.0f;
    
    float totalEthics = std::accumulate(moralValues_.begin(), moralValues_.end(), 0.0f);
    return totalEthics / moralValues_.size();
}

void AgentBrain::enableConsciousnessDevelopment() {
    // Enable consciousness development capabilities
    consciousnessDevelopment_ = true;
    
    // Initialize consciousness metrics
    awarenessLevel_ = 0.5f;
    selfAwareness_ = 0.3f;
    metacognitiveAbility_ = 0.4f;
    
    NLM_LOG_INFO("Consciousness development enabled");
}

void AgentBrain::updateAwarenessLevel() {
    // Update awareness level based on experience
    if (!brain_) return;
    
    // Calculate awareness based on sensory processing and memory usage
    float sensoryAwareness = calculateSensoryAwareness();
    float memoryAwareness = calculateMemoryAwareness();
    float socialAwareness = calculateSocialAwareness();
    
    awarenessLevel_ = (sensoryAwareness + memoryAwareness + socialAwareness) / 3.0f;
}

float AgentBrain::calculateSensoryAwareness() const {
    // Calculate sensory awareness based on input processing
    if (vision_.empty()) return 0.0f;
    
    // Calculate sensory entropy ( unpredictability in sensory input)
    float mean = std::accumulate(vision_.begin(), vision_.end(), 0.0f) / vision_.size();
    float variance = 0.0f;
    
    for (float val : vision_) {
        float diff = val - mean;
        variance += diff * diff;
    }
    
    variance /= vision_.size();
    float entropy = std::sqrt(variance);
    
    // Higher entropy = higher awareness
    return std::min(entropy * 10.0f, 1.0f);
}

float AgentBrain::calculateMemoryAwareness() const {
    // Calculate memory awareness based on memory system usage
    if (!brain_) return 0.0f;
    
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) return 0.0f;
    
    float workingMemoryUsage = static_cast<float>(workingMemory->getActiveTraces()) / workingMemory->getCapacity();
    float episodicMemoryUsage = static_cast<float>(episodicMemory->getEpisodeCount()) / episodicMemory->getMaxEpisodes();
    
    return (workingMemoryUsage + episodicMemoryUsage) / 2.0f;
}

float AgentBrain::calculateSocialAwareness() const {
    // Calculate social awareness based on social interactions
    if (socialInteractions_.empty()) return 0.0f;
    
    // Calculate social complexity based on interaction variety
    std::set<std::string> interactionTypes;
    for (const auto& interaction : socialInteractions_) {
        interactionTypes.insert(interaction.type);
    }
    
    float interactionVariety = static_cast<float>(interactionTypes.size()) / std::max(static_cast<size_t>(1), socialInteractions_.size());
    
    // Calculate social success rate
    float socialSuccess = calculateSocialSuccessRate();
    
    return (interactionVariety + socialSuccess) / 2.0f;
}

void AgentBrain::updateSelfAwareness() {
    // Update self-awareness based on reflection and meta-cognition
    if (!brain_) return;
    
    // Calculate self-awareness based on learning metrics
    float learningEfficiency = calculateLearningEfficiency();
    float emotionalStability = calculateEmotionalStability();
    float ethicalConsistency = calculateEthicalConsistency();
    
    selfAwareness_ = (learningEfficiency + emotionalStability + ethicalConsistency) / 3.0f;
}

void AgentBrain::updateMetacognitiveAbility() {
    // Update metacognitive ability (thinking about thinking)
    if (!brain_) return;
    
    // Calculate metacognitive ability based on various factors
    float awarenessMetacognition = awarenessLevel_ * 0.4f;
    float memoryMetacognition = calculateMemoryAwareness() * 0.3f;
    float emotionalMetacognition = calculateEmotionalStability() * 0.3f;
    
    metacognitiveAbility_ = awarenessMetacognition + memoryMetacognition + emotionalMetacognition;
    metacognitiveAbility_ = std::clamp(metacognitiveAbility_, 0.0f, 1.0f);
}

void AgentBrain::updateConsciousnessMetrics() {
    // Update all consciousness-related metrics
    if (!consciousnessDevelopment_) return;
    
    updateAwarenessLevel();
    updateSelfAwareness();
    updateMetacognitiveAbility();
    
    // Log consciousness development progress
    NLM_LOG_INFO("Consciousness development - Awareness: " + std::to_string(awarenessLevel_) + 
                 ", Self-Awareness: " + std::to_string(selfAwareness_) + 
                 ", Metacognitive Ability: " + std::to_string(metacognitiveAbility_));
}

void AgentBrain::enableAutonomyDevelopment() {
    // Enable autonomy development capabilities
    autonomyDevelopment_ = true;
    
    // Initialize autonomy metrics
    goalSettingAbility_ = 0.5f;
    decisionMakingAbility_ = 0.5f;
    selfRegulationAbility_ = 0.5f;
    
    NLM_LOG_INFO("Autonomy development enabled");
}

void AgentBrain::setGoals(const std::vector<std::string>& goals) {
    // Set goals for autonomy development
    goals_ = goals;
    NLM_LOG_INFO("Set " + std::to_string(goals.size()) + " goals for autonomy development");
}

void AgentBrain::updateGoalProgress(const std::string& goal, float progress) {
    // Update progress on specific goals
    goalProgress_[goal] = progress;
}

void AgentBrain::applyAutonomyDevelopment() {
    // Apply autonomy development principles to behavior
    if (!autonomyDevelopment_) return;
    
    // Calculate overall autonomy level
    float autonomyLevel = calculateAutonomyLevel();
    
    // Apply autonomy to behavior
    if (autonomyLevel > 0.7f) {
        // High autonomy - more self-directed behavior
        explorationRate_ *= 1.2f;
        NLM_LOG_INFO("High autonomy. More self-directed behavior.");
    } else if (autonomyLevel < 0.3f) {
        // Low autonomy - more guidance needed
        explorationRate_ *= 0.8f;
        NLM_LOG_INFO("Low autonomy. More guidance needed.");
    }
}

float AgentBrain::calculateAutonomyLevel() const {
    // Calculate overall autonomy level
    float goalProgressSum = 0.0f;
    for (const auto& pair : goalProgress_) {
        goalProgressSum += pair.second;
    }
    
    float goalProgressAvg = goalProgress_.empty() ? 0.0f : goalProgressSum / goalProgress_.size();
    
    float decisionMaking = decisionMakingAbility_ * 0.4f;
    float selfRegulation = selfRegulationAbility_ * 0.3f;
    float goalSetting = goalProgressAvg * 0.3f;
    
    return decisionMaking + selfRegulation + goalSetting;
}

void AgentBrain::reset() {
    // Enhanced reset function
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    lastActionTime_ = 0.0;
    actionSequenceLength_ = 0;
    explorationRate_ = 0.2f;
    
    // Clear all history
    previousVision_.clear();
    curiosityHistory_.clear();
    adaptationHistory_.clear();
    learningMetrics_.clear();
    successHistory_.clear();
    failureHistory_.clear();
    socialInteractions_.clear();
    socialSuccessRate_.clear();
    emotionHistory_.clear();
    moralValues_.clear();
    ethicalDecisions_.clear();
    environmentalHistory_.clear();
    goalProgress_.clear();
    
    NLM_LOG_INFO("Enhanced AgentBrain reset complete");
}

} // namespace nlm
