#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Advanced STDP parameters
    float tripletU;       // Triplet potentiation parameter
    float tripletV;       // Triplet depression parameter
    float tripletW;       // Triplet interaction parameter
    float softSigma;      // Soft STDP sigma parameter
    float homeostaticRate; // Homeostatic plasticity rate
    float learningRate;    // Overall learning rate
    float adaptationRate;  // Meta-learning adaptation rate
    
    // Reward modulation parameters
    float rewardGain;      // Reward signal gain
    float eligibilityDecay; // Eligibility trace decay
    
    // Biological accuracy parameters
    float nmdaThreshold;   // NMDA receptor activation threshold
    float calciumPeak;     // Peak calcium concentration
    float calciumDecay;    // Calcium decay rate
    float backpropStrength; // Backpropagation strength
    
    // Learning state
    float currentLearningRate;
    float performanceHistory;
    float uncertaintyEstimate;
    float lastAdaptationStep;
    std::vector<float> eligibilityTrace;
    
    // Biological parameters
    struct CalciumDynamics {
        float concentration;
        float timeConstant;
        float threshold;
        
        CalciumDynamics() : concentration(0.0f), timeConstant(50.0f), threshold(1.0f) {}
    };
    
    CalciumDynamics calcium;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             tripletU(0.005f), tripletV(0.005f), tripletW(0.0025f),
             softSigma(0.5f), homeostaticRate(0.01f), learningRate(1.0f),
             adaptationRate(0.001f), rewardGain(1.0f), eligibilityDecay(0.1f),
             nmdaThreshold(-55.0f), calciumPeak(1.0f), calciumDecay(0.1f),
             backpropStrength(0.3f), currentLearningRate(1.0f),
             performanceHistory(0.0f), uncertaintyEstimate(1.0f),
             lastAdaptationStep(0.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

const char* STDP::getName() const {
    return "AdvancedSTDP";
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    pImpl->ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
    pImpl->ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
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
    
    // Basic pairwise STDP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                totalDelta += pImpl->ltpWeight * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Post before pre: depression
                totalDelta += -pImpl->ltdWeight * std::exp(dt / tau);
            }
        }
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy * pImpl->learningRate;
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * (1.0f - pImpl->eligibilityDecay));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply biological constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply NMDA receptor threshold effect
    if (newWeight > 0.0f && newWeight < 0.1f) {
        // Near threshold region
        newWeight *= pImpl->nmdaThreshold;
    }
    
    // Apply calcium dynamics
    float calciumEffect = calculateCalciumEffect(synapse);
    newWeight *= calciumEffect;
    
    // Apply bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

void STDP::setParameters(const STDPParameters& params) {
    pImpl->ltpWeight = params.ltpWeight;
    pImpl->ltdWeight = params.ltdWeight;
    pImpl->timeConstant = params.tau;
    pImpl->tripletU = params.tripletU;
    pImpl->tripletV = params.tripletV;
    pImpl->tripletW = params.tripletW;
    pImpl->softSigma = params.softSigma;
    pImpl->homeostaticRate = params.homeostaticRate;
    pImpl->learningRate = params.learningRate;
    pImpl->adaptationRate = params.adaptationRate;
    pImpl->rewardGain = params.rewardGain;
    pImpl->eligibilityDecay = params.eligibilityDecay;
}

const STDP::STDPParameters& STDP::getParameters() const {
    // Return default parameters (in a real implementation, these would be stored)
    static STDPParameters params;
    params.ltpWeight = pImpl->ltpWeight;
    params.ltdWeight = pImpl->ltdWeight;
    params.tau = pImpl->timeConstant;
    params.tripletU = pImpl->tripletU;
    params.tripletV = pImpl->tripletV;
    params.tripletW = pImpl->tripletW;
    params.softSigma = pImpl->softSigma;
    params.homeostaticRate = pImpl->homeostaticRate;
    params.learningRate = pImpl->learningRate;
    params.adaptationRate = pImpl->adaptationRate;
    params.rewardGain = pImpl->rewardGain;
    params.eligibilityDecay = pImpl->eligibilityDecay;
    return params;
}

void STDP::setBiologicalParameters(const BiologicalParameters& params) {
    pImpl->nmdaThreshold = params.nmdaThreshold;
    pImpl->calciumPeak = params.calciumPeak;
    pImpl->calciumDecay = params.calciumDecay;
    pImpl->backpropStrength = params.backpropStrength;
}

const STDP::BiologicalParameters& STDP::getBiologicalParameters() const {
    static BiologicalParameters params;
    params.nmdaThreshold = pImpl->nmdaThreshold;
    params.calciumPeak = pImpl->calciumPeak;
    params.calciumDecay = pImpl->calciumDecay;
    params.backpropStrength = pImpl->backpropStrength;
    return params;
}

void STDP::adaptLearningRate(float performanceError, float confidence) {
    // Adaptive learning rate based on performance and confidence
    float adaptation = pImpl->adaptationRate * performanceError * confidence;
    pImpl->currentLearningRate += adaptation;
    
    // Clamp learning rate
    pImpl->currentLearningRate = std::clamp(pImpl->currentLearningRate, 0.01f, 2.0f);
    
    // Update performance history
    pImpl->performanceHistory = 0.9f * pImpl->performanceHistory + 0.1f * performanceError;
    
    // Update uncertainty estimate
    pImpl->uncertaintyEstimate = 0.9f * pImpl->uncertaintyEstimate + 0.1f * (1.0f - confidence);
}

float STDP::getCurrentLearningRate() const {
    return pImpl->currentLearningRate;
}

STDP::LearningState STDP::getLearningState() const {
    LearningState state;
    state.cumulativeLearning = pImpl->currentLearningRate;
    state.performanceHistory = pImpl->performanceHistory;
    state.uncertaintyEstimate = pImpl->uncertaintyEstimate;
    state.lastAdaptationStep = pImpl->lastAdaptationStep;
    state.eligibilityTrace = pImpl->eligibilityTrace;
    return state;
}

void STDP::updateLearningState(const LearningState& state) {
    pImpl->currentLearningRate = state.cumulativeLearning;
    pImpl->performanceHistory = state.performanceHistory;
    pImpl->uncertaintyEstimate = state.uncertaintyEstimate;
    pImpl->lastAdaptationStep = state.lastAdaptationStep;
    pImpl->eligibilityTrace = state.eligibilityTrace;
}

void STDP::calculateTripletContribution(std::vector<float>& contributions,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    // Implement triplet STDP calculation
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            for (size_t k = 0; k < postSpikes.size(); ++k) {
                if (i != j && j != k) {
                    float t1 = static_cast<float>(preSpikes[i]);
                    float t2 = static_cast<float>(postSpikes[j]);
                    float t3 = static_cast<float>(postSpikes[k]);
                    
                    // Calculate triplet interactions
                    float triplet1 = pImpl->tripletU * std::exp(-(t2 - t1) / pImpl->timeConstant);
                    float triplet2 = pImpl->tripletV * std::exp(-(t3 - t2) / pImpl->timeConstant);
                    float triplet3 = pImpl->tripletW * std::exp(-(t3 - t1) / pImpl->timeConstant);
                    
                    contributions.push_back(triplet1 + triplet2 + triplet3);
                }
            }
        }
    }
}

void STDP::calculateNeuromodulatedEligibility(float& eligibility,
                                               const std::vector<Timestamp>& preSpikes,
                                               const std::vector<Timestamp>& postSpikes,
                                               TimestepDuration dt,
                                               float reward,
                                               float predictionError) {
    // Calculate eligibility trace with reward modulation
    float rewardComponent = reward * pImpl->rewardGain;
    float errorComponent = predictionError * pImpl->rewardGain;
    
    eligibility = rewardComponent + errorComponent;
    eligibility *= (1.0f - pImpl->eligibilityDecay);
}

void STDP::calculateHomeostaticAdjustment(float& adjustment,
                                           const std::vector<Synapse*>& allSynapses,
                                           float targetRatio) {
    // Calculate homeostatic adjustment to maintain E/I balance
    float currentEiRatio = calculateCurrentEIRatio(allSynapses);
    float deviation = currentEiRatio - targetRatio;
    
    adjustment = deviation * pImpl->homeostaticRate;
}

float STDP::calculateCalciumEffect(Synapse* synapse) const {
    // Calculate calcium dynamics effect on synaptic plasticity
    float calciumEffect = 1.0f;
    
    if (synapse) {
        // Get calcium concentration from synapse if available
        // For now, use a simplified model
        float calciumInput = std::abs(synapse->getWeight()) * pImpl->calciumPeak;
        pImpl->calcium.concentration += calciumInput * pImpl->calciumDecay;
        
        if (pImpl->calcium.concentration > pImpl->calcium.threshold) {
            calciumEffect = pImpl->calcium.concentration / pImpl->calcium.threshold;
        }
    }
    
    return calciumEffect;
}

float STDP::calculateCurrentEIRatio(const std::vector<Synapse*>& synapses) const {
    // Calculate current excitation/inhibition ratio
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto* synapse : synapses) {
        if (synapse) {
            float weight = synapse->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    } else if (totalExcitatory > 0.0f) {
        return std::numeric_limits<float>::infinity();
    } else {
        return 1.0f;
    }
}

} // namespace nlm