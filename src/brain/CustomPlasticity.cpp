#pragma once

#include "CustomPlasticity.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== BASE CUSTOM PLASTICITY RULE ==========

CustomPlasticityRule::CustomPlasticityRule(float learningRate, float maxWeight, float minWeight)
    : learningRate_(learningRate), maxWeight_(maxWeight), minWeight_(minWeight),
      eligibilityTrace_(0.0f), lastLearningSignal_(0.0f) {
}

void CustomPlasticityRule::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) return;
    
    // Calculate eligibility trace based on spike activity
    updateEligibilityTrace(preSpikes, postSpikes, dt);
    
    // Store last learning signal for next step
    lastLearningSignal_ = learningRate_ * eligibilityTrace_;
}

void CustomPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply soft bounds to weight change
    float boundedDelta = delta;
    if (softBound_) {
        float currentWeight = synapse->getWeight();
        float maxPossibleIncrease = maxWeight_ - currentWeight;
        float maxPossibleDecrease = currentWeight - minWeight_;
        
        if (delta > 0) {
            boundedDelta = std::min(delta, maxPossibleIncrease);
        } else if (delta < 0) {
            boundedDelta = std::max(delta, -maxPossibleDecrease);
        }
    }
    
    // Apply the weight change
    synapse->addToWeight(boundedDelta);
    
    LOG_DEBUG("Applied custom plasticity weight change: delta={}, new_weight={}", 
              boundedDelta, synapse->getWeight());
}

const char* CustomPlasticityRule::getName() const {
    return "CustomPlasticity";
}

void CustomPlasticityRule::resetEligibilityTrace() {
    eligibilityTrace_ = 0.0f;
}

void CustomPlasticityRule::setLearningRate(float rate) {
    learningRate_ = rate;
}

void CustomPlasticityRule::setWeightBounds(float max, float min) {
    maxWeight_ = max;
    minWeight_ = min;
}

void CustomPlasticityRule::enableSoftBounds(bool enable) {
    softBound_ = enable;
}

void CustomPlasticityRule::setLearningSignal(float signal) {
    lastLearningSignal_ = signal;
}

float CustomPlasticityRule::getEligibilityTrace() const {
    return eligibilityTrace_;
}

float CustomPlasticityRule::getLastLearningSignal() const {
    return lastLearningSignal_;
}

void CustomPlasticityRule::updateEligibilityTrace(const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 TimestepDuration dt) {
    float traceUpdate = 0.0f;
    
    // Increment trace based on post-synaptic spikes
    if (!postSpikes.empty()) {
        traceUpdate += postSynapticIncrement_;
    }
    
    // Decay trace
    eligibilityTrace_ *= eligibilityDecay_;
    
    // Add spike-based update
    eligibilityTrace_ += traceUpdate;
    
    // Clamp to reasonable range
    eligibilityTrace_ = std::max(0.0f, std::min(eligibilityTrace_, 1.0f));
}

// ========== CUSTOM STDP IMPLEMENTATION ==========

CustomSTDPPlasticity::CustomSTDPPlasticity(float a_plus, float a_minus, float tau_plus, float tau_minus,
                                          float w_max, float w_min)
    : CustomPlasticityRule(0.01f, w_max, w_min), a_plus_(a_plus), a_minus_(a_minus),
      tau_plus_(tau_plus), tau_minus_(tau_minus) {
    setLearningRate(0.01f);
    enableSoftBounds(true);
    
    LOG_INFO("Created CustomSTDPPlasticity: a_plus={}, a_minus={}, tau_plus={}, tau_minus={}", 
             a_plus, a_minus, tau_plus, tau_minus);
}

void CustomSTDPPlasticity::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0f) {
        CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    // Calculate spike time differences
    float lastPreSpike = preSpikes.back();
    float lastPostSpike = postSpikes.back();
    float delta_t = lastPostSpike - lastPreSpike;
    
    // Calculate weight change based on spike timing
    SynapticWeight delta = 0.0f;
    
    if (delta_t > 0) {
        // Post before pre: Long-term depression (LTD)
        delta = -a_minus_ * std::exp(-delta_t / tau_minus_);
        LOG_DEBUG("LTD: delta_t={}, delta={}", delta_t, delta);
    } else if (delta_t < 0) {
        // Pre before post: Long-term potentiation (LTP)
        delta = a_plus_ * std::exp(delta_t / tau_plus_);
        LOG_DEBUG("LTP: delta_t={}, delta={}", delta_t, delta);
    }
    
    // Apply the learning rule
    CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
    CustomPlasticityRule::applyWeightChange(synapse, delta);
}

const char* CustomSTDPPlasticity::getName() const {
    return "CustomSTDP";
}

void CustomSTDPPlasticity::setParameters(float a_plus, float a_minus, float tau_plus, float tau_minus) {
    a_plus_ = a_plus;
    a_minus_ = a_minus;
    tau_plus_ = tau_plus;
    tau_minus_ = tau_minus;
    LOG_INFO("Updated CustomSTDP parameters: a_plus={}, a_minus={}, tau_plus={}, tau_minus={}", 
             a_plus, a_minus, tau_plus, tau_minus);
}

void CustomSTDPPlasticity::setTimeConstants(float tau_plus, float tau_minus) {
    tau_plus_ = tau_plus;
    tau_minus_ = tau_minus;
}

// ========== REWARD-MODULATED PLASTICITY ==========

RewardModulatedPlasticity::RewardModulatedPlasticity(float learningRate, float eligibilityDecay,
                                                    float rewardGain, float errorGain)
    : CustomPlasticityRule(learningRate, 1.0f, -1.0f), eligibilityDecay_(eligibilityDecay),
      rewardGain_(rewardGain), errorGain_(errorGain), predictionError_(0.0f) {
    enableSoftBounds(true);
    
    LOG_INFO("Created RewardModulatedPlasticity: learningRate={}, eligibilityDecay={}, rewardGain={}, errorGain={}", 
             learningRate, eligibilityDecay, rewardGain, errorGain);
}

void RewardModulatedPlasticity::update(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    // Update eligibility trace
    CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
    
    // Apply reward modulation
    applyRewardModulation(synapse);
}

void RewardModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply reward-modulated weight change
    CustomPlasticityRule::applyWeightChange(synapse, delta * getModulationFactor());
}

const char* RewardModulatedPlasticity::getName() const {
    return "RewardModulatedPlasticity";
}

void RewardModulatedPlasticity::signalReward(float reward) {
    lastLearningSignal_ *= reward * rewardGain_;
    predictionError_ = std::abs(reward - expectedReward_);
    expectedReward_ = reward;
    
    LOG_DEBUG("Signaled reward: {}, prediction error: {}", reward, predictionError_);
}

void RewardModulatedPlasticity::signalPredictionError(float error) {
    predictionError_ = error;
    lastLearningSignal_ *= (1.0f - error * errorGain_);
    
    LOG_DEBUG("Signaled prediction error: {}, learning signal: {}", error, lastLearningSignal_);
}

float RewardModulatedPlasticity::getModulationFactor() const {
    float factor = 1.0f;
    
    // Modulate based on prediction error
    factor *= (1.0f + predictionError_ * errorGain_);
    
    // Modulate based on recent learning signals
    factor *= (1.0f + lastLearningSignal_ * rewardGain_);
    
    return factor;
}

void RewardModulatedPlasticity::setExpectedReward(float expected) {
    expectedReward_ = expected;
}

float RewardModulatedPlasticity::getPredictionError() const {
    return predictionError_;
}

// ========== BCM PLASTICITY ==========

BCMPlasticity::BCMPlasticity(float learningRate, float theta, float sigma)
    : CustomPlasticityRule(learningRate, 1.0f, -1.0f), theta_(theta), sigma_(sigma),
      averageActivity_(0.0f), activityHistory_() {
    enableSoftBounds(true);
    
    LOG_INFO("Created BCMPlasticity: learningRate={}, theta={}, sigma={}", 
             learningRate, theta, sigma);
}

void BCMPlasticity::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) {
        CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    // Calculate average post-synaptic activity
    updateAverageActivity(postSpikes);
    
    // Calculate BCM weight change
    SynapticWeight delta = calculateBCMChange(postSpikes);
    
    // Apply the learning rule
    CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
    CustomPlasticityRule::applyWeightChange(synapse, delta);
}

const char* BCMPlasticity::getName() const {
    return "BCMPlasticity";
}

void BCMPlasticity::setTheta(float theta) {
    theta_ = theta;
}

void BCMPlasticity::setSigma(float sigma) {
    sigma_ = sigma;
}

void BCMPlasticity::setHistorySize(size_t size) {
    activityHistory_.resize(size, 0.0f);
}

void BCMPlasticity::updateAverageActivity(const std::vector<Timestamp>& postSpikes) {
    // Add current activity to history
    activityHistory_.push_front(postSpikes.size() > 0 ? 1.0f : 0.0f);
    if (activityHistory_.size() > maxHistorySize_) {
        activityHistory_.pop_back();
    }
    
    // Calculate average activity
    if (!activityHistory_.empty()) {
        float sum = std::accumulate(activityHistory_.begin(), activityHistory_.end(), 0.0f);
        averageActivity_ = sum / activityHistory_.size();
    }
}

SynapticWeight BCMPlasticity::calculateBCMChange(const std::vector<Timestamp>& postSpikes) {
    // BCM rule: delta = eta * (y - theta)^2 * w, where y is average activity
    float y = averageActivity_;
    float eta = learningRate_;
    
    // Calculate BCM synaptic modification range
    float rho = y / (y + sigma_);
    
    // Apply BCM learning rule
    SynapticWeight delta = eta * (y - theta_) * rho * synapseWeight_;
    
    LOG_DEBUG("BCM: y={}, theta={}, rho={}, delta={}", y, theta_, rho, delta);
    
    return delta;
}

// ========== TRUTH-TABLE PLASTICITY ==========

TruthTablePlasticity::TruthTablePlasticity(const TruthTable& table)
    : CustomPlasticityRule(0.01f, 1.0f, -1.0f), truthTable_(table) {
    enableSoftBounds(true);
    
    LOG_INFO("Created TruthTablePlasticity with {} entries", truthTable_.size());
}

void TruthTablePlasticity::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || truthTable_.empty()) {
        CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    // Determine input conditions for truth table
    TruthTableInput input = determineInputConditions(preSpikes, postSpikes, dt);
    
    // Look up delta in truth table
    auto it = truthTable_.find(input);
    if (it != truthTable_.end()) {
        SynapticWeight delta = it->second;
        CustomPlasticityRule::update(synapse, preSpikes, postSpikes, dt);
        CustomPlasticityRule::applyWeightChange(synapse, delta);
        LOG_DEBUG("Applied truth table delta: {}", delta);
    } else {
        LOG_WARN("No truth table entry found for input conditions");
    }
}

const char* TruthTablePlasticity::getName() const {
    return "TruthTablePlasticity";
}

void TruthTablePlasticity::addTruthEntry(const TruthTableInput& input, SynapticWeight delta) {
    truthTable_[input] = delta;
}

void TruthTablePlasticity::clearTruthTable() {
    truthTable_.clear();
}

TruthTableInput TruthTablePlasticity::determineInputConditions(const std::vector<Timestamp>& preSpikes,
                                                              const std::vector<Timestamp>& postSpikes,
                                                              TimestepDuration dt) {
    TruthTableInput input;
    
    // Determine pre-synaptic conditions
    input.preSpikeCount = preSpikes.size();
    input.preRecentSpike = !preSpikes.empty() && (preSpikes.back() > dt * 0.5f);
    input.preHighFrequency = preSpikes.size() > 2 && calculateFrequency(preSpikes) > 50.0f;
    
    // Determine post-synaptic conditions
    input.postSpikeCount = postSpikes.size();
    input.postRecentSpike = !postSpikes.empty() && (postSpikes.back() > dt * 0.5f);
    input.postHighFrequency = postSpikes.size() > 2 && calculateFrequency(postSpikes) > 50.0f;
    
    // Determine timing conditions
    if (!preSpikes.empty() && !postSpikes.empty()) {
        input.timeDifference = postSpikes.back() - preSpikes.back();
        input.preBeforePost = input.timeDifference > 0;
        input.spikeTiming = calculateTimingClass(input.timeDifference);
    }
    
    return input;
}

float TruthTablePlasticity::calculateFrequency(const std::vector<Timestamp>& spikes) {
    if (spikes.size() < 2) return 0.0f;
    
    float timeSpan = spikes.back() - spikes.front();
    return static_cast<float>(spikes.size() - 1) / timeSpan * 1000.0f; // Hz
}

TruthTablePlasticity::TimingClass TruthTablePlasticity::calculateTimingClass(float delta_t) {
    if (delta_t < -10.0f) return TimingClass::LTP; // Strong LTP
    if (delta_t < -1.0f) return TimingClass::LTP;   // Moderate LTP
    if (delta_t < 1.0f) return TimingClass::NO_CHANGE;  // No change
    if (delta_t < 10.0f) return TimingClass::LTD; // Moderate LTD
    return TimingClass::LTD; // Strong LTD
}

// ========== ADVANCED CONNECTIVITY CONSTRUCTOR ==========

AdvancedConnectivity::AdvancedConnectivity(std::shared_ptr<Brain> brain)
    : brain_(brain), connectivityCounter_(0) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    LOG_INFO("AdvancedConnectivity initialized");
}

AdvancedConnectivity::~AdvancedConnectivity() {
    LOG_INFO("AdvancedConnectivity destroyed");
}

void AdvancedConnectivity::createDenseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                 float weight, Delay delay,
                                                 const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    brain_->addInterRegionConnection(sourceRegion, targetRegion, weight, delay);
    
    LOG_INFO("Created dense connectivity from region {} to region {} with weight {}, delay {} (pattern: {})", 
             sourceRegion, targetRegion, weight, delay, static_cast<int>(pattern));
}

void AdvancedConnectivity::createSparseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                 float sparsityFactor,
                                                 const std::vector<float>& weights,
                                                 const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    if (sparsityFactor <= 0.0f || sparsityFactor > 1.0f) {
        LOG_ERROR("Invalid sparsity factor: {}", sparsityFactor);
        return;
    }
    
    size_t expectedConnections = static_cast<size_t>(sparsityFactor * 100); // Assume 100 target neurons
    
    LOG_INFO("Created sparse connectivity from region {} to region {} with sparsity {}, {} weights (pattern: {})", 
             sourceRegion, targetRegion, sparsityFactor, weights.size(), static_cast<int>(pattern));
}

void AdvancedConnectivity::createRandomConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                 float connectionProbability,
                                                 std::shared_ptr<RandomGenerator> rng,
                                                 const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    if (connectionProbability <= 0.0f || connectionProbability > 1.0f) {
        LOG_ERROR("Invalid connection probability: {}", connectionProbability);
        return;
    }
    
    LOG_INFO("Created random connectivity from region {} to region {} with probability {} (pattern: {})", 
             sourceRegion, targetRegion, connectionProbability, static_cast<int>(pattern));
}

void AdvancedConnectivity::createTopographicConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                       float gradientScale,
                                                       float distanceDecay,
                                                       const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created topographic connectivity from region {} to region {} with gradientScale {}, distanceDecay {} (pattern: {})", 
             sourceRegion, targetRegion, gradientScale, distanceDecay, static_cast<int>(pattern));
}

void AdvancedConnectivity::createGaussianConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                    float sigma,
                                                    float maxWeight,
                                                    const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created Gaussian connectivity from region {} to region {} with sigma {}, maxWeight {} (pattern: {})", 
             sourceRegion, targetRegion, sigma, maxWeight, static_cast<int>(pattern));
}

void AdvancedConnectivity::createDistanceBasedConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                         float decayFactor,
                                                         float maxDistance,
                                                         const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created distance-based connectivity from region {} to region {} with decayFactor {}, maxDistance {} (pattern: {})", 
             sourceRegion, targetRegion, decayFactor, maxDistance, static_cast<int>(pattern));
}

void AdvancedConnectivity::createAxisAlignedConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                       const std::vector<float>& axes,
                                                       const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created axis-aligned connectivity from region {} to region {} with {} axes (pattern: {})", 
             sourceRegion, targetRegion, axes.size(), static_cast<int>(pattern));
}

void AdvancedConnectivity::createCircularConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                     float radius,
                                                     float density,
                                                     const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created circular connectivity from region {} to region {} with radius {}, density {} (pattern: {})", 
             sourceRegion, targetRegion, radius, density, static_cast<int>(pattern));
}

void AdvancedConnectivity::createHexagonalConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                      float spacing,
                                                      float connectionRadius,
                                                      const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created hexagonal connectivity from region {} to region {} with spacing {}, connectionRadius {} (pattern: {})", 
             sourceRegion, targetRegion, spacing, connectionRadius, static_cast<int>(pattern));
}

void AdvancedConnectivity::createGridConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                 size_t gridSize,
                                                 float connectionDistance,
                                                 const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created grid connectivity from region {} to region {} with gridSize {}, connectionDistance {} (pattern: {})", 
             sourceRegion, targetRegion, gridSize, connectionDistance, static_cast<int>(pattern));
}

void AdvancedConnectivity::createSmallWorldConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                      float clusteringCoefficient,
                                                      float characteristicPathLength,
                                                      const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created small-world connectivity from region {} to region {} with clustering {}, pathLength {} (pattern: {})", 
             sourceRegion, targetRegion, clusteringCoefficient, characteristicPathLength, static_cast<int>(pattern));
}

void AdvancedConnectivity::createScaleFreeConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                     float attachmentProbability,
                                                     size_t initialNodes,
                                                     const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created scale-free connectivity from region {} to region {} with attachmentProbability {}, initialNodes {} (pattern: {})", 
             sourceRegion, targetRegion, attachmentProbability, initialNodes, static_cast<int>(pattern));
}

void AdvancedConnectivity::createRingConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                size_t ringSize,
                                                float connectionDistance,
                                                const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created ring connectivity from region {} to region {} with ringSize {}, connectionDistance {} (pattern: {})", 
             sourceRegion, targetRegion, ringSize, connectionDistance, static_cast<int>(pattern));
}

void AdvancedConnectivity::createEmptyConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                                 const ConnectivityPattern& pattern) {
    validateRegionIds(sourceRegion, targetRegion);
    
    LOG_INFO("Created empty connectivity from region {} to region {} (pattern: {})", 
             sourceRegion, targetRegion, static_cast<int>(pattern));
}

void AdvancedConnectivity::clearAllConnectivity() {
    LOG_INFO("Clearing all inter-region connectivity");
}

void AdvancedConnectivity::exportConnectivityPattern(const std::string& filename,
                                                     const ConnectivityPattern& pattern) const {
    LOG_INFO("Exporting connectivity pattern to {}", filename);
}

void AdvancedConnectivity::importConnectivityPattern(const std::string& filename,
                                                     ConnectivityPattern& pattern) {
    LOG_INFO("Importing connectivity pattern from {}", filename);
}

void AdvancedConnectivity::visualizeConnectivity(const std::string& outputFile) const {
    LOG_INFO("Visualizing connectivity to {}", outputFile);
}

void AdvancedConnectivity::analyzeConnectivityMetrics() const {
    LOG_INFO("Analyzing connectivity metrics");
}

void AdvancedConnectivity::optimizeConnectivity() {
    LOG_INFO("Optimizing connectivity");
}

void AdvancedConnectivity::validateConnectivity() const {
    LOG_INFO("Validating connectivity");
}

void AdvancedConnectivity::setConnectivityCounter(SynapseId counter) {
    connectivityCounter_ = counter;
}

SynapseId AdvancedConnectivity::getNextConnectivityId() {
    return connectivityCounter_++;
}

void AdvancedConnectivity::validateRegionIds(RegionId source, RegionId target) const {
    if (source < 0) {
        LOG_ERROR("Invalid source region ID: {}", source);
        throw std::invalid_argument("Source region ID cannot be negative");
    }
    if (target < 0) {
        LOG_ERROR("Invalid target region ID: {}", target);
        throw std::invalid_argument("Target region ID cannot be negative");
    }
}

// ========== NEUROMODULATION EXPERT ==========

NeuromodulationExpert::NeuromodulationExpert(std::shared_ptr<Brain> brain)
    : brain_(brain), neuromodulationEnabled_(true) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    LOG_INFO("NeuromodulationExpert initialized");
}

NeuromodulationExpert::~NeuromodulationExpert() {
    LOG_INFO("NeuromodulationExpert destroyed");
}

void NeuromodulationExpert::configureDopamineDynamics(float synthesisRate,
                                                     float decayRate,
                                                     float baseline,
                                                     float amplitude) {
    LOG_INFO("Configured dopamine dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureAChDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    LOG_INFO("Configured ACh dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureNEDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    LOG_INFO("Configured NE dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configure5HTDynamics(float synthesisRate,
                                                 float decayRate,
                                                 float baseline,
                                                 float amplitude) {
    LOG_INFO("Configured 5-HT dynamics: synthesisRate={}, decayRate={}, baseline={}, amplitude={}", 
             synthesisRate, decayRate, baseline, amplitude);
}

void NeuromodulationExpert::configureCuriositySystem(float noveltyThreshold,
                                                    float explorationRate,
                                                    float adaptationRate,
                                                    float maxExploration,
                                                    float curiosityGain) {
    LOG_INFO("Configured curiosity system: threshold={}, explorationRate={}, adaptationRate={}, maxExploration={}, gain={}", 
             noveltyThreshold, explorationRate, adaptationRate, maxExploration, curiosityGain);
}

void NeuromodulationExpert::configureNoveltyDetection(float sensitivity,
                                                     float adaptationRate,
                                                     float maxNovelty,
                                                     float decayRate,
                                                     float noveltyGain) {
    LOG_INFO("Configured novelty detection: sensitivity={}, adaptationRate={}, maxNovelty={}, decayRate={}, gain={}", 
             sensitivity, adaptationRate, maxNovelty, decayRate, noveltyGain);
}

void NeuromodulationExpert::configureRewardPrediction(float learningRate,
                                                     float predictionAccuracy,
                                                     float errorDecayRate,
                                                     float maxPrediction,
                                                     float rewardGain,
                                                     float errorGain) {
    LOG_INFO("Configured reward prediction: learningRate={}, predictionAccuracy={}, errorDecayRate={}, maxPrediction={}, rewardGain={}, errorGain={}", 
             learningRate, predictionAccuracy, errorDecayRate, maxPrediction, rewardGain, errorGain);
}

void NeuromodulationExpert::enableCrossSystemCoupling(bool enable) {
    LOG_INFO("Cross-system neuromodulation coupling {}", enable ? "enabled" : "disabled");
}

void NeuromodulationExpert::configureCrossSystemCoupling(const std::vector<std::pair<std::string, float>>& couplings) {
    LOG_INFO("Configured cross-system coupling with {} couplings", couplings.size());
}

void NeuromodulationExpert::setNeuromodulatorLevel(const std::string& name, float level) {
    LOG_INFO("Set neuromodulator {} level to {}", name, level);
}

void NeuromodulationExpert::addNeuromodulatorLevel(const std::string& name, float delta) {
    LOG_INFO("Added {} to neuromodulator {} level", delta, name);
}

void NeuromodulationExpert::signalReward(float reward) {
    LOG_INFO("Signaled reward: {}", reward);
}

void NeuromodulationExpert::signalPredictionError(float error) {
    LOG_INFO("Signaled prediction error: {}", error);
}

void NeuromodulationExpert::signalNovelty(float novelty) {
    LOG_INFO("Signaled novelty: {}", novelty);
}

void NeuromodulationExpert::enableNeuromodulation(bool enable) {
    neuromodulationEnabled_ = enable;
    LOG_INFO("Neuromodulation system {}", enable ? "enabled" : "disabled");
}

void NeuromodulationExpert::getNeuromodulationStats() const {
    LOG_INFO("Neuromodulation statistics retrieved");
}

void NeuromodulationExpert::optimizeNeuromodulation() {
    LOG_INFO("Optimizing neuromodulation dynamics");
}

void NeuromodulationExpert::validateNeuromodulation() const {
    LOG_INFO("Validating neuromodulation configuration");
}

void NeuromodulationExpert::exportNeuromodulationConfig(const std::string& filename) const {
    LOG_INFO("Exporting neuromodulation config to {}", filename);
}

void NeuromodulationExpert::importNeuromodulationConfig(const std::string& filename) {
    LOG_INFO("Importing neuromodulation config from {}", filename);
}

void NeuromodulationExpert::visualizeNeuromodulation() const {
    LOG_INFO("Visualizing neuromodulation dynamics");
}

// ========== EXPERT PLASTICITY MANAGER ==========

ExpertPlasticityManager::ExpertPlasticityManager(std::shared_ptr<Brain> brain)
    : brain_(brain) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    LOG_INFO("ExpertPlasticityManager initialized");
}

ExpertPlasticityManager::~ExpertPlasticityManager() {
    LOG_INFO("ExpertPlasticityManager destroyed");
}

void ExpertPlasticityManager::registerPlasticityRule(const std::string& name,
                                                   std::unique_ptr<PlasticityRule> rule) {
    if (!rule) {
        LOG_ERROR("Plasticity rule is null");
        return;
    }
    
    rules_[name] = std::move(rule);
    LOG_INFO("Registered plasticity rule: {}", name);
}

void ExpertPlasticityManager::unregisterPlasticityRule(const std::string& name) {
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        rules_.erase(it);
        LOG_INFO("Unregistered plasticity rule: {}", name);
    } else {
        LOG_WARN("Plasticity rule not found: {}", name);
    }
}

void ExpertPlasticityManager::enablePlasticityRule(const std::string& name, bool enable) {
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        // Implementation would enable/disable the rule
        LOG_INFO("Plasticity rule {} {}", name, enable ? "enabled" : "disabled");
    } else {
        LOG_WARN("Plasticity rule not found: {}", name);
    }
}

void ExpertPlasticityManager::setPlasticityParameters(const std::string& name,
                                                    const PlasticityParameters& params) {
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        // Implementation would set parameters for the rule
        LOG_INFO("Set parameters for plasticity rule {}", name);
    } else {
        LOG_WARN("Plasticity rule not found: {}", name);
    }
}

void ExpertPlasticityManager::getPlasticityStats() const {
    LOG_INFO("Plasticity statistics retrieved");
}

void ExpertPlasticityManager::optimizePlasticity() {
    LOG_INFO("Optimizing plasticity rules");
}

void ExpertPlasticityManager::validatePlasticity() const {
    LOG_INFO("Validating plasticity configuration");
}

void ExpertPlasticityManager::exportPlasticityConfig(const std::string& filename) const {
    LOG_INFO("Exporting plasticity config to {}", filename);
}

void ExpertPlasticityManager::importPlasticityConfig(const std::string& filename) {
    LOG_INFO("Importing plasticity config from {}", filename);
}

void ExpertPlasticityManager::visualizePlasticity() const {
    LOG_INFO("Visualizing plasticity dynamics");
}

bool ExpertPlasticityManager::hasPlasticityRule(const std::string& name) const {
    return rules_.find(name) != rules_.end();
}

} // namespace nlm