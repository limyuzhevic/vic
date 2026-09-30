#include "Glutamate.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct GlutamateImpl {
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
    
    // Excitatory and plasticity parameters
    float excitatoryStrength;       // Excitatory signal strength
    float LTPStrength;              // Long-term potentiation strength
    float LTDStrength;              // Long-term depression strength
    float synapticEfficiency;        // Synaptic efficiency
    
    // Memory and learning
    float learningRate;              // Learning rate enhancement
    float patternCompletion;         // Pattern completion capability
    float interference;              // Interference with other memories
    
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
    
    GlutamateImpl() :
        level(0.0f), baseline(0.01f), peak(2.0f),
        decayRate(0.1f), releaseRate(0.5f), synthesisRate(0.03f),
        reuptakeRate(0.15f), receptorDensity(1.4f), bindingAffinity(0.95f),
        excitatoryStrength(1.0f), LTPStrength(1.0f), LTDStrength(1.0f),
        synapticEfficiency(1.0f), learningRate(1.0f), patternCompletion(1.0f),
        interference(0.1f), developmentalOffset(1.0f),
        maturationStage(1.0f), experienceModulation(1.0f),
        memoryTrace(0.0f), eligibilityTrace(0.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for Glu
        // Glutamate is the main excitatory neurotransmitter
        interactionMatrix[0][0] = 1.0f;  // Glu-Glu self-interaction
        interactionMatrix[0][1] = -0.3f; // Glu-5HT: inhibition
        interactionMatrix[0][2] = -0.2f; // Glu-NE: inhibition
        interactionMatrix[0][3] = -0.2f; // Glu-ACh: inhibition
        interactionMatrix[0][4] = 0.5f;  // Glu-GABA: excitation
        interactionMatrix[0][5] = 1.0f;  // Glu-Glu: self
        
        // Other neuromodulators' effects on Glu
        interactionMatrix[1][5] = -0.4f; // 5HT-Glu: inhibition
        interactionMatrix[2][5] = -0.3f; // NE-Glu: inhibition
        interactionMatrix[3][5] = -0.3f; // ACh-Glu: inhibition
        interactionMatrix[4][5] = 0.6f;  // GABA-Glu: excitation
        interactionMatrix[5][0] = 1.0f;  // Glu-Glu: self
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Glutamate::Glutamate() : pImpl(new Impl) {
    NLM_LOG_INFO("Glutamate neuromodulator initialized (Phase 6 Enhanced)");
}

Glutamate::~Glutamate() = default;

const char* Glutamate::getName() const {
    return "Glu";
}

float Glutamate::getLevel() const {
    return pImpl->level;
}

void Glutamate::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Glutamate::getPlasticityFactor() const {
    // Glutamate-modulated plasticity factor
    // Glu supports synaptic plasticity and memory formation
    float plasticity = 0.8f + pImpl->level * 1.5f;  // Higher Glu = more plasticity
    
    // Apply LTP/LTD balance
    float ltpLTDBalance = pImpl->LTPStrength / (pImpl->LTPStrength + pImpl->LTDStrength);
    plasticity *= (0.7f + ltpLTDBalance * 0.6f);  // More LTP = more overall plasticity
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.3f, 3.0f);
}

void Glutamate::update(TimestepDuration dt) {
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
    
    NLM_LOG_DEBUG("Glutamate updated: level=" + std::to_string(pImpl->level) + 
                 ", excitatory_strength=" + std::to_string(pImpl->excitatoryStrength) +
                 ", learning_rate=" + std::to_string(pImpl->learningRate));
}

void Glutamate::enhanceExcitatorySignaling(float synapticStrength) {
    // Excitatory signaling based on Glu activity
    float excitationGain = synapticStrength * pImpl->level * 0.5f;
    pImpl->excitatoryStrength += excitationGain;
    pImpl->excitatoryStrength = std::clamp(pImpl->excitatoryStrength, 0.0f, 5.0f);
    
    // Enhanced excitation increases baseline
    pImpl->baseline = std::min(pImpl->baseline + excitationGain * 0.1f, 1.0f);
    
    NLM_LOG_DEBUG("Glutamate excitatory signaling: synaptic_strength=" + std::to_string(synapticStrength) + 
                 ", gain=" + std::to_string(excitationGain) +
                 ", strength=" + std::to_string(pImpl->excitatoryStrength));
}

void Glutamate::facilitateLongTermPotentiation(float learningRate, float predictionError) {
    // Long-term potentiation facilitation based on Glu activity
    // LTP is the cellular mechanism for learning and memory
    float ltpGain = learningRate * pImpl->level * 0.4f;
    pImpl->LTPStrength += ltpGain;
    pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
    
    // Positive prediction errors enhance LTP
    if (predictionError > 0.0f) {
        pImpl->LTPStrength += predictionError * 0.2f;
        pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
        
        // Enhance learning rate based on positive prediction errors
        pImpl->learningRate += predictionError * 0.1f;
        pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    }
    
    // Negative prediction errors promote LTD
    if (predictionError < 0.0f) {
        pImpl->LTDStrength += std::abs(predictionError) * 0.2f;
        pImpl->LTDStrength = std::clamp(pImpl->LTDStrength, 0.5f, 3.0f);
    }
    
    NLM_LOG_DEBUG("Glutamate LTP facilitation: learning_rate=" + std::to_string(learningRate) + 
                 ", prediction_error=" + std::to_string(predictionError) +
                 ", ltp_strength=" + std::to_string(pImpl->LTPStrength) +
                 ", ltd_strength=" + std::to_string(pImpl->LTDStrength));
}

void Glutamate::activateCorticalExcitability(float neuralActivation) {
    // Cortical excitability based on Glu activity
    float activationGain = neuralActivation * pImpl->level * 0.3f;
    pImpl->excitatoryStrength += activationGain;
    pImpl->excitatoryStrength = std::clamp(pImpl->excitatoryStrength, 0.0f, 5.0f);
    
    // Enhanced excitation increases cortical activation
    pImpl->synapticEfficiency = std::min(pImpl->synapticEfficiency, pImpl->excitatoryStrength);
    
    NLM_LOG_DEBUG("Glutamate cortical activation: neural_activation=" + std::to_string(neuralActivation) + 
                 ", gain=" + std::to_string(activationGain) +
                 ", strength=" + std::to_string(pImpl->excitatoryStrength));
}

void Glutamate::modulateSensoryExcitability(float stimulusIntensity) {
    // Sensory processing modulation based on Glu activity
    float sensoryGain = stimulusIntensity * pImpl->level * 0.4f;
    pImpl->excitatoryStrength += sensoryGain;
    pImpl->excitatoryStrength = std::clamp(pImpl->excitatoryStrength, 0.0f, 5.0f);
    
    // Stronger sensory inputs with high Glu
    pImpl->synapticEfficiency = std::min(pImpl->synapticEfficiency, 1.5f * stimulusIntensity);
    
    NLM_LOG_DEBUG("Glutamate sensory excitability: stimulus_intensity=" + std::to_string(stimulusIntensity) + 
                 ", gain=" + std::to_string(sensoryGain) +
                 ", strength=" + std::to_string(pImpl->excitatoryStrength));
}

void Glutamate::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // Glu supports prediction error-based learning
    if (predictionError != 0.0f) {
        // Enhance LTP for positive prediction errors
        if (predictionError > 0.0f) {
            pImpl->LTPStrength += predictionError * 0.1f;
            pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
        }
        
        // Enhance LTD for negative prediction errors
        if (predictionError < 0.0f) {
            pImpl->LTDStrength += std::abs(predictionError) * 0.1f;
            pImpl->LTDStrength = std::clamp(pImpl->LTDStrength, 0.5f, 3.0f);
        }
        
        // Update learning rate based on prediction error magnitude
        pImpl->learningRate += std::abs(predictionError) * 0.05f;
        pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
    }
}

void Glutamate::enhanceSynapticPlasticity(float eligibilityTrace) {
    // Enhance synaptic plasticity based on Glu activity
    pImpl->plasticityFactors.push_back(eligibilityTrace * pImpl->level);
    if (pImpl->plasticityFactors.size() > 100) {
        pImpl->plasticityFactors.erase(pImpl->plasticityFactors.begin());
    }
    
    // Glu enhances both LTP and LTD based on eligibility traces
    if (eligibilityTrace > 0.0f) {
        pImpl->LTPStrength += eligibilityTrace * 0.1f;
        pImpl->LTDStrength += eligibilityTrace * 0.05f;
        pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
        pImpl->LTDStrength = std::clamp(pImpl->LTDStrength, 0.5f, 3.0f);
    }
}

void Glutamate::updateExcitatoryDynamics(float dt) {
    // Excitatory dynamics including burst and tonic firing
    
    // Tonic firing - steady background activity
    float tonicChange = pImpl->synthesisRate * dt - pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += tonicChange;
    
    // Phasic firing - burst responses to salient stimuli
    if (pImpl->level > pImpl->baseline) {
        float phasicBurst = pImpl->level * 0.03f * dt;
        pImpl->level += phasicBurst;
    }
    
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Glutamate::encodeLTPMemory(float potentiation, float duration) {
    // LTP memory encoding
    float memoryStrength = potentiation * pImpl->level * 0.3f;
    pImpl->memoryTrace += memoryStrength;
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    // Longer duration of potentiation enhances memory strength
    pImpl->LTPStrength += duration * 0.01f;
    pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
    
    NLM_LOG_DEBUG("Glutamate LTP memory: potentiation=" + std::to_string(potentiation) + 
                 ", duration=" + std::to_string(duration) +
                 ", strength=" + std::to_string(memoryStrength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace) +
                 ", ltp_strength=" + std::to_string(pImpl->LTPStrength));
}

void Glutamate::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery of other parameters
    pImpl->LTPStrength += (1.0f - pImpl->LTPStrength) * recoveryRate * dt;
    pImpl->LTDStrength += (1.0f - pImpl->LTDStrength) * recoveryRate * dt;
    pImpl->learningRate += (1.0f - pImpl->learningRate) * recoveryRate * dt;
    
    pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
    pImpl->LTDStrength = std::clamp(pImpl->LTDStrength, 0.5f, 3.0f);
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
}

void Glutamate::setSensoryGain(float gainLevel) {
    pImpl->excitatoryStrength = std::clamp(gainLevel, 0.0f, 5.0f);
}

void Glutamate::updateTemporalIntegration(float timeWindow) {
    // Temporal integration based on Glu activity
    pImpl->synapticEfficiency *= (1.0f + timeWindow * 0.1f);
    pImpl->synapticEfficiency = std::clamp(pImpl->synapticEfficiency, 1.0f, 3.0f);
}

void Glutamate::enhancePatternSeparation(float patternDistance) {
    // Pattern separation enhancement
    float separationEffect = patternDistance * pImpl->level * 0.3f;
    pImpl->LTPStrength += separationEffect;
    pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
}

void Glutamate::modulateStructuralPlasticity(float structuralChange) {
    // Structural plasticity modulation
    pImpl->synapticEfficiency *= (1.0f + structuralChange * 0.2f);
    pImpl->synapticEfficiency = std::clamp(pImpl->synapticEfficiency, 1.0f, 3.0f);
}

void Glutamate::updateHebbianLearning(float correlation) {
    // Hebbian learning modulation
    pImpl->learningRate += correlation * pImpl->level * 0.1f;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 1.0f, 3.0f);
}

void Glutamate::enhanceMetaplasticity(float metaplasticState) {
    // Metaplasticity enhancement
    pImpl->LTPStrength *= (1.0f + metaplasticState * 0.1f);
    pImpl->LTDStrength *= (1.0f + metaplasticState * 0.1f);
    pImpl->LTPStrength = std::clamp(pImpl->LTPStrength, 0.5f, 3.0f);
    pImpl->LTDStrength = std::clamp(pImpl->LTDStrength, 0.5f, 3.0f);
}

float Glutamate::getExcitabilityModulation() const {
    return pImpl->excitatoryStrength;
}

float Glutamate::getLTPModulation() const {
    return pImpl->LTPStrength;
}

float Glutamate::getSynapticModulation() const {
    return pImpl->synapticEfficiency;
}

float Glutamate::getLearningModulation() const {
    return pImpl->learningRate;
}

} // namespace nlm