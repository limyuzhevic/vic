#pragma once

#include "PlasticityRule.hpp"
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Enhanced STDP parameters based on biological models
struct STDPParameters {
    // Standard STDP parameters (from experimental data)
    float ltpWeight;           // Long-term potentiation weight (~0.01-0.1)
    float ltdWeight;           // Long-term depression weight (~0.01-0.1)
    float tauPre;              // Pre-synaptic spike time constant (ms)
    float tauPost;             // Post-synaptic spike time constant (ms)
    float maxWeight;           // Maximum synaptic weight (biological constraint)
    float minWeight;           // Minimum synaptic weight (biological constraint)
    
    // Advanced STDP parameters
    float learningRate;        // Global learning rate (0.0-1.0)
    float normalizationFactor; // Weight normalization factor
    float metaplasticity;      // Metaplasticity parameter (changes learning rate over time)
    float baselineLTP;         // Baseline LTP magnitude
    float baselineLTD;         // Baseline LTD magnitude
    
    // Spike interaction parameters
    float tripletInteraction;  // Triplet interaction strength
    float axonalDelay;         // Axonal transmission delay (ms)
    
    STDPParameters() 
        : ltpWeight(0.02f), ltdWeight(0.015f), tauPre(20.0f), tauPost(20.0f),
          maxWeight(1.0f), minWeight(-1.0f), learningRate(1.0f),
          normalizationFactor(0.5f), metaplasticity(0.0f), baselineLTP(0.02f),
          baselineLTD(0.015f), tripletInteraction(0.0f), axonalDelay(1.0f) {}
};

// Spike pair-based STDP learning rule
struct STDPule {
    // Pair-wise STDP as described by Bi & Poo (1998) and others
    // Pre -> Post (LTP): if pre spike precedes post spike by Δt < 0
    // Post -> Pre (LTD): if post spike precedes pre spike by Δt > 0
    
    static float computeLTP(float delta_t) {
        if (delta_t <= 0.0f) return 0.0f;  // No LTP for simultaneous or later spikes
        return std::exp(-delta_t / STDPParameters::tauPre) * STDPParameters::ltpWeight;
    }
    
    static float computeLTD(float delta_t) {
        if (delta_t >= 0.0f) return 0.0f;  // No LTD for simultaneous or earlier spikes
        return std::exp(delta_t / STDPParameters::tauPost) * STDPParameters::ltdWeight;
    }
    
    static float computePairWeightChange(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         float currentWeight) {
        float weightChange = 0.0f;
        
        // For each pre-post pair
        for (const auto& preTime : preSpikes) {
            for (const auto& postTime : postSpikes) {
                float delta_t = static_cast<float>(postTime) - static_cast<float>(preTime) + STDPParameters::axonalDelay;
                weightChange += computeLTP(delta_t);
            }
        }
        
        // For each post-pre pair  
        for (const auto& postTime : postSpikes) {
            for (const auto& preTime : preSpikes) {
                float delta_t = static_cast<float>(preTime) - static_cast<float>(postTime) - STDPParameters::axonalDelay;
                weightChange += computeLTD(delta_t);
            }
        }
        
        return weightChange * STDPParameters::learningRate;
    }
};

// Triplet-based STDP for more complex spike interactions
struct TripletSTDPulse {
    // Enhanced STDP with triplet interactions (better biological accuracy)
    // Accounts for bursts of spikes and more complex temporal patterns
    
    static float computeTripletLTP(float t1_pre, float t2_pre, float t3_post) {
        if (t1_pre >= t2_pre || t2_pre >= t3_post) return 0.0f;
        
        float delta1 = t3_post - t2_pre;  // Pre-post pair
        float delta2 = t2_pre - t1_pre;  // Pre-pre pair
        
        float ltp1 = std::exp(-delta1 / STDPParameters::tauPre) * STDPParameters::ltpWeight;
        float ltp2 = std::exp(-delta2 / STDPParameters::tauPre) * STDPParameters::tripletInteraction;
        
        return ltp1 + ltp2;
    }
    
    static float computeTripletLTD(float t1_post, float t2_post, float t3_pre) {
        if (t1_post >= t2_post || t2_post >= t3_pre) return 0.0f;
        
        float delta1 = t3_pre - t2_post;  // Post-pre pair
        float delta2 = t2_post - t1_post;  // Post-post pair
        
        float ltd1 = std::exp(delta1 / STDPParameters::tauPost) * STDPParameters::ltdWeight;
        float ltd2 = std::exp(delta2 / STDPParameters::tauPost) * STDPParameters::tripletInteraction;
        
        return ltd1 + ltd2;
    }
}; 

// STDP: Spike-Timing-Dependent Plasticity
// Implements real biological STDP rules with enhanced features
class STDP : public PlasticityRule {
public:
    STDP() : pImpl(new Impl()) {
        pImpl->parameters = STDPParameters();
        pImpl->useTripletRule = false;
        pImpl->metaplasticityTimer = 0.0f;
        pImpl->totalWeightChange = 0.0f;
        pImpl->normFactor = 1.0f;
    }
    
    ~STDP() override {
        delete pImpl;
    }
    
    // Disable copying
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    
    // Enable moving
    STDP(STDP&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    STDP& operator=(STDP&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    // Main STDP update function
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        pImpl->totalWeightChange = 0.0f;
        pImpl->metaplasticityTimer += dt * 1000.0f;  // Convert to ms
        
        float currentWeight = synapse->getWeight();
        
        if (pImpl->useTripletRule) {
            pImpl->totalWeightChange = computeTripletWeightChange(preSpikes, postSpikes, currentWeight);
        } else {
            pImpl->totalWeightChange = computePairWeightChange(preSpikes, postSpikes, currentWeight);
        }
        
        // Apply metaplasticity (changes learning rate based on history)
        float plasticityModulator = computeMetaplasticityModulator();
        float finalWeightChange = pImpl->totalWeightChange * plasticityModulator;
        
        // Apply weight normalization and constraints
        float normalizedWeightChange = applyWeightNormalization(currentWeight, finalWeightChange);
        
        // Apply the weight change
        applyWeightChange(synapse, normalizedWeightChange);
        
        // Update eligibility trace for reward-modulated learning
        updateEligibilityTrace(synapse, normalizedWeightChange, dt);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // Apply biological weight constraints
        if (newWeight > pImpl->parameters.maxWeight) {
            newWeight = pImpl->parameters.maxWeight;
        } else if (newWeight < pImpl->parameters.minWeight) {
            newWeight = pImpl->parameters.minWeight;
        }
        
        // Apply normalization to maintain average weight
        applyNormalization(synapse, newWeight, currentWeight);
        
        synapse->setWeight(newWeight);
        
        // Log the weight change for analysis
        pImpl->weightChangeHistory.push_back({synapse->getId(), delta, newWeight});
        
        // Clean up history if too large
        if (pImpl->weightChangeHistory.size() > 1000) {
            pImpl->weightChangeHistory.erase(pImpl->weightChangeHistory.begin());
        }
    }
    
    const char* getName() const override {
        return "STDP";
    }
    
    // STDP parameter setters and getters
    void setLTPWeight(float weight) {
        pImpl->parameters.ltpWeight = weight;
    }
    
    float getLTPWeight() const {
        return pImpl->parameters.ltpWeight;
    }
    
    void setLTDWeight(float weight) {
        pImpl->parameters.ltdWeight = weight;
    }
    
    float getLTDWeight() const {
        return pImpl->parameters.ltdWeight;
    }
    
    void setTimeConstant(float tau) {
        pImpl->parameters.tauPre = tau;
        pImpl->parameters.tauPost = tau;
    }
    
    float getTimeConstant() const {
        return pImpl->parameters.tauPre;
    }
    
    void setPreTimeConstant(float tau) {
        pImpl->parameters.tauPre = tau;
    }
    
    float getPreTimeConstant() const {
        return pImpl->parameters.tauPre;
    }
    
    void setPostTimeConstant(float tau) {
        pImpl->parameters.tauPost = tau;
    }
    
    float getPostTimeConstant() const {
        return pImpl->parameters.tauPost;
    }
    
    void setMaxWeight(float maxWeight) {
        pImpl->parameters.maxWeight = maxWeight;
    }
    
    float getMaxWeight() const {
        return pImpl->parameters.maxWeight;
    }
    
    void setMinWeight(float minWeight) {
        pImpl->parameters.minWeight = minWeight;
    }
    
    float getMinWeight() const {
        return pImpl->parameters.minWeight;
    }
    
    void setLearningRate(float rate) {
        pImpl->parameters.learningRate = rate;
    }
    
    float getLearningRate() const {
        return pImpl->parameters.learningRate;
    }
    
    void setTripletRule(bool enable) {
        pImpl->useTripletRule = enable;
    }
    
    bool isTripletRule() const {
        return pImpl->useTripletRule;
    }
    
    void setNormalizationFactor(float factor) {
        pImpl->parameters.normalizationFactor = factor;
    }
    
    float getNormalizationFactor() const {
        return pImpl->parameters.normalizationFactor;
    }
    
    void setAxonalDelay(float delay) {
        pImpl->parameters.axonalDelay = delay;
    }
    
    float getAxonalDelay() const {
        return pImpl->parameters.axonalDelay;
    }
    
    // Update parameters from configuration
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float maxWeight = 1.0f, float minWeight = -1.0f,
                   float learningRate = 1.0f) {
        setLTPWeight(ltpWeight);
        setLTDWeight(ltdWeight);
        setTimeConstant(tau);
        setMaxWeight(maxWeight);
        setMinWeight(minWeight);
        setLearningRate(learningRate);
    }
    
    // Query STDP state and statistics
    float getTotalWeightChange() const {
        return pImpl->totalWeightChange;
    }
    
    float getMetaplasticityModulator() const {
        return computeMetaplasticityModulator();
    }
    
    size_t getWeightChangeHistorySize() const {
        return pImpl->weightChangeHistory.size();
    }
    
    // Get historical weight changes for analysis
    const std::vector<std::tuple<SynapseId, float, float>>& getWeightChangeHistory() const {
        return pImpl->weightChangeHistory;
    }
    
    // Reset STDP state
    void reset() {
        pImpl->totalWeightChange = 0.0f;
        pImpl->metaplasticityTimer = 0.0f;
        pImpl->weightChangeHistory.clear();
        pImpl->eligibilityTrace.clear();
    }
    
    // Get current STDP parameters
    const STDPParameters& getParameters() const {
        return pImpl->parameters;
    }
    
    void setParameters(const STDPParameters& params) {
        pImpl->parameters = params;
    }
    
private:
    struct Impl {
        STDPParameters parameters;
        bool useTripletRule;
        float metaplasticityTimer;
        float totalWeightChange;
        float normFactor;
        
        // History and statistics
        std::vector<std::tuple<SynapseId, float, float>> weightChangeHistory;
        std::unordered_map<SynapseId, float> eligibilityTrace;
        
        // Average weight tracking for normalization
        std::unordered_map<SynapseId, float> previousWeights;
        float globalAverageWeight;
        
        Impl() : parameters(), useTripletRule(false), metaplasticityTimer(0.0f),
                 totalWeightChange(0.0f), normFactor(1.0f), globalAverageWeight(0.0f) {}
    };
    
    Impl* pImpl;
    
    // Helper functions
    float computePairWeightChange(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  float currentWeight) {
        float totalChange = 0.0f;
        
        // Compute pair-wise STDP contributions
        for (const auto& preTime : preSpikes) {
            for (const auto& postTime : postSpikes) {
                float delta_t = static_cast<float>(postTime) - static_cast<float>(preTime) + 
                              pImpl->parameters.axonalDelay;
                
                // LTP: pre before post
                if (delta_t > 0.0f) {
                    float ltp = std::exp(-delta_t / pImpl->parameters.tauPre) * pImpl->parameters.ltpWeight;
                    totalChange += ltp;
                }
                
                // LTD: post before pre  
                else if (delta_t < 0.0f) {
                    float ltd = std::exp(delta_t / pImpl->parameters.tauPost) * pImpl->parameters.ltdWeight;
                    totalChange += ltd;
                }
            }
        }
        
        return totalChange * pImpl->parameters.learningRate;
    }
    
    float computeTripletWeightChange(const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     float currentWeight) {
        float totalChange = 0.0f;
        
        // Sort spikes by time for triplet processing
        std::vector<float> preTimes, postTimes;
        preTimes.reserve(preSpikes.size());
        postTimes.reserve(postSpikes.size());
        
        for (const auto& time : preSpikes) {
            preTimes.push_back(static_cast<float>(time));
        }
        for (const auto& time : postSpikes) {
            postTimes.push_back(static_cast<float>(time));
        }
        
        std::sort(preTimes.begin(), preTimes.end());
        std::sort(postTimes.begin(), postTimes.end());
        
        // Process triplet interactions
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = i + 1; j < preTimes.size(); ++j) {
                for (size_t k = 0; k < postTimes.size(); ++k) {
                    if (preTimes[i] < preTimes[j] && preTimes[j] < postTimes[k]) {
                        // Pre-pre-post triplet
                        float delta1 = postTimes[k] - preTimes[j];  // Pre-post
                        float delta2 = preTimes[j] - preTimes[i];  // Pre-pre
                        
                        float ltp1 = std::exp(-delta1 / pImpl->parameters.tauPre) * pImpl->parameters.ltpWeight;
                        float ltp2 = std::exp(-delta2 / pImpl->parameters.tauPre) * pImpl->parameters.tripletInteraction;
                        
                        totalChange += ltp1 + ltp2;
                    }
                }
            }
        }
        
        for (size_t i = 0; i < postTimes.size(); ++i) {
            for (size_t j = i + 1; j < postTimes.size(); ++j) {
                for (size_t k = 0; k < preTimes.size(); ++k) {
                    if (postTimes[i] < postTimes[j] && postTimes[j] < preTimes[k]) {
                        // Post-post-pre triplet
                        float delta1 = preTimes[k] - postTimes[j];  // Pre-post
                        float delta2 = postTimes[j] - postTimes[i];  // Post-post
                        
                        float ltd1 = std::exp(delta1 / pImpl->parameters.tauPost) * pImpl->parameters.ltdWeight;
                        float ltd2 = std::exp(delta2 / pImpl->parameters.tauPost) * pImpl->parameters.tripletInteraction;
                        
                        totalChange += ltd1 + ltd2;
                    }
                }
            }
        }
        
        return totalChange * pImpl->parameters.learningRate;
    }
    
    float computeMetaplasticityModulator() const {
        // Metaplasticity: learning rate adapts based on recent weight changes
        // High recent weight changes -> lower learning rate (stability)
        // Low recent weight changes -> higher learning rate (exploration)
        
        if (pImpl->metaplasticityTimer > 1000.0f) {  // Reset every second
            pImpl->metaplasticityTimer = 0.0f;
            // For now, simple implementation
            return 1.0f;
        }
        
        // Compute recent weight change magnitude
        float recentChange = 0.0f;
        for (const auto& change : pImpl->weightChangeHistory) {
            recentChange += std::abs(std::get<1>(change));
        }
        
        // Scale metaplasticity based on recent activity
        float activityLevel = std::min(recentChange / 1000.0f, 1.0f);
        float modulator = 1.0f - pImpl->parameters.metaplasticity * activityLevel;
        
        return std::max(0.1f, std::min(modulator, 2.0f));  // Keep in [0.1, 2.0]
    }
    
    float applyWeightNormalization(float currentWeight, float weightChange) {
        // Apply normalization to maintain system stability
        float normalizedChange = weightChange * pImpl->parameters.normalizationFactor;
        
        // Ensure change is not too large relative to current weight
        float relativeChange = std::abs(normalizedChange) / (std::abs(currentWeight) + 0.01f);
        if (relativeChange > 1.0f) {
            normalizedChange *= 1.0f / relativeChange;
        }
        
        return normalizedChange;
    }
    
    void applyNormalization(Synapse* synapse, float newWeight, float oldWeight) {
        // Maintain global average weight for network stability
        if (synapse) {
            // Update global average (simplified)
            pImpl->globalAverageWeight = (pImpl->globalAverageWeight * 0.9f + newWeight * 0.1f);
            
            // Keep synapse weights within reasonable bounds of global average
            float weightDiff = newWeight - pImpl->globalAverageWeight;
            if (std::abs(weightDiff) > pImpl->parameters.maxWeight * 0.5f) {
                // Clamp to reasonable range around global average
                float targetWeight = std::clamp(pImpl->globalAverageWeight,
                                              pImpl->parameters.minWeight,
                                              pImpl->parameters.maxWeight);
                float clampedNewWeight = (newWeight + targetWeight) * 0.5f;
                synapse->setWeight(clampedNewWeight);
            }
        }
    }
    
    void updateEligibilityTrace(Synapse* synapse, float weightChange, TimestepDuration dt) {
        if (!synapse) return;
        
        SynapseId synId = synapse->getId();
        float decayRate = 0.001f;  // Fast decay
        
        // Update eligibility trace
        if (pImpl->eligibilityTrace.find(synId) == pImpl->eligibilityTrace.end()) {
            pImpl->eligibilityTrace[synId] = 0.0f;
        }
        
        pImpl->eligibilityTrace[synId] += weightChange * (1.0f - decayRate * dt * 1000.0f);
        
        // Decay over time
        pImpl->eligibilityTrace[synId] *= (1.0f - decayRate);
        
        // Remove traces that are too small
        if (std::abs(pImpl->eligibilityTrace[synId]) < 0.0001f) {
            pImpl->eligibilityTrace.erase(synId);
        }
    }
}; 

// Reward-Modulated STDP (R-STDP)
// Combines STDP with reward prediction error signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP() : pImpl(new Impl()) {
        pImpl->baseSTDP = std::make_unique<STDP>();
        pImpl->rewardSignal = 0.0f;
        pImpl->predictionError = 0.0f;
        pImpl->learningRateMultiplier = 1.0f;
        pImpl->eligibilityDecayRate = 0.001f;
    }
    
    ~RewardModulatedSTDP() override {
        delete pImpl;
    }
    
    RewardModulatedSTDP(RewardModulatedSTDP&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    RewardModulatedSTDP& operator=(RewardModulatedSTDP&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse) return;
        
        // Update base STDP
        pImpl->baseSTDP->update(synapse, preSpikes, postSpikes, dt);
        
        // Get weight change from base STDP
        float weightChange = pImpl->baseSTDP->getTotalWeightChange();
        
        // Apply reward modulation
        float modulatedChange = weightChange * computeRewardModulation();
        
        // Apply the modulated weight change
        pImpl->baseSTDP->applyWeightChange(synapse, modulatedChange);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        // R-STDP modifies weight change based on reward signals
        float modulatedDelta = delta * pImpl->learningRateMultiplier;
        
        pImpl->baseSTDP->applyWeightChange(synapse, modulatedDelta);
    }
    
    const char* getName() const override {
        return "R-STDP";
    }
    
    // Reward-related methods
    void setRewardSignal(float reward) {
        pImpl->rewardSignal = reward;
    }
    
    float getRewardSignal() const {
        return pImpl->rewardSignal;
    }
    
    void setPredictionError(float error) {
        pImpl->predictionError = error;
    }
    
    float getPredictionError() const {
        return pImpl->predictionError;
    }
    
    void setLearningRateMultiplier(float multiplier) {
        pImpl->learningRateMultiplier = multiplier;
    }
    
    float getLearningRateMultiplier() const {
        return pImpl->learningRateMultiplier;
    }
    
    // Configure base STDP parameters
    void configureSTDP(float ltpWeight, float ltdWeight, float tau) {
        pImpl->baseSTDP->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Compute reward modulation factor
    float computeRewardModulation() const {
        // Combine reward prediction error with temporal difference learning
        float rewardModulation = 1.0f;
        
        if (pImpl->rewardSignal > 0.0f) {
            // Positive reward enhances learning
            rewardModulation *= (1.0f + pImpl->rewardSignal * 0.1f);
        } else if (pImpl->rewardSignal < 0.0f) {
            // Negative reward suppresses learning
            rewardModulation *= std::max(0.1f, 1.0f + pImpl->rewardSignal * 0.2f);
        }
        
        if (pImpl->predictionError != 0.0f) {
            // Prediction error modulates learning strength
            rewardModulation *= (1.0f + std::abs(pImpl->predictionError) * 0.5f);
        }
        
        return rewardModulation;
    }
    
    // Access to base STDP
    STDP* getBaseSTDP() {
        return pImpl->baseSTDP.get();
    }
    
    const STDP* getBaseSTDP() const {
        return pImpl->baseSTDP.get();
    }
    
private:
    struct Impl {
        std::unique_ptr<STDP> baseSTDP;
        float rewardSignal;
        float predictionError;
        float learningRateMultiplier;
        float eligibilityDecayRate;
        
        Impl() : rewardSignal(0.0f), predictionError(0.0f), learningRateMultiplier(1.0f),
                 eligibilityDecayRate(0.001f) {}
    };
    
    Impl* pImpl;
}; 

// Triplet STDP
// Advanced STDP with triplet spike interactions for better biological accuracy
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP() : pImpl(new Impl()) {
        pImpl->baseSTDP = std::make_unique<STDP>();
        pImpl->tripletStrength = 0.5f;
        pImpl->useAdvancedTriplet = true;
    }
    
    ~TripletSTDP() override {
        delete pImpl;
    }
    
    TripletSTDP(TripletSTDP&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    TripletSTDP& operator=(TripletSTDP&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse) return;
        
        // Enable triplet rule in base STDP
        pImpl->baseSTDP->setTripletRule(true);
        pImpl->baseSTDP->setLearningRate(pImpl->baseSTDP->getLearningRate() * pImpl->tripletStrength);
        
        // Update with triplet-based STDP
        pImpl->baseSTDP->update(synapse, preSpikes, postSpikes, dt);
        
        // Apply triplet-specific adjustments
        applyTripletAdjustments(synapse, preSpikes, postSpikes, dt);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        pImpl->baseSTDP->applyWeightChange(synapse, delta);
    }
    
    const char* getName() const override {
        return "TripletSTDP";
    }
    
    // Triplet-specific parameters
    void setTripletStrength(float strength) {
        pImpl->tripletStrength = strength;
    }
    
    float getTripletStrength() const {
        return pImpl->tripletStrength;
    }
    
    void setAdvancedTriplet(bool advanced) {
        pImpl->useAdvancedTriplet = advanced;
    }
    
    bool isAdvancedTriplet() const {
        return pImpl->useAdvancedTriplet;
    }
    
    // Configure base STDP
    void configure(float ltpWeight, float ltdWeight, float tau) {
        pImpl->baseSTDP->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Get base STDP reference
    STDP* getBaseSTDP() {
        return pImpl->baseSTDP.get();
    }
    
    const STDP* getBaseSTDP() const {
        return pImpl->baseSTDP.get();
    }
    
    // Apply triplet-specific adjustments
    void applyTripletAdjustments(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
        if (!synapse || !pImpl->useAdvancedTriplet) return;
        
        // Analyze spike timing patterns for triplet effects
        analyzeTripletPatterns(synapse, preSpikes, postSpikes, dt);
    }
    
    void analyzeTripletPatterns(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
        // Convert timestamps to float for easier processing
        std::vector<float> preTimes, postTimes;
        preTimes.reserve(preSpikes.size());
        postTimes.reserve(postSpikes.size());
        
        for (const auto& time : preSpikes) {
            preTimes.push_back(static_cast<float>(time));
        }
        for (const auto& time : postSpikes) {
            postTimes.push_back(static_cast<float>(time));
        }
        
        std::sort(preTimes.begin(), preTimes.end());
        std::sort(postTimes.begin(), postTimes.end());
        
        // Count different temporal patterns
        int prePrePost = 0;  // Pre-pre-post triplets
        int postPostPre = 0; // Post-post-pre triplets
        int prePost = 0;     // Pre-post pairs
        int postPre = 0;     // Post-pre pairs
        
        // Count pair patterns
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = 0; j < postTimes.size(); ++j) {
                if (preTimes[i] < postTimes[j]) {
                    prePost++;
                } else if (preTimes[i] > postTimes[j]) {
                    postPre++;
                }
            }
        }
        
        // Count triplet patterns
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = i + 1; j < preTimes.size(); ++j) {
                for (size_t k = 0; k < postTimes.size(); ++k) {
                    if (preTimes[i] < preTimes[j] && preTimes[j] < postTimes[k]) {
                        prePrePost++;
                    }
                }
            }
        }
        
        for (size_t i = 0; i < postTimes.size(); ++i) {
            for (size_t j = i + 1; j < postTimes.size(); ++j) {
                for (size_t k = 0; k < preTimes.size(); ++k) {
                    if (postTimes[i] < postTimes[j] && postTimes[j] < preTimes[k]) {
                        postPostPre++;
                    }
                }
            }
        }
        
        // Apply adjustments based on pattern analysis
        float adjustmentFactor = 1.0f;
        if (prePrePost > 0 || postPostPre > 0) {
            // Strong triplet patterns enhance learning
            adjustmentFactor += pImpl->tripletStrength * 0.5f;
        }
        
        if (prePost == 0 || postPre == 0) {
            // Weak pair patterns reduce learning
            adjustmentFactor *= 0.5f;
        }
        
        // Apply adjustment to synapse weight if significant patterns found
        if (adjustmentFactor != 1.0f && prePrePost + postPostPre > 0) {
            float currentWeight = synapse->getWeight();
            float adjustment = (adjustmentFactor - 1.0f) * currentWeight * 0.1f;
            float newWeight = currentWeight + adjustment;
            
            // Clamp to reasonable bounds
            newWeight = std::clamp(newWeight, pImpl->baseSTDP->getMinWeight(),
                                  pImpl->baseSTDP->getMaxWeight());
            
            synapse->setWeight(newWeight);
        }
    }
    
private:
    struct Impl {
        std::unique_ptr<STDP> baseSTDP;
        float tripletStrength;
        bool useAdvancedTriplet;
        
        Impl() : baseSTDP(std::make_unique<STDP>()), tripletStrength(0.5f),
                 useAdvancedTriplet(true) {}
    };
    
    Impl* pImpl;
};

} // namespace nlm
