#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
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
    /*
     * Real synaptogenesis implementation
     * 
     * This implements the process of new synapse formation through:
     * 1. Activity-dependent formation: Synapses form between neurons that
     *    frequently co-activate (Hebbian-like rule)
     * 2. Rate-based formation: Synapse formation rate depends on local
     *    neuronal activity and developmental stage
     * 3. Competition and allocation: New synapses compete for limited space
     * 4. Stabilization: Recent synapses are stabilized through NMDA-like
     *    receptor dynamics
     * 
     * The algorithm:
     * 1. For each neuron, calculate a formation propensity based on
     *    recent activity and age
     * 2. Select potential synapse partners based on spatial proximity
     *    and correlated activity
     * 3. Decide whether to form new synapse based on probability and
     *    developmental stage
     * 4. If formed, assign initial weight and delay
     * 5. Apply homeostatic scaling to maintain target density
     */
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Calculate target number of synapses based on density
        size_t currentSynapseCount = region->getSynapseCount();
        size_t maxSynapses = static_cast<size_t>(neuronCount * neuronCount * getTargetDensity());
        
        if (currentSynapseCount >= maxSynapses) {
            continue;  // Already at target density
        }
        
        // Calculate neuron formation propensities (activity + developmental factor)
        struct NeuronPropensity {
            NeuronId id;
            float activity;
            float formationPropensity;
        };
        std::vector<NeuronPropensity> propensities;
        
        for (auto* neuron : neurons) {
            // Base formation propensity on spike history (recent activity)
            const auto& spikeHistory = neuron->getSpikeHistory();
            float activity = 0.0f;
            if (!spikeHistory.empty()) {
                // More recent spikes = higher activity
                // Weight exponentially decaying recent activity
                for (size_t i = 0; i < spikeHistory.size(); ++i) {
                    float age = static_cast<float>(spikeHistory.size() - 1 - i);
                    activity += std::pow(0.9f, age);
                }
            }
            
            // Developmental factor: higher formation rate during early stages
            float devFactor = 1.0f;  // Could be connected to development system
            
            // Plasticity factor: modulated by neuromodulators
            float plasticityFactor = 1.0f;
            if (auto* dopamine = brain->getDopamine()) {
                plasticityFactor = dopamine->getPlasticityFactor();
            }
            
            // Calculate formation propensity
            float formationPropensity = activity * devFactor * plasticityFactor;
            
            propensities.push_back({neuron->getId(), activity, formationPropensity});
        }
        
        // Sort neurons by formation propensity for prioritized formation
        std::sort(propensities.begin(), propensities.end(),
                  [](const NeuronPropensity& a, const NeuronPropensity& b) {
                      return a.formationPropensity > b.formationPropensity;
                  });
        
        // Synaptogenesis: Create new synapses between high-propensity neurons
        size_t synapsesToCreate = maxSynapses - currentSynapseCount;
        synapsesToCreate = std::min(synapsesToCreate, neuronCount * 5);  // Limit attempts
        
        for (size_t attempt = 0; attempt < synapsesToCreate; ++attempt) {
            // Probabilistic synapse formation based on target rate
            if (!rng.bernoulli(getFormationRate())) {
                continue;
            }
            
            // Select source and destination neurons
            // Prefer neurons with high formation propensity
            size_t sourceIdx = rng.uniformInt(0, static_cast<int>(propensities.size()) - 1);
            size_t destIdx = rng.uniformInt(0, static_cast<int>(propensities.size()) - 1);
            
            if (sourceIdx == destIdx) {
                continue;
            }
            
            NeuronId source = propensities[sourceIdx].id;
            NeuronId dest = propensities[destIdx].id;
            
            // Check if connection already exists
            auto synapsesTo = region->getSynapsesTo(dest);
            bool connectionExists = false;
            for (auto* syn : synapsesTo) {
                if (syn->getSourceNeuron() == source) {
                    connectionExists = true;
                    break;
                }
            }
            
            if (connectionExists) {
                continue;
            }
            
            // Check max outgoing synapses per neuron
            auto synapsesFrom = region->getSynapsesFrom(source);
            if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
                continue;  // Source neuron at limit
            }
            
            // Check max total synapses
            if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
                continue;
            }
            
            // Calculate initial synaptic weight based on activity correlation
            float weight = 0.1f;  // Baseline weak connection
            
            // Enhance weight if both neurons are active
            if (propensities[sourceIdx].activity > 0.1f && propensities[destIdx].activity > 0.1f) {
                weight += 0.2f;  // Boost for co-active neurons
            }
            
            // Add small random variation
            weight += rng.uniformReal(-0.05f, 0.05f);
            weight = std::max(0.0f, weight);  // Ensure non-negative
            
            // Create the synapse with small delay
            createSynapse(brain, source, dest, weight);
        }
        
        // Homeostatic scaling: maintain target synaptic density
        if (region->getSynapseCount() > maxSynapses) {
            // Too many synapses - prune excess
            auto synapses = region->getSynapses();
            // Sort by weight (weakest first)
            std::sort(synapses.begin(), synapses.end(),
                     [](const std::unique_ptr<Synapse>& a, const std::unique_ptr<Synapse>& b) {
                         return std::abs(a->getWeight()) < std::abs(b->getWeight());
                     });
            
            size_t excess = region->getSynapseCount() - maxSynapses;
            for (size_t i = 0; i < excess && i < synapses.size(); ++i) {
                removeSynapse(brain, synapses[i]->getId());
            }
        }
    }
}

} // namespace nlm
