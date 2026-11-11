#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <unordered_set>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float activityThreshold;
    float distanceFactor;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), activityThreshold(5.0f), distanceFactor(0.5f) {}
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
    // More sophisticated implementation based on biological principles
    
    if (!brain) return;
    
    auto regions = brain->getRegions();
    std::unordered_set<SynapseId> existingSynapses;
    
    // Collect existing synapses for duplicate checking
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            existingSynapses.insert(syn->getId());
        }
    }
    
    // Calculate current synaptic density
    size_t totalNeurons = 0;
    for (const auto& region : regions) {
        totalNeurons += region->getTotalNeuronCount();
    }
    
    size_t currentSynapses = 0;
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        currentSynapses += synapses.size();
    }
    
    float currentDensity = (totalNeurons > 0) ? (currentSynapses * 1.0f / (totalNeurons * totalNeurons)) : 0.0f;
    
    // Check if we need more synapses (based on target density)
    if (currentDensity >= pImpl->targetDensity) {
        return;  // Target density reached, no need to form new synapses
    }
    
    // Calculate how many new synapses to form
    size_t targetSynapses = static_cast<size_t>(totalNeurons * totalNeurons * pImpl->targetDensity);
    size_t synapsesToForm = targetSynapses - currentSynapses;
    
    if (synapsesToForm == 0) return;
    
    // Select neurons for potential synaptogenesis
    std::vector<NeuronId> neuronIds;
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                neuronIds.push_back(neuron->getId());
            }
        }
    }
    
    if (neuronIds.size() < 2) return;  // Need at least 2 neurons to form a synapse
    
    // Form new synapses based on multiple factors
    for (size_t attempts = 0; attempts < synapsesToForm * 10; ++attempts) {  // Try multiple times
        if (neuronIds.size() < 2) break;
        
        // Randomly select two neurons
        size_t idx1 = rng.uniformInt(size_t(0), neuronIds.size() - 1);
        size_t idx2 = rng.uniformInt(size_t(0), neuronIds.size() - 1);
        
        if (idx1 == idx2) continue;  // Same neuron
        
        const NeuronId& sourceId = neuronIds[idx1];
        const NeuronId& targetId = neuronIds[idx2];
        
        // Check if synapse already exists
        uint64_t sourceIndex = sourceId.index();
        uint64_t targetIndex = targetId.index();
        SynapseId synId(sourceIndex * 1000 + targetIndex);  // Simple ID generation
        
        if (existingSynapses.find(synId) != existingSynapses.end()) {
            continue;  // Synapse already exists
        }
        
        // Calculate formation probability based on multiple factors
        float formationProbability = calculateFormationProbability(brain, sourceId, targetId, rng);
        
        if (rng.uniformReal(0.0f, 1.0f) < formationProbability) {
            // Form new synapse
            formSynapse(brain, sourceId, targetId, rng);
            existingSynapses.insert(synId);
        }
    }
}

float Synaptogenesis::calculateFormationProbability(Brain* brain, const NeuronId& sourceId, 
                                                     const NeuronId& targetId, RandomGenerator& rng) const {
    // Calculate probability of synaptogenesis based on various biological factors
    
    if (!brain) return 0.0f;
    
    // Get neurons
    auto regions = brain->getRegions();
    Neuron* sourceNeuron = nullptr;
    Neuron* targetNeuron = nullptr;
    
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                if (neuron->getId() == sourceId) {
                    sourceNeuron = const_cast<Neuron*>(neuron);
                }
                if (neuron->getId() == targetId) {
                    targetNeuron = const_cast<Neuron*>(neuron);
                }
            }
        }
    }
    
    if (!sourceNeuron || !targetNeuron) {
        return 0.0f;
    }
    
    float probability = pImpl->formationRate;
    
    // Factor 1: Neuronal activity (Hebbian-like)
    float sourceActivity = sourceNeuron->getFiringRate();
    float targetActivity = targetNeuron->getFiringRate();
    float activityProduct = sourceActivity * targetActivity;
    
    if (activityProduct > 0.0f) {
        probability *= (1.0f + activityProduct);
    }
    
    // Factor 2: Distance between neurons (closer neurons more likely to connect)
    float distanceFactor = 1.0f - std::min(pImpl->distanceFactor, 1.0f);
    probability *= distanceFactor;
    
    // Factor 3: Neuron type compatibility
    NeuronType sourceType = sourceNeuron->getType();
    NeuronType targetType = targetNeuron->getType();
    
    // Excitatory-excitatory connections are more common
    if ((sourceType == NeuronType::Excitatory && targetType == NeuronType::Excitatory) ||
        (sourceType == NeuronType::Inhibitory && targetType == NeuronType::Inhibitory)) {
        probability *= 1.5f;
    }
    
    // Factor 4: Refractory period (neurons that recently fired are less likely to form new connections)
    if (sourceNeuron->isRefractory() || targetNeuron->isRefractory()) {
        probability *= 0.5f;
    }
    
    // Factor 5: Developmental stage (synaptogenesis decreases with age)
    float developmentalFactor = 1.0f;  // Would depend on developmental stage
    probability *= developmentalFactor;
    
    // Clamp probability to reasonable bounds
    return std::min(probability, 1.0f);
}

void Synaptogenesis::formSynapse(Brain* brain, const NeuronId& sourceId, const NeuronId& targetId, 
                                  RandomGenerator& rng) const {
    if (!brain) return;
    
    // Find source and target neurons
    Neuron* sourceNeuron = nullptr;
    Neuron* targetNeuron = nullptr;
    
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                if (neuron->getId() == sourceId) {
                    sourceNeuron = const_cast<Neuron*>(neuron);
                }
                if (neuron->getId() == targetId) {
                    targetNeuron = const_cast<Neuron*>(neuron);
                }
            }
        }
    }
    
    if (!sourceNeuron || !targetNeuron) return;
    
    // Determine synapse type based on neuron types
    SynapseType synapseType;
    if (sourceNeuron->getType() == NeuronType::Excitatory && targetNeuron->getType() == NeuronType::Inhibitory) {
        synapseType = SynapseType::Inhibitory;
    } else {
        synapseType = SynapseType::Excitatory;
    }
    
    // Create synapse
    SynapseId synId(sourceId.index() * 1000 + targetId.index());
    auto synapse = std::make_unique<Synapse>(synId, sourceId, targetId);
    synapse->setType(synapseType);
    synapse->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
    
    // Initialize synapse with random weight
    synapse->initializeRandom(rng);
    
    // Add synapse to brain's region
    // This would require access to the region where the neurons reside
    // For simplicity, we'll assume the synapse is added to the region containing the source neuron
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                if (neuron->getId() == sourceId) {
                    // Add outgoing synapse to source neuron
                    sourceNeuron->addOutgoingSynapse(synapse->getId());
                    // Note: The region would need to add the synapse to its synapse list
                    // This is a simplified implementation
                    break;
                }
            }
        }
    }
}

} // namespace nlm
