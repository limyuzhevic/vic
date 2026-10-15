std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                               float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Apply ACh modulation to attention
    // ACh enhances attentional gain and lowers competition thresholds
    float cholinergicModulation = 1.0f;
    if (brain_ && brain_->getAcetylcholine()) {
        cholinergicModulation = 1.0f + brain_->getAcetylcholine()->getAttentionGain() * 2.0f;
        
        // Modify competition threshold based on ACh level
        // High ACh = lower threshold = more neurons become winners
        float originalThreshold = competitionThreshold_ * totalActivity / competitors.size();
        float modulatedThreshold = originalThreshold / cholinergicModulation;
        
        // Apply modulated threshold to winner selection
        for (size_t i = 0; i < competitors.size(); ++i) {
            if (activities[i] >= modulatedThreshold) {
                winners_.push_back(competitors[i]);
                
                // Enhanced excitation for winners when ACh is high
                float enhancedExcitation = excitationStrength_ * cholinergicModulation;
                if (brain_) {
                    brain_->injectCurrent(competitors[i], enhancedExcitation * 2.0f);
                }
            }
        }
    } else {
        // Apply original threshold logic if no ACh
        float threshold = competitionThreshold_ * totalActivity / competitors.size();
        
        for (size_t i = 0; i < competitors.size(); ++i) {
            if (activities[i] >= threshold) {
                winners_.push_back(competitors[i]);
                
                // Apply excitation to winners
                if (brain_) {
                    brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
                }
            }
        }
    }
    
    return winners_;
}