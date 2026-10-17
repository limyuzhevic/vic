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

    /**
     * @brief Compute weight changes based on correlated firing
     * 
     * This method implements Hebbian learning by detecting coincident spikes
     * and applying weight potentiation proportional to the correlation strength.
     * The rule strengthens synapses when presynaptic and postsynaptic neurons
     * fire within a specified time window.
     * 
     * @note PHASE 2: Enhanced implementation with biological realism and
     *       detailed synaptic dynamics is planned for future development.
     *       This placeholder provides the basic conceptual framework for
     *       Hebbian learning in the NLM framework.
     * 
     * @param synapse The synaptic connection to modify
     * @param preSpikes History of presynaptic spike times
     * @param postSpikes History of postsynaptic spike times
     * @param dt Simulation timestep duration
     */
    void HebbianRule::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
        if (preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Count coincident spikes (simplified)
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < 10.0) {  // 10ms window
                    ++coincidences;
                }
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
