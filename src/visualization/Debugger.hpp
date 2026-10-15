#pragma once

#include "VisualizationInterface.hpp"
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Visualization component types
enum class VisualizationComponent {
    NeuralActivity,
    PerformanceProfiler,
    Simulation,
    Configuration,
    Memory,
    Neuromodulation,
    Network,
    Spikes,
    Weights
};

// Debug state information
struct DebugState {
    SimulationStep currentStep;
    Timestamp currentTime;
    double simulationTime;
    size_t neuronCount;
    size_t spikeCount;
    double excitationInhibitionRatio;
    float averageFiringRate;
    std::chrono::system_clock::time_point lastUpdate;
};

// Main debugger that coordinates all visualization components
class Debugger {
public:
    Debugger();
    ~Debugger();
    
    // Initialize debugger with components
    bool initialize(const std::vector<VisualizationComponent>& components = {});
    
    // Add a visualization component
    bool addComponent(VisualizationComponent type, std::shared_ptr<VisualizationInterface> component);
    
    // Remove component
    void removeComponent(VisualizationComponent type);
    
    // Get component
    std::shared_ptr<VisualizationInterface> getComponent(VisualizationComponent type);
    
    // Update all components
    void update(double deltaTime);
    
    // Render all components
    void render();
    
    // Close all components
    void close();
    
    // Set brain reference for visualization
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Set simulation reference
    void setSimulationClock(std::shared_ptr<SimulationClock> clock);
    
    // Set configuration reference
    void setConfig(std::shared_ptr<Config> config);
    
    // Set logger reference
    void setLogger(std::shared_ptr<Logger> logger);
    
    // Set memory system references
    void setMemory(std::shared_ptr<Memory> memory);
    
    // Set neuromodulation system references
    void setNeuromodulation(std::shared_ptr<NeuromodulationSystem> neuromodulation);
    
    // Set spike system reference
    void setSpikeSystem(std::shared_ptr<SpikeSystem> spikeSystem);
    
    // Export all visualizations
    bool exportAll(const std::string& directory, VisualizationFormat format = VisualizationFormat::TEXT);
    
    // Import all visualizations
    bool importAll(const std::string& directory);
    
    // Get current debug state
    DebugState getDebugState() const;
    
    // Get all component data as string
    std::string getAllComponentData() const;
    
    // Get status report
    std::string getStatusReport() const;
    
    // Enable/disable specific component
    void setComponentEnabled(VisualizationComponent type, bool enabled);
    
    // Check if component is enabled
    bool isComponentEnabled(VisualizationComponent type) const;
    
    // Update rate control
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Set output format
    void setOutputFormat(VisualizationFormat format);
    
    // Get output format
    VisualizationFormat getOutputFormat() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
