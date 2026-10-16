// Visualization updater for experiment integration
namespace VisualizationIntegration {

class VisualizationUpdater {
public:
    VisualizationUpdater(std::shared_ptr<VisualizationInterface> viz, std::shared_ptr<Brain> brain, 
                       std::shared_ptr<SpikeSystem> spikeSystem)
        : visualization(viz), brain(brain), spikeSystem(spikeSystem) {}
    
    void updateVisualization(double simulationTime, Timestamp time) {
        if (!visualization || !visualization->isActive()) {
            return;
        }
        
        // Update brain visualization
        VisualizationHelpers::updateBrainVisualization(visualization->pImpl->currentState, *brain);
        
        // Update spike visualization
        VisualizationHelpers::updateSpikeVisualization(visualization->pImpl->currentState, *spikeSystem);
        
        // Update memory visualization
        VisualizationHelpers::updateMemoryVisualization(visualization->pImpl->currentState, *brain);
        
        // Update neuromodulation visualization
        VisualizationHelpers::updateNeuromodulationVisualization(visualization->pImpl->currentState, *brain);
        
        // Update development visualization
        VisualizationHelpers::updateDevelopmentVisualization(visualization->pImpl->currentState, *brain);
        
        // Update prediction visualization
        VisualizationHelpers::updatePredictionVisualization(visualization->pImpl->currentState, *brain);
        
        // Update cognition visualization
        VisualizationHelpers::updateCognitionVisualization(visualization->pImpl->currentState, *brain);
        
        // Calculate average synaptic weight
        visualization->pImpl->currentState.averageSynapticWeight = 
            VisualizationHelpers::calculateAverageSynapticWeight(*brain);
        
        // Update step and time
        visualization->pImpl->currentState.currentStep = spikeSystem->getSpikeCount();
        visualization->pImpl->currentState.currentTime = time;
        
        // Update visualization interface
        visualization->update();
    }
    
    void visualizeNetwork() {
        if (visualization) {
            visualization->visualizeNetwork(*brain);
        }
    }
    
    void visualizeActivity() {
        if (visualization) {
            visualization->visualizeActivity(*brain);
        }
    }
    
    void visualizeSpikes() {
        if (visualization && spikeSystem) {
            const auto& spikeHistory = spikeSystem->getSpikeHistory();
            visualization->visualizeSpikes(spikeHistory);
        }
    }
    
    void visualizeWeights() {
        if (visualization) {
            visualization->visualizeWeights(*brain);
        }
    }
    
    std::string getVisualizationStatus() {
        if (visualization) {
            return visualization->getStatus();
        }
        return "Visualization not available";
    }
    
    std::string getWebSocketStatus() {
        if (visualization) {
            return visualization->getWebSocketStatus();
        }
        return "Visualization not available";
    }

private:
    std::shared_ptr<VisualizationInterface> visualization;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SpikeSystem> spikeSystem;
};

} // namespace VisualizationIntegration
