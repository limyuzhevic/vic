#include "StatisticsManager.hpp"
#include <numeric>
#include <algorithm>

namespace nlm {

// StatisticsManager implementation

void StatisticsManager::initializePopulationStats(PopulationStats& stats, size_t neuronCount) {
    stats.clear();
    stats.firingRates.clear();
    stats.firingRates.reserve(neuronCount);
    stats.membranePotentials.clear();
    stats.membranePotentials.reserve(neuronCount);
    stats.firingStates.clear();
    stats.firingStates.reserve(neuronCount);
    
    // Initialize with zeros for performance
    stats.firingRates.assign(neuronCount, 0.0f);
    stats.membranePotentials.assign(neuronCount, 0.0f);
    stats.firingStates.assign(neuronCount, 0);
}

void StatisticsManager::updatePopulationStats(PopulationStats& stats,
                                             const std::vector<float>& firingRates,
                                             const std::vector<float>& membranePotentials,
                                             const std::vector<uint8_t>& firingStates) {
    // SoA access pattern for optimal cache performance
    stats.updateFromSOA(firingRates, membranePotentials, firingStates);
}

void StatisticsManager::initializeRegionStats(RegionStats& stats, size_t populationCount, 
                                             size_t synapseCount) {
    stats.clear();
    stats.populationSizes.clear();
    stats.populationSizes.reserve(populationCount);
    stats.synapseWeights.clear();
    stats.synapseWeights.reserve(synapseCount);
    stats.synapseConnections.clear();
    stats.synapseConnections.reserve(synapseCount);
    
    // Initialize with zeros for performance
    stats.populationSizes.assign(populationCount, 0);
    stats.synapseWeights.assign(synapseCount, 0.0f);
    stats.synapseConnections.assign(synapseCount, 0);
}

void StatisticsManager::updateRegionStats(RegionStats& stats,
                                         const std::vector<size_t>& populationSizes,
                                         const std::vector<float>& synapseWeights,
                                         const std::vector<size_t>& synapseConnections) {
    stats.updateFromSOA(populationSizes, synapseWeights, synapseConnections);
}

void StatisticsManager::updateBrainStats(BrainStats& stats, const std::vector<RegionStats>& regionStats) {
    stats.aggregateFromRegions(regionStats);
}

void StatisticsManager::batchUpdate(const std::vector<PopulationStats>& popStats,
                                   const std::vector<RegionStats>& regionStats,
                                   BrainStats& brainStats) {
    // Fast path for empty statistics
    if (popStats.empty() && regionStats.empty()) {
        brainStats.clear();
        return;
    }
    
    // Aggregate all region statistics
    updateBrainStats(brainStats, regionStats);
}

} // namespace nlm
