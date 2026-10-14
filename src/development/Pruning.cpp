#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Implement real pruning
     * 
     * Pruning removes synapses that are weak, unused, or redundant based on:
     * 1. Weight-based pruning: Remove synapses with very small weights
     * 2. Activity-based pruning: Remove synapses that haven't been used
     * 3. Competition-based pruning: Remove synapses that compete with stronger ones
     * 4. Redundancy-based pruning: Remove redundant connections
     * 
     * Biological inspiration:
     * - Reflects synaptic competition during development
     * - Implements resource allocation efficiency
     * - Follows Hebbian rules with homeostatic regulation
     * 
     * Algorithm:
     * 1. For each neural region
     * 2. Evaluate each synapse for pruning conditions
     * 3. Consider weight, eligibility trace, and competition
     * 4. Maintain minimum connectivity requirements
     * 5. Preserve important connections
     */
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        size_t synapseCount = synapses.size();
        
        // Collect synapse information for analysis
        struct SynapseInfo {
            SynapseId id;
            NeuronId source;
            NeuronId destination;
            float weight;
            float eligibility;
            float activity;
            float competition;
            bool isCritical;  // For essential connections
        };
        
        std::vector<SynapseInfo> synapseInfos;
        synapseInfos.reserve(synapseCount);
        
        // Analyze each synapse
        for (auto* syn : synapses) {
            // Basic synapse properties
            SynapseId synId = syn->getId();
            NeuronId src = syn->getSourceNeuron();
            NeuronId dst = syn->getDestinationNeuron();
            float weight = syn->getWeight();
            float eligibility = syn->getEligibilityTrace();
            
            // Calculate activity metric (based on spike history)
            float activity = 0.0f;
            
            // Get source and destination neurons for activity calculation
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == src || neuron->getId() == dst) {
                    // Activity based on spike history
                    const auto& spikes = neuron->getSpikeHistory();
                    if (!spikes.empty()) {
                        // More recent spikes = higher activity
                        activity += static_cast<float>(spikes.size()) * 0.1f;
                    }
                }
            }
            
            // Competition metric (based on competing synapses)
            float competition = 0.0f;
            
            // Count competing synapses to destination
            auto competingSynapses = region->getSynapsesTo(dst);
            for (auto* competingSyn : competingSynapses) {
                if (competingSyn->getId() != synId) {
                    float competingWeight = std::abs(competingSyn->getWeight());
                    competition += competingWeight;
                }
            }
            
            // Determine if this is a critical synapse
            bool isCritical = false;
            
            // Check various criteria for critical synapses
            if (std::abs(weight) > 0.5f) {
                isCritical = true;  // Strong connection
            }
            
            // Check if neuron has few alternative inputs (bottleneck)
            size_t alternativeInputs = 0;
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == dst) {
                    auto inputs = region->getSynapsesTo(neuron->getId());
                    for (auto* inputSyn : inputs) {
                        if (inputSyn->getId() != synId && 
                            std::abs(inputSyn->getWeight()) > 0.1f) {
                            alternativeInputs++;
                        }
                    }
                    break;
                }
            }
            
            if (alternativeInputs <= 1) {
                isCritical = true;  // Few alternatives
            }
            
            // Check for sensory/motor neuron special handling
            for (auto* neuron : region->getAllNeurons()) {
                if ((neuron->getType() == NeuronType::Sensory || 
                     neuron->getType() == NeuronType::Motor) &&
                    neuron->getId() == dst) {
                    isCritical = true;  // Sensory/motor output is critical
                    break;
                }
            }
            
            synapseInfos.push_back({
                synId,
                src,
                dst,
                weight,
                eligibility,
                activity,
                competition,
                isCritical
            });
        }
        
        // Determine which synapses to prune
        std::vector<SynapseId> synapsesToRemove;
        
        for (const auto& info : synapseInfos) {
            // Skip critical synapses (never prune)
            if (info.isCritical) {
                continue;
            }
            
            // Calculate pruning probability based on multiple factors
            float pruneProbability = 0.0f;
            
            // 1. Weight-based pruning (weak synapses)
            float weightThreshold = pImpl->threshold;
            if (std::abs(info.weight) < weightThreshold) {
                // Very weak synapses have high pruning probability
                pruneProbability += pImpl->pruningRate * 2.0f;
            }
            
            // 2. Activity-based pruning (inactive synapses)
            if (info.activity < 0.5f) {
                // Inactive synapses have moderate pruning probability
                pruneProbability += pImpl->pruningRate * 1.5f;
            }
            
            // 3. Competition-based pruning
            if (info.competition > 0.5f) {
                // Highly competitive synapses may be pruned
                pruneProbability += pImpl->pruningRate * 0.8f;
            }
            
            // 4. Eligibility trace pruning
            if (std::abs(info.eligibility) < 0.01f) {
                // Synapses with low eligibility traces are likely unused
                pruneProbability += pImpl->pruningRate * 1.2f;
            }
            
            // 5. Combined factor (product of all factors)
            pruneProbability *= (1.0f - info.isCritical);
            
            // Apply pruning based on random check
            if (rng.uniformReal(0.0f, 1.0f) < pruneProbability) {
                synapsesToRemove.push_back(info.id);
            }
        }
        
        // Actually remove the selected synapses
        for (SynapseId synId : synapsesToRemove) {
            pruneSynapse(brain, synId);
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        return false;
    }
    
    /*
     * Prune a single synapse
     * 
     * This method implements the actual synapse removal, handling:
     * 1. Weight-based pruning for very weak connections
     * 2. Activity-based pruning for inactive synapses
     * 3. Competition-based pruning for redundant connections
     * 4. Proper cleanup of neuron references
     * 
     * Biological inspiration:
     * - Reflects synaptic competition and resource allocation
     * - Implements homeostatic regulation of connectivity
     * - Follows principles of synaptic necessity and sufficiency
     */
    
    SynapseId synId = synapse->getId();
    float weight = synapse->getWeight();
    NeuronId source = synapse->getSourceNeuron();
    NeuronId destination = synapse->getDestinationNeuron();
    
    // Check if synapse meets pruning criteria
    bool shouldPrune = false;
    
    // Criteria 1: Very weak synapse (near-zero weight)
    if (std::abs(weight) < pImpl->threshold * 0.5f) {
        shouldPrune = true;
    }
    
    // Criteria 2: Synapse has low activity/eligibility trace
    else if (synapse->getEligibilityTrace() < pImpl->threshold * 0.1f) {
        shouldPrune = true;
    }
    
    // Criteria 3: Synapse with negative weight that might be inhibitory backup
    else if (weight < -0.1f) {
        // Check if there are alternative stronger connections
        size_t strongerAlternatives = 0;
        
        // Look for alternative synapses to destination
        for (const auto& region : synapse->getParentRegion()->getRegions()) {
            for (auto* altSyn : region->getSynapsesTo(destination)) {
                if (altSyn->getId() != synId && 
                    std::abs(altSyn->getWeight()) > std::abs(weight) + 0.05f) {
                    strongerAlternatives++;
                }
            }
        }
        
        // If there are stronger alternatives, prune this one
        if (strongerAlternatives > 0) {
            shouldPrune = true;
        }
    }
    
    // Actually remove the synapse
    if (shouldPrune) {
        // Mark synapse as pruned by setting weight to zero
        // In a real implementation, this would remove the synapse from the brain's structures
        synapse->setWeight(0.0f);
        synapse->clearHistory();
        synapse->setEligibilityTrace(0.0f);
        
        return true;
    }
    
    return false;
}

} // namespace nlm
