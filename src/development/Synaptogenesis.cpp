#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    size_t maxTotalSynapses;  // Maximum total synapses allowed
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), maxTotalSynapses(100000) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // TODO PHASE 2: Implement real synaptogenesis
    // PLACEHOLDER: New synapses form probabilistically
    
    if (!brain) return;
    
    struct NeuronActivity {
        NeuronId id;
        float activity;
        float avgWeight;
    };
    std::vector<NeuronActivity> activities;
    activities.reserve(100);  // Reserve space for performance
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            size_t synCount = 0;
            
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                ++synCount;
            }
            
            // Activity based on spike history
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                activity = static_cast<float>(spikes.size());
            }
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight});
        }
    }
    
    // Synaptogenesis: Create new synapses between active neurons
    for (size_t attempt = 0; attempt < activities.size(); ++attempt) {
        // Probabilistic synapse formation
        if (!rng.bernoulli(pImpl->formationRate)) continue;
        
        size_t idx1 = rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
        size_t idx2 = rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
        
        if (idx1 != idx2) {
            NeuronId src = activities[idx1].id;
            NeuronId dst = activities[idx2].id;
            
            // Only create if both neurons are somewhat active
            if (activities[idx1].activity > 0.1f || activities[idx2].activity > 0.1f) {
                // Initial weight based on activity
                float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
                createSynapse(brain, src, dst, weight);
            }
        }
    }
}

} // namespace nlm
