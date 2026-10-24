#include "AdvancedAgentBrainSystems.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <sstream>

namespace nlm {

// MotorDecoder implementation
MotorCommand MotorDecoder::decodeFromNeuralActivity(
    const std::vector<Neuron*>& motorForward,
    const std::vector<Neuron*>& motorBackward,
    const std::vector<Neuron*>& motorTurnLeft,
    const std::vector<Neuron*>& motorTurnRight,
    const std::vector<Neuron*>& motorInteract,
    const std::vector<Neuron*>& motorWait) const {
    
    // Calculate activities for each motor group
    std::vector<float> activities;
    std::vector<MotorCommand> commandMap;
    
    if (!motorForward.empty()) {
        activities.push_back(calculateNeuralActivity(motorForward));
        commandMap.push_back(MotorCommand::MoveForward);
    }
    if (!motorBackward.empty()) {
        activities.push_back(calculateNeuralActivity(motorBackward));
        commandMap.push_back(MotorCommand::MoveBackward);
    }
    if (!motorTurnLeft.empty()) {
        activities.push_back(calculateNeuralActivity(motorTurnLeft));
        commandMap.push_back(MotorCommand::TurnLeft);
    }
    if (!motorTurnRight.empty()) {
        activities.push_back(calculateNeuralActivity(motorTurnRight));
        commandMap.push_back(MotorCommand::TurnRight);
    }
    if (!motorInteract.empty()) {
        activities.push_back(calculateNeuralActivity(motorInteract));
        commandMap.push_back(MotorCommand::Interact);
    }
    if (!motorWait.empty()) {
        activities.push_back(calculateNeuralActivity(motorWait));
        commandMap.push_back(MotorCommand::Wait);
    }
    
    if (activities.empty()) return MotorCommand::Wait;
    
    // Dynamic threshold calculation
    float threshold = calculateDynamicThreshold(activities);
    
    // Enhanced selection with softmax
    size_t selectedIdx = softmaxSelection(activities);
    
    return commandMap[selectedIdx];
}

std::vector<float> MotorDecoder::calculateNeuralActivity(const std::vector<Neuron*>& neurons) const {
    std::vector<float> activities;
    activities.reserve(neurons.size());
    
    for (const Neuron* neuron : neurons) {
        if (!neuron) {
            activities.push_back(0.0f);
            continue;
        }
        
        const NeuronState& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        // Apply smoothing for noise reduction
        static thread_local float previousActivity = 0.0f;
        static thread_local float smoothingFactor = 0.7f;
        
        activity = previousActivity * smoothingFactor + activity * (1.0f - smoothingFactor);
        previousActivity = activity;
        
        // Apply sigmoid-like scaling
        activity = 1.0f / (1.0f + std::exp(-activity * 10.0f + 5.0f));
        
        activities.push_back(activity);
    }
    
    return activities;
}

float MotorDecoder::calculateWeightedActivity(const std::vector<Neuron*>& neurons,
                                             const std::vector<float>& weights) const {
    if (neurons.empty() || weights.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t minSize = std::min(neurons.size(), weights.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        const Neuron* neuron = neurons[i];
        if (!neuron) continue;
        
        const NeuronState& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        totalActivity += activity * weights[i];
    }
    
    return totalActivity / minSize;
}

size_t MotorDecoder::softmaxSelection(const std::vector<float>& activities) const {
    if (activities.empty()) return 0;
    
    // Apply temperature scaling
    float maxActivity = *std::max_element(activities.begin(), activities.end());
    
    std::vector<float> probabilities;
    float sum = 0.0f;
    
    for (float activity : activities) {
        float prob = std::exp((activity - maxActivity) / config_.explorationTemperature);
        probabilities.push_back(prob);
        sum += prob;
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sum;
    }
    
    // Random selection based on probabilities
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());
    
    return dist(gen);
}

size_t MotorDecoder::gatedAttentionSelection(const std::vector<float>& activities,
                                          const std::vector<float>& attentionWeights) const {
    if (activities.size() != attentionWeights.size()) {
        return softmaxSelection(activities);
    }
    
    // Apply attention weights
    std::vector<float> gatedActivities;
    for (size_t i = 0; i < activities.size(); ++i) {
        gatedActivities.push_back(activities[i] * attentionWeights[i]);
    }
    
    return softmaxSelection(gatedActivities);
}

float MotorDecoder::calculateDynamicThreshold(const std::vector<float>& activities) const {
    if (activities.empty()) return config_.motorActivityThreshold;
    
    // Calculate mean and standard deviation
    float mean = std::accumulate(activities.begin(), activities.end(), 0.0f) / activities.size();
    
    float variance = 0.0f;
    for (float activity : activities) {
        variance += (activity - mean) * (activity - mean);
    }
    variance /= activities.size();
    float stddev = std::sqrt(variance);
    
    // Dynamic threshold based on activity distribution
    float threshold = config_.motorActivityThreshold;
    
    // If variance is high (conflicting activities), raise threshold
    if (stddev > 0.3f) {
        threshold += 0.1f;
    }
    
    // If mean activity is very high, lower threshold
    if (mean > 0.8f) {
        threshold -= 0.1f;
    }
    
    // Clamp threshold
    threshold = std::clamp(threshold, 0.0f, 1.0f);
    
    return threshold;
}

MotorCommand MotorDecoder::smoothTransition(const MotorCommand& previous,
                                           const MotorCommand& current,
                                           double timestep) const {
    if (previous == current) return current;
    
    // Check if transition should happen based on timestep
    static thread_local double lastTransitionTime = 0.0;
    
    if (timestep - lastTransitionTime < 0.1) { // Minimum transition time
        return previous; // Still transitioning
    }
    
    lastTransitionTime = timestep;
    return current;
}

// CuriositySystem implementation
float CuriositySystem::calculateCuriosity(float novelty, float predictionError,
                                         float explorationState, float timestamp) const {
    // Multiple components of curiosity
    float noveltyCuriosity = novelty * 2.0f;
    float peCuriosity = std::abs(predictionError) * 0.5f;
    float explorationCuriosity = explorationState * 1.5f;
    
    // Time-dependent curiosity (peaks at certain ages)
    float timeCuriosity = 1.0f; // Simplified time component
    
    // Weighted sum of curiosity components
    float totalCuriosity = noveltyCuriosity + peCuriosity + explorationCuriosity + timeCuriosity;
    
    // Apply noise
    float noise = applyCuriosityNoise(0.0f, config_.curiosityNoiseLevel);
    totalCuriosity += noise;
    
    // Normalize
    return std::clamp(totalCuriosity, 0.0f, 1.0f);
}

float CuriositySystem::calculateExplorationProbability(float curiosity, float temperature) const {
    // Temperature-scaled softmax for exploration probability
    float baseProb = curiosity / (curiosity + temperature);
    
    // Apply max exploration limit
    return std::min(baseProb, config_.maxExplorationChance);
}

MotorCommand CuriositySystem::selectAction(const MotorCommand& defaultAction,
                                          const std::vector<MotorCommand>& allActions,
                                          float curiosity, float explorationProb,
                                          std::shared_ptr<RandomGenerator> rng) const {
    if (!config_.curiosityEnabled || curiosity < config_.curiosityThreshold) {
        return defaultAction;
    }
    
    // Check if exploration should happen
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);
    
    if (dis(gen) < explorationProb) {
        // Random exploration
        if (allActions.empty()) return defaultAction;
        
        size_t randomIdx = dis(gen) * allActions.size();
        return allActions[randomIdx];
    }
    
    return defaultAction;
}

void CuriositySystem::updateExplorationState(float& explorationState, double timestep) const {
    // Decay exploration over time
    explorationState *= config_.explorationDecay;
    
    // Add some growth based on time
    explorationState += 0.01f * timestep;
    explorationState = std::clamp(explorationState, 0.0f, 1.0f);
}

float CuriositySystem::getAdaptiveCuriosityThreshold(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 0.1f; // Lower threshold for exploration in early stages
        case DevelopmentalStage::CriticalPeriod:
            return 0.3f; // Medium threshold
        case DevelopmentalStage::Maturation:
            return 0.4f; // Higher threshold as brain matures
        case DevelopmentalStage::Adult:
            return 0.5f; // Highest threshold in adulthood
        default:
            return config_.curiosityThreshold;
    }
}

// RewardModulation implementation
void RewardModulation::applyModulatedLearning(float reward, float predictedReward,
                                             std::vector<Synapse*>& synapses,
                                             float eligibilityDecay) const {
    // Normalize reward
    reward = normalizeReward(reward);
    
    // Calculate prediction error
    float predictionError = calculatePredictionError(reward, predictedReward, 0.0f);
    
    // Apply learning to synapses
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        // Update eligibility trace
        float eligibility = synapse->getEligibilityTrace();
        eligibility *= (1.0f - eligibilityDecay);
        
        // Apply reward-modulated learning
        float delta = eligibility * calculateDopamineSignal(predictionError, config_.dopamineGain);
        
        // Update weight with plasticity modulation
        float plasticityMod = calculatePlasticityModulator(calculateDopamineSignal(predictionError, 1.0f));
        synapse->addToWeight(delta * plasticityMod);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(eligibilityDecay);
    }
}

float RewardModulation::calculatePredictionError(float reward, float predictedReward,
                                                float eligibilityTrace) const {
    return reward - predictedReward;
}

void RewardModulation::updateExpectedReward(float& expectedReward, float reward,
                                          float alpha) const {
    expectedReward = (1.0f - alpha) * expectedReward + alpha * reward;
}

float RewardModulation::calculateDopamineSignal(float predictionError, float gain) const {
    return predictionError * gain;
}

float RewardModulation::calculatePlasticityModulator(float dopamine, float baseline,
                                                     float sensitivity) const {
    return baseline + sensitivity * dopamine;
}

float RewardModulation::normalizeReward(float reward, float minReward, float maxReward) const {
    if (reward < minReward) return minReward;
    if (reward > maxReward) return maxReward;
    return reward;
}

// DevelopmentSystem implementation
DevelopmentalStage DevelopmentSystem::updateDevelopmentalStage(double age, double timestep) const {
    if (age < 60.0) return DevelopmentalStage::Initial;
    else if (age < 300.0) return DevelopmentalStage::CriticalPeriod;
    else if (age < 900.0) return DevelopmentalStage::Maturation;
    else return DevelopmentalStage::Adult;
}

float DevelopmentSystem::calculatePlasticityModifier(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return config_.plasticityModifiers[0];
        case DevelopmentalStage::CriticalPeriod:
            return config_.plasticityModifiers[1];
        case DevelopmentalStage::Maturation:
            return config_.plasticityModifiers[2];
        case DevelopmentalStage::Adult:
            return config_.plasticityModifiers[3];
        default:
            return config_.plasticityModifiers[0];
    }
}

bool DevelopmentSystem::isCriticalPeriod(DevelopmentalStage stage) const {
    return stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getDevelopmentalReward(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 2.0f; // High reward for exploration
        case DevelopmentalStage::CriticalPeriod:
            return 1.5f; // Moderate reward
        case DevelopmentalStage::Maturation:
            return 1.0f; // Normal reward
        case DevelopmentalStage::Adult:
            return 0.5f; // Low reward for stability
        default:
            return 1.0f;
    }
}

float DevelopmentSystem::calculateMaturationalChange(DevelopmentalStage from, DevelopmentalStage to) const {
    // Calculate change in behavior based on developmental stage transition
    float fromMod = calculatePlasticityModifier(from);
    float toMod = calculatePlasticityModifier(to);
    
    return toMod - fromMod;
}

bool DevelopmentSystem::checkMilestone(double age, DevelopmentalStage stage) const {
    if (stage == DevelopmentalStage::Initial) {
        return age < 60.0;
    } else if (stage == DevelopmentalStage::CriticalPeriod) {
        return age >= 60.0 && age < 300.0;
    } else if (stage == DevelopmentalStage::Maturation) {
        return age >= 300.0 && age < 900.0;
    } else {
        return age >= 900.0;
    }
}

// PerformanceMonitor implementation
void PerformanceMonitor::recordStep(size_t activeNeurons, float firingRate) {
    totalSteps_++;
    activeNeuronsHistory_.push_back(activeNeurons);
    firingRateHistory_.push_back(firingRate);
    
    // Keep history manageable
    if (activeNeuronsHistory_.size() > 1000) {
        activeNeuronsHistory_.erase(activeNeuronsHistory_.begin());
        firingRateHistory_.erase(firingRateHistory_.begin());
    }
}

float PerformanceMonitor::getAverageFiringRate() const {
    if (firingRateHistory_.empty()) return 0.0f;
    
    float sum = std::accumulate(firingRateHistory_.begin(), firingRateHistory_.end(), 0.0f);
    return sum / firingRateHistory_.size();
}

float PerformanceMonitor::getActivityLevel() const {
    if (activeNeuronsHistory_.empty()) return 0.0f;
    
    float sum = std::accumulate(activeNeuronsHistory_.begin(), activeNeuronsHistory_.end(), 0.0f);
    return sum / activeNeuronsHistory_.size();
}

bool PerformanceMonitor::detectActivityDrop() const {
    if (activeNeuronsHistory_.size() < 10) return false;
    
    size_t recentSum = 0;
    size_t recentCount = std::min(size_t(10), activeNeuronsHistory_.size());
    
    for (size_t i = 0; i < recentCount; ++i) {
        recentSum += activeNeuronsHistory_[activeNeuronsHistory_.size() - 1 - i];
    }
    
    float recentAvg = static_cast<float>(recentSum) / recentCount;
    
    size_t totalSum = std::accumulate(activeNeuronsHistory_.begin(), activeNeuronsHistory_.end(), 0);
    float overallAvg = static_cast<float>(totalSum) / activeNeuronsHistory_.size();
    
    return recentAvg < 0.5f * overallAvg;
}

bool PerformanceMonitor::detectExcessiveActivity() const {
    if (activeNeuronsHistory_.empty()) return false;
    
    float avgActivity = getActivityLevel();
    size_t maxNeurons = *std::max_element(activeNeuronsHistory_.begin(), activeNeuronsHistory_.end());
    
    return maxNeurons > 2.0f * avgActivity;
}

std::string PerformanceMonitor::getPerformanceRecommendations() const {
    std::stringstream ss;
    
    if (detectActivityDrop()) {
        ss << "Warning: Activity level dropping. Consider reducing exploration or increasing reward. ";
    }
    
    if (detectExcessiveActivity()) {
        ss << "Warning: Excessive activity. Consider reducing learning rate or increasing inhibition. ";
    }
    
    if (getAverageFiringRate() < 0.1f) {
        ss << "Suggestion: Low firing rate. Consider increasing sensory input or reducing thresholds. ";
    }
    
    if (ss.str().empty()) {
        ss << "Performance within normal range. ";
    }
    
    return ss.str();
}

void PerformanceMonitor::reset() {
    totalSteps_ = 0;
    activeNeuronsHistory_.clear();
    firingRateHistory_.clear();
}

// AgentBrainIntegrator implementation
std::vector<float> AgentBrainIntegrator::processSensoryInput(const SensoryPercept& percept) {
    // Enhanced sensory processing with multiple stages
    std::vector<float> processedInput = percept.getAllSignalsCompressed();
    
    // Apply normalization if configured
    if (config_.enableCompression) {
        // Apply compression if configured
    }
    
    // Apply scaling
    float scaling = config_.sensoryInjectionScale;
    for (float& val : processedInput) {
        val *= scaling;
    }
    
    // Ensure valid range
    for (float& val : processedInput) {
        val = std::clamp(val, -1.0f, 1.0f);
    }
    
    return processedInput;
}

MotorCommand AgentBrainIntegrator::processMotorCommand(double timestep) {
    // Calculate neural activity for motor decoding
    std::vector<float> motorActivities;
    
    // Get motor neuron groups (simplified access - in real implementation would need proper interfaces)
    // This is a placeholder for the actual motor neuron group access
    
    // Use motor decoder for selection
    MotorCommand decodedCmd = motorDecoder_.decodeFromNeuralActivity(
        {}, {}, {}, {}, {}); // Would need actual neuron groups
    
    // Apply smoothing
    MotorCommand smoothCmd = motorDecoder_.smoothTransition(
        previousMotorCommand_, decodedCmd, timestep);
    
    previousMotorCommand_ = smoothCmd;
    lastMotorCommandTime_ = timestep;
    
    return smoothCmd;
}

void AgentBrainIntegrator::processReward(float reward, float predictedReward) {
    // Apply reward modulation
    std::vector<Synapse*> synapses; // Would need actual synapse access
    rewardModulation_.applyModulatedLearning(reward, predictedReward, synapses);
    
    // Update expected reward
    rewardModulation_.updateExpectedReward(expectedReward_, reward);
}

void AgentBrainIntegrator::processDevelopment(double timestep) {
    // Update developmental age
    developmentalAge_ += timestep;
    
    // Update developmental stage
    developmentalStage_ = developmentSystem_.updateDevelopmentalStage(developmentalAge_, timestep);
    
    // Calculate plasticity modifier
    plasticityModifier_ = developmentSystem_.calculatePlasticityModifier(developmentalStage_);
    
    // Update exploration state
    curiositySystem_.updateExplorationState(explorationState_, timestep);
}

AgentBrainIntegrator::AgentState AgentBrainIntegrator::getState() const {
    AgentState state;
    state.curiosityLevel = curiosityLevel_;
    state.noveltyLevel = noveltyLevel_;
    state.predictionError = predictionError_;
    state.dopamineLevel = dopamineLevel_;
    state.developmentalStage = developmentalStage_;
    state.explorationState = explorationState_;
    state.plasticityModifier = plasticityModifier_;
    state.sensoryInputSize = 0; // Would need actual calculation
    state.motorOutputSize = config_.numMotorGroups;
    
    return state;
}

std::string AgentBrainIntegrator::getOptimizationRecommendations() const {
    std::stringstream ss;
    ss << "Performance: " << performanceMonitor_.getPerformanceRecommendations();
    return ss.str();
}

// AdvancedAgentBrain implementation
void AdvancedAgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Use integrator for enhanced processing
    std::vector<float> processedInput = integrator_.processSensoryInput(percept);
    
    // Convert to vector for brain injection (simplified)
    // In real implementation would need proper brain injection interfaces
    
    // Update state
    noveltyLevel_ = curiositySystem_.calculateCuriosity(
        processedInput.empty() ? 0.0f : *std::max_element(processedInput.begin(), processedInput.end()),
        predictionError_,
        explorationState_,
        integrator_.getState().developmentalStage);
}

MotorCommand AdvancedAgentBrain::decodeMotorCommand() {
    double currentTime = integrator_.getState().developmentalStage; // Would use actual time
    return integrator_.processMotorCommand(currentTime);
}

void AdvancedAgentBrain::applyRewardModulation(float reward, float predictedReward) {
    integrator_.processReward(reward, predictedReward);
}

void AdvancedAgentBrain::updateDevelopment(double timestep) {
    integrator_.processDevelopment(timestep);
}

std::string AdvancedAgentBrain::getPerformanceAnalysis() const {
    std::stringstream ss;
    ss << "Agent Performance Analysis:\n";
    ss << "  Curiosity Level: " << getCuriosityLevel() << "\n";
    ss << "  Novelty Level: " << getNoveltyLevel() << "\n";
    ss << "  Prediction Error: " << getPredictionError() << "\n";
    ss << "  Neuromodulation Level: " << getNeuromodulationLevel() << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
    ss << "  Exploration State: " << explorationState_ << "\n";
    ss << "  Plasticity Modifier: " << plasticityModifier_ << "\n";
    return ss.str();
}

std::string AdvancedAgentBrain::getOptimizationSuggestions() const {
    return integrator_.getOptimizationRecommendations();
}

bool AdvancedAgentBrain::loadConfigFromFile(const std::string& filepath) {
    // Load configuration from file
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        return false;
    }
    
    // Apply configuration
    AgentBrainConfig newConfig = config_->getDefaultConfig(); // Simplified
    newConfig.loadFromConfig(*config);
    setConfig(newConfig);
    
    return true;
}

bool AdvancedAgentBrain::loadConfigFromCommandLine(int argc, char** argv) {
    // Load configuration from command line
    auto config = std::make_shared<Config>();
    if (!config->loadFromArgs(argc, argv)) {
        return false;
    }
    
    // Apply configuration
    AgentBrainConfig newConfig = config_->getDefaultConfig(); // Simplified
    newConfig.loadFromConfig(*config);
    setConfig(newConfig);
    
    return true;
}

// Utility function implementations
namespace MotorUtils {
    float calculateDistance(MotorCommand cmd1, MotorCommand cmd2) {
        return static_cast<float>(std::abs(static_cast<int>(cmd1) - static_cast<int>(cmd2)));
    }
    
    MotorCommand interpolateCommands(MotorCommand cmd1, MotorCommand cmd2, float t) {
        int idx1 = static_cast<int>(cmd1);
        int idx2 = static_cast<int>(cmd2);
        int interpolated = static_cast<int>(idx1 + (idx2 - idx1) * t);
        interpolated = std::clamp(interpolated, 0, 7);
        return static_cast<MotorCommand>(interpolated);
    }
    
    std::vector<float> commandToVector(MotorCommand cmd) {
        std::vector<float> vec(8, 0.0f);
        vec[static_cast<int>(cmd)] = 1.0f;
        return vec;
    }
    
    MotorCommand vectorToCommand(const std::vector<float>& vec) {
        if (vec.empty()) return MotorCommand::Wait;
        
        size_t maxIdx = 0;
        float maxVal = vec[0];
        
        for (size_t i = 1; i < vec.size(); ++i) {
            if (vec[i] > maxVal) {
                maxVal = vec[i];
                maxIdx = i;
            }
        }
        
        if (maxIdx >= static_cast<size_t>(MotorCommand::Wait) + 1) {
            return MotorCommand::Wait;
        }
        
        return static_cast<MotorCommand>(maxIdx);
    }
    
    std::vector<MotorCommand> getAllMotorCommands() {
        return {
            MotorCommand::MoveForward,
            MotorCommand::MoveBackward,
            MotorCommand::TurnLeft,
            MotorCommand::TurnRight,
            MotorCommand::LookLeft,
            MotorCommand::LookRight,
            MotorCommand::Interact,
            MotorCommand::Wait
        };
    }
    
    bool isValidMotorCommand(MotorCommand cmd) {
        return static_cast<int>(cmd) >= 0 && static_cast<int>(cmd) < 8;
    }
}

namespace CuriosityUtils {
    float calculateCuriosityEntropy(float curiosityLevel) {
        if (curiosityLevel <= 0.0f || curiosityLevel >= 1.0f) return 0.0f;
        
        float p = curiosityLevel;
        float entropy = -(p * std::log(p) + (1.0f - p) * std::log(1.0f - p));
        return entropy;
    }
    
    float calculateCuriosityGradient(float novelty, float predictionError) {
        return novelty * 0.5f + std::abs(predictionError) * 0.3f;
    }
    
    float applyCuriosityNoise(float baseCuriosity, float noiseLevel) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-noiseLevel, noiseLevel);
        return baseCuriosity + dis(gen);
    }
    
    float temperatureSoftmax(float value, float temperature) {
        if (temperature <= 0.0f) return value;
        return value / temperature;
    }
    
    float normalizeCuriosity(float novelty, float predictionError, float exploration) {
        float total = novelty + std::abs(predictionError) + exploration;
        if (total <= 0.0f) return 0.0f;
        return (novelty + std::abs(predictionError) + exploration) / total;
    }
}

namespace RewardUtils {
    float calculateDiscountedReward(float currentReward, float futureRewards, float gamma) {
        return currentReward + gamma * futureRewards;
    }
    
    float calculateTDError(float reward, float valueNext, float valueCurrent, float discount) {
        return reward + discount * valueNext - valueCurrent;
    }
    
    float normalizeRewards(const std::vector<float>& rewards) {
        if (rewards.empty()) return 0.0f;
        
        float min = *std::min_element(rewards.begin(), rewards.end());
        float max = *std::max_element(rewards.begin(), rewards.end());
        
        if (max - min <= 0.0f) return 0.0f;
        
        float sum = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
        return sum / rewards.size();
    }
    
    float scaleRewards(const std::vector<float>& rewards, float targetMean, float targetStddev) {
        if (rewards.empty()) return 0.0f;
        
        float mean = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        
        float variance = 0.0f;
        for (float reward : rewards) {
            variance += (reward - mean) * (reward - mean);
        }
        variance /= rewards.size();
        float stddev = std::sqrt(variance);
        
        if (stddev <= 0.0f) return mean;
        
        float scale = targetStddev / stddev;
        float offset = targetMean - scale * mean;
        
        float scaledMean = scale * mean + offset;
        return scaledMean;
    }
}

namespace DevelopmentUtils {
    float calculateDevelopmentalRate(double age) {
        if (age < 60.0) return 1.0f;      // Fast development initially
        else if (age < 300.0) return 0.3f; // Slower critical period
        else if (age < 900.0) return 0.1f; // Very slow maturation
        else return 0.01f;                  // Minimal development in adulthood
    }
    
    float getDevelopmentalLearningReward(double age) {
        if (age < 60.0) return 2.0f;
        else if (age < 300.0) return 1.0f;
        else if (age < 900.0) return 0.5f;
        else return 0.1f;
    }
    
    bool isDevelopmentComplete(double age) {
        return age >= 900.0;
    }
    
    float calculateMaturationCoefficient(double age) {
        return 1.0f / (1.0f + std::exp(-(age - 450.0) / 150.0)); // Sigmoid function
    }
}

} // namespace nlm