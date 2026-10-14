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
     * Implement real synaptogenesis
     * 
     * Synaptogenesis creates new connections between neurons based on:
     * 1. Activity-dependent rules: Synapses form between frequently co-active neurons
     * 2. Competition: New connections compete with existing ones for resources
     * 3. Plasticity: Formation rate modulates with developmental stage
     * 
     * Biological inspiration:
     * - Reflects Hebbian learning during critical periods
     * - Implements competition for synaptic resources
     * - Follows developmental trajectory of connectivity
     * 
     * Algorithm:
     * 1. For each neuron region in the brain
     * 2. Calculate current synaptic density
     * 3. If below target density, form new connections
     * 4. New synapses preferentially form between:
     *    - Recently active neurons
     *    - Neurons with similar firing patterns
     *    - Neurons that are already somewhat connected
     * 5. New synapses start with small initial weights
     * 6. Maintain maximum synapse limits per neuron
     */
    if (!brain) return;
    
    // Calculate current synaptic density
    size_t totalNeurons = 0;
    size_t totalPossibleSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        totalNeurons += neurons.size();
        
        // Each neuron can connect to every other neuron in the same region
        if (neurons.size() > 1) {
            totalPossibleSynapses += neurons.size() * (neurons.size() - 1);
        }
    }
    
    if (totalPossibleSynapses == 0) return;
    
    float currentDensity = static_cast<float>(brain->getTotalSynapseCount()) / totalPossibleSynapses;
    
    // If we're below target density, attempt synaptogenesis
    if (currentDensity < pImpl->targetDensity) {
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            size_t neuronCount = neurons.size();
            
            if (neuronCount < 2) continue;
            
            // Activity-based selection for synaptogenesis
            struct NeuronActivity {
                NeuronId id;
                float recentActivity;  // Based on spike history
                float currentConnectivity;  // Number of existing synapses
                float avgSynapticWeight;  // Mean weight of incoming synapses
            };
            
            std::vector<NeuronActivity> activities;
            activities.reserve(neuronCount);
            
            // Calculate activity metrics for each neuron
            for (auto* neuron : neurons) {
                float recentActivity = 0.0f;
                float currentConnectivity = 0.0f;
                float avgSynapticWeight = 0.0f;
                
                // Activity from recent spike history (more recent = higher)
                const auto& spikes = neuron->getSpikeHistory();
                if (!spikes.empty()) {
                    // Weight more heavily for recent spikes
                    recentActivity = std::min(static_cast<float>(spikes.size()), 10.0f) * 0.1f;
                }
                
                // Current connectivity (number of incoming synapses)
                auto incoming = region->getSynapsesTo(neuron->getId());
                currentConnectivity = incoming.size();
                
                // Average synaptic weight
                for (auto* syn : incoming) {
                    avgSynapticWeight += std::abs(syn->getWeight());
                }
                
                if (currentConnectivity > 0) {
                    avgSynapticWeight /= currentConnectivity;
                }
                
                activities.push_back({
                    neuron->getId(),
                    recentActivity,
                    currentConnectivity,
                    avgSynapticWeight
                });
            }
            
            // Attempt synaptogenesis based on formation rate
            for (size_t attempt = 0; attempt < std::min<size_t>(neuronCount * 2, 100); ++attempt) {
                // Check if formation rate threshold is met
                if (rng.uniformReal(0.0f, 1.0f) > pImpl->formationRate) {
                    continue;
                }
                
                // Select candidate neurons for connection
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 == idx2) continue;  // No self-connections
                
                const auto& act1 = activities[idx1];
                const auto& act2 = activities[idx2];
                
                // Synaptogenesis rules:
                // 1. Prefer connections between neurons with similar activity levels
                // 2. Prefer connections to neurons that already have moderate connectivity
                // 3. Prefer connections to neurons with moderate synaptic weights
                
                float activityMatch = 1.0f - std::abs(act1.recentActivity - act2.recentActivity);
                float connectivityMatch = 1.0f - std::abs(
                    act1.currentConnectivity - act2.currentConnectivity) / 50.0f;
                float weightMatch = 1.0f - std::abs(
                    act1.avgSynapticWeight - act2.avgSynapticWeight);
                
                float connectionProbability = pImpl->formationRate * 
                    activityMatch * connectivityMatch * weightMatch;
                
                if (rng.uniformReal(0.0f, 1.0f) > connectionProbability) {
                    continue;
                }
                
                // Ensure we don't exceed maximum connectivity limits
                auto synapsesToDst1 = region->getSynapsesTo(act1.id);
                auto synapsesToDst2 = region->getSynapsesTo(act2.id);
                
                // Check if connection would exceed limits
                bool canConnect = true;
                
                // For source = act2, destination = act1
                if (std::count_if(synapsesToDst1.begin(), synapsesToDst1.end(),
                                 [act2](const Synapse* syn) { return syn->getSourceNeuron() == act2.id; }) < 10) {
                    
                    // For source = act1, destination = act2
                    if (std::count_if(synapsesToDst2.begin(), synapsesToDst2.end(),
                                     [act1](const Synapse* syn) { return syn->getSourceNeuron() == act1.id; }) < 10) {
                        // Check if this specific connection already exists
                        bool exists = false;
                        for (auto* syn : synapsesToDst1) {
                            if (syn->getSourceNeuron() == act2.id) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            // Create new synapse with initial weight based on activity
                            SynapticWeight initialWeight = 0.05f;  // Small initial weight
                            
                            // Modulate by activity match
                            initialWeight *= activityMatch;
                            
                            // Ensure weight is reasonable
                            initialWeight = std::clamp(initialWeight, -0.2f, 0.2f);
                            
                            // Create the synapse
                            SynapseId synId = region->addSynapse(act2.id, act1.id, initialWeight, 1);
                            
                            if (synId != INVALID_SYNAPSE_ID) {
                                // Enable plasticity for the new synapse
                                auto* syn = region->getSynapse(synId);
                                if (syn) {
                                    syn->enablePlasticity(true, true, false);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

} // namespace nlm
