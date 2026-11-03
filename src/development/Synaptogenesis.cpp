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
    // PHASE 2: Real synaptogenesis implementation
    // Implements activity-dependent synapse formation with biological rules
    
    if (!brain) return;
    
    // Calculate current synapse density
    float currentDensity = brain->getTotalSynapseCount() / 
                           std::max(1.0f, brain->getTotalNeuronCount());
    
    // Target synapse density for the network
    float targetDensity = getTargetDensity();
    
    // Determine if new synapses should form based on density difference
    if (currentDensity < targetDensity) {
        // Network needs more synapses - form new ones
        float densityDeficit = targetDensity - currentDensity;
        float formationProbability = getFormationRate() * densityDeficit;
        
        // Iterate through all neurons and form potential new synapses
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (Neuron* preNeuron : pop->getNeurons()) {
                    // Only form synapses from neurons that have been active recently
                    if (preNeuron->getState().lastSpikeTime > 0.0f) {
                        // Find postsynaptic targets
                        for (auto& targetPop : region->getPopulations()) {
                            // Don't form self-connections
                            if (targetPop.getId() == pop.getId()) {
                                continue;
                            }
                            
                            // Probability of forming new connection
                            float connectionProbability = 0.01f;  // Base connection probability
                            
                            // Modulate by neuron type compatibility
                            NeuronType preType = preNeuron->getType();
                            NeuronType targetType = targetPop.getNeuronType();
                            
                            // Excitatory neurons preferentially form connections
                            // with other excitatory neurons
                            if (preType == NeuronType::Excitatory && targetType == NeuronType::Excitatory) {
                                connectionProbability *= 2.0f;
                            }
                            
                            // Higher probability during early development
                            if (brain->getDevelopmentalStage() == DevelopmentalStage::Early) {
                                connectionProbability *= 3.0f;
                            }
                            
                            // Random chance to form new synapse
                            if (rng.uniformReal(0.0f, 1.0f) < connectionProbability) {
                                // Create new synapse
                                SynapseId newSynapseId = createNewSynapseId(region.getId());
                                NeuronId preId = preNeuron->getId();
                                NeuronId postId = targetPop.getNeuron(0)->getId(); // Simplified: use first neuron
                                
                                // Initialize synapse with appropriate parameters
                                Synapse* newSynapse = new Synapse(newSynapseId, preId, postId);
                                newSynapse->initializeRandom(rng);
                                
                                // Add synapse to both neurons and region
                                preNeuron->addOutgoingSynapse(newSynapse->getId());
                                auto* postNeuron = brain->getNeuron(postId);
                                if (postNeuron) {
                                    postNeuron->addIncomingSynapse(newSynapse->getId());
                                }
                                region->addSynapse(newSynapse);
                            }
                        }
                    }
                }
            }
        }
    }
}

SynapseId Synaptogenesis::createNewSynapseId(RegionId regionId) {
    // Generate unique synapse ID for the region
    // In a real implementation, this would maintain a counter per region
    static std::unordered_map<RegionId, SynapseId> regionCounters;
    
    auto it = regionCounters.find(regionId);
    if (it == regionCounters.end()) {
        it = regionCounters.insert({regionId, 1}).first;
    }
    
    return SynapseId(it->second++);
}

} // namespace nlm

