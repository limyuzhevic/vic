#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// Full implementation with biological realism

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Norepinephrine-specific functions
    
    // Enhance signal detection in neural circuits
    void enhanceSignalDetection(const std::vector<float>& neuralActivity,
                               const std::vector<float>& noiseLevels,
                               std::vector<float>& signalToNoiseRatios);
    
    // Generate phasic response to novel, surprising, or threatening stimuli
    void generatePhasicResponse(const std::vector<float>& stimulusFeatures,
                               const class Novelty& novelty,
                               const class PredictionError& predictionError,
                               const std::vector<float>& threatLevel);
    
    // Enhance arousal based on environmental demands
    void enhanceArousal(float environmentalDemand, float stressorLevel, TimestepDuration dt);
    
    // Boost vigilance during uncertain or threatening situations
    void enhanceVigilance(const std::vector<float>& sensoryInput,
                        const std::vector<float>& threatSignals,
                        std::vector<float>& vigilanceIndices);
    
    // Prioritize behaviorally relevant information
    void prioritizeInformation(const std::vector<float>& informationSalience,
                               const std::vector<float>& behavioralRelevance,
                               std::vector<float>& priorityWeights);
    
    // Boost working memory encoding and retrieval
    void enhanceWorkingMemory(const std::vector<float>& memoryItems,
                           std::vector<float>& enhancedEncoding,
                           std::vector<float>& enhancedRetrieval);
    
    // Apply tonic baseline level
    void applyTonicLevel(float baselineLevel);
    
    // Decay between phasic events
    void applyDecay(float decayRate, TimestepDuration dt);
    
    // Recovery mechanisms after stress responses
    void recoverFromStress(float recoveryRate, TimestepDuration dt);
    
    // Adaptation to chronic stress levels
    void adaptToStress(const std::vector<float>& stressHistory, TimestepDuration dt);
    
    // Integrate with other neuromodulatory systems
    
    // Work with acetylcholine for attention-arousal balance
    void integrateWithAcetylcholine(const class Acetylcholine& acetylcholine,
                                    float& attentionArousalBalance);
    
    // Work with dopamine for salience-reward integration
    void integrateWithDopamine(const class Dopamine& dopamine,
                               float& salienceRewardIntegration);
    
    // Work with novelty for threat detection
    void integrateWithNovelty(const class Novelty& novelty,
                              std::vector<float>& noveltyThreatAssociation);
    
    // Influence working memory encoding and retrieval
    void integrateWithWorkingMemory(class NeuralWorkingMemory* workingMemory,
                                    const std::vector<float>& memoryRequests);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Full implementation with biological realism

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Serotonin-specific functions
    
    // Compute mood state based on reward and threat inputs
    void computeMoodState(float rewardInput, float threatInput);
    
    // Generate behavioral inhibition signal
    void generateBehavioralInhibition(float currentBehaviorStrength, 
                                       std::vector<float>& inhibitionSignals);
    
    // Modulate social behavior based on interaction history
    void modulateSocialBehavior(float socialCohesion, float statusConflict);
    
    // Regulate sleep-wake cycle through circadian rhythm integration
    void regulateSleepWakeCycle(float timeOfDay, float arousalLevel);
    
    // Modulate reward sensitivity
    void modulateRewardSensitivity(float predictionError, float predictionConfidence);
    
    // Enhance cognitive flexibility
    void enhanceCognitiveFlexibility(const std::vector<float>& currentStrategy, 
                                    std::vector<float>& strategyAlternatives);
    
    // Modulate pain sensitivity
    void modulatePainSensitivity(float nociceptiveInput, float anxietyLevel);
    
    // Generate phasic response to social and reward events
    void generatePhasicResponse(const class Reward& reward,
                               const class Novelty& novelty,
                               const class PredictionError& predictionError,
                               const std::vector<float>& socialInput);
    
    // Apply state-dependent modulation of other neuromodulators
    void applyStateDependentModulation(class Dopamine& dopamine,
                                       class Norepinephrine& norepinephrine,
                                       class Acetylcholine& acetylcholine,
                                       class Novelty& novelty);
    
    // Homeostatic regulation to maintain balance
    void homeostaticRegulation(const std::vector<float>& serotoninHistory,
                               float targetLevel);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
