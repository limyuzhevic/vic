#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string lastOutput;
    
    Impl() : active(false), updateRate(30.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize text-based visualization
    pImpl->active = true;
    pImpl->lastOutput = "\n=== NLM Visualization Interface Initialized ===\n";
    return true;
}

void VisualizationInterface::update() {
    // Update visualization data (placeholder for now)
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    std::cout << pImpl->lastOutput;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    pImpl->lastOutput = "\n=== Visualization Interface Closed ===\n";
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::ostringstream oss;
    oss << "\n=== Network Visualization ===\n";
    
    oss << "Regions: " << brain.getRegionCount() << "\n";
    oss << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    oss << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    oss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    oss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    oss << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
    oss << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    oss << "Excitatory/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    oss << "Current Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n\n";
    
    // Show region breakdown
    auto regionIds = brain.getRegionIds();
    oss << "Region Details:";
    for (const auto& id : regionIds) {
        auto region = brain.getRegion(id);
        if (region) {
            oss << "\n  Region " << id.value << ": ";
            oss << region->getName() << " (" << region->getPopulationCount() << " populations, ";
            oss << region->getNeuronCount() << " neurons, " << region->getSynapseCount() << " synapses)";
        }
    }
    oss << "\n\n";
    
    pImpl->lastOutput = oss.str();
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    std::ostringstream oss;
    oss << "\n=== Activity Visualization ===\n";
    
    // Create activity histogram
    int maxFiringRate = 100; // Max firing rate for histogram scaling
    int histogramWidth = 50;
    
    oss << "Firing Rate Histogram:" << std::endl;
    oss << std::string(histogramWidth, '-') << std::endl;
    
    // Get population firing rates (simplified - using neuron count as proxy)
    double avgFiringRate = brain.getAverageFiringRate();
    int barHeight = static_cast<int>(std::min(avgFiringRate * 10, 50.0)); // Scale for display
    
    oss << std::string(histogramWidth * barHeight / 100, '=') << " " << avgFiringRate << " Hz\n";
    oss << std::string(histogramWidth, '-') << std::endl;
    
    oss << "Network Activity Summary:" << std::endl;
    oss << "  Synchronous Firing: " << (brain.getFiringNeuronCount() > 0 ? "Yes" : "No") << std::endl;
    oss << "  Peak Activity Detected: " << (brain.getActiveNeuronCount() > 0 ? "Yes" : "No") << std::endl;
    oss << "  Network Engagement: " << (brain.getFiringNeuronCount() / std::max(brain.getTotalNeuronCount(), 1)) * 100 << "%\n\n";
    
    pImpl->lastOutput = oss.str();
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::ostringstream oss;
    oss << "\n=== Spike Raster Visualization ===\n";
    
    if (spikes.empty()) {
        oss << "No spikes recorded.\n\n";
        pImpl->lastOutput = oss.str();
        return;
    }
    
    oss << "Spike Summary:" << std::endl;
    oss << "  Total Spikes: " << spikes.size() << std::endl;
    
    // Count spikes by neuron (simplified)
    std::unordered_map<uint64_t, int> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId.value]++;
    }
    
    oss << "  Neurons Firing: " << spikesByNeuron.size() << std::endl;
    oss << "  Average Spikes per Neuron: " 
        << (spikes.size() / std::max(spikesByNeuron.size(), 1)) << std::endl;
    
    // Find peak firing neuron
    uint64_t maxSpikes = 0;
    uint64_t peakNeuron = INVALID_NEURON_ID;
    for (const auto& [neuronId, count] : spikesByNeuron) {
        if (count > maxSpikes) {
            maxSpikes = count;
            peakNeuron = neuronId;
        }
    }
    
    oss << "  Peak Firing Neuron: " << peakNeuron.value << " (" << maxSpikes << " spikes)\n\n";
    
    oss << "Recent Spike Timeline (last 20 spikes):" << std::endl;
    size_t recentCount = std::min(spikes.size(), size_t(20));
    for (size_t i = 0; i < recentCount; ++i) {
        const auto& spike = spikes[spikes.size() - 1 - i];
        oss << "  Time " << spike.timestamp << ": Neuron " << spike.neuronId.value
            << " (Pop " << spike.populationId.value << ")\n";
    }
    oss << "\n";
    
    pImpl->lastOutput = oss.str();
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::ostringstream oss;
    oss << "\n=== Weight Matrix Visualization ===\n";
    
    oss << "Synaptic Weight Statistics:" << std::endl;
    
    // Get all synapses to compute statistics
    std::vector<float> allWeights;
    for (const auto& region : brain.getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            allWeights.push_back(syn->getWeight());
        }
    }
    
    if (allWeights.empty()) {
        oss << "No synapses in the network.\n\n";
        pImpl->lastOutput = oss.str();
        return;
    }
    
    // Compute statistics
    float minWeight = *std::min_element(allWeights.begin(), allWeights.end());
    float maxWeight = *std::max_element(allWeights.begin(), allWeights.end());
    double avgWeight = std::accumulate(allWeights.begin(), allWeights.end(), 0.0f) / allWeights.size();
    
    oss << "  Total Synaptic Weights: " << allWeights.size() << std::endl;
    oss << "  Minimum Weight: " << minWeight << std::endl;
    oss << "  Maximum Weight: " << maxWeight << std::endl;
    oss << "  Average Weight: " << avgWeight << std::endl;
    oss << "  Weight Range: " << (maxWeight - minWeight) << std::endl;
    
    // Create simple weight histogram
    int histogramBins = 10;
    float binWidth = (maxWeight - minWeight) / histogramBins;
    std::vector<int> binCounts(histogramBins, 0);
    
    for (float weight : allWeights) {
        int bin = static_cast<int>((weight - minWeight) / binWidth);
        if (bin == histogramBins) bin = histogramBins - 1; // Handle edge case
        if (bin >= 0 && bin < histogramBins) {
            binCounts[bin]++;
        }
    }
    
    oss << "\nWeight Distribution Histogram:" << std::endl;
    for (int bin = 0; bin < histogramBins; ++bin) {
        float binMin = minWeight + bin * binWidth;
        float binMax = binMin + binWidth;
        int count = binCounts[bin];
        int barWidth = (count * 40) / std::max(allWeights.size(), 1); // Scale for display
        
        oss << "  [" << std::fixed << std::setprecision(3) << binMin << " - " << binMax << "]";
        oss << " |" << std::string(barWidth, '=') << "| " << count << std::endl;
    }
    
    oss << "\n";
    
    pImpl->lastOutput = oss.str();
}

} // namespace nlm
