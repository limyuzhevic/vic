// Updated VisualizationInterface.hpp
// This now acts as a wrapper around the new VisualizationSystem

#pragma once

#include <string>
#include <memory>
#include "VisualizationSystem.hpp"
#include "RealTimeMonitor.hpp"

namespace nlm {

// Enhanced VisualizationInterface that wraps the new VisualizationSystem
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize with configuration
    bool initialize(std::shared_ptr<Config> config, 
                    VisualizationSystem::BackendType backend = 
                        VisualizationSystem::BackendType::SOFTWARE_RENDERER);
    
    // Initialize with visualization system
    bool initialize(std::shared_ptr<VisualizationSystem> visualizer);
    
    // Initialize with monitor
    bool initialize(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Initialize with both
    bool initialize(std::shared_ptr<Brain> brain, 
                    std::shared_ptr<RealTimeMonitor> monitor,
                    VisualizationSystem::BackendType backend = 
                        VisualizationSystem::BackendType::SOFTWARE_RENDERER);
    
    // Update visualization with current brain state
    void update(std::shared_ptr<Brain> brain);
    
    // Update visualization with monitor data
    void update(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Render current frame
    void render();
    
    // Render specific visualization types
    void render(VisualizationSystem::VisualizationType type);
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Control visualization
    void startVisualization(double fps = 60.0);
    void stopVisualization();
    bool isRunning() const;
    
    // Set data sources
    void setBrainData(std::shared_ptr<Brain> brain);
    void setMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Configuration
    void setBackend(VisualizationSystem::BackendType backend);
    VisualizationSystem::BackendType getBackendType() const;
    
    // Camera control
    void setCameraPosition(float x, float y, float z);
    void setCameraTarget(float x, float y, float z);
    void setZoom(float zoom);
    void rotateCamera(float yaw, float pitch);
    
    // Export
    bool exportVisualization(const std::string& filepath, 
                             const std::string& format = "png");
    bool exportAnimation(const std::string& filepath, 
                         double duration, double fps = 30.0);
    
    // Performance
    size_t getTotalNeuronCount() const;
    float getRenderingFPS() const;
    size_t getMemoryUsage() const;
    
    // Get underlying systems
    std::shared_ptr<VisualizationSystem> getVisualizationSystem() const;
    std::shared_ptr<RealTimeMonitor> getMonitor() const;
    
    // Legacy interface (for backward compatibility)
    void visualizeNetwork(const class Brain& brain) {
        if (pImpl->visualizationSystem) {
            pImpl->visualizationSystem->addVisualization(
                VisualizationSystem::VisualizationType::NETWORK);
        }
    }
    
    void visualizeActivity(const class Brain& brain) {
        if (pImpl->visualizationSystem) {
            pImpl->visualizationSystem->addVisualization(
                VisualizationSystem::VisualizationType::ACTIVITY);
        }
    }
    
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes) {
        if (pImpl->visualizationSystem) {
            pImpl->visualizationSystem->addVisualization(
                VisualizationSystem::VisualizationType::NETWORK);
        }
    }
    
    void visualizeWeights(const class Brain& brain) {
        if (pImpl->visualizationSystem) {
            pImpl->visualizationSystem->addVisualization(
                VisualizationSystem::VisualizationType::NETWORK);
        }
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
