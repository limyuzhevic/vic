#include "Endorphins.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct EndorphinsImpl {
    // Core neuromodulator parameters
    float level;                    // Current concentration/activity
    float baseline;                 // Baseline level
    float peak;                     // Maximum achievable level
    float decayRate;                // Natural decay rate
    float releaseRate;              // Release rate for phasic signals
    float synthesisRate;            // Synthesis/production rate
    float reuptakeRate;             // Reuptake/metabolism rate
    float receptorDensity;          // Receptor density for sensitivity
    float bindingAffinity;          // Binding affinity to receptors
    
    // Pain and reward parameters
    float painModulation;            // Pain perception modulation
    float pleasureModulation;        // Pleasure enhancement
    float rewardProcessing;           // Reward processing efficiency
    float hedonicResponse;            // Hedonic response level
    
    // Stress and homeostasis
    float stressReduction;           // Stress reduction level
    float homeostaticBalance;        // Homeostatic balance
    float energyRegulation;           // Energy regulation
    float recoveryRate;              // Recovery rate
    
    // Social bonding and attachment
    float socialBonding;              // Social bonding enhancement
    float attachmentFormation;        // Attachment formation
    float maternalBehavior;           // Maternal behavior modulation
    
    // Developmental trajectory
    float developmentalOffset;      // Developmental modulation
    float maturationStage;           // Current developmental stage
    float experienceModulation;     // Learning/modulation based on experience
    
    // Memory and prediction integration
    std::vector<float> history;     // Historical activity levels
    std::vector<float> predictionErrors; // Prediction error history
    float memoryTrace;              // Short-term memory trace
    float eligibilityTrace;         // Eligibility for plasticity
    
    // Interaction with other systems
    float interactionMatrix[6][6];  // Interaction with other neuromodulators
    std::vector<float> plasticityFactors; // Plasticity effects on synapses
    std::vector<float> neuralExcitability; // Effects on neural populations
    
    // Random generator for probabilistic responses
    std::mt19937 rng;
    std::uniform_real_distribution<float> responseDist;
    
    EndorphinsImpl() :
        level(0.0f), baseline(0.01f), peak(1.5f),
        decayRate(0.08f), releaseRate(0.4f), synthesisRate(0.02f),
        reuptakeRate(0.1f), receptorDensity(1.1f), bindingAffinity(0.85f),
        painModulation(1.0f), pleasureModulation(1.0f), rewardProcessing(1.0f),
        hedonicResponse(0.5f), stressReduction(1.0f), 
        homeostaticBalance(1.0f), energyRegulation(1.0f), recoveryRate(1.0f),
        socialBonding(1.0f), attachmentFormation(1.0f), maternalBehavior(1.0f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for Endorphins
        // Endorphins are natural painkillers and mood enhancers
        interactionMatrix[0][0] = 1.0f;  // Endorphins-Endorphins self-interaction
        interactionMatrix[0][1] = 0.2f;  // Endorphins-5HT: excitation
        interactionMatrix[0][2] = 0.3f;  // Endorphins-NE: excitation
        interactionMatrix[0][3] = 0.3f;  // Endorphins-ACh: excitation
        interactionMatrix[0][4] = 0.2f;  // Endorphins-GABA: excitation
        interactionMatrix[0][5] = 0.1f;  // Endorphins-Glu: excitation
        
        // Other neuromodulators' effects on Endorphins
        interactionMatrix[1][0] = 0.3f;  // 5HT-Endorphins: excitation
        interactionMatrix[2][0] = 0.4f;  // NE-Endorphins: excitation
        interactionMatrix[3][0] = 0.3f;  // ACh-Endorphins: excitation
        interactionMatrix[4][0] = 0.2f;  // GABA-Endorphins: excitation
        interactionMatrix[5][0] = 0.2f;  // Glu-Endorphins: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Endorphins::Endorphins() : pImpl(new Impl) {
    NLM_LOG_INFO("Endorphins neuromodulator initialized (Phase 6 Enhanced)");
}

Endorphins::~Endorphins() = default;

const char* Endorphins::getName() const {
    return "Endorphin";
}

float Endorphins::getLevel() const {
    return pImpl->level;
}

void Endorphins::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Endorphins::getPlasticityFactor() const {
    // Endorphins-modulated plasticity factor
    // Endorphins enhance plasticity through pain reduction and reward processing
    float plasticity = 0.6f + pImpl->level * 1.2f;  // Higher Endorphins = more plasticity
    
    // Apply pleasure and reward modulations
    if (pImpl->pleasureModulation > 0.7f) {  // High pleasure
        plasticity *= 1.5f;  // Enhanced plasticity with high pleasure
    }
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.2f, 2.5f);
}

void Endorphins::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Core dynamics
    float decayAmount = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Activity-dependent potentiation
    if (pImpl->level > pImpl->baseline) {
        float facilitation = pImpl->level * 0.008f * deltaTime;
        pImpl->baseline += facilitation;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 1.0f);
    }
    
    // Reuptake and synthesis balance
    float synthesis = pImpl->synthesisRate * deltaTime;
    float reuptake = pImpl->reuptakeRate * pImpl->level * deltaTime;
    pImpl->level += synthesis - reuptake;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update state tracking
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Decay memory traces
    pImpl->memoryTrace *= 0.95f;
    pImpl->eligibilityTrace *= 0.9f;
    
    NLM_LOG_DEBUG("Endorphins updated: level=" + std::to_string(pImpl->level) + 
                 ", pleasure_modulation=" + std::to_string(pImpl->pleasureModulation) +
                 ", pain_modulation=" + std::to_string(pImpl->painModulation));
}

void Endorphins::modulatePainPerception(float painLevel, float intensity) {
    // Pain modulation based on Endorphins activity
    // Endorphins are natural painkillers
    float painReduction = painLevel * pImpl->level * (1.0f + intensity * 0.5f);
    pImpl->painModulation -= painReduction;
    pImpl->painModulation = std::clamp(pImpl->painModulation, 0.0f, 2.0f);
    
    // High pain triggers endorphin release
    if (painLevel > 0.7f) {
        pImpl->level += 0.2f * intensity;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    }
    
    NLM_LOG_DEBUG("Endorphins pain modulation: pain_level=" + std::to_string(painLevel) + 
                 ", intensity=" + std::to_string(intensity) +
                 ", reduction=" + std::to_string(painReduction) +
                 ", pain_modulation=" + std::to_string(pImpl->painModulation));
}

void Endorphins::enhancePleasureProcessing(float rewardValue) {
    // Pleasure enhancement based on Endorphins activity
    float pleasureGain = rewardValue * pImpl->level * 0.5f;
    pImpl->pleasureModulation += pleasureGain;
    pImpl->pleasureModulation = std::clamp(pImpl->pleasureModulation, 0.0f, 3.0f);
    
    // Reward processing enhancement
    pImpl->rewardProcessing += pleasureGain * 0.3f;
    pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    
    // Hedonic response enhancement
    pImpl->hedonicResponse += pleasureGain * 0.5f;
    pImpl->hedonicResponse = std::clamp(pImpl->hedonicResponse, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Endorphins pleasure enhancement: reward_value=" + std::to_string(rewardValue) + 
                 ", gain=" + std::to_string(pleasureGain) +
                 ", pleasure_modulation=" + std::to_string(pImpl->pleasureModulation) +
                 ", hedonic_response=" + std::to_string(pImpl->hedonicResponse));
}

void Endorphins::reduceStressAndAnxiety(float stressLevel) {
    // Stress and anxiety reduction based on Endorphins activity
    float stressReduction = stressLevel * pImpl->level * 0.4f;
    pImpl->stressReduction += stressReduction;
    pImpl->stressReduction = std::clamp(pImpl->stressReduction, 0.0f, 2.0f);
    
    // Stress reduction affects pain and pleasure
    pImpl->painModulation *= (1.0f - stressReduction * 0.3f);
    pImpl->pleasureModulation *= (1.0f + stressReduction * 0.2f);
    
    pImpl->painModulation = std::clamp(pImpl->painModulation, 0.0f, 2.0f);
    pImpl->pleasureModulation = std::clamp(pImpl->pleasureModulation, 0.0f, 3.0f);
    
    NLM_LOG_DEBUG("Endorphins stress reduction: stress_level=" + std::to_string(stressLevel) + 
                 ", reduction=" + std::to_string(stressReduction) +
                 ", stress_reduction=" + std::to_string(pImpl->stressReduction));
}

void Endorphins::activateRewardCircuits(float rewardPredictionError) {
    // Reward circuit activation based on Endorphins activity
    float rewardGain = rewardPredictionError * pImpl->level * 0.6f;
    pImpl->rewardProcessing += rewardGain;
    pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    
    // Enhance hedonic response with reward prediction error
    if (rewardPredictionError > 0.0f) {
        pImpl->hedonicResponse += rewardPredictionError * 0.3f;
        pImpl->hedonicResponse = std::clamp(pImpl->hedonicResponse, 0.5f, 2.0f);
    }
    
    NLM_LOG_DEBUG("Endorphins reward circuit activation: reward_prediction_error=" + std::to_string(rewardPredictionError) + 
                 ", gain=" + std::to_string(rewardGain) +
                 ", reward_processing=" + std::to_string(pImpl->rewardProcessing) +
                 ", hedonic_response=" + std::to_string(pImpl->hedonicResponse));
}

void Endorphins::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // Endorphins modulate response to prediction errors
    if (predictionError != 0.0f) {
        // Positive prediction errors increase pleasure and reduce pain
        if (predictionError > 0.0f) {
            pImpl->pleasureModulation += predictionError * 0.1f;
            pImpl->painModulation -= predictionError * 0.05f;
        }
        
        // Negative prediction errors increase pain and decrease pleasure
        if (predictionError < 0.0f) {
            pImpl->painModulation += std::abs(predictionError) * 0.1f;
            pImpl->pleasureModulation -= std::abs(predictionError) * 0.05f;
        }
        
        pImpl->pleasureModulation = std::clamp(pImpl->pleasureModulation, 0.0f, 3.0f);
        pImpl->painModulation = std::clamp(pImpl->painModulation, 0.0f, 2.0f);
    }
}

void Endorphins::enhanceHedonicResponse(float hedonicValence) {
    // Hedonic response enhancement
    pImpl->hedonicResponse += hedonicValence * pImpl->level * 0.3f;
    pImpl->hedonicResponse = std::clamp(pImpl->hedonicResponse, 0.5f, 2.0f);
    
    // Enhanced hedonic response affects reward processing
    pImpl->rewardProcessing += pImpl->hedonicResponse * 0.2f;
    pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    
    NLM_LOG_DEBUG("Endorphins hedonic response enhancement: hedonic_valence=" + std::to_string(hedonicValence) + 
                 ", enhancement=" + std::to_string(hedonicValence * pImpl->level * 0.3f) +
                 ", response=" + std::to_string(pImpl->hedonicResponse) +
                 ", processing=" + std::to_string(pImpl->rewardProcessing));
}

void Endorphins::updateEndorphinDynamics(float dt) {
    // Endorphin dynamics including stress-induced release
    
    // Stress-induced release
    if (pImpl->stressReduction > 0.5f) {
        float stressInducedRelease = pImpl->stressReduction * 0.1f * dt;
        pImpl->level += stressInducedRelease;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    }
    
    // Tonic firing - steady background activity
    float tonicChange = pImpl->synthesisRate * dt - pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += tonicChange;
    
    // Reward-induced release
    if (pImpl->rewardProcessing > 1.5f) {
        float rewardInducedRelease = (pImpl->rewardProcessing - 1.5f) * 0.05f * dt;
        pImpl->level += rewardInducedRelease;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    }
    
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Endorphins::encodeRewardMemory(float reward, float predictionError) {
    // Reward memory encoding based on Endorphins activity
    float memoryStrength = reward * pImpl->level * (1.0f + std::abs(predictionError) * 0.5f);
    pImpl->memoryTrace += memoryStrength;
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    // Reward prediction error affects memory encoding
    if (predictionError != 0.0f) {
        pImpl->rewardProcessing += std::abs(predictionError) * 0.2f;
        pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    }
    
    NLM_LOG_DEBUG("Endorphins reward memory encoding: reward=" + std::to_string(reward) + 
                 ", prediction_error=" + std::to_string(predictionError) +
                 ", strength=" + std::to_string(memoryStrength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace) +
                 ", processing=" + std::to_string(pImpl->rewardProcessing));
}

void Endorphins::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery of other parameters
    pImpl->painModulation *= (1.0f - recoveryRate * dt);
    pImpl->pleasureModulation += (1.0f - pImpl->pleasureModulation) * recoveryRate * dt;
    pImpl->stressReduction *= (1.0f - recoveryRate * dt);
    
    pImpl->painModulation = std::clamp(pImpl->painModulation, 0.0f, 2.0f);
    pImpl->pleasureModulation = std::clamp(pImpl->pleasureModulation, 0.0f, 3.0f);
    pImpl->stressReduction = std::clamp(pImpl->stressReduction, 0.0f, 2.0f);
}

void Endorphins::setSocialBondingStrength(float bondingStrength) {
    pImpl->socialBonding = std::clamp(bondingStrength, 0.0f, 2.0f);
}

void Endorphins::updateMaternalBehavior(float maternalLevel) {
    // Maternal behavior enhancement based on Endorphins activity
    pImpl->maternalBehavior += maternalLevel * pImpl->level * 0.1f;
    pImpl->maternalBehavior = std::clamp(pImpl->maternalBehavior, 0.0f, 2.0f);
}

void Endorphins::enhanceRewardLearning(float rewardPrediction) {
    // Reward learning enhancement
    pImpl->rewardProcessing += rewardPrediction * pImpl->level * 0.2f;
    pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    
    // Positive reward predictions enhance pleasure
    if (rewardPrediction > 0.0f) {
        pImpl->pleasureModulation += rewardPrediction * 0.1f;
        pImpl->pleasureModulation = std::clamp(pImpl->pleasureModulation, 0.0f, 3.0f);
    }
}

void Endorphins::modulatePainThreshold(float thresholdLevel) {
    // Pain threshold modulation
    pImpl->painModulation *= (1.0f + thresholdLevel * 0.3f);
    pImpl->painModulation = std::clamp(pImpl->painModulation, 0.0f, 2.0f);
    
    NLM_LOG_DEBUG("Endorphins pain threshold: threshold_level=" + std::to_string(thresholdLevel) + 
                 ", modulation=" + std::to_string(pImpl->painModulation));
}

void Endorphins::updateHomeostaticBalance(float balanceFactor) {
    // Homeostatic balance enhancement
    pImpl->homeostaticBalance += balanceFactor * pImpl->level * 0.1f;
    pImpl->homeostaticBalance = std::clamp(pImpl->homeostaticBalance, 1.0f, 3.0f);
    
    // Better homeostatic balance affects stress and recovery
    pImpl->stressReduction += balanceFactor * 0.05f;
    pImpl->recoveryRate += balanceFactor * 0.05f;
}

void Endorphins::enhanceStressResilience(float resilienceFactor) {
    // Stress resilience enhancement
    pImpl->stressReduction += resilienceFactor * 0.1f;
    pImpl->stressReduction = std::clamp(pImpl->stressReduction, 0.0f, 2.0f);
    
    // Higher resilience reduces pain and increases pleasure
    pImpl->painModulation *= (1.0f - resilienceFactor * 0.2f);
    pImpl->pleasureModulation *= (1.0f + resilienceFactor * 0.1f);
}

void Endorphins::modulateSexualBehavior(float sexualDrive) {
    // Sexual behavior modulation
    pImpl->socialBonding += sexualDrive * pImpl->level * 0.1f;
    pImpl->socialBonding = std::clamp(pImpl->socialBonding, 0.0f, 2.0f);
    
    // Enhanced social bonding affects maternal behavior
    if (sexualDrive > 0.5f) {
        pImpl->maternalBehavior *= (1.0f + sexualDrive * 0.1f);
        pImpl->maternalBehavior = std::clamp(pImpl->maternalBehavior, 0.0f, 2.0f);
    }
}

void Endorphins::updateAddictivePotential(float addictionPotential) {
    // Addictive potential modulation
    pImpl->rewardProcessing *= (1.0f + addictionPotential * 0.2f);
    pImpl->rewardProcessing = std::clamp(pImpl->rewardProcessing, 1.0f, 3.0f);
    
    // Higher addiction potential affects pleasure and stress
    pImpl->pleasureModulation *= (1.0f + addictionPotential * 0.3f);
    pImpl->stressReduction *= (1.0f - addictionPotential * 0.1f);
}

float Endorphins::getPainModulation() const {
    return pImpl->painModulation;
}

float Endorphins::getPleasureModulation() const {
    return pImpl->pleasureModulation;
}

float Endorphins::getStressModulation() const {
    return pImpl->stressReduction;
}

float Endorphins::getRewardModulation() const {
    return pImpl->rewardProcessing;
}

} // namespace nlm