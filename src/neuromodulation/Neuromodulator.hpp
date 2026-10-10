// Neuromodulation: Abstract base for neuromodulatory signals
// Enhanced Phase 2 implementation with real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns multiplier for learning rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Initialize with brain reference (optional for some neuromodulators)
    virtual void initialize(class Brain* brain) {}
    
    // Get effect type for categorization
    virtual const char* getEffectType() const { return "Base"; }
    
protected:
    Neuromodulator() = default;
};

// Enhanced Dopamine: Reward and reinforcement learning signal
// Real implementation with burst dynamics and plasticity modulation
class Dopamine {
public:
    Dopamine() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.1f), 
                 releaseRate_(1.0f), timeConstant_(5.0f), history_() {}
    ~Dopamine() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Dopamine system initialized - reward modulation ready");
    }
    
    // Get current dopamine level
    float getLevel() const { return level_; }
    
    // Set dopamine level with bounds checking
    void setLevel(float level) {
        level_ = std::clamp(level, 0.0f, peak_);
    }
    
    // Apply reward signal (burst of dopamine)
    void signalReward(float reward) {
        if (reward > 0.0f) {
            // Calcium spike - burst of dopamine
            float burst = std::min(reward * releaseRate_ * 2.0f, peak_ - level_);
            level_ += burst;
            history_.push_back(level_);
            
            NLM_LOG_INFO(std::string("Dopamine burst: +") + std::to_string(burst) +
                        " (reward=" + std::to_string(reward) + ")");
        }
    }
    
    // Apply reward prediction error (dopamine's core function)
    void signalRewardPredictionError(float error) {
        // Positive error = larger than expected reward = dopamine increase
        // Negative error = smaller than expected = dopamine decrease
        float change = error * releaseRate_ * 0.5f;
        float newLevel = std::max(0.0f, level_ + change);
        
        if (std::abs(newLevel - level_) > 0.01f) {
            level_ = newLevel;
            history_.push_back(level_);
            
            NLM_LOG_INFO(std::string("Dopamine RPE: ") + (error > 0.0f ? "+" : "") +
                        std::to_string(change) + " (error=" + std::to_string(error) + ")");
        }
    }
    
    // Get plasticity factor (dopamine modulates learning)
    float getPlasticityFactor() const {
        // Inverted U-shaped relationship: moderate dopamine optimizes learning
        // Too little = under-arousal, too much = over-arousal
        float normalized = level_ / peak_;
        return 0.5f + 0.8f * std::exp(-4.0f * std::pow(normalized - 0.5f, 2.0f));
    }
    
    // Update dopamine dynamics (decay towards baseline)
    void update(float dt) {
        // Natural decay
        float decay = decayRate_ * dt;
        level_ = std::max(baseline_, level_ - decay);
        
        // Slow baseline adaptation
        if (baseline_ < 0.3f && level_ > 0.3f) {
            baseline_ = std::min(0.3f, baseline_ + 0.01f * dt);
        }
    }
    
    // Get level history
    const std::vector<float>& getHistory() const { return history_; }
    void clearHistory() { history_.clear(); }
    
    // Set baseline for longer-term adaptation
    void setBaseline(float baseline) { baseline_ = std::clamp(baseline, 0.0f, 1.0f); }
    
private:
    float level_;        // Current dopamine concentration
    float baseline_;     // Long-term average level
    float peak_;         // Maximum possible level
    float decayRate_;    // Decay rate back to baseline
    float releaseRate_;  // Rate of dopamine release
    float timeConstant_; // Time constant for dynamics
    std::vector<float> history_;
    class Brain* brain_;
};

// Enhanced Acetylcholine: Attention and memory consolidation
// Real implementation with attentional effects
class Acetylcholine {
public:
    Acetylcholine() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.05f),
                      attentionalGain_(1.0f), memoryBoost_(1.0f), arousalLevel_(0.5f) {}
    ~Acetylcholine() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Acetylcholine system initialized - attentional modulation ready");
    }
    
    // Get acetylcholine level
    float getLevel() const { return level_; }
    
    // Set acetylcholine level
    void setLevel(float level) {
        level_ = std::clamp(level, 0.0f, peak_);
        updateAttentionalGain();
    }
    
    // Apply attentional signal
    void signalAttention(float salience) {
        // ACh responds to novel/stimulating input
        float boost = salience * 0.8f;
        level_ += boost;
        
        if (level_ > peak_) level_ = peak_;
        history_.push_back(level_);
        
        NLM_LOG_INFO(std::string("Acetylcholine attentional burst: +") + std::to_string(boost) +
                    " (salience=" + std::to_string(salience) + ")");
    }
    
    // Apply memory consolidation signal
    void signalMemoryConsolidation(float strength) {
        // ACh promotes memory consolidation during learning
        level_ *= (1.0f + strength * 0.3f);
        if (level_ > peak_) level_ = peak_;
        history_.push_back(level_);
        
        // Boost memory systems
        memoryBoost_ = std::min(memoryBoost_, 1.0f + strength * 0.5f);
    }
    
    // Get attentional gain factor for sensory processing
    float getAttentionalGain() const { return attentionalGain_; }
    
    // Get memory consolidation factor
    float getMemoryBoost() const { return memoryBoost_; }
    
    // Get plasticity factor (ACh enhances LTP)
    float getPlasticityFactor() const {
        // ACh generally enhances plasticity, especially LTP
        return 1.0f + 0.5f * level_;
    }
    
    // Update attentional dynamics
    void update(float dt) {
        // Decay back to baseline
        level_ = std::max(baseline_, level_ - decayRate_ * dt);
        updateAttentionalGain();
        
        // Slow memory boost decay
        if (memoryBoost_ > 1.0f) {
            memoryBoost_ = std::max(1.0f, memoryBoost_ - 0.01f * dt);
        }
    }
    
    // Update attentional gain based on current ACh level
    void updateAttentionalGain() {
        // Inverted U-shaped relationship: moderate ACh optimizes attention
        float normalized = level_ / peak_;
        attentionalGain_ = 1.0f + 1.5f * std::exp(-4.0f * std::pow(normalized - 0.6f, 2.0f));
    }
    
    // Apply arousal modulation
    void signalArousal(float arousal) {
        arousalLevel_ = std::clamp(arousal, 0.0f, 1.0f);
        // Arousal modulates ACh release
        if (arousalLevel_ > 0.7f) {
            level_ = std::min(peak_, level_ + 0.1f);
        }
    }
    
    // Get current arousal level
    float getArousalLevel() const { return arousalLevel_; }
    
private:
    float level_;            // Current acetylcholine concentration
    float baseline_;         // Baseline level
    float peak_;             // Maximum level
    float decayRate_;        // Decay rate
    float attentionalGain_;  // Factor for sensory processing gain
    float memoryBoost_;      // Factor for memory consolidation
    float arousalLevel_;     // Overall arousal state
    std::vector<float> history_;
    class Brain* brain_;
};

// Enhanced Norepinephrine: Arousal and vigilance
// Real implementation with stress and focus effects
class Norepinephrine {
public:
    Norepinephrine() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.08f),
                       arousalLevel_(0.5f), stressLevel_(0.0f), focusGain_(1.0f) {}
    ~Norepinephrine() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Norepinephrine system initialized - arousal modulation ready");
    }
    
    // Get norepinephrine level
    float getLevel() const { return level_; }
    
    // Set norepinephrine level
    void setLevel(float level) {
        level_ = std::clamp(level, 0.0f, peak_);
        updateArousalState();
    }
    
    // Apply stress signal
    void signalStress(float stress) {
        // NE responds to stress with increased arousal
        stressLevel_ = std::clamp(stress, 0.0f, 1.0f);
        
        float response = stressLevel_ * 0.6f;
        level_ += response;
        
        if (level_ > peak_) level_ = peak_;
        history_.push_back(level_);
        
        NLM_LOG_INFO(std::string("Norepinephrine stress response: +") + std::to_string(response) +
                    " (stress=" + std::to_string(stress) + ")");
    }
    
    // Apply focus signal
    void signalFocus(float focus) {
        // NE enhances focus and selective attention
        float focusBoost = focus * 0.4f;
        level_ += focusBoost;
        
        if (level_ > peak_) level_ = peak_;
        history_.push_back(level_);
        
        // Increase focus gain
        focusGain_ = std::min(focusGain_, 1.0f + focus * 0.5f);
        
        NLM_LOG_INFO(std::string("Norepinephrine focus boost: +") + std::to_string(focusBoost) +
                    " (focus=" + std::to_string(focus) + ")");
    }
    
    // Get current arousal level
    float getArousalLevel() const { return arousalLevel_; }
    
    // Get focus gain factor
    float getFocusGain() const { return focusGain_; }
    
    // Get plasticity factor (NE modulates learning)
    float getPlasticityFactor() const {
        // NE has complex effects: enhances some forms of learning, impairs others
        // Typically enhances alertness and working memory
        if (stressLevel_ > 0.5f) {
            // High stress: impaired learning
            return 0.5f * (1.0f + level_ * 0.5f);
        } else {
            // Moderate arousal: enhanced learning
            float normalized = level_ / peak_;
            return 0.8f + 0.5f * normalized;
        }
    }
    
    // Update arousal dynamics
    void update(float dt) {
        // Natural decay towards baseline
        level_ = std::max(baseline_, level_ - decayRate_ * dt);
        updateArousalState();
        
        // Stress decays more slowly
        if (stressLevel_ > 0.0f) {
            stressLevel_ = std::max(0.0f, stressLevel_ - 0.005f * dt);
        }
        
        // Focus gain decays
        if (focusGain_ > 1.0f) {
            focusGain_ = std::max(1.0f, focusGain_ - 0.01f * dt);
        }
    }
    
    // Update arousal state
    void updateArousalState() {
        // Arousal is a function of current NE level
        float normalized = level_ / peak_;
        arousalLevel_ = 0.3f + 0.7f * normalized;
    }
    
    // Get stress level
    float getStressLevel() const { return stressLevel_; }
    
    // Apply habituation (repeated exposure reduces response)
    void habituate(float amount) {
        if (amount > 0.0f) {
            stressLevel_ = std::max(0.0f, stressLevel_ - amount * 0.5f);
            NLM_LOG_INFO("Norepinephrine habituation: stress reduced by " + std::to_string(amount * 0.5f));
        }
    }
    
private:
    float level_;            // Current norepinephrine concentration
    float baseline_;         // Baseline level
    float peak_;             // Maximum level
    float decayRate_;        // Decay rate
    float arousalLevel_;     // Current arousal state
    float stressLevel_;      // Current stress level
    float focusGain_;        // Factor for attentional focus
    std::vector<float> history_;
    class Brain* brain_;
};

// Enhanced Serotonin: Mood, impulsivity, and social behavior
// Real implementation with mood and behavioral effects
class Serotonin {
public:
    Serotonin() : level_(0.0f), baseline_(0.0f), peak_(1.0f), decayRate_(0.06f),
                  moodLevel_(0.5f), impulsivity_(0.5f), socialBonding_(0.5f) {}
    ~Serotonin() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Serotonin system initialized - mood modulation ready");
    }
    
    // Get serotonin level
    float getLevel() const { return level_; }
    
    // Set serotonin level
    void setLevel(float level) {
        level_ = std::clamp(level, 0.0f, peak_);
        updateMoodState();
    }
    
    // Apply positive mood signal
    void signalPositiveMood(float mood) {
        // Serotonin responds to positive experiences
        if (mood > 0.0f) {
            float boost = mood * 0.5f;
            level_ += boost;
            
            if (level_ > peak_) level_ = peak_;
            history_.push_back(level_);
            
            NLM_LOG_INFO(std::string("Serotonin mood boost: +") + std::to_string(boost) +
                        " (mood=" + std::to_string(mood) + ")");
        }
    }
    
    // Apply social bonding signal
    void signalSocialBonding(float bonding) {
        // Serotonin enhances social behavior and bonding
        float bondingBoost = bonding * 0.4f;
        level_ += bondingBoost;
        
        if (level_ > peak_) level_ = peak_;
        history_.push_back(level_);
        
        // Increase social bonding factor
        socialBonding_ = std::min(socialBonding_, 1.0f + bonding * 0.3f);
        
        NLM_LOG_INFO(std::string("Serotonin social bonding: +") + std::to_string(bondingBoost) +
                    " (bonding=" + std::to_string(bonding) + ")");
    }
    
    // Get current mood level
    float getMoodLevel() const { return moodLevel_; }
    
    // Get impulsivity factor (serotonin reduces impulsivity)
    float getImpulsivityFactor() const {
        // Normalized relationship: higher serotonin = lower impulsivity
        return 1.0f - 0.8f * (level_ / peak_);
    }
    
    // Get social bonding factor
    float getSocialBondingFactor() const { return socialBonding_; }
    
    // Get plasticity factor (serotonin modulates different types of learning)
    float getPlasticityFactor() const {
        // Serotonin generally promotes stable, long-term changes
        // Reduces catastrophic interference
        float normalized = level_ / peak_;
        
        if (normalized < 0.3f) {
            // Low serotonin: unstable learning
            return 0.6f + 0.3f * normalized;
        } else {
            // Moderate to high serotonin: stable, long-term learning
            return 0.9f + 0.1f * std::exp(-2.0f * (normalized - 0.7f));
        }
    }
    
    // Update mood dynamics
    void update(float dt) {
        // Natural decay towards baseline
        level_ = std::max(baseline_, level_ - decayRate_ * dt);
        updateMoodState();
        
        // Social bonding decays
        if (socialBonding_ > 1.0f) {
            socialBonding_ = std::max(1.0f, socialBonding_ - 0.02f * dt);
        }
    }
    
    // Update mood state
    void updateMoodState() {
        // Mood is influenced by serotonin levels
        float normalized = level_ / peak_;
        
        if (normalized < 0.2f) {
            moodLevel_ = 0.2f + 0.6f * normalized;  // Low mood
        } else if (normalized < 0.8f) {
            moodLevel_ = 0.5f + 0.5f * normalized;  // Neutral/moderate mood
        } else {
            moodLevel_ = 0.9f + 0.1f * (normalized - 0.8f);  // High mood
        }
    }
    
    // Apply negative experience (reduces serotonin)
    void signalNegativeExperience(float severity) {
        float reduction = severity * 0.3f;
        level_ = std::max(0.0f, level_ - reduction);
        history_.push_back(level_);
        
        NLM_LOG_INFO(std::string("Serotonin depletion: -") + std::to_string(reduction) +
                    " (severity=" + std::to_string(severity) + ")");
    }
    
    // Get resiliency factor (ability to recover from negative experiences)
    float getResiliencyFactor() const {
        // Higher baseline serotonin = better resilience
        return 0.5f + 0.5f * baseline_;
    }
    
private:
    float level_;              // Current serotonin concentration
    float baseline_;           // Baseline level
    float peak_;               // Maximum level
    float decayRate_;          // Decay rate
    float moodLevel_;          // Current mood state
    float impulsivity_;        // Current impulsivity level
    float socialBonding_;      // Current social bonding factor
    std::vector<float> history_;
    class Brain* brain_;
};

// Novelty detection system (already exists but enhanced)
class Novelty {
public:
    Novelty() : level_(0.0f), peak_(1.0f), decayRate_(0.05f), memoryWindow_(100),
                detectionThreshold_(0.3f), surpriseMagnitude_(0.5f) {}
    ~Novelty() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Novelty system initialized - pattern change detection ready");
    }
    
    // Get novelty level
    float getLevel() const { return level_; }
    
    // Set novelty level
    void setLevel(float level) {
        level_ = std::clamp(level, 0.0f, peak_);
    }
    
    // Detect novelty from observation change
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern) {
        if (currentPattern.size() != previousPattern.size() || currentPattern.empty()) {
            return;
        }
        
        // Calculate pattern distance
        float distance = 0.0f;
        for (size_t i = 0; i < currentPattern.size(); ++i) {
            float diff = currentPattern[i] - previousPattern[i];
            distance += diff * diff;
        }
        distance = std::sqrt(distance);
        
        // Normalize distance to 0-1 range
        float novelty = std::min(distance / detectionThreshold_, 1.0f) * surpriseMagnitude_;
        
        level_ = novelty;
        history_.push_back(level_);
        
        NLM_LOG_INFO(std::string("Novelty detected: ") + std::to_string(novelty) +
                    " (pattern change=") + std::to_string(distance) + ")");
    }
    
    // Decay novelty over time
    void update(float dt) {
        level_ = std::max(0.0f, level_ - decayRate_ * dt);
    }
    
    // Get novelty level
    float getLevel() const { return level_; }
    
    // Get history
    const std::vector<float>& getHistory() const { return history_; }
    void clearHistory() { history_.clear(); }
    
private:
    float level_;               // Current novelty level
    float peak_;                // Maximum level
    float decayRate_;           // Decay rate
    size_t memoryWindow_;       // Number of patterns to remember
    float detectionThreshold_;  // Threshold for novelty detection
    float surpriseMagnitude_;   // Magnitude of novelty response
    std::vector<float> history_;
    class Brain* brain_;
};

// Prediction error system (enhanced)
class PredictionError {
public:
    PredictionError() : error_(0.0f), confidence_(0.5f), learningRate_(0.1f),
                       predictionHistory_(), errorHistory_() {}
    ~PredictionError() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Prediction error system initialized - predictive coding ready");
    }
    
    // Get prediction error value
    float getError() const { return error_; }
    
    // Get prediction confidence
    float getConfidence() const { return confidence_; }
    
    // Compute prediction error
    void computeError(float predicted, float actual) {
        float newError = actual - predicted;  // Simple prediction error
        
        // Apply learning rate for error adaptation
        float learningFactor = learningRate_ * confidence_;
        float adaptedError = error_ + learningFactor * (newError - error_);
        
        error_ = adaptedError;
        errorHistory_.push_back(std::abs(error_));
        
        NLM_LOG_INFO(std::string("Prediction error: ") + (error_ > 0.0f ? "+" : "") +
                    std::to_string(error_) + " (pred=" + std::to_string(predicted) +
                    ", actual=" + std::to_string(actual) + ")");
    }
    
    // Update prediction
    void updatePrediction(float newPrediction) {
        predictionHistory_.push_back(newPrediction);
        
        // Simple predictive update based on current error
        if (!predictionHistory_.empty()) {
            float avgPrediction = 0.0f;
            for (float pred : predictionHistory_) {
                avgPrediction += pred;
            }
            avgPrediction /= predictionHistory_.size();
            
            // Adapt prediction based on error
            float predictionChange = learningRate_ * error_ * (1.0f - confidence_);
            float updatedPrediction = avgPrediction + predictionChange;
            
            // Update current error based on improved prediction
            error_ = (newPrediction - updatedPrediction) * 0.5f;
        }
    }
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const { return std::abs(error_); }
    
    // Update prediction system dynamics
    void update(float dt) {
        // Confidence naturally decays towards baseline
        confidence_ = std::max(0.1f, confidence_ - 0.05f * dt);
        
        // Error can strengthen or weaken predictions
        if (std::abs(error_) > 0.1f) {
            // Large errors increase confidence temporarily
            confidence_ = std::min(1.0f, confidence_ + 0.1f * dt);
        }
    }
    
    // Get error history
    const std::vector<float>& getErrorHistory() const { return errorHistory_; }
    void clearHistory() { errorHistory_.clear(); }
    
private:
    float error_;                    // Current prediction error
    float confidence_;               // Confidence in predictions
    float learningRate_;             // Learning rate for predictions
    std::vector<float> predictionHistory_;
    std::vector<float> errorHistory_;
    class Brain* brain_;
};

// Curiosity system (enhanced)
class Curiosity {
public:
    Curiosity() : level_(0.0f), peak_(1.0f), decayRate_(0.03f),
                  noveltyWeight_(0.6f), errorWeight_(0.4f), explorationDrive_(0.5f) {}
    ~Curiosity() = default;
    
    void initialize(class Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("Curiosity system initialized - exploration drive ready");
    }
    
    // Get curiosity level
    float getLevel() const { return level_; }
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, float dt) {
        // Curiosity is weighted combination of novelty and prediction error
        float target = noveltyWeight_ * novelty + errorWeight_ * predictionError;
        
        // Smooth update (exponential moving average)
        float learningRate = 0.1f;
        float delta = target - level_;
        level_ += learningRate * delta * dt;
        
        // Decay
        level_ = std::max(0.0f, level_ - decayRate_ * dt);
        
        // Update exploration drive
        explorationDrive_ = level_;
        
        NLM_LOG_INFO(std::string("Curiosity update: ") + std::to_string(level_) +
                    " (novelty=" + std::to_string(novelty) +
                    ", error=" + std::to_string(predictionError) + ")");
    }
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const { return explorationDrive_; }
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight) { noveltyWeight_ = std::clamp(weight, 0.0f, 1.0f); }
    void setPredictionErrorWeight(float weight) { errorWeight_ = std::clamp(weight, 0.0f, 1.0f); }
    
    // Reset
    void reset() {
        level_ = 0.0f;
        explorationDrive_ = 0.0f;
    }
    
private:
    float level_;              // Current curiosity level
    float peak_;               // Maximum level
    float decayRate_;          // Decay rate
    float noveltyWeight_;      // Weight for novelty in curiosity
    float errorWeight_;        // Weight for prediction error in curiosity
    float explorationDrive_;   // Exploration drive factor
    class Brain* brain_;
};