// BrainStatistics.cpp - Implementation of BrainStatistics class

#include "BrainStatistics.hpp"
#include "Brain.hpp"

namespace nlm {

void BrainStatistics::collectGlobalStatistics(Brain& brain) {
    // Collect global statistics
    // This includes updating all statistics from various systems
    updateSpikeStatistics(brain);
    updateNeuronStatistics(brain);
    updateSynapseStatistics(brain);
    updateMemoryStatistics(brain);
    updateNeuromodulationStatistics(brain);
    updateDevelopmentStatistics(brain);
    calculateMetrics(brain);
}

void BrainStatistics::logStatus(const Brain& brain) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain.getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));

    // Memory system status
    if (brain.pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(brain.pImpl->workingMemory->getActiveTraces()));
    }
    if (brain.pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain.pImpl->episodicMemory->getEpisodeCount()));
    }

    // Neuromodulation status
    if (brain.pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(brain.pImpl->dopamine->getLevel()));
    }

    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));

    for (const auto& region : brain.getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

void BrainStatistics::updateSpikeStatistics(Brain& brain) {
    // Update spike statistics
    // This is already done in the main step function
}

void BrainStatistics::updateNeuronStatistics(Brain& brain) {
    // Update neuron statistics
    // Placeholder for neuron statistics
}

void BrainStatistics::updateSynapseStatistics(Brain& brain) {
    // Update synapse statistics
    // Placeholder for synapse statistics
}

void BrainStatistics::updateMemoryStatistics(Brain& brain) {
    // Update memory statistics
    // Placeholder for memory statistics
}

void BrainStatistics::updateNeuromodulationStatistics(Brain& brain) {
    // Update neuromodulation statistics
    // Placeholder for neuromodulation statistics
}

void BrainStatistics::updateDevelopmentStatistics(Brain& brain) {
    // Update development statistics
    // Placeholder for development statistics
}

void BrainStatistics::calculateMetrics(Brain& brain) {
    // Calculate various metrics
    updateEIEfficiency(brain);
    updateFiringRateDistribution(brain);
    updateSynapticStrengthDistribution(brain);
    updateConnectivityMetrics(brain);
    updateTemporalMetrics(brain);
    updateNetworkMetrics(brain);
    updateDynamicalMetrics(brain);
    updatePlasticityMetrics(brain);
    updateMemoryMetrics(brain);
    updateNeuromodulationMetrics(brain);
    updateDevelopmentMetrics(brain);
}

void BrainStatistics::resetStatistics(Brain& brain) {
    brain.pImpl->currentStep = 0;
    brain.pImpl->currentTime = 0.0;
    brain.pImpl->totalSpikesThisStep = 0;
}

void BrainStatistics::updateRateOfChange(Brain& brain) {
    // Update rate of change statistics
    // Placeholder for rate of change updates
}

void BrainStatistics::updateHistoricalTrends(Brain& brain) {
    // Update historical trends
    // Placeholder for historical trend updates
}

void BrainStatistics::exportStatistics(Brain& brain) {
    // Export statistics to file or external system
    // Placeholder for statistics export
}

void BrainStatistics::importStatistics(Brain& brain) {
    // Import statistics from file or external system
    // Placeholder for statistics import
}

void BrainStatistics::analyzePerformance(Brain& brain) {
    // Analyze system performance
    // Placeholder for performance analysis
}

void BrainStatistics::validateStatistics(Brain& brain) {
    // Validate statistics for consistency
    // Placeholder for statistics validation
}

void BrainStatistics::optimizeStatisticsCollection(Brain& brain) {
    // Optimize statistics collection for performance
    // Placeholder for optimization
}

void BrainStatistics::enableStatisticalTracking(Brain& brain) {
    // Enable statistical tracking
    // Placeholder for enabling tracking
}

void BrainStatistics::disableStatisticalTracking(Brain& brain) {
    // Disable statistical tracking
    // Placeholder for disabling tracking
}

void BrainStatistics::setStatisticalCollectionInterval(Brain& brain, float interval) {
    // Set statistical collection interval
    // Placeholder for interval setting
}

void BrainStatistics::setStatisticalCollectionThreshold(Brain& brain, float threshold) {
    // Set statistical collection threshold
    // Placeholder for threshold setting
}

void BrainStatistics::updateEIEfficiency(Brain& brain) {
    // Update E/I efficiency metrics
    // Placeholder for E/I efficiency updates
}

void BrainStatistics::updateFiringRateDistribution(Brain& brain) {
    // Update firing rate distribution
    // Placeholder for firing rate distribution updates
}

void BrainStatistics::updateSynapticStrengthDistribution(Brain& brain) {
    // Update synaptic strength distribution
    // Placeholder for synaptic strength distribution updates
}

void BrainStatistics::updateConnectivityMetrics(Brain& brain) {
    // Update connectivity metrics
    // Placeholder for connectivity metric updates
}

void BrainStatistics::updateTemporalMetrics(Brain& brain) {
    // Update temporal metrics
    // Placeholder for temporal metric updates
}

void BrainStatistics::updateSpatialMetrics(Brain& brain) {
    // Update spatial metrics
    // Placeholder for spatial metric updates
}

void BrainStatistics::updateNetworkMetrics(Brain& brain) {
    // Update network metrics
    // Placeholder for network metric updates
}

void BrainStatistics::updateDynamicalMetrics(Brain& brain) {
    // Update dynamical metrics
    // Placeholder for dynamical metric updates
}

void BrainStatistics::updatePlasticityMetrics(Brain& brain) {
    // Update plasticity metrics
    // Placeholder for plasticity metric updates
}

void BrainStatistics::updateMemoryMetrics(Brain& brain) {
    // Update memory metrics
    // Placeholder for memory metric updates
}

void BrainStatistics::updateNeuromodulationMetrics(Brain& brain) {
    // Update neuromodulation metrics
    // Placeholder for neuromodulation metric updates
}

void BrainStatistics::updateDevelopmentMetrics(Brain& brain) {
    // Update development metrics
    // Placeholder for development metric updates
}
