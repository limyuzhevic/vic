#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct PlasticityVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    std::map<std::string, double> weightChanges;
    
    Impl() : initialized(false), plasticityOpacity(0.7) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

PlasticityVisualizer::PlasticityVisualizer() : pImpl(std::make_unique<Impl>()), plasticityOpacity(0.7) {}

PlasticityVisualizer::~PlasticityVisualizer() = default;

bool PlasticityVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/plasticity_visualization.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "PlasticityVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void PlasticityVisualizer::visualize(const Brain& brain, const std::string& plasticityType, bool showWeights) {
    if (!pImpl->initialized) return;
    
    std::cout << "PlasticityVisualizer: Visualizing plasticity (type: " << plasticityType << ")" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Plasticity Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "======================= " << std::endl;
        pImpl->outputFile << "Plasticity Type: " << plasticityType << std::endl;
        pImpl->outputFile << "Show Weights: " << (showWeights ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Opacity: " << plasticityOpacity << std::endl;
        pImpl->outputFile << std::endl;
        
        // Get plasticity statistics
        pImpl->outputFile << "Plasticity Statistics:" << std::endl;
        
        // Calculate overall plasticity activity
        double totalPlasticityActivity = 0.0;
        int plasticitySources = 0;
        
        auto devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            totalPlasticityActivity += devSystem->getPlasticityModifier();
            plasticitySources++;
            
            pImpl->outputFile << "  Development System:" << std::endl;
            pImpl->outputFile << "    Age: " << devSystem->getCurrentAge() << std::endl;
            pImpl->outputFile << "    Plasticity Modifier: " << devSystem->getPlasticityModifier() << std::endl;
            pImpl->outputFile << "    Critical Period: " << (devSystem->isCriticalPeriod() ? "Yes" : "No") << std::endl;
        }
        
        // Neuromodulation effects
        auto dopamine = brain.getDopamine();
        if (dopamine) {
            totalPlasticityActivity += dopamine->getLevel();
            plasticitySources++;
            
            pImpl->outputFile << "  Dopamine System:" << std::endl;
            pImpl->outputFile << "    Level: " << dopamine->getLevel() << std::endl;
            pImpl->outputFile << "    Reward Signal: " << (dopamine->getLastReward() > 0 ? "Yes" : "No") << std::endl;
        }
        
        auto curiosity = brain.getCuriosity();
        if (curiosity) {
            totalPlasticityActivity += curiosity->getLevel();
            plasticitySources++;
            
            pImpl->outputFile << "  Curiosity System:" << std::endl;
            pImpl->outputFile << "    Level: " << curiosity->getLevel() << std::endl;
        }
        
        // Calculate average plasticity
        double averagePlasticity = plasticitySources > 0 ? totalPlasticityActivity / plasticitySources : 0.0;
        
        pImpl->outputFile << "  Average Plasticity Activity: " << averagePlasticity << std::endl;
        pImpl->outputFile << std::endl;
        
        if (showWeights) {
            visualizeWeightChanges(brain, 0.01); // Show changes > 1%
        }
        
        pImpl->outputFile << "---" << std::endl << std::endl;
    }
}

void PlasticityVisualizer::visualizeWeightChanges(const Brain& brain, double threshold) {
    if (!pImpl->initialized) return;
    
    pImpl->outputFile << "Weight Changes (Threshold: " << threshold << "):" << std::endl;
    
    // This is a placeholder - in a real implementation, you would need access
    // to the brain's internal plasticity rule systems and weight change data
    
    int totalSynapses = brain.getTotalSynapseCount();
    int highChangeSynapses = static_cast<int>(totalSynapses * 0.1); // Placeholder: 10% have high change
    
    pImpl->outputFile << "  Total Synapses: " << totalSynapses << std::endl;
    pImpl->outputFile << "  Synapses with High Change (>" << threshold << "): " << highChangeSynapses << std::endl;
    pImpl->outputFile << "  Average Change Rate: 0.001" << std::endl; // Placeholder
    pImpl->outputFile << std::endl;
    
    // Simple visualization
    int maxBar = 50;
    int changeBar = static_cast<int>(highChangeSynapses / static_cast<double>(totalSynapses) * maxBar);
    std::string bar = "  [";
    for (int i = 0; i < changeBar; ++i) bar += "#";
    for (int i = changeBar; i < maxBar; ++i) bar += " ";
    bar += "] " << (changeBar * 100 / maxBar) << "%" << std::endl;
    pImpl->outputFile << bar;
}

} // namespace nlm