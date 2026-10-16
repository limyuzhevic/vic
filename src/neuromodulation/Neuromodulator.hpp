#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides interface for neuromodulatory effects on neural processing
// Includes methods for level control, plasticity modulation, and state updates
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name (e.g., "Dopamine", "Curiosity")
    virtual const char* getName() const = 0;
    
    // Get current concentration/level of neuromodulator
    // Returns value between 0.0 (inactive) and 1.0+ (strongly active)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor that scales plasticity rates (1.0 = normal, >1.0 = enhanced, <1.0 = reduced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on simulation time
    // Implements dynamic changes in neuromodulator concentration over time
    virtual void update(TimestepDuration dt) = 0;
    
    // Get modulation effects on neural excitability
    // Some neuromodulators increase or decrease neural excitability
    virtual float getExcitabilityModifier() const { return 1.0f; }
    
    // Check if neuromodulator is currently active
    // Returns true if neuromodulator has meaningful concentration (>0.001)
    virtual bool isActive() const { return getLevel() > 0.001f; }
    
    // Get time remaining until neuromodulator effects decay
    // Returns negative value if not applicable
    virtual float getTimeToDecay() const { return -1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error and reinforcement learning mechanisms
// Essential for reward-based learning and motivation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling methods
    // These methods update internal reward prediction error signals
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get current reward prediction
    float getExpectedReward() const;
    
    // Set expected reward for prediction
    void setExpectedReward(float reward);
    
    // Get reward learning rate
    float getLearningRate() const;
    void setLearningRate(float rate);
    
    // Reset reward signals to baseline
    void resetRewardSignals();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Placeholder implementation with full documentation for Phase 2
// ACh is crucial for attention, learning, and memory processes
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Attention modulation functions (Phase 2)
    // float getAttentionLevel() const; // Current attention strength
    // void setAttentionLevel(float level); // Control attention
    // float getFocusingBias() const; // Bias toward specific regions
    // void setFocusingBias(float bias); // Set focusing bias
    // 
    // // Memory consolidation effects
    // float getMemoryConsolidationFactor() const; // Effect on memory formation
    // void setMemoryConsolidationFactor(float factor); // Set consolidation strength
};

// Norepinephrine: Arousal and vigilance
// Placeholder implementation with full documentation for Phase 2
// NE regulates arousal, alertness, and stress responses
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Arousal modulation functions (Phase 2)
    // float getArousalLevel() const; // Current arousal state
    // void setArousalLevel(float level); // Control arousal
    // float getVigilanceLevel() const; // Alertness/focus level
    // void setVigilanceLevel(float level); // Set vigilance
    // 
    // // Stress and response effects
    // float getStressLevel() const; // Internal stress state
    // void setStressLevel(float level); // Set stress level
};

// Serotonin: Mood, impulsivity, and social behavior
// Placeholder implementation with full documentation for Phase 2
// 5-HT regulates mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Mood and behavioral modulation functions (Phase 2)
    // float getMoodLevel() const; // Current mood state
    // void setMoodLevel(float level); // Control mood
    // float getImpulsivityLevel() const; // Tendency toward impulsive actions
    // void setImpulsivityLevel(float level); // Set impulsivity
    // 
    // // Social behavior effects
    // float getSocialCohesion() const; // Tendency toward social behavior
    // void setSocialCohesion(float cohesion); // Set social cohesion
};

// Novelty: Novelty detection and processing
// Placeholder implementation for Phase 2
// Detects novel stimuli and drives exploration of new information
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection methods
    float getNoveltyScore(const std::vector<float>& input) const;
    void setNoveltyThreshold(float threshold);
    float getNoveltyThreshold() const;
    
    // Learning from novelty
    void incorporateNovelty(const std::vector<float>& input);
    float getNoveltyLearningRate() const;
    void setNoveltyLearningRate(float rate);
    
    // Reset novelty signals
    void resetNoveltySignals();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation
// Placeholder implementation for Phase 2
// Drives exploration behavior and reduces exploitation bias
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Exploration control
    float getExplorationLevel() const;
    void setExplorationLevel(float level);
    
    // Motivation and drive
    float getMotivationLevel() const;
    void setMotivationLevel(float level);
    
    // Action selection influence
    float getActionBias() const;
    void setActionBias(float bias);
    
    // Reset curiosity drives
    void resetCuriosityDrives();
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError: Prediction error signal
// Placeholder implementation for Phase 2
// Computes prediction errors for learning and adaptation
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Error computation
    float getError(const std::vector<float>& prediction,
                   const std::vector<float>& target) const;
    
    // Learning from error
    void incorporateError(float error);
    float getErrorLearningRate() const;
    void setErrorLearningRate(float rate);
    
    // Reset error signals
    void resetErrorSignals();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
