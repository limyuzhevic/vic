#include "Statistics.hpp"

namespace nlm {

// PopulationStats implementation
void PopulationStats::updateFromSOA(const std::vector<float>& rates,
                                   const std::vector<float>& potentials,
                                   const std::vector<uint8_t>& states) {
    // SoA access pattern for optimal cache performance
    firingRates = rates;
    membranePotentials = potentials;
    firingStates = states;
    
    computeStatistics();
}

// RegionStats implementation
void RegionStats::updateFromSOA(const std::vector<size_t>& sizes,
                               const std::vector<float>& weights,
                               const std::vector<size_t>& connections) {
    // Direct references for fast access
    populationSizes = sizes;
    synapseWeights = weights;
    synapseConnections = connections;
    
    computeStatistics();
}

// BrainStats implementation
void BrainStats::updateFromSOA(const std::vector<float>& regionRates,
                             const std::vector<size_t>& regionCounts,
                             const std::vector<float>& regionWeights_) {
    // SoA pattern for fast global statistics computation
    regionFiringRates = regionRates;
    regionNeuronCounts = regionCounts;
    regionWeights = regionWeights_;
    
    computeStatistics();
}

void BrainStats::aggregateFromRegions(const std::vector<RegionStats>& regionStatsList) {
    // Aggregate statistics from all regions
    clear();
    regionCount = regionStatsList.size();
    
    // Fast aggregation using SoA patterns
    for (const auto& stats : regionStatsList) {
        totalNeuronCount += stats.totalNeuronCount;
        activeNeuronCount += stats.activeNeuronCount;
        firingNeuronCount += stats.firingNeuronCount;
        
        // Store for SoA access
        regionNeuronCounts.push_back(stats.totalNeuronCount);
        regionFiringRates.push_back(stats.averageFiringRate);
        regionWeights.push_back((stats.totalSynapseCount > 0) ? stats.averageSynapticWeight : 0.0f);
    }
    
    // Compute averages
    if (regionCount > 0) {
        averageFiringRate = static_cast<float>(activeNeuronCount) / static_cast<float>(regionCount);
        excitationInhibitionRatio = computeEIRatio();
    }
}

float BrainStats::computeEIRatio() {
    // Compute excitation/inhibition ratio efficiently
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (float weight : regionWeights) {
        if (weight > 0.0f) {
            totalExcitatory += weight;
        } else if (weight < 0.0f) {
            totalInhibitory += std::abs(weight);
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    } else if (totalExcitatory > 0.0f) {
        return std::numeric_limits<float>::infinity();
    }
    return 0.0f;
}

} // namespace nlm
