#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <random>

namespace nlm {

struct SerotoninImpl {
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
    
    // Mood and affective state
    float moodModulation;            // Mood enhancement/suppression
    float anxietyModulation;        // Anxiety regulation
    float arousalModulation;        // Arousal level
    float socialModulation;          // Social behavior modulation
    float impulsivityModulation;     // Impulsivity control
    
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
    
    // Stress and emotional state
    float stressLevel;              // Current stress level
    float emotionalState;            // Current emotional state
    float rewardSensitivity;         // Sensitivity to reward
    
    // Random generator for probabilistic responses
    std::mt19937 rng;
    std::uniform_real_distribution<float> responseDist;
    
    SerotoninImpl() :
        level(0.0f), baseline(0.03f), peak(1.0f),
        decayRate(0.08f), releaseRate(0.3f), synthesisRate(0.04f),
        reuptakeRate(0.3f), receptorDensity(1.2f), bindingAffinity(0.9f),
        moodModulation(1.0f), anxietyModulation(1.0f), arousalModulation(0.5f),
        socialModulation(1.0f), impulsivityModulation(1.0f),
        developmentalOffset(1.0f), maturationStage(1.0f),
        experienceModulation(1.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
        stressLevel(0.0f), emotionalState(0.5f), rewardSensitivity(1.0f),
        rng(std::random_device()()), responseDist(0.0f, 1.0f) {
        
        history.reserve(100);
        predictionErrors.reserve(100);
        plasticityFactors.reserve(100);
        neuralExcitability.reserve(100);
        
        // Initialize interaction matrix for 5-HT
        // 5-HT regulates mood, reduces anxiety, increases satiety
        interactionMatrix[0][0] = 1.0f;  // 5-HT-5-HT self-interaction
        interactionMatrix[0][1] = -0.5f; // 5-HT-DA: inhibition (reduces approach)
        interactionMatrix[0][2] = -0.3f; // 5-HT-NE: inhibition (reduces arousal)
        interactionMatrix[0][3] = 0.2f;  // 5-HT-ACh: excitation
        interactionMatrix[0][4] = 0.4f;  // 5-HT-GABA: excitation (inhibits excitation)
        interactionMatrix[0][5] = -0.2f; // 5-HT-Glu: inhibition
        
        // Other neuromodulators' effects on 5-HT
        interactionMatrix[1][0] = 0.5f;  // DA-5-HT: excitation
        interactionMatrix[2][0] = 0.3f;  // NE-5-HT: excitation
        interactionMatrix[3][0] = -0.2f; // ACh-5-HT: inhibition
        interactionMatrix[4][0] = 0.2f;  // GABA-5-HT: excitation
        interactionMatrix[5][0] = -0.3f; // Glu-5-HT: inhibition
        
        for (int i = 1; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                if (i != j) {
                    interactionMatrix[i][j] = (i == j) ? 1.0f : 0.1f + ((i + j) % 3) * 0.2f;
                }
            }
        }
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin neuromodulator initialized (Phase 6 Enhanced)");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin-modulated plasticity factor
    // 5-HT generally enhances cortical plasticity and learning
    float plasticity = 0.7f + pImpl->level * 0.8f;  // Higher 5-HT = more plasticity
    
    // Apply mood and anxiety modulations
    if (pImpl->anxietyModulation < 0.5f) {  // High anxiety
        plasticity *= 0.8f;  // Reduced plasticity under high anxiety
    }
    
    // Apply developmental modulation
    plasticity *= pImpl->developmentalOffset;
    
    // Apply experience modulation
    plasticity *= pImpl->experienceModulation;
    
    return std::clamp(plasticity, 0.2f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
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
    
    NLM_LOG_DEBUG("Serotonin updated: level=" + std::to_string(pImpl->level) + 
                 ", mood=" + std::to_string(pImpl->moodModulation) +
                 ", anxiety=" + std::to_string(pImpl->anxietyModulation));
}

void Serotonin::regulateMood(float moodState) {
    // Mood regulation based on 5-HT activity
    float moodEffect = moodState * pImpl->level * 0.5f;
    pImpl->moodModulation += moodEffect;
    pImpl->moodModulation = std::clamp(pImpl->moodModulation, 0.0f, 2.0f);
    
    NLM_LOG_DEBUG("Serotonin mood regulation: mood_state=" + std::to_string(moodState) + 
                 ", effect=" + std::to_string(moodEffect) +
                 ", modulation=" + std::to_string(pImpl->moodModulation));
}

void Serotonin::modulateAnxiety(float anxietyLevel) {
    // Anxiety modulation based on 5-HT activity
    // 5-HT generally reduces anxiety
    float anxietyReduction = anxietyLevel * pImpl->level * 0.3f;
    pImpl->anxietyModulation -= anxietyReduction;
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
    
    // Update stress level based on anxiety
    pImpl->stressLevel = std::min(pImpl->stressLevel + anxietyLevel * 0.1f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin anxiety modulation: anxiety_level=" + std::to_string(anxietyLevel) + 
                 ", reduction=" + std::to_string(anxietyReduction) +
                 ", stress=" + std::to_string(pImpl->stressLevel));
}

void Serotonin::controlArousal(float arousalTarget) {
    // Arousal control through 5-HT
    float arousalAdjustment = (arousalTarget - pImpl->arousalLevel) * pImpl->level * 0.2f;
    pImpl->arousalLevel += arousalAdjustment;
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel, 0.0f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin arousal control: target=" + std::to_string(arousalTarget) + 
                 ", current=" + std::to_string(pImpl->arousalLevel) +
                 ", adjustment=" + std::to_string(arousalAdjustment));
}

void Serotonin::enhanceSocialBehavior(float socialContext) {
    // Social behavior enhancement
    float socialEnhancement = socialContext * pImpl->level * 0.4f;
    pImpl->socialModulation += socialEnhancement;
    pImpl->socialModulation = std::clamp(pImpl->socialModulation, 0.0f, 2.0f);
    
    // Reduce aggression in social contexts
    if (socialContext > 0.5f) {
        pImpl->emotionalState = std::min(pImpl->emotionalState, 0.8f);
    }
    
    NLM_LOG_DEBUG("Serotonin social behavior enhancement: context=" + std::to_string(socialContext) + 
                 ", enhancement=" + std::to_string(socialEnhancement) +
                 ", modulation=" + std::to_string(pImpl->socialModulation));
}

void Serotonin::integratePredictionError(float predictionError) {
    // Integrate prediction error for learning and adaptation
    pImpl->predictionErrors.push_back(predictionError);
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // 5-HT helps regulate response to prediction errors
    if (predictionError > 0.0f) {
        // Positive prediction error: increase mood and reduce anxiety
        pImpl->moodModulation += 0.1f * std::abs(predictionError);
        pImpl->anxietyModulation -= 0.05f * std::abs(predictionError);
    } else {
        // Negative prediction error: decrease mood and increase anxiety
        pImpl->moodModulation -= 0.1f * std::abs(predictionError);
        pImpl->anxietyModulation += 0.05f * std::abs(predictionError);
    }
    
    pImpl->emotionalState = std::clamp(pImpl->emotionalState, 0.0f, 1.0f);
    pImpl->moodModulation = std::clamp(pImpl->moodModulation, 0.0f, 2.0f);
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
}

void Serotonin::updateDevelopmentalTrajectory(float age) {
    // Developmental changes in serotonin system
    pImpl->maturationStage = age;
    
    // Adjust parameters based on developmental stage
    if (age < 1.0f) {  // Early development
        pImpl->baseline = 0.02f;
        pImpl->peak = 1.5f;
        pImpl->receptorDensity = 1.5f;  // More receptors during development
        pImpl->synthesisRate = 0.06f;
    } else if (age < 10.0f) {  // Childhood/Teenage
        pImpl->baseline = 0.03f;
        pImpl->peak = 1.2f;
        pImpl->receptorDensity = 1.3f;
        pImpl->synthesisRate = 0.05f;
    } else {  // Adult
        pImpl->baseline = 0.03f;
        pImpl->peak = 1.0f;
        pImpl->receptorDensity = 1.2f;
        pImpl->synthesisRate = 0.04f;
    }
}

void Serotonin::encodeEmotionalMemory(float emotion, float strength) {
    // Emotional memory encoding
    pImpl->memoryTrace += strength * (1.0f + std::abs(emotion) * 0.5f);
    pImpl->memoryTrace = std::min(pImpl->memoryTrace, 10.0f);
    
    // Update emotional state based on emotion
    pImpl->emotionalState += emotion * strength * 0.1f;
    pImpl->emotionalState = std::clamp(pImpl->emotionalState, 0.0f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin emotional memory encoding: emotion=" + std::to_string(emotion) + 
                 ", strength=" + std::to_string(strength) +
                 ", memory_trace=" + std::to_string(pImpl->memoryTrace) +
                 ", emotional_state=" + std::to_string(pImpl->emotionalState));
}

void Serotonin::applyStressResponse(float stressLevel, float duration) {
    // Stress response modulation
    pImpl->stressLevel += stressLevel * duration * 0.1f;
    pImpl->stressLevel = std::min(pImpl->stressLevel, 1.0f);
    
    // High stress increases anxiety and decreases mood
    pImpl->anxietyModulation += stressLevel * 0.05f;
    pImpl->moodModulation -= stressLevel * 0.03f;
    
    pImpl->stressLevel = std::clamp(pImpl->stressLevel, 0.0f, 1.0f);
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
    pImpl->moodModulation = std::clamp(pImpl->moodModulation, 0.0f, 2.0f);
    
    NLM_LOG_DEBUG("Serotonin stress response: stress_level=" + std::to_string(stressLevel) + 
                 ", duration=" + std::to_string(duration) +
                 ", stress=" + std::to_string(pImpl->stressLevel) +
                 ", mood=" + std::to_string(pImpl->moodModulation));
}

void Serotonin::resetToBaseline(float recoveryRate, float dt) {
    // Reset to baseline with recovery rate
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Gradual recovery from stress and anxiety
    pImpl->stressLevel *= (1.0f - recoveryRate * dt);
    pImpl->anxietyModulation *= (1.0f - recoveryRate * dt);
    pImpl->moodModulation += (1.0f - pImpl->moodModulation) * recoveryRate * dt;
    
    pImpl->stressLevel = std::clamp(pImpl->stressLevel, 0.0f, 1.0f);
    pImpl->anxietyModulation = std::clamp(pImpl->anxietyModulation, 0.0f, 1.0f);
    pImpl->moodModulation = std::clamp(pImpl->moodModulation, 0.0f, 2.0f);
}

void Serotonin::setEmotionalState(float state) {
    pImpl->emotionalState = std::clamp(state, 0.0f, 1.0f);
}

void Serotonin::updateImpulseControl(float cognitiveLoad) {
    // Impulse control modulation
    float impulseControl = 1.0f - cognitiveLoad * 0.3f;
    pImpl->impulsivityModulation = std::clamp(impulseControl, 0.1f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin impulse control update: cognitive_load=" + std::to_string(cognitiveLoad) + 
                 ", control=" + std::to_string(pImpl->impulsivityModulation));
}

void Serotonin::enhanceSatietyResponse(float foodIntake) {
    // Satiety response enhancement
    float satietyEffect = foodIntake * 0.2f;
    pImpl->rewardSensitivity += satietyEffect * 0.5f;
    pImpl->rewardSensitivity = std::clamp(pImpl->rewardSensitivity, 0.5f, 2.0f);
    
    NLM_LOG_DEBUG("Serotonin satiety response: food_intake=" + std::to_string(foodIntake) + 
                 ", effect=" + std::to_string(satietyEffect) +
                 ", sensitivity=" + std::to_string(pImpl->rewardSensitivity));
}

void Serotonin::modulateAggression(float provocationLevel) {
    // Aggression modulation
    float aggressionModulation = 1.0f - provocationLevel * pImpl->level * 0.4f;
    pImpl->emotionalState *= aggressionModulation;
    pImpl->emotionalState = std::clamp(pImpl->emotionalState, 0.0f, 1.0f);
    
    NLM_LOG_DEBUG("Serotonin aggression modulation: provocation=" + std::to_string(provocationLevel) + 
                 ", modulation=" + std::to_string(aggressionModulation) +
                 ", emotional_state=" + std::to_string(pImpl->emotionalState));
}

void Serotonin::updateNeurogenesis(float neurogenesisRate) {
    // Neurogenesis modulation
    pImpl->experienceModulation += neurogenesisRate * 0.1f;
    pImpl->experienceModulation = std::clamp(pImpl->experienceModulation, 1.0f, 2.0f);
}

float Serotonin::getMoodModulation() const {
    return pImpl->moodModulation;
}

float Serotonin::getAnxietyModulation() const {
    return pImpl->anxietyModulation;
}

float Serotonin::getArousalModulation() const {
    return pImpl->arousalModulation;
}

float Serotonin::getSocialModulation() const {
    return pImpl->socialModulation;
}

} // namespace nlm