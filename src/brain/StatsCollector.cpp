// StatsCollector.cpp - Statistics collection implementation

#include "StatsCollector.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

StatsCollector::StatsCollector() {
    // Initialize if needed
}

StatsCollector::BrainStats StatsCollector::collect(const Brain* brain) {
    BrainStats stats;
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot collect stats: brain is null");
        return stats;
    }
    
    // Basic network statistics
    stats.totalNeurons = brain->getTotalNeuronCount();
    stats.totalSynapses = brain->getTotalSynapseCount();
    stats.activeNeurons = brain->getActiveNeuronCount();
    stats.firingNeurons = brain->getFiringNeuronCount();
    stats.totalSpikes = brain->getTotalSpikeCount();
    stats.pendingSpikes = brain->getPendingSpikeEventCount();
    stats.averageFiringRate = brain->getAverageFiringRate();
    stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    // Collect regional statistics
    auto regionIds = brain->getRegionIds();
    for (const auto& regionId : regionIds) {
        if (auto* region = brain->getRegion(regionId)) {
            stats.regionStats.push_back(collectRegionStats(region));
        }
    }
    
    // Calculate derived metrics
    stats.stabilityScore = calculateStability(stats);
    stats.learningScore = 0.0f;  // Will be calculated when comparing with previous state
    stats.coherenceScore = calculateCoherence(stats);
    
    return stats;
}

StatsCollector::BrainStats::RegionStats StatsCollector::collectRegionStats(const NeuralRegion* region) {
    RegionStats stats;
    
    if (!region) return stats;
    
    stats.name = region->getName();
    stats.neuronCount = region->getTotalNeuronCount();
    stats.synapseCount = region->getSynapseCount();
    stats.firingRate = region->getAverageFiringRate();
    stats.weightMean = region->getAverageSynapticWeight();
    
    return stats;
}

std::string StatsCollector::formatStats(const BrainStats& stats, const std::string& title) {
    std::stringstream ss;
    
    ss << title << "\n";
    ss << std::string(title.length(), '=') << "\n\n";
    
    ss << "=== Network Overview ===\n";
    ss << "Neurons: " << stats.totalNeurons << "\n";
    ss << "Synapses: " << stats.totalSynapses << "\n";
    ss << "Connectivity Density: " << (stats.totalSynapses / (stats.totalNeurons * stats.totalNeurons)) * 100.0f << "%\n";
    ss << "Active Neurons: " << stats.activeNeurons << " (" << 
        (stats.totalNeurons > 0 ? (stats.activeNeurons * 100.0f / stats.totalNeurons) : 0.0f) << "% of network)\n";
    ss << "Firing Neurons: " << stats.firingNeurons << " (" << 
        (stats.totalNeurons > 0 ? (stats.firingNeurons * 100.0f / stats.totalNeurons) : 0.0f) << "% of network)\n";
    ss << "Total Spikes: " << stats.totalSpikes << "\n";
    ss << "Pending Spikes: " << stats.pendingSpikes << "\n";
    ss << std::fixed << std::setprecision(3);
    ss << "Average Firing Rate: " << stats.averageFiringRate << " Hz\n";
    ss << "Excitatory/Inhibitory Ratio: " << stats.excitationInhibitionRatio << "\n";
    
    ss << "\n=== Network Health ===\n";
    ss << "Membrane Potential - Mean: " << stats.meanMembranePotential << " mV, Variance: " << stats.varianceMembranePotential << " mV²\n";
    ss << "Synaptic Weight - Mean: " << stats.meanSynapticWeight << "\n";
    ss << "Synaptic Density: " << stats.synapticDensity << " synapses per neuron\n";
    
    ss << "\n=== Regional Distribution ===\n";
    for (const auto& region : stats.regionStats) {
        ss << "Region: " << region.name << "\n";
        ss << "  Neurons: " << region.neuronCount << ", Synapses: " << region.synapseCount << "\n";
        ss << "  Firing Rate: " << region.firingRate << " Hz\n";
        ss << "  Avg Synaptic Weight: " << region.weightMean << "\n";
    }
    
    ss << "\n=== Quality Metrics ===\n";
    ss << "Stability Score: " << stats.stabilityScore << " (0.0 = unstable, 1.0 = very stable)\n";
    ss << "Learning Score: " << stats.learningScore << " (0.0 = no learning, 1.0 = significant learning)\n";
    ss << "Coherence Score: " << stats.coherenceScore << " (0.0 = desynchronized, 1.0 = perfectly synchronized)\n";
    
    ss << "\n=== Performance ===\n";
    ss << "Steps Completed: " << stats.stepsCompleted << "\n";
    ss << "Simulation Time: " << stats.simulationTime << " seconds\n";
    if (stats.simulationTime > 0.0) {
        ss << "Performance: " << stats.performanceFPS << " FPS\n";
    }
    
    ss << "\n=== Memory Usage ===\n";
    ss << "Estimated Memory Usage: " << (stats.memoryUsageBytes / 1024) << " KB\n";
    ss << "Checkpoint Size: " << (stats.checkpointSizeBytes / 1024) << " KB\n";
    
    return ss.str();
}

std::string StatsCollector::formatRegionStats(const BrainStats::RegionStats& stats) {
    std::stringstream ss;
    
    ss << "Region: " << stats.name << "\n";
    ss << "  Neurons: " << stats.neuronCount << "\n";
    ss << "  Synapses: " << stats.synapseCount << "\n";
    ss << "  Density: " << (stats.neuronCount > 0 ? (stats.synapseCount / static_cast<float>(stats.neuronCount)) : 0.0f) << " synapses per neuron\n";
    ss << "  Firing Rate: " << stats.firingRate << " Hz\n";
    ss << "  Average Synaptic Weight: " << stats.weightMean << "\n";
    
    return ss.str();
}

float StatsCollector::calculateStability(const BrainStats& stats) {
    if (stats.totalNeurons == 0) return 0.0f;
    
    // Stability is based on:
    // 1. Low spike variance (don't want seizures)
    // 2. Balanced E/I ratio (not runaway excitation)
    // 3. Reasonable firing rates (not complete silence)
    
    float spikeStability = 1.0f / (1.0f + std::sqrt(static_cast<float>(stats.totalSpikes)));
    
    float eiratioStability = 1.0f / (1.0f + std::abs(stats.excitationInhibitionRatio - 1.0f));
    
    float activityStability = 1.0f - std::abs(stats.averageFiringRate - 20.0f) / 100.0f; // Ideal around 20 Hz
    if (activityStability < 0.0f) activityStability = 0.0f;
    
    return (spikeStability + eiratioStability + activityStability) / 3.0f;
}

float StatsCollector::calculateLearning(const BrainStats& stats, const BrainStats& previousStats) {
    if (stats.totalSynapses == 0) return 0.0f;
    
    // Learning is measured by synaptic weight changes
    // This would be more sophisticated if we had access to weight history
    // For now, estimate based on current activity and stability
    
    float activityBased = std::min(stats.averageFiringRate / 50.0f, 1.0f); // More activity = more learning
    float stabilityBased = stats.stabilityScore; // Stable networks can learn more
    
    return (activityBased + stabilityBased) / 2.0f;
}

float StatsCollector::calculateCoherence(const BrainStats& stats) {
    if (stats.totalNeurons == 0) return 0.0f;
    
    // Coherence is based on synchronous activity
    // High firing rate neuron percentage indicates coordination
    float firingPercentage = stats.totalNeurons > 0 ? (stats.firingNeurons * 100.0f / stats.totalNeurons) : 0.0f;
    
    // Ideal coherence is around 10-30% of neurons firing simultaneously
    if (firingPercentage < 10.0f) {
        return firingPercentage / 10.0f;
    } else if (firingPercentage > 30.0f) {
        return std::max(0.0f, 1.0f - (firingPercentage - 30.0f) / 70.0f);
    } else {
        return 1.0f;
    }
}

bool StatsCollector::validateStats(const BrainStats& stats) {
    if (stats.totalNeurons == 0) return false;
    if (stats.excitationInhibitionRatio < 0.0f) return false;
    if (stats.averageFiringRate < 0.0f) return false;
    
    // Check that regional stats sum to total neurons
    size_t regionalNeuronSum = 0;
    for (const auto& region : stats.regionStats) {
        regionalNeuronSum += region.neuronCount;
    }
    
    return std::abs(regionalNeuronSum - stats.totalNeurons) <= 1;
}

std::string StatsCollector::exportToJSON(const BrainStats& stats) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"total_neurons\": " << stats.totalNeurons << ",\n";
    ss << "  \"total_synapses\": " << stats.totalSynapses << ",\n";
    ss << "  \"active_neurons\": " << stats.activeNeurons << ",\n";
    ss << "  \"firing_neurons\": " << stats.firingNeurons << ",\n";
    ss << "  \"total_spikes\": " << stats.totalSpikes << ",\n";
    ss << "  \"pending_spikes\": " << stats.pendingSpikes << ",\n";
    ss << "  \"average_firing_rate\": " << stats.averageFiringRate << ",\n";
    ss << "  \"excitation_inhibition_ratio\": " << stats.excitationInhibitionRatio << ",\n";
    ss << "  \"stability_score\": " << stats.stabilityScore << ",\n";
    ss << "  \"learning_score\": " << stats.learningScore << ",\n";
    ss << "  \"coherence_score\": " << stats.coherenceScore << "\n";
    ss << "}\n";
    return ss.str();
}

std::string StatsCollector::exportToCSV(const BrainStats& stats) {
    std::stringstream ss;
    ss << "metric,value\n";
    ss << "total_neurons," << stats.totalNeurons << "\n";
    ss << "total_synapses," << stats.totalSynapses << "\n";
    ss << "active_neurons," << stats.activeNeurons << "\n";
    ss << "firing_neurons," << stats.firingNeurons << "\n";
    ss << "total_spikes," << stats.totalSpikes << "\n";
    ss << "pending_spikes," << stats.pendingSpikes << "\n";
    ss << "average_firing_rate," << stats.averageFiringRate << "\n";
    ss << "excitation_inhibition_ratio," << stats.excitationInhibitionRatio << "\n";
    ss << "stability_score," << stats.stabilityScore << "\n";
    ss << "learning_score," << stats.learningScore << "\n";
    ss << "coherence_score," << stats.coherenceScore << "\n";
    return ss.str();
}

std::string StatsCollector::exportToMarkdown(const BrainStats& stats) {
    std::stringstream ss;
    ss << "# Brain Statistics\n\n";
    ss << "| Metric | Value |\n";
    ss << "|--------|-------|\n";
    ss << "| Total Neurons | " << stats.totalNeurons << " |\n";
    ss << "| Total Synapses | " << stats.totalSynapses << " |\n";
    ss << "| Active Neurons | " << stats.activeNeurons << " |\n";
    ss << "| Firing Neurons | " << stats.firingNeurons << " |\n";
    ss << "| Total Spikes | " << stats.totalSpikes << " |\n";
    ss << "| Pending Spikes | " << stats.pendingSpikes << " |\n";
    ss << "| Average Firing Rate | " << stats.averageFiringRate << " Hz |\n";
    ss << "| E/I Ratio | " << stats.excitationInhibitionRatio << " |\n";
    ss << "| Stability Score | " << stats.stabilityScore << " |\n";
    ss << "| Learning Score | " << stats.learningScore << " |\n";
    ss << "| Coherence Score | " << stats.coherenceScore << " |\n";
    return ss.str();
}

} // namespace nlm
