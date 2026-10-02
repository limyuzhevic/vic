// Updated Dopamine::Impl with real neuromodulation dynamics
struct Dopamine::Impl {
    float level;           // Current dopamine level
    float baseline;        // Baseline dopamine level
    float peak;            // Peak dopamine level
    float decayRate;       // Rate of decay towards baseline
    float releaseRate;     // Rate of dopamine release on events
    float adaptationTime;  // Time constant for adaptation
    float predictionError; // Reward prediction error for this step
    
    // Track recent events for burst detection
    std::vector<float> recentRewards;
    float maxRecentRewards; // Maximum recent rewards for burst detection
    
    Impl() 
        : level(0.1f)  // Start with low baseline activity
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)  // Slower decay for longer-lasting effects
        , releaseRate(2.0f)  // Stronger response to rewards
        , adaptationTime(100.0f)  // Slower adaptation (100ms)
        , predictionError(0.0f)
        , maxRecentRewards(0.0f) {}
};

// Initialize with low baseline activity
Dopamine::Dopamine() : pImpl(new Impl) {
    // Biological constraint: dopamine levels don't start at zero
    // There's always some tonic dopamine release
    pImpl->level = pImpl->baseline + 0.05f;  // 5% above baseline
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on biological research:
    // - Dopamine modulates synaptic strength through D1 and D2 receptors
    // - D1 receptors (in cortex): enhance LTP, strengthen synapses
    // - D2 receptors (in striatum): enhance LTD, weaken synapses  
    // - Net effect depends on concentration: low levels promote exploration,
    //   high levels promote exploitation
    
    // Low dopamine (<0.3): promotes synaptic weakening (LTD)
    if (pImpl->level < 0.3f) {
        return 0.3f + pImpl->level * 0.5f;  // Range: 0.3 to 0.8
    }
    // Medium dopamine (0.3-0.7): balanced plasticity
    else if (pImpl->level < 0.7f) {
        return 0.8f;  // Stable plasticity
    }
    // High dopamine (>0.7): promotes synaptic strengthening (LTP)
    else {
        return 0.8f + (pImpl->level - 0.7f) * 2.0f;  // Range: 0.8 to 2.0
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on biological neuron models:
    // Model: dDA/dt = (DA_target - DA_current) / tau + input
    
    float targetLevel = pImpl->baseline + pImpl->predictionError * 0.5f;
    
    // Smooth approach to target level with time constant
    float adaptationRate = 1.0f / pImpl->adaptationTime;
    pImpl->level += (targetLevel - pImpl->level) * adaptationRate * static_cast<float>(dt);
    
    // Decay towards baseline (tonic release)
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on dopamine neuron responses:
    // - Unexpected large rewards cause burst firing
    // - Predictable rewards cause phasic but sustained release
    
    // Track recent rewards for burst detection
    pImpl->recentRewards.push_back(reward);
    pImpl->maxRecentRewards = std::max(pImpl->maxRecentRewards, reward);
    
    // Keep only last 10 rewards
    if (pImpl->recentRewards.size() > 10) {
        pImpl->recentRewards.erase(pImpl->recentRewards.begin());
    }
    
    // Compute reward prediction error if available
    float error = pImpl->predictionError;
    
    // Phasic dopamine response: larger for unexpected rewards
    float rewardStrength = reward * 0.5f;  // Scaling factor
    
    // Add prediction error component
    if (error != 0.0f) {
        rewardStrength += std::abs(error) * 0.3f;
    }
    
    // Response saturation: prevent unlimited growth
    float response = rewardStrength * pImpl->releaseRate;
    
    // Check for burst condition: large, unexpected reward
    if (reward > 0.5f && error > 0.2f) {
        // Burst response: stronger and longer-lasting
        pImpl->level = std::min(pImpl->peak, pImpl->level + response * 1.5f);
        pImpl->decayRate *= 0.5f;  // Slower decay after burst
    } else {
        // Normal phasic response
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
    }
    
    // Reset prediction error
    pImpl->predictionError = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling (Rescorla-Wagner learning):
    // - Positive error (better than expected): increases dopamine
    // - Negative error (worse than expected): decreases dopamine
    // - Error magnitude determines response strength
    
    pImpl->predictionError = error;
    
    // Apply error with biological constraints:
    // - Error is bounded (-1.0 to +1.0)
    // - Response is linear with error magnitude
    // - Different time courses for positive vs negative errors
    
    if (error > 0) {
        // Positive error: reward prediction error (learning is good)
        float errorResponse = error * pImpl->releaseRate * 0.8f;  // Slightly dampened
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorResponse);
        
        // Positive errors have longer-lasting effects
        pImpl->decayRate *= 0.7f;
    } else {
        // Negative error: prediction error (learning is bad)
        float errorResponse = std::abs(error) * pImpl->releaseRate * 0.6f;
        pImpl->level = std::max(0.0f, pImpl->level - errorResponse);
        
        // Negative errors have faster recovery
        pImpl->decayRate *= 1.2f;
    }
}

// Get recent reward history for analysis
const std::vector<float>& Dopamine::getRecentRewards() const {
    return pImpl->recentRewards;
}

// Check if dopamine level indicates burst firing
bool Dopamine::isBurst() const {
    return pImpl->level > 0.8f && pImpl->maxRecentRewards > 0.5f;
}

// Get current prediction error
float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

// Set prediction error (for external control)
void Dopamine::setPredictionError(float error) {
    pImpl->predictionError = error;
}

// Get adaptation time constant
float Dopamine::getAdaptationTime() const {
    return pImpl->adaptationTime;
}

// Set adaptation time constant
void Dopamine::setAdaptationTime(float time) {
    pImpl->adaptationTime = std::max(1.0f, time);
}

} // namespace nlm