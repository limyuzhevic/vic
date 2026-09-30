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

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified - improved for performance)
    size_t coincidences = 0;
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Create sorted copies to avoid modifying originals
    std::vector<Timestamp> sortedPre(preSpikes.begin(), preSpikes.end());
    std::vector<Timestamp> sortedPost(postSpikes.begin(), postSpikes.end());
    
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    // Use two-pointer approach for O(N+M) complexity
    size_t i = 0, j = 0;
    while (i < sortedPre.size() && j < sortedPost.size()) {
        Timestamp diff = std::abs(sortedPre[i] - sortedPost[j]);
        
        if (diff < 10.0) {  // 10ms coincidence window
            ++coincidences;
            ++i;
            ++j;
        } else if (sortedPre[i] < sortedPost[j]) {
            // Pre spike is earlier, try next post spike
            ++j;
        } else {
            // Post spike is earlier, try next pre spike  
            ++i;
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
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
