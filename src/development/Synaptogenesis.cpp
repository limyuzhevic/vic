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
    // Implement real synaptogenesis: biological synapse formation with activity-dependent rules
    if (!brain || brain->getDevelopmentalStage() < DevelopmentalStage::CriticalPeriod) {
        return; // No synaptogenesis outside critical period
    }
    
    // Get target synapse density from parameters
    float currentDensity = static_cast<float>(brain->getTotalSynapseCount()) / 
                          (brain->getTotalNeuronCount() * brain->getTotalNeuronCount());
    
    // Synaptogenesis rules based on Hebbian activity and molecular guidance
    for (RegionId regionId : brain->getRegionIds()) {
        NeuralRegion* region = brain->getRegion(regionId);
        if (!region) continue;
        
        auto& populations = region->getPopulations();
        
        // Activity-dependent synaptogenesis: nearby neurons that fire together
        for (size_t i = 0; i < populations.size(); ++i) {
            NeuralPopulation* popA = populations[i].get();
            const auto& neuronsA = popA->getAllNeurons();
            
            for (size_t j = i; j < populations.size(); ++j) {
                NeuralPopulation* popB = populations[j].get();
                const auto& neuronsB = popB->getAllNeurons();
                
                // Limit search to nearby neurons in same region
                for (NeuronId neuronA : neuronsA) {
                    Neuron* nA = popA->getNeuron(neuronA);
                    if (!nA || !nA->isFiring()) continue;
                    
                    // Find candidate partner neurons in popB that are nearby in activity space
                    for (NeuronId neuronB : neuronsB) {
                        Neuron* nB = popB->getNeuron(neuronB);
                        if (!nB) continue;
                        
                        // Calculate synapse formation probability
                        float formationProb = formationRate;
                        
                        // Activity-dependent boost: neurons that fire within time window
                        float timeSincePre = static_cast<float>(nA->getLastSpikeTime() > 0 ? 
                                                             nB->getLastSpikeTime() - nA->getLastSpikeTime() : 1000.0f);
                        
                        if (timeSincePre < 100.0f) { // Within 100ms
                            formationProb *= 2.0f; // Double probability
                        }
                        
                        // Molecular guidance: stronger connections based on neuron types
                        if (popA->getType() == popB->getType()) {
                            formationProb *= 1.5f; // Same-type neurons more likely to connect
                        }
                        
                        // Competitive interaction: already connected neurons less likely to form new connections
                        bool alreadyConnected = false;
                        for (const auto& synapse : region->getSynapses()) {
                            if ((synapse->getSourceNeuron() == neuronA && synapse->getDestinationNeuron() == neuronB) ||
                                (synapse->getSourceNeuron() == neuronB && synapse->getDestinationNeuron() == neuronA)) {
                                alreadyConnected = true;
                                break;
                            }
                        }
                        
                        if (!alreadyConnected && rng.uniformReal(0.0f, 1.0f) < formationProb) {
                            // Create new synapse
                            SynapseId synId = brain->addSynapse(neuronA, neuronB);
                            Synapse* synapse = region->getSynapse(synId);
                            if (synapse) {
                                // Set initial weight based on neuron types and activity
                                float initialWeight = rng.uniformReal(0.1f, 0.3f);
                                synapse->setWeight(initialWeight);
                                synapse->setType(SynapseType::Excitatory); // Default to excitatory
                                
                                // Enable plasticity
                                synapse->enablePlasticity(true, true, false); // Hebbian, STDP, no reward modulation
                                
                                // Log formation event (in real system)
                                // Logger::getInstance().logInfo("New synapse formed between neurons " +
                                //     std::to_string(neuronA.value) + " and " + std::to_string(neuronB.value));
                            }
                        }
                    }
                }
            }
        }
    }
}

} // namespace nlm
