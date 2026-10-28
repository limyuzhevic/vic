#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(std::make_unique<Impl>()) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning with covariance rule
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZATION: For efficient coincidence detection, use a sliding window
    // Approach: sort spike times and use two-pointer technique
    size_t coincidences = 0;
    
    // If we have fewer spikes in one vector, iterate over that one
    const std::vector<Timestamp>* inner = &preSpikes;
    const std::vector<Timestamp>* outer = &postSpikes;
    
    if (preSpikes.size() > postSpikes.size()) {
        std::swap(inner, outer);
    }
    
    // OPTIMIZATION: Sort outer vector for binary search
    // This reduces the inner loop from O(n*m) to O(n log m)
    std::vector<Timestamp> sortedOuter(*outer);
    std::sort(sortedOuter.begin(), sortedOuter.end());
    
    // For each spike in inner vector, use binary search to find matching spikes
    for (Timestamp spikeTime : *inner) {
        // Use binary search to find spikes within coincidence window
        auto lower = std::lower_bound(sortedOuter.begin(), sortedOuter.end(), 
                                      spikeTime - 10.0f);  // 10ms window
        auto upper = std::upper_bound(sortedOuter.begin(), sortedOuter.end(), 
                                      spikeTime + 10.0f);
        
        coincidences += std::distance(lower, upper);
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;  // Null pointer check
    }
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
