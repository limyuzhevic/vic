#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
    
protected:
    Neuromodulator() = default;
};

// Forward declaration for Brain class
class Brain;

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory signaling
    void initialize(Brain* brain);
    void signalAttention(float attentionSignal);
    void signalMemoryRehearsal();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance signaling
    void initialize(Brain* brain);
    void signalStress(float stressLevel);
    void signalNovelty(float novelty);
    float getArousal() const;
    float getVigilance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social behavior signaling
    void initialize(Brain* brain);
    void signalReward(float reward);
    void signalSocialInteraction(float socialValue);
    void signalPunishment(float punishment);
    float getMood() const;
    float getImpulsivity() const;
    float getSocialBehavior() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty: Novelty detection and curiosity driving
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection
    void initialize(Brain* brain);
    void detectNovelty(const Observation& observation,
                      const Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern,
                      const std::vector<float>& previousPattern);
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward prediction error signal
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Prediction error computation
    void initialize(Brain* brain);
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    const std::vector<float>& getHistory() const;
    void clearHistory();
    float getMagnitude() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward signal
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward computation
    void add(float delta);
    void reset();
    float computeReward(const Observation& observation) const;
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity system
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity and exploration drive
    void initialize(Brain* brain);
    void update(float novelty, float predictionError, TimestepDuration dt);
    float getExplorationDrive() const;
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm