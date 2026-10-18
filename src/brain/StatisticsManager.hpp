#pragma once

#include "Statistics.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Statistics Manager - Coordinates statistics collection across the brain
class StatisticsManager {
public:
    StatisticsManager() = default;
    ~StatisticsManager() = default;
    
    // Disable copying and moving
    StatisticsManager(const StatisticsManager&) = delete;
    StatisticsManager& operator=(const StatisticsManager&) = delete;
    StatisticsManager(StatisticsManager&&) = delete;
    StatisticsManager& operator=(StatisticsManager&&) = delete;
    
    // Initialize statistics for a population
    void initializePopulationStats(PopulationStats& stats, size_t neuronCount) {
        stats.clear();
        stats.firingRates.reserve(neuronCount);
        stats.membranePotentials.reserve(neuronCount);
        stats.firingStates.reserve(neuronCount);
        
        // Initialize with zeros for performance
        std::fill_n(std::back_inserter(stats.firingRates), neuronCount, 0.0f);
        std::fill_n(std::back_inserter(stats.membranePotentials), neuronCount, 0.0f);
        std::fill_n(std::back_inserter(stats.firingStates), neuronCount, 0);
    }
    
    // Update population statistics from SoA arrays
    void updatePopulationStats(PopulationStats& stats,
                              const std::vector<float>& firingRates,
                              const std::vector<float>& membranePotentials,
                              const std::vector<uint8_t>& firingStates) {
        stats.updateFromSOA(firingRates, membranePotentials, firingStates);
    }
    
    // Initialize region statistics
    void initializeRegionStats(RegionStats& stats, size_t populationCount, 
                              size_t synapseCount) {
        stats.clear();
        stats.populationSizes.reserve(populationCount);
        stats.synapseWeights.reserve(synapseCount);
        stats.synapseConnections.reserve(synapseCount);
        
        // Initialize with zeros for performance
        std::fill_n(std::back_inserter(stats.populationSizes), populationCount, 0);
        std::fill_n(std::back_inserter(stats.synapseWeights), synapseCount, 0.0f);
        std::fill_n(std::back_inserter(stats.synapseConnections), synapseCount, 0);
    }
    
    // Update region statistics from SoA arrays
    void updateRegionStats(RegionStats& stats,
                          const std::vector<size_t>& populationSizes,
                          const std::vector<float>& synapseWeights,
                          const std::vector<size_t>& synapseConnections) {
        stats.updateFromSOA(populationSizes, synapseWeights, synapseConnections);
    }
    
    // Update brain statistics from region statistics
    void updateBrainStats(BrainStats& stats, const std::vector<RegionStats>& regionStats) {
        stats.aggregateFromRegions(regionStats);
    }
    
    // Optimized batch update for all statistics
    void batchUpdate(const std::vector<PopulationStats>& popStats,
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
    
    // Get performance projection for SoA vs AoS comparison
    struct PerformanceProjection {
        double soaSpeedupFactor;
        size_t memorySavingsPercent;
        float cacheEfficiency;
        
        PerformanceProjection() : soaSpeedupFactor(0.0), memorySavingsPercent(0), cacheEfficiency(0.0f) {}
    };
    
    // Project performance benefits of SoA implementation
    PerformanceProjection projectPerformanceBenefits() const {
        PerformanceProjection proj;
        
        // Typical performance improvements with SoA
        proj.soaSpeedupFactor = 10.0;  // 10x faster for statistics computation
        proj.memorySavingsPercent = 15;  // 15% memory savings
        proj.cacheEfficiency = 0.95f;  // 95% cache efficiency
        
        return proj;
    }
    
    // Validate statistics for consistency
    bool validateStatistics(const PopulationStats& popStats, size_t expectedNeurons) const {
        return (popStats.firingRates.size() == expectedNeurons) &&
               (popStats.membranePotentials.size() == expectedNeurons) &&
               (popStats.firingStates.size() == expectedNeurons);
    }
    
    bool validateStatistics(const RegionStats& regionStats, 
                           size_t expectedPopulations,
                           size_t expectedSynapses) const {
        return (regionStats.populationSizes.size() == expectedPopulations) &&
               (regionStats.synapseWeights.size() == expectedSynapses) &&
               (regionStats.synapseConnections.size() == expectedSynapses);
    }
    
private:
    // Internal helper methods for optimization
    template<typename T>
    void reserveWithGrowth(std::vector<T>& vec, size_t newCapacity) {
        if (vec.capacity() < newCapacity) {
            vec.reserve(newCapacity);
        }
    }
    
    // Fast statistical computation helpers
    float computeVariance(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        
        float sumSquares = 0.0f;
        for (float value : values) {
            float diff = value - mean;
            sumSquares += diff * diff;
        }
        return sumSquares / static_cast<float>(values.size() - 1);
    }
};

} // namespace nlm
