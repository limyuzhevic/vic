namespace nlm {

// PlasticityRule constructor
PlasticityRule::PlasticityRule() : pImpl(new Impl) {}

// PlasticityRule destructor
PlasticityRule::~PlasticityRule() = default;

// PlasticityRule move constructor
PlasticityRule::PlasticityRule(PlasticityRule&&) noexcept = default;

// PlasticityRule move assignment operator
PlasticityRule& PlasticityRule::operator=(PlasticityRule&&) noexcept = default;

void PlasticityRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate base weight change
    float baseChange = calculateWeightChange(synapse, preSpikes, postSpikes, dt);
    
    // Apply learning rate
    float scaledChange = baseChange * pImpl->learningRate;
    
    // Apply adaptation factor to prevent runaway changes
    if (pImpl->adaptationFactor > 0.0f) {
        scaledChange = scaledChange * pImpl->adaptationFactor;
    }
    
    // Only apply if change exceeds threshold
    if (std::abs(scaledChange) >= pImpl->weightChangeThreshold) {
        applyWeightChange(synapse, scaledChange);
    }
}

void PlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Clamp to bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if there's a meaningful change
    if (std::abs(newWeight - currentWeight) >= 1e-6f) {
        synapse->setWeight(newWeight);
    }
}

void PlasticityRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float PlasticityRule::getLearningRate() const {
    return pImpl->learningRate;
}

void PlasticityRule::setWeightBounds(float minWeight, float maxWeight) {
    pImpl->minWeight = std::min(minWeight, maxWeight);
    pImpl->maxWeight = std::max(minWeight, maxWeight);
}

void PlasticityRule::setWeightChangeThreshold(float threshold) {
    pImpl->weightChangeThreshold = std::max(threshold, 0.0f);
}

void PlasticityRule::setAdaptationFactor(float factor) {
    pImpl->adaptationFactor = std::clamp(factor, 0.0f, 1.0f);
}

float PlasticityRule::calculateCovariance(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) const {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Convert spike times to rates (spikes per ms)
    float preRate = static_cast<float>(preSpikes.size()) / 100.0f;  // over 100ms window
    float postRate = static_cast<float>(postSpikes.size()) / 100.0f;
    
    // Calculate covariance of spike patterns
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window
            if (std::abs(dt) < 100.0f) {  // 100ms window
                correlationCount++;
            }
        }
    }
    
    float expectedCorrelation = preRate * postRate * 100.0f;
    float actualCorrelation = static_cast<float>(correlationCount);
    
    return actualCorrelation - expectedCorrelation;
}

float PlasticityRule::calculatePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes,
                                           float ltpWeight, float ltdWeight,
                                           float timeConstant) const {
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = ltpWeight * std::exp(-dt / timeConstant);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -ltdWeight * std::exp(dt / timeConstant);
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    return totalDelta;
}

float PlasticityRule::decayEligibilityTrace(float currentTrace, TimestepDuration dt,
                                             float decayRate) const {
    if (decayRate <= 0.0f) return currentTrace;
    
    float timeInMs = dt * 1000.0f;  // Convert to ms
    float decayFactor = std::exp(-timeInMs / decayRate);
    
    return currentTrace * decayFactor;
}

float PlasticityRule::calculateBaseFiringRate(const std::vector<Timestamp>& spikeHistory,
                                               Timestamp currentTime,
                                               float timeWindow) const {
    if (spikeHistory.empty()) {
        return 0.0f;
    }
    
    // Count spikes in the time window
    size_t spikeCount = 0;
    for (Timestamp spikeTime : spikeHistory) {
        float timeDiff = static_cast<float>(currentTime - spikeTime);
        if (timeDiff >= 0.0f && timeDiff <= timeWindow) {
            spikeCount++;
        }
    }
    
    return static_cast<float>(spikeCount) / (timeWindow / 1000.0f);  // spikes per second
}

void PlasticityRule::updateEligibilityTrace(Synapse* synapse, float rewardPredictionError,
                                            TimestepDuration dt) {
    if (!synapse) return;
    
    float currentTrace = synapse->getEligibilityTrace();
    
    // Decay existing trace
    currentTrace = decayEligibilityTrace(currentTrace, dt, 100.0f);  // 100ms decay
    
    // Add new contribution from reward prediction error
    currentTrace += rewardPredictionError;
    
    // Clamp eligibility trace to prevent runaway values
    currentTrace = std::clamp(currentTrace, -10.0f, 10.0f);
    
    synapse->setEligibilityTrace(currentTrace);
}

std::string PlasticityRule::getSummaryStats(const std::vector<Timestamp>& spikeHistory,
                                            Timestamp currentTime) const {
    if (spikeHistory.empty()) {
        return "No spike activity recorded";
    }
    
    // Calculate basic statistics
    size_t spikeCount = spikeHistory.size();
    float maxTimeSinceLastSpike = 0.0f;
    
    for (Timestamp spikeTime : spikeHistory) {
        float timeSinceSpike = static_cast<float>(currentTime - spikeTime);
        maxTimeSinceLastSpike = std::max(maxTimeSinceLastSpike, timeSinceSpike);
    }
    
    std::ostringstream oss;
    oss << "Spike statistics: " << spikeCount << " spikes, "
        << "average interval: " << (spikeCount > 1 ? 
            (static_cast<float>(currentTime - spikeHistory.front()) / (spikeCount - 1)) : 0.0f) 
        << "ms, "
        << "last spike: " << maxTimeSinceLastSpike << "ms ago";
    
    return oss.str();
}

} // namespace nlm
