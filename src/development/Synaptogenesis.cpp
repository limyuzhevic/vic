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
    // Real synaptogenesis with biological constraints
    // Synaptogenesis follows an activity-dependent pattern where neural activity
    // drives new synapse formation, typically during critical periods
    
    // Get current density and neural population info
    float currentDensity = brain->getCurrentSynapseDensity();
    float densityGap = pImpl->targetDensity - currentDensity;
    
    if (densityGap > 0.0f) {
        // Formation rate depends on neural activity
        float activityFactor = brain->getOverallNeuralActivity();
        float formationRate = pImpl->formationRate * (1.0f + activityFactor * 2.0f);
        
        // Calculate potential new synapses based on probability and random chance
        size_t totalPotentialConnections = brain->getTotalPotentialConnections();
        size_t maxNewSynapses = static_cast<size_t>(totalPotentialConnections * formationRate);
        
        // Create new synapses based on target density
        size_t synapsesToCreate = static_cast<size_t>(totalPotentialConnections * pImpl->targetDensity) - 
                                 brain->getCurrentSynapseCount();
        
        if (synapsesToCreate > 0 && maxNewSynapses > 0) {
            size_t actualNewSynapses = std::min(synapsesToCreate, maxNewSynapses);
            
            for (size_t i = 0; i < actualNewSynapses; ++i) {
                // Select source and destination neurons randomly
                NeuronId sourceId = brain->getRandomNeuronId(rng);
                NeuronId destId = brain->getRandomNeuronId(rng);
                
                // Ensure neurons are different
                if (sourceId == destId) continue;
                
                // Check if synapse already exists
                if (brain->hasSynapse(sourceId, destId)) continue;
                
                // Create new synapse with initial parameters
                Synapse synapse(sourceId, destId);
                
                // Determine synapse type based on neuron types
                NeuronType sourceType = brain->getNeuronType(sourceId);
                NeuronType destType = brain->getNeuronType(destId);
                
                if (sourceType == NeuronType::Excitatory || destType == NeuronType::Sensory) {
                    synapse.setType(SynapseType::Excitatory);
                } else if (sourceType == NeuronType::Inhibitory) {
                    synapse.setType(SynapseType::Inhibitory);
                } else {
                    synapse.setType(SynapseType::Modulatory);
                }
                
                // Set initial weight based on neuron types
                float weight = 1.0f;
                if (synapse.isInhibitory()) {
                    weight = -1.0f + rng.uniformReal(-0.5f, 0.5f);
                } else {
                    weight = 1.0f + rng.uniformReal(-0.5f, 0.5f);
                }
                
                synapse.setWeight(weight);
                
                // Set initial delay (shorter for nearby neurons)
                float distance = brain->getNeuralDistance(sourceId, destId);
                synapse.setDelay(static_cast<uint32_t>(std::max(1.0f, distance * 10.0f)));
                
                // Enable plasticity
                synapse.enablePlasticity(true, true, true);
                
                // Add to brain
                brain->addSynapse(std::move(synapse));
            }
        }
    }
}

} // namespace nlm
