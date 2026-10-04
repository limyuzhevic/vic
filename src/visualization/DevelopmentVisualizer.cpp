#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct DevelopmentVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    std::vector<double> plasticityHistory;
    
    Impl() : initialized(false) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

DevelopmentVisualizer::DevelopmentVisualizer() : pImpl(std::make_unique<Impl>()), showPlasticity(true), showCriticalPeriod(true) {}

DevelopmentVisualizer::~DevelopmentVisualizer() = default;

bool DevelopmentVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/development_visualization.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "DevelopmentVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void DevelopmentVisualizer::visualize(const Brain& brain, int showStages, bool showTransitions) {
    if (!pImpl->initialized) return;
    
    std::cout << "DevelopmentVisualizer: Visualizing development stages" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Development Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "========================== " << std::endl;
        pImpl->outputFile << "Show Stages: " << showStages << std::endl;
        pImpl->outputFile << "Show Transitions: " << (showTransitions ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Plasticity: " << (showPlasticity ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Critical Period: " << (showCriticalPeriod ? "Yes" : "No") << std::endl;
        pImpl->outputFile << std::endl;
        
        // Get current developmental stage
        auto stage = brain.getDevelopmentalStage();
        pImpl->outputFile << "Current Developmental Stage: " << static_cast<int>(stage) << std::endl;
        pImpl->outputFile << "Stage Name: " << getStageName(stage) << std::endl;
        
        // Show stage progression
        if (showStages > 0) {
            pImpl->outputFile << std::endl << "Stage Progression:" << std::endl;
            for (int i = static_cast<int>(DevelopmentalStage::Initial); 
                 i <= static_cast<int>(DevelopmentalStage::Aging); ++i) {
                auto devStage = static_cast<DevelopmentalStage>(i);
                bool isCurrent = (devStage == stage);
                std::string marker = isCurrent ? " -> " : "    ";
                
                pImpl->outputFile << marker << "Stage " << i << ": " << getStageName(devStage) << std::endl;
                
                if (isCurrent && showTransitions) {
                    // Show transition details
                    auto devSystem = brain.getDevelopmentSystem();
                    if (devSystem) {
                        pImpl->outputFile << "      Plasticity Modifier: " << devSystem->getPlasticityModifier() << std::endl;
                        pImpl->outputFile << "      Critical Period: " << (devSystem->isCriticalPeriod() ? "Yes" : "No") << std::endl;
                        pImpl->outputFile << "      Age: " << devSystem->getCurrentAge() << std::endl;
                    }
                }
            }
        }
        
        // Plasticity visualization
        if (showPlasticity) {
            pImpl->outputFile << std::endl << "Plasticity Visualization:" << std::endl;
            
            auto devSystem = brain.getDevelopmentSystem();
            if (devSystem) {
                pImpl->outputFile << "  Current Age: " << devSystem->getCurrentAge() << std::endl;
                pImpl->outputFile << "  Plasticity Modifier: " << devSystem->getPlasticityModifier() << std::endl;
                pImpl->outputFile << "  Critical Period: " << (devSystem->isCriticalPeriod() ? "Active" : "Inactive") << std::endl;
                
                // Track plasticity changes
                double currentPlasticity = devSystem->getPlasticityModifier();
                pImpl->plasticityHistory.push_back(currentPlasticity);
                
                if (pImpl->plasticityHistory.size() > 10) {
                    pImpl->plasticityHistory.erase(pImpl->plasticityHistory.begin());
                }
                
                if (pImpl->plasticityHistory.size() > 1) {
                    pImpl->outputFile << "  Plasticity Trend:" << std::endl;
                    for (size_t i = 0; i < pImpl->plasticityHistory.size(); ++i) {
                        pImpl->outputFile << "    Step " << i << ": " << pImpl->plasticityHistory[i] << std::endl;
                    }
                }
            }
        }
        
        // Critical period visualization
        if (showCriticalPeriod) {
            pImpl->outputFile << std::endl << "Critical Period Visualization:" << std::endl;
            
            auto devSystem = brain.getDevelopmentSystem();
            if (devSystem && devSystem->isCriticalPeriod()) {
                pImpl->outputFile << "  CRITICAL PERIOD ACTIVE" << std::endl;
                pImpl->outputFile << "  Enhanced plasticity: Yes" << std::endl;
                pImpl->outputFile << "  Synaptogenesis rate: High" << std::endl;
                pImpl->outputFile << "  Pruning rate: Low" << std::endl;
            } else {
                pImpl->outputFile << "  Critical Period: Inactive" << std::endl;
            }
        }
        
        pImpl->outputFile << std::endl << "---" << std::endl << std::endl;
    }
}

// Helper function to get stage name
std::string DevelopmentVisualizer::getStageName(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "Critical Period";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        case DevelopmentalStage::End: return "End";
        default: return "Unknown";
    }
}

} // namespace nlm