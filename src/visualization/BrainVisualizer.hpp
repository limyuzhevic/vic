// NLM Visualization System
// Provides simple terminal-based visualization for brain state monitoring
// Phase 6: Enhanced monitoring and analysis capabilities

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <thread>

namespace nlm {

class BrainVisualizer {
public:
    struct VisualizationConfig {
        bool showNeurons = true;
        bool showConnections = true;
        bool showMemory = true;
        bool showActivity = true;
        int updateInterval = 1000;  // milliseconds
        bool enableColor = true;
        bool showStats = true;
        std::string outputFile = "";
    };

    BrainVisualizer() = default;
    ~BrainVisualizer() = default;
    
    // Main visualization loop
    void startVisualization(std::shared_ptr<Brain> brain, const VisualizationConfig& config);
    
    // Single frame visualization
    void visualizeFrame(std::shared_ptr<Brain> brain, const VisualizationConfig& config);
    
    // Terminal-based visualization
    void renderTerminalView(std::shared_ptr<Brain> brain, const VisualizationConfig& config);
    
    // Export visualization to file
    void exportVisualization(std::shared_ptr<Brain> brain, const std::string& filename);
    
    // Performance monitoring
    void startPerformanceMonitoring();
    double getCurrentFPS() const;
    double getAverageFrameTime() const;
    
    // Control functions
    void stopVisualization();
    bool isRunning() const;
    
    // Configuration
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;

private:
    struct Impl;
    Impl* pImpl;

public:
    BrainVisualizer(BrainVisualizer&&) noexcept = default;
    BrainVisualizer& operator=(BrainVisualizer&&) noexcept = default;
};

// Simple text-based neuron visualization
class TextNeuronVisualizer {
public:
    void renderNeuronState(const Neuron* neuron, bool highlight = false);
    void renderNetworkState(std::shared_ptr<Brain> brain);
    void renderMemoryState(std::shared_ptr<Brain> brain);
    void renderActivityMap(std::shared_ptr<Brain> brain);
    void renderStatistics(std::shared_ptr<Brain> brain);
    
private:
    void clearScreen();
    void setColor(int colorCode);
    void resetColor();
};

// Visualization utilities
namespace vis {
    // Convert brain metrics to visual representation
    std::string formatActivityLevel(float firingRate);
    std::string formatWeight(float weight);
    std::string formatMemorySize(size_t size);
    std::string formatTime(double seconds);
    
    // Generate color codes for different states
    int getActivityColor(float firingRate);
    int getWeightColor(float weight);
    int getMemoryColor(float utilization);
    
    // Create formatted output strings
    std::string createHeader(const std::string& title);
    std::string createSeparator(char separator = '=');
    std::string createFooter();
}

} // namespace nlm
