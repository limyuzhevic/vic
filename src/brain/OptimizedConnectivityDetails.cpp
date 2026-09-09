#include "OptimizedConnectivity.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Helper function to compute distance between two 3D points
template<typename T>
float distance3D(const std::array<T, 3>& p1, const std::array<T, 3>& p2) {
    float dx = static_cast<float>(p1[0] - p2[0]);
    float dy = static_cast<float>(p1[1] - p2[1]);
    float dz = static_cast<float>(p1[2] - p2[2]);
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

void BiologicalConnectivityOptimizer::applyDistanceDependency(
    CSRConnectivity& connectivity,
    const std::vector<std::array<float, 3>>& neuronPositions,
    float maxDistance,
    float distanceDecay) {
    
    // Clear existing connections
    connectivity.clear();
    connectivity.reserveNeurons(neuronPositions.size());
    
    // Add connections based on distance
    for (size_t i = 0; i < neuronPositions.size(); ++i) {
        for (size_t j = 0; j < neuronPositions.size(); ++j) {
            if (i == j) continue; // No self-connections
            
            float dist = distance3D(neuronPositions[i], neuronPositions[j]);
            
            if (dist <= maxDistance) {
                // Distance-dependent probability
                float probability = std::exp(-distanceDecay * dist / maxDistance);
                
                // Add connection with probability (in real implementation, would use random number)
                // For now, connect if within 50% of maxDistance
                if (dist <= maxDistance * 0.5f) {
                    float weight = 1.0f - (dist / maxDistance); // Stronger for closer neurons
                    connectivity.addSynapse(i, j, weight, 
                                          (i < j) ? 0 : 1,  // Excitatory for forward, inhibitory for backward
                                          1);  // Default delay
                }
            }
        }
    }
}

void BiologicalConnectivityOptimizer::createRegionalHierarchies(
    CSRConnectivity& connectivity,
    const std::vector<NeuronType>& neuronTypes,
    const std::vector<uint64_t>& regionMapping) {
    
    // Clear existing connections
    connectivity.clear();
    connectivity.reserveNeurons(neuronTypes.size());
    
    size_t numRegions = *std::max_element(regionMapping.begin(), regionMapping.end()) + 1;
    
    // Create connections within regions (stronger)
    for (size_t region = 0; region < numRegions; ++region) {
        // Find neurons in this region
        std::vector<size_t> regionNeurons;
        for (size_t i = 0; i < regionMapping.size(); ++i) {
            if (regionMapping[i] == region) {
                regionNeurons.push_back(i);
            }
        }
        
        // Connect neurons within region with higher probability
        for (size_t i = 0; i < regionNeurons.size(); ++i) {
            for (size_t j = 0; j < regionNeurons.size(); ++j) {
                if (i == j) continue;
                
                // Within-region connection (stronger)
                float weight = 0.5f; // Strong intra-regional connection
                connectivity.addSynapse(regionNeurons[i], regionNeurons[j],
                                      weight, 0,  // Excitatory
                                      1);
            }
        }
        
        // Add some long-range connections to other regions
        if (region < numRegions - 1) {
            for (size_t i : regionNeurons) {
                // Connect to neurons in next region
                for (size_t j = 0; j < regionMapping.size(); ++j) {
                    if (regionMapping[j] == region + 1) {
                        float weight = 0.2f; // Weaker inter-regional connection
                        connectivity.addSynapse(i, j,
                                              weight, 1,  // Inhibitory
                                              2);  // Longer delay
                    }
                }
            }
        }
    }
}

void BiologicalConnectivityOptimizer::compressConnectivity(
    CSRConnectivity& connectivity,
    float compressionThreshold) {
    
    Stats stats = connectivity.getStats();
    float currentProbability = stats.connectivityProbability;
    
    if (currentProbability <= compressionThreshold) {
        return; // Already compressed enough
    }
    
    // Clear and re-add with compressed connections
    connectivity.clear();
    connectivity.reserveNeurons(stats.numNeurons);
    
    // Only keep the strongest connections
    // In a full implementation, we would sort synapses by weight
    // and keep only top percentile based on threshold
    
    // For now, just add a subset of connections
    size_t targetSynapses = static_cast<size_t>(currentProbability * 0.5f * 
                                                 stats.numNeurons * (stats.numNeurons - 1));
    
    if (targetSynapses > 0) {
        // Add a subset of connections (simplified implementation)
        size_t added = 0;
        for (size_t i = 0; i < stats.numNeurons && added < targetSynapses; ++i) {
            for (size_t j = 0; j < stats.numNeurons && added < targetSynapses; ++j) {
                if (i != j) {
                    float weight = 0.3f + 0.7f * static_cast<float>(rand()) / RAND_MAX;
                    if (weight > 0.5f) {  // Only keep strong connections
                        connectivity.addSynapse(i, j, weight, 0, 1);
                        added++;
                    }
                }
            }
        }
    }
}

void BiologicalConnectivityOptimizer::applyWeightQuantization(
    CSRConnectivity& connectivity,
    uint8_t bits) {
    
    // Note: In a real implementation, this would quantize weights
    // For now, just log the operation
    Stats stats = connectivity.getStats();
    
    // Weight quantization implementation would go here:
    // - For 8-bit: scale weights to [-1, 1] and quantize to 256 levels
    // - For 16-bit: scale weights to [-128, 127] and quantize to 65536 levels
    // - Update all synapses with quantized weights
}

// NeuronIndexer implementation
template<typename T>
NeuronIndexer::NeuronIndexer(size_t neuronCount) {
    neuronIndices_.reserve(neuronCount);
    neuronToIndexMap_.reserve(neuronCount);
}

template<typename T>
void NeuronIndexer::addNeuron(uint64_t neuronId, uint32_t regionIndex,
                               uint32_t populationIndex, uint32_t localIndex) {
    NeuronIndex index{neuronId, regionIndex, populationIndex, localIndex};
    neuronIndices_.push_back(index);
    neuronToIndexMap_[neuronId] = neuronIndices_.size() - 1;
    
    // Add to type-based index (type would need to be stored elsewhere)
    // For now, placeholder implementation
}

template<typename T>
void NeuronIndexer::removeNeuron(uint64_t neuronId) {
    auto it = neuronToIndexMap_.find(neuronId);
    if (it != neuronToIndexMap_.end()) {
        uint32_t idx = it->second;
        neuronToIndexMap_.erase(it);
        
        // Remove from neuronIndices_ (swap with last element for efficiency)
        if (idx < neuronIndices_.size() - 1) {
            neuronIndices_[idx] = neuronIndices_.back();
            // Update the map for the swapped neuron
            neuronToIndexMap_[neuronIndices_[idx].neuronId] = idx;
        }
        neuronIndices_.pop_back();
    }
}

// Other implementations would follow...

} // namespace nlm
