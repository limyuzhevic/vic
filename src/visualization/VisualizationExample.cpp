// src/visualization/VisualizationExample.cpp
// Example demonstrating the new visualization features

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "visualization/NeuralVisualizer.hpp"
#include <iostream>
#include <memory>

using namespace nlm;
using namespace nlm::visualization;

void demonstrateNeuralNetworkVisualization() {
    std::cout << "=== Neural Network Visualization Example ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config->set("connection_probability", 0.1f, ConfigSource::Runtime);
    
    // Create a brain
    auto brain = std::make_shared<Brain>(config);
    std::cout << "Initializing brain..." << std::endl;
    
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully" << std::endl;
        
        // Demonstrate different visualization formats
        std::cout << "\n--- Generating HTML visualization ---" << std::endl;
        if (visualizeNeuralNetwork(brain, "brain_viz", "html")) {
            std::cout << "✓ HTML visualization generated: brain_viz.html" << std::endl;
        } else {
            std::cout << "✗ Failed to generate HTML visualization" << std::endl;
        }
        
        std::cout << "\n--- Generating JSON visualization ---" << std::endl;
        if (visualizeNeuralNetwork(brain, "brain_viz_json", "json")) {
            std::cout << "✓ JSON visualization generated: brain_viz_json.json" << std::endl;
        } else {
            std::cout << "✗ Failed to generate JSON visualization" << std::endl;
        }
        
        std::cout << "\n--- Generating CSV visualization ---" << std::endl;
        if (visualizeNeuralNetwork(brain, "brain_viz_csv", "csv")) {
            std::cout << "✓ CSV visualization generated: brain_viz_csv.csv" << std::endl;
        } else {
            std::cout << "✗ Failed to generate CSV visualization" << std::endl;
        }
        
        // Demonstrate spike activity visualization
        std::cout << "\n--- Generating spike activity visualization ---" << std::endl;
        if (visualizeSpikeActivity(brain, "spike_activity.html")) {
            std::cout << "✓ Spike activity visualization generated: spike_activity.html" << std::endl;
        } else {
            std::cout << "✗ Failed to generate spike activity visualization" << std::endl;
        }
        
        // Demonstrate network topology visualization
        std::cout << "\n--- Generating network topology visualization ---" << std::endl;
        if (visualizeNetworkTopology(brain, "network_topology.html")) {
            std::cout << "✓ Network topology visualization generated: network_topology.html" << std::endl;
        } else {
            std::cout << "✗ Failed to generate network topology visualization" << std::endl;
        }
        
        // Demonstrate interactive dashboard
        std::cout << "\n--- Generating interactive dashboard ---" << std::endl;
        if (createInteractiveDashboard(brain, "neural_dashboard.html")) {
            std::cout << "✓ Interactive dashboard generated: neural_dashboard.html" << std::endl;
        } else {
            std::cout << "✗ Failed to generate interactive dashboard" << std::endl;
        }
        
        // Demonstrate advanced visualization with neural visualizer
        std::cout << "\n--- Using NeuralNetworkVisualizer for advanced features ---" << std::endl;
        NeuralNetworkVisualizer visualizer;
        
        // Configure visualizer
        visualizer.setFormat(VisualizationFormat::HTML);
        visualizer.setImageWidth(1600);
        visualizer.setImageHeight(900);
        visualizer.setColorScheme("heatmap");
        visualizer.setNeuronLayoutAlgorithm("force_directed");
        
        // Collect data and generate advanced visualization
        auto data = visualizer.collectVisualizationData(brain);
        
        if (visualizer.generateVisualization(data, "advanced_brain_viz.html")) {
            std::cout << "✓ Advanced visualization generated: advanced_brain_viz.html" << std::endl;
        } else {
            std::cout << "✗ Failed to generate advanced visualization" << std::endl;
        }
        
        // Generate text report
        if (visualizer.generateTextReport(data, "brain_analysis.txt")) {
            std::cout << "✓ Text report generated: brain_analysis.txt" << std::endl;
        }
        
        // Generate JSON file
        if (visualizer.generateJSONFile(data, "brain_data.json")) {
            std::cout << "✓ JSON data file generated: brain_data.json" << std::endl;
        }
        
        // Demonstrate specialized visualizers
        std::cout << "\n--- Specialized visualizers ---" << std::endl;
        
        SpikeActivityVisualizer spike_visualizer;
        if (spike_visualizer.generateSpikeRasterPlot(brain, "spike_raster.html")) {
            std::cout << "✓ Spike raster plot generated: spike_raster.html" << std::endl;
        }
        
        PlasticityVisualizer plasticity_visualizer;
        if (plasticity_visualizer.generateWeightChangeVisualization(brain, "weight_changes.html")) {
            std::cout << "✓ Weight change visualization generated: weight_changes.html" << std::endl;
        }
        
        NetworkTopologyVisualizer topology_visualizer;
        if (topology_visualizer.generateConnectivityMatrix(brain, "connectivity_matrix.html")) {
            std::cout << "✓ Connectivity matrix generated: connectivity_matrix.html" << std::endl;
        }
        
    } else {
        std::cout << "✗ Failed to initialize brain" << std::endl;
    }
}

void demonstrateVisualizationFeatures() {
    std::cout << "\n=== Visualization Features Demonstration ===" << std::endl;
    
    std::cout << "\nAvailable visualization formats:" << std::endl;
    std::cout << "  - HTML: Interactive web-based visualizations" << std::endl;
    std::cout << "  - CSV: Spreadsheet-compatible data export" << std::endl;
    std::cout << "  - JSON: Structured data for programmatic access" << std::endl;
    std::cout << "  - Text: Human-readable reports" << std::endl;
    std::cout << "  - PNG/SVG/PDF: Static images for publications" << std::endl;
    
    std::cout << "\nVisualization capabilities:" << std::endl;
    std::cout << "  - Neural network topology visualization" << std::endl;
    std::cout << "  - Spike activity visualization" << std::endl;
    std::cout << "  - Weight change tracking" << std::endl;
    std::cout << "  - STDP analysis visualization" << std::endl;
    std::cout << "  - Network connectivity matrices" << std::endl;
    std::cout << "  - Degree distribution analysis" << std::endl;
    std::cout << "  - Community detection visualization" << std::endl;
    std::cout << "  - Small-world metrics visualization" << std::endl;
    
    std::cout << "\nLayout algorithms available:" << std::endl;
    std::cout << "  - Force-directed (default)" << std::endl;
    std::cout << "  - Circular (ring layout)" << std::endl;
    std::cout << "  - Radial (hub-and-spoke)" << std::endl;
    std::cout << "  - Grid (matrix layout)" << std::endl;
    std::cout << "  - Hierarchy (layered structure)" << std::endl;
    
    std::cout << "\nColor schemes available:" << std::endl;
    std::cout << "  - Default (categorical by type)" << std::endl;
    std::cout << "  - Heatmap (based on membrane potential)" << std::endl;
    std::cout << "  - Neuron type (specific colors for each type)" << std::endl;
    std::cout << "  - Weight based (blue to red gradient)" << std::endl;
    std::cout << "  - Activity based (intensity based on firing)" << std::endl;
    
    std::cout << "\nExport options:" << std::endl;
    std::cout << "  - Python scripts (for machine learning)" << std::endl;
    std::cout << "  - MATLAB format" << std::endl;
    std::cout << "  - R format" << std::endl;
    std::cout << "  - Interactive HTML dashboards" << std::endl;
    std::cout << "  - Batch processing for multiple simulations" << std::endl;
}

int main() {
    std::cout << "NLM Visualization Examples" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    demonstrateVisualizationFeatures();
    demonstrateNeuralNetworkVisualization();
    
    std::cout << "\n=== Visualization Examples Completed ===" << std::endl;
    std::cout << "All visualization files should be available in the current directory." << std::endl;
    std::cout << "Key files generated:" << std::endl;
    std::cout << "  - brain_viz.html: Main neural network visualization" << std::endl;
    std::cout << "  - brain_viz_json.json: Structured data export" << std::endl;
    std::cout << "  - brain_viz_csv.csv: Spreadsheet-compatible data" << std::endl;
    std::cout << "  - spike_activity.html: Spike activity visualization" << std::endl;
    std::cout << "  - network_topology.html: Network topology analysis" << std::endl;
    std::cout << "  - neural_dashboard.html: Interactive dashboard" << std::endl;
    std::cout << "  - advanced_brain_viz.html: Advanced visualization with heatmap colors" << std::endl;
    std::cout << "  - brain_analysis.txt: Text analysis report" << std::endl;
    std::cout << "  - brain_data.json: Comprehensive JSON data" << std::endl;
    
    return 0;
}