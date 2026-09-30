#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/SpikeSystem.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool useConsoleOutput;
    
    Impl() : active(false), updateRate(30.0), useConsoleOutput(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization system
    pImpl->active = true;
    NLM_LOG_INFO("Visualization system initialized (console-based)");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update rate limiting
    static double lastTime = 0.0;
    // Simple update rate control - in real implementation would use timing
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // For console-based visualization, render is handled in update
    // In a real GUI implementation, this would render to screen
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("Visualization system closed");
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
    if (!pImpl->active || !pImpl->useConsoleOutput) return;
    
    std::cout << "\n=== NEURAL NETWORK VISUALIZATION ===\n";
    std::cout << "Regions: " << brain.getRegionCount() << "\n";
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    std::cout << "Excitatory/Inhibitory Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
    std::cout << "===================================\n";
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active || !pImpl->useConsoleOutput) return;
    
    std::cout << "\n=== NEURAL ACTIVITY VISUALIZATION ===\n";
    
    // Find most active region
    size_t maxActivity = 0;
    RegionId mostActiveRegion = RegionId(0);
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            size_t activity = 0;
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    activity++;
                }
            }
            if (activity > maxActivity) {
                maxActivity = activity;
                mostActiveRegion = region->getId();
            }
        }
    }
    
    std::cout << "Most Active Region: " << mostActiveRegion.getId() << " (" << maxActivity << " firing neurons)\n";
    
    // Show activity distribution
    std::vector<size_t> activityCounts;
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            size_t activity = 0;
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    activity++;
                }
            }
            activityCounts.push_back(activity);
        }
    }
    
    if (!activityCounts.empty()) {
        size_t maxVal = *std::max_element(activityCounts.begin(), activityCounts.end());
        std::cout << "Activity Distribution: ";
        for (size_t count : activityCounts) {
            int barWidth = (maxVal > 0) ? (count * 50 / maxVal) : 0;
            std::cout << "[" << std::string(barWidth, '=') << "]" << count << " ";
        }
        std::cout << "\n";
    }
    
    std::cout << "===================================\n";
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active || !pImpl->useConsoleOutput) return;
    
    std::cout << "\n=== SPIKE RASTER PLOT ===\n";
    std::cout << "Total Spikes: " << spikes.size() << "\n";
    
    if (spikes.size() > 0) {
        std::cout << "Spike times (ms): ";
        for (size_t i = 0; i < std::min(spikes.size(), size_t(20)); ++i) {
            std::cout << spikes[i].timestamp * 1000 << "ms ";
        }
        if (spikes.size() > 20) {
            std::cout << "... and " << (spikes.size() - 20) << " more";
        }
        std::cout << "\n";
        
        // Show spike statistics
        double avgTime = 0.0;
        for (const auto& spike : spikes) {
            avgTime += spike.timestamp;
        }
        avgTime /= spikes.size();
        std::cout << "Average spike rate: " << (spikes.size() / (avgTime / 1.0)) << " spikes/sec\n";
    }
    
    std::cout << "===============================\n";
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active || !pImpl->useConsoleOutput) return;
    
    std::cout << "\n=== WEIGHT MATRIX VISUALIZATION ===\n";
    
    float minWeight = 1e9f;
    float maxWeight = -1e9f;
    
    // Find weight range
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight < minWeight) minWeight = weight;
            if (weight > maxWeight) maxWeight = weight;
        }
    }
    
    if (maxWeight > minWeight) {
        std::cout << "Weight Range: [" << minWeight << ", " << maxWeight << "]\n";
        
        // Show synapse statistics
        size_t excitatory = 0;
        size_t inhibitory = 0;
        
        for (const auto& region : brain.getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                if (syn->getWeight() > 0) {
                    excitatory++;
                } else {
                    inhibitory++;
                }
            }
        }
        
        std::cout << "Excitatory Synapses: " << excitatory << "\n";
        std::cout << "Inhibitory Synapses: " << inhibitory << "\n";
        
        // Weight distribution histogram
        std::vector<size_t> weightBins(10, 0);
        
        for (const auto& region : brain.getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                float weight = syn->getWeight();
                size_t bin = ((weight - minWeight) / (maxWeight - minWeight) * 9);
                if (bin >= weightBins.size()) bin = weightBins.size() - 1;
                weightBins[bin]++;
            }
        }
        
        std::cout << "Weight Distribution:\n";
        for (size_t i = 0; i < weightBins.size(); ++i) {
            float binMin = minWeight + (i * (maxWeight - minWeight) / weightBins.size());
            float binMax = minWeight + ((i + 1) * (maxWeight - minWeight) / weightBins.size());
            size_t count = weightBins[i];
            std::cout << "  [" << std::fixed << std::setprecision(3) << binMin << ", " << binMax << "] : ";
            for (size_t j = 0; j < count; ++j) {
                std::cout << "*";
            }
            std::cout << " " << count << "\n";
        }
    } else {
        std::cout << "No synapses found\n";
    }
    
    std::cout << "===================================\n";
}

} // namespace nlm
