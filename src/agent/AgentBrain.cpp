#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>

namespace nlm {

// Statistical helper functions
namespace stats {
    template<typename T>
    T mean(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        T sum = std::accumulate(values.begin(), values.end(), T(0));
        return sum / values.size();
    }
    
    template<typename T>
    T variance(const std::vector<T>& values, T mean) {
        if (values.empty()) return T(0);
        T sqSum = 0;
        for (const auto& val : values) {
            sqSum += (val - mean) * (val - mean);
        }
        return sqSum / values.size();
    }
    
    template<typename T>
    T standardDeviation(const std::vector<T>& values, T mean) {
        return std::sqrt(variance(values, mean));
    }
}

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
    , enableSocialLearning_(false)
    , enableEmergencyResponses_(true)
    , stressLevel_(0.0f)
    , fatigueLevel_(0.0f)
    , cognitiveLoad_(0.0f)
    , learningEfficiency_(1.0f)
    , homeostaticState_(1.0f)
    , motivationLevel_(0.5f)
    , intrinsicMotivation_(0.5f)
    , isSleeping_(false)
    , inEmergency_(false)
    , lastConsolidationTime_(0.0)
    , lastObservationTime_(0.0)
    , lastIntrinsicUpdateTime_(0.0)
{
    NLM_LOG_DEBUG("Initializing Advanced AgentBrain");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain with null brain pointer");
        return;
    }
    
    // Initialize advanced neuromodulation
    serotoninLevel_ = 0.5f;
    norepinephrineLevel_ = 0.5f;
    acetylcholineLevel_ = 0.5f;
    
    // Initialize memory system
    memoryTraces_.reserve(1000);
    rewardPredictions_.reserve(100);
    rewardPredictionErrors_.reserve(100);
    observedBehaviors_.reserve(100);
    
    // Initialize motor and sensory neuron groups with advanced distribution
    const auto& regions = brain_->getRegions();
    NLM_LOG_DEBUG("Found " + std::to_string(regions.size()) + " regions");
    
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("Found null region in brain");
            continue;
        }
        
        const auto& populations = region->getPopulations();
        NLM_LOG_DEBUG("Found " + std::to_string(populations.size()) + " populations in region");
        
        for (auto& pop : populations) {
            if (!pop) {
                NLM_LOG_WARNING("Found null population in region");
                continue;
            }
            
            NeuronType type = pop->getNeuronType();
            const auto& neurons = pop->getNeurons();
            NLM_LOG_DEBUG("Found " + std::to_string(neurons.size()) + " neurons of type " + 
                         getNeuronTypeString(type));
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_WARNING("Found null neuron in motor population");
                        continue;
                    }
                    
                    // Advanced distribution: prioritize based on neuron state
                    float priority = 1.0f;
                    if (n->getState().membranePotential > 0.5f) {
                        priority *= 1.5f; // Higher priority for active neurons
                    }
                    
                    // Weighted random distribution
                    size_t idx = getWeightedRandomIndex(priority);
                    switch (idx % 6) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : neurons) {
                    if (!n) {
                        NLM_LOG_WARNING("Found null neuron in sensory population");
                        continue;
                    }
                    
                    // Advanced distribution: prioritize based on neuron sensitivity
                    float sensitivity = n->getSensitivity();
                    size_t idx = getWeightedRandomIndex(sensitivity);
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(n); break;
                        case 1: sensoryTouch_.push_back(n); break;
                        case 2: sensoryInternal_.push_back(n); break;
                        case 3: sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
    
    // Initialize advanced systems
    initializeAdvancedSystems();
    
    NLM_LOG_INFO("Advanced AgentBrain initialized with " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) +
                 " motor neurons and " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size()) +
                 " sensory neurons");
}

std::string AgentBrain::getNeuronTypeString(NeuronType type) const {
    switch (type) {
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Interneuron: return "Interneuron";
        default: return "Unknown";
    }
}

size_t AgentBrain::getWeightedRandomIndex(float priority) const {
    // Simple weighted random distribution
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    float rand = dist(rng);
    float threshold = std::min(priority, 2.0f) / 2.0f;
    
    if (rand < threshold) {
        return 0;
    } else {
        return 1;
    }
}

void AgentBrain::initializeAdvancedSystems() {
    NLM_LOG_DEBUG("Initializing advanced systems");
    
    // Initialize neuromodulation levels
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.5f;
    norepinephrineLevel_ = 0.5f;
    acetylcholineLevel_ = 0.5f;
    
    // Initialize memory system
    memoryTraces_.clear();
    rewardPredictions_.clear();
    rewardPredictionErrors_.clear();
    observedBehaviors_.clear();
    
    // Initialize stress and fatigue systems
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    cognitiveLoad_ = 0.0f;
    
    // Initialize motivational systems
    homeostaticState_ = 1.0f;
    motivationLevel_ = 0.5f;
    intrinsicMotivation_ = 0.5f;
    
    // Initialize sleep system
    isSleeping_ = false;
    sleepStartTime_ = 0.0;
    
    // Initialize emergency system
    inEmergency_ = false;
    
    NLM_LOG_DEBUG("Advanced systems initialized");
}

AgentBrain::~AgentBrain() {
    NLM_LOG_DEBUG("Destroying Advanced AgentBrain");
    
    // Clear all neuron pointers
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
    
    previousVision_.clear();
    memoryTraces_.clear();
    rewardPredictions_.clear();
    rewardPredictionErrors_.clear();
    observedBehaviors_.clear();
    
    NLM_LOG_DEBUG("Advanced AgentBrain destroyed successfully");
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input - brain pointer is null");
        return;
    }
    
    if (!isValidPercept( percept)) {
        NLM_LOG_ERROR("Received invalid sensory percept");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    NLM_LOG_DEBUG("Processing vision input: " + std::to_string(vision.size()) + " values");
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (!sensoryVision_[i]) {
            NLM_LOG_ERROR("Null pointer in sensoryVision_ at index " + std::to_string(i));
            continue;
        }
        
        // Advanced processing: feature extraction
        float feature = extractSensoryFeature(vision[i], i);
        
        // Apply feature filtering
        float filteredFeature = applyFeatureFilter(feature, i);
        
        // Inject current proportional to feature intensity
        float current = filteredFeature * 5.0f;
        sensoryVision_[i]->injectCurrent(current);
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    NLM_LOG_DEBUG("Processing touch input: " + std::to_string(touch.size()) + " values");
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (!sensoryTouch_[i]) {
            NLM_LOG_ERROR("Null pointer in sensoryTouch_ at index " + std::to_string(i));
            continue;
        }
        
        float current = touch[i] * 8.0f;
        sensoryTouch_[i]->injectCurrent(current);
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    NLM_LOG_DEBUG("Processing internal input: " + std::to_string(intern.size()) + " values");
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (!sensoryInternal_[i]) {
            NLM_LOG_ERROR("Null pointer in sensoryInternal_ at index " + std::to_string(i));
            continue;
        }
        
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
        sensoryInternal_[i]->injectCurrent(current);
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    NLM_LOG_DEBUG("Processing proprioception input: " + std::to_string(proprio.size()) + " values");
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (!sensoryProprioception_[i]) {
            NLM_LOG_ERROR("Null pointer in sensoryProprioception_ at index " + std::to_string(i));
            continue;
        }
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        sensoryProprioception_[i]->injectCurrent(current);
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply nonlinear transformation
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ = noveltyLevel_ * noveltyLevel_; // Emphasize larger changes
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
        
        // Update intrinsic motivation
        updateIntrinsicMotivation();
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update cognitive load
    updateCognitiveLoad(vision.size(), touch.size(), intern.size(), proprio.size());
    
    // Update stress level
    updateStressLevel(estimatedEnvironmentDifficulty(vision, touch, intern, proprio));
    
    NLM_LOG_DEBUG("Sensory processing completed - novelty: " + std::to_string(noveltyLevel_) +
                 " curiosity: " + std::to_string(curiosityLevel_) +
                 " cognitive load: " + std::to_string(cognitiveLoad_));
}

float AgentBrain::extractSensoryFeature(float rawValue, size_t neuronIndex) const {
    // Advanced feature extraction with nonlinear transformations
    float feature = rawValue;
    
    // Apply sigmoidal transformation to compress range
    feature = 1.0f / (1.0f + std::exp(-feature * 2.0f + 1.0f)) - 0.5f;
    
    // Add noise for biological realism
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::normal_distribution<float> noise(0.0f, 0.01f);
    feature += noise(rng);
    
    // Apply neuron-specific tuning
    float tuning = 1.0f + std::sin(neuronIndex * 0.1f) * 0.2f;
    feature *= tuning;
    
    return std::clamp(feature, -1.0f, 1.0f);
}

float AgentBrain::applyFeatureFilter(float feature, size_t neuronIndex) const {
    // Advanced feature filtering
    float filtered = feature;
    
    // Temporal filtering (simple moving average)
    static std::vector<float> featureHistory(5, 0.0f);
    featureHistory.erase(featureHistory.begin());
    featureHistory.push_back(feature);
    
    float temporalSum = std::accumulate(featureHistory.begin(), featureHistory.end(), 0.0f);
    filtered = temporalSum / featureHistory.size();
    
    // Frequency filtering (sine wave modulation)
    float frequency = 0.5f + 0.5f * std::sin(neuronIndex * 0.2f);
    filtered = filtered * (1.0f - frequency) + filtered * frequency * 0.5f;
    
    return std::clamp(filtered, -1.0f, 1.0f);
}

float AgentBrain::computeSensorySurprise(const SensoryPercept& percept) const {
    const auto& vision = percept.getVision();
    if (vision.empty()) {
        return 0.0f;
    }
    
    // Compute surprise as negative log probability of observation
    // Using a simple entropy-based approach
    std::vector<float> histogram(10, 0.0f); // Simplified histogram
    for (float val : vision) {
        int bin = static_cast<int>((val + 1.0f) * 4.5f); // Map [-1,1] to [0,9]
        if (bin >= 0 && bin < 10) {
            histogram[bin]++;
        }
    }
    
    // Compute entropy
    float entropy = 0.0f;
    for (float count : histogram) {
        if (count > 0) {
            float probability = count / vision.size();
            entropy -= probability * std::log(probability);
        }
    }
    
    // Surprise is normalized entropy
    return entropy / std::log(10.0f);
}

void AgentBrain::updateIntrinsicMotivation() {
    // Update intrinsic motivation based on novelty and curiosity
    double currentTime = getSimulationTime();
    
    if (currentTime - lastIntrinsicUpdateTime_ > 1.0) {
        // Decay intrinsic motivation
        intrinsicMotivation_ *= 0.95f;
        
        // Boost intrinsic motivation based on novelty
        intrinsicMotivation_ += noveltyLevel_ * 0.1f;
        
        // Cap intrinsic motivation
        intrinsicMotivation_ = std::clamp(intrinsicMotivation_, 0.0f, 1.0f);
        
        // Update motivation level based on intrinsic motivation
        motivationLevel_ = (motivationLevel_ + intrinsicMotivation_) * 0.5f;
        
        lastIntrinsicUpdateTime_ = currentTime;
        
        NLM_LOG_DEBUG("Intrinsic motivation updated: " + std::to_string(intrinsicMotivation_));
    }
}

void AgentBrain::updateCognitiveLoad(size_t visionSize, size_t touchSize, 
                                    size_t internalSize, size_t proprioSize) const {
    // Estimate cognitive load based on sensory input size and complexity
    float inputComplexity = (visionSize * 0.4f + touchSize * 0.2f + 
                            internalSize * 0.2f + proprioSize * 0.2f) / 256.0f;
    
    // Apply nonlinear transformation
    cognitiveLoad_ = inputComplexity * inputComplexity * 2.0f;
    
    // Update stress level based on cognitive load
    stressLevel_ += cognitiveLoad_ * 0.1f;
    stressLevel_ = std::clamp(stressLevel_, 0.0f, 1.0f);
    
    // Apply fatigue effects
    applyFatigueEffects();
}

void AgentBrain::applyFatigueEffects() {
    // Apply fatigue based on prolonged activity
    fatigueLevel_ += 0.001f;
    
    // Recovery when not active
    fatigueLevel_ *= 0.995f;
    
    // Fatigue affects cognitive load
    cognitiveLoad_ += fatigueLevel_ * 0.5f;
    
    // Cap cognitive load
    cognitiveLoad_ = std::min(cognitiveLoad_, 1.0f);
}

void AgentBrain::updateStressLevel(float environmentDifficulty) {
    // Update stress level based on environmental difficulty
    float stressIncrement = environmentDifficulty * 0.1f;
    
    if (stressIncrement > 0.0f) {
        stressLevel_ += stressIncrement;
    } else {
        stressLevel_ *= 0.95f; // Recover from stress
    }
    
    // Clamp stress level
    stressLevel_ = std::clamp(stressLevel_, 0.0f, 1.0f);
    
    // Stress affects neuromodulation
    updateNeuromodulationFromStress();
}

void AgentBrain::updateNeuromodulationFromStress() {
    // Adjust neuromodulation based on stress level
    
    // Serotonin: stress reduces serotonin
    serotoninLevel_ = 0.5f * (1.0f - stressLevel_);
    
    // Dopamine: moderate stress increases dopamine (alertness)
    dopamineLevel_ = stressLevel_ * 0.8f + 0.1f;
    
    // Norepinephrine: stress increases norepinephrine (arousal)
    norepinephrineLevel_ = stressLevel_ * 0.7f + 0.2f;
    
    // Acetylcholine: stress reduces acetylcholine (attention)
    acetylcholineLevel_ = 0.5f * (1.0f - stressLevel_ * 0.5f);
    
    // Clamp levels
    serotoninLevel_ = std::clamp(serotoninLevel_, 0.0f, 1.0f);
    dopamineLevel_ = std::clamp(dopamineLevel_, 0.0f, 1.0f);
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
}

double AgentBrain::getSimulationTime() const {
    return developmentalAge_;
}

float AgentBrain::estimatedEnvironmentDifficulty(const std::vector<float>& vision,
                                               const std::vector<float>& touch,
                                               const std::vector<float>& internal,
                                               const std::vector<float>& proprio) const {
    // Estimate environmental difficulty based on input statistics
    float totalComplexity = 0.0f;
    
    if (!vision.empty()) {
        float mean = stats::mean(vision);
        float variance = stats::variance(vision, mean);
        totalComplexity += variance;
    }
    
    if (!touch.empty()) {
        float mean = stats::mean(touch);
        float variance = stats::variance(touch, mean);
        totalComplexity += variance;
    }
    
    if (!internal.empty()) {
        float mean = stats::mean(internal);
        float variance = stats::variance(internal, mean);
        totalComplexity += variance;
    }
    
    if (!proprio.empty()) {
        float mean = stats::mean(proprio);
        float variance = stats::variance(proprio, mean);
        totalComplexity += variance;
    }
    
    return std::min(totalComplexity / 4.0f, 1.0f);
}

void AgentBrain::initializeAdvancedSystems() {
    NLM_LOG_DEBUG("Initializing advanced neuromodulation systems");
    
    // Initialize neuromodulation levels
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.5f;
    norepinephrineLevel_ = 0.5f;
    acetylcholineLevel_ = 0.5f;
    
    // Initialize memory system
    memoryTraces_.clear();
    rewardPredictions_.clear();
    rewardPredictionErrors_.clear();
    observedBehaviors_.clear();
    
    // Initialize stress and fatigue systems
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    cognitiveLoad_ = 0.0f;
    
    // Initialize motivational systems
    homeostaticState_ = 1.0f;
    motivationLevel_ = 0.5f;
    intrinsicMotivation_ = 0.5f;
    
    // Initialize sleep system
    isSleeping_ = false;
    sleepStartTime_ = 0.0;
    
    // Initialize emergency system
    inEmergency_ = false;
    
    NLM_LOG_DEBUG("Advanced neuromodulation systems initialized");
}

bool AgentBrain::isValidPercept(const SensoryPercept& percept) const {
    // Check percept dimensions against expected sizes
    bool isValid = true;
    
    // Check vision dimensions
    auto vision = percept.getVision();
    if (!vision.empty()) {
        auto visionWidth = percept.getVisionWidth();
        auto visionHeight = percept.getVisionHeight();
        size_t expectedVisionSize = visionWidth * visionHeight;
        if (vision.size() != expectedVisionSize) {
            NLM_LOG_WARNING("Vision size mismatch: expected " + std::to_string(expectedVisionSize) +
                           " but got " + std::to_string(vision.size()));
            isValid = false;
        }
    }
    
    // Check touch size
    auto touch = percept.getTouch();
    if (touch.size() > 20) {
        NLM_LOG_WARNING("Touch array too large: " + std::to_string(touch.size()));
        isValid = false;
    }
    
    // Check internal size
    auto internal = percept.getInternal();
    if (internal.size() > 10) {
        NLM_LOG_WARNING("Internal array too large: " + std::to_string(internal.size()));
        isValid = false;
    }
    
    // Check proprioception size
    auto proprioception = percept.getProprioception();
    if (proprioception.size() > 20) {
        NLM_LOG_WARNING("Proprioception array too large: " + std::to_string(proprioception.size()));
        isValid = false;
    }
    
    return isValid;
}

void AgentBrain::decaySensoryNovelty() {
    if (sensoryNoveltyDecay_ > 0.0f) {
        noveltyLevel_ *= sensoryNoveltyDecay_;
        NLM_LOG_DEBUG("Sensory novelty decayed to: " + std::to_string(noveltyLevel_));
    }
}

AgentBrain::SensoryNeuronCounts AgentBrain::getSensoryNeuronCounts() const {
    SensoryNeuronCounts counts;
    counts.vision = sensoryVision_.size();
    counts.touch = sensoryTouch_.size();
    counts.internal = sensoryInternal_.size();
    counts.proprioception = sensoryProprioception_.size();
    return counts;
}

AgentBrain::AgentStats AgentBrain::getStats() const {
    AgentStats stats;
    stats.totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    stats.totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
    stats.currentCuriosity = curiosityLevel_;
    stats.currentNovelty = noveltyLevel_;
    stats.simulationAge = developmentalAge_;
    return stats;
}

void AgentBrain::applyDopamineModulation(float reward, float predictionError) {
    // Advanced dopamine-based reinforcement learning
    float learningRate = 0.01f * plasticityModifier_;
    
    // Prediction error learning
    float weightChange = learningRate * predictionError;
    
    // Apply dopamine modulation to synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * weightChange;
                syn->addToWeight(delta);
            }
        }
    }
    
    NLM_LOG_DEBUG("Applied dopamine modulation: reward=" + std::to_string(reward) +
                 " prediction error=" + std::to_string(predictionError) +
                 " weight change=" + std::to_string(weightChange));
}

void AgentBrain::applySerotoninModulation(float stressLevel) {
    // Advanced serotonin-based behavioral inhibition
    float inhibitionStrength = stressLevel * 0.8f;
    
    // Apply inhibition to motor output
    float motorInhibition = 1.0f - inhibitionStrength;
    motorInhibition = std::max(0.0f, motorInhibition);
    
    NLM_LOG_DEBUG("Applied serotonin modulation: stress=" + std::to_string(stressLevel) +
                 " motor inhibition=" + std::to_string(motorInhibition));
}

void AgentBrain::applyAcetylcholineModulation(float noveltyLevel) {
    // Advanced acetylcholine-based attention
    float attentionBoost = noveltyLevel * 0.5f;
    
    // Enhance processing of novel stimuli
    for (auto* neuron : sensoryVision_) {
        if (neuron) {
            float attention = 1.0f + attentionBoost;
            neuron->setAttentionalGain(attention);
        }
    }
    
    NLM_LOG_DEBUG("Applied acetylcholine modulation: novelty=" + std::to_string(noveltyLevel) +
                 " attention boost=" + std::to_string(attentionBoost));
}

void AgentBrain::consolidateMemory() {
    // Memory consolidation with temporal compression
    double currentTime = getSimulationTime();
    
    if (currentTime - lastConsolidationTime_ > 60.0) { // Consolidate every minute
        NLM_LOG_DEBUG("Starting memory consolidation");
        
        // Transfer recent memories to long-term storage
        for (auto& trace : memoryTraces_) {
            // Apply consolidation rules
            trace = consolidateTrace(trace);
        }
        
        // Update reward prediction model
        updateRewardPredictionModel();
        
        // Update observational learning
        updateObservationalLearning();
        
        lastConsolidationTime_ = currentTime;
        
        NLM_LOG_DEBUG("Memory consolidation completed");
    }
}

std::vector<float> AgentBrain::consolidateTrace(const std::vector<float>& trace) {
    // Advanced memory consolidation with synaptic consolidation
    std::vector<float> consolidated = trace;
    
    // Apply synaptic consolidation rules
    for (float& value : consolidated) {
        // Decay weak memories
        value *= 0.9f;
        
        // Strengthen strong memories
        if (std::abs(value) > 0.5f) {
            value = std::clamp(value * 1.1f, -1.0f, 1.0f);
        }
    }
    
    return consolidated;
}

void AgentBrain::updateMemoryTraces() {
    // Update memory traces with new experiences
    double currentTime = getSimulationTime();
    
    // Add new experiences to memory
    if (currentTime - lastConsolidationTime_ > 0.1) {
        // Create new memory trace from current experience
        std::vector<float> newTrace = createMemoryTrace();
        memoryTraces_.push_back(newTrace);
        
        // Keep memory trace history manageable
        if (memoryTraces_.size() > 1000) {
            memoryTraces_.erase(memoryTraces_.begin());
        }
        
        lastConsolidationTime_ = currentTime;
    }
}

std::vector<float> AgentBrain::createMemoryTrace() const {
    // Create new memory trace from current sensory input
    std::vector<float> trace;
    
    // Extract features from current state
    trace.push_back(curiosityLevel_);
    trace.push_back(noveltyLevel_);
    trace.push_back(predictionError_);
    trace.push_back(stressLevel_);
    trace.push_back(fatigueLevel_);
    trace.push_back(cognitiveLoad_);
    trace.push_back(homeostaticState_);
    trace.push_back(motivationLevel_);
    
    // Add environmental context
    trace.push_back(estimatedEnvironmentDifficulty({}, {}, {}, {}));
    
    // Add recent rewards
    if (!rewardPredictions_.empty()) {
        float recentReward = rewardPredictions_.back();
        trace.push_back(recentReward);
    }
    
    return trace;
}

void AgentBrain::observeOtherAgent(const AgentBrain& otherAgent) {
    // Observe another agent's behavior and learn from it
    double currentTime = getSimulationTime();
    
    if (currentTime - lastObservationTime_ > 1.0) {
        NLM_LOG_DEBUG("Observing other agent");
        
        // Record observed behaviors
        auto otherStats = otherAgent.getStats();
        
        // Learn from observed behaviors
        learnFromObservation(otherStats);
        
        // Update observational memory
        observedBehaviors_.push_back(MotorCommand::Wait); // Placeholder
        
        if (observedBehaviors_.size() > 100) {
            observedBehaviors_.erase(observedBehaviors_.begin());
        }
        
        lastObservationTime_ = currentTime;
    }
}

void AgentBrain::learnFromObservation(const AgentStats& otherStats) {
    // Learn from observed agent's statistics
    if (enableSocialLearning_) {
        // Adjust curiosity based on observed agent's behavior
        curiosityLevel_ = (curiosityLevel_ + otherStats.currentCuriosity) * 0.5f;
        
        // Adjust novelty detection
        noveltyLevel_ = (noveltyLevel_ + otherStats.currentNovelty) * 0.5f;
        
        NLM_LOG_DEBUG("Learned from observation: curiosity=" + std::to_string(curiosityLevel_) +
                     " novelty=" + std::to_string(noveltyLevel_));
    }
}

float AgentBrain::computeExplorationBonus() const {
    // Compute exploration bonus based on uncertainty
    float uncertainty = 1.0f - learningEfficiency_;
    
    // Boost exploration when uncertain
    float explorationBonus = uncertainty * intrinsicMotivation_ * curiosityLevel_;
    
    return std::clamp(explorationBonus, 0.0f, 1.0f);
}

MotorCommand AgentBrain::applyCuriosityExploration(const MotorCommand& defaultCommand) {
    // Apply curiosity-driven exploration
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f; // Up to 30% random
        
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        float r = dist(rng);
        if (r < exploreChance) {
            // Random exploration
            int choice = std::uniform_int_distribution<int>(0, 7)(rng);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCommand;
}

void AgentBrain::imitateBehavior(const MotorCommand& command) {
    // Imitate observed behavior
    if (enableSocialLearning_) {
        NLM_LOG_DEBUG("Imitating behavior: " + std::to_string(static_cast<int>(command)));
        
        // Record imitated behavior
        observedBehaviors_.push_back(command);
        
        if (observedBehaviors_.size() > 50) {
            observedBehaviors_.erase(observedBehaviors_.begin());
        }
    }
}

void AgentBrain::initializeAdvancedNeuromodulation() {
    NLM_LOG_DEBUG("Initializing advanced neuromodulation systems");
    
    // Initialize neuromodulation levels
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.5f;
    norepinephrineLevel_ = 0.5f;
    acetylcholineLevel_ = 0.5f;
    
    // Initialize memory system
    memoryTraces_.clear();
    rewardPredictions_.clear();
    rewardPredictionErrors_.clear();
    observedBehaviors_.clear();
    
    // Initialize stress and fatigue systems
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    cognitiveLoad_ = 0.0f;
    
    // Initialize motivational systems
    homeostaticState_ = 1.0f;
    motivationLevel_ = 0.5f;
    intrinsicMotivation_ = 0.5f;
    
    // Initialize sleep system
    isSleeping_ = false;
    sleepStartTime_ = 0.0;
    
    // Initialize emergency system
    inEmergency_ = false;
    
    NLM_LOG_DEBUG("Advanced neuromodulation systems initialized");
}

void AgentBrain::reset() {
    NLM_LOG_INFO("Resetting Advanced AgentBrain for new episode");
    
    // Reset neuromodulation
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.5f;
    norepinephrineLevel_ = 0.5f;
    acetylcholineLevel_ = 0.5f;
    
    // Reset memory system
    memoryTraces_.clear();
    rewardPredictions_.clear();
    rewardPredictionErrors_.clear();
    observedBehaviors_.clear();
    
    // Reset stress and fatigue
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    
    // Reset cognitive and motivational systems
    cognitiveLoad_ = 0.0f;
    learningEfficiency_ = 1.0f;
    homeostaticState_ = 1.0f;
    motivationLevel_ = 0.5f;
    intrinsicMotivation_ = 0.5f;
    
    // Reset sleep system
    isSleeping_ = false;
    
    // Reset emergency system
    inEmergency_ = false;
    
    // Reset development
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset configuration
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Clear sensory state
    previousVision_.clear();
    
    NLM_LOG_DEBUG("Advanced AgentBrain reset completed");
}

// Implementation of missing methods
void AgentBrain::applyDopamineModulation(float reward, float predictionError) {
    // Simplified implementation
    float learningRate = 0.01f * plasticityModifier_;
    float weightChange = learningRate * predictionError;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * weightChange;
                syn->addToWeight(delta);
            }
        }
    }
}

void AgentBrain::applySerotoninModulation(float stressLevel) {
    float inhibitionStrength = stressLevel * 0.8f;
    float motorInhibition = 1.0f - inhibitionStrength;
    motorInhibition = std::max(0.0f, motorInhibition);
}

void AgentBrain::applyAcetylcholineModulation(float noveltyLevel) {
    float attentionBoost = noveltyLevel * 0.5f;
    for (auto* neuron : sensoryVision_) {
        if (neuron) {
            float attention = 1.0f + attentionBoost;
            neuron->setAttentionalGain(attention);
        }
    }
}

void AgentBrain::consolidateMemory() {
    // Simplified implementation
    NLM_LOG_DEBUG("Consolidating memory");
}

void AgentBrain::updateMemoryTraces() {
    // Simplified implementation
}

void AgentBrain::observeOtherAgent(const AgentBrain& otherAgent) {
    // Simplified implementation
    if (enableSocialLearning_) {
        auto otherStats = otherAgent.getStats();
        learnFromObservation(otherStats);
    }
}

void AgentBrain::learnFromObservation(const AgentStats& otherStats) {
    if (enableSocialLearning_) {
        curiosityLevel_ = (curiosityLevel_ + otherStats.currentCuriosity) * 0.5f;
        noveltyLevel_ = (noveltyLevel_ + otherStats.currentNovelty) * 0.5f;
    }
}

float AgentBrain::computeExplorationBonus() const {
    float uncertainty = 1.0f - learningEfficiency_;
    return uncertainty * intrinsicMotivation_ * curiosityLevel_;
}

MotorCommand AgentBrain::applyCuriosityExploration(const MotorCommand& defaultCommand) {
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        float r = dist(rng);
        if (r < exploreChance) {
            int choice = std::uniform_int_distribution<int>(0, 7)(rng);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    return defaultCommand;
}

void AgentBrain::imitateBehavior(const MotorCommand& command) {
    if (enableSocialLearning_) {
        observedBehaviors_.push_back(command);
        if (observedBehaviors_.size() > 50) {
            observedBehaviors_.erase(observedBehaviors_.begin());
        }
    }
}

void AgentBrain::updateStressLevel(float environmentDifficulty) {
    float stressIncrement = environmentDifficulty * 0.1f;
    if (stressIncrement > 0.0f) {
        stressLevel_ += stressIncrement;
    } else {
        stressLevel_ *= 0.95f;
    }
    stressLevel_ = std::clamp(stressLevel_, 0.0f, 1.0f);
    updateNeuromodulationFromStress();
}

void AgentBrain::updateNeuromodulationFromStress() {
    serotoninLevel_ = 0.5f * (1.0f - stressLevel_);
    dopamineLevel_ = stressLevel_ * 0.8f + 0.1f;
    norepinephrineLevel_ = stressLevel_ * 0.7f + 0.2f;
    acetylcholineLevel_ = 0.5f * (1.0f - stressLevel_ * 0.5f);
    
    serotoninLevel_ = std::clamp(serotoninLevel_, 0.0f, 1.0f);
    dopamineLevel_ = std::clamp(dopamineLevel_, 0.0f, 1.0f);
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
}

void AgentBrain::updateIntrinsicMotivation() {
    double currentTime = getSimulationTime();
    
    if (currentTime - lastIntrinsicUpdateTime_ > 1.0) {
        intrinsicMotivation_ *= 0.95f;
        intrinsicMotivation_ += noveltyLevel_ * 0.1f;
        intrinsicMotivation_ = std::clamp(intrinsicMotivation_, 0.0f, 1.0f);
        motivationLevel_ = (motivationLevel_ + intrinsicMotivation_) * 0.5f;
        lastIntrinsicUpdateTime_ = currentTime;
    }
}

void AgentBrain::updateCognitiveLoad(size_t visionSize, size_t touchSize, 
                                    size_t internalSize, size_t proprioSize) const {
    float inputComplexity = (visionSize * 0.4f + touchSize * 0.2f + 
                            internalSize * 0.2f + proprioSize * 0.2f) / 256.0f;
    cognitiveLoad_ = inputComplexity * inputComplexity * 2.0f;
    stressLevel_ += cognitiveLoad_ * 0.1f;
    stressLevel_ = std::clamp(stressLevel_, 0.0f, 1.0f);
}

void AgentBrain::applyFatigueEffects() {
    fatigueLevel_ += 0.001f;
    fatigueLevel_ *= 0.995f;
    cognitiveLoad_ += fatigueLevel_ * 0.5f;
    cognitiveLoad_ = std::min(cognitiveLoad_, 1.0f);
}

void AgentBrain::enterSleepCycle() {
    if (!isSleeping_) {
        isSleeping_ = true;
        sleepStartTime_ = getSimulationTime();
    }
}

void AgentBrain::wakeUp() {
    if (isSleeping_) {
        isSleeping_ = false;
        developmentalAge_ += 60.0;
        applySleepEffects();
    }
}

void AgentBrain::applySleepEffects() {
    for (auto& trace : memoryTraces_) {
        for (float& value : trace) {
            value *= 1.05f;
        }
    }
    stressLevel_ *= 0.5f;
    fatigueLevel_ *= 0.5f;
    cognitiveLoad_ *= 0.5f;
}

void AgentBrain::metaLearn() {
    adaptLearningRate(getLearningEfficiency());
    updateRewardPredictionModel();
    optimizeMemoryConsolidation();
}

void AgentBrain::adaptLearningRate(float performance) {
    float currentLearningRate = 0.01f;
    if (performance > 0.8f) {
        currentLearningRate *= 0.8f;
    } else if (performance < 0.3f) {
        currentLearningRate *= 1.5f;
    }
    plasticityModifier_ = std::clamp(currentLearningRate, 0.1f, 2.0f);
}

float AgentBrain::getLearningEfficiency() const {
    float rewardPredictionAccuracy = computeRewardPredictionAccuracy();
    float memoryRetention = computeMemoryRetention();
    float behavioralAdaptation = computeBehavioralAdaptation();
    return (rewardPredictionAccuracy + memoryRetention + behavioralAdaptation) / 3.0f;
}

float AgentBrain::computeRewardPredictionAccuracy() const {
    if (rewardPredictions_.empty()) return 0.5f;
    float totalError = 0.0f;
    size_t count = 0;
    for (size_t i = 0; i < rewardPredictions_.size() - 1; ++i) {
        float predicted = rewardPredictions_[i];
        float actual = rewardPredictions_[i + 1];
        totalError += std::abs(predicted - actual);
        count++;
    }
    if (count == 0) return 0.5f;
    float meanError = totalError / count;
    return std::max(0.0f, 1.0f - meanError);
}

float AgentBrain::computeMemoryRetention() const {
    if (memoryTraces_.empty()) return 0.5f;
    float totalStability = 0.0f;
    for (const auto& trace : memoryTraces_) {
        float stability = 0.0f;
        for (float value : trace) {
            stability += std::abs(value);
        }
        stability /= trace.size();
        totalStability += stability;
    }
    return totalStability / memoryTraces_.size();
}

float AgentBrain::computeBehavioralAdaptation() const {
    return std::clamp(motivationLevel_ * curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::updateRewardPredictionModel() {
    if (!rewardPredictions_.empty()) {
        rewardPredictions_.erase(rewardPredictions_.begin());
    }
    rewardPredictions_.push_back(expectedReward_);
    if (rewardPredictions_.size() > 50) {
        rewardPredictions_.erase(rewardPredictions_.begin());
    }
}

void AgentBrain::optimizeMemoryConsolidation() {
    double memoryLoad = memoryTraces_.size();
    if (memoryLoad > 500) {
        // Consolidate more aggressively
    } else if (memoryLoad < 100) {
        // Consolidate less frequently
    }
}

void AgentBrain::triggerEmergencyResponse() {
    if (enableEmergencyResponses_) {
        inEmergency_ = true;
        applyEmergencyResponse();
    }
}

void AgentBrain::applyEmergencyResponse() {
    dopamineLevel_ = 1.0f;
    norepinephrineLevel_ = 1.0f;
    acetylcholineLevel_ = 0.5f;
    motivationLevel_ = 1.0f;
    stressLevel_ = 0.0f;
}

void AgentBrain::recoverFromEmergency() {
    if (inEmergency_) {
        inEmergency_ = false;
        dopamineLevel_ *= 0.8f;
        norepinephrineLevel_ *= 0.8f;
        acetylcholineLevel_ *= 0.9f;
        motivationLevel_ *= 0.9f;
    }
}

void AgentBrain::maintainHomeostasis() {
    if (homeostaticState_ > 1.0f) {
        homeostaticState_ = 1.0f;
    } else if (homeostaticState_ < 0.5f) {
        homeostaticState_ = 0.5f;
    }
    applyHomeostaticPressure();
}

void AgentBrain::applyHomeostaticPressure() {
    float pressure = 1.0f - homeostaticState_;
    motivationLevel_ += pressure * 0.01f;
    motivationLevel_ = std::clamp(motivationLevel_, 0.0f, 1.0f);
    cognitiveLoad_ += pressure * 0.005f;
    cognitiveLoad_ = std::clamp(cognitiveLoad_, 0.0f, 1.0f);
}

float AgentBrain::generateIntrinsicMotivation() const {
    float explorationDrive = curiosityLevel_ * 0.5f;
    float achievementDrive = learningEfficiency_ * 0.3f;
    float socialDrive = enableSocialLearning_ ? 0.2f : 0.0f;
    return explorationDrive + achievementDrive + socialDrive;
}

void AgentBrain::applyIntrinsicMotivation() {
    curiosityLevel_ += intrinsicMotivation_ * 0.1f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    learningEfficiency_ += intrinsicMotivation_ * 0.05f;
    learningEfficiency_ = std::clamp(learningEfficiency_, 0.5f, 2.0f);
}

// Getters for advanced features
float AgentBrain::getStressLevel() const { return stressLevel_; }
float AgentBrain::getCognitiveLoad() const { return cognitiveLoad_; }
bool AgentBrain::isSleeping() const { return isSleeping_; }
float AgentBrain::getLearningEfficiency() const { return learningEfficiency_; }
float AgentBrain::getRewardPredictionError() const { return predictionError_; }
float AgentBrain::getHomeostaticState() const { return homeostaticState_; }
float AgentBrain::getMotivationLevel() const { return motivationLevel_; }
bool AgentBrain::isInEmergency() const { return inEmergency_; }

} // namespace nlm
