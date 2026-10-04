#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/SpikeEvent.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <limits>

namespace nlm {

struct SpikeTrainVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    int spikeCount;
    double timeScale;
    bool initialized;
    
    Impl() : spikeCount(0), timeScale(1.0), initialized(false) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

SpikeTrainVisualizer::SpikeTrainVisualizer() : pImpl(std::make_unique<Impl>()), spikeColor("red"), backgroundColor("white"), gridVisible(true) {}

SpikeTrainVisualizer::~SpikeTrainVisualizer() = default;

bool SpikeTrainVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/spike_trains.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "SpikeTrainVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void SpikeTrainVisualizer::visualize(const std::vector<SpikeEvent>& spikes, double timeScale, int windowWidth, int windowHeight) {
    if (!pImpl->initialized) return;
    
    pImpl->spikeCount++;
    pImpl->timeScale = timeScale;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Spike Train Visualization - Step " << pImpl->spikeCount << std::endl;
        pImpl->outputFile << "============================= " << std::endl;
        pImpl->outputFile << "Spike Count: " << spikes.size() << std::endl;
        pImpl->outputFile << "Time Scale: " << timeScale << std::endl;
        pImpl->outputFile << "Window Size: " << windowWidth << "x" << windowHeight << std::endl;
        pImpl->outputFile << "Spike Color: " << spikeColor << std::endl;
        pImpl->outputFile << "Background Color: " << backgroundColor << std::endl;
        pImpl->outputFile << "Grid Visible: " << (gridVisible ? "Yes" : "No") << std::endl;
        pImpl->outputFile << std::endl;
        
        // Write spike data
        pImpl->outputFile << "Spike Events:" << std::endl;
        for (size_t i = 0; i < spikes.size() && i < 100; ++i) { // Limit to 100 spikes for readability
            const auto& spike = spikes[i];
            pImpl->outputFile << "  Spike " << i << ": Time=" << spike.time 
                              << ", Neuron=" << spike.neuronId.value 
                              << ", Amplitude=" << spike.amplitude << std::endl;
        }
        if (spikes.size() > 100) {
            pImpl->outputFile << "  ... and " << (spikes.size() - 100) << " more spikes" << std::endl;
        }
        
        // Calculate statistics
        double maxTime = 0.0, minTime = std::numeric_limits<double>::max();
        for (const auto& spike : spikes) {
            maxTime = std::max(maxTime, spike.time);
            minTime = std::min(minTime, spike.time);
        }
        
        pImpl->outputFile << std::endl;
        pImpl->outputFile << "Statistics:" << std::endl;
        pImpl->outputFile << "  Total Spikes: " << spikes.size() << std::endl;
        pImpl->outputFile << "  Time Range: " << minTime << " - " << maxTime << " ms" << std::endl;
        if (!spikes.empty()) {
            pImpl->outputFile << "  Average Rate: " << (spikes.size() / (maxTime - minTime) * 1000.0) << " Hz" << std::endl;
        }
        pImpl->outputFile << std::endl << "---" << std::endl << std::endl;
    }
    
    // Terminal output
    std::cout << "SpikeTrainVisualizer: Visualizing " << spikes.size() << " spikes (Step " << pImpl->spikeCount << ")" << std::endl;
    std::cout << "  Time Scale: " << timeScale << "x" << std::endl;
    std::cout << "  Window: " << windowWidth << "x" << windowHeight << " pixels" << std::endl;
    std::cout << "  Color Scheme: " << spikeColor << " on " << backgroundColor << std::endl;
}

} // namespace nlm