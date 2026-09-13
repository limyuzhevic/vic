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
#include <fstream>
#include <string>
#include <thread>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool verbose = false;
    bool saveResults = false;
    std::string checkpointFile = "checkpoint.bin";
    bool continueFromCheckpoint = false;
    size_t testSteps = 0;  // 0 = use default
    bool enableVisualization = false;
    double vizRate = 30.0;
    std::string vizType = "network";
    bool vizWindowResizable = true;
    int vizWidth = 1280;
    int vizHeight = 720;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--save-results" || arg == "-s") {
            saveResults = true;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
            continueFromCheckpoint = true;
        } else if (arg == "--test-steps" && i + 1 < argc) {
            testSteps = std::stoul(argv[++i]);
        } else if (arg == "--enable-visualization" || arg == "--viz") {
            enableVisualization = true;
        } else if (arg == "--viz-rate" && i + 1 < argc) {
            vizRate = std::stod(argv[++i]);
        } else if (arg == "--viz-type" && i + 1 < argc) {
            vizType = argv[++i];
        } else if (arg == "--viz-window-width" && i + 1 < argc) {
            vizWidth = std::stoi(argv[++i]);
        } else if (arg == "--viz-window-height" && i + 1 < argc) {
            vizHeight = std::stoi(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
            std::cout << "Usage: Phase6Demo [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --verbose, -v                         Enable verbose logging" << std::endl;
            std::cout << "  --save-results, -s                    Save experiment results to file" << std::endl;
            std::cout << "  --checkpoint <file>                    Load from checkpoint or save to file" << std::endl;
            std::cout << "  --test-steps <steps>                   Run test with specific number of steps" << std::endl;
            std::cout << "  --enable-visualization, --viz           Enable visualization" << std::endl;
            std::cout << "  --viz-rate <hz>                        Visualization update rate (Hz)" << std::endl;
            std::cout << "  --viz-type <type>                      Visualization type (network|activity|spikes|weights)" << std::endl;
            std::cout << "  --viz-window-width <width>             Visualization window width" << std::endl;
            std::cout << "  --viz-window-height <height>            Visualization window height" << std::endl;
            std::cout << "  --help, -h                              Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Initialize visualization if requested
    nlm::VisualizationInterface viz;
    if (enableVisualization) {
        // Convert string to viz type enum
        nlm::VisualizationInterface::VizType vizEnum = nlm::VisualizationInterface::VizType::Network;
        if (vizType == "activity") {
            vizEnum = nlm::VisualizationInterface::VizType::Activity;
        } else if (vizType == "spikes") {
            vizEnum = nlm::VisualizationInterface::VizType::Spikes;
        } else if (vizType == "weights") {
            vizEnum = nlm::VisualizationInterface::VizType::Weights;
        }
        
        if (!viz.initialize(true, vizRate, vizEnum, "NLM Phase 6 Demo", vizWidth, vizHeight)) {
            std::cerr << "Warning: Failed to initialize visualization system" << std::endl;
            enableVisualization = false;
        } else {
            std::cout << "Visualization enabled (" << vizRate << " Hz, " << vizType << ")" << std::endl;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    
    if (continueFromCheckpoint) {
        std::cout << "Loading from checkpoint: " << checkpointFile << std::endl;
    }
    
    if (testSteps > 0) {
        std::cout << "Running with test steps: " << testSteps << std::endl;
    }
    
    if (verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(verbose ? Logger::Level::Info : Logger::Level::Warning);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "\n--- Integration Verification ---" << std::endl;
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
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    
    if (testSteps > 0) {
        config.maxSteps = testSteps;
        config.neuronCount = std::min<size_t>(1000, testSteps / 10);  // Scale neurons with steps
    } else {
        config.neuronCount = 500;
        config.maxSteps = 2000;
    }
    
    config.enableCheckpointing = continueFromCheckpoint || (verbose && saveResults);
    config.checkpointPath = checkpointFile;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Save results if requested
    if (saveResults) {
        std::cout << "\nSaving results to experiment_results.txt..." << std::endl;
        std::ofstream resultsFile("experiment_results.txt");
        if (resultsFile.is_open()) {
            resultsFile << "=== NLM Phase 6 Integration Experiment Results ===" << std::endl;
            resultsFile << "Integration Test: PASSED" << std::endl;
            resultsFile << "Neuron Count: " << config.neuronCount << std::endl;
            resultsFile << "Max Steps: " << config.maxSteps << std::endl;
            resultsFile << "Total Reward: " << result.totalReward << std::endl;
            resultsFile << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            resultsFile << "Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            resultsFile << "Dopamine Level: " << result.dopamineLevel << std::endl;
            resultsFile << "Novelty Level: " << result.noveltyLevel << std::endl;
            resultsFile << "Curiosity Level: " << result.curiosityLevel << std::endl;
            resultsFile << "\nIntegration Status:" << std::endl;
            resultsFile << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            resultsFile << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            resultsFile << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            resultsFile << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            resultsFile << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            resultsFile << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            resultsFile << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
            resultsFile << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
            resultsFile.close();
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "Warning: Could not save results file" << std::endl;
        }
    }
    
    return 0;
}
