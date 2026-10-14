#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides biological neuromodulation for learning, attention, and behavior

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
    
    // Initialize with brain reference
    virtual void initialize(Brain* brain) {}
    
    // Get attention modulation
    virtual float getAttentionModulation() const { return 1.0f; }
    
    // Get memory modulation
    virtual float getMemoryModulation() const { return 1.0f; }
    
    // Get arousal modulation
    virtual float getArousalModulation() const { return 1.0f; }
    
    // Get behavioral modulation
    virtual float getBehavioralModulation() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error and action selection
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get attention modulation
    float getAttentionModulation() const override;
    
    // Get memory modulation
    float getMemoryModulation() const override;
    
    // Get behavioral modulation
    float getBehavioralModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements attention gating and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Focus attention on specific sensory inputs
    void focusAttention(const std::vector<float>& features);
    
    // Consolidate working memory to long-term storage
    void consolidateMemory();
    
    // Get attention modulation
    float getAttentionModulation() const override;
    
    // Get memory modulation
    float getMemoryModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements alertness, arousal, and stress responses
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Signal environmental novelty
    void signalNovelty(const std::vector<float>& input);
    
    // Increase arousal for salient stimuli
    void increaseArousal(float salience);
    
    // Get arousal modulation
    float getArousalModulation() const override;
    
    // Get attention modulation
    float getAttentionModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements mood regulation, impulsivity control, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Regulate mood based on social feedback
    void regulateMood(bool socialPositive, float socialValue);
    
    // Control impulsivity
    void controlImpulsivity(bool hasWaitOption);
    
    // Get behavioral modulation
    float getBehavioralModulation() const override;
    
    // Get attention modulation
    float getAttentionModulation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
