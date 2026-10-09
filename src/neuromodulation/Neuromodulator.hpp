#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for neuromodulatory effects
class Brain;
class Synapse;

// Neuromodulator: Abstract base for neuromodulatory signals
// Modern biological implementation with phasic/tonic release and receptor dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (nM - nanomolar)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns modulation factor (1.0 = baseline plasticity, >1.0 = enhanced, <1.0 = suppressed)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on biological dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Get decision uncertainty modulation (0.0 = certain, 1.0 = highly uncertain)
    virtual float getDecisionUncertainty() const = 0;
    
    // Get exploration probability modulation (0.0 = exploitation only, 1.0 = pure exploration)
    virtual float getExplorationBias() const = 0;
    
    // Get learning rate modulation
    virtual float getLearningRateModulation() const = 0;
    
    // Get reward prediction error signal
    virtual float getRewardPredictionError() const = 0;
    
    // Get novelty signal
    virtual float getNoveltySignal() const = 0;
    
    // Get attentional focus modulation
    virtual float getAttentionalFocus() const = 0;
    
    // Get arousal/activation level
    virtual float getArousal() const = 0;
    
    // Signal external event (reward, punishment, novelty, etc.)
    virtual void signalEvent(float value, EventType type) = 0;
    
    // Apply neuromodulatory effect to synapse
    virtual void applyToSynapse(Synapse* synapse) = 0;
    
    // Compute state-dependent neuromodulation
    virtual void updateStateDependent(Brain* brain) = 0;
    
protected:
    Neuromodulator() = default;
};

// Neuromodulator event types for signaling
enum class EventType {
    Reward,                    // Positive outcome
    Punishment,                // Negative outcome  
    Novelty,                   // Unpredictable stimulus
    PredictionError,           // Mismatch between prediction and reality
    Curiosity,                 // Exploratory drive
    Attention,                 // Focus on relevant information
    Stress,                    // Threat or challenge
    Arousal,                   // General activation level
    MemoryConsolidation,       // Memory processing
    Decision,                  // Choice point
    ActionSelection            // Action selection
};

// Receptor dynamics for neuromodulatory signaling
struct ReceptorDynamics {
    float concentration;        // Current neurotransmitter concentration (nM)
    float boundFraction;        // Fraction of receptors bound (0-1)
    float affinity;            // Receptor affinity (Kd - dissociation constant)
    float density;             // Receptor density (receptors per membrane area)
    float couplingEfficiency;   // Efficiency of GPCR coupling to intracellular pathways
    float desensitizationRate;  // Rate of receptor desensitization
    
    ReceptorDynamics() 
        : concentration(0.0f), boundFraction(0.0f), affinity(10.0f), density(1.0f),
          couplingEfficiency(1.0f), desensitizationRate(0.01f) {}
    
    // Update receptor binding based on neurotransmitter concentration
    void updateBinding(float dt, float newConcentration) {
        const float rate = 1.0f / affinity;
        boundFraction = newConcentration / (newConcentration + rate);
        boundFraction = std::clamp(boundFraction, 0.0f, 1.0f);
    }
    
    // Apply desensitization
    void applyDesensitization(float dt) {
        boundFraction *= (1.0f - desensitizationRate * static_cast<float>(dt));
    }
};

// Dopamine: Reward and reinforcement learning signal
// Realistic implementation with phasic bursts, tonic baseline, and prediction error
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
    
    // Get tonic and phasic components
    float getTonicLevel() const;
    float getPhasicBurst() const;
    
    // Configure dopamine parameters
    void setBaseline(float baseline);
    void setPeakLevel(float peak);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setReceptorParameters(float affinity, float density);
    
    // Get receptor state
    const ReceptorDynamics& getReceptorState() const;
    
    // Dopamine-specific plasticity modulation
    float getRewardModulatedPlasticity() const;
    float getLearningRateEnhancement() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, learning rate modulation, and novelty signaling
// Realistic implementation with tonic and phasic release
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin signaling
    void signalNovelty(float novelty);
    void signalMoodState(float mood);
    void signalLearningRateAdjustment(float adjustment);
    
    // Get serotonin components
    float getTonicSerotonin() const;
    float getPhasicSerotonin() const;
    
    // Learning rate modulation
    float getLearningRateModulation() const override;
    void setLearningRateBase(float baseRate);
    void setLearningRateVariance(float variance);
    
    // Novelty detection modulation
    void setNoveltySensitivity(float sensitivity);
    float getNoveltySensitivity() const;
    
    // Get receptor state
    const ReceptorDynamics& getReceptorState() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Realistic implementation with attentional modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttentionalFocus(float focusLevel);
    void signalWorkingMemoryLoad(float load);
    void signalSensoryGain(float gain);
    
    // Get cholinergic components
    float getTonicACh() const;
    float getPhasicACh() const;
    
    // Attentional modulation
    float getAttentionalFocus() const override;
    float getSensoryDetectionThreshold() const;
    void setAttentionThreshold(float threshold);
    
    // Memory consolidation
    float getMemoryConsolidationBoost() const;
    void setMemoryConsolidationRate(float rate);
    
    // Get receptor state
    const ReceptorDynamics& getReceptorState() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and stress response
// Realistic implementation with arousal and stress modulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    void signalStressResponse(float stressLevel);
    void signal Vigilance(float vigilance);
    
    // Get norepinephrine components
    float getTonicNE() const;
    float getPhasicNE() const;
    
    // Arousal modulation
    float getArousal() const override;
    float getReactionTimeModulation() const;
    void setReactionTimeFactor(float factor);
    
    // Stress response
    float getStressResponseLevel() const;
    float getFightOrFlightActivation() const;
    void setStressSensitivity(float sensitivity);
    
    // Get receptor state
    const ReceptorDynamics& getReceptorState() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulator ensemble for integrated control
// Combines multiple neuromodulators for comprehensive behavioral regulation
class NeuromodulatorEnsemble {
public:
    NeuromodulatorEnsemble();
    ~NeuromodulatorEnsemble();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update ensemble state
    void update(TimestepDuration dt);
    
    // Signal event to all neuromodulators
    void signalEvent(EventType type, float value);
    
    // Apply neuromodulatory effects to synapse
    void applyToSynapse(Synapse* synapse);
    
    // Get combined neuromodulatory effects
    float getCombinedPlasticityFactor() const;
    float getExplorationProbability() const;
    float getDecisionUncertainty() const;
    
    // Get individual neuromodulator levels
    float getDopamineLevel() const;
    float getSerotoninLevel() const;
    float getAChLevel() const;
    float getNELevel() const;
    
    // Get neuromodulator ensemble statistics
    float getRewardPredictionError() const;
    float getNoveltySignal() const;
    float getAttentionFocus() const;
    float getArousalLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
