#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

class ConsoleBrainVisualizer;

class BrainVisualizer {
public:
    // Main entry point for visualization
    static bool initializeVisualization();
    static void updateVisualization();
    static void renderVisualization();
    static void closeVisualization();
    static bool isVisualizationActive();
    
    // Create specific visualizers
    static std::shared_ptr<ConsoleBrainVisualizer> createConsoleVisualizer();
    
    // Configuration
    static void setVisualizationUpdateRate(double hz);
    static double getVisualizationUpdateRate();
    
    // Integration with simulation
    static void updateFromBrain(const Brain& brain, double timestep, int step);
    static void visualizeNetwork(const Brain& brain);
    static void visualizeActivity(const Brain& brain);
    static void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    static void visualizeWeights(const Brain& brain);
    
private:
    struct Impl;
    static std::unique_ptr<Impl> pImpl;
    
    // Internal helper functions
    static void printStatusLine(const std::string& status);
    static void printNetworkInfo(const Brain& brain);
    static void printActivityBar(double activityLevel, double totalSpikes);
    static void printFiringStats(const Brain& brain);
    static std::string getNetworkState(double totalSpikes);
    static void printNetworkDiagram(const Brain& brain);
    static void printActivityPlot(const Brain& brain);
    static void printSpikeRaster(const std::vector<SpikeEvent>& spikes);
    static void printWeightMatrix(const Brain& brain);
    static double getActivityAtTime(int timeIndex);
};

} // namespace nlm