#include "StructuralPlasticity.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// EnhancedStructuralPlasticity implementation
struct EnhancedStructuralPlasticity::Impl {
    StructuralPlasticityParameters parameters;
    float simulationTime;
    float timeSinceLastScaling;
    std::vector<SynapseCreationEvent> pendingCreations;
    std::vector<PruningEvent> pendingRemovals;
    std::vector<NeuronCreationEvent> pendingNeuronCreations;
    std::vector<SynapseCreationEvent> completedCreations;
    std::vector<PruningEvent> completedRemovals;
    std::vector<NeuronCreationEvent> completedNeuronCreations;
    size_t nextSynapseId;
    size_t nextNeuronId;
    float globalExcitatoryRatio;
    size_t totalSynapseCount;
    size_t totalNeuronCount;
    std::unordered_map<NeuronId, float> neuronActivity;
    std::unordered_map<SynapseId, float> synapseWeights;
    
    Impl() : simulationTime(0.0f), timeSinceLastScaling(0.0f),
             nextSynapseId(1), nextNeuronId(1),
             globalExcitatoryRatio(0.5f), totalSynapseCount(0), totalNeuronCount(0) {
        parameters = StructuralPlasticityParameters();
    }
};

EnhancedStructuralPlasticity::EnhancedStructuralPlasticity() : pImpl(new Impl()) {}

EnhancedStructuralPlasticity::~EnhancedStructuralPlasticity() {
    delete pImpl;
}

SynapseId EnhancedStructuralPlasticity::createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                                                   SynapticWeight weight) {
    if (!brain) return INVALID_SYNAPSE_ID;
    
    SynapseId newId = pImpl->nextSynapseId++;
    float currentTime = pImpl->simulationTime;
    
    if (!brain->hasNeuron(source) || !brain->hasNeuron(destination)) {
        return INVALID_SYNAPSE_ID;
    }
    
    float distance = computeDistance(brain, source, destination);
    if (distance > pImpl->parameters.maxDistance) {
        weight *= (1.0f - pImpl->parameters.connectionCost);
    }
    
    SynapseCreationEvent event(newId, source, destination, weight, currentTime);
    pImpl->pendingCreations.push_back(event);
    pImpl->totalSynapseCount++;
    
    return newId;
}

bool EnhancedStructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain) return false;
    
    PruningEvent event(synapse, 0.0f, -1.0f, pImpl->simulationTime);
    pImpl->pendingRemovals.push_back(event);
    pImpl->totalSynapseCount--;
    
    return true;
}

NeuronId EnhancedStructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    if (!brain) return INVALID_NEURON_ID;
    
    NeuronId newId = pImpl->nextNeuronId++;
    RegionId region = getOptimalRegionForNewNeuron(brain, type);
    
    float initialExcitability = computeInitialExcitability(type);
    
    NeuronCreationEvent event(newId, type, region, pImpl->simulationTime, initialExcitability);
    pImpl->pendingNeuronCreations.push_back(event);
    pImpl->totalNeuronCount++;
    
    return newId;
}

bool EnhancedStructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain) return false;
    
    auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuron));
    if (region) {
        for (auto& synapse : region->getSynapses()) {
            if (synapse->getSourceNeuron() == neuron || 
                synapse->getDestinationNeuron() == neuron) {
                return false;
            }
        }
    }
    
    brain->removeNeuron(neuron);
    pImpl->totalNeuronCount--;
    
    return true;
}

void EnhancedStructuralPlasticity::processSynapseCreations(Brain* brain, RandomGenerator& rng) {
    for (auto& creation : pImpl->pendingCreations) {
        if (isNeuronMature(brain, creation.source) && isNeuronMature(brain, creation.target)) {
            brain->createSynapse(creation.source, creation.target, creation.weight);
            
            creation.mature = (pImpl->parameters.developmentalStage > 0.5f);
            creation.efficacy = computeInitialEfficacy(creation.weight, creation.source, creation.target);
            
            pImpl->completedCreations.push_back(creation);
        }
    }
    
    pImpl->pendingCreations.clear();
}

void EnhancedStructuralPlasticity::processSynapseRemovals(Brain* brain) {
    for (auto& removal : pImpl->pendingRemovals) {
        brain->removeSynapse(removal.synapseId);
        pImpl->completedRemovals.push_back(removal);
    }
    
    pImpl->pendingRemovals.clear();
}

void EnhancedStructuralPlasticity::processNeuronCreations(Brain* brain, RandomGenerator& rng) {
    for (auto& creation : pImpl->pendingNeuronCreations) {
        if (pImpl->parameters.developmentalStage > 0.5f || 
            rng.uniformReal(0.0f, 1.0f) < pImpl->parameters.neuronCreationRate) {
            
            brain->createNeuron(creation.type, creation.region, creation.initialExcitability);
            creation.mature = false;
            creation.maturationTime = pImpl->parameters.neuronMaturationTime;
            
            pImpl->completedNeuronCreations.push_back(creation);
        }
    }
    
    pImpl->pendingNeuronCreations.clear();
}

void EnhancedStructuralPlasticity::processNeuronPruning(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    std::vector<NeuronId> neuronsToPrune;
    
    for (RegionId regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& neuron : region->getAllNeurons()) {
            NeuronId neuronId = neuron->getId();
            
            if (shouldPruneNeuron(brain, neuronId, rng)) {
                neuronsToPrune.push_back(neuronId);
            }
        }
    }
    
    for (NeuronId neuronId : neuronsToPrune) {
        removeNeuron(brain, neuronId);
    }
}

float EnhancedStructuralPlasticity::computeDistance(Brain* brain, NeuronId neuron1, NeuronId neuron2) {
    if (!brain) return 0.0f;
    
    auto* region1 = brain->getRegion(brain->getRegionIdForNeuron(neuron1));
    auto* region2 = brain->getRegion(brain->getRegionIdForNeuron(neuron2));
    
    if (!region1 || !region2) return 1000.0f;
    
    auto* neuronA = region1->getNeuron(neuron1);
    auto* neuronB = region2->getNeuron(neuron2);
    
    if (!neuronA || !neuronB) return 1000.0f;
    
    float dx = neuronA->getX() - neuronB->getX();
    float dy = neuronA->getY() - neuronB->getY();
    return std::sqrt(dx*dx + dy*dy);
}

RegionId EnhancedStructuralPlasticity::getOptimalRegionForNewNeuron(Brain* brain, NeuronType type) {
    if (!brain) return INVALID_REGION_ID;
    
    std::vector<RegionId> regionIds = brain->getRegionIds();
    if (!regionIds.empty()) {
        return regionIds[0];
    }
    
    return brain->addRegion();
}

float EnhancedStructuralPlasticity::computeInitialExcitability(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return 1.0f;
        case NeuronType::Inhibitory: return 0.5f;
        case NeuronType::Sensory: return 0.8f;
        case NeuronType::Motor: return 0.7f;
        case NeuronType::Internal: return 0.6f;
        default: return 0.5f;
    }
}

float EnhancedStructuralPlasticity::computeInitialEfficacy(SynapticWeight weight, NeuronId source, NeuronId target) {
    float efficacy = std::abs(weight);
    
    if (weight > 0.0f) {
        efficacy *= 1.2f;
    } else {
        efficacy *= 0.8f;
    }
    
    return std::clamp(efficacy, 0.0f, 2.0f);
}

bool EnhancedStructuralPlasticity::shouldPruneNeuron(Brain* brain, NeuronId neuronId, RandomGenerator& rng) {
    if (!brain) return false;
    
    float activity = getNeuronActivity(neuronId);
    
    if (activity < pImpl->parameters.neuronActivityThreshold) {
        return true;
    }
    
    float metabolicCost = pImpl->parameters.metabolicCost;
    float expectedBenefit = computeNeuronExpectedBenefit(brain, neuronId);
    
    if (metabolicCost > expectedBenefit && rng.uniformReal(0.0f, 1.0f) < 0.3f) {
        return true;
    }
    
    return false;
}

float EnhancedStructuralPlasticity::getNeuronActivity(NeuronId neuronId) {
    auto it = pImpl->neuronActivity.find(neuronId);
    if (it != pImpl->neuronActivity.end()) {
        return it->second;
    }
    return 0.0f;
}

float EnhancedStructuralPlasticity::computeNeuronExpectedBenefit(Brain* brain, NeuronId neuronId) {
    float benefit = 0.0f;
    
    if (!brain) return benefit;
    
    auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuronId));
    if (!region) return benefit;
    
    for (auto& synapse : region->getSynapses()) {
        if (synapse->getSourceNeuron() == neuronId || 
            synapse->getDestinationNeuron() == neuronId) {
            
            float weight = synapse->getWeight();
            float activity = getNeuronActivity(synapse->getSourceNeuron()) + 
                           getNeuronActivity(synapse->getDestinationNeuron());
            
            benefit += std::abs(weight) * activity;
        }
    }
    
    return benefit;
}

bool EnhancedStructuralPlasticity::isNeuronMature(Brain* brain, NeuronId neuronId) {
    if (!brain) return false;
    
    auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuronId));
    if (!region) return false;
    
    auto* neuron = region->getNeuron(neuronId);
    if (!neuron) return false;
    
    float activity = getNeuronActivity(neuronId);
    return activity > pImpl->parameters.activityThreshold;
}

void EnhancedStructuralPlasticity::applySynapticScaling(Brain* brain) {
    if (!brain) return;
    
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    for (RegionId regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& synapse : region->getSynapses()) {
            totalWeight += std::abs(synapse->getWeight());
            synapseCount++;
        }
    }
    
    if (synapseCount == 0) return;
    
    float averageWeight = totalWeight / synapseCount;
    
    for (RegionId regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& synapse : region->getSynapses()) {
            float currentWeight = synapse->getWeight();
            float scaledWeight = currentWeight + 
                               (averageWeight - currentWeight) * 
                               pImpl->parameters.weightNormalizationFactor;
            
            scaledWeight = std::clamp(scaledWeight, 
                                     pImpl->parameters.strengthThreshold,
                                     pImpl->parameters.redundancyThreshold);
            
            synapse->setWeight(scaledWeight);
        }
    }
}

void EnhancedStructuralPlasticity::updateNetworkStatistics(Brain* brain) {
    if (!brain) return;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (RegionId regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (auto& synapse : region->getSynapses()) {
            if (synapse->isExcitatory()) {
                totalExcitatory += std::abs(synapse->getWeight());
            } else if (synapse->isInhibitory()) {
                totalInhibitory += std::abs(synapse->getWeight());
            }
        }
    }
    
    float total = totalExcitatory + totalInhibitory;
    if (total > 0.0f) {
        pImpl->globalExcitatoryRatio = totalExcitatory / total;
    }
}

void EnhancedStructuralPlasticity::updateParametersForDevelopment() {
    float stage = pImpl->parameters.developmentalStage;
    
    if (stage < 0.3f) {
        pImpl->parameters.synaptogenesisRate *= (1.0f + stage * 2.0f);
        pImpl->parameters.pruningRate *= (1.0f - stage);
        pImpl->parameters.neuronCreationRate *= (1.0f + stage * 1.5f);
    } else if (stage < 0.7f) {
        pImpl->parameters.synaptogenesisRate *= 0.8f;
        pImpl->parameters.pruningRate *= 1.2f;
        pImpl->parameters.neuronCreationRate *= 0.8f;
    } else {
        pImpl->parameters.synaptogenesisRate *= 0.5f;
        pImpl->parameters.pruningRate *= 1.5f;
        pImpl->parameters.neuronCreationRate *= 0.5f;
    }
    
    pImpl->parameters.synaptogenesisRate *= pImpl->parameters.plasticityModulation;
    pImpl->parameters.pruningRate *= pImpl->parameters.plasticityModulation;
}

} // namespace nlm
