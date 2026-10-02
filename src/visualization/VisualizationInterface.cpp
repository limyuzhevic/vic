#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../brain/SpikeSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../utils/TextRenderer.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool useTextOutput;
    std::string outputFile;
    
    Impl() : active(false), updateRate(30.0), useTextOutput(true), outputFile("nlm_visualization.txt") {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization system
    pImpl->active = true;
    
    // Try to initialize text output
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile);
        if (file.is_open()) {
            file.close();
            NLM_LOG_INFO("Text visualization initialized: " + pImpl->outputFile);
        }
    }
    
    NLM_LOG_INFO("Visualization interface initialized at " + 
                std::to_string(pImpl->updateRate) + " Hz");
    return true;
}

void VisualizationInterface::update() {
    if (!isActive()) return;
    
    // Update visualization data
    static double lastUpdate = 0.0;
    static double updateInterval = 1.0 / pImpl->updateRate;
    
    // Simple time-based update
}

void VisualizationInterface::render() {
    if (!isActive()) return;
    
    // Render to text file if enabled
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== NLM Visualization Update ===" << std::endl;
            file << "Timestamp: " << std::fixed << std::setprecision(3) 
                 << "TODO: current time" << std::endl;
            file << "" << std::endl;
            file.close();
        }
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("Visualization data saved to: " + pImpl->outputFile);
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    NLM_LOG_INFO("Visualization update rate set to " + std::to_string(hz) + " Hz");
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!isActive()) return;
    
    NLM_LOG_INFO("=== Network Visualization ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active regions: " + std::to_string(brain.getTotalRegionCount()));
    
    // Visualize network topology
    NLM_LOG_INFO("Network structure:");
    for (RegionId i = 0; i < brain.getTotalRegionCount(); ++i) {
        auto* region = brain.getRegion(i);
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(i) + ": " + 
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses");
        }
    }
    
    // Save to text output
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== Network Visualization ===" << std::endl;
            file << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
            file << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
            file << "Regions: " << brain.getTotalRegionCount() << std::endl;
            
            for (RegionId i = 0; i < brain.getTotalRegionCount(); ++i) {
                auto* region = brain.getRegion(i);
                if (region) {
                    file << "Region " << i << ": " << region->getTotalNeuronCount()
                         << " neurons, " << region->getSynapseCount() << " synapses" << std::endl;
                }
            }
            file << "" << std::endl;
            file.close();
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!isActive()) return;
    
    NLM_LOG_INFO("=== Activity Visualization ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + 
                std::to_string(brain.getFiringNeuronCount() * 10.0 / brain.getTotalNeuronCount()) + " Hz");
    
    // Get spike statistics
    auto spikeStats = brain.getSpikeSystem()->getSpikeStatistics();
    
    NLM_LOG_INFO("Spike statistics:");
    NLM_LOG_INFO("  Average interval: " + std::to_string(spikeStats.averageInterval) + " ms");
    NLM_LOG_INFO("  Coefficient of variation: " + std::to_string(spikeStats.coefficientOfVariation));
    
    // Time series data (simplified)
    NLM_LOG_INFO("Activity over time (sampled):");
    for (size_t i = 0; i < std::min(spikeStats.timeSeries.size(), size_t(10)); ++i) {
        NLM_LOG_INFO("    Time " + std::to_string(spikeStats.timeSeries[i].time) + "s: " +
                    std::to_string(spikeStats.timeSeries[i].spikeCount) + " spikes");
    }
    
    // Save to text output
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== Activity Visualization ===" << std::endl;
            file << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
            file << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
            file << "Average firing rate: " << 
                        brain.getFiringNeuronCount() * 10.0 / brain.getTotalNeuronCount() << " Hz" << std::endl;
            
            file << "Spike statistics:" << std::endl;
            file << "  Average interval: " << spikeStats.averageInterval << " ms" << std::endl;
            file << "  Coefficient of variation: " << spikeStats.coefficientOfVariation << std::endl;
            file << "" << std::endl;
            file.close();
        }
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!isActive()) return;
    
    NLM_LOG_INFO("=== Spike Raster Visualization ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(spikes.size()));
    
    if (spikes.empty()) {
        NLM_LOG_INFO("No spikes to visualize.");
        return;
    }
    
    // Analyze spike timing
    std::vector<float> spikeTimes;
    std::vector<int> neuronIds;
    
    for (const auto& spike : spikes) {
        spikeTimes.push_back(spike.timestamp);
        neuronIds.push_back(static_cast<int>(spike.neuronId));
    }
    
    // Sort by time
    std::vector<size_t> indices(spikes.size());
    for (size_t i = 0; i < spikes.size(); ++i) indices[i] = i;
    std::sort(indices.begin(), indices.end(),
              [&spikeTimes](size_t i1, size_t i2) { return spikeTimes[i1] < spikeTimes[i2]; });
    
    // Calculate statistics
    float minTime = spikeTimes[indices[0]];
    float maxTime = spikeTimes[indices.back()];
    float timeRange = maxTime - minTime;
    
    NLM_LOG_INFO("Time range: " + std::to_string(minTime) + " - " + std::to_string(maxTime) + "s");
    NLM_LOG_INFO("Spike rate: " + std::to_string(spikes.size() / (timeRange > 0.0f ? timeRange : 0.001f)) + " spikes/s");
    
    // Create raster plot data (simplified text representation)
    NLM_LOG_INFO("Spike raster (first " + std::to_string(std::min(spikes.size(), size_t(50))) + " spikes):");
    size_t count = 0;
    for (size_t i = 0; i < indices.size() && count < 50; ++i, ++count) {
        size_t idx = indices[i];
        NLM_LOG_INFO("    " + std::to_string(spikeTimes[idx]) + "s: Neuron " + 
                    std::to_string(neuronIds[idx]));
    }
    
    if (spikes.size() > 50) {
        NLM_LOG_INFO("    ... and " + std::to_string(spikes.size() - 50) + " more spikes");
    }
    
    // Save to text output
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== Spike Raster Visualization ===" << std::endl;
            file << "Total spikes: " << spikes.size() << std::endl;
            
            if (timeRange > 0.0f) {
                file << "Spike rate: " << spikes.size() / timeRange << " spikes/s" << std::endl;
            }
            
            file << "Spike times (first " << std::min(spikes.size(), size_t(50)) << "):" << std::endl;
            size_t count = 0;
            for (size_t i = 0; i < indices.size() && count < 50; ++i, ++count) {
                size_t idx = indices[i];
                file << "  " << spikeTimes[idx] << ": Neuron " << neuronIds[idx] << std::endl;
            }
            
            if (spikes.size() > 50) {
                file << "  ... and " << spikes.size() - 50 << " more spikes" << std::endl;
            }
            file << "" << std::endl;
            file.close();
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!isActive()) return;
    
    NLM_LOG_INFO("=== Weight Matrix Visualization ===");
    NLM_LOG_INFO("Analyzing synaptic weights...");
    
    // Collect all synapses
    std::vector<float> weights;
    std::vector<float> prePostDeltas;
    
    for (RegionId i = 0; i < brain.getTotalRegionCount(); ++i) {
        auto* region = brain.getRegion(i);
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
                
                // Calculate pre-post spike timing difference if available
                float delta = syn->getEligibilityTrace(); // Using eligibility as proxy for weight dynamics
                prePostDeltas.push_back(delta);
            }
        }
    }
    
    if (weights.empty()) {
        NLM_LOG_INFO("No synapses to visualize.");
        return;
    }
    
    // Calculate weight statistics
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    float mean = sum / weights.size();
    
    float minWeight = weights[0];
    float maxWeight = weights[0];
    for (float weight : weights) {
        if (weight < minWeight) minWeight = weight;
        if (weight > maxWeight) maxWeight = weight;
    }
    
    float variance = 0.0f;
    for (float weight : weights) {
        variance += (weight - mean) * (weight - mean);
    }
    variance /= weights.size();
    float stddev = std::sqrt(variance);
    
    NLM_LOG_INFO("Weight statistics:");
    NLM_LOG_INFO("  Mean: " + std::to_string(mean));
    NLM_LOG_INFO("  Min: " + std::to_string(minWeight));
    NLM_LOG_INFO("  Max: " + std::to_string(maxWeight));
    NLM_LOG_INFO("  Std dev: " + std::to_string(stddev));
    
    // Weight distribution histogram
    NLM_LOG_INFO("Weight distribution (10 bins):");
    float binWidth = (maxWeight - minWeight) / 10.0f;
    std::vector<size_t> histogram(10, 0);
    
    for (float weight : weights) {
        size_t bin = static_cast<size_t>((weight - minWeight) / binWidth);
        if (bin >= 10) bin = 9;
        histogram[bin]++;
    }
    
    for (size_t i = 0; i < histogram.size(); ++i) {
        float binMin = minWeight + i * binWidth;
        float binMax = binMin + binWidth;
        NLM_LOG_INFO("    [" + std::to_string(binMin) + ", " + std::to_string(binMax) + "]: " +
                    std::to_string(histogram[i]) + " synapses");
    }
    
    // Save to text output
    if (pImpl->useTextOutput) {
        std::ofstream file(pImpl->outputFile, std::ios::app);
        if (file.is_open()) {
            file << "=== Weight Matrix Visualization ===" << std::endl;
            file << "Total synapses: " << weights.size() << std::endl;
            file << "Weight statistics:" << std::endl;
            file << "  Mean: " << mean << std::endl;
            file << "  Min: " << minWeight << std::endl;
            file << "  Max: " << maxWeight << std::endl;
            file << "  Std dev: " << stddev << std::endl;
            file << "" << std::endl;
            file << "Weight distribution:" << std::endl;
            
            for (size_t i = 0; i < histogram.size(); ++i) {
                float binMin = minWeight + i * binWidth;
                float binMax = binMin + binWidth;
                file << "  [" << binMin << ", " << binMax << "]: " << histogram[i] << " synapses" << std::endl;
            }
            file << "" << std::endl;
            file.close();
        }
    }
}

} // namespace nlm
