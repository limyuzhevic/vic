#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

// Neuromodulator interaction types
enum class NeuromodulatorInteraction {
    EXCITATORY,
    INHIBITORY,
    MODULATORY,
    FEEDBACK,
    FEEDFORWARD
};

// Neuromodulator state for adaptive behavior
struct NeuromodulatorState {
    float level;                    // Current concentration (0.0 to 1.0)
    float baseline;                  // Baseline level
    float peak;                      // Peak achievable level
    float decayRate;                 // Natural decay rate
    float releaseRate;               // Release rate on stimulation
    float targetLevel;               // Target level for adaptive behavior
    float adaptationRate;            // Rate of adaptation to target
    float memory;                    // Short-term memory of level
    float learningRate;              // Learning rate parameter (Serotonin)
    float attentionMod;              // Attention modulation factor (Acetylcholine)
    float motivationMod;             // Motivation modulation factor (Endorphins)
    std::vector<float> history;     // Recent level history for adaptive behavior
    std::map<std::string, float> crossModulatorInfluence; // Influence from other neuromodulators
};

// Neuromodulator: Abstract base for neuromodulatory signals with advanced features
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator type
    virtual const char* getType() const = 0;
    
    // Get neuromodulator strength
    virtual float getStrength() const = 0;
    virtual void setStrength(float strength) = 0;
    
    // Check if neuromodulator is active
    virtual bool isActive() const = 0;
    
    // Activate/deactivate neuromodulator
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    
    // Adaptive behavior control
    virtual void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) = 0;
    
    // Add interaction with other neuromodulators
    virtual void addInteraction(const std::string& targetModulator, 
                              NeuromodulatorInteraction interactionType, 
                              float influenceStrength) = 0;
    
    // Get cross-modulator influence
    virtual float getCrossModulatorInfluence(const std::string& modulatorName) const = 0;
    
    // Process external signals
    virtual void processSignal(float signal, const std::string& signalType) = 0;
    
    // Get current state for advanced users
    virtual const NeuromodulatorState& getState() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal with advanced features
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    const char* getType() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getStrength() const override;
    void setStrength(float strength) override;
    bool isActive() const override;
    void activate() override;
    void deactivate() override;
    void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) override;
    void addInteraction(const std::string& targetModulator, 
                       NeuromodulatorInteraction interactionType, 
                       float influenceStrength) override;
    float getCrossModulatorInfluence(const std::string& modulatorName) const override;
    void processSignal(float signal, const std::string& signalType) override;
    const NeuromodulatorState& getState() const override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Advanced dopamine functions
    void setBaseline(float baseline);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setPeak(float peak);
    void setLearningProgress(float progress);
    float getLearningProgress() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation with advanced features
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    const char* getType() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getStrength() const override;
    void setStrength(float strength) override;
    bool isActive() const override;
    void activate() override;
    void deactivate() override;
    void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) override;
    void addInteraction(const std::string& targetModulator, 
                       NeuromodulatorInteraction interactionType, 
                       float influenceStrength) override;
    float getCrossModulatorInfluence(const std::string& modulatorName) const override;
    void processSignal(float signal, const std::string& signalType) override;
    const NeuromodulatorState& getState() const override;
    
    // Advanced acetylcholine functions
    void setAttentionGain(float gain);
    void setMemoryConsolidation(float consolidation);
    void setAttentionalFocus(float focus);
    float getAttentionDemand() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior with learning rate modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    const char* getType() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getStrength() const override;
    void setStrength(float strength) override;
    bool isActive() const override;
    void activate() override;
    void deactivate() override;
    void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) override;
    void addInteraction(const std::string& targetModulator, 
                       NeuromodulatorInteraction interactionType, 
                       float influenceStrength) override;
    float getCrossModulatorInfluence(const std::string& modulatorName) const override;
    void processSignal(float signal, const std::string& signalType) override;
    const NeuromodulatorState& getState() const override;
    
    // Advanced serotonin functions
    void setLearningRate(float rate);
    void setMoodModulation(float mood);
    void setImpulsivityControl(float control);
    void setSocialBehavior(float behavior);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Endorphins: Motivation and pain modulation
class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override;
    const char* getType() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getStrength() const override;
    void setStrength(float strength) override;
    bool isActive() const override;
    void activate() override;
    void deactivate() override;
    void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) override;
    void addInteraction(const std::string& targetModulator, 
                       NeuromodulatorInteraction interactionType, 
                       float influenceStrength) override;
    float getCrossModulatorInfluence(const std::string& modulatorName) const override;
    void processSignal(float signal, const std::string& signalType) override;
    const NeuromodulatorState& getState() const override;
    
    // Advanced endorphin functions
    void setMotivationLevel(float motivation);
    void setPainModulation(float pain);
    void setRewardFeeling(float feeling);
    void setSatisfaction(float satisfaction);
    float getMotivationLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    const char* getType() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getStrength() const override;
    void setStrength(float strength) override;
    bool isActive() const override;
    void activate() override;
    void deactivate() override;
    void adaptToSystemState(float learningProgress, float attentionDemand, float motivationLevel) override;
    void addInteraction(const std::string& targetModulator, 
                       NeuromodulatorInteraction interactionType, 
                       float influenceStrength) override;
    float getCrossModulatorInfluence(const std::string& modulatorName) const override;
    void processSignal(float signal, const std::string& signalType) override;
    const NeuromodulatorState& getState() const override;
    
    // Advanced norepinephrine functions
    void setArousalLevel(float arousal);
    void setVigilance(float vigilance);
    void setStressResponse(float stress);
    float getArousalLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
