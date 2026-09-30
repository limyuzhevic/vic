#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
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
    
    // Memory and attention parameters
    float memoryConsolidation;      // Memory consolidation strength
    float attentionSpan;            // Attention span enhancement
    float corticalActivation;        // Cortical activation level
    float sensoryProcessing;        // Sensory processing modulation
    
    // Learning rate modulation
    float learningRate;              // Learning rate enhancement
    float noveltySensitivity;        // Sensitivity to novel information
    float explorationBias;           // Bias toward exploration vs exploitation
    
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
    
    AcetylcholineImpl() :
        level(0.0f), baseline(0.02f), peak(1.5f),
        decayRate(0.15f), releaseRate(0.35f), synthesisRate(0.025f),
        reuptakeRate(0.2f), receptorDensity(1.8f), bindingAffinity(0.85f),
        memoryConsolidation(1.0f), attentionSpan(1.0f), corticalActivation(0.5f),
        sensoryProcessing(1.0f), learningRate(1.0f), noveltySensitivity(1.0f),
        explorationBias(0.5f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for ACh
        // ACh enhances memory, attention, and cortical activation
        interactionMatrix[0][0] = 1.0f;  // ACh-ACh self-interaction
        interactionMatrix[0][1] = 0.4f;  // ACh-5HT: excitation
        interactionMatrix[0][2] = 0.5f;  // ACh-NE: excitation
        interactionMatrix[0][3] = 1.0f;  // ACh-ACh: self
        interactionMatrix[0][4] = 0.3f;  // ACh-GABA: excitation
        interactionMatrix[0][5] = 0.6f;  // ACh-Glu: excitation
        
        // Other neuromodulators' effects on ACh
        interactionMatrix[1][3] = 0.5f;  // 5HT-ACh: excitation
        interactionMatrix[2][3] = 0.6f;  // NE-ACh: excitation
        interactionMatrix[3][0] = 1.0f;  // ACh-ACh: self
        interactionMatrix[4][3] = 0.4f;  // GABA-ACh: excitation
        interactionMatrix[5][3] = 0.5f;  // Glu-ACh: excitation
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized (Phase 6 Enhanced)");
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine-modulated plasticity factor
    // ACh enhances cortical plasticity and memory formation
    float plasticity = 0.8f + pImpl->level * 1.2f;  // Higher ACh = more plasticity
    
    // Apply learning rate modulation
    plasticity *= pImpl->learningRate;
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.2f, 2.5f);
}

void Acetylcholine::update(TimestepDuration dt) {
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
    
    NLM_LOG_DEBUG("Acetylcholine updated: level=" + std::to_string(pImpl->level) + 
                 ", memory_consolidation=" + std::to_string(pImpl->memoryConsolidation) +
                 ", learning_rate=" + std::to_string(pImpl->learningRate));
}

void Acetylcholine::consolidateMemory(float memoryStrength, float age) {
    // Memory consolidation based on ACh activity
    // ACh helps transfer memories from hippocampus to neocortex
    float consolidationEffect = memoryStrength * pImpl->level * (1.0f + age * 0.1f);
    pImpl->memoryConsolidation += consolidationEffect;
    pImpl->memoryConsolidation = std::clamp(pImpl->memoryConsolidation, 0.0f, 5.0f);
    
    // Update memory trace
    pImpl->memoryTrace += memoryStrength * (1.0f + pImpl->level * 0.5f);
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    NLM_LOG_DEBUG("Acetylcholine memory consolidation: memory_strength=" + std::to_string(memoryStrength) + 
                 ", age=" + std::to_string(age) +
                 ", effect=" + std::to_string(consolidationEffect) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace));
}

void Acetylcholine::enhanceAttentionSpan(float taskComplexity) {
    // Attention span enhancement based on ACh activity
    float attentionGain = taskComplexity * pImpl->level * 0.4f;
    pImpl->attentionSpan += attentionGain;
    pImpl->attentionSpan = std::clamp(pImpl->attentionSpan, 0.5f, 5.0f);
    
    // Better attention for complex tasks
    if (taskComplexity > 0.5f) {
        pImpl->corticalActivation += pImpl->level * 0.2f;
        pImpl->corticalActivation = std::clamp(pImpl->corticalActivation, 0.5f, 2.0f);
    }
    
    NLM_LOG_DEBUG("Acetylcholine attention enhancement: task_complexity=" + std::to_string(taskComplexity) + 
                 ", gain=" + std::to_string(attentionGain) +
                 ", attention_span=" + std::to_string(pImpl->attentionSpan));
}

void Acetylcholine::activateCorticalAreas(float targetActivation) {
    // Cortical activation based on ACh activity
    float activationAdjustment = (targetActivation - pImpl->corticalActivation) * pImpl->level * 0.3f;
    pImpl->corticalActivation += activationAdjustment;
    pImpl->corticalActivation = std::clamp(pImpl->corticalActivation, 0.5f, 2.0f);
    
    // Enhanced sensory processing with cortical activation
    pImpl->sensoryProcessing = std::min(pImpl->sensoryProcessing, pImpl->corticalActivation);
    
    NLM_LOG_DEBUG("Acetylcholine cortical activation: target=" + std::to_string(targetActivation) + 
                 ", current=" + std::to_string(pImpl->corticalActivation) +
                 ", adjustment=" + std::to_string(activationAdjustment));
}

void Acetylcholine::modulateSensoryProcessing(float stimulusIntensity) {
    // Sensory processing modulation based on ACh activity
    float sensoryGain = stimulusIntensity * pImpl->level * 0.3f;
    pImpl->sensoryProcessing += sensoryGain;
    pImpl->sensoryProcessing = std::clamp(pImpl->sensoryProcessing, 1.0f, 3.0f);
    
    NLM_LOG_DEBUG("Acetylcholine sensory processing: stimulus_intensity=" + std::to_string(stimulusIntensity) + 
                 ", gain=" + std::to_string(sensoryGain) +
                 ", processing=" + std::to_string(pImpl->sensoryProcessing));
}

void Acetylcholine::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // ACh helps reduce prediction error through enhanced learning
    if (predictionError != 0.0f) {
        // Enhance learning rate based on prediction error
        pImpl->learningRate += std::abs(predictionError) * 0.2f;
        pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
        
        // Update novelty sensitivity
        pImpl->noveltySensitivity += std::abs(predictionError) * 0.1f;
        pImpl->noveltySensitivity = std::clamp(pImpl->noveltySensitivity, 0.5f, 2.0f);
    }
}

void Acetylcholine::enhanceLearningRate(float novelty, float uncertainty) {
    // Learning rate enhancement based on novelty and uncertainty
    float learningBoost = (novelty + uncertainty) * pImpl->level * 0.3f;
    pImpl->learningRate += learningBoost;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    
    // Higher learning rate with high novelty encourages exploration
    if (novelty > 0.7f) {
        pImpl->explorationBias += 0.1f;
        pImpl->explorationBias = std::clamp(pImpl->explorationBias, 0.3f, 1.0f);
    }
    
    NLM_LOG_DEBUG("Acetylcholine learning rate enhancement: novelty=" + std::to_string(novelty) + 
                 ", uncertainty=" + std::to_string(uncertainty) +
                 ", boost=" + std::to_string(learningBoost) +
                 ", learning_rate=" + std::to_string(pImpl->learningRate));
}

void Acetylcholine::updateCholinergicDynamics(float dt) {
    // Cholinergic dynamics including burst and tonic firing modes
    
    // Tonic firing - steady background activity
    float tonicChange = pImpl->synthesisRate * dt - pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += tonicChange;
    
    // Phasic firing - burst responses to salient stimuli
    if (pImpl->level > pImpl->baseline) {
        float phasicBurst = pImpl->level * 0.02f * dt;
        pImpl->level += phasicBurst;
    }
    
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Acetylcholine::encodeWorkingMemory(float content, float relevance) {
    // Working memory encoding based on ACh activity
    float encodingStrength = relevance * pImpl->level * 0.3f;
    pImpl->memoryTrace += encodingStrength;
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    // Higher ACh enhances working memory capacity
    pImpl->learningRate += pImpl->level * 0.05f;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    
    NLM_LOG_DEBUG("Acetylcholine working memory encoding: content=" + std::to_string(content) + 
                 ", relevance=" + std::to_string(relevance) +
                 ", strength=" + std::to_string(encodingStrength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace));
}

void Acetylcholine::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery of other parameters
    pImpl->learningRate += (1.0f - pImpl->learningRate) * recoveryRate * dt;
    pImpl->noveltySensitivity += (1.0f - pImpl->noveltySensitivity) * recoveryRate * dt;
    pImpl->explorationBias += (0.5f - pImpl->explorationBias) * recoveryRate * dt;
    
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    pImpl->noveltySensitivity = std::clamp(pImpl->noveltySensitivity, 0.5f, 2.0f);
    pImpl->explorationBias = std::clamp(pImpl->explorationBias, 0.3f, 1.0f);
}

void Acetylcholine::setNoveltyDetection(float detectionThreshold) {
    pImpl->noveltySensitivity = std::clamp(detectionThreshold, 0.0f, 2.0f);
}

void Acetylcholine::updatePatternSeparation(float patternSimilarity) {
    // Pattern separation based on ACh activity
    float separationEffect = (1.0f - patternSimilarity) * pImpl->level * 0.4f;
    pImpl->learningRate += separationEffect * 0.5f;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
}

void Acetylcholine::enhanceTemporalProcessing(float temporalPrecision) {
    // Temporal processing enhancement
    pImpl->sensoryProcessing *= (1.0f + temporalPrecision * 0.3f);
    pImpl->sensoryProcessing = std::clamp(pImpl->sensoryProcessing, 1.0f, 3.0f);
}

void Acetylcholine::modulateInhibitoryControl(float inhibitionLevel) {
    // Inhibitory control modulation
    pImpl->corticalActivation *= (1.0f - inhibitionLevel * 0.2f);
    pImpl->corticalActivation = std::clamp(pImpl->corticalActivation, 0.5f, 2.0f);
}

void Acetylcholine::updateReceptiveFieldExpansion(float expansionRate) {
    // Receptive field expansion
    pImpl->sensoryProcessing *= (1.0f + expansionRate * 0.2f);
    pImpl->sensoryProcessing = std::clamp(pImpl->sensoryProcessing, 1.0f, 3.0f);
}

float Acetylcholine::getMemoryModulation() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionSpan;
}

float Acetylcholine::getCorticalModulation() const {
    return pImpl->corticalActivation;
}

float Acetylcholine::getLearningModulation() const {
    return pImpl->learningRate;
}

} // namespace nlm