/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool showHelp = false;
    bool enableVisualization = false;
    
    if (argc > 1) {
        if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
            showHelp = true;
        } else {
            // Parse neuron count
            try {
                neuronCount = std::stoull(argv[1]);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid neuron count: " << argv[1] << std::endl;
                return 1;
            }
        }
    }
    
    if (argc > 2) {
        if (std::string(argv[2]) == "--help" || std::string(argv[2]) == "-h") {
            showHelp = true;
        } else {
            // Parse max steps
            try {
                maxSteps = std::stoull(argv[2]);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid max steps: " << argv[2] << std::endl;
                return 1;
            }
        }
    }
    
    if (showHelp) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [neurons] [max_steps] [visualization]" << std::endl;
        std::cout << "  neurons         Number of neurons in the brain (default: 500)" << std::endl;
        std::cout << "  max_steps       Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "  visualization   Enable visualization (true/1/yes/enable or false/0/no/disable) (default: false)" << std::endl;
        std::cout << "  --help, -h      Show this help message" << std::endl;
        return 0;
    }
    
    // Parse visualization flag (need to check if it was set)
    if (argc > 3 && !(std::string(argv[3]) == "--help" || std::string(argv[3]) == "-h")) {
        std::string vizArg = argv[3];
        std::transform(vizArg.begin(), vizArg.end(), vizArg.begin(), ::tolower);
        enableVisualization = (vizArg == "true" || vizArg == "1" || vizArg == "yes" || vizArg == "enable");
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Initialize visualization if enabled
    VisualizationInterface visualization;
    if (enableVisualization) {
        std::cout << "Initializing visualization..." << std::endl;
        if (!visualization.initialize()) {
            std::cerr << "WARNING: Failed to initialize visualization, continuing without it" << std::endl;
            enableVisualization = false;
        } else {
            visualization.setUpdateRate(10.0); // 10 Hz update rate
            std::cout << "Visualization initialized successfully" << std::endl;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    
    // Update visualization if enabled
    if (enableVisualization) {
        visualization.update();
        visualization.render();
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    
    // Update visualization if enabled
    if (enableVisualization) {
        visualization.update();
        visualization.render();
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    
    // Update visualization if enabled
    if (enableVisualization) {
        visualization.update();
        visualization.render();
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    
    // Update visualization if enabled
    if (enableVisualization) {
        visualization.update();
        visualization.render();
    }
    
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    // Update visualization if enabled
    if (enableVisualization) {
        visualization.update();
        visualization.render();
    }
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Clean up visualization if it was initialized
    if (enableVisualization && visualization.isActive()) {
        visualization.close();
        std::cout << "Visualization closed" << std::endl;
    }
    
    return 0;
}
