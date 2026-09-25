/**
 * Example: Advanced Visualization with Multiple Brain Systems
 * 
 * This example demonstrates advanced visualization features
 * including all brain system integration monitoring.
 */

#include "experiments/Phase6Demo.cpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace nlm;

void monitorSystemStatus(const VisualizationInterface& viz, int step) {
    auto metrics = viz.getCurrentMetrics();
    std::cout << std::endl;
    std::cout << "=== System Status Update (Step " << step << ") ===" << std::endl;
    
    // Check integration status
    bool allSystemsWorking = metrics.memoryIntegrated && 
                           metrics.episodicMemoryIntegrated && 
                           metrics.neuromodulationIntegrated && 
                           metrics.predictionIntegrated && 
                           metrics.developmentIntegrated;
    
    std::cout << "Integration Status: " << (allSystemsWorking ? "✅ ALL SYSTEMS WORKING" : "❌ SYSTEMS NEED ATTENTION") << std::endl;
    std::cout << "Memory: " << (metrics.memoryIntegrated ? "✅" : "❌") << std::endl;
    std::cout << "Episodic Memory: " << (metrics.episodicMemoryIntegrated ? "✅" : "❌") << std::endl;
    std::cout << "Neuromodulation: " << (metrics.neuromodulationIntegrated ? "✅" : "❌") << std::endl;
    std::cout << "Prediction: " << (metrics.predictionIntegrated ? "✅" : "❌") << std::endl;
    std::cout << "Development: " << (metrics.developmentIntegrated ? "✅" : "❌") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Reward: " << metrics.totalReward << std::endl;
    std::cout << "  Novelty: " << metrics.noveltyLevel << std::endl;
    std::cout << "  Curiosity: " << metrics.curiosityLevel << std::endl;
    std::cout << "  Dopamine: " << metrics.dopamineLevel << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Advanced Visualization Example ===" << std::endl;
    std::cout << "Demonstrating advanced monitoring of brain system integration." << std::endl << std::endl;
    
    // Create visualization in compact mode for real-time monitoring
    VisualizationInterface viz(VisualizationMode::Compact);
    viz.initialize("NLM Phase 6 - System Monitor", 100, 50);
    viz.setUpdateRate(5.0); // 5 Hz for compact display
    
    // Create Phase 6 demo
    Phase6Demo demo;
    
    // Enable automatic export
    auto config = viz.getConfig();
    config.autoSave = true;
    config.exportPath = "./advanced_visualization_output";
    viz.setConfig(config);
    
    std::cout << "Starting visualization..." << std::endl;
    std::cout << "Features enabled:" << std::endl;
    std::cout << "  - Compact text display" << std::endl;
    std::cout << "  - Automatic data export" << std::endl;
    std::cout << "  - System integration monitoring" << std::endl;
    std::cout << std::endl;
    
    // Run the demo with periodic status monitoring
    demo.run(viz);
    
    // Manual status updates every 100 steps
    int step = 0;
    while (step < 500) {
        monitorSystemStatus(viz, step);
        step += 100;
        
        if (step < 500) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    // Export final data
    viz.exportData("advanced_visualization.json");
    viz.exportMetrics("advanced_visualization_metrics.csv");
    
    std::cout << std::endl;
    std::cout << "=== Advanced Visualization Complete ===" << std::endl;
    std::cout << "Data exported to:" << std::endl;
    std::cout << "  - advanced_visualization.json (full snapshot)" << std::endl;
    std::cout << "  - advanced_visualization_metrics.csv (analysis data)" << std::endl;
    std::cout << "  - advanced_visualization_output/ (continuous exports)" << std::endl;
    
    viz.close();
    return 0;
}