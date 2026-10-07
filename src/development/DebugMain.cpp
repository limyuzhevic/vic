// Debug and Testing Application
// Demonstrates advanced NLM debugging and visualization capabilities

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "development/DebugVisualization.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <fstream>

using namespace nlm;

void printDebugBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM DEBUG — 熙然                                               ║
    ║     Neural Learning Machine Debug Tools                         ║
    ║                                                               ║
    ║     Advanced debugging and visualization for Phase 2           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runDebugSession(std::shared_ptr<Config> config) {
    printDebugBanner();
    
    std::cout << "\n=== NLM Debug Session Started ===\n" << std::endl;
    
    // Initialize brain with configuration
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create debug output interface
    DebugOutput debugOutput;
    debugOutput.initialize("configs/debug.cfg");
    
    // Create visualization interface  
    VisualizationInterface vizInterface;
    vizInterface.initialize("configs/viz.cfg");
    
    // Create brain debugger
    BrainDebugger debugger;
    debugger.attach(brain);
    debugger.enablePerformanceLogging("debug/performance.log");
    
    std::cout << "Debug tools initialized successfully." << std::endl;
    std::cout << "Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "and " << brain->getTotalSynapseCount() << " synapses." << std::endl;
    
    // Collect initial network metrics
    auto initialMetrics = vizInterface.computeMetrics(brain);
    std::cout << "\nInitial Network Metrics:" << std::endl;
    std::cout << "  Neuron Count: " << initialMetrics.neuronCount << std::endl;
    std::cout << "  Synapse Count: " << initialMetrics.synapseCount << std::endl;
    std::cout << "  Connection Density: " << std::fixed << std::setprecision(4) << initialMetrics.connectionDensity << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << initialMetrics.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Average Weight: " << std::fixed << std::setprecision(4) << initialMetrics.averageWeight << std::endl;
    
    // Run debugging steps
    const int debugSteps = config->getOr<int>("debug_steps", 100);
    
    std::cout << "\nStarting debug simulation (" << debugSteps << " steps)..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < debugSteps; ++step) {
        // Update debug interface
        debugOutput.update(brain);
        
        // Apply minimal sensory input to stimulate network
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        // Perform simulation step
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every 10 steps
        if (step % 10 == 0) {
            std::cout << "Step " << step << "/" << debugSteps 
                     << " | Active Neurons: " << brain->getActiveNeuronCount()
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
        
        // Capture state periodically
        if (step % 25 == 0) {
            std::string filename = "debug_states/step_" + std::to_string(step) + ".bin";
            debugOutput.captureState(filename);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\nDebug simulation completed in " << duration.count() << " seconds." << std::endl;
    
    // Collect final metrics
    auto finalMetrics = vizInterface.computeMetrics(brain);
    std::cout << "\nFinal Network Metrics:" << std::endl;
    std::cout << "  Neuron Count: " << finalMetrics.neuronCount << std::endl;
    std::cout << "  Synapse Count: " << finalMetrics.synapseCount << std::endl;
    std::cout << "  Connection Density: " << std::fixed << std::setprecision(4) << finalMetrics.connectionDensity << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << finalMetrics.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Average Weight: " << std::fixed << std::setprecision(4) << finalMetrics.averageWeight << std::endl;
    
    // Generate network visualizations
    std::cout << "\nGenerating network visualizations..." << std::endl;
    debugOutput.generateNeuronGraph("debug_visualizations/neuron_graph.txt");
    debugOutput.generateConnectivityMatrix("debug_visualizations/connectivity_matrix.txt");
    
    // Analyze network properties
    std::cout << "\nPerforming network analysis..." << std::endl;
    vizInterface.analyzeNetworkProperties(brain);
    vizInterface.detectCommunities(brain);
    vizInterface.identifyHubNeurons(brain);
    
    // Save debug results
    std::cout << "\nSaving debug results..." << std::endl;
    debugOutput.saveToFile("debug_results/summary.txt");
    debugOutput.printSummary();
    
    // Export for analysis
    std::cout << "\nExporting data for analysis..." << std::endl;
    debugOutput.exportForMatlab("debug_analysis/neural_data.matlab");
    debugOutput.exportForPython("debug_analysis/neural_data.python");
    
    std::cout << "\n=== Debug Session Complete ===\n" << std::endl;
    std::cout << "Results saved to:" << std::endl;
    std::cout << "  - debug_results/ directory" << std::endl;
    std::cout << "  - debug_visualizations/ directory" << std::endl;
    std::cout << "  - debug_analysis/ directory" << std::endl;
    std::cout << "  - debug_states/ directory" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string configFile = "configs/debug.cfg";
    bool enableDebugger = true;
    bool enableVisualization = true;
    std::string outputDir = "debug_output";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            std::cout << "Usage: nlm_debug [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>      Debug config file (default: configs/debug.cfg)" << std::endl;
            std::cout << "  --no-debugger        Disable brain debugger" << std::endl;
            std::cout << "  --no-visualization   Disable visualization" << std::endl;
            std::cout << "  --output <dir>       Output directory (default: debug_output)" << std::endl;
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--no-debugger") {
            enableDebugger = false;
        } else if (arg == "--no-visualization") {
            enableVisualization = false;
        } else if (arg.substr(0, 8) == "--output") {
            if (arg.find('=') != std::string::npos) {
                outputDir = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputDir = argv[++i];
            }
        }
    }
    
    printDebugBanner();
    
    std::cout << "\nInitializing NLM Debug Environment...\n" << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load debug configuration
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded debug configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default debug configuration." << std::endl;
    }
    
    // Set debug-specific configuration values
    config->set("debug_enabled", true, ConfigSource::Default);
    config->set("visualization_enabled", enableVisualization, ConfigSource::CommandLine);
    config->set("debugger_enabled", enableDebugger, ConfigSource::CommandLine);
    config->set("output_directory", outputDir, ConfigSource::CommandLine);
    
    // Initialize logging for debugging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Debug configuration loaded successfully." << std::endl;
    
    // Run debug session
    runDebugSession(config);
    
    return 0;
}
