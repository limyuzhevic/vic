#include "VisualizationInterface.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

class ConsoleBrainVisualizer : public VisualizationInterface {
public:
    ConsoleBrainVisualizer() : VisualizationInterface(), pImpl(nullptr) {}
    ~ConsoleBrainVisualizer() override = default;
    
    bool initialize() override {
        pImpl = std::make_unique<Impl>();
        pImpl->active = true;
        return true;
    }
    
    void update() override {
        if (!pImpl->active) return;
        
        // Get brain state (this would need to be integrated with the main brain)
        // For now, we'll just simulate some state
        std::string state = "Simulation Step: " + std::to_string(stepCounter++);
        printStatusLine(state);
        
        // Print network statistics
        printNetworkInfo();
        
        // Print activity visualization
        printActivityBar();
        
        // Print firing statistics
        printFiringStats();
    }
    
    void render() override {
        if (!pImpl->active) return;
        
        // Clear screen
        system("clear");
        
        std::cout << "=== NLM Brain Visualization ===\n\n";
        
        if (updateCounter % 10 == 0) {
            std::cout << "Simulation Time: " << currentTime << "s" << std::endl;
            std::cout << "Step Count: " << stepCounter << std::endl;
            std::cout << "Network State: " << getNetworkState() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void close() override {
        pImpl->active = false;
        std::cout << "\nVisualization closed.\n";
    }
    
    bool isActive() const override {
        return pImpl->active;
    }
    
    void setUpdateRate(double hz) override {
        pImpl->updateRate = hz;
        pImpl->updateInterval = 1.0 / hz;
    }
    
    double getUpdateRate() const override {
        return pImpl->updateRate;
    }
    
    // Visualization-specific methods
    void visualizeNetwork(const Brain& brain) override {
        printNetworkDiagram(brain);
    }
    
    void visualizeActivity(const Brain& brain) override {
        printActivityPlot(brain);
    }
    
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) override {
        printSpikeRaster(spikes);
    }
    
    void visualizeWeights(const Brain& brain) override {
        printWeightMatrix(brain);
    }

private:
    struct Impl {
        bool active;
        double updateRate;
        double updateInterval;
        double lastUpdateTime;
        int stepCounter;
        int updateCounter;
        double currentTime;
        
        Impl() : active(false), updateRate(30.0), updateInterval(1.0/30.0), 
                 lastUpdateTime(0.0), stepCounter(0), updateCounter(0), currentTime(0.0) {}
    };
    
    std::unique_ptr<Impl> pImpl;
    
    void printStatusLine(const std::string& status) {
        std::cout << "[STATUS] " << status << "\n";
    }
    
    void printNetworkInfo() {
        std::cout << "\n=== Network Information ===" << std::endl;
        std::cout << "Neurons: " << neuronCount << " | Regions: " << regionCount << std::endl;
        std::cout << "Total Synapses: " << totalSynapses << std::endl;
        std::cout << "Active Neurons: " << activeNeurons << std::endl;
        std::cout << "Firing Neurons: " << firingNeurons << std::endl;
        std::cout << "E/I Ratio: " << eiRatio << std::endl;
    }
    
    void printActivityBar() {
        std::cout << "\n=== Activity Visualization ===" << std::endl;
        std::cout << "["; // Start bar
        int barWidth = 50;
        int activityLevel = static_cast<int>(std::min(100.0, totalSpikes / 10.0));
        for (int i = 0; i < barWidth; ++i) {
            if (i < activityLevel) {
                std::cout << "=";
            } else if (i == activityLevel) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << activityLevel << "% (Total Spikes: " << totalSpikes << ")" << std::endl;
    }
    
    void printFiringStats() {
        std::cout << "\n=== Firing Statistics ===" << std::endl;
        std::cout << "Average Firing Rate: " << avgFiringRate << " Hz" << std::endl;
        std::cout << "Peak Firing Rate: " << peakFiringRate << " Hz" << std::endl;
        std::cout << "Refractory Neurons: " << refractoryNeurons << std::endl;
    }
    
    std::string getNetworkState() {
        if (totalSpikes < 100) return "Initializing...";
        else if (totalSpikes < 1000) return "Active";
        else if (totalSpikes < 5000) return "High Activity";
        else return "Burst Activity";
    }
    
    void printNetworkDiagram(const Brain& brain) {
        std::cout << "\n=== Network Diagram ===" << std::endl;
        
        // Simple text-based network diagram
        int regionCount = brain.getRegionCount();
        std::cout << "Regions: ";
        for (int i = 1; i <= regionCount; ++i) {
            auto* region = brain.getRegion(RegionId(i));
            if (region) {
                std::cout << "R" << i << "(";
                std::cout << region->getTotalNeuronCount() << " neurons, ";
                std::cout << region->getSynapseCount() << " synapses) ";
            }
        }
        std::cout << std::endl;
        
        // Show some connection information
        std::cout << "Connections: " << brain.getTotalSynapseCount() << " total" << std::endl;
    }
    
    void printActivityPlot(const Brain& brain) {
        std::cout << "\n=== Activity Plot ===" << std::endl;
        
        // Show activity over time (simplified)
        int plotWidth = 60;
        int timeSteps = 10;
        
        std::cout << "Time [";
        for (int t = 0; t < plotWidth; ++t) {
            int timeIndex = (t * timeSteps) / plotWidth;
            std::cout << (timeIndex % 10);
            if (t < plotWidth - 1) std::cout << " ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "[|";
        for (int t = 0; t < plotWidth; ++t) {
            double activity = getActivityAtTime(t);
            int barHeight = static_cast<int>(activity * 10.0);
            for (int i = 0; i < barHeight; ++i) {
                std::cout << "#";
            }
            if (t < plotWidth - 1) std::cout << " ";
        }
        std::cout << "|]" << std::endl;
    }
    
    void printSpikeRaster(const std::vector<SpikeEvent>& spikes) {
        std::cout << "\n=== Spike Raster ===" << std::endl;
        std::cout << "Showing last " << std::min(20, (int)spikes.size()) << " spikes:" << std::endl;
        
        int displayCount = std::min(20, (int)spikes.size());
        for (int i = std::max(0, (int)spikes.size() - displayCount); i < (int)spikes.size(); ++i) {
            const auto& spike = spikes[i];
            std::cout << "Neuron " << spike.neuronId.index() << " @ t=" 
                      << spike.timestamp << "s, step=" << spike.step << std::endl;
        }
    }
    
    void printWeightMatrix(const Brain& brain) {
        std::cout << "\n=== Weight Matrix (Sample) ===" << std::endl;
        
        // Show a small sample of the weight matrix
        auto* region = brain.getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                std::cout << "Sample weight from Neuron " << neurons[0]->getId().index() 
                          << " to Neuron " << neurons[1]->getId().index() 
                          << ": " << neurons[1]->getTotalCurrent() << std::endl;
            }
        }
        
        std::cout << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    }
    
    double getActivityAtTime(int timeIndex) {
        // Simulate activity based on time
        double baseActivity = 0.1;
        double oscillation = std::sin(timeIndex * 0.5) * 0.3;
        double noise = ((rand() % 100) / 100.0 - 0.5) * 0.1;
        return std::max(0.0, baseActivity + oscillation + noise);
    }
    
    // Simulation statistics (these would be updated from the actual brain simulation)
    int neuronCount = 1000;
    int regionCount = 2;
    int totalSynapses = 15000;
    int activeNeurons = 234;
    int firingNeurons = 45;
    double eiRatio = 1.8;
    double avgFiringRate = 2.4;
    double peakFiringRate = 8.2;
    int refractoryNeurons = 12;
    double totalSpikes = 1234.5;
};

} // namespace nlm