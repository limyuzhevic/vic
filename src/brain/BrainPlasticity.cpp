#include "BrainHeaders.hpp"

namespace nlm {

void BrainPlasticity::applySTDPPlasticity(Brain* brain, float plasticityMod, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* stdp = brain->getSTDP();
    if (!stdp) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
        }
    }
}

void BrainPlasticity::applyHebbianPlasticity(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* hebbian = brain->getHebbian();
    if (!hebbian) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
        }
    }
}

void BrainPlasticity::applyStructuralPlasticity(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* structural = brain->getStructuralPlasticity();
    if (structural) {
        structural->update(timestep);
        
        // Synaptogenesis based on activity
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getType() == NeuronType::Internal) {
                    float activityLevel = pop->getAverageFiringRate();
                    if (activityLevel > 0.5f) {
                        region->addSynapseToPopulation(pop->getId());
                    }
                }
            }
        }
        
        // Pruning based on inactivity
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getType() == NeuronType::Internal) {
                    float activityLevel = pop->getAverageFiringRate();
                    if (activityLevel < 0.1f && pop->getNeuronCount() > 10) {
                        region->removeInactiveSynapsesFromPopulation(pop->getId(), 0.1f);
                    }
                }
            }
        }
    }
}

void BrainPlasticity::updateSynapticStrength(Brain* brain, Synapse* synapse, TimestepDuration timestep) {
    if (!brain || !synapse) return;
    synapse->update(timestep);
}

void BrainPlasticity::stabilizeNetwork(Brain* brain) {
    if (!brain) return;
    
    // Prevent runaway excitation by normalizing weights
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    size_t excitatoryCount = 0;
    size_t inhibitoryCount = 0;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getType() == SynapseType::Excitatory) {
                totalExcitatory += syn->getWeight();
                ++excitatoryCount;
            } else {
                totalInhibitory += syn->getWeight();
                ++inhibitoryCount;
            }
        }
    }
    
    // Adjust weights to maintain balance
    if (excitatoryCount > 0 && inhibitoryCount > 0) {
        float targetRatio = 1.0f; // Target E/I ratio
        float currentRatio = totalExcitatory / (totalInhibitory + 0.001f);
        float adjustmentFactor = targetRatio / currentRatio;
        
        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getType() == SynapseType::Excitatory) {
                    syn->setWeight(syn->getWeight() * adjustmentFactor);
                } else {
                    syn->setWeight(syn->getWeight() / adjustmentFactor);
                }
            }
        }
    }
}

} // namespace nlm