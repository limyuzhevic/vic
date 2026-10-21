#include "ActivityDependentSynaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct ActivityDependentSynaptogenesis::Impl {
    // Base parameters
    float synaptogenesisRate;     // Base synaptogenesis rate
    float activityThreshold;      // Minimum activity for synapse formation
    float coactivityWindow;       // Time window for coactivity (ms)
    size_t maxSynapsesPerPair;    // Maximum synapses per neuron pair
    float activityDependencyWeight; // Weight of activity dependence
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesRemoved;
    
    Impl() : synaptogenesisRate(0.0001f), activityThreshold(0.1f), coactivityWindow(100.0f),
             maxSynapsesPerPair(10), activityDependencyWeight(0.5f),
             totalSynapsesCreated(0), totalSynapsesRemoved(0) {}
};

ActivityDependentSynaptogenesis::ActivityDependentSynaptogenesis() : pImpl(new Impl) {}

ActivityDependentSynaptogenesis::~ActivityDependentSynaptogenesis() = default;

SynapseId ActivityDependentSynaptogenesis::createSynapse(Brain* brain, NeuronId source, 
                                                NeuronId destination, SynapticWeight weight) {
    if (!brain) return INVALID_SYNAPSE_ID;
    
    // Calculate coactivity between neurons
    float coactivity = calculateCoactivity(brain, source, destination);
    
    // Determine if synapse should be created
    if (!shouldCreateSynapse(brain, source, destination)) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Check if connection already exists
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check maximum synapses per pair
        auto synapsesFrom = region->getSynapsesFrom(source);
        size_t synapseCount = 0;
        for (Synapse* syn : synapsesFrom) {
            if (syn->getDestinationNeuron() == destination) {
                ++synapseCount;
            }
        }
        
        if (synapseCount >= pImpl->maxSynapsesPerPair) {
            continue;  // Skip this region
        }
        
        // Create the synapse with activity-dependent weight
        float activityBasedWeight = weight * (1.0f + coactivity * pImpl->activityDependencyWeight);
        SynapseId synId = region->addSynapse(source, destination, activityBasedWeight, 1);
        
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool ActivityDependentSynaptogenesis::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) return false;
    
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesRemoved;
            return true;
        }
    }
    
    return false;
}

bool ActivityDependentSynaptogenesis::removeNeuron(Brain* brain, NeuronId neuron) {
    // Simplified implementation - zero out all synapses to/from the neuron
    if (!brain) return false;
    
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(neuron);
        auto synapsesFrom = region->getSynapsesFrom(neuron);
        
        for (Synapse* syn : synapsesTo) {
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesRemoved;
        }
        
        for (Synapse* syn : synapsesFrom) {
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesRemoved;
        }
    }
    
    return true;
}

void ActivityDependentSynaptogenesis::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

float ActivityDependentSynaptogenesis::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void ActivityDependentSynaptogenesis::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float ActivityDependentSynaptogenesis::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void ActivityDependentSynaptogenesis::setCoactivityWindow(float window) {
    pImpl->coactivityWindow = std::clamp(window, 1.0f, 1000.0f);
}

float ActivityDependentSynaptogenesis::getCoactivityWindow() const {
    return pImpl->coactivityWindow;
}

void ActivityDependentSynaptogenesis::setMaxSynapsesPerPair(size_t max) {
    pImpl->maxSynapsesPerPair = std::max<size_t>(1, max);
}

size_t ActivityDependentSynaptogenesis::getMaxSynapsesPerPair() const {
    return pImpl->maxSynapsesPerPair;
}

void ActivityDependentSynaptogenesis::setActivityDependencyWeight(float weight) {
    pImpl->activityDependencyWeight = std::clamp(weight, 0.0f, 5.0f);
}

float ActivityDependentSynaptogenesis::getActivityDependencyWeight() const {
    return pImpl->activityDependencyWeight;
}

float ActivityDependentSynaptogenesis::calculateCoactivity(Brain* brain, NeuronId source, NeuronId destination) const {
    if (!brain) return 0.0f;
    
    // Find regions containing both neurons
    for (auto& region : brain->getRegions()) {
        // Get neurons
        Neuron* srcNeuron = nullptr;
        Neuron* dstNeuron = nullptr;
        
        for (Neuron* neuron : region->getAllNeurons()) {
            if (neuron->getId() == source) srcNeuron = neuron;
            if (neuron->getId() == destination) dstNeuron = neuron;
        }
        
        if (!srcNeuron || !dstNeuron) continue;
        
        // Calculate coactivity based on spike history
        const auto& srcSpikes = srcNeuron->getSpikeHistory();
        const auto& dstSpikes = dstNeuron->getSpikeHistory();
        
        if (srcSpikes.empty() || dstSpikes.empty()) return 0.0f;
        
        // Count spikes within coactivity window
        size_t coincidences = 0;
        for (Timestamp srcTime : srcSpikes) {
            for (Timestamp dstTime : dstSpikes) {
                if (std::abs(srcTime - dstTime) < pImpl->coactivityWindow) {
                    ++coincidences;
                }
            }
        }
        
        // Normalize by activity
        float srcActivity = static_cast<float>(srcSpikes.size());
        float dstActivity = static_cast<float>(dstSpikes.size());
        float totalActivity = srcActivity + dstActivity;
        
        return (totalActivity > 0.0f) ? static_cast<float>(coincidences) / totalActivity : 0.0f;
    }
    
    return 0.0f;
}

bool ActivityDependentSynaptogenesis::shouldCreateSynapse(Brain* brain, NeuronId source, NeuronId destination) const {
    if (!brain) return false;
    
    // Calculate activities of both neurons
    float sourceActivity = 0.0f;
    float destActivity = 0.0f;
    
    for (auto& region : brain->getRegions()) {
        for (Neuron* neuron : region->getAllNeurons()) {
            if (neuron->getId() == source) {
                sourceActivity = static_cast<float>(neuron->getSpikeHistory().size());
            }
            if (neuron->getId() == destination) {
                destActivity = static_cast<float>(neuron->getSpikeHistory().size());
            }
        }
    }
    
    // Check if activities exceed thresholds
    if (sourceActivity < pImpl->activityThreshold || destActivity < pImpl->activityThreshold) {
        return false;
    }
    
    // Check probabilistic synaptogenesis
    // In a real implementation, this would use a random number generator
    return true;
}

} // namespace nlm