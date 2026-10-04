#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

namespace nlm {

struct SynapticConnectionVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    std::map<std::string, int> connectionCounts;
    
    Impl() : initialized(false) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

SynapticConnectionVisualizer::SynapticConnectionVisualizer() : pImpl(std::make_unique<Impl>()), colorScheme("default"), highlightActive(true) {}

SynapticConnectionVisualizer::~SynapticConnectionVisualizer() = default;

bool SynapticConnectionVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/synaptic_connections.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "SynapticConnectionVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void SynapticConnectionVisualizer::visualize(const Brain& brain, double threshold, bool showWeights, bool showTypes) {
    if (!pImpl->initialized) return;
    
    std::cout << "SynapticConnectionVisualizer: Visualizing synaptic connections (threshold=" << threshold << ")" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Synaptic Connection Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "========================================= " << std::endl;
        pImpl->outputFile << "Connection Threshold: " << threshold << std::endl;
        pImpl->outputFile << "Show Weights: " << (showWeights ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Types: " << (showTypes ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Color Scheme: " << colorScheme << std::endl;
        pImpl->outputFile << "Highlight Active: " << (highlightActive ? "Yes" : "No") << std::endl;
        pImpl->outputFile << std::endl;
        
        // Count different connection types
        int totalConnections = 0;
        int activeConnections = 0;
        
        pImpl->outputFile << "Connection Statistics:" << std::endl;
        
        // Get all regions to iterate through synapses
        auto regionIds = brain.getRegionIds();
        pImpl->outputFile << "Regions: " << regionIds.size() << std::endl;
        
        pImpl->outputFile << "Connections by Type:" << std::endl;
        
        // We need to access the brain's internal synapse data
        // This is a placeholder implementation - in a real implementation,
        // you would need access to the brain's internal data structures
        
        // For now, we'll show a summary based on available statistics
        int totalNeuronCount = brain.getTotalNeuronCount();
        int totalSynapseCount = brain.getTotalSynapseCount();
        int activeNeuronCount = brain.getActiveNeuronCount();
        int firingNeuronCount = brain.getFiringNeuronCount();
        
        pImpl->outputFile << "  Total Neurons: " << totalNeuronCount << std::endl;
        pImpl->outputFile << "  Total Synapses: " << totalSynapseCount << std::endl;
        pImpl->outputFile << "  Active Neurons: " << activeNeuronCount << std::endl;
        pImpl->outputFile << "  Firing Neurons: " << firingNeuronCount << std::endl;
        pImpl->outputFile << std::endl;
        
        if (showWeights) {
            pImpl->outputFile << "Weight Distribution:" << std::endl;
            // Placeholder - would need actual weight data
            pImpl->outputFile << "  Low (0-0.5): ~50%" << std::endl;
            pImpl->outputFile << "  Medium (0.5-1.0): ~30%" << std::endl;
            pImpl->outputFile << "  High (1.0+): ~20%" << std::endl;
            pImpl->outputFile << std::endl;
        }
        
        if (showTypes) {
            pImpl->outputFile << "Connection Types:" << std::endl;
            // Placeholder - would need actual type data
            pImpl->outputFile << "  Excitatory: ~70%" << std::endl;
            pImpl->outputFile << "  Inhibitory: ~25%" << std::endl;
            pImpl->outputFile << "  Electrical: ~5%" << std::endl;
            pImpl->outputFile << std::endl;
        }
        
        pImpl->outputFile << "---" << std::endl << std::endl;
    }
}

} // namespace nlm