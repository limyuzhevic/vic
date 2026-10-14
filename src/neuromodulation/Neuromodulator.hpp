// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced Phase 2 implementation with biological realism

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Enhanced: Biological-based plasticity modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Biological interaction methods
    // Enable integration between neuromodulators
    virtual void signalOther(Neuromodulator* target) = 0;
    virtual void respondToLearning(float predictionError, float reward) = 0;
    virtual void affectNeuralExcitability() = 0;
    virtual void influenceMemoryConsolidation() = 0;
    virtual void modulateCognition() = 0;
    
    // Integration state
    virtual void connectToBrain(class Brain* brain) = 0;
    virtual class Brain* getConnectedBrain() const = 0;
    
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
// Enhanced Phase 2 implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation based on behavioral relevance
    void modulateAttention(float relevance, float novelty);
    
    // Working memory integration
    void encodeToWorkingMemory(float activationPattern);
    void consolidateToEpisodicMemory();
    
    // Novelty and prediction error influence
    void updateNoveltySensitivity(float predictionError);
    void updatePredictionError(float predictionError);
    
    // Learning rate modulation
    void modulateHippocampalLearning(float stability);
    void modulateCorticalLearning(float novelty);
    
    // Integration with attention system
    void updateAttentionalFocus(const std::vector<float>& attentionWeights);
    void setAttentionalPriority(float priority);
    
    // Biological interaction methods
    void signalOther(Neuromodulator* target) override;
    void respondToLearning(float predictionError, float reward) override;
    void affectNeuralExcitability() override;
    void influenceMemoryConsolidation() override;
    void modulateCognition() override;
    
    void connectToBrain(class Brain* brain) override;
    class Brain* getConnectedBrain() const override;

private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced Phase 2 implementation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance modulation
    void modulateArousal(float stimulusStrength);
    void updateVigilance(float importance);
    
    // Attention breadth modulation
    void setAttentionBreadth(float breadth);
    void modulateFocusVsExploration();
    
    // Stress and motivation responses
    void signalStress(float stressLevel);
    void signalMotivation(float motivationLevel);
    
    // Integration with locus coerulean system
    void detectNoveltyForLC(float novelty);
    void updateLocusCoeruleanActivity();
    
    // Curiosity system integration
    void updateCuriosityDrive(float curiosityLevel);
    void modulateExplorationBehavior();
    
    // Biological interaction methods
    void signalOther(Neuromodulator* target) override;
    void respondToLearning(float predictionError, float reward) override;
    void affectNeuralExcitability() override;
    void influenceMemoryConsolidation() override;
    void modulateCognition() override;
    
    void connectToBrain(class Brain* brain) override;
    class Brain* getConnectedBrain() const override;

private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced Phase 2 implementation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and behavioral regulation
    void updateMood(float socialValue, float stressLevel);
    void regulateBehavior(float approachLevel);
    
    // Reward prediction error integration (inverted relationship)
    void integrateWithRPE(float rewardPredictionError);
    void updatePredictionErrorResponse(float error);
    
    // Impulsivity and decision making modulation
    void modulateImpulsivity(float impulsivityLevel);
    void updateDecisionThresholds(float threshold);
    
    // Social behavior and bonding
    void updateSocialBonding(float socialCohesion);
    void modulateTrustResponse(float trustLevel);
    
    // Novelty detection for approach/avoidance
    void updateApproachAvoidanceBias(float novelty, float threatLevel);
    void integrateWithNoveltySystem(float noveltyLevel);
    
    // Biological interaction methods
    void signalOther(Neuromodulator* target) override;
    void respondToLearning(float predictionError, float reward) override;
    void affectNeuralExcitability() override;
    void influenceMemoryConsolidation() override;
    void modulateCognition() override;
    
    void connectToBrain(class Brain* brain) override;
    class Brain* getConnectedBrain() const override;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
