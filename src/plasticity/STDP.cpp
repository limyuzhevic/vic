#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <functional>

namespace nlm {

struct STDP::Impl {
    // Basic STDP parameters
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float timeConstant;           // Tau for exponential window (ms)
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    
    // Advanced STDP parameters
    float additiveLTPWeight;      // Additive LTP component
    float additiveLTDWeight;      // Additive LTD component
    float tau1;                   // First time constant
    float tau2;                   // Second time constant
    bool depressionNormalization;  // Normalization for depression
    float eligibilityTraceDecay;  // Eligibility trace decay rate
    
    // Learning curve parameters
    float learningRate;           // Base learning rate
    float forgettingRate;         // Weight decay rate
    
    // State variables
    float weightChangeSum;        // Total accumulated weight changes
    bool converged;               // Convergence flag
    
    // Reward modulation
    bool rewardModulated;         // Whether reward modulation is enabled
    float modulationFactor;       // Current modulation factor (0.0 to 1.0)
    float predictionErrorWeight;  // Weight for prediction error
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             additiveLTPWeight(0.005f), additiveLTDWeight(0.006f),
             tau1(20.0f), tau2(100.0f),
             depressionNormalization(true), eligibilityTraceDecay(0.01f),
             learningRate(0.01f), forgettingRate(0.001f),
             weightChangeSum(0.0f), converged(false),
             rewardModulated(false), modulationFactor(1.0f),
             predictionErrorWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    pImpl->weightChangeSum += totalDelta;  // Track total changes
    
    // Apply basic STDP
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                // Add additive component
                delta += pImpl->additiveLTPWeight * std::exp(-dt / (pImpl->tau1 * 2));
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                // Add additive component
                delta -= pImpl->additiveLTDWeight * std::exp(dt / (pImpl->tau2 * 2));
                totalDelta += delta;
            }
        }
    }
    
    // Apply forgetting (weight decay)
    totalDelta -= synapse->getWeight() * pImpl->forgettingRate;
    
    // Check for convergence
    if (std::abs(totalDelta) < 1e-8f) {
        pImpl->converged = true;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply reward modulation if enabled
        if (pImpl->rewardModulated) {
            totalDelta *= pImpl->modulationFactor;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * (1.0f - pImpl->eligibilityTraceDecay));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Update weight change sum
    pImpl->weightChangeSum += std::abs(delta);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::setAdditiveLTPWeight(float weight) {
    pImpl->additiveLTPWeight = std::clamp(weight, 0.0f, 0.1f);
}

float STDP::getAdditiveLTPWeight() const {
    return pImpl->additiveLTPWeight;
}

void STDP::setAdditiveLTDWeight(float weight) {
    pImpl->additiveLTDWeight = std::clamp(weight, 0.0f, 0.1f);
}

float STDP::getAdditiveLTDWeight() const {
    return pImpl->additiveLTDWeight;
}

void STDP::setMultiTimeConstant(float tau1, float tau2) {
    pImpl->tau1 = std::clamp(tau1, 5.0f, 200.0f);
    pImpl->tau2 = std::clamp(tau2, 10.0f, 500.0f);
}

void STDP::getMultiTimeConstant(float& tau1, float& tau2) const {
    tau1 = pImpl->tau1;
    tau2 = pImpl->tau2;
}

void STDP::setDepressionNormalization(bool enable) {
    pImpl->depressionNormalization = enable;
}

bool STDP::getDepressionNormalization() const {
    return pImpl->depressionNormalization;
}

void STDP::setEligibilityTraceDecay(float decayRate) {
    pImpl->eligibilityTraceDecay = std::clamp(decayRate, 0.0f, 1.0f);
}

float STDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::configureAdvanced(float ltpWeight, float ltdWeight, float tau1, float tau2,
                            bool normalizeDepression, float traceDecay) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setMultiTimeConstant(tau1, tau2);
    setDepressionNormalization(normalizeDepression);
    setEligibilityTraceDecay(traceDecay);
}

void STDP::enableRewardModulation(float predictionError) {
    pImpl->rewardModulated = true;
    pImpl->modulationFactor = std::clamp(predictionError, 0.0f, 2.0f);
    pImpl->predictionErrorWeight = predictionError;
}

void STDP::disableRewardModulation() {
    pImpl->rewardModulated = false;
    pImpl->modulationFactor = 1.0f;
}

bool STDP::isRewardModulated() const {
    return pImpl->rewardModulated;
}

float STDP::getModulationFactor() const {
    return pImpl->modulationFactor;
}

void STDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float STDP::getLearningRate() const {
    return pImpl->learningRate;
}

void STDP::setForgettingRate(float rate) {
    pImpl->forgettingRate = std::clamp(rate, 0.0f, 0.1f);
}

float STDP::getForgettingRate() const {
    return pImpl->forgettingRate;
}

void STDP::reset() {
    pImpl->weightChangeSum = 0.0f;
    pImpl->converged = false;
    pImpl->rewardModulated = false;
    pImpl->modulationFactor = 1.0f;
}

bool STDP::isConverged() const {
    return pImpl->converged;
}

float STDP::getWeightChangeSum() const {
    return pImpl->weightChangeSum;
}

void STDP::processSpikeTrains(const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            std::vector<std::pair<float, float>>& weightChanges) {
    weightChanges.clear();
    
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            float delta = 0.0f;
            if (dt > 0) {
                delta = pImpl->ltpWeight * std::exp(-dt / tau);
                delta += pImpl->additiveLTPWeight * std::exp(-dt / (pImpl->tau1 * 2));
            } else if (dt < 0) {
                delta = -pImpl->ltdWeight * std::exp(dt / tau);
                delta -= pImpl->additiveLTDWeight * std::exp(dt / (pImpl->tau2 * 2));
            }
            
            weightChanges.push_back(std::make_pair(dt, delta));
        }
    }
}

} // namespace nlm

// RewardModulatedSTDP implementation
struct RewardModulatedSTDP::Impl {
    float predictionErrorWeight;
    float temporalDifferenceFactor;
    size_t rewardHistorySize;
    float learningRate;
    
    std::vector<std::pair<Timestamp, float>> rewardHistory;
    float currentPredictionError;
    std::function<float(Timestamp)> rewardSource;
    
    Impl() : predictionErrorWeight(1.0f), temporalDifferenceFactor(0.1f),
             rewardHistorySize(100), learningRate(0.01f),
             currentPredictionError(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current prediction error
    float predError = getRewardPredictionError();
    
    // Apply standard STDP with modulation
    float baseChange = 0.0f;
    float tau = 20.0f; // Default time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta = static_cast<float>(postTime - preTime);
            
            if (delta > 0) {
                baseChange += pImpl->predictionErrorWeight * std::exp(-delta / tau);
            } else if (delta < 0) {
                baseChange -= pImpl->predictionErrorWeight * std::exp(delta / tau);
            }
        }
    }
    
    // Apply reward modulation
    float modulatedChange = baseChange * pImpl->temporalDifferenceFactor * predError;
    
    // Apply weight change
    if (std::abs(modulatedChange) > 1e-6f) {
        applyWeightChange(synapse, modulatedChange);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

void RewardModulatedSTDP::setTemporalDifferenceFactor(float factor) {
    pImpl->temporalDifferenceFactor = std::clamp(factor, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTemporalDifferenceFactor() const {
    return pImpl->temporalDifferenceFactor;
}

void RewardModulatedSTDP::setRewardHistorySize(size_t size) {
    pImpl->rewardHistorySize = size;
}

size_t RewardModulatedSTDP::getRewardHistorySize() const {
    return pImpl->rewardHistorySize;
}

void RewardModulatedSTDP::addReward(float reward, Timestamp time) {
    pImpl->rewardHistory.push_back(std::make_pair(time, reward));
    
    // Maintain history size
    if (pImpl->rewardHistory.size() > pImpl->rewardHistorySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Calculate prediction error (simplified: difference from average)
    if (!pImpl->rewardHistory.empty()) {
        float sum = 0.0f;
        for (const auto& entry : pImpl->rewardHistory) {
            sum += entry.second;
        }
        float avg = sum / pImpl->rewardHistory.size();
        pImpl->currentPredictionError = reward - avg;
    }
}

void RewardModulatedSTDP::clearRewards() {
    pImpl->rewardHistory.clear();
    pImpl->currentPredictionError = 0.0f;
}

float RewardModulatedSTDP::getAverageReward() const {
    if (pImpl->rewardHistory.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& entry : pImpl->rewardHistory) {
        sum += entry.second;
    }
    return sum / pImpl->rewardHistory.size();
}

float RewardModulatedSTDP::getRewardPredictionError() const {
    return pImpl->currentPredictionError;
}

void RewardModulatedSTDP::setRewardSource(std::function<float(Timestamp)> source) {
    pImpl->rewardSource = source;
}

} // namespace nlm

// TripletSTDP implementation
struct TripletSTDP::Impl {
    float pairingStrength;
    float postBeforePreFactor;
    float preBeforePostFactor;
    bool threeSpikeRulesEnabled;
    float preWindow;
    float postWindow;
    
    Impl() : pairingStrength(1.0f), postBeforePreFactor(1.0f),
             preBeforePostFactor(1.0f), threeSpikeRulesEnabled(true),
             preWindow(100.0f), postWindow(100.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Apply pairing rules (pre-post interactions)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta = static_cast<float>(postTime - preTime);
            
            if (delta > 0) {
                // Pre-before-post pairing
                totalDelta += pImpl->preBeforePostFactor * 
                             std::exp(-delta / pImpl->postWindow);
            } else if (delta < 0) {
                // Post-before-pre pairing
                totalDelta -= pImpl->postBeforePreFactor * 
                             std::exp(delta / pImpl->preWindow);
            }
        }
    }
    
    // Apply triplet rules if enabled
    if (pImpl->threeSpikeRulesEnabled) {
        // This would be more complex with actual triplet spike detection
        // For now, just add a simple scaling factor
        totalDelta *= pImpl->pairingStrength;
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void TripletSTDP::setPairingStrength(float strength) {
    pImpl->pairingStrength = std::clamp(strength, 0.0f, 5.0f);
}

float TripletSTDP::getPairingStrength() const {
    return pImpl->pairingStrength;
}

void TripletSTDP::setPostBeforePreFactor(float factor) {
    pImpl->postBeforePreFactor = std::clamp(factor, 0.0f, 5.0f);
}

float TripletSTDP::getPostBeforePreFactor() const {
    return pImpl->postBeforePreFactor;
}

void TripletSTDP::setPreBeforePostFactor(float factor) {
    pImpl->preBeforePostFactor = std::clamp(factor, 0.0f, 5.0f);
}

float TripletSTDP::getPreBeforePostFactor() const {
    return pImpl->preBeforePostFactor;
}

void TripletSTDP::enableThreeSpikeRules(bool enable) {
    pImpl->threeSpikeRulesEnabled = enable;
}

bool TripletSTDP::areThreeSpikeRulesEnabled() const {
    return pImpl->threeSpikeRulesEnabled;
}

void TripletSTDP::setTripleWindows(float preWindow, float postWindow) {
    pImpl->preWindow = std::clamp(preWindow, 1.0f, 1000.0f);
    pImpl->postWindow = std::clamp(postWindow, 1.0f, 1000.0f);
}

void TripletSTDP::getTripleWindows(float& preWindow, float& postWindow) const {
    preWindow = pImpl->preWindow;
    postWindow = pImpl->postWindow;
}

} // namespace nlm