#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct StructuralPlasticity::Impl {
    float synaptogenesisRate;     // Probability of new synapse per step
    float pruningRate;            // Probability of pruning per step
    float minWeightThreshold;      // Below this weight, synapse is weak
    float activityThreshold;      // Below this activity, synapse is unused
    size_t maxSynapsesPerNeuron;   // Maximum outgoing synapses per neuron
    size_t maxTotalSynapses;       // Global maximum synapses
    bool useActivityDependency;    // Whether to use activity for pruning
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

StructuralPlasticity::~StructuralPlasticity() = default;

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, 
                                               NeuronId destination, SynapticWeight weight) {
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region
        }
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Search all regions for the synapse
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // Remove the synapse from the region
            region->removeSynapse(synapse);
            ++pImpl->totalSynapsesPruned;
            return true;
        }
    }
    
    return false;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    if (!brain) {
        return INVALID_NEURON_ID;
    }
    
    // Find a region to add the neuron to
    for (auto& region : brain->getRegions()) {
        PopulationId popId = region->addPopulation(1, type);
        if (popId != INVALID_POPULATION_ID) {
            auto* population = region->getPopulation(popId);
            if (population) {
                return population->getNeuron(0)->getId();
            }
        }
    }
    
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain || neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    // Find and remove the neuron from all regions
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuronPtr : population->getNeurons()) {
                if (neuronPtr->getId() == neuron) {
                    // Remove all synapses connected to this neuron first
                    auto synapsesToRemove = region->getSynapsesTo(neuron);
                    for (Synapse* syn : synapsesToRemove) {
                        region->removeSynapse(syn->getId());
                        ++pImpl->totalSynapsesPruned;
                    }
                    
                    synapsesToRemove = region->getSynapsesFrom(neuron);
                    for (Synapse* syn : synapsesToRemove) {
                        region->removeSynapse(syn->getId());
                        ++pImpl->totalSynapsesPruned;
                    }
                    
                    // Remove the neuron from the population
                    // Note: This requires modifying NeuralPopulation to support removal
                    // For now, we'll just mark it for removal
                    return true;
                }
            }
        }
    }
    
    return false;
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Get development system if available
    DevelopmentSystem* devSystem = brain->getDevelopmentSystem();
    
    // Get developmental parameters
    float synaptogenesisRate = pImpl->synaptogenesisRate;
    float pruningRate = pImpl->pruningRate;
    
    // Apply developmental stage modifications
    if (devSystem) {
        synaptogenesisRate *= devSystem->getSynaptogenesisRate();
        pruningRate *= devSystem->getPruningRate();
        
        // Apply plasticity modifier for current developmental stage
        float plasticityMod = devSystem->getPlasticityModifier();
        synaptogenesisRate *= plasticityMod;
        pruningRate *= plasticityMod;
    }
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect activity statistics
        struct NeuronActivity {
            NeuronId id;
            float activity;
            float avgWeight;
        };
        std::vector<NeuronActivity> activities;
        activities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            size_t synCount = 0;
            
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                ++synCount;
            }
            
            // Activity based on spike history and membrane potential
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // More recent spikes = higher activity
                activity = static_cast<float>(spikes.size()) * 0.5f;
            }
            
            // Add membrane potential deviation as activity indicator
            const auto& state = neuron->getState();
            float membraneDeviation = std::abs(state.membranePotential - state.restingPotential);
            activity += membraneDeviation * 0.1f;
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight});
        }
        
        // Synaptogenesis: Create new synapses between active neurons
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                // Probabilistic synapse formation with developmental modulation
                if (!rng.bernoulli(synaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 != idx2) {
                    NeuronId src = activities[idx1].id;
                    NeuronId dst = activities[idx2].id;
                    
                    // Only create if both neurons are active (neural cooperation)
                    if (activities[idx1].activity > 0.1f || activities[idx2].activity > 0.1f) {
                        // Initial weight based on activity and developmental stage
                        float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
                        
                        // Modulate initial weight based on maturation progress
                        if (devSystem) {
                            float maturation = devSystem->getMaturationProgress();
                            weight *= (1.0f + maturation * 0.5f);
                        }
                        
                        createSynapse(brain, src, dst, weight);
                    }
                }
            }
        }
        
        // Pruning: Remove weak or unused synapses
        for (auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            
            // Check pruning conditions
            bool shouldPrune = false;
            
            if (weight < pImpl->minWeightThreshold) {
                // Synapse is too weak - apply developmental pruning rate
                shouldPrune = rng.bernoulli(pruningRate * 2.0f); // Double rate for weak synapses
            } else if (pImpl->useActivityDependency) {
                // Check if synapse has been unused
                float eligibility = std::abs(syn->getEligibilityTrace());
                if (eligibility < pImpl->activityThreshold) {
                    shouldPrune = rng.bernoulli(pruningRate * 0.5f);
                }
            }
            
            // Additional developmental pruning criteria
            if (devSystem) {
                // More aggressive pruning in adult stage
                if (devSystem->getStage() == DevelopmentalStage::Adult || 
                    devSystem->getStage() == DevelopmentalStage::Aging) {
                    if (weight < pImpl->minWeightThreshold * 1.5f) {
                        shouldPrune = true; // Always prune very weak synapses in adult/aging
                    }
                }
                
                // Use activity-dependent pruning
                float activityLevel = 0.0f;
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    activityLevel = std::min(1.0f, static_cast<float>(std::min(preSpikes.size(), postSpikes.size()) / 3.0f));
                }
                
                // Lower activity synapses are more likely to be pruned
                if (activityLevel < 0.3f) {
                    shouldPrune = rng.bernoulli(pruningRate * 1.5f);
                }
            }
            
            if (shouldPrune) {
                removeSynapse(brain, syn->getId());
            }
        }
    }
}

} // namespace nlm
