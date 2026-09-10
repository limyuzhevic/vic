#include "src/visualization/VisualizationInterface.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== NLM Visualization System ===" << std::endl;
    
    // Create visualization interface
    nlm::VisualizationInterface viz;
    
    // Initialize with default settings
    nlm::VisualizationSettings settings;
    settings.enableNetworkView = true;
    settings.enableActivityView = true;
    settings.enableSpikeView = true;
    settings.enableMemoryView = true;
    settings.enableNeuromodulationView = true;
    settings.enableDevelopmentView = true;
    settings.enableRecording = true;
    settings.recordingInterval = 100;
    
    if (!viz.initialize(settings)) {
        std::cerr << "Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully!" << std::endl;
    
    // Start recording
    viz.startRecording("nlm_visualization.csv");
    
    // Display system status
    auto& vizSettings = viz.getSettings();
    std::cout << "Settings loaded from configuration:" << std::endl;
    std::cout << "  Recording enabled: " << (vizSettings.enableRecording ? "YES" : "NO") << std::endl;
    std::cout << "  Recording format: " << vizSettings.recordingFormat << std::endl;
    std::cout << "  Recording interval: " << vizSettings.recordingInterval << std::endl;
    
    // Check performance metrics
    auto metrics = viz.getMetrics();
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "  CPU usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory usage: " << metrics.memoryUsage << " MB" << std::endl;
    std::cout << "  Simulation time per step: " << metrics.simulationTime << " ms" << std::endl;
    
    std::cout << "\nVisualization system is ready for brain integration!" << std::endl;
    
    return 0;
}