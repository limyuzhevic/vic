// Real implementation of Neuromodulator - Dopamine
Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // This implements the dopamine modulation of synaptic plasticity
    // According to RL theory, dopamine signals prediction error which modulates learning rate
    
    float basePlasticity = 0.01f;  // Base learning rate
    
    if (pImpl->level < 0.1f) {
        // Low dopamine - suppressed learning
        return 0.0f;
    } else if (pImpl->level < 0.5f) {
        // Moderate dopamine - normal learning
        return basePlasticity * (1.0f + pImpl->level);
    } else {
        // High dopamine - enhanced learning (reward prediction)
        return basePlasticity * (2.0f - pImpl->level);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Implements temporal difference in dopamine signaling
    // with decay towards baseline and activity-dependent release
    
    float timeStep = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * timeStep);
    
    // Add small noise for biological realism
    if (static_cast<unsigned int>(timeStep * 1000.0f) % 100 == 0) {
        pImpl->level += (rand() % 100 - 50) * 0.001f;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling
    // Implements reward prediction error (RPE) coding
    // Dopamine bursts for unexpected rewards
    
    if (reward > 0.0f) {
        // Strong burst for unexpected positive reward
        float burstMagnitude = std::min(1.0f, reward * pImpl->releaseRate * 2.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
        
        // Encode reward prediction error
        float predictionError = reward - pImpl->level * 0.5f;  // Simple prediction
        if (predictionError > 0.0f) {
            pImpl->level = std::min(pImpl->peak, 
                                   pImpl->level + predictionError * pImpl->releaseRate);
        }
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Implements temporal difference learning
    // Phasic dopamine encodes prediction errors
    
    if (std::abs(error) > 0.01f) {  // Only respond to significant errors
        if (error > 0.0f) {
            // Positive error - reward prediction exceeded expectation
            // Phasic burst
            float errorStrength = std::min(1.0f, error * pImpl->releaseRate);
            pImpl->level = std::min(pImpl->peak, pImpl->level + errorStrength);
        } else {
            // Negative error - reward prediction exceeded actual reward
            // Phasic dip
            float dipMagnitude = std::min(0.5f, std::abs(error) * pImpl->releaseRate);
            pImpl->level = std::max(pImpl->baseline, pImpl->level - dipMagnitude);
        }
    }
}