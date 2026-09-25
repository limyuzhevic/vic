// Enhanced Plasticity Rules Implementation
// 
// This file provides advanced plasticity rules that extend the basic STDP and Hebbian
// implementations with biological accuracy, more sophisticated learning mechanisms,
// and adaptive behavior.

#pragma once

#include "plasticity/PlasticityRule.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// === Triplet STDP Implementation ===
// 
// Spike-timing dependent plasticity with triplet interactions
// Based on biological evidence that synaptic plasticity depends on multiple
// spike combinations (pre-post, pre-pre-post, post-post-pre)
//
// Mathematical formulation:
// Δw = A+ * (exp(-Δt/τ+) - b+) + A- * (exp(-Δt/τ-) - b-)
// 
// Where triplet terms account for interactions with previous spikes

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // Triplet STDP specific parameters
        ltpWeight_ = 0.01f;
        ltdWeight_ = 0.012f;
        timeConstant_ = 20.0f;
        
        // Triplet interaction parameters
        tripletLTP_ = 0.005f;    // A+ for triplet potentiation
        tripletLTD_ = 0.006f;    // A- for triplet depression
        tripletTimeConstant_ = 50.0f;  // Time constant for triplet effects
        
        // Baseline offsets
        ltpBaseline_ = 0.001f;
        ltdBaseline_ = 0.001f;
    }
    
    const char* getName() const override { return "TripletSTDP"; }
    
    void setLTPWeight(float weight) {
        ltpWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getLTPWeight() const { return ltpWeight_; }
    
    void setLTDWeight(float weight) {
        ltdWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getLTDWeight() const { return ltdWeight_; }
    
    void setTimeConstant(float tau) {
        timeConstant_ = std::clamp(tau, 1.0f, 100.0f);
    }
    
    float getTimeConstant() const { return timeConstant_; }
    
    void setTripletLTP(float weight) {
        tripletLTP_ = std::clamp(weight, 0.0f, 0.1f);
    }
    
    float getTripletLTP() const { return tripletLTP_; }
    
    void setTripletLTD(float weight) {
        tripletLTD_ = std::clamp(weight, 0.0f, 0.1f);
    }
    
    float getTripletLTD() const { return tripletLTD_; }
    
    void configure(float ltp, float ltd, float tau, float tripletLtp, float tripletLtd) {
        setLTPWeight(ltp);
        setLTDWeight(ltd);
        setTimeConstant(tau);
        setTripletLTP(tripletLtp);
        setTripletLTD(tripletLtd);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate pairwise STDP contributions
        float pairwiseDelta = calculatePairwiseSTDP(preSpikes, postSpikes);
        
        // Calculate triplet contributions
        float tripletDelta = calculateTripletSTDP(synapse, preSpikes, postSpikes, dt);
        
        // Combine contributions
        float totalDelta = pairwiseDelta + tripletDelta;
        
        // Apply with eligibility trace
        if (std::abs(totalDelta) > 1e-6f) {
            totalDelta = computeSafeDelta(totalDelta);
            recordLearningEvent(totalDelta, static_cast<float>(preSpikes.back()));
            
            // Update eligibility trace
            updateEligibilityTrace(synapse, totalDelta);
            
            // Apply weight change
            applyWeightChange(synapse, totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = clampWeight(newWeight);
        synapse->setWeight(newWeight);
    }
    
protected:
    void onParametersChanged() override {
        // Update parameters from PlasticityParameters
        ltpWeight_ = params_.learningRate * 1.0f;
        ltdWeight_ = params_.learningRate * 1.2f;
        timeConstant_ = params_.timeConstant;
        tripletTimeConstant_ = params_.timeConstant * 2.5f;
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    float calculatePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes) {
        float totalDelta = 0.0f;
        float tau = timeConstant_;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                
                if (dt > 0) {
                    // Pre before post: LTP
                    float delta = ltpWeight_ * std::exp(-dt / tau) - ltpBaseline_;
                    totalDelta += delta;
                } else if (dt < 0) {
                    // Post before pre: LTD
                    float delta = -ltdWeight_ * std::exp(dt / tau) - ltdBaseline_;
                    totalDelta += delta;
                }
            }
        }
        
        return totalDelta;
    }
    
    float calculateTripletSTDP(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
        if (preSpikes.size() < 2 || postSpikes.size() < 2) {
            return 0.0f;
        }
        
        // Calculate recent spike patterns for triplet interactions
        size_t recentPreCount = std::min<size_t>(5, preSpikes.size());
        size_t recentPostCount = std::min<size_t>(5, postSpikes.size());
        
        float prePrePostDelta = 0.0f;
        float postPostPreDelta = 0.0f;
        
        // Analyze pre-pre-post patterns (potentiation)
        for (size_t i = 0; i < recentPreCount - 1; ++i) {
            for (size_t j = 0; j < recentPostCount; ++j) {
                float timeDiff1 = static_cast<float>(preSpikes[i] - preSpikes[i + 1]);
                float timeDiff2 = static_cast<float>(postSpikes[j] - preSpikes[i + 1]);
                
                if (timeDiff2 > 0 && timeDiff1 > 0) {
                    // Pre-pre-post triplet: strong potentiation
                    prePrePostDelta += tripletLTP_ * std::exp(-timeDiff1 / tripletTimeConstant_) *
                                     std::exp(-timeDiff2 / timeConstant_);
                }
            }
        }
        
        // Analyze post-post-pre patterns (depression)
        for (size_t i = 0; i < recentPostCount - 1; ++i) {
            for (size_t j = 0; j < recentPreCount; ++j) {
                float timeDiff1 = static_cast<float>(postSpikes[i] - postSpikes[i + 1]);
                float timeDiff2 = static_cast<float>(preSpikes[j] - postSpikes[i + 1]);
                
                if (timeDiff2 < 0 && timeDiff1 > 0) {
                    // Post-post-pre triplet: strong depression
                    postPostPreDelta += tripletLTD_ * std::exp(-timeDiff1 / tripletTimeConstant_) *
                                      std::exp(timeDiff2 / timeConstant_);
                }
            }
        }
        
        return prePrePostDelta + postPostPreDelta;
    }
    
private:
    // Core STDP parameters
    float ltpWeight_;
    float ltdWeight_;
    float timeConstant_;
    
    // Triplet interaction parameters
    float tripletLTP_;
    float tripletLTD_;
    float tripletTimeConstant_;
    
    // Baseline offsets
    float ltpBaseline_;
    float ltdBaseline_;
};

// === BCM (Bienenstock-Cooper-Munro) Rule ===
// 
// Competitive learning rule based on synaptic modification range (SMR)
// Implements homeostatic competition between synapses
//
// Mathematical formulation:
// Δw = η * (x - θ(y)) * (y - θ(y))
// 
// Where x is pre-synaptic activity, y is post-synaptic activity
// θ is the sliding threshold (excitability)

class BCMRule : public PlasticityRule {
public:
    BCMRule() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // BCM-specific parameters
        postMeanActivity_ = 0.5f;
        smrWidth_ = 2.0f;          // Synaptic modification range width
        thresholdLearningRate_ = 0.01f;  // Learning rate for threshold
        stabilizationFactor_ = 0.95f;   // Threshold stabilization
        
        // Activity history for threshold calculation
        preActivityHistory_.resize(100, 0.0f);
        postActivityHistory_.resize(100, 0.0f);
        historyIndex_ = 0;
    }
    
    const char* getName() const override { return "BCM"; }
    
    void setPostMeanActivity(float activity) {
        postMeanActivity_ = std::clamp(activity, 0.0f, 2.0f);
    }
    
    float getPostMeanActivity() const { return postMeanActivity_; }
    
    void setSMRWidth(float width) {
        smrWidth_ = std::max(0.5f, width);
    }
    
    float getSMRWidth() const { return smrWidth_; }
    
    void setThresholdLearningRate(float rate) {
        thresholdLearningRate_ = std::clamp(rate, 0.0f, 0.1f);
    }
    
    float getThresholdLearningRate() const { return thresholdLearningRate_; }
    
    void configure(float postMean, float smrWidth, float thresholdRate) {
        setPostMeanActivity(postMean);
        setSMRWidth(smrWidth);
        setThresholdLearningRate(thresholdRate);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate current activities
        float preActivity = calculateActivityFromSpikes(preSpikes, dt);
        float postActivity = calculateActivityFromSpikes(postSpikes, dt);
        
        // Update activity histories
        updateActivityHistory(preActivity, postActivity);
        
        // Calculate BCM threshold (sliding threshold)
        float threshold = calculateBCMThreshold();
        
        // Compute BCM learning rule
        float delta = computeBCMLearning(preActivity, postActivity, threshold);
        
        // Apply weight change
        if (std::abs(delta) > 1e-6f) {
            delta = computeSafeDelta(delta);
            recordLearningEvent(delta, static_cast<float>(preSpikes.back()));
            applyWeightChange(synapse, delta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = clampWeight(newWeight);
        synapse->setWeight(newWeight);
    }
    
    // Get current threshold for debugging/monitoring
    float getCurrentThreshold() const {
        return calculateBCMThreshold();
    }
    
    // Get activity statistics
    float getMeanPreActivity() const {
        float sum = 0.0f;
        for (float activity : preActivityHistory_) {
            sum += activity;
        }
        return sum / preActivityHistory_.size();
    }
    
    float getMeanPostActivity() const {
        float sum = 0.0f;
        for (float activity : postActivityHistory_) {
            sum += activity;
        }
        return sum / postActivityHistory_.size();
    }
    
protected:
    void onParametersChanged() override {
        // Update parameters from PlasticityParameters
        postMeanActivity_ = params_.learningRate * 2.0f;
        smrWidth_ = params_.timeConstant * 0.05f;
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    float calculateActivityFromSpikes(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
        if (spikes.empty()) return 0.0f;
        
        // Calculate firing rate from spike times
        float timeSpan = static_cast<float>(spikes.back() - spikes.front());
        if (timeSpan <= 0.0f) return 1.0f;
        
        // Activity is proportional to spike count normalized by time
        float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
        return std::min(2.0f, std::max(0.0f, activity));
    }
    
    void updateActivityHistory(float preActivity, float postActivity) {
        preActivityHistory_[historyIndex_] = preActivity;
        postActivityHistory_[historyIndex_] = postActivity;
        
        historyIndex_ = (historyIndex_ + 1) % preActivityHistory_.size();
    }
    
    float calculateBCMThreshold() {
        // Sliding threshold based on post-synaptic activity history
        float meanPostActivity = getMeanPostActivity();
        
        // Threshold follows mean activity with some inertia
        float threshold = meanPostActivity * smrWidth_;
        
        // Apply stabilization
        threshold = threshold * stabilizationFactor_ + postMeanActivity_ * (1.0f - stabilizationFactor_);
        
        // Constrain to reasonable range
        return std::clamp(threshold, 0.1f, 3.0f);
    }
    
    float computeBCMLearning(float preActivity, float postActivity, float threshold) {
        // BCM learning rule: Δw = η * (x - θ(y)) * (y - θ(y))
        float error = preActivity - threshold;
        float modulation = postActivity - threshold;
        
        float delta = params_.learningRate * error * modulation;
        
        // Normalize by SMR width
        delta /= smrWidth_;
        
        // Apply non-linear saturation
        delta = std::tanh(delta / 10.0f) * params_.learningRate;
        
        return delta;
    }
    
private:
    // BCM parameters
    float postMeanActivity_;
    float smrWidth_;
    float thresholdLearningRate_;
    float stabilizationFactor_;
    
    // Activity history for threshold calculation
    std::vector<float> preActivityHistory_;
    std::vector<float> postActivityHistory_;
    size_t historyIndex_;
};

// === Oja Learning Rule ===
// 
// Normalization-based learning rule for competitive learning
// Implements Hebbian learning with weight normalization
//
// Mathematical formulation:
// Δw = η * y * (x - y * w)
// 
// Where y is post-synaptic activity, x is pre-synaptic activity
// The second term provides competitive normalization

class OjaRule : public PlasticityRule {
public:
    OjaRule() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // Oja-specific parameters
        normalizationFactor_ = 1.0f;
        maxWeightMagnitude_ = 1.0f;
        activitySmoothing_ = 0.1f;
        
        // Activity tracking
        postActivityHistory_.resize(50, 0.0f);
        historyIndex_ = 0;
    }
    
    const char* getName() const override { return "Oja"; }
    
    void setMaxWeightMagnitude(float max) {
        maxWeightMagnitude_ = std::max(0.5f, max);
    }
    
    float getMaxWeightMagnitude() const { return maxWeightMagnitude_; }
    
    void setActivitySmoothing(float smoothing) {
        activitySmoothing_ = std::clamp(smoothing, 0.01f, 0.5f);
    }
    
    float getActivitySmoothing() const { return activitySmoothing_; }
    
    void configure(float maxWeight, float smoothing) {
        setMaxWeightMagnitude(maxWeight);
        setActivitySmoothing(smoothing);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate current activities
        float preActivity = calculatePreActivity(preSpikes, dt);
        float postActivity = calculatePostActivity(postSpikes, dt);
        
        // Update activity history
        updateActivityHistory(postActivity);
        
        // Calculate Oja learning rule
        float delta = computeOjaLearning(preActivity, postActivity, synapse);
        
        // Apply weight change
        if (std::abs(delta) > 1e-6f) {
            delta = computeSafeDelta(delta);
            recordLearningEvent(delta, static_cast<float>(preSpikes.back()));
            applyWeightChange(synapse, delta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        
        // Apply Oja normalization
        newWeight = normalizeOjaWeight(synapse, newWeight);
        
        // Apply standard clamping
        newWeight = clampWeight(newWeight);
        synapse->setWeight(newWeight);
    }
    
    // Get current normalization factor
    float getNormalizationFactor() const {
        return normalizationFactor_;
    }
    
    // Get activity statistics
    float getMeanPostActivity() const {
        float sum = 0.0f;
        for (float activity : postActivityHistory_) {
            sum += activity;
        }
        return sum / postActivityHistory_.size();
    }
    
protected:
    void onParametersChanged() override {
        // Update parameters from PlasticityParameters
        maxWeightMagnitude_ = params_.maxWeight;
        activitySmoothing_ = std::clamp(params_.learningRate, 0.01f, 0.5f);
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    float calculatePreActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
        if (spikes.empty()) return 0.0f;
        
        // Calculate pre-synaptic activity from spike times
        float timeSpan = static_cast<float>(spikes.back() - spikes.front());
        if (timeSpan <= 0.0f) return 1.0f;
        
        float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
        return std::min(2.0f, std::max(0.0f, activity));
    }
    
    float calculatePostActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
        if (spikes.empty()) return 0.0f;
        
        // Calculate post-synaptic activity from spike times
        float timeSpan = static_cast<float>(spikes.back() - spikes.front());
        if (timeSpan <= 0.0f) return 1.0f;
        
        float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
        return std::min(2.0f, std::max(0.0f, activity));
    }
    
    void updateActivityHistory(float postActivity) {
        postActivityHistory_[historyIndex_] = postActivity;
        historyIndex_ = (historyIndex_ + 1) % postActivityHistory_.size();
        
        // Update normalization factor based on average activity
        float meanActivity = getMeanPostActivity();
        normalizationFactor_ = activitySmoothing_ * meanActivity +
                               (1.0f - activitySmoothing_) * normalizationFactor_;
        normalizationFactor_ = std::clamp(normalizationFactor_, 0.1f, 10.0f);
    }
    
    float computeOjaLearning(float preActivity, float postActivity, Synapse* synapse) {
        // Oja learning rule: Δw = η * y * (x - y * w)
        float currentWeight = synapse->getWeight();
        
        float delta = params_.learningRate * postActivity * (preActivity - postActivity * currentWeight);
        
        // Apply normalization factor
        delta /= normalizationFactor_;
        
        // Apply non-linear saturation
        delta = std::tanh(delta / 5.0f) * params_.learningRate;
        
        return delta;
    }
    
    float normalizeOjaWeight(Synapse* synapse, float newWeight) {
        float currentWeight = synapse->getWeight();
        
        // Oja normalization: w_new = w / sqrt(w^2 + η * y^2 * dt)
        float denominator = std::sqrt(currentWeight * currentWeight +
                                      params_.learningRate * postActivityHistory_[0] * postActivityHistory_[0] *
                                      static_cast<float>(synapse->getLastSpikeTime() - 1.0f));
        
        if (denominator > 0.0f) {
            newWeight = currentWeight / denominator;
        }
        
        // Constrain to maximum magnitude
        float magnitude = std::sqrt(newWeight * newWeight);
        if (magnitude > maxWeightMagnitude_) {
            newWeight = newWeight / magnitude * maxWeightMagnitude_;
        }
        
        return newWeight;
    }
    
private:
    // Oja-specific parameters
    float normalizationFactor_;
    float maxWeightMagnitude_;
    float activitySmoothing_;
    
    // Activity tracking
    std::vector<float> postActivityHistory_;
    size_t historyIndex_;
};

// === Reward-Modulated STDP ===
// 
// STDP with reward-modulated plasticity factor
// Combines spike-timing dependent plasticity with reward prediction error
//
// Mathematical formulation:
// Δw = A(t) * (w_LTP * exp(-Δt/τ+) - w_LTD * exp(-Δt/τ-))
// 
// Where A(t) is a reward-modulated plasticity factor

class RewardModulatedSTDP : public EnhancedSTDP {
public:
    RewardModulatedSTDP() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // Reward modulation parameters
        rewardWeight_ = 0.5f;
        predictionWeight_ = 0.3f;
        eligibilityDecay_ = 0.95f;
        maxEligibilityTrace_ = 10.0f;
        baselinePlasticity_ = 1.0f;
        
        // Reward history tracking
        rewardHistory_.resize(100, 0.0f);
        historyIndex_ = 0;
    }
    
    const char* getName() const override { return "RewardModulatedSTDP"; }
    
    void setRewardWeight(float weight) {
        rewardWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getRewardWeight() const { return rewardWeight_; }
    
    void setPredictionWeight(float weight) {
        predictionWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getPredictionWeight() const { return predictionWeight_; }
    
    void setEligibilityDecay(float decay) {
        eligibilityDecay_ = std::clamp(decay, 0.5f, 1.0f);
    }
    
    float getEligibilityDecay() const { return eligibilityDecay_; }
    
    void setBaselinePlasticity(float baseline) {
        baselinePlasticity_ = std::clamp(baseline, 0.5f, 2.0f);
    }
    
    float getBaselinePlasticity() const { return baselinePlasticity_; }
    
    // Apply reward modulation
    void applyRewardModulation(float reward, float predictionError) {
        // Update reward history
        rewardHistory_[historyIndex_] = reward;
        historyIndex_ = (historyIndex_ + 1) % rewardHistory_.size();
        
        // Calculate reward-modulated plasticity factor
        float averageReward = calculateAverageReward();
        float rewardModulation = 1.0f + (averageReward - baselinePlasticity_) * rewardWeight_;
        
        // Calculate prediction error modulation
        float predictionModulation = 1.0f + predictionError * predictionWeight_;
        
        // Combine modulations
        float plasticityFactor = rewardModulation * predictionModulation;
        plasticityFactor = std::clamp(plasticityFactor, 0.5f, 2.0f);
        
        // Apply to STDP weights
        float currentLTP = getLTPWeight();
        float currentLTD = getLTDWeight();
        
        setLTPWeight(currentLTP * plasticityFactor);
        setLTDWeight(currentLTD * plasticityFactor);
        
        NLM_LOG_INFO("R-STDP modulation: reward=" + std::to_string(reward) +
                    " pred_err=" + std::to_string(predictionError) +
                    " factor=" + std::to_string(plasticityFactor));
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Update eligibility trace with decay
        if (synapse) {
            float currentTrace = synapse->getEligibilityTrace();
            currentTrace *= eligibilityDecay_;
            synapse->setEligibilityTrace(currentTrace);
        }
        
        // Apply reward-modulated STDP
        EnhancedSTDP::update(synapse, preSpikes, postSpikes, dt);
    }
    
    // Get average reward from history
    float calculateAverageReward() const {
        float sum = 0.0f;
        for (float reward : rewardHistory_) {
            sum += reward;
        }
        return sum / rewardHistory_.size();
    }
    
protected:
    void onParametersChanged() override {
        EnhancedSTDP::onParametersChanged();
        
        // Adjust reward weights based on learning rate
        rewardWeight_ = std::min(1.0f, params_.learningRate * 5.0f);
        predictionWeight_ = std::min(1.0f, params_.learningRate * 3.0f);
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    // Reward modulation parameters
    float rewardWeight_;
    float predictionWeight_;
    float eligibilityDecay_;
    float maxEligibilityTrace_;
    float baselinePlasticity_;
    
    // Reward history tracking
    std::vector<float> rewardHistory_;
    size_t historyIndex_;
};

// === Factory Functions ===

// Create Triplet STDP rule
inline std::unique_ptr<PlasticityRule> createTripletSTDP(
    float ltpWeight = 0.01f,
    float ltdWeight = 0.012f,
    float timeConstant = 20.0f,
    float tripletLTP = 0.005f,
    float tripletLTD = 0.006f) {
    
    auto rule = std::make_unique<TripletSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant);
    static_cast<TripletSTDP*>(rule.get())->setTripletLTP(tripletLTP);
    static_cast<TripletSTDP*>(rule.get())->setTripletLTD(tripletLTD);
    return rule;
}

// Create BCM rule
inline std::unique_ptr<PlasticityRule> createBCM(
    float postMeanActivity = 0.5f,
    float smrWidth = 2.0f,
    float thresholdLearningRate = 0.01f) {
    
    auto rule = std::make_unique<BCMRule>();
    rule->configure(postMeanActivity, smrWidth, thresholdLearningRate);
    return rule;
}

// Create Oja rule
inline std::unique_ptr<PlasticityRule> createOja(
    float maxWeightMagnitude = 1.0f,
    float activitySmoothing = 0.1f) {
    
    auto rule = std::make_unique<OjaRule>();
    rule->configure(maxWeightMagnitude, activitySmoothing);
    return rule;
}

// Create Reward-Modulated STDP
inline std::unique_ptr<PlasticityRule> createRewardModulatedSTDP(
    float ltpWeight = 0.01f,
    float ltdWeight = 0.012f,
    float timeConstant = 20.0f,
    float rewardWeight = 0.5f) {
    
    auto rule = std::make_unique<RewardModulatedSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant);
    static_cast<RewardModulatedSTDP*>(rule.get())->setRewardWeight(rewardWeight);
    return rule;
}

// Get list of available plasticity rules
inline std::vector<std::string> getAvailablePlasticityRules() {
    return {
        "STDP", "Hebbian", "EnhancedSTDP", "EnhancedHebbian",
        "TripletSTDP", "BCM", "Oja", "RewardModulatedSTDP"
    };
}

// Create plasticity rule by name
inline std::unique_ptr<PlasticityRule> createPlasticityRule(
    const std::string& ruleName,
    const std::map<std::string, std::string>& params = {}) {
    
    std::string name = ruleName;
    // Convert to lowercase for case-insensitive comparison
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (name == "stdp") {
        float ltpWeight = params.count("ltp_weight") ? 
            std::stof(params.at("ltp_weight")) : 0.01f;
        float ltdWeight = params.count("ltd_weight") ? 
            std::stof(params.at("ltd_weight")) : 0.012f;
        float timeConstant = params.count("time_constant") ? 
            std::stof(params.at("time_constant")) : 20.0f;
        return createSTDP(ltpWeight, ltdWeight, timeConstant);
    }
    else if (name == "hebbian") {
        float learningRate = params.count("learning_rate") ? 
            std::stof(params.at("learning_rate")) : 0.01f;
        float correlationWindow = params.count("correlation_window") ? 
            std::stof(params.at("correlation_window")) : 100.0f;
        return createHebbian(learningRate, correlationWindow);
    }
    else if (name == "enhancedstdp") {
        float learningRate = params.count("learning_rate") ? 
            std::stof(params.at("learning_rate")) : 0.01f;
        float minWeight = params.count("min_weight") ? 
            std::stof(params.at("min_weight")) : -1.0f;
        float maxWeight = params.count("max_weight") ? 
            std::stof(params.at("max_weight")) : 1.0f;
        float timeConstant = params.count("time_constant") ? 
            std::stof(params.at("time_constant")) : 20.0f;
        
        auto rule = std::make_unique<EnhancedSTDP>();
        rule->configure(learningRate, minWeight, maxWeight, timeConstant);
        return rule;
    }
    else if (name == "enhancedhebbian") {
        float learningRate = params.count("learning_rate") ? 
            std::stof(params.at("learning_rate")) : 0.01f;
        float correlationWindow = params.count("correlation_window") ? 
            std::stof(params.at("correlation_window")) : 100.0f;
        
        auto rule = std::make_unique<EnhancedHebbian>();
        PlasticityParameters p;
        p.learningRate = learningRate;
        p.timeConstant = correlationWindow / 5.0f;
        rule->setParameters(p);
        return rule;
    }
    else if (name == "tripletstdp") {
        float ltpWeight = params.count("ltp_weight") ? 
            std::stof(params.at("ltp_weight")) : 0.01f;
        float ltdWeight = params.count("ltd_weight") ? 
            std::stof(params.at("ltd_weight")) : 0.012f;
        float timeConstant = params.count("time_constant") ? 
            std::stof(params.at("time_constant")) : 20.0f;
        float tripletLTP = params.count("triplet_ltp") ? 
            std::stof(params.at("triplet_ltp")) : 0.005f;
        float tripletLTD = params.count("triplet_ltd") ? 
            std::stof(params.at("triplet_ltd")) : 0.006f;
        return createTripletSTDP(ltpWeight, ltdWeight, timeConstant, tripletLTP, tripletLTD);
    }
    else if (name == "bcm") {
        float postMeanActivity = params.count("post_mean_activity") ? 
            std::stof(params.at("post_mean_activity")) : 0.5f;
        float smrWidth = params.count("smr_width") ? 
            std::stof(params.at("smr_width")) : 2.0f;
        float thresholdLearningRate = params.count("threshold_learning_rate") ? 
            std::stof(params.at("threshold_learning_rate")) : 0.01f;
        return createBCM(postMeanActivity, smrWidth, thresholdLearningRate);
    }
    else if (name == "oja") {
        float maxWeightMagnitude = params.count("max_weight_magnitude") ? 
            std::stof(params.at("max_weight_magnitude")) : 1.0f;
        float activitySmoothing = params.count("activity_smoothing") ? 
            std::stof(params.at("activity_smoothing")) : 0.1f;
        return createOja(maxWeightMagnitude, activitySmoothing);
    }
    else if (name == "rewardmodulatedstdp") {
        float ltpWeight = params.count("ltp_weight") ? 
            std::stof(params.at("ltp_weight")) : 0.01f;
        float ltdWeight = params.count("ltd_weight") ? 
            std::stof(params.at("ltd_weight")) : 0.012f;
        float timeConstant = params.count("time_constant") ? 
            std::stof(params.at("time_constant")) : 20.0f;
        float rewardWeight = params.count("reward_weight") ? 
            std::stof(params.at("reward_weight")) : 0.5f;
        return createRewardModulatedSTDP(ltpWeight, ltdWeight, timeConstant, rewardWeight);
    }
    else {
        // Default to EnhancedSTDP for unknown rule names
        return createSTDP();
    }
}

} // namespace nlm