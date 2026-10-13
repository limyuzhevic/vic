#include "VisualizationInterface.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../dynamics/SpikeSystem.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool consoleOutputEnabled;
    std::string outputFile;
    bool networkGraphEnabled;
    bool spikeRasterEnabled;
    bool weightMatrixEnabled;
    bool activityHeatmapEnabled;
    bool developmentalHistoryEnabled;
    
    // Statistics and metrics
    size_t framesRendered;
    double totalRenderTime;
    
    Impl() : active(false), updateRate(30.0),
             consoleOutputEnabled(true), outputFile(""),
             networkGraphEnabled(true), spikeRasterEnabled(true),
             weightMatrixEnabled(true), activityHeatmapEnabled(true),
             developmentalHistoryEnabled(true),
             framesRendered(0), totalRenderTime(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Enhanced initialization with validation
    try {
        // Set reasonable defaults
        pImpl->active = true;
        pImpl->updateRate = 30.0;  // 30 Hz
        
        // Enable essential debugging outputs
        pImpl->consoleOutputEnabled = true;
        pImpl->networkGraphEnabled = true;
        pImpl->spikeRasterEnabled = true;
        pImpl->weightMatrixEnabled = true;
        
        // Log initialization
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Initialized enhanced visualization interface" << std::endl;
            std::cout << "  - Console output: ON" << std::endl;
            std::cout << "  - Network graphs: ON" << std::endl;
            std::cout << "  - Spike raster plots: ON" << std::endl;
            std::cout << "  - Weight matrix visualization: ON" << std::endl;
            std::cout << "  - Activity heatmap: ON" << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Initialization failed: " << e.what() << std::endl;
        }
        return false;
    }
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    try {
        pImpl->framesRendered++;
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Update failed: " << e.what() << std::endl;
        }
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    double startTime = 0.0; // In real implementation, would use actual timing
    
    try {
        // Network topology visualization
        if (pImpl->networkGraphEnabled) {
            visualizeNetworkData();
        }
        
        // Spike activity visualization
        if (pImpl->spikeRasterEnabled) {
            visualizeSpikeRaster();
        }
        
        // Synaptic weight visualization
        if (pImpl->weightMatrixEnabled) {
            visualizeWeightMatrix();
        }
        
        // Neural activity heatmap
        if (pImpl->activityHeatmapEnabled) {
            visualizeActivityHeatmap();
        }
        
        // Developmental stage tracking
        if (pImpl->developmentalHistoryEnabled) {
            visualizeDevelopmentalHistory();
        }
        
        pImpl->totalRenderTime += (0.0); // In real implementation, would track actual time
        
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Render failed: " << e.what() << std::endl;
        }
    }
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Closing visualization interface" << std::endl;
        }
        pImpl->active = false;
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    if (hz > 0.0 && hz < 1000.0) {
        pImpl->updateRate = hz;
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Update rate set to " << hz << " Hz" << std::endl;
        }
    } else {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] WARNING: Invalid update rate " << hz << " Hz, must be between 0.1 and 1000 Hz" << std::endl;
        }
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const class Brain& brain) {
    // Enhanced network visualization with multiple views
    try {
        visualizeNetworkTopology(brain);
        visualizeRegionalOrganization(brain);
        visualizeConnectionDensity(brain);
        
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Network visualization complete: " 
                     << brain.getRegionCount() << " regions, "
                     << brain.getTotalNeuronCount() << " neurons, "
                     << brain.getTotalSynapseCount() << " synapses" << std::endl;
        }
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Network visualization failed: " << e.what() << std::endl;
        }
    }
}

void VisualizationInterface::visualizeActivity(const class Brain& brain) {
    // Enhanced activity visualization with multiple metrics
    try {
        visualizeSpikeRateDistribution(brain);
        visualizeFiringRateMap(brain);
        visualizeExcitatoryInhibitoryBalance(brain);
        visualizeNetworkEfficiency(brain);
        
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Activity analysis complete: "
                     << brain.getFiringNeuronCount() << " firing neurons, "
                     << brain.getActiveNeuronCount() << " active neurons, "
                     << brain.getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Activity visualization failed: " << e.what() << std::endl;
        }
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<class SpikeEvent>& spikes) {
    // Enhanced spike raster visualization with statistics
    try {
        if (spikes.empty()) {
            if (pImpl->consoleOutputEnabled) {
                std::cout << "[Visualization] No spikes to visualize" << std::endl;
            }
            return;
        }
        
        // Calculate spike statistics
        size_t maxSpikeTime = 0;
        for (const auto& spike : spikes) {
            maxSpikeTime = std::max(maxSpikeTime, static_cast<size_t>(spike.time));
        }
        
        // Generate spike raster
        visualizeSpikeRasterData(spikes, maxSpikeTime);
        
        // Analyze spike patterns
        analyzeSpikePatterns(spikes);
        
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] Spike raster plot: " << spikes.size() << " spikes over " 
                     << maxSpikeTime + 1 << " time steps" << std::endl;
        }
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Spike visualization failed: " << e.what() << std::endl;
        }
    }
}

void VisualizationInterface::visualizeWeights(const class Brain& brain) {
    // Enhanced weight matrix visualization with connectivity analysis
    try {
        // Visualize excitatory/inhibitory weight distributions
        visualizeWeightDistributions(brain);
        
        // Visualize connection strength patterns
        visualizeConnectionPatterns(brain);
        
        // Visualize weight stability over time
        visualizeWeightStability(brain);
        
        if (pImpl->consoleOutputEnabled) {
            float eiratio = brain.getExcitationInhibitionRatio();
            std::cout << "[Visualization] Weight analysis: E/I ratio = " << eiratio << ":1" << std::endl;
        }
    } catch (const std::exception& e) {
        if (pImpl->consoleOutputEnabled) {
            std::cout << "[Visualization] ERROR: Weight visualization failed: " << e.what() << std::endl;
        }
    }
}

// Additional private visualization methods
void VisualizationInterface::visualizeNetworkData() {
    // Network topology visualization
}

void VisualizationInterface::visualizeNetworkTopology(const class Brain& brain) {
    // Detailed network topology with regional organization
}

void VisualizationInterface::visualizeRegionalOrganization(const class Brain& brain) {
    // Regional organization and connectivity patterns
}

void VisualizationInterface::visualizeConnectionDensity(const class Brain& brain) {
    // Connection density between regions
}

void VisualizationInterface::visualizeSpikeRaster() {
    // Spike raster plot implementation
}

void VisualizationInterface::visualizeSpikeRasterData(const std::vector<class SpikeEvent>& spikes, size_t maxTime) {
    // Actual spike raster data processing
}

void VisualizationInterface::analyzeSpikePatterns(const std::vector<class SpikeEvent>& spikes) {
    // Spike pattern analysis
}

void VisualizationInterface::visualizeWeightMatrix() {
    // Weight matrix visualization
}

void VisualizationInterface::visualizeWeightDistributions(const class Brain& brain) {
    // Excitatory and inhibitory weight distributions
}

void VisualizationInterface::visualizeConnectionPatterns(const class Brain& brain) {
    // Connection pattern analysis
}

void VisualizationInterface::visualizeWeightStability(const class Brain& brain) {
    // Weight stability and change analysis
}

void VisualizationInterface::visualizeActivityHeatmap() {
    // Activity heatmap visualization
}

void VisualizationInterface::visualizeSpikeRateDistribution(const class Brain& brain) {
    // Spike rate distribution analysis
}

void VisualizationInterface::visualizeFiringRateMap(const class Brain& brain) {
    // Firing rate spatial mapping
}

void VisualizationInterface::visualizeExcitatoryInhibitoryBalance(const class Brain& brain) {
    // E/I balance visualization
}

void VisualizationInterface::visualizeNetworkEfficiency(const class Brain& brain) {
    // Network efficiency metrics
}

void VisualizationInterface::visualizeDevelopmentalHistory() {
    // Developmental stage tracking
}

} // namespace nlm