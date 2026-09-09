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
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate covariance-based Hebbian learning: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    const double LEARNING_RATE = pImpl->learningRate;
    const double COINCIDENCE_WINDOW = 10.0;  // ms
    
    // Calculate coactivity: number of spike pairs within coincidence window
    // Efficient O(n+m) algorithm using two pointers (better than O(n²) double loop)
    size_t coactivity = 0;
    size_t preIdx = 0, postIdx = 0;
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    while (preIdx < preCount && postIdx < postCount) {
        double timeDiff = preSpikes[preIdx] - postSpikes[postIdx];
        if (std::abs(timeDiff) < COINCIDENCE_WINDOW) {
            coactivity++;
            preIdx++;
            postIdx++;
        } else if (timeDiff < 0) {
            preIdx++;
        } else {
            postIdx++;
        }
    }
    
    // Calculate baseline: η * mean(activity) - short-term memory of recent activity
    double baseline = 0.0;
    const std::vector<Timestamp>& preHistory = synapse->getPreSpikeHistory();
    const std::vector<Timestamp>& postHistory = synapse->getPostSpikeHistory();
    
    // Incorporate short-term memory through recent spike history
    if (!preHistory.empty() && !postHistory.empty()) {
        // Simple activity measure: spike count over recent window
        double preActivity = static_cast<double>(preHistory.size()) / COINCIDENCE_WINDOW;
        double postActivity = static_cast<double>(postHistory.size()) / COINCIDENCE_WINDOW;
        baseline = LEARNING_RATE * preActivity * postActivity;
    }
    
    // Apply covariance rule: Δw = η * (coactivity - baseline)
    double weightChange = LEARNING_RATE * static_cast<double>(coactivity - baseline);
    
    // Apply short-term memory through eligibility trace
    float eligibilityTrace = synapse->getEligibilityTrace();
    float decayFactor = static_cast<float>(std::exp(-static_cast<double>(dt) * 1000.0));  // ms^-1
    eligibilityTrace = eligibilityTrace * decayFactor + static_cast<float>(weightChange);
    synapse->setEligibilityTrace(eligibilityTrace);
    
    // Normalize weight change using synaptic efficacy to prevent runaway potentiation
    float effectiveChange = eligibilityTrace * synapse->getEfficacy();
    
    // Apply magnitude clamping for stability - prevents runaway potentiation
    const float MAX_CHANGE_PER_STEP = 0.1f;
    if (effectiveChange > MAX_CHANGE_PER_STEP) {
        effectiveChange = MAX_CHANGE_PER_STEP;
    } else if (effectiveChange < -MAX_CHANGE_PER_STEP) {
        effectiveChange = -MAX_CHANGE_PER_STEP;
    }
    
    // Apply the weight change using synapse's built-in clamping mechanism
    applyWeightChange(synapse, static_cast<SynapticWeight>(effectiveChange));
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
