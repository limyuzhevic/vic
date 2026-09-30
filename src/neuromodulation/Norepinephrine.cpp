#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct NorepinephrineImpl {
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
    
    // Attention and arousal parameters
    float attentionModulation;       // Attention enhancement
    float arousalModulation;         // Arousal level
    float vigilanceModulation;       // Vigilance/arousal level
    float locusCoeruleusActivity;    // LC neuron firing rate
    
    // Stress and threat response
    float stressResponse;            // Fight-or-flight response
    float threatSensitivity;         // Sensitivity to threats
    float emergencyActivation;       // Emergency activation level
    
    // Cognitive processing enhancement
    float workingMemoryModulation;   // Working memory enhancement
    float cognitiveFlexibility;      // Cognitive flexibility
    float executiveFunction;         // Executive function modulation
    
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
    
    NorepinephrineImpl() :
        level(0.0f), baseline(0.01f), peak(2.0f),
        decayRate(0.2f), releaseRate(0.4f), synthesisRate(0.03f),
        reuptakeRate(0.25f), receptorDensity(1.5f), bindingAffinity(0.95f),
        attentionModulation(1.0f), arousalModulation(0.5f), 
        vigilanceModulation(0.5f), locusCoeruleusActivity(0.1f),
        stressResponse(0.0f), threatSensitivity(0.5f), emergencyActivation(0.0f),
        workingMemoryModulation(1.0f), cognitiveFlexibility(1.0f),
        executiveFunction(1.0f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for NE
        // NE increases arousal, vigilance, and attention
        interactionMatrix[0][0] = 1.0f;  // NE-NE self-interaction
        interactionMatrix[0][1] = 0.3f;  // NE-5HT: excitation (modulated)
        interactionMatrix[0][2] = 1.0f;  // NE-NE: strong self
        interactionMatrix[0][3] = 0.2f;  // NE-ACh: excitation
        interactionMatrix[0][4] = -0.5f; // NE-GABA: strong inhibition
        interactionMatrix[0][5] = 0.6f;  // NE-Glu: excitation
        
        // Other neuromodulators' effects on NE
        interactionMatrix[1][2] = 0.5f;  // 5HT-NE: excitation
        interactionMatrix[2][0] = 1.2f;  // NE-NE: stronger
        interactionMatrix[3][2] = 0.3f;  // ACh-NE: excitation
        interactionMatrix[4][2] = -0.4f; // GABA-NE: inhibition
        interactionMatrix[5][2] = 0.5f;  // Glu-NE: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized (Phase 6 Enhanced)");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine-modulated plasticity factor
    // NE generally enhances attention and arousal, affecting learning
    float plasticity = 0.7f + pImpl->level * 1.5f;  // Higher NE = more plasticity
    
    // Apply attention modulation
    if (pImpl->attentionModulation > 0.8f) {  // High attention
        plasticity *= 1.2f;  // Enhanced plasticity with high attention
    }
    
    // Apply arousal modulation
    if (pImpl->arousalModulation > 0.7f) {  // High arousal
        plasticity *= 1.1f;  // Enhanced plasticity with high arousal
    }
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.3f, 2.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Core dynamics
    float decayAmount = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Activity-dependent potentiation
    if (pImpl->level > pImpl->baseline) {
        float facilitation = pImpl->level * 0.01f * deltaTime;
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
    
    NLM_LOG_DEBUG("Norepinephrine updated: level=" + std::to_string(pImpl->level) + 
                 ", attention=" + std::to_string(pImpl->attentionModulation) +
                 ", arousal=" + std::to_string(pImpl->arousalModulation));
}

void Norepinephrine::enhanceAttention(float stimulusSalience) {
    // Attention enhancement based on NE activity
    float attentionGain = stimulusSalience * pImpl->level * 0.6f;
    pImpl->attentionModulation += attentionGain;
    pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
    
    // Enhanced attention improves working memory
    pImpl->workingMemoryModulation += attentionGain * 0.3f;
    pImpl->workingMemoryModulation = std::clamp(pImpl->workingMemoryModulation, 0.5f, 2.0f);
    
    // High attention increases arousal
    pImpl->arousalModulation += attentionGain * 0.2f;
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Norepinephrine attention enhancement: stimulus_salience=" + std::to_string(stimulusSalience) + 
                 ", gain=" + std::to_string(attentionGain) +
                 ", attention=" + std::to_string(pImpl->attentionModulation));
}

void Norepinephrine::increaseArousal(float level, float rate) {
    // Arousal enhancement based on NE activity
    float arousalGain = level * pImpl->level * rate;
    pImpl->arousalModulation += arousalGain;
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    
    // Higher arousal improves vigilance
    pImpl->vigilanceModulation += arousalGain * 0.3f;
    pImpl->vigilanceModulation = std::clamp(pImpl->vigilanceModulation, 0.5f, 2.0f);
    
    // Enhanced arousal improves cognitive flexibility
    pImpl->cognitiveFlexibility += arousalGain * 0.2f;
    pImpl->cognitiveFlexibility = std::clamp(pImpl->cognitiveFlexibility, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Norepinephrine arousal enhancement: level=" + std::to_string(level) + 
                 ", rate=" + std::to_string(rate) +
                 ", gain=" + std::to_string(arousalGain) +
                 ", arousal=" + std::to_string(pImpl->arousalModulation));
}

void Norepinephrine::activateFightOrFlightResponse(float threatLevel, float urgency) {
    // Fight-or-flight response activation
    float emergencyActivation = threatLevel * urgency * pImpl->level * 0.8f;
    pImpl->emergencyActivation += emergencyActivation;
    pImpl->emergencyActivation = std::clamp(pImpl->emergencyActivation, 0.0f, 2.0f);
    
    // Emergency response increases arousal and stress
    pImpl->arousalModulation += emergencyActivation * 0.5f;
    pImpl->stressResponse += emergencyActivation * 0.3f;
    
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    pImpl->stressResponse = std::clamp(pImpl->stressResponse, 0.0f, 2.0f);
    
    NLM_LOG_DEBUG("Norepinephrine fight-or-flight: threat_level=" + std::to_string(threatLevel) + 
                 ", urgency=" + std::to_string(urgency) +
                 ", activation=" + std::to_string(emergencyActivation) +
                 ", stress_response=" + std::to_string(pImpl->stressResponse));
}

void Norepinephrine::modulateStressResponse(float stressInput, float adaptation) {
    // Stress response modulation
    float stressGain = stressInput * adaptation * pImpl->level * 0.4f;
    pImpl->stressResponse += stressGain;
    pImpl->stressResponse = std::clamp(pImpl->stressResponse, 0.0f, 2.0f);
    
    // Stress response affects arousal and attention
    pImpl->arousalModulation += stressGain * 0.3f;
    pImpl->attentionModulation -= stressGain * 0.2f;  // Stress reduces attention
    
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
    
    NLM_LOG_DEBUG("Norepinephrine stress response: stress_input=" + std::to_string(stressInput) + 
                 ", adaptation=" + std::to_string(adaptation) +
                 ", gain=" + std::to_string(stressGain) +
                 ", stress_response=" + std::to_string(pImpl->stressResponse));
}

void Norepinephrine::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // NE helps focus attention on prediction errors
    if (predictionError != 0.0f) {
        // Positive prediction errors enhance attention
        if (predictionError > 0.0f) {
            pImpl->attentionModulation += predictionError * 0.1f;
            pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
        }
        
        // Negative prediction errors reduce attention
        if (predictionError < 0.0f) {
            pImpl->attentionModulation -= std::abs(predictionError) * 0.1f;
            pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
        }
        
        // Prediction errors enhance cognitive flexibility
        pImpl->cognitiveFlexibility += std::abs(predictionError) * 0.05f;
        pImpl->cognitiveFlexibility = std::clamp(pImpl->cognitiveFlexibility, 0.5f, 2.0f);
    }
}

void Norepinephrine::enhanceCognitiveProcessing(float taskDemand) {
    // Cognitive processing enhancement based on NE activity
    float cognitiveGain = taskDemand * pImpl->level * 0.5f;
    pImpl->executiveFunction += cognitiveGain;
    pImpl->executiveFunction = std::clamp(pImpl->executiveFunction, 0.5f, 2.0f);
    
    // Higher cognitive demand improves working memory
    if (taskDemand > 0.7f) {
        pImpl->workingMemoryModulation += taskDemand * 0.3f;
        pImpl->workingMemoryModulation = std::clamp(pImpl->workingMemoryModulation, 0.5f, 2.0f);
    }
    
    NLM_LOG_DEBUG("Norepinephrine cognitive processing: task_demand=" + std::to_string(taskDemand) + 
                 ", gain=" + std::to_string(cognitiveGain) +
                 ", executive_function=" + std::to_string(pImpl->executiveFunction));
}

void Norepinephrine::updateLocusCoeruleusDynamics(float dt) {
    // Locus coeruleus dynamics - central to NE signaling
    
    // Tonic LC firing - steady background activity
    float tonicChange = pImpl->synthesisRate * dt - pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += tonicChange;
    
    // Phasic LC firing - burst responses to novel or salient stimuli
    if (pImpl->level > pImpl->baseline) {
        float phasicBurst = pImpl->level * 0.03f * dt;
        pImpl->level += phasicBurst;
        
        // Phasic bursts strongly enhance attention and arousal
        pImpl->attentionModulation += phasicBurst * 0.5f;
        pImpl->arousalModulation += phasicBurst * 0.5f;
    }
    
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
}

void Norepinephrine::encodeAttentionMemory(float focus, float duration) {
    // Attention memory encoding
    float memoryStrength = focus * pImpl->level * 0.3f;
    pImpl->memoryTrace += memoryStrength;
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    // Longer attention duration enhances memory strength
    pImpl->workingMemoryModulation += duration * 0.01f;
    pImpl->workingMemoryModulation = std::clamp(pImpl->workingMemoryModulation, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Norepinephrine attention memory: focus=" + std::to_string(focus) + 
                 ", duration=" + std::to_string(duration) +
                 ", strength=" + std::to_string(memoryStrength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace) +
                 ", working_memory=" + std::to_string(pImpl->workingMemoryModulation));
}

void Norepinephrine::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery of other parameters
    pImpl->attentionModulation *= (1.0f - recoveryRate * dt);
    pImpl->arousalModulation *= (1.0f - recoveryRate * dt);
    pImpl->stressResponse *= (1.0f - recoveryRate * dt);
    pImpl->executiveFunction += (1.0f - pImpl->executiveFunction) * recoveryRate * dt;
    
    pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 2.0f);
    pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    pImpl->stressResponse = std::clamp(pImpl->stressResponse, 0.0f, 2.0f);
    pImpl->executiveFunction = std::clamp(pImpl->executiveFunction, 0.5f, 2.0f);
}

void Norepinephrine::setVigilanceLevel(float level) {
    pImpl->vigilanceModulation = std::clamp(level, 0.5f, 2.0f);
}

void Norepinephrine::updateThreateningInput(float threatLevel) {
    // Update threat response based on NE activity
    pImpl->threatSensitivity += threatLevel * 0.1f * pImpl->level;
    pImpl->threatSensitivity = std::clamp(pImpl->threatSensitivity, 0.5f, 2.0f);
    
    // High threat increases stress response
    if (threatLevel > 0.7f) {
        pImpl->emergencyActivation += threatLevel * 0.2f;
        pImpl->emergencyActivation = std::clamp(pImpl->emergencyActivation, 0.0f, 2.0f);
    }
}

void Norepinephrine::enhanceMemoryEncoding(float learningRate) {
    // Memory encoding enhancement
    pImpl->learningRate += learningRate * pImpl->level * 0.1f;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    
    // Enhanced learning rate improves working memory
    pImpl->workingMemoryModulation += learningRate * 0.1f;
    pImpl->workingMemoryModulation = std::clamp(pImpl->workingMemoryModulation, 0.5f, 2.0f);
}

void Norepinephrine::modulateResponseSelection(float competition) {
    // Response selection modulation
    pImpl->executiveFunction *= (1.0f - competition * 0.2f);
    pImpl->executiveFunction = std::clamp(pImpl->executiveFunction, 0.5f, 2.0f);
    
    // High competition reduces response speed
    if (competition > 0.8f) {
        pImpl->arousalModulation *= 0.8f;
        pImpl->arousalModulation = std::clamp(pImpl->arousalModulation, 0.5f, 2.0f);
    }
}

void Norepinephrine::updateAdaptiveAllocation(float resourceAvailability) {
    // Adaptive resource allocation
    pImpl->workingMemoryModulation += resourceAvailability * pImpl->level * 0.1f;
    pImpl->workingMemoryModulation = std::clamp(pImpl->workingMemoryModulation, 0.5f, 2.0f);
    
    // Better resource allocation improves executive function
    pImpl->executiveFunction += resourceAvailability * 0.05f;
    pImpl->executiveFunction = std::clamp(pImpl->executiveFunction, 0.5f, 2.0f);
}

float Norepinephrine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Norepinephrine::getArousalModulation() const {
    return pImpl->arousalModulation;
}

float Norepinephrine::getStressModulation() const {
    return pImpl->stressResponse;
}

float Norepinephrine::getCognitiveModulation() const {
    return pImpl->executiveFunction;
}

} // namespace nlm