#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// STDP implementation
struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() {
    delete pImpl;
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            
            if (dt_val > 0) {
                float delta = pImpl->ltpWeight * std::exp(-dt_val / tau);
                totalDelta += delta;
            } else if (dt_val < 0) {
                float delta = -pImpl->ltdWeight * std::exp(dt_val / tau);
                totalDelta += delta;
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        synapse->addToWeight(totalDelta);
        
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
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

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Reward-modulated STDP implementation
struct RewardModulatedSTDP::Impl {
    float learningRate;
    float traceDecay;
    float rewardModulation;
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), traceDecay(0.95f), rewardModulation(1.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() {
    delete pImpl;
}

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            
            if (dt_val > 0) {
                float delta = pImpl->learningRate * std::exp(-dt_val / 20.0f);
                totalDelta += delta;
            } else if (dt_val < 0) {
                float delta = -pImpl->learningRate * std::exp(dt_val / 20.0f);
                totalDelta += delta;
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float plasticityFactor = 1.0f;
        
        if (synapse->getPlasticityFlags().reward_modulated) {
            plasticityFactor = pImpl->rewardModulation;
        }
        
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy * plasticityFactor;
        
        applyWeightChange(synapse, totalDelta);
        
        float eligibility = synapse->getEligibilityTrace();
        eligibility = std::clamp(eligibility + totalDelta * pImpl->traceDecay, 
                                pImpl->minWeight, pImpl->maxWeight);
        synapse->setEligibilityTrace(eligibility);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 0.1f, 0.99f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::setRewardModulation(float modulation) {
    pImpl->rewardModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float RewardModulatedSTDP::getRewardModulation() const {
    return pImpl->rewardModulation;
}

// Triplet STDP implementation
struct TripletSTDP::Impl {
    float aPlus;          // Potentiation amplitude (A+)
    float aMinus;         // Depression amplitude (A-)
    float tauPlus;        // Potentiation time constant
    float tauMinus;       // Depression time constant
    float aPlusPrime;     // Triplet A+'
    float bPlus;          // Triplet B+
    float minWeight;
    float maxWeight;
    
    Impl() : aPlus(0.005f), aMinus(0.006f), tauPlus(20.0f), tauMinus(20.0f),
             aPlusPrime(0.002f), bPlus(0.003f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() {
    delete pImpl;
}

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.size() < 2 || postSpikes.size() < 2) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tauPlus = pImpl->tauPlus;
    float tauMinus = pImpl->tauMinus;
    
    for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
        for (size_t j = 0; j < postSpikes.size() - 1; ++j) {
            float dt_val = static_cast<float>(postSpikes[j] - preSpikes[i]);
            
            if (dt_val > 0) {
                float tripletTerm = pImpl->aPlus * std::exp(-dt_val / tauPlus);
                float preInterval = static_cast<float>(preSpikes[i+1] - preSpikes[i]);
                tripletTerm += pImpl->bPlus * std::exp(-(dt_val + preInterval) / tauPlus);
                totalDelta += tripletTerm;
            } else if (dt_val < 0) {
                float tripletTerm = -pImpl->aMinus * std::exp(dt_val / tauMinus);
                float postInterval = static_cast<float>(postSpikes[j+1] - postSpikes[j]);
                tripletTerm += pImpl->bPlus * std::exp(-(dt_val + postInterval) / tauMinus);
                totalDelta += tripletTerm;
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        applyWeightChange(synapse, totalDelta);
        
        float eligibility = synapse->getEligibilityTrace();
        eligibility = std::clamp(eligibility + totalDelta * 0.9f, 
                                pImpl->minWeight, pImpl->maxWeight);
        synapse->setEligibilityTrace(eligibility);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAPlus(float weight) {
    pImpl->aPlus = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAPlus() const {
    return pImpl->aPlus;
}

void TripletSTDP::setAMinus(float weight) {
    pImpl->aMinus = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAMinus() const {
    return pImpl->aMinus;
}

void TripletSTDP::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 5.0f, 50.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 5.0f, 50.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setAPlusPrime(float weight) {
    pImpl->aPlusPrime = std::clamp(weight, 0.0f, 0.05f);
}

float TripletSTDP::getAPlusPrime() const {
    return pImpl->aPlusPrime;
}

void TripletSTDP::setBPlus(float weight) {
    pImpl->bPlus = std::clamp(weight, 0.0f, 0.05f);
}

float TripletSTDP::getBPlus() const {
    return pImpl->bPlus;
}

} // namespace nlm