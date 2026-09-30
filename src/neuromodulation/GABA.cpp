#include "GABA.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct GABA::Impl {
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
    
    // Inhibition and anxiety parameters
    float inhibitoryControl;         // Inhibitory control strength
    float anxietyModulation;        // Anxiety regulation
    float fearResponse;             // Fear response level
    float impulseControl;            // Impulse control enhancement
    
    // Behavioral inhibition
    float behavioralInhibition;     // Behavioral inhibition level
    float responseSuppression;      // Response suppression strength
    float riskAssessment;            // Risk assessment and avoidance
    
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
    
    GABAImpl() :
        level(0.0f), baseline(0.02f), peak(2.0f),
        decayRate(0.1f), releaseRate(0.3f), synthesisRate(0.02f),
        reuptakeRate(0.25f), receptorDensity(1.3f), bindingAffinity(0.9f),
        inhibitoryControl(1.0f), anxietyModulation(1.0f), fearResponse(0.5f),
        impulseControl(1.0f), behavioralInhibition(1.0f), 
        responseSuppression(1.0f), riskAssessment(0.5f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for GABA
        // GABA is the main inhibitory neurotransmitter
        interactionMatrix[0][0] = 1.0f;  // GABA-GABA self-interaction
        interactionMatrix[0][1] = 0.3f;  // GABA-5HT: excitation
        interactionMatrix[0][2] = 0.2f;  // GABA-NE: excitation
        interactionMatrix[0][3] = 0.2f;  // GABA-ACh: excitation
        interactionMatrix[0][4] = 1.0f;  // GABA-GABA: self
        interactionMatrix[0][5] = -0.5f; // GABA-Glu: inhibition
        
        // Other neuromodulators' effects on GABA
        interactionMatrix[1][4] = 0.4f;  // 5HT-GABA: excitation
        interactionMatrix[2][4] = 0.3f;  // NE-GABA: excitation
        interactionMatrix[3][4] = 0.3f;  // ACh-GABA: excitation
        interactionMatrix[4][0] = 1.0f;  // GABA-GABA: self
        interactionMatrix[5][4] = -0.3f; // Glu-GABA: inhibition
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

GABA::GABA() : pImpl(new Impl) {
    NLM_LOG_INFO("GABA neuromodulator initialized (Phase 6 Enhanced)");
}

GABA::~GABA() = default;

const char* GABA::getName() const {
    return "GABA";
}

float GABA::getLevel() const {
    return pImpl->level;
}

void GABA::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float GABA::getPlasticityFactor() const {
    // GABA-modulated plasticity factor
    // GABA generally reduces plasticity by providing inhibition
    float plasticity = 0.6f + pImpl->level * 0.4f;  // Higher GABA = less plasticity
    
    // Apply anxiety modulation
    if (pImpl->anxietyModulation > 0.7f) {  // High anxiety
        plasticity *= 1.2f;  // Increased plasticity under anxiety
    }
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.2f, 1.8f);
}

void GABA::update(TimestepDuration dt) {
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
    
    NLM_LOG_DEBUG("GABA updated: level=" + std::to_string(pImpl->level) + 
                 ", inhibitory_control=" + std::to_string(pImpl->inhibitoryControl) +
                 ", anxiety=" + std::to_string(pImpl->anxietyModulation));
}

void GABA::inhibitExcitation(float excitationLevel) {
    // Inhibition of excitation based on GABA activity
    float inhibitionStrength = excitationLevel * pImpl->level * 0.4f;
    pImpl->inhibitoryControl += inhibitionStrength;
    pImpl->inhibitoryControl = std::clamp(pImpl->inhibitoryControl, 0.0f, 2.0f);
    
    // Reduce neural excitability
    if (inhibitionStrength > 0.5f) {
        // Significant inhibition reduces fear and anxiety responses
        pImpl->fearResponse *= (1.0f - inhibitionStrength);
        pImpl->fearResponse = std::clamp(pImpl->fearResponse, 0.0f, 1.0f);
    }
    
    NLM_LOG_DEBUG("GABA inhibition: excitation_level=" + std::to_string(excitationLevel) + 
                 ", strength=" + std::to_string(inhibitionStrength) +
                 ", control=" + std::to_string(pImpl->inhibitoryControl));
}

void GABA::regulateAnxiety(float anxietyLevel) {
    // Anxiety regulation based on GABA activity
    // GABA generally reduces anxiety
    float anxietyReduction = anxietyLevel * pImpl->level * 0.3f;
    pImpl->anxietyModulation -= anxietyReduction;
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
    
    // Update fear response based on anxiety
    pImpl->fearResponse = std::max(pImpl->fearResponse - anxietyLevel * 0.05f, 0.0f);
    
    NLM_LOG_DEBUG("GABA anxiety regulation: anxiety_level=" + std::to_string(anxietyLevel) + 
                 ", reduction=" + std::to_string(anxietyReduction) +
                 ", anxiety=" + std::to_string(pImpl->anxietyModulation) +
                 ", fear=" + std::to_string(pImpl->fearResponse));
}

void GABA::controlImpulsiveBehavior(float impulseStrength) {
    // Impulse control enhancement based on GABA activity
    float controlStrength = impulseStrength * pImpl->level * 0.5f;
    pImpl->impulseControl += controlStrength;
    pImpl->impulseControl = std::clamp(pImpl->impulseControl, 0.0f, 2.0f);
    
    // Reduce behavioral inhibition when impulse control is high
    if (controlStrength > 0.5f) {
        pImpl->behavioralInhibition *= (1.0f - controlStrength * 0.3f);
        pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
    }
    
    NLM_LOG_DEBUG("GABA impulse control: impulse_strength=" + std::to_string(impulseStrength) + 
                 ", strength=" + std::to_string(controlStrength) +
                 ", control=" + std::to_string(pImpl->impulseControl));
}

void GABA::modulateFearResponse(float fearLevel, float context) {
    // Fear response modulation based on GABA activity
    // GABA generally reduces fear responses
    float fearReduction = fearLevel * pImpl->level * (1.0f - context * 0.3f);
    pImpl->fearResponse -= fearReduction;
    pImpl->fearResponse = std::max(pImpl->fearResponse, 0.0f);
    
    // High GABA increases risk assessment (more cautious behavior)
    pImpl->riskAssessment += pImpl->level * 0.05f;
    pImpl->riskAssessment = std::clamp(pImpl->riskAssessment, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("GABA fear modulation: fear_level=" + std::to_string(fearLevel) + 
                 ", context=" + std::to_string(context) +
                 ", reduction=" + std::to_string(fearReduction) +
                 ", fear=" + std::to_string(pImpl->fearResponse) +
                 ", risk_assessment=" + std::to_string(pImpl->riskAssessment));
}

void GABA::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // GABA helps stabilize responses to prediction errors
    if (predictionError != 0.0f) {
        // Moderate prediction error increases anxiety (adaptive)
        pImpl->anxietyModulation += std::abs(predictionError) * 0.1f;
        pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
        
        // Reduce impulsivity based on prediction error
        pImpl->impulseControl -= std::abs(predictionError) * 0.05f;
        pImpl->impulseControl = std::clamp(pImpl->impulseControl, 0.0f, 2.0f);
    }
}

void GABA::enhanceInhibitoryPlasticity(float learningRate) {
    // Enhance inhibitory plasticity based on GABA activity
    pImpl->experienceModulation += learningRate * 0.1f;
    pImpl->experienceModulation = std::clamp(pImpl->experienceModulation, 1.0f, 2.0f);
    
    // Higher GABA increases synaptic inhibition
    pImpl->inhibitoryControl += pImpl->level * 0.02f;
    pImpl->inhibitoryControl = std::clamp(pImpl->inhibitoryControl, 0.0f, 2.0f);
}

void GABA::updateInhibitoryDynamics(float dt) {
    // Inhibitory dynamics including tonic and phasic inhibition
    
    // Tonic inhibition - steady background activity
    float tonicChange = pImpl->synthesisRate * dt - pImpl->reuptakeRate * pImpl->level * dt;
    pImpl->level += tonicChange;
    
    // Phasic inhibition - burst responses to salient stimuli
    if (pImpl->level > pImpl->baseline) {
        float phasicBurst = pImpl->level * 0.02f * dt;
        pImpl->level += phasicBurst;
    }
    
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void GABA::encodeInhibitionMemory(float inhibition, float duration) {
    // Inhibition memory encoding
    float memoryStrength = inhibition * pImpl->level * 0.3f;
    pImpl->memoryTrace += memoryStrength;
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 20.0f);
    
    // Longer duration of inhibition enhances inhibitory control
    pImpl->behavioralInhibition += duration * 0.01f;
    pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("GABA inhibition memory: inhibition=" + std::to_string(inhibition) + 
                 ", duration=" + std::to_string(duration) +
                 ", strength=" + std::to_string(memoryStrength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace) +
                 ", behavioral_inhibition=" + std::to_string(pImpl->behavioralInhibition));
}

void GABA::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery of other parameters
    pImpl->anxietyModulation *= (1.0f - recoveryRate * dt);
    pImpl->fearResponse *= (1.0f - recoveryRate * dt);
    pImpl->impulseControl += (1.0f - pImpl->impulseControl) * recoveryRate * dt;
    
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
    pImpl->fearResponse = std::clamp(pImpl->fearResponse, 0.0f, 1.0f);
    pImpl->impulseControl = std::clamp(pImpl->impulseControl, 0.0f, 2.0f);
}

void GABA::setFearConditioning(float conditioningStrength) {
    pImpl->fearResponse = std::clamp(conditioningStrength, 0.0f, 1.0f);
}

void GABA::updateThreateningInput(float threatLevel) {
    // Update threat response based on GABA activity
    pImpl->fearResponse += threatLevel * 0.05f * pImpl->level;
    pImpl->fearResponse = std::clamp(pImpl->fearResponse, 0.0f, 1.0f);
    
    // High threat increases anxiety
    pImpl->anxietyModulation += threatLevel * 0.02f;
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
}

void GABA::enhanceConflictResolution(float conflictLevel) {
    // Conflict resolution enhancement
    float resolutionStrength = conflictLevel * pImpl->level * 0.3f;
    pImpl->behavioralInhibition += resolutionStrength;
    pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
}

void GABA::modulateSocialBehavior(float socialContext) {
    // Social behavior modulation
    if (socialContext > 0.5f) {  // Positive social context
        pImpl->behavioralInhibition *= 0.9f;  // Reduced inhibition in positive contexts
        pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
    } else {  // Negative social context
        pImpl->behavioralInhibition *= 1.1f;  // Increased inhibition in negative contexts
        pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
    }
}

void GABA::updateStressResilience(float resilienceFactor) {
    // Stress resilience enhancement
    pImpl->inhibitoryControl += resilienceFactor * 0.1f;
    pImpl->inhibitoryControl = std::clamp(pImpl->inhibitoryControl, 0.0f, 2.0f);
    
    // Higher resilience reduces fear and anxiety
    pImpl->fearResponse *= (1.0f - resilienceFactor * 0.2f);
    pImpl->anxietyModulation *= (1.0f - resilienceFactor * 0.2f);
}

void GABA::enhanceEmotionalRegulation(float emotionalStability) {
    // Emotional regulation enhancement
    pImpl->behavioralInhibition += emotionalStability * 0.05f;
    pImpl->behavioralInhibition = std::clamp(pImpl->behavioralInhibition, 0.5f, 2.0f);
    
    // Better emotional regulation reduces impulse control issues
    pImpl->impulseControl += emotionalStability * 0.1f;
    pImpl->impulseControl = std::clamp(pImpl->impulseControl, 0.0f, 2.0f);
}

float GABA::getInhibitionModulation() const {
    return pImpl->inhibitoryControl;
}

float GABA::getAnxietyModulation() const {
    return pImpl->anxietyModulation;
}

float GABA::getImpulseControlModulation() const {
    return pImpl->impulseControl;
}

float GABA::getFearModulation() const {
    return pImpl->fearResponse;
}

} // namespace nlm