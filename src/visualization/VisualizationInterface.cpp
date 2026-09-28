#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool textMode;  // Use text-based visualization instead of graphical
    int width;      // Terminal width for text visualization
    int height;     // Terminal height for text visualization
    
    Impl() : active(false), updateRate(30.0), textMode(true), width(80), height(40) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization with text-based mode for simplicity
    // Users can optionally enable graphical mode with external libraries
    pImpl->active = true;
    pImpl->textMode = true; // Default to text-based for portability
    NLM_LOG_INFO("Visualization interface initialized (text mode)");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update logic for visualization
    // In text mode, this could update internal state
    // In graphical mode, this would handle frame updates
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Render visualization
    if (pImpl->textMode) {
        renderTextVisualization();
    } else {
        // Placeholder for graphical rendering
        // Would use GLFW, SDL, or other graphics library
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("Visualization interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (hz <= 0.0) pImpl->updateRate = 30.0;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    // Create network visualization
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NETWORK VISUALIZATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Basic network statistics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    
    std::cout << "Network Statistics:" << std::endl;
    std::cout << "  Total Neurons:      " << totalNeurons << std::endl;
    std::cout << "  Total Synapses:     " << totalSynapses << std::endl;
    std::cout << "  Active Neurons:     " << activeNeurons << std::endl;
    std::cout << "  Firing Neurons:    " << firingNeurons << std::endl;
    std::cout << "  Average Firing Rate:" << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    // Region information
    auto regionIds = brain.getRegionIds();
    std::cout << "\nRegions:" << std::endl;
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain.getRegion(regionIds[i]);
        if (region) {
            size_t regionNeurons = region->getAllNeurons().size();
            size_t regionSynapses = region->getSynapses().size();
            std::cout << "  Region " << i << " (ID: " << regionIds[i] << "):" << std::endl;
            std::cout << "    Neurons: " << regionNeurons 
                     << ", Synapses: " << regionSynapses << std::endl;
        }
    }
    
    // Connection density
    if (totalNeurons > 0 && totalSynapses > 0) {
        double density = static_cast<double>(totalSynapses) / 
                        (static_cast<double>(totalNeurons) * totalNeurons);
        std::cout << "\nNetwork Density: " << (density * 100.0) << "%" << std::endl;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NEURAL ACTIVITY VISUALIZATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Get activity data
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    size_t totalSpikes = brain.getTotalSpikeCount();
    
    // Activity level histogram
    std::cout << "Activity Distribution:" << std::endl;
    
    // Create a simple bar chart of neuron activity
    int barWidth = 40;
    int activeBarWidth = static_cast<int>((static_cast<double>(activeNeurons) / totalNeurons) * barWidth);
    int firingBarWidth = static_cast<int>((static_cast<double>(firingNeurons) / totalNeurons) * barWidth);
    
    std::cout << "  Active:    ";
    for (int i = 0; i < activeBarWidth; ++i) std::cout << "█";
    std::cout << " " << activeNeurons << " (" 
              << std::fixed << std::setprecision(1) 
              << (static_cast<double>(activeNeurons) / totalNeurons * 100.0) << "%)" << std::endl;
    
    std::cout << "  Firing:    ";
    for (int i = 0; i < firingBarWidth; ++i) std::cout << "░";
    std::cout << " " << firingNeurons << " (" 
              << std::fixed << std::setprecision(1)
              << (static_cast<double>(firingNeurons) / totalNeurons * 100.0) << "%)" << std::endl;
    
    // Firing rate statistics
    float avgFiringRate = brain.getAverageFiringRate();
    std::cout << "\nFiring Rate Statistics:" << std::endl;
    std::cout << "  Average:   " << avgFiringRate << " Hz" << std::endl;
    std::cout << "  Total Spikes: " << totalSpikes << std::endl;
    
    // Performance metrics
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "  Update Rate:    " << pImpl->updateRate << " Hz" << std::endl;
    double fps = pImpl->updateRate;
    std::cout << "  Target FPS:     " << static_cast<int>(fps) << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "SPIKE HISTORY" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded." << std::endl;
    } else {
        // Sort spikes by timestamp
        std::vector<SpikeEvent> sortedSpikes = spikes;
        std::sort(sortedSpikes.begin(), sortedSpikes.end(),
                 [](const SpikeEvent& a, const SpikeEvent& b) { return a.timestamp < b.timestamp; });
        
        // Spike statistics
        size_t totalSpikes = spikes.size();
        double avgInterval = 0.0;
        if (totalSpikes > 1) {
            double totalDuration = sortedSpikes.back().timestamp - sortedSpikes.front().timestamp;
            avgInterval = totalDuration / (totalSpikes - 1);
        }
        
        std::cout << "Spike Statistics:" << std::endl;
        std::cout << "  Total Spikes:    " << totalSpikes << std::endl;
        std::cout << "  Average Interval:" << avgInterval << " ms" << std::endl;
        std::cout << "  First Spike:     " << sortedSpikes.front().timestamp << " ms" << std::endl;
        std::cout << "  Last Spike:      " << sortedSpikes.back().timestamp << " ms" << std::endl;
        
        // Create simple spike timeline (showing last 20 spikes)
        size_t spikesToShow = std::min(size_t(20), sortedSpikes.size());
        std::cout << "\nRecent Spikes (last " << spikesToShow << "):" << std::endl;
        
        for (size_t i = sortedSpikes.size() - spikesToShow; i < sortedSpikes.size(); ++i) {
            const SpikeEvent& spike = sortedSpikes[i];
            std::cout << "  " << std::setw(6) << spike.timestamp << " ms";
            
            // Neuron type indicator
            std::string typeStr;
            switch (spike.neuronType) {
                case NeuronType::Sensory: typeStr = "SEN"; break;
                case NeuronType::Motor:   typeStr = "MOT"; break;
                case NeuronType::Internal: typeStr = "INT"; break;
                case NeuronType::Executive: typeStr = "EXC"; break;
                default: typeStr = "UNK"; break;
            }
            std::cout << "  Type: " << typeStr;
            std::cout << "  Region: " << spike.regionId << std::endl;
        }
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "SYNAPTIC WEIGHTS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto regionIds = brain.getRegionIds();
    if (regionIds.empty()) {
        std::cout << "No regions found." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain.getRegion(regionIds[i]);
        if (!region) continue;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) continue;
        
        std::cout << "\nRegion " << i << " (" << synapses.size() << " synapses):" << std::endl;
        
        // Weight statistics
        double minWeight = std::numeric_limits<float>::max();
        double maxWeight = std::numeric_limits<float>::lowest();
        double sumWeight = 0.0;
        
        for (const auto& syn : synapses) {
            float weight = syn->getWeight();
            minWeight = std::min(minWeight, static_cast<double>(weight));
            maxWeight = std::max(maxWeight, static_cast<double>(weight));
            sumWeight += weight;
        }
        
        double avgWeight = sumWeight / synapses.size();
        
        std::cout << "  Min Weight:  " << minWeight << std::endl;
        std::cout << "  Max Weight:  " << maxWeight << std::endl;
        std::cout << "  Avg Weight:  " << avgWeight << std::endl;
        std::cout << "  Range:       " << (maxWeight - minWeight) << std::endl;
        
        // Weight histogram
        std::cout << "  Weight Distribution:" << std::endl;
        int histogramWidth = 40;
        
        // Count weights in ranges
        size_t negativeCount = 0, zeroCount = 0, positiveCount = 0;
        for (const auto& syn : synapses) {
            float weight = syn->getWeight();
            if (weight < -0.1) negativeCount++;
            else if (weight > 0.1) positiveCount++;
            else zeroCount++;
        }
        
        // Create histogram
        size_t totalCounted = negativeCount + zeroCount + positiveCount;
        if (totalCounted > 0) {
            int negWidth = static_cast<int>((static_cast<double>(negativeCount) / totalCounted) * histogramWidth);
            int zeroWidth = static_cast<int>((static_cast<double>(zeroCount) / totalCounted) * histogramWidth);
            int posWidth = static_cast<int>((static_cast<double>(positiveCount) / totalCounted) * histogramWidth);
            
            std::cout << "    [-1,0):   ";
            for (int j = 0; j < negWidth; ++j) std::cout << "░";
            std::cout << " " << negativeCount << std::endl;
            
            std::cout << "    [0,0]:    ";
            for (int j = 0; j < zeroWidth; ++j) std::cout << "▒";
            std::cout << " " << zeroCount << std::endl;
            
            std::cout << "    (0,1]:    ";
            for (int j = 0; j < posWidth; ++j) std::cout << "▄";
            std::cout << " " << positiveCount << std::endl;
        }
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void VisualizationInterface::renderTextVisualization() {
    // Simple text-based visualization using escape codes
    std::cout << "\033[2J\033[H" << std::endl;  // Clear screen and move cursor to top-left
    std::cout << "NLM - Neural Learning Machine Visualization" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Press 'q' to quit, 's' to toggle between text/graphical modes" << std::endl;
    
    // Update and render would be called in main loop
    // For now, just show a welcome message
}

} // namespace nlm
