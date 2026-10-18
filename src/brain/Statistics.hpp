#pragma once

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

namespace nlm {

// Population Statistics Class
class PopulationStats {
public:
    // Basic statistics
    size_t activeCount = 0;
    size_t firingCount = 0;
    float averageFiringRate = 0.0f;
    float activityLevel = 0.0f;
    
    // Membrane potential statistics
    float meanMembranePotential = 0.0f;
    float varianceMembranePotential = 0.0f;
    
    // Performance optimization - SoA arrays for fast computation
    std::vector<float> firingRates;
    std::vector<float> membranePotentials;
    std::vector<uint8_t> firingStates;
    std::vector<float> membranePotentialsRecord;
    
    // Clear all statistics
    void clear() {
        activeCount = 0;
        firingCount = 0;
        averageFiringRate = 0.0f;
        activityLevel = 0.0f;
        meanMembranePotential = 0.0f;
        varianceMembranePotential = 0.0f;
        
        firingRates.clear();
        membranePotentials.clear();
        firingStates.clear();
        membranePotentialsRecord.clear();
    }
    
    // Update statistics from SoA arrays (optimized for performance)
    void updateFromSOA(const std::vector<float>& rates, 
                      const std::vector<float>& potentials,
                      const std::vector<uint8_t>& states) {
        // Store references to avoid copying
        firingRates = rates;
        membranePotentials = potentials;
        firingStates = states;
        
        // Fast computation using SoA access patterns
        computeStatistics();
    }
    
private:
    void computeStatistics() {
        size_t size = firingRates.size();
        
        if (size == 0) {
            clear();
            return;
        }
        
        // Compute active count
        activeCount = 0;
        firingCount = 0;
        float sumRates = 0.0f;
        float sumPotentials = 0.0f;
        
        for (size_t i = 0; i < size; ++i) {
            if (firingRates[i] > 0.0f) activeCount++;
            if (firingStates[i] > 0) firingCount++;
            sumRates += firingRates[i];
            sumPotentials += membranePotentials[i];
        }
        
        // Compute averages
        averageFiringRate = sumRates / static_cast<float>(size);
        meanMembranePotential = sumPotentials / static_cast<float>(size);
        activityLevel = static_cast<float>(activeCount) / static_cast<float>(size);
        
        // Compute variance using two-pass algorithm
        if (size > 1) {
            float mean = meanMembranePotential;
            float sumSquares = 0.0f;
            for (size_t i = 0; i < size; ++i) {
                float diff = membranePotentials[i] - mean;
                sumSquares += diff * diff;
            }
            varianceMembranePotential = sumSquares / static_cast<float>(size - 1);
        }
    }
};

// Region Statistics Class
class RegionStats {
public:
    // Basic statistics
    size_t totalNeuronCount = 0;
    size_t activeNeuronCount = 0;
    size_t firingNeuronCount = 0;
    float averageFiringRate = 0.0f;
    
    // Connectivity statistics
    size_t totalSynapseCount = 0;
    float averageSynapticWeight = 0.0f;
    float synapticDensity = 0.0f;
    
    // Performance optimization - SoA arrays for fast computation
    std::vector<size_t> populationSizes;
    std::vector<float> synapseWeights;
    std::vector<size_t> synapseConnections;
    
    // Clear all statistics
    void clear() {
        totalNeuronCount = 0;
        activeNeuronCount = 0;
        firingNeuronCount = 0;
        averageFiringRate = 0.0f;
        totalSynapseCount = 0;
        averageSynapticWeight = 0.0f;
        synapticDensity = 0.0f;
        
        populationSizes.clear();
        synapseWeights.clear();
        synapseConnections.clear();
    }
    
    // Update statistics from SoA arrays
    void updateFromSOA(const std::vector<size_t>& sizes,
                      const std::vector<float>& weights,
                      const std::vector<size_t>& connections) {
        // Store references for fast access
        populationSizes = sizes;
        synapseWeights = weights;
        synapseConnections = connections;
        
        computeStatistics();
    }
    
private:
    void computeStatistics() {
        // Compute neuron statistics
        totalNeuronCount = 0;
        for (size_t size : populationSizes) {
            totalNeuronCount += size;
        }
        
        // Compute synaptic statistics
        totalSynapseCount = synapseConnections.size();
        
        if (!synapseWeights.empty()) {
            float sum = 0.0f;
            for (float weight : synapseWeights) {
                sum += std::abs(weight);  // Use absolute weight for density
            }
            averageSynapticWeight = sum / static_cast<float>(synapseWeights.size());
        }
        
        // Compute synaptic density
        if (totalNeuronCount > 1) {
            size_t maxConnections = totalNeuronCount * (totalNeuronCount - 1);
            synapticDensity = static_cast<float>(totalSynapseCount) / 
                           static_cast<float>(maxConnections);
        }
    }
};

// Global Brain Statistics Class
class BrainStats {
public:
    // Performance metrics
    size_t totalSpikes = 0;
    size_t pendingSpikeEvents = 0;
    float excitationInhibitionRatio = 0.0f;
    
    // Activity metrics
    size_t totalNeuronCount = 0;
    size_t activeNeuronCount = 0;
    size_t firingNeuronCount = 0;
    float averageFiringRate = 0.0f;
    
    // State information
    size_t regionCount = 0;
    Timestamp currentTime = 0.0;
    SimulationStep currentStep = 0;
    
    // Performance optimization - SoA arrays for global statistics
    std::vector<float> regionFiringRates;
    std::vector<size_t> regionNeuronCounts;
    std::vector<float> regionWeights;
    
    // Clear all statistics
    void clear() {
        totalSpikes = 0;
        pendingSpikeEvents = 0;
        excitationInhibitionRatio = 0.0f;
        totalNeuronCount = 0;
        activeNeuronCount = 0;
        firingNeuronCount = 0;
        averageFiringRate = 0.0f;
        regionCount = 0;
        currentTime = 0.0;
        currentStep = 0;
        
        regionFiringRates.clear();
        regionNeuronCounts.clear();
        regionWeights.clear();
    }
    
    // Update statistics from SoA arrays
    void updateFromSOA(const std::vector<float>& regionRates,
                      const std::vector<size_t>& regionCounts,
                      const std::vector<float>& regionWeights_) {
        // Store references for fast access
        regionFiringRates = regionRates;
        regionNeuronCounts = regionCounts;
        regionWeights = regionWeights_;
        
        computeStatistics();
    }
    
    // Aggregate statistics from regions
    void aggregateFromRegions(const std::vector<RegionStats>& regionStatsList) {
        clear();
        regionCount = regionStatsList.size();
        
        size_t totalNeurons = 0;
        size_t totalActive = 0;
        size_t totalFiring = 0;
        float sumRates = 0.0f;
        
        for (const auto& stats : regionStatsList) {
            totalNeurons += stats.totalNeuronCount;
            totalActive += stats.activeNeuronCount;
            totalFiring += stats.firingNeuronCount;
            sumRates += stats.averageFiringRate;
            
            // Store for global SoA access
            regionNeuronCounts.push_back(stats.totalNeuronCount);
            regionFiringRates.push_back(stats.averageFiringRate);
        }
        
        totalNeuronCount = totalNeurons;
        activeNeuronCount = totalActive;
        firingNeuronCount = totalFiring;
        averageFiringRate = regionCount > 0 ? sumRates / static_cast<float>(regionCount) : 0.0f;
    }
    
private:
    void computeStatistics() {
        // Compute global averages
        if (!regionNeuronCounts.empty()) {
            float sumRates = 0.0f;
            for (float rate : regionFiringRates) {
                sumRates += rate;
            }
            averageFiringRate = sumRates / static_cast<float>(regionFiringRates.size());
        }
        
        // Compute excitation/inhibition ratio
        if (!regionWeights.empty()) {
            float totalExcitatory = 0.0f;
            float totalInhibitory = 0.0f;
            for (float weight : regionWeights) {
                if (weight > 0.0f) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
            
            if (totalInhibitory > 0.0f) {
                excitationInhibitionRatio = totalExcitatory / totalInhibitory;
            } else if (totalExcitatory > 0.0f) {
                excitationInhibitionRatio = std::numeric_limits<float>::infinity();
            }
        }
    }
};

} // namespace nlm
