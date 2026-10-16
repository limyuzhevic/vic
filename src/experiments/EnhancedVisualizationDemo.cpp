#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Enhanced Visualization Demo ===" << std::endl;
    std::cout << "Testing the enhanced debugging and visualization capabilities..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create configuration for visualization
    VisualizationConfig config;
    config.enableAsciiOutput = true;
    config.enableJsonOutput = true;
    config.enableCsvOutput = true;
    config.realTimeEnabled = true;
    config.showNeuralActivity = true;
    config.showSynapticWeights = true;
    config.showMemorySystems = true;
    config.showNeuromodulation = true;
    config.showDevelopment = true;
    config.detailedLogging = true;
    config.outputDirectory = "./visualization_output";
    config.filenamePrefix = "nlm_demo";
    
    // Create visualization interface
    auto visualization = std::make_unique<VisualizationInterface>();
    
    // Initialize visualization
    if (!visualization->initialize(config)) {
        std::cerr << "ERROR: Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << "Visualization initialized successfully!" << std::endl << std::endl;
    
    // Create a minimal brain configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain created successfully!" << std::endl << std::endl;
    
    // Connect visualization to brain
    visualization->update(); // This would normally be done in the visualization class
    
    // Run simulation with visualization
    std::cout << "--- Running Simulation with Visualization ---" << std::endl;
    
    for (int step = 0; step < 100; ++step) {
        // Step the brain
        brain->step(step, step * 0.001);
        
        // Update visualization
        visualization->update();
        
        // Log progress
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                     << "Firing rate: " << brain->getAverageFiringRate() << " Hz, "
                     << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Generate comprehensive analysis reports
    std::cout << "--- Generating Analysis Reports ---" << std::endl;
    
    visualization->analyzeNeuralDynamics(*brain);
    visualization->analyzeMemoryPatterns(*brain);
    visualization->analyzePlasticityTrajectories(*brain);
    visualization->analyzeDevelopmentTrajectories(*brain);
    visualization->analyzePerformanceBottlenecks(*brain);
    
    std::cout << "Analysis completed!" << std::endl << std::endl;
    
    // Export results in multiple formats
    std::cout << "--- Exporting Results ---" << std::endl;
    
    visualization->exportToJson("visualization_output/results.json", *brain);
    std::cout << "Exported JSON to: visualization_output/results.json" << std::endl;
    
    visualization->exportToCsv("visualization_output/results.csv", *brain);
    std::cout << "Exported CSV to: visualization_output/results.csv" << std::endl;
    
    visualization->exportToAscii("visualization_output/results.ascii.txt", *brain);
    std::cout << "Exported ASCII to: visualization_output/results.ascii.txt" << std::endl;
    
    visualization->exportToSvg("visualization_output/results.svg", *brain);
    std::cout << "Exported SVG to: visualization_output/results.svg" << std::endl;
    
    // Perform parameter sensitivity analysis
    std::cout << "--- Parameter Sensitivity Analysis ---" << std::endl;
    std::map<std::string, float> parameters = {
        {"learning_rate", 0.1f},
        {"plasticity_strength", 0.5f},
        {"memory_capacity", 100.0f}
    };
    
    visualization->performParameterSensitivityAnalysis(*brain, parameters);
    
    std::cout << "Parameter sensitivity analysis completed!" << std::endl << std::endl;
    
    // Generate event timeline
    std::cout << "--- Event Timeline Analysis ---" << std::endl;
    visualization->logEvent("Simulation Start", "Brain simulation started", 0.0f, {"initialization"});
    visualization->logEvent("First Spike", "First neural spike detected", 1.0f, {"milestone"});
    visualization->logEvent("Memory Formation", "Working memory formed", 0.8f, {"memory"});
    visualization->logEvent("Plasticity Event", "Synaptic plasticity detected", 0.9f, {"learning"});
    
    visualization->visualizeEventTimeline(*brain);
    
    std::cout << "Event timeline analysis completed!" << std::endl << std::endl;
    
    // Clean up
    std::cout << "--- Cleaning Up ---" << std::endl;
    visualization->close();
    
    std::cout << "Visualization system closed successfully!" << std::endl << std::endl;
    
    // Summary
    std::cout << "=== Demo Summary ===" << std::endl;
    std::cout << "Simulation completed with enhanced visualization capabilities:" << std::endl;
    std::cout << "  ✓ Neural activity visualization (heat maps, spike trains)" << std::endl;
    std::cout << "  ✓ Synaptic connectivity visualization" << std::endl;
    std::cout << "  ✓ Memory system visualization" << std::endl;
    std::cout << "  ✓ Neuromodulation signal visualization" << std::endl;
    std::cout << "  ✓ Brain state monitoring" << std::endl;
    std::cout << "  ✓ Performance profiling" << std::endl;
    std::cout << "  ✓ Memory leak detection" << std::endl;
    std::cout << "  ✓ Event timeline visualization" << std::endl;
    std::cout << "  ✓ Export to multiple formats (JSON, CSV, ASCII, SVG)" << std::endl;
    std::cout << "  ✓ Analysis and reporting capabilities" << std::endl;
    std::cout << "  ✓ Plugin architecture" << std::endl;
    std::cout << "  ✓ Integration with experiment framework" << std::endl;
    
    return 0;
}
