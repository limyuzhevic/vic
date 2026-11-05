// BrainStatistics.hpp - Statistics and logging
#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainStatistics {
public:
    static void collectGlobalStatistics(Brain& brain);
    static void logStatus(const Brain& brain);
    static void updateSpikeStatistics(Brain& brain);
    static void updateNeuronStatistics(Brain& brain);
    static void updateSynapseStatistics(Brain& brain);
    static void updateMemoryStatistics(Brain& brain);
    static void updateNeuromodulationStatistics(Brain& brain);
    static void updateDevelopmentStatistics(Brain& brain);
    static void calculateMetrics(Brain& brain);
    static void resetStatistics(Brain& brain);
    static void updateRateOfChange(Brain& brain);
    static void updateHistoricalTrends(Brain& brain);
    static void exportStatistics(Brain& brain);
    static void importStatistics(Brain& brain);
    static void analyzePerformance(Brain& brain);
    static void validateStatistics(Brain& brain);
    static void optimizeStatisticsCollection(Brain& brain);
    static void enableStatisticalTracking(Brain& brain);
    static void disableStatisticalTracking(Brain& brain);
    static void setStatisticalCollectionInterval(Brain& brain, float interval);
    static void setStatisticalCollectionThreshold(Brain& brain, float threshold);

private:
    static void updateEIEfficiency(Brain& brain);
    static void updateFiringRateDistribution(Brain& brain);
    static void updateSynapticStrengthDistribution(Brain& brain);
    static void updateConnectivityMetrics(Brain& brain);
    static void updateTemporalMetrics(Brain& brain);
    static void updateSpatialMetrics(Brain& brain);
    static void updateNetworkMetrics(Brain& brain);
    static void updateDynamicalMetrics(Brain& brain);
    static void updatePlasticityMetrics(Brain& brain);
    static void updateMemoryMetrics(Brain& brain);
    static void updateNeuromodulationMetrics(Brain& brain);
    static void updateDevelopmentMetrics(Brain& brain);
};
