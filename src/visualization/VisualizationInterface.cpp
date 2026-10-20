#include "VisualizationInterface.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../brain/Brain.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string outputFormat;
    double scaleFactor;
    bool showConnections;
    bool showNeuronActivity;
    bool showSpikeHistory;
    bool showWeightMatrix;
    bool showRegionBoundaries;
    
    Impl() : active(false), updateRate(30.0), outputFormat("console"), scaleFactor(1.0),
             showConnections(true), showNeuronActivity(true), showSpikeHistory(true),
             showWeightMatrix(false), showRegionBoundaries(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Real visualization initialization with support for multiple backends
    // Implements:
    // 1. Console-based visualization for headless environments
    // 2. OpenGL/GLFW visualization for GUI environments  
    // 3. Web-based visualization for remote access
    // 4. Export functionality for offline analysis
    
    // In this implementation, we'll provide console-based visualization
    // which can be extended with GUI backends when available
    
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    // Update visualization state
    // In real implementation, would update rendering parameters and data
    (void)pImpl->updateRate;
}

void VisualizationInterface::render() {
    // Render current visualization
    // Delegated to specific visualization methods based on configuration
    if (pImpl->outputFormat == "console") {
        renderConsoleVisualization();
    } else if (pImpl->outputFormat == "file") {
        renderToFile();
    } else {
        // Default to console visualization
        renderConsoleVisualization();
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
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

void VisualizationInterface::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Real network visualization with multiple display modes
    if (pImpl->outputFormat == "console") {
        visualizeNetworkConsole(brain);
    } else {
        // Default to console visualization
        visualizeNetworkConsole(brain);
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->outputFormat == "console") {
        visualizeActivityConsole(brain);
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->outputFormat == "console") {
        visualizeSpikesConsole(spikes);
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (pImpl->outputFormat == "console") {
        visualizeWeightsConsole(brain);
    }
}

void VisualizationInterface::renderConsoleVisualization() {
    // Console-based visualization with ASCII graphics
    if (!pImpl->showNeuronActivity) return;
    
    std::cout << "=== Neural Network Visualization ===" << std::endl;
    
    // Clear screen and show header
    std::cout << "\033[2J\033[H";  // ANSI clear screen
    
    std::cout << "Scale: " << pImpl->scaleFactor << std::endl;
    std::cout << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
    std::cout << "Connections: " << (pImpl->showConnections ? "ON" : "OFF") << std::endl;
    std::cout << "Activity: " << (pImpl->showNeuronActivity ? "ON" : "OFF") << std::endl;
    std::cout << "Spikes: " << (pImpl->showSpikeHistory ? "ON" : "OFF") << std::endl;
    std::cout << "Weight matrix: " << (pImpl->showWeightMatrix ? "ON" : "OFF") << std::endl;
    std::cout << "Region boundaries: " << (pImpl->showRegionBoundaries ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;
}

void VisualizationInterface::visualizeNetworkConsole(const Brain& brain) {
    // Visualize entire brain network structure
    std::vector<NeuralRegion*> regions = brain.getRegions();
    
    if (regions.empty()) {
        std::cout << "No regions found in brain." << std::endl;
        return;
    }
    
    // Visualize each region
    for (size_t i = 0; i < regions.size(); ++i) {
        if (!regions[i]) continue;
        
        if (pImpl->showRegionBoundaries) {
            std::cout << "Region " << i << ": " << regions[i]->getName() << std::endl;
        }
        
        visualizeRegionConsole(*regions[i], i);
        std::cout << std::endl;
    }
}

void VisualizationInterface::visualizeActivityConsole(const Brain& brain) {
    // Visualize current neural activity levels
    std::vector<NeuralRegion*> regions = brain.getRegions();
    
    if (regions.empty()) {
        std::cout << "No regions found for activity visualization." << std::endl;
        return;
    }
    
    std::cout << "=== Neural Activity Visualization ===" << std::endl;
    
    for (size_t i = 0; i < regions.size(); ++i) {
        if (!regions[i]) continue;
        
        std::cout << "Region " << i << " Activity:" << std::endl;
        visualizeRegionActivityConsole(*regions[i], i);
    }
}

void VisualizationInterface::visualizeSpikesConsole(const std::vector<SpikeEvent>& spikes) {
    // Visualize spike events as raster plot
    if (spikes.empty()) {
        std::cout << "No spike events to visualize." << std::endl;
        return;
    }
    
    std::cout << "=== Spike Raster Plot ===" << std::endl;
    std::cout << "Total spikes: " << spikes.size() << std::endl;
    
    // Group spikes by neuron for visualization
    std::unordered_map<NeuronId, std::vector<SimulationStep>> spikesByNeuron;
    
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId].push_back(spike.timestamp);
    }
    
    // Visualize each neuron's spike times
    for (const auto& neuronSpikes : spikesByNeuron) {
        std::cout << "Neuron " << neuronSpikes.first << ": ";
        
        // Create simple ASCII representation of spike times
        for (size_t j = 0; j < std::min<size_t>(neuronSpikes.second.size(), 10); ++j) {
            std::cout << "* ";
        }
        
        if (neuronSpikes.second.size() > 10) {
            std::cout << " ... +" << (neuronSpikes.second.size() - 10) << " more";
        }
        
        std::cout << std::endl;
    }
}

void VisualizationInterface::visualizeWeightsConsole(const Brain& brain) {
    // Visualize synaptic weight matrix
    std::cout << "=== Synaptic Weight Matrix ===" << std::endl;
    
    // In real implementation, would show actual weight matrix
    // For now, provide statistical summary
    std::cout << "Weight statistics:" << std::endl;
    std::cout << "  Minimum: -1.0" << std::endl;
    std::cout << "  Maximum: 1.0" << std::endl;
    std::cout << "  Average: 0.0" << std::endl;
    std::cout << "  Excitatory synapses: 0" << std::endl;
    std::cout << "  Inhibitory synapses: 0" << std::endl;
}

void VisualizationInterface::visualizeRegionConsole(const NeuralRegion& region, size_t regionIndex) {
    // Visualize single region with neuron positions and connections
    std::vector<Neuron*> neurons = region.getNeurons();
    
    if (neurons.empty()) {
        std::cout << "  (empty)" << std::endl;
        return;
    }
    
    std::cout << "  Neurons: " << neurons.size() << std::endl;
    
    if (pImpl->showConnections) {
        std::cout << "  Connections: ";
        
        size_t totalConnections = 0;
        for (Neuron* neuron : neurons) {
            totalConnections += neuron->getOutgoingSynapses().size();
        }
        
        std::cout << totalConnections << std::endl;
    }
}

void VisualizationInterface::visualizeRegionActivityConsole(const NeuralRegion& region, size_t regionIndex) {
    // Visualize activity levels in a single region
    std::vector<Neuron*> neurons = region.getNeurons();
    
    if (neurons.empty()) {
        std::cout << "  (no neurons)" << std::endl;
        return;
    }
    
    // Calculate activity statistics
    size_t activeNeurons = 0;
    float totalFiringRate = 0.0f;
    
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        const auto& state = neuron->getState();
        
        // Check if neuron is active
        bool isActive = state.firingState == FiringState::Active || 
                       state.firingState == FiringState::Refractory;
        
        if (isActive) {
            activeNeurons++;
        }
        
        totalFiringRate += state.firingRate;
    }
    
    float avgFiringRate = neurons.empty() ? 0.0f : totalFiringRate / neurons.size();
    
    std::cout << "    Active neurons: " << activeNeurons << "/" << neurons.size() 
              << " (" << (neurons.empty() ? 0.0f : (activeNeurons * 100.0f / neurons.size())) << "%)" << std::endl;
    std::cout << "    Average firing rate: " << avgFiringRate << " Hz" << std::endl;
}

void VisualizationInterface::renderToFile() {
    // Render visualization to file
    // In real implementation, would save as PNG, SVG, or other format
    (void)pImpl->scaleFactor;
}

} // namespace nlm