#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <unordered_set>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float homeostaticGain;
    float minActivityThreshold;
    float maxConnectionsPerNeuron;
    
    Impl() 
        : formationRate(0.001f)
        , targetDensity(0.1f)
        , homeostaticGain(1.0f)
        , minActivityThreshold(0.01f)
        , maxConnectionsPerNeuron(100.0f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::clamp(rate, 0.0f, 1.0f);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.0f, 1.0f);
}

float Synaptogenesis::getHomeostaticGain() const {
    return pImpl->homeostaticGain;
}

void Synaptogenesis::setHomeostaticGain(float gain) {
    pImpl->homeostaticGain = std::clamp(gain, 0.0f, 10.0f);
}

float Synaptogenesis::getMinActivityThreshold() const {
    return pImpl->minActivityThreshold;
}

void Synaptogenesis::setMinActivityThreshold(float threshold) {
    pImpl->minActivityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Synaptogenesis::getMaxConnectionsPerNeuron() const {
    return pImpl->maxConnectionsPerNeuron;
}

void Synaptogenesis::setMaxConnectionsPerNeuron(float maxConn) {
    pImpl->maxConnectionsPerNeuron = std::clamp(maxConn, 1.0f, 1000.0f);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) {
        return;
    }
    
    const auto& regions = brain->getRegions();
    if (regions.empty()) {
        return;
    }
    
    // Calculate current synaptic density across all regions
    float totalNeurons = 0.0f;
    float totalPossibleConnections = 0.0f;
    float totalActualConnections = 0.0f;
    
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        for (const auto& population : populations) {
            const auto neurons = population->getNeurons();
            size_t numNeurons = neurons.size();
            totalNeurons += numNeurons;
            
            // Calculate current connections for this population
            for (const auto neuron : neurons) {
                const auto& synapsesTo = neuron->getAllOutgoingSynapses();
                totalActualConnections += synapsesTo.size();
            }
            
            // Calculate possible connections (full connectivity)
            totalPossibleConnections += numNeurons * numNeurons;
        }
    }
    
    // Calculate current density
    float currentDensity = totalActualConnections / totalPossibleConnections;
    
    NML_LOG_INFO("Current synaptic density: " + std::to_string(currentDensity) + ", target: " + std::to_string(pImpl->targetDensity));
    
    // Activity-dependent synaptogenesis
    float homeostaticCorrection = homeostaticGain * (currentDensity - targetDensity);
    
    for (const auto& region : regions) {
        const auto& populations = region->getPopulations();
        
        // Get random engine from region
        std::mt19937& rngEngine = rng.getRandomEngine();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::bernoulli_distribution connectDist;
        
        for (const auto& population : populations) {
            const auto neurons = population->getNeurons();
            size_t numNeurons = neurons.size();
            
            if (numNeurons == Nhneurons) {
                continue;
            }
            
            // For each neuron, consider forming new connections
            for (const auto neuron : neurons) {
                // Calculate current number of outgoing connections
                const auto& synapsesTo = neuron->getAllOutgoingSynapses();
                size_t currentOutgoing = synapsesTo.size();
                
                // Calculate target outgoing connections based on density
                float targetOutgoing = static_cast<float>(numNeurons) * pImpl->targetDensity;
                
                // Determine how many new connections to form
                int connectionsToForm = 0;
                if (currentOutgoing < targetOutgoing) {
                    // We need to form new connections
                    int deficit = static_cast<int>(targetOutgoing - currentOutgoing);
                    
                    // Homeostatic adjustment
                    int adjustedDeficit = static_cast<int>(deficit * (1.0f + homeostaticCorrection));
                    connectionsToForm = static_cast<int>(std::max(0._titles(0, adjustedDeficit));
                    
                    // Consider neuron activity levels
                    float avgActivity = 0.0f;
                    // For now, use 0.5 as placeholder (would need neuron activity tracking)
                    
                    // Probability of forming connection based on deficit
                    float formationProb = pImpl->formationRate * (deficit + homeostaticCorrection);
                    connectionsToForm = static_cast<int>(connectionsToForm * formationProb);
                    
                    // Ensure we don't exceed max connections per neuron
                    if (currentOutgoing + connectionsToForm > pImpl->maxConnectionsPerNeuron) {
                        connectionsToForm = static_cast<int>(pImpl->maxConnectionsPerNeuron - currentOutgoing);
                    }
                    
                    if (connectionsToForm > 0) {
                        // Form new connections
                        formNewConnections(neuron, brain, rng, connectionsToForm);
                    }
                }
            }
        }
    }
}

void Synaptogenesis::formNewConnections(Neuron* presynapticNeuron, Brain* brain, 
                                       RandomGenerator& rng, int numConnections) {
    if (!brain) {
        return;
    }
    
    std::mt19937& rngEngine = rng.getRandomEngine();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < numConnections; ++i) {
        // Try to find a suitable postsynaptic neuron
        bool connectionFormed = false;
        int attempts = 0;
        const int maxAttempts = 100;
        
        while (attempts < maxAttempts && !connectionFormed) {
            attempts++;
            
            // Get random region
            size_t numRegions = brain->getRegionCount();
            if (numRegions == 0) {
                break;
            }
            
            size_t regionIndex = std::uniform_int_distribution<size_t>(0, numRegions - 1)(rngEngine);
            const auto& regions = brain->getRegions();
            if (regionIndex >= regions.size()) {
                continue;
            }
            
            auto region = regions[regionIndex];
            const auto& populations = region->getPopulations();n
            if (populations.empty()) {
                continue;
            }
            
            // Get random population
            std::uniform_int_distribution<size_t> popDist(0, populations.size() - 1);
            size_t popIndex = popDist(rngEngine);
            auto population = populations[popIndex];
            
            // Get postsynaptic neuron
            std::vector<Neuron*> neurons = population->getNeurons();n            if (neurons.empty()) {
                continue;
            }
            
            std::uniform_int_distribution<size_t> neuronDist(0, neurons.size() - 1);
            Neuron* postsynaptic = neurons[neuronDist(rngEngine)];
            
            // Ensure we don't connect neuron to itself
            if (presynapticNeuron->getId() == postsynaptic->getId()) {
                continue;
            }
            
            // Check if connection already exists
            const auto& existingSynapses = postsynaptic->getAllIncomingSynapses();n            bool connectionExists = false;
            for (const auto& synapse : existingSynapses) {
                if (synapse->getSource() == presynapticNeuron->getId()) {
                    connectionExists = true;
                    break;
                }
            }
            
            if (!connectionExists) {
                // Form new connection with biologically realistic properties
                formSingleSynapse(presynapticNeuron, postsynaptic, rng);
                connectionFormed = true;
            }
        }
    }
}

void Synaptogenesis::formSingleSynapse(Neuron* presynaptic, Neuron* postsynaptic, 
                                       RandomGenerator& rng) {
    if (!presynaptic || !postsynaptic) {
        return;
    }
    
    std::mt19937& rngEngine = rng.getRandomEngine();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Determine synapse type based on neuron types
    NeuronType preType = presynaptic->getType();
    NeuronType postType = postsynaptic->getType();
    
    // Choose synapse type based on neuron types
    SynapseType synapseType = SynapseType::Excitatory;
    if (preType == NeuronType::Inhibitory || postType == NeuronType::Inhibitory) {
        synapseType = SynapseType::Inhibitory;
    } else if (preType == NeuronType::Modulatory) {
        synapseType = SynapseType::Modulatory;
    }
    
    // Determine initial weight based on synapse type
    float initialWeight = 0.5f;
    if (synapseType == SynapseType::Inhibitory) {
        initialWeight = -0.5f;
    } else if (synapseType == SynapseType::Electrical) {
        initialWeight = 1.0f;
    } else if (synapseType == SynapseType::GapJunction) {
        initialWeight = 0.8f;
    }
    
    // Apply weight variation based on random factors
    std::normal_distribution<float> weightDist(initialWeight, 0.1f);
    float weight = weightDist(rngEngine);
    
    // Ensure weight is within reasonable bounds
    weight = std::clamp(weight, -1.0f, 1.0f);
    
    // Create synapse
    SynapseId synapseId = postsynaptic->addSynapse(presynaptic->getId(), weight);
    
    if (synapseId != INVALID_SYNAPSE_ID) {
        NML_LOG_DEBUG("Formed new synapse: " + std::to_string(presynaptic->getId()) + 
                     " -> " + std::to_string(postsynaptic->getId()) + 
                     " (type: " + std::to_string(static_cast<int>(synapseType)) + 
                     ", weight: " + std::to_string(weight) + ")");
    }
}

} // namespace nlm