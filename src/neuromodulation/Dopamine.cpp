#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    // Neuromodulation dynamics
    float level;                    // Current dopamine concentration
    float tonicLevel;               // Baseline tonic firing level
    float phasicGain;               // Phasic response gain factor
    float baseline;                 // Adaptive baseline

    // Reward prediction error system
    float currentRPE;               // Current reward prediction error
    float predictedReward;          // Predicted reward value
    float temporalDifference;       // Temporal difference error
    float discountFactor;           // Discount factor for RPE computation

    // Eligibility traces for temporal difference learning
    float eligibilityTrace;         // Eligibility trace value for TD learning
    float traceDecayRate;           // Decay rate for eligibility traces

    // Time-dependent dynamics
    float decayRate;                // Exponential decay rate
    float recoveryRate;              // Recovery toward baseline
    float peakLevel;                // Peak achievable level

    // Plasticity effects
    float plasticityFactor;         // Modulation factor for synaptic plasticity
    float stdpModulation;           // STDP-specific modulation
    float hebbianModulation;        // Hebbian-specific modulation

    // Biological plausibility parameters
    float STP potentiation;         // Short-term potentiation factor
    float STP depression;           // Short-term depression factor
    float LTP_threshold;            // LTP threshold for synaptic modification
    float LTD_threshold;            // LTD threshold for synaptic modification

    // Historical tracking
    std::vector<float> history;     // Dopamine level history
    std::vector<float> rewardHistory; // Reward history for homeostatic regulation
    SimulationStep lastPhasicReleaseStep; // Time of last phasic release

    // Integration state
    bool isIntegrated;              // Whether integrated with brain systems
    Brain* brainReference;          // Reference to brain for system integration
    NeuralWorkingMemory* workingMemory; // Working memory integration
    Curiosity* curiosity;           // Curiosity system integration
    PredictionError* predictionError; // Prediction error integration

    // Configuration parameters
    float homeostaticTarget;        // Target level for homeostatic regulation
    float homeostaticRate;          // Speed of homeostatic adjustment
    float explorationBonus;         // Bonus for exploratory behavior
    float exploitationBonus;        // Bonus for exploitative behavior

    Impl() 
        : level(0.0f)
        , tonicLevel(0.05f)           // Biological: ~5% of peak for tonic firing
        , phasicGain(2.0f)            // Biological: phasic responses can be 2-5x baseline
        , baseline(tonicLevel)
        , currentRPE(0.0f)
        , predictedReward(0.0f)
        , temporalDifference(0.0f)
        , discountFactor(0.99f)
        , eligibilityTrace(0.0f)
        , traceDecayRate(0.01f)
        , decayRate(0.1f)            // Biological: ~100ms decay time constant
        , recoveryRate(0.05f)         // Slower recovery to maintain baseline
        , peakLevel(1.0f)            // Normalized peak level
        , plasticityFactor(1.0f)
        , stdpModulation(1.0f)
        , hebbianModulation(1.0f)
        , STP potentiation(1.05f)    // 5% potentiation per spike
        , STP depression(0.95f)      // 5% depression per spike
        , LTP_threshold(0.0f)        // Will be set adaptively
        , LTD_threshold(0.0f)        // Will be set adaptively
        , lastPhasicReleaseStep(0)
        , isIntegrated(false)
        , brainReference(nullptr)
        , workingMemory(nullptr)
        , curiosity(nullptr)
        , predictionError(nullptr)
        , homeostaticTarget(tonicLevel)
        , homeostaticRate(0.001f)     // Slow homeostatic adjustment
        , explorationBonus(0.2f)
        , exploitationBonus(0.8f) {
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Clamp to biological range
    pImpl->level = std::clamp(level, 0.0f, pImpl->peakLevel);

    // Record in history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Dopamine::getPlasticityFactor() const {
    // Biological: Dopamine modulates plasticity in a non-linear manner
    // Low levels suppress LTP, high levels enhance both LTP and LTD
    float normalizedLevel = pImpl->level / pImpl->peakLevel;

    if (normalizedLevel < 0.3f) {
        return 0.8f + 0.2f * normalizedLevel;  // Suppress plasticity at low levels
    } else {
        return 1.0f + 0.5f * (normalizedLevel - 0.3f) / 0.7f;  // Enhance at high levels
    }
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);

    // 1. Apply time-dependent decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);

    // 2. Simulate tonic firing
    simulateTonicFiring(dt);

    // 3. Simulate phasic release if needed
    simulatePhasicRelease();

    // 4. Apply homeostatic regulation
    if (!pImpl->rewardHistory.empty()) {
        float avgReward = std::accumulate(pImpl->rewardHistory.begin(),
                                         pImpl->rewardHistory.end(), 0.0f) /
                          static_cast<float>(pImpl->rewardHistory.size());
        applyHomeostaticRegulation(avgReward, 0.01f);
    }

    // 5. Decay eligibility trace
    pImpl->eligibilityTrace *= std::pow(1.0f - pImpl->traceDecayRate, dtFloat);
}

void Dopamine::signalReward(float reward) {
    // Store in reward history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }

    // Phasic dopamine release proportional to reward
    float phasicRelease = reward * pImpl->phasicGain;
    setLevel(std::min(pImpl->peakLevel, pImpl->level + phasicRelease));

    NLM_LOG_INFO("Dopamine: Reward signal received: " + std::to_string(reward) +
                 ", level: " + std::to_string(getLevel()));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Update prediction error
    pImpl->currentRPE = error;
    pImpl->temporalDifference = error;

    // Integrate with prediction error system if available
    if (pImpl->predictionError) {
        pImpl->predictionError->computeError(pImpl->predictedReward,
                                            pImpl->predictedReward + error);
    }

    // Apply RPE to dopamine level with biological constraints
    // Positive RPE increases dopamine (reward prediction), negative decreases (punishment prediction)
    float rpeEffect = error * pImpl->phasicGain * 0.5f;  // Half the gain for RPE signaling
    setLevel(std::max(0.0f, pImpl->level + rpeEffect));

    // Update eligibility trace
    updateEligibilityTrace(error, error);

    NLM_LOG_INFO("Dopamine: RPE signal: " + std::to_string(error) +
                 ", current level: " + std::to_string(getLevel()));
}

void Dopamine::setTonicLevel(float level) {
    pImpl->tonicLevel = std::clamp(level, 0.0f, pImpl->peakLevel);
    pImpl->baseline = pImpl->tonicLevel;
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = std::clamp(gain, 1.0f, 10.0f);  // Biological range
}

float Dopamine::getTonicLevel() const {
    return pImpl->tonicLevel;
}

float Dopamine::getPhasicGain() const {
    return pImpl->phasicGain;
}

void Dopamine::adaptBaseline(float targetLevel, float rate) {
    // Biological: Baseline adapts slowly toward target level
    pImpl->baseline += rate * (targetLevel - pImpl->baseline);
    pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, pImpl->peakLevel);
}

void Dopamine::applyHomeostaticRegulation(float rewardHistory, float timeConstant) {
    // Implement homeostatic regulation
    // Target maintains reward prediction accuracy while avoiding runaway excitation
    float homeostaticTarget = std::min(1.0f, 0.5f + 0.5f * std::abs(rewardHistory));

    // Slowly adjust baseline toward homeostatic target
    adaptBaseline(homeostaticTarget, homeostaticRate);
}

void Dopamine::updateEligibilityTrace(float reward, float temporalDifference) {
    // Implement eligibility trace for temporal difference learning
    // Trace marks synapses that have been activated and are eligible for plasticity

    // Update trace: add new eligibility based on reward prediction
    pImpl->eligibilityTrace += reward * temporalDifference * 0.1f;

    // Constrain trace to biological limits
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::clearEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

void Dopamine::simulatePhasicRelease() {
    // Spike-based phasic dopamine release
    // Simulate the burst firing that occurs in response to rewards or prediction errors

    // Simple Poisson-like process for phasic release
    static float phasicReleaseProbability = 0.01f;  // 1% chance per update
    static float phasicMagnitude = 0.3f;           // Typical phasic magnitude

    // This would be replaced with actual spike detection in a full implementation
    // For now, simulate occasional phasic bursts
    if (rand() / (float)RAND_MAX < phasicReleaseProbability) {
        setLevel(std::min(pImpl->peakLevel, pImpl->level + phasicMagnitude));
        pImpl->lastPhasicReleaseStep = pImpl->brainReference ?
            pImpl->brainReference->getCurrentStep() : 0;
    }
}

void Dopamine::simulateTonicFiring(TimestepDuration dt) {
    // Tonic baseline dopamine firing
    // Represents the continuous low-level dopamine tone

    float dtFloat = static_cast<float>(dt);

    // Tonic firing adds to current level
    // In biology, tonic firing is continuous but modulated by various inputs
    float tonicInflux = pImpl->tonicLevel * dtFloat * 0.01f;  // Scale to match update rate

    // Apply with some noise to simulate biological variability
    float noise = (rand() / (float)RAND_MAX - 0.5f) * 0.1f;
    float newLevel = pImpl->level + tonicInflux + noise;

    // Constrain to reasonable bounds
    newLevel = std::clamp(newLevel, 0.0f, pImpl->peakLevel);

    // Blend with existing level for smooth transition
    pImpl->level = 0.9f * pImpl->level + 0.1f * newLevel;
}

void Dopamine::integrateWithWorkingMemory(NeuralWorkingMemory* workingMemory) {
    pImpl->workingMemory = workingMemory;

    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Dopamine integrated with working memory system");
    }
}

void Dopamine::integrateWithCuriosity(Curiosity* curiosity) {
    pImpl->curiosity = curiosity;

    if (pImpl->curiosity) {
        // Initialize curiosity system based on current dopamine level
        pImpl->curiosity->update(getLevel(), pImpl->currentRPE, 0.001f);
        NLM_LOG_INFO("Dopamine integrated with curiosity system");
    }
}

void Dopamine::integrateWithPredictionError(PredictionError* predictionError) {
    pImpl->predictionError = predictionError;

    if (pImpl->predictionError) {
        NLM_LOG_INFO("Dopamine integrated with prediction error system");
    }
}

void Dopamine::integrateWithBrainSystems(Brain* brain) {
    pImpl->brainReference = brain;
    pImpl->isIntegrated = true;

    if (pImpl->brainReference) {
        // Register with brain systems
        if (pImpl->workingMemory) {
            // Working memory already integrated
        }
        if (pImpl->curiosity) {
            // Curiosity system already integrated
        }
        if (pImpl->predictionError) {
            // Prediction error system already integrated
        }

        NLM_LOG_INFO("Dopamine fully integrated with brain systems");
    }
}

void Dopamine::modulateSTDP(STDP* stdp, float dopamineLevel, TimestepDuration dt) {
    if (!stdp) return;

    // Biological: Dopamine modulates STDP in a bidirectional manner
    // Moderate levels enhance LTP, very high levels also enhance LTD

    float modulation = dopamineLevel;

    if (modulation < 0.3f) {
        // Low dopamine suppresses STDP
        stdp->setLTPWeight(stdp->getLTPWeight() * 0.5f);
        stdp->setLTDWeight(stdp->getLTDWeight() * 0.5f);
    } else if (modulation < 0.7f) {
        // Moderate dopamine enhances LTP
        stdp->setLTPWeight(stdp->getLTPWeight() * 1.2f);
        stdp->setLTDWeight(stdp->getLTDWeight() * 1.0f);
    } else {
        // High dopamine enhances LTD (prevents runaway excitation)
        stdp->setLTPWeight(stdp->getLTPWeight() * 1.0f);
        stdp->setLTDWeight(stdp->getLTDWeight() * 1.2f);
    }

    // Apply plasticity factor
    pImpl->stdpModulation = getPlasticityFactor() * 0.5f + 0.5f;
}

void Dopamine::modulateHebbian(Hebbian* hebbian, float dopamineLevel, TimestepDuration dt) {
    if (!hebbian) return;

    // Biological: Dopamine enhances Hebbian learning proportionally to its level
    float learningRate = hebbian->getLearningRate();

    // Dopamine-dependent learning rate modulation
    float dopamineModulation = 1.0f + 2.0f * dopamineLevel;  // Up to 3x enhancement
    hebbian->setLearningRate(std::clamp(learningRate * dopamineModulation, 0.0f, 1.0f));

    pImpl->hebbianModulation = dopamineModulation;
}

void Dopamine::modulateStructuralPlasticity(StructuralPlasticity* structuralPlasticity,
                                          float dopamineLevel, TimestepDuration dt) {
    if (!structuralPlasticity) return;

    // Biological: Dopamine increases structural plasticity rate
    // Higher dopamine leads to more synaptogenesis and pruning

    float baseRate = structuralPlasticity->getSynaptogenesisRate();
    float pruningBase = structuralPlasticity->getPruningRate();

    // Dopamine enhances both creation and pruning
    float enhancement = 1.0f + 5.0f * dopamineLevel;  // Up to 6x enhancement

    structuralPlasticity->setSynaptogenesisRate(std::clamp(baseRate * enhancement, 0.0f, 0.1f));
    structuralPlasticity->setPruningRate(std::clamp(pruningBase * enhancement, 0.0f, 0.01f));
}

void Dopamine::enhanceNeuralExcitability(std::vector<Neuron*>& neurons, float dopamineLevel) {
    // Biological: Dopamine modulates neural excitability through multiple mechanisms
    // D1 receptors increase excitability, D2 receptors decrease it

    float excitabilityMod = dopamineLevel * 0.5f;  // Up to 0.5x additional current

    for (auto* neuron : neurons) {
        if (neuron) {
            // Inject current proportional to dopamine level
            // This models D1 receptor-mediated enhancement of neuronal firing
            neuron->injectCurrent(excitabilityMod * (0.5f + rand() / (float)RAND_MAX * 0.5f));
        }
    }
}

void Dopamine::computeRewardPredictionError(float predicted, float actual, float discountFactor) {
    pImpl->predictedReward = predicted;
    pImpl->discountFactor = discountFactor;

    // Compute RPE using temporal difference learning formula
    // RPE = δ = r + γV(s') - V(s)
    // In simplified form: RPE = actual - predicted

    pImpl->currentRPE = actual - predicted;
    pImpl->temporalDifference = pImpl->currentRPE;

    // Signal the computed RPE
    signalRewardPredictionError(pImpl->currentRPE);
}

float Dopamine::getCurrentRPE() const {
    return pImpl->currentRPE;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

void Dopamine::applySTPLTP(float weight, float timeDifference) {
    // Implement STDP LTP rule
    // If pre fires before post (positive Δt), potentiate synapse

    if (timeDifference > 0.0f) {
        float ltp = pImpl->STP potentiation * std::exp(-timeDifference / 20.0f);  // 20ms time constant
        float newWeight = weight + ltp;
        // Would apply to actual synapse
    }
}

void Dopamine::applySTPLTD(float weight, float timeDifference) {
    // Implement STDP LTD rule
    // If post fires before pre (negative Δt), depress synapse

    if (timeDifference < 0.0f) {
        float ltd = pImpl->STP depression * std::exp(timeDifference / 20.0f);  // 20ms time constant
        float newWeight = weight + ltd;
        // Would apply to actual synapse
    }
}

void Dopamine::applyShortTermPlasticity(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;

    // Implement short-term plasticity (STP)
    // Model facilitation and depression

    float dtFloat = static_cast<float>(dt);
    float currentWeight = synapse->getWeight();

    // Simple STP model: alternating potentiation and depression
    // This would be replaced with more biologically realistic models
    static float stpPhase = 0.0f;
    stpPhase = std::fmod(stpPhase + dtFloat * 0.1f, 2.0f * M_PI);

    float stpEffect = 0.1f * std::sin(stpPhase);
    float newWeight = currentWeight * (1.0f + stpEffect);
    synapse->setWeight(std::clamp(newWeight, -1.0f, 1.0f));
}

void Dopamine::applyLongTermPotentiation(Synapse* synapse, float LTPAmount) {
    if (!synapse) return;

    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + LTPAmount;
    synapse->setWeight(std::clamp(newWeight, -1.0f, 1.0f));

    NLM_LOG_INFO("Dopamine: LTP applied, weight: " + std::to_string(newWeight));
}

void Dopamine::applyLongTermDepression(Synapse* synapse, float LTDAmount) {
    if (!synapse) return;

    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight - LTDAmount;
    synapse->setWeight(std::clamp(newWeight, -1.0f, 1.0f));

    NLM_LOG_INFO("Dopamine: LTD applied, weight: " + std::to_string(newWeight));
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

float Dopamine::getPeakLevel() const {
    return pImpl->peakLevel;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.01f, 1.0f);
}

void Dopamine::resetHistory() {
    pImpl->history.clear();
    pImpl->rewardHistory.clear();
}

bool Dopamine::isIntegrated() const {
    return pImpl->isIntegrated;
}

void Dopamine::markIntegrated(bool integrated) {
    pImpl->isIntegrated = integrated;
}

} // namespace nlm