#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float developmentalInfluence;
    float homeostaticTarget;
    float minActivityThreshold;
    float maxSynapsesPerNeuron;
    size_t maxTotalSynapses;
    float targetConnectionProbability;
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t synapsesCreatedThisStep;
    size_t activeConnections;
    float formationEfficiency;
    
    // Activity history for activity-dependent synaptogenesis
    std::unordered_map<NeuronId, float> neuronActivityHistory;
    double activityUpdateTime;
    
    Impl() 
        : formationRate(0.001f), targetDensity(0.1f), developmentalInfluence(1.0f), 
          homeostaticTarget(0.1f), minActivityThreshold(0.1f),
          maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
          targetConnectionProbability(0.1f),
          totalSynapsesCreated(0), synapsesCreatedThisStep(0),
          activeConnections(0), formationEfficiency(0.0f),
          activityUpdateTime(0.0), neuronActivityHistory() {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::clamp(rate, 0.0f, 0.1f);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.0f, 0.5f);
    pImpl->homeostaticTarget = pImpl->targetDensity;
}

float Synaptogenesis::getDevelopmentalInfluence() const {
    return pImpl->developmentalInfluence;
}

void Synaptogenesis::setDevelopmentalInfluence(float factor) {
    pImpl->developmentalInfluence = std::clamp(factor, 0.0f, 2.0f);
}

float Synaptogenesis::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Synaptogenesis::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, 0.0f, 0.5f);
}

float Synaptogenesis::getCurrentDensity(Brain* brain) const {
    if (!brain) return 0.0f;
    
    size_t totalNeurons = 0;
    size_t totalPossibleSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        totalNeurons += region->getTotalNeuronCount();
        if (totalNeurons >= 2) {
            totalPossibleSynapses += totalNeurons * (totalNeurons - 1);
        }
    }
    
    if (totalPossibleSynapses == 0) return 0.0f;
    
    size_t actualSynapses = brain->getTotalSynapseCount();
    return static_cast<float>(actualSynapses) / static_cast<float>(totalPossibleSynapses);
}

bool Synaptogenesis::needsSynaptogenesis(Brain* brain) const {
    if (!brain) return false;
    
    float currentDensity = getCurrentDensity(brain);
    float target = pImpl->homeostaticTarget * pImpl->developmentalInfluence;
    
    // Need synaptogenesis if density is below target with some tolerance
    return currentDensity < (target * 0.9f);
}

SynapseId Synaptogenesis::createActivityDependentSynapse(Brain* brain, 
                                                         NeuronId source, NeuronId destination,
                                                         float sourceActivity, float targetActivity,
                                                         RandomGenerator& rng) {
    if (!brain || source == destination) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        // Check if destination neuron is in this region
        bool destinationFound = false;
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == destination) {
                    destinationFound = true;
                    break;
                }
            }
            if (destinationFound) break;
        }
        
        if (!destinationFound) continue;
        
        // Check if source neuron exists in this region
        bool sourceFound = false;
        NeuronId sourceId = INVALID_NEURON_ID;
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == source) {
                    sourceFound = true;
                    sourceId = source;
                    break;
                }
            }
            if (sourceFound) break;
        }
        
        if (!sourceFound) continue;
        
        // Check if synapse already exists
        auto synapsesTo = region->getSynapsesTo(destination);
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check synapse limits
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region
        }
        
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            continue;
        }
        
        // Activity-dependent weight based on pre- and post-synaptic activity
        // Higher activity correlation = stronger initial weight
        float activityCorrelation = 0.0f;
        if (sourceActivity > 0.0f && targetActivity > 0.0f) {
            activityCorrelation = std::min(1.0f, (sourceActivity + targetActivity) / 2.0f);
        }
        
        // Base weight with activity modulation
        float baseWeight = 0.1f;
        float activityWeight = activityCorrelation * 0.5f;
        float weightVariation = rng.uniformReal(-0.05f, 0.05f);
        
        float weight = baseWeight + activityWeight + weightVariation;
        weight = std::clamp(weight, 0.01f, 1.0f);
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            ++pImpl->synapsesCreatedThisStep;
            ++pImpl->activeConnections;
            
            NLM_LOG_DEBUG("Created activity-dependent synapse " + std::to_string(synId.index()) +
                         " from neuron " + std::to_string(source.index()) + " to neuron " +
                         std::to_string(destination.index()) + " with weight " +
                         std::to_string(weight) + " (activity correlation: " +
                         std::to_string(activityCorrelation) + ")");
            
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

size_t Synaptogenesis::getTotalSynapsesCreated() const {
    return pImpl->totalSynapsesCreated;
}

size_t Synaptogenesis::getSynapsesCreatedThisStep() const {
    return pImpl->synapsesCreatedThisStep;
}

float Synaptogenesis::getFormationEfficiency() const {
    return pImpl->formationEfficiency;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    pImpl->synapsesCreatedThisStep = 0;
    
    if (!brain) return;
    
    NLM_LOG_DEBUG("Synaptogenesis update: Current density = " + 
                 std::to_string(getCurrentDensity(brain)) + ", Target = " +
                 std::to_string(pImpl->homeostaticTarget * pImpl->developmentalInfluence));
    
    // Update activity history with recent spikes
    double currentTime = 0.0;  // TODO: Get from brain simulation time
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            const auto& spikes = neuron->getSpikeHistory();
            float recentActivity = 0.0f;
            
            if (!spikes.empty()) {
                // Calculate activity based on recent spikes
                recentActivity = static_cast<float>(spikes.size());
                
                // Update activity history (exponential decay)
                float decayFactor = 0.9f;
                if (pImpl->neuronActivityHistory.find(neuron->getId()) != pImpl->neuronActivityHistory.end()) {
                    pImpl->neuronActivityHistory[neuron->getId()] = 
                        pImpl->neuronActivityHistory[neuron->getId()] * decayFactor + recentActivity * (1.0f - decayFactor);
                } else {
                    pImpl->neuronActivityHistory[neuron->getId()] = recentActivity;
                }
            }
        }
    }
    
    // Check if we need to form new synapses
    if (!needsSynaptogenesis(brain)) {
        NLM_LOG_DEBUG("Synaptogenesis: Network at target density, skipping synaptogenesis");
        pImpl->formationEfficiency = 1.0f;  // Efficient - no need for new synapses
        return;
    }
    
    // Activity-dependent synaptogenesis
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect neuron activities
        std::vector<std::pair<NeuronId, float>> neuronActivities;
        neuronActivities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            
            // Get activity from history or recent spikes
            auto it = pImpl->neuronActivityHistory.find(neuron->getId());
            if (it != pImpl->neuronActivityHistory.end()) {
                activity = it->second;
            } else {
                const auto& spikes = neuron->getSpikeHistory();
                activity = static_cast<float>(spikes.size());
            }
            
            neuronActivities.push_back({neuron->getId(), activity});
        }
        
        // Perform activity-dependent synaptogenesis
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            // For each pair of neurons, potentially create a new synapse
            for (size_t i = 0; i < neuronCount; ++i) {
                for (size_t j = i + 1; j < neuronCount; ++j) {
                    // Skip if both neurons are inactive
                    if (neuronActivities[i].second < pImpl->minActivityThreshold && 
                        neuronActivities[j].second < pImpl->minActivityThreshold) {
                        continue;
                    }
                    
                    // Probabilistic synapse formation based on formation rate and development
                    float effectiveRate = pImpl->formationRate * pImpl->developmentalInfluence;
                    effectiveRate = std::min(effectiveRate, 1.0f);
                    
                    if (!rng.bernoulli(effectiveRate)) {
                        continue;
                    }
                    
                    // Create activity-dependent synapse
                    SynapseId synId = createActivityDependentSynapse(
                        brain,
                        neuronActivities[i].first,
                        neuronActivities[j].first,
                        neuronActivities[i].second,
                        neuronActivities[j].second,
                        rng
                    );
                    
                    if (synId != INVALID_SYNAPSE_ID) {
                        // Successfully created a synapse
                        break;
                    }
                }
                
                if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
                    break;
                }
            }
        }
    }
    
    // Calculate formation efficiency
    float targetSynapses = static_cast<float>(brain->getTotalNeuronCount()) * 
                          static_cast<float>(brain->getTotalNeuronCount() - 1) * 
                          pImpl->targetConnectionProbability * pImpl->developmentalInfluence;
    
    if (targetSynapses > 0.0f) {
        pImpl->formationEfficiency = std::min(1.0f, 
            static_cast<float>(pImpl->synapsesCreatedThisStep) / targetSynapses);
    } else {
        pImpl->formationEfficiency = 1.0f;
    }
    
    NLM_LOG_INFO("Synaptogenesis complete: Created " + std::to_string(pImpl->synapsesCreatedThisStep) +
                 " synapses, efficiency = " + std::to_string(pImpl->formationEfficiency));
}

} // namespace nlm
