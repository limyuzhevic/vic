// Enhanced helper functions for advanced Hebbian learning

float Hebbian::calculateSpikeCorrelation(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Calculate temporal correlation with enhanced window
    float correlation = 0.0f;
    float maxCorrelation = 0.0f;
    
    for (size_t i = 0; i < std::min(preSpikes.size(), postSpikes.size()); ++i) {
        float timeDiff = (postSpikes[i] - preSpikes[i]) * 1000.0f; // Convert to ms
        
        if (std::abs(timeDiff) < 50.0f) { // 50ms correlation window
            float weight = std::exp(-std::abs(timeDiff) / 10.0f); // Gaussian temporal kernel
            correlation += weight;
            maxCorrelation += weight;
        }
    }
    
    if (maxCorrelation > 0) {
        correlation /= maxCorrelation;
    }
    
    return correlation;
}

float Hebbian::calculateHomeostaticFactor(float weight) {
    // Homeostatic scaling: weaker synapses learn faster
    return 1.0f / (1.0f + std::abs(weight));
}

float Hebbian::calculateVoltageFactor(float voltage) {
    // Voltage-dependent modulation
    if (std::abs(voltage) < 30.0f) {
        return 0.5f; // Low voltage = less learning
    } else if (std::abs(voltage) > 80.0f) {
        return 1.5f; // High voltage = more learning
    }
    return 1.0f;
}

float Hebbian::calculateRewardModulation(float trace) {
    // Reward modulation with temporal dynamics
    return 1.0f + std::tanh(trace * 2.0f) * 0.5f; // Range [1.0, 1.5]
}

float Hebbian::calculateTripletInfluence(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Triplet influence: consider interaction with previous spike patterns
    if (preSpikes.size() < 2 || postSpikes.size() < 2) {
        return 1.0f;
    }
    
    // Calculate temporal asymmetry
    float preInterval = (preSpikes.back() - preSpikes[preSpikes.size() - 2]);
    float postInterval = (postSpikes.back() - postSpikes[postSpikes.size() - 2]);
    
    float influence = 1.0f;
    if (preInterval > 0 && postInterval < 0) {
        influence *= 1.5f; // Facilitatory interaction
    } else if (preInterval < 0 && postInterval > 0) {
        influence *= 1.2f; // Depressory interaction
    }
    
    return influence;
}

float Hebbian::calculateHomeostaticScaling(float weight, float correlation) {
    // Prevent runaway excitation while maintaining pattern separation
    if (correlation > 0.8f) { // High correlation
        return 1.0f - std::abs(weight) * 0.3f;
    } else { // Low correlation
        return 1.0f + correlation * 0.5f;
    }
}

float Hebbian::calculateInhibitoryConstraint(float weight) {
    // Constraints for inhibitory plasticity
    if (weight > 0) {
        return 0.0f; // Strong inhibition penalty
    }
    return 1.0f + std::abs(weight) * 0.2f; // Weaker inhibition gets stronger
}

float Hebbian::calculateModulatoryConstraint(float weight) {
    // Constraints for modulatory plasticity
    return 1.0f + std::tanh(weight * 0.5f); // Dynamic scaling
}

float Hebbian::calculateModulatoryFactor(float weight) {
    // Modulatory factor based on current weight
    return 1.0f + std::sin(weight * 10.0f) * 0.3f; // Oscillatory modulation
}

const char* Hebbian::getName() const {
    return "Enhanced Hebbian";
}

void Hebbian::setLearningRate(float rate, bool adaptive) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    // If adaptive learning is enabled, we'll use different dynamics
    // This is a placeholder for future enhancement
}

void Hebbian::setLearningParameters(float learningRate, float covThreshold, 
                                    bool enableTriplet, bool enableVoltageDep) {
    setLearningRate(learningRate);
    pImpl->covarianceThreshold = covThreshold;
    // These parameters would control the enhanced learning behaviors
    // This is a placeholder for future enhancement
}

py::dict Hebbian::getLearningParameters() const {
    py::dict params;
    params["learning_rate"] = pImpl->learningRate;
    params["max_weight"] = pImpl->maxWeight;
    params["min_weight"] = pImpl->minWeight;
    params["covariance_threshold"] = pImpl->covarianceThreshold;
    
    // Add enhanced parameters
    params["triplet_enabled"] = true; // Placeholder for future enhancement
    params["voltage_dependent"] = true;
    params["reward_modulated"] = true;
    params["homeostatic"] = true;
    
    return params;
}

void Hebbian::updateLearningParameters(const py::dict& params) {
    if (params.contains("learning_rate")) {
        setLearningRate(params["learning_rate"].cast<float>());
    }
    if (params.contains("covariance_threshold")) {
        pImpl->covarianceThreshold = params["covariance_threshold"].cast<float>();
    }
    // Additional parameters would be set here for future enhancements
}
