#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct RegionActivityVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    std::map<size_t, double> previousActivity;
    
    Impl() : initialized(false) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

RegionActivityVisualizer::RegionActivityVisualizer() : pImpl(std::make_unique<Impl>()) {}

RegionActivityVisualizer::~RegionActivityVisualizer() = default;

bool RegionActivityVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/region_activity.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "RegionActivityVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void RegionActivityVisualizer::visualize(const Brain& brain, bool showHeatmap, bool showLabels) {
    if (!pImpl->initialized) return;
    
    std::cout << "RegionActivityVisualizer: Visualizing brain region activity" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Brain Region Activity Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "=========================================== " << std::endl;
        pImpl->outputFile << "Show Heatmap: " << (showHeatmap ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Labels: " << (showLabels ? "Yes" : "No") << std::endl;
        pImpl->outputFile << std::endl;
        
        auto regionIds = brain.getRegionIds();
        pImpl->outputFile << "Region Activity Summary:" << std::endl;
        
        for (size_t regionId : regionIds) {
            const auto* region = brain.getRegion(regionId);
            if (!region) continue;
            
            double activityLevel = 0.0;
            int neuronCount = region->getNeuronIds().size();
            
            // Calculate activity level based on neuron states
            for (const auto& neuronId : region->getNeuronIds()) {
                const auto* neuron = brain.getNeuron(neuronId);
                if (neuron) {
                    activityLevel += neuron->getMembranePotential();
                }
            }
            
            if (!neuronCount) {
                activityLevel = 0.0;
            } else {
                activityLevel /= neuronCount;
            }
            
            // Store for comparison
            pImpl->previousActivity[regionId] = activityLevel;
            
            pImpl->outputFile << "  Region " << regionId << ": " << neuronCount << " neurons, "
                              << "Activity = " << activityLevel << " mV" << std::endl;
            
            if (showHeatmap) {
                // Simple heatmap visualization using ASCII art
                int heatmapBar = static_cast<int>(activityLevel / 100.0 * 50); // Normalize to 50 chars
                std::string heatmap = "  [";
                for (int i = 0; i < heatmapBar; ++i) heatmap += "=";
                for (int i = heatmapBar; i < 50; ++i) heatmap += " ";
                heatmap += "] " << activityLevel << std::endl;
                pImpl->outputFile << heatmap;
            }
        }
        
        pImpl->outputFile << std::endl;
        
        // Activity change analysis
        if (!pImpl->previousActivity.empty()) {
            pImpl->outputFile << "Activity Changes:" << std::endl;
            for (const auto& pair : pImpl->previousActivity) {
                double change = pair.second - pImpl->previousActivity[pair.first];
                pImpl->outputFile << "  Region " << pair.first << ": Change = " << change << " mV" << std::endl;
            }
            pImpl->outputFile << std::endl;
        }
        
        pImpl->outputFile << "---" << std::endl << std::endl;
    }
}

void RegionActivityVisualizer::updateRegionActivity(size_t regionId, double activityLevel) {
    pImpl->previousActivity[regionId] = activityLevel;
}

} // namespace nlm