/**
 * Example: Basic Visualization with Phase 6 Demo
 * 
 * This example demonstrates how to use the visualization tools
 * with the standard Phase 6 demo.
 */

#include "experiments/Phase6Demo.cpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Basic Visualization Example ===" << std::endl;
    std::cout << "Demonstrating visualization with the standard Phase 6 demo." << std::endl << std::endl;
    
    // Create visualization interface in text-advanced mode
    VisualizationInterface viz(VisualizationMode::TextAdvanced);
    viz.initialize("NLM Phase 6 - Basic Visualization", 120, 80);
    viz.setUpdateRate(10.0); // 10 Hz update rate
    
    // Run the visualization demo
    Phase6Demo demo;
    demo.run(viz);
    
    // Export visualization data
    viz.exportData("basic_visualization.json");
    viz.exportMetrics("basic_visualization_metrics.csv");
    
    std::cout << std::endl;
    std::cout << "=== Visualization Complete ===" << std::endl;
    std::cout << "Data exported to basic_visualization.json" << std::endl;
    
    viz.close();
    return 0;
}