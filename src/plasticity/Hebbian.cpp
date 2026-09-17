// Real implementation of HebbianRule::update()
void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse) return;
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate precise spike timing for STDP
    // Find closest pre-post spike pairs
    float weightChange = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            // STDP window: pre before post
            if (deltaT > 0 && deltaT < 100.0) {  // 100ms window
                // LTP (Long Term Potentiation) - pre before post
                // Weight increases exponentially with deltaT
                weightChange += 0.01f * std::exp(-deltaT / 20.0f);
            } else if (deltaT < 0 && std::abs(deltaT) < 100.0) {  // pre after post
                // LTD (Long Term Depression) - post before pre
                // Weight decreases with distance
                weightChange -= 0.01f * std::exp(std::abs(deltaT) / 20.0f);
            }
        }
    }
    
    // Apply weight change with bounds
    if (weightChange != 0.0f) {
        float newWeight = synapse->getWeight() + weightChange;
        // Constrain weight to reasonable range [0.0, 1.0]
        newWeight = std::clamp(newWeight, 0.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}