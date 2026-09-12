#include "Synaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float homeostaticGain;           // Gain for homeostatic regulation
    float activityFactor;            // Factor for activity-dependent formation
    float temporalConstant;          // Time constant for homeostatic adaptation
    
    // State variables
    float currentDensity;            // Current synapse density
    float targetMet;                 // Whether target density has been met
    float homeostaticDrive;          // Homeostatic drive to maintain target density
    
    // Statistics
    size_t synapsesFormedThisStep;
    size_t homeostaticAdjustments;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), homeostaticGain(0.01f),
             activityFactor(0.5f), temporalConstant(100.0f),
             currentDensity(0.0f), targetMet(false), homeostaticDrive(0.0f),
             synapsesFormedThisStep(0), homeostaticAdjustments(0) {}
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
    // Real synaptogenesis implementation with activity-dependent formation and homeostatic regulation
    if (!brain) return;
    
    pImpl->synapsesFormedThisStep = 0;
    
    // Calculate current synapse density across all regions
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getSynapseCount();
    }
    
    if (totalNeurons > 0) {
        pImpl->currentDensity = static_cast<float>(totalSynapses) / static_cast<float>(totalNeurons);
    }
    
    // Check if target density has been reached
    pImpl->targetMet = (pImpl->currentDensity >= pImpl->targetDensity);
    
    // Update homeostatic drive
    float densityError = pImpl->targetDensity - pImpl->currentDensity;
    pImpl->homeostaticDrive += (densityError - pImpl->homeostaticDrive) * (1.0f / pImpl->temporalConstant);
    
    // Activity-dependent synaptogenesis
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect neuron activities and available targets
        struct NeuronActivity {
            NeuronId id;
            float activity;
            size_t outgoingSynapses;
            size_t maxOutgoing;
        };
        
        std::vector<NeuronActivity> neuronActivities;
        neuronActivities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            size_t outgoing = 0;
            
            // Get outgoing synapse count
            auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
            outgoing = outgoingSynapses.size();
            
            // Get activity from spike history (more recent spikes = higher activity)
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // Weight recent spikes more heavily
                activity = static_cast<float>(spikes.size()) * pImpl->activityFactor;
                if (spikes.size() > 1) {
                    float timeSinceLast = spikes.back() - spikes[spikes.size() - 2];
                    activity *= std::exp(-timeSinceLast / 50.0f);  // Decay over time
                }
            }
            
            neuronActivities.push_back({
                neuron->getId(), 
                activity, 
                outgoing,
                pImpl->maxSynapsesPerNeuron
            });
        }
        
        // Form synapses probabilistically
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            for (size_t i = 0; i < neuronCount; ++i) {
                // Skip if we have too many outgoing synapses
                if (neuronActivities[i].outgoing >= neuronActivities[i].maxOutgoing) {
                    continue;
                }
                
                // Check if target density has been met
                if (pImpl->targetMet && rng.bernoulli(0.01f)) {
                    // Make small adjustments even when at target
                    continue;
                }
                
                // Probabilistic synapse formation with multiple factors
                float formationProb = pImpl->formationRate;
                
                // Modulate by source neuron activity
                formationProb *= (1.0f + neuronActivities[i].activity * 2.0f);
                
                // Modulate by homeostatic drive
                formationProb *= (1.0f + pImpl->homeostaticDrive);
                
                // Modulate by available target neurons
                size_t availableTargets = 0;
                for (size_t j = 0; j < neuronCount; ++j) {
                    if (i != j && neuronActivities[j].outgoing < neuronActivities[j].maxOutgoing) {
                        ++availableTargets;
                    }
                }
                
                if (availableTargets > 0) {
                    formationProb *= static_cast<float>(availableTargets) / neuronCount;
                }
                
                // Consider if we should form a synapse
                if (rng.bernoulli(formationProb)) {
                    // Select random target neuron
                    std::vector<size_t> validTargets;
                    for (size_t j = 0; j < neuronCount; ++j) {
                        if (i != j && neuronActivities[j].outgoing < neuronActivities[j].maxOutgoing) {
                            validTargets.push_back(j);
                        }
                    }
                    
                    if (!validTargets.empty()) {
                        size_t targetIdx = validTargets[rng.pickRandomIndex(validTargets.size())];
                        NeuronId src = neuronActivities[i].id;
                        NeuronId dst = neuronActivities[targetIdx].id;
                        
                        // Initial weight based on source activity
                        float initialWeight = 0.01f + neuronActivities[i].activity * 0.1f + rng.uniformReal(-0.05f, 0.05f);
                        initialWeight = std::clamp(initialWeight, -1.0f, 1.0f);
                        
                        // Create the synapse
                        SynapseId newSynId = region->addSynapse(src, dst, initialWeight, 1);
                        if (newSynId != INVALID_SYNAPSE_ID) {
                            ++pImpl->synapsesFormedThisStep;
                        }
                    }
                }
            }
        }
        
        // Apply homeostatic regulation if needed
        if (!pImpl->targetMet && pImpl->homeostaticDrive > 0.0f) {
            // Increase formation rate when below target
            float homeostaticAdjustment = pImpl->homeostaticGain * pImpl->homeostaticDrive;
            
            // Distribute additional formation opportunities
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                if (rng.bernoulli(homeostaticAdjustment)) {
                    // Try to create a synapse in a less-connected region
                    size_t srcIdx = rng.pickRandomIndex(neuronCount);
                    
                    // Find target with available capacity
                    std::vector<size_t> validTargets;
                    for (size_t j = 0; j < neuronCount; ++j) {
                        if (srcIdx != j && neuronActivities[j].outgoing < neuronActivities[j].maxOutgoing) {
                            validTargets.push_back(j);
                        }
                    }
                    
                    if (!validTargets.empty()) {
                        size_t targetIdx = validTargets[rng.pickRandomIndex(validTargets.size())];
                        NeuronId src = neuronActivities[srcIdx].id;
                        NeuronId dst = neuronActivities[targetIdx].id;
                        
                        // Create with intermediate weight
                        float homeostaticWeight = 0.05f + rng.uniformReal(-0.02f, 0.02f);
                        SynapseId synId = region->addSynapse(src, dst, homeostaticWeight, 1);
                        if (synId != INVALID_SYNAPSE_ID) {
                            ++pImpl->homeostaticAdjustments;
                        }
                    }
                }
            }
        }
    }
}

} // namespace nlm
