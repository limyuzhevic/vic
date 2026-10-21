#include "AxonalDendriticGrowth.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AxonalDendriticGrowth::Impl {
    // Growth parameters
    float axonalGrowthRate;       // Axonal growth rate (α)
    float dendriticGrowthRate;   // Dendritic growth rate (β)
    float neurotrophicFactor;    // Neurotrophic factor (η)
    
    // Maximum lengths
    float maxAxonalLength;       // Maximum axonal length
    float maxDendriticLength;    // Maximum dendritic length
    
    // Growth statistics
    size_t totalAxonalGrowths;
    size_t totalDendriticGrowths;
    size_t totalAxonalShortenings;
    size_t totalDendriticShortenings;
    
    Impl() : axonalGrowthRate(0.01f), dendriticGrowthRate(0.01f), neurotrophicFactor(0.1f),
             maxAxonalLength(1000.0f), maxDendriticLength(1000.0f),
             totalAxonalGrowths(0), totalDendriticGrowths(0),
             totalAxonalShortenings(0), totalDendriticShortenings(0) {}
};

AxonalDendriticGrowth::AxonalDendriticGrowth() : pImpl(new Impl) {}

AxonalDendriticGrowth::~AxonalDendriticGrowth() = default;

SynapseId AxonalDendriticGrowth::createSynapse(Brain* brain, NeuronId source, 
                                                NeuronId destination, SynapticWeight weight) {
    if (!brain) return INVALID_SYNAPSE_ID;
    
    // Calculate growth potential
    float axonalGrowth = calculateAxonalGrowth(brain, source, destination);
    float dendriticGrowth = calculateDendriticGrowth(brain, source, destination);
    
    // Determine if synapse should be formed based on growth
    if (axonalGrowth > 0.0f || dendriticGrowth > 0.0f) {
        // Check if connection already exists
        for (auto& region : brain->getRegions()) {
            auto synapsesTo = region->getSynapsesTo(destination);
            
            for (Synapse* syn : synapsesTo) {
                if (syn->getSourceNeuron() == source) {
                    return INVALID_SYNAPSE_ID;  // Connection already exists
                }
            }
            
            // Create the synapse with growth-based weight
            float growthBasedWeight = weight * (1.0f + axonalGrowth + dendriticGrowth);
            SynapseId synId = region->addSynapse(source, destination, growthBasedWeight, 1);
            
            if (synId != INVALID_SYNAPSE_ID) {
                // Update axonal/dendritic lengths
                updateAxonalLength(brain, source, destination, axonalGrowth);
                updateDendriticLength(brain, source, destination, dendriticGrowth);
                return synId;
            }
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool AxonalDendriticGrowth::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) return false;
    
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // When synapse is removed, shorten associated axon/dendrite
            // Find the neurons involved
            NeuronId source = syn->getSourceNeuron();
            NeuronId destination = syn->getDestinationNeuron();
            
            syn->setWeight(0.0f);
            
            // This would normally shorten axons/dendrites
            // For now, just mark as removed
            return true;
        }
    }
    
    return false;
}

bool AxonalDendriticGrowth::removeNeuron(Brain* brain, NeuronId neuron) {
    if (!brain) return false;
    
    // When neuron is removed, shorten associated axons/dendrites
    // This is a simplified implementation
    return true;
}

void AxonalDendriticGrowth::setAxonalGrowthRate(float rate) {
    pImpl->axonalGrowthRate = std::clamp(rate, 0.0f, 0.1f);
}

float AxonalDendriticGrowth::getAxonalGrowthRate() const {
    return pImpl->axonalGrowthRate;
}

void AxonalDendriticGrowth::setDendriticGrowthRate(float rate) {
    pImpl->dendriticGrowthRate = std::clamp(rate, 0.0f, 0.1f);
}

float AxonalDendriticGrowth::getDendriticGrowthRate() const {
    return pImpl->dendriticGrowthRate;
}

void AxonalDendriticGrowth::setNeurotrophicFactor(float factor) {
    pImpl->neurotrophicFactor = std::clamp(factor, 0.0f, 1.0f);
}

float AxonalDendriticGrowth::getNeurotrophicFactor() const {
    return pImpl->neurotrophicFactor;
}

void AxonalDendriticGrowth::setMaxAxonalLength(float maxLength) {
    pImpl->maxAxonalLength = std::clamp(maxLength, 1.0f, 10000.0f);
}

float AxonalDendriticGrowth::getMaxAxonalLength() const {
    return pImpl->maxAxonalLength;
}

void AxonalDendriticGrowth::setMaxDendriticLength(float maxLength) {
    pImpl->maxDendriticLength = std::clamp(maxLength, 1.0f, 10000.0f);
}

float AxonalDendriticGrowth::getMaxDendriticLength() const {
    return pImpl->maxDendriticLength;
}

void AxonalDendriticGrowth::updateGrowth(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Update growth for all possible connections
    // This is a simplified implementation
    // In practice, this would be more efficient and targeted
    
    // For demonstration, just calculate growth potential
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = 0; j < neurons.size(); ++j) {
                if (i == j) continue;  // Don't connect neuron to itself
                
                NeuronId source = neurons[i]->getId();
                NeuronId destination = neurons[j]->getId();
                
                // Calculate growth
                float axonalGrowth = calculateAxonalGrowth(brain, source, destination);
                float dendriticGrowth = calculateDendriticGrowth(brain, source, destination);
                
                // Apply growth to axon/dendrite
                updateAxonalLength(brain, source, destination, axonalGrowth * dt);
                updateDendriticLength(brain, source, destination, dendriticGrowth * dt);
            }
        }
    }
}

float AxonalDendriticGrowth::calculateAxonalGrowth(Brain* brain, NeuronId source, NeuronId destination) const {
    // Calculate axonal growth potential based on activity and neurotrophic factors
    if (!brain) return 0.0f;
    
    // Find activity between source and destination neurons
    float activity = 0.0f;
    
    for (auto& region : brain->getRegions()) {
        Neuron* srcNeuron = nullptr;
        Neuron* dstNeuron = nullptr;
        
        for (Neuron* neuron : region->getAllNeurons()) {
            if (neuron->getId() == source) srcNeuron = neuron;
            if (neuron->getId() == destination) dstNeuron = neuron;
        }
        
        if (!srcNeuron || !dstNeuron) continue;
        
        const auto& srcSpikes = srcNeuron->getSpikeHistory();
        const auto& dstSpikes = dstNeuron->getSpikeHistory();
        
        if (srcSpikes.empty() || dstSpikes.empty()) continue;
        
        // Calculate coactivity (simplified)
        size_t coincidences = 0;
        for (Timestamp srcTime : srcSpikes) {
            for (Timestamp dstTime : dstSpikes) {
                if (std::abs(srcTime - dstTime) < 100.0f) {  // 100ms window
                    ++coincidences;
                }
            }
        }
        
        float srcActivity = static_cast<float>(srcSpikes.size());
        float dstActivity = static_cast<float>(dstSpikes.size());
        float totalActivity = srcActivity + dstActivity;
        
        if (totalActivity > 0.0f) {
            activity += static_cast<float>(coincidences) / totalActivity;
        }
    }
    
    // Calculate axonal growth based on activity and neurotrophic factor
    float growth = pImpl->axonalGrowthRate * activity * pImpl->neurotrophicFactor;
    
    return growth;
}

float AxonalDendriticGrowth::calculateDendriticGrowth(Brain* brain, NeuronId source, NeuronId destination) const {
    // Calculate dendritic growth potential
    if (!brain) return 0.0f;
    
    // Similar to axonal growth but for dendritic side
    // For now, use the same calculation as axonal growth
    return calculateAxonalGrowth(brain, source, destination);
}

void AxonalDendriticGrowth::updateAxonalLength(Brain* brain, NeuronId source, NeuronId destination, float growthAmount) {
    // Update axonal length between source and destination neurons
    if (!brain || growthAmount <= 0.0f) return;
    
    // In a full implementation, this would:
    // 1. Find current axonal length
    // 2. Add growthAmount to length
    // 3. Check if length exceeds max
    // 4. If so, shorten or prevent further growth
    
    // For demonstration, just update statistics
    if (growthAmount > 0.0f) {
        ++pImpl->totalAxonalGrowths;
    } else if (growthAmount < 0.0f) {
        ++pImpl->totalAxonalShortenings;
    }
}

void AxonalDendriticGrowth::updateDendriticLength(Brain* brain, NeuronId source, NeuronId destination, float growthAmount) {
    // Update dendritic length between source and destination neurons
    if (!brain || growthAmount <= 0.0f) return;
    
    // Similar to axonal length update
    if (growthAmount > 0.0f) {
        ++pImpl->totalDendriticGrowths;
    } else if (growthAmount < 0.0f) {
        ++pImpl->totalDendriticShortenings;
    }
}

} // namespace nlm