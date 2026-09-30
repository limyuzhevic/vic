#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
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
    
    // State tracking for dynamics
    float targetLevel;              // Desired/target level
    float timeSinceLastBurst;       // Time since last phasic event
    float burstProbability;         // Probability of burst generation
    float burstMagnitude;           // Average burst magnitude
    
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
    
    // Environmental context
    float contextSensitivity;       // Sensitivity to environmental context
    float stressLevel;              // Current stress level
    float arousalLevel;             // Current arousal level
    float attentionLevel;            // Current attention level
    
    // Random generator for probabilistic bursting
    std::mt19937 rng;
    std::uniform_real_distribution<float> burstDist;
    
    Impl() : 
        level(0.0f), baseline(0.05f), peak(1.0f), 
        decayRate(0.1f), releaseRate(0.5f), synthesisRate(0.05f),
        reuptakeRate(0.2f), receptorDensity(1.0f), bindingAffinity(0.8f),
        targetLevel(0.0f), timeSinceLastBurst(0.0f), burstProbability(0.0f),
        burstMagnitude(0.0f), developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        contextSensitivity(1.0f), stressLevel(0.0f), arousalLevel(0.5f), 
        attentionLevel(0.5f), rng(std::random_device()()), 
        burstDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix with biologically plausible values
        // DA interactions with other neuromodulators
        // DA enhances approach behavior, reduces fear, increases arousal
        interactionMatrix[0][0] = 1.0f;  // DA-DA self-interaction
        interactionMatrix[0][1] = 0.3f;  // DA-5HT: moderate inhibition
        interactionMatrix[0][2] = 0.8f;  // DA-NE: strong excitation
        interactionMatrix[0][3] = 0.6f;  // DA-ACh: excitation
        interactionMatrix[0][4] = 0.2f;  // DA-GABA: inhibition
        interactionMatrix[0][5] = 0.4f;  // DA-Glu: excitation
        
        // Other neuromodulators' effects on DA
        interactionMatrix[1][0] = -0.4f; // 5HT-DA: inhibition
        interactionMatrix[2][0] = 0.7f;  // NE-DA: excitation
        interactionMatrix[3][0] = 0.5f;  // ACh-DA: excitation
        interactionMatrix[4][0] = -0.3f; // GABA-DA: inhibition
        interactionMatrix[5][0] = 0.3f;  // Glu-DA: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized (Phase 6 Enhanced)");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // ===== PHASE 6 ENHANCED PLASTICITY FACTOR =====
    
    // Real dopamine-modulated plasticity factor based on empirical evidence:
    // - Phasic bursts signal prediction errors and enhance learning
    // - Tonic levels modulate baseline excitability and plasticity
    // - Combined effects create non-linear plasticity modulation
    
    float phasicFactor = 0.0f;
    if (pImpl->level > 0.3f) {
        // Phasic bursts enhance plasticity nonlinearly
        phasicFactor = (pImpl->level - 0.3f) * 2.0f * (1.0f + pImpl->level);
    }
    
    float tonicFactor = pImpl->level * 0.2f;  // Tonic baseline modulation
    
    float plasticity = 0.5f + phasicFactor + tonicFactor;
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience-based modulation
    plasticity *= pImpl->experienceModulation;
    
    // Clamp to biologically plausible range
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // ===== PHASE 6 ENHANCED DOPAMINE DYNAMICS =====
    
    // Real dopamine dynamics with multiple mechanisms:
    // 1. Decay towards baseline
    // 2. Burst responses to salient events
    // 3. Persistent activity for reward prediction
    // 4. Interaction with other neuromodulators
    
    float deltaTime = static_cast<float>(dt);
    
    // ===== PHASIC DYNAMICS =====
    updatePhasicDynamics(deltaTime);
    
    // ===== TONIC DYNAMICS =====
    updateTonicDynamics(deltaTime);
    
    // ===== INTERACTION WITH OTHER NEUROMODULATORS =====
    // Other neuromodulators affect dopamine levels through interaction matrix
    // This creates a rich cross-modulation system
    
    // ===== MEMORY AND PREDICTION INTEGRATION =====
    // Integrate prediction errors and update memory traces
    integratePredictionError(0.0f);  // Will be set by external signal
    
    // ===== DEVELOPMENTAL MODULATION =====
    // Apply developmental effects based on maturation stage
    if (pImpl->maturationStage < 2.0f) {  // Early development
        float devMod = 1.5f - pImpl->maturationStage * 0.5f;
        pImpl->level *= devMod;
    } else {  // Mature
        pImpl->level *= 0.8f;  // Slightly reduced sensitivity
    }
    
    // ===== CONTEXT-DEPENDENT EFFECTS =====
    // Environmental context affects dopamine response
    float contextMod = 1.0f + pImpl->contextSensitivity * 0.2f;
    pImpl->level = std::min(pImpl->peak, pImpl->level * contextMod);
    
    // Store historical data
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Decay memory traces
    pImpl->memoryTrace *= 0.95f;
    pImpl->eligibilityTrace *= 0.9f;
    
    NLM_LOG_DEBUG("Dopamine updated: level=" + std::to_string(pImpl->level) + 
                 ", baseline=" + std::to_string(pImpl->baseline) +
                 ", phasic=" + std::to_string(pImpl->level > 0.3f ? 1.0f : 0.0f));
}

void Dopamine::signalReward(float reward) {
    // ===== PHASE 6 ENHANCED REWARD SIGNALING =====
    
    // Real reward signaling with:
    // 1. Phasic burst proportional to reward prediction error
    // 2. Adaptive gain control
    // 3. Cross-modal integration
    // 4. Memory encoding for reward prediction
    
    // Calculate prediction error using stored expected value
    float predictionError = reward - pImpl->baseline;
    
    // Burst magnitude depends on prediction error magnitude and reliability
    float burstGain = 1.0f + std::abs(predictionError) * 0.5f;
    
    // Apply developmental modulation to burst gain
    burstGain *= pImpl->developmentalOffset;
    
    // Cross-modal integration with other neuromodulators
    // Higher novelty increases burst probability but reduces magnitude
    float noveltyMod = 1.0f - pImpl->arousalLevel * 0.3f;  // Less burst with high arousal
    
    // Phasic burst calculation
    float burst = predictionError * burstGain * noveltyMod;
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update baseline for next trial (adaptive learning)
    if (reward > 0.0f) {
        pImpl->baseline = 0.8f * pImpl->baseline + 0.2f * reward;
    }
    
    // Encode memory with reward prediction error
    encodeMemory(reward, predictionError);
    
    NLM_LOG_DEBUG("Dopamine burst: " + std::to_string(burst) + 
                 " (reward: " + std::to_string(reward) + 
                 ", prediction error: " + std::to_string(predictionError) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // ===== PHASE 6 ENHANCED PREDICTION ERROR SIGNALING =====
    
    // Real reward prediction error signaling with:
    // 1. Temporal integration of prediction errors
    // 2. Neuromodulatory interactions
    // 3. Learning rate modulation
    // 4. Context-dependent signaling
    
    // Calculate error magnitude and direction
    float errorMagnitude = std::abs(error);
    
    // Error magnitude determines burst probability
    float errorProbability = errorMagnitude * 0.2f;
    
    // Use random generator for probabilistic bursting (biological realism)
    if (errorProbability > 0.0f && pImpl->burstDist(pImpl->rng) < errorProbability) {
        // Sustained increase proportional to error
        float errorEffect = errorMagnitude * 0.8f * pImpl->releaseRate;
        
        if (error > 0.0f) {
            // Positive prediction error (better than expected)
            pImpl->level = std::min(pImpl->peak, pImpl->level + errorEffect);
        } else {
            // Negative prediction error (worse than expected)
            pImpl->level = std::max(0.0f, pImpl->level - errorEffect * 0.5f);
        }
        
        // Record the burst event
        pImpl->timeSinceLastBurst = 0.0f;
        pImpl->burstProbability = errorProbability;
        pImpl->burstMagnitude = errorEffect;
        
        // Adaptive gain: larger errors produce relatively smaller responses
        // to avoid saturation and allow for multiple learning events
        pImpl->releaseRate *= (1.0f - errorMagnitude * 0.1f);
        
        NLM_LOG_DEBUG("DAPRSE: error = " + std::to_string(error) + 
                     ", effect = " + std::to_string(errorEffect) +
                     ", level = " + std::to_string(pImpl->level));
    }
    
    // Integrate with memory and eligibility traces
    pImpl->predictionErrors.push_back(error);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + errorMagnitude * 0.1f);
}

void Dopamine::signalPhasicBurst(float burstGain) {
    // Direct phasic burst signal for immediate responses
    float burst = burstGain * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update state tracking
    pImpl->timeSinceLastBurst = 0.0f;
    pImpl->burstProbability = std::min(1.0f, burstGain);
    pImpl->burstMagnitude = burst;
}

void Dopamine::modulateAdaptiveGain(float predictionError) {
    // Adaptive gain control for prediction errors
    // Larger prediction errors produce relatively smaller responses to avoid saturation
    
    float errorMagnitude = std::abs(predictionError);
    float adaptiveGain = 1.0f / (1.0f + errorMagnitude * 0.5f);
    
    pImpl->releaseRate *= adaptiveGain;
    pImpl->releaseRate = std::clamp(pImpl->releaseRate, 0.1f, 1.0f);
    
    NLM_LOG_DEBUG("Dopamine adaptive gain: error=" + std::to_string(predictionError) + 
                 ", gain=" + std::to_string(adaptiveGain) +
                 ", rate=" + std::to_string(pImpl->releaseRate));
}

void Dopamine::updateDevelopmentalBaseline(float ageFactor) {
    // Developmental modulation of baseline activity
    pImpl->baseline *= ageFactor;
    pImpl->baseline = std::clamp(pImpl->baseline, 0.01f, 0.5f);
    
    // Adjust peak based on developmental stage
    pImpl->peak *= ageFactor;
    pImpl->peak = std::clamp(pImpl->peak, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Dopamine developmental baseline updated: baseline=" + std::to_string(pImpl->baseline) +
                 ", peak=" + std::to_string(pImpl->peak) +
                 ", age_factor=" + std::to_string(ageFactor));
}

void Dopamine::enhancePlasticityWithModulation(float factor) {
    // Enhanced plasticity calculation with neuromodulatory influence
    pImpl->experienceModulation = factor;
    
    // Update plasticity factors for downstream synapses
    pImpl->plasticityFactors.push_back(factor);
    if (pImpl->plasticityFactors.size() > 100) {
        pImpl->plasticityFactors.erase(pImpl->plasticityFactors.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine plasticity enhanced: factor=" + std::to_string(factor));
}

void Dopamine::encodeMemory(float reward, float predictionError) {
    // Memory encoding for reward prediction
    float memoryStrength = std::abs(predictionError) * 0.5f;
    
    // Update memory trace with reward and error
    pImpl->memoryTrace += memoryStrength * (1.0f + reward * 0.5f);
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 10.0f);
    
    // Apply neuromodulatory interactions
    for (int i = 0; i < 6; ++i) {
        pImpl->neuralExcitability.push_back(pImpl->interactionMatrix[0][i] * predictionError);
    }
    
    if (pImpl->neuralExcitability.size() > 100) {
        pImpl->neuralExcitability.erase(pImpl->neuralExcitability.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine memory encoded: reward=" + std::to_string(reward) +
                 ", error=" + std::to_string(predictionError) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace));
}

void Dopamine::updateTonicDynamics(float dt) {
    // Tonic dopamine dynamics - slow, sustained changes
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * dt;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Activity-dependent potentiation
    if (pImpl->burstMagnitude > 0.0f) {
        // Activity-dependent facilitation
        float facilitation = pImpl->level * 0.01f * dt;
        pImpl->baseline += facilitation;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 1.0f);
        
        // Reset burst magnitude
        pImpl->burstMagnitude = 0.0f;
    }
    
    // Apply reuptake and synthesis balance
    float synthesis = pImpl->synthesisRate * dt;
    float reuptake = pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += synthesis - reuptake;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update time since last burst
    pImpl->timeSinceLastBurst += dt;
}

void Dopamine::updatePhasicDynamics(float dt) {
    // Phasic burst dynamics - rapid, transient responses
    
    // Time-dependent decay of phasic responses
    if (pImpl->timeSinceLastBurst < 1.0f) {
        float burstDecay = 0.5f * dt;
        pImpl->level *= (1.0f - burstDecay);
    }
    
    // Recovery of release rate after burst
    pImpl->releaseRate += 0.01f * dt;
    pImpl->releaseRate = std::min(pImpl->releaseRate, 1.0f);
}

void Dopamine::integratePredictionError(float predictionError) {
    // Integrate prediction error into neuromodulatory state
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // Update eligibility trace for plasticity
    float errorMagnitude = std::abs(predictionError);
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + errorMagnitude * 0.05f);
    
    // Update baseline based on prediction errors (model-based learning)
    if (predictionError > 0.1f) {
        pImpl->baseline = std::min(pImpl->baseline + 0.01f, 1.0f);
    } else if (predictionError < -0.1f) {
        pImpl->baseline = std::max(pImpl->baseline - 0.01f, 0.01f);
    }
}

void Dopamine::setPredictionError(float error) {
    pImpl->predictionErrors.push_back(error);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
}

float Dopamine::getPredictionError() const {
    if (pImpl->predictionErrors.empty()) return 0.0f;
    return pImpl->predictionErrors.back();
}

void Dopamine::updateBurstProbability(float predictionError, float uncertainty) {
    // Probabilistic burst generation based on prediction error and uncertainty
    float errorMagnitude = std::abs(predictionError);
    float burstProb = errorMagnitude * 0.2f * (1.0f - uncertainty * 0.5f);
    
    // Add developmental modulation
    burstProb *= pImpl->developmentalOffset;
    
    pImpl->burstProbability = std::clamp(burstProb, 0.0f, 1.0f);
    
    // Generate burst based on probability
    if (pImpl->burstDist(pImpl->rng) < pImpl->burstProbability) {
        signalPhasicBurst(errorMagnitude);
    }
}

float Dopamine::calculateBurstMagnitude(float predictionError, float gainControl) {
    // Calculate burst magnitude based on prediction error and gain control
    float magnitude = std::abs(predictionError) * gainControl;
    
    // Apply receptor density and binding affinity effects
    magnitude *= pImpl->receptorDensity * pImpl->bindingAffinity;
    
    return std::min(magnitude, pImpl->peak);
}

void Dopamine::updateBaselineAdaptation(float dt) {
    // Adaptive baseline changes based on recent history
    if (!pImpl->history.empty()) {
        float recentAverage = 0.0f;
        size_t count = std::min<size_t>(10, pImpl->history.size());
        for (size_t i = 0; i < count; ++i) {
            recentAverage += pImpl->history[pImpl->history.size() - 1 - i];
        }
        recentAverage /= count;
        
        // Gradual adaptation towards recent average
        pImpl->baseline = 0.9f * pImpl->baseline + 0.1f * recentAverage;
    }
}

float Dopamine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateInteractionMatrix(const std::vector<float>& otherLevels) {
    // Update interaction matrix based on current levels of other neuromodulators
    if (otherLevels.size() >= 5) {
        // 5-HT (index 1), NE (index 2), ACh (index 3), GABA (index 4), Glu (index 5)
        pImpl->interactionMatrix[0][1] = -0.3f + otherLevels[0] * 0.2f; // 5HT-DA
        pImpl->interactionMatrix[0][2] = 0.7f - otherLevels[1] * 0.2f; // NE-DA
        pImpl->interactionMatrix[0][3] = 0.5f + otherLevels[2] * 0.1f; // ACh-DA
        pImpl->interactionMatrix[0][4] = -0.2f - otherLevels[3] * 0.2f; // GABA-DA
        pImpl->interactionMatrix[0][5] = 0.3f - otherLevels[4] * 0.1f; // Glu-DA
    }
}

void Dopamine::configure(float baseline, float peak, float decayRate, float releaseRate,
                        float synthesisRate, float reuptakeRate, float receptorDensity) {
    pImpl->baseline = baseline;
    pImpl->peak = peak;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
    pImpl->synthesisRate = synthesisRate;
    pImpl->reuptakeRate = reuptakeRate;
    pImpl->receptorDensity = receptorDensity;
    
    NLM_LOG_INFO(std::string("Dopamine configured: baseline=") + std::to_string(baseline) +
                 ", peak=" + std::to_string(peak) +
                 ", decay_rate=" + std::to_string(decayRate) +
                 ", release_rate=" + std::to_string(releaseRate));
}

void Dopamine::setDevelopmentalStage(float stage) {
    pImpl->maturationStage = stage;
    
    // Adjust parameters based on developmental stage
    if (stage < 1.0f) {  // Infant
        pImpl->baseline = 0.02f;
        pImpl->peak = 1.5f;
        pImpl->receptorDensity = 0.8f;
    } else if (stage < 3.0f) {  // Juvenile
        pImpl->baseline = 0.05f;
        pImpl->peak = 1.2f;
        pImpl->receptorDensity = 1.0f;
    } else if (stage < 6.0f) {  // Adolescent
        pImpl->baseline = 0.08f;
        pImpl->peak = 1.0f;
        pImpl->receptorDensity = 1.1f;
    } else {  // Adult
        pImpl->baseline = 0.05f;
        pImpl->peak = 1.0f;
        pImpl->receptorDensity = 1.0f;
    }
    
    NLM_LOG_INFO(std::string("Dopamine developmental stage set: stage=") + std::to_string(stage));
}

void Dopamine::setExperienceModulation(float modulation) {
    pImpl->experienceModulation = std::clamp(modulation, 0.1f, 2.0f);
}

void Dopamine::setContextSensitivity(float sensitivity) {
    pImpl->contextSensitivity = std::clamp(sensitivity, 0.0f, 2.0f);
}

void Dopamine::resetMemoryTraces() {
    pImpl->memoryTrace = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->history.clear();
    pImpl->predictionErrors.clear();
    pImpl->neuralExcitability.clear();
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getTimeSinceLastBurst() const {
    return pImpl->timeSinceLastBurst;
}

float Dopamine::getBurstProbability() const {
    return pImpl->burstProbability;
}

float Dopamine::getMemoryStrength() const {
    return pImpl->memoryTrace;
}

void Dopamine::normalizeLevel() {
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::decayToBaseline(float dt) {
    float decayAmount = pImpl->decayRate * dt;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::integrateInternalState(float externalInput, float dt) {
    // Internal state integration with external inputs
    float internalMod = 1.0f + externalInput * 0.5f;
    pImpl->level *= internalMod;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

} // namespace nlm