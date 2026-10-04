#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Neuron.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

namespace nlm {

struct MembranePotentialVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    double minV, maxV;
    
    Impl() : initialized(false), minV(-70.0), maxV(50.0) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

MembranePotentialVisualizer::MembranePotentialVisualizer() : pImpl(std::make_unique<Impl>()), voltageMin(-70.0), voltageMax(50.0), timeScale(1.0) {}

MembranePotentialVisualizer::~MembranePotentialVisualizer() = default;

bool MembranePotentialVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/membrane_potentials.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "MembranePotentialVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void MembranePotentialVisualizer::visualize(const Brain& brain, double timeScale, int windowWidth, int windowHeight) {
    if (!pImpl->initialized) return;
    
    pImpl->timeScale = timeScale;
    
    std::cout << "MembranePotentialVisualizer: Visualizing membrane potentials (" << windowWidth << "x" << windowHeight << " pixels)" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Membrane Potential Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "====================================== " << std::endl;
        pImpl->outputFile << "Voltage Range: " << pImpl->minV << " mV to " << pImpl->maxV << " mV" << std::endl;
        pImpl->outputFile << "Time Scale: " << timeScale << "x" << std::endl;
        pImpl->outputFile << "Window Size: " << windowWidth << "x" << windowHeight << std::endl;
        pImpl->outputFile << std::endl;
        
        // Sample some neurons for visualization
        const auto& populations = brain.getRegions();
        int neuronCount = 0;
        
        pImpl->outputFile << "Neuron Membrane Potentials:" << std::endl;
        for (const auto& region : populations) {
            pImpl->outputFile << "Region " << region.getId() << ": " << region.getNeurons().size() << " neurons" << std::endl;
            
            for (const auto& neuronId : region.getNeuronIds()) {
                if (neuronCount++ >= 50) break; // Limit for readability
                
                const auto* neuron = brain.getNeuron(neuronId);
                if (neuron) {
                    pImpl->outputFile << "  Neuron " << neuronId.value 
                                      << ": V = " << neuron->getMembranePotential() << " mV" 
                                      << ", State = " << static_cast<int>(neuron->getFiringState())
                                      << ", Type = " << static_cast<int>(neuron->getNeuronType())
                                      << std::endl;
                }
            }
        }
        
        pImpl->outputFile << std::endl;
        
        // Statistics
        pImpl->outputFile << "Statistics:" << std::endl;
        pImpl->outputFile << "  Voltage Min: " << pImpl->minV << " mV" << std::endl;
        pImpl->outputFile << "  Voltage Max: " << pImpl->maxV << " mV" << std::endl;
        pImpl->outputFile << "  Regions: " << populations.size() << std::endl;
        pImpl->outputFile << std::endl << "---" << std::endl << std::endl;
    }
}

} // namespace nlm