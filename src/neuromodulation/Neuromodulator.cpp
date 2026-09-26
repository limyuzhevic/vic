Dopamine::Dopamine() : pImpl(new Impl) {}

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
    // Phase 2: Implement real dopamine-modulated plasticity factor
    // Based on biological evidence, dopamine modulates STDP timing
    // Higher dopamine facilitates LTP, lower dopamine facilitates LTD
    if (pImpl->level < 0.3f) {
        // Low dopamine - enhances LTD, reduces LTP
        return 0.3f + 0.4f * pImpl->level;  // Range: 0.3 to 0.7
    } else {
        // High dopamine - enhances LTP, reduces LTD
        return 0.7f + 0.3f * pImpl->level;  // Range: 0.7 to 1.0
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Implement real dopamine dynamics
    // Biological model: dopamine decays exponentially to baseline
    // with some noise and synaptic input
    float dt_float = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * (pImpl->decayRate * dt_float);
    
    // Add noise for realism
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f * pImpl->level;
    pImpl->level += noise;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Implement real reward signaling
    // Biological model: reward prediction error (RPE) triggers dopamine burst
    // RPE = reward - predicted_reward
    // This updates synaptic strength for learning
    
    // Scale reward to realistic dopamine response
    float rewardMagnitude = std::clamp(reward, 0.0f, 1.0f);
    
    // Synaptic modulation based on reward
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + rewardMagnitude * pImpl->releaseRate * 2.0f);
    
    // Also update baseline for future prediction
    pImpl->baseline = std::max(pImpl->baseline, pImpl->level * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Implement reward prediction error signaling
    // Biological model: dopamine neurons encode RPE
    // Positive RPE (better than expected) -> burst
    // Negative RPE (worse than expected) -> dip
    
    // Apply prediction error with asymmetric response
    float predictionError = std::clamp(error, -1.0f, 1.0f);
    
    if (predictionError > 0.0f) {
        // Positive prediction error: burst response
        pImpl->level = std::min(pImpl->peak, 
                               pImpl->level + predictionError * pImpl->releaseRate * 1.5f);
    } else {
        // Negative prediction error: dip response
        float dip = std::abs(predictionError) * pImpl->decayRate;
        pImpl->level = std::max(0.0f, pImpl->level - dip);
    }
}

} // namespace nlm
