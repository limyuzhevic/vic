#include "Synaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float minWeight;
    float activityThreshold;
    size_t maxSynapsesPerNeuron;
    
    // Statistics
    size_t totalSynapsesCreated;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), minWeight(0.01f), 
             activityThreshold(0.1f), maxSynapsesPerNeuron(50),
             totalSynapsesCreated(0) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::clamp(rate, 0.0f, 0.01f);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.01f, 0.5f);
}

float Synaptogenesis::getMinWeight() const {
    return pImpl->minWeight;
}

void Synaptogenesis::setMinWeight(float weight) {
    pImpl->minWeight = std::clamp(weight, 0.0f, 1.0f);
}

float Synaptogenesis::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void Synaptogenesis::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

size_t Synaptogenesis::getMaxSynapsesPerNeuron() const {
    return pImpl->maxSynapsesPerNeuron;
}

void Synaptogenesis::setMaxSynapsesPerNeuron(size_t max) {
    pImpl->maxSynapsesPerNeuron = std::max<size_t>(max, 1);
}

size_t Synaptogenesis::getTotalSynapsesCreated() const {
    return pImpl->totalSynapsesCreated;
}

void Synaptogenesis::resetStats() {
    pImpl->totalSynapsesCreated = 0;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current synapse density
    size_t totalPossibleSynapses = 0;
    size_t totalExistingSynapses = 0;
    
    for (auto& region : brain->getRegions()) {
        size_t neuronsInRegion = region->getTotalNeuronCount();
        if (neuronsInRegion >= 2) {
            totalPossibleSynapses += neuronsInRegion * (neuronsInRegion - 1);
            totalExistingSynapses += region->getSynapseCount();
        }
    }
    
    // If density is below target, create new synapses
    if (totalPossibleSynapses > 0) {
        float currentDensity = static_cast<float>(totalExistingSynapses) / totalPossibleSynapses;
        
        if (currentDensity < pImpl->targetDensity) {
            // Collect neuron activity statistics
            struct NeuronActivity {
                NeuronId id;
                float activityScore;  // Combined measure of recent firing and membrane potential
                size_t outgoingSynapseCount;
            };
            
            std::vector<NeuronActivity> neuronActivities;
            
            for (auto& region : brain->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    const auto& state = neuron->getState();
                    float activityScore = 0.0f;
                    
                    // Base activity on firing history
                    const auto& spikes = neuron->getSpikeHistory();
                    if (!spikes.empty()) {
                        activityScore += static_cast<float>(spikes.size()) * 0.5f;
                    }
                    
                    // Add membrane potential proximity to threshold
                    float potentialDelta = std::abs(state.membranePotential - state.threshold);
                    if (potentialDelta < 20.0f) {  // Near threshold
                        activityScore += (20.0f - potentialDelta) / 20.0f * 2.0f;
                    }
                    
                    // Get outgoing synapse count
                    auto outgoing = region->getSynapsesFrom(neuron->getId());
                    size_t outgoingCount = outgoing.size();
                    
                    neuronActivities.push_back({neuron->getId(), activityScore, outgoingCount});
                }
            }
            
            // Create new synapses between active neurons
            size_t neuronsAttempted = neuronActivities.size();
            for (size_t attempt = 0; attempt < neuronsAttempted; ++attempt) {
                // Probabilistic synapse formation
                if (rng.bernoulli(pImpl->formationRate)) {
                    // Select source neuron based on activity (more active neurons are better sources)
                    size_t sourceIdx = selectNeuronByActivity(neuronActivities);
                    if (sourceIdx == (size_t)-1) continue;
                    
                    size_t destIdx = selectNeuronByActivity(neuronActivities);
                    if (destIdx == (size_t)-1 || sourceIdx == destIdx) continue;
                    
                    NeuronId source = neuronActivities[sourceIdx].id;
                    NeuronId dest = neuronActivities[destIdx].id;
                    
                    // Check if synapse already exists and if we have room
                    bool synapseExists = false;
                    for (auto& region : brain->getRegions()) {
                        auto synapsesTo = region->getSynapsesTo(dest);
                        for (Synapse* syn : synapsesTo) {
                            if (syn->getSourceNeuron() == source) {
                                synapseExists = true;
                                break;
                            }
                        }
                        if (synapseExists) break;
                    }
                    
                    if (!synapseExists && neuronActivities[sourceIdx].outgoingSynapseCount < pImpl->maxSynapsesPerNeuron) {
                        // Create new synapse with weight based on activity
                        float weight = calculateInitialWeight(neuronActivities[sourceIdx], neuronActivities[destIdx]);
                        if (weight > pImpl->minWeight) {
                            SynapseId newSynapseId = createSynapseInBrain(brain, source, dest, weight);
                            if (newSynapseId != INVALID_SYNAPSE_ID) {
                                ++pImpl->totalSynapsesCreated;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Synaptogenesis::finalizeDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Complete synaptogenesis phase
    // Ensure minimum viable connectivity for each region
    for (auto& region : brain->getRegions()) {
        size_t neuronCount = region->getTotalNeuronCount();
        size_t currentSynapses = region->getSynapseCount();
        
        // Aim for at least 2% connectivity
        size_t targetSynapses = static_cast<size_t>(neuronCount * neuronCount * 0.02f);
        
        if (currentSynapses < targetSynapses) {
            // This would be implemented in a real scenario
            // For now, log that synaptogenesis should continue
            NLM_LOG_INFO("Region needs more synapses: " + std::to_string(currentSynapses) + 
                        "/" + std::to_string(targetSynapses));
        }
    }
}

NeuronId Synaptogenesis::createSynapseInBrain(Brain* brain, NeuronId source, 
                                               NeuronId destination, SynapticWeight weight) {
    if (!brain) {
        return INVALID_NEURON_ID;
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
        
        // Check max outgoing synapses per neuron
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region (though typically only one region contains both)
        }
        
        // Create the synapse
        return region->addSynapse(source, destination, weight, 1);
    }
    
    return INVALID_SYNAPSE_ID;
}

NeuronId Synaptogenesis::selectNeuronByActivity(std::vector<NeuronActivity>& activities) {
    if (activities.empty()) return (size_t)-1;
    
    // Weight selection by activity score
    float totalWeight = 0.0f;
    for (const auto& neuron : activities) {
        totalWeight += neuron.activityScore;
    }
    
    if (totalWeight <= 0.0f) {
        // All neurons have no activity, select randomly
        size_t idx = rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
        return idx;
    }
    
    // Weighted random selection
    float threshold = rng.uniformReal(0.0f, totalWeight);
    float accumulated = 0.0f;
    
    for (size_t i = 0; i < activities.size(); ++i) {
        accumulated += activities[i].activityScore;
        if (threshold <= accumulated) {
            return i;
        }
    }
    
    return activities.size() - 1;  // Fallback
}

float Synaptogenesis::calculateInitialWeight(const NeuronActivity& source, const NeuronActivity& dest) {
    // Weight based on source activity and target suitability
    float baseWeight = 0.05f + (source.activityScore / 10.0f) * 0.2f;
    
    // Adjust based on destination activity (lower destination activity might need stronger initial weight)
    if (dest.activityScore < pImpl->activityThreshold) {
        baseWeight *= 1.5f;  // Boost weight for less active targets
    }
    
    // Clamp to reasonable range
    return std::clamp(baseWeight, pImpl->minWeight, 1.0f);
}

} // namespace nlm