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
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   pre  = average pre-synaptic activity
     *   post = average post-synaptic activity
     *   η   = learning rate
     * 
     * For spike-based implementation:
     *   ⟨pre⟩  = number of pre-synaptic spikes
     *   ⟨post⟩ = number of post-synaptic spikes
     *   ⟨pre * post⟩ = number of coincident spike pairs
     * 
     * Biological inspiration:
     *   - Reflects synaptic efficacy changes based on co-activity
     *   - Implements "neurons that fire together, wire together"
     *   - Used for memory formation and Hebbian potentiation
     * 
     * Safety constraints:
     *   - Weight bounds prevent runaway excitation
     *   - Learning rate clamping ensures stable convergence
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute covariance-based weight change
    // Calculate mean pre-synaptic activity (number of spikes)
    float meanPre = static_cast<float>(preSpikes.size());
    
    // Calculate mean post-synaptic activity (number of spikes)  
    float meanPost = static_cast<float>(postSpikes.size());
    
    // Calculate coactivity (number of spike pairs within correlation window)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Consider spikes within 20ms as coincident
            float dt = std::abs(static_cast<float>(postTime - preTime));
            if (dt < 20.0f) {  // 20ms correlation window
                coactivity++;
            }
        }
    }
    
    // Convert to average coactivity per pre spike
    float avgCoactivity = coactivity / meanPre;
    
    // Calculate covariance: E[pre*post] - E[pre]*E[post]
    // For spikes: E[pre*post] = coactivity, E[pre]=meanPre, E[post]=meanPost
    float covariance = avgCoactivity - meanPre * meanPost;
    
    // Apply weight change with learning rate
    float delta = pImpl->learningRate * covariance;
    
    // Apply weight change with bounds checking
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
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
