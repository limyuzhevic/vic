/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   Phase6Demo [options]
 * 
 * Options:
 *   --neuron-count <N>            Number of neurons (default: 500)
 *   --max-steps <N>               Maximum simulation steps (default: 2000)
 *   --enable-checkpointing [val]   Enable/disable checkpointing (default: true)
 *   --checkpoint-path <path>      Checkpoint file path (default: ./checkpoint_demo.bin)
 *   --enable-replay [val]         Enable/disable replay (default: true)
 *   --enable-development [val]    Enable/disable development (default: true)
 *   --region-count <N>            Number of regions (default: 1)
 *   --connection-prob <p>         Connection probability (default: 0.1)
 *   --output-file <path>          Save experiment results to file
 *   --verbose                     Enable verbose output
 *   --help                        Show this help message
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

using namespace nlm;

// Helper function to convert string to bool
bool stringToBool(const std::string& str) {
    std::string lower = str;
    for (char& c : lower) c = std::tolower(c);
    return lower == "true" || lower == "1" || lower == "yes";
}

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

int parseArgs(int argc, char* argv[], Phase6Config& config, std::string& outputFile, bool& verbose) {
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            std::cout << "=== Phase 6 Integration Demo Help ===" << std::endl;
            std::cout << "Usage: Phase6Demo [options]" << std::endl << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --neuron-count <N>            Number of neurons (default: 500)" << std::endl;
            std::cout << "  --max-steps <N>               Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --enable-checkpointing [val]   Enable/disable checkpointing (default: true)" << std::endl;
            std::cout << "  --checkpoint-path <path>      Checkpoint file path (default: ./checkpoint_demo.bin)" << std::endl;
            std::cout << "  --enable-replay [val]         Enable/disable replay (default: true)" << std::endl;
            std::cout << "  --enable-development [val]    Enable/disable development (default: true)" << std::endl;
            std::cout << "  --region-count <N>            Number of regions (default: 1)" << std::endl;
            std::cout << "  --connection-prob <p>         Connection probability (default: 0.1)" << std::endl;
            std::cout << "  --output-file <path>          Save experiment results to file" << std::endl;
            std::cout << "  --verbose                     Enable verbose output" << std::endl;
            std::cout << "  --help                        Show this help message" << std::endl;
            return 1;
        }
        
        if (arg.substr(0, 2) == "--") {
            std::string key = arg;
            std::string value = "";
            
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            
            args[key] = value;
        }
    }
    
    // Set output file
    if (args.find("--output-file") != args.end()) {
        outputFile = args["--output-file"];
    }
    
    // Set verbose mode
    if (args.find("--verbose") != args.end()) {
        verbose = true;
    }
    
    // Set neuron count
    if (args.find("--neuron-count") != args.end()) {
        try {
            config.neuronCount = std::stoull(trim(args["--neuron-count"]));
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid neuron-count value: " << args["--neuron-count"] << std::endl;
            return 1;
        }
    }
    
    // Set max steps
    if (args.find("--max-steps") != args.end()) {
        try {
            config.maxSteps = std::stoull(trim(args["--max-steps"]));
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid max-steps value: " << args["--max-steps"] << std::endl;
            return 1;
        }
    }
    
    // Set enable checkpointing
    if (args.find("--enable-checkpointing") != args.end()) {
        config.enableCheckpointing = stringToBool(args["--enable-checkpointing"]);
    }
    
    // Set checkpoint path
    if (args.find("--checkpoint-path") != args.end()) {
        config.checkpointPath = trim(args["--checkpoint-path"]);
    }
    
    // Set enable replay
    if (args.find("--enable-replay") != args.end()) {
        config.enableReplay = stringToBool(args["--enable-replay"]);
    }
    
    // Set enable development
    if (args.find("--enable-development") != args.end()) {
        config.enableDevelopment = stringToBool(args["--enable-development"]);
    }
    
    // Set region count
    if (args.find("--region-count") != args.end()) {
        try {
            config.regionCount = std::stoull(trim(args["--region-count"]));
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid region-count value: " << args["--region-count"] << std::endl;
            return 1;
        }
    }
    
    // Set connection probability
    if (args.find("--connection-prob") != args.end()) {
        try {
            config.connectionProbability = std::stof(trim(args["--connection-prob"]));
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid connection-prob value: " << args["--connection-prob"] << std::endl;
            return 1;
        }
    }
    
    return 0;
}

void printExperimentHeader(const Phase6Config& config, bool verbose) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    if (verbose) {
        std::cout << "=== Configuration ===" << std::endl;
        std::cout << "Neuron Count: " << config.neuronCount << std::endl;
        std::cout << "Max Steps: " << config.maxSteps << std::endl;
        std::cout << "Region Count: " << config.regionCount << std::endl;
        std::cout << "Connection Probability: " << config.connectionProbability << std::endl;
        std::cout << "Enable Checkpointing: " << (config.enableCheckpointing ? "true" : "false") << std::endl;
        std::cout << "Checkpoint Path: " << config.checkpointPath << std::endl;
        std::cout << "Enable Replay: " << (config.enableReplay ? "true" : "false") << std::endl;
        std::cout << "Enable Development: " << (config.enableDevelopment ? "true" : "false") << std::endl;
        std::cout << std::endl;
    }
}

void printExperimentFooter(const Phase6IntegrationResult& result) {
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
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
    std::cout << "Experiment duration: " << (result.endTime - result.startTime) << "s" << std::endl;
}

bool saveResultsToFile(const Phase6IntegrationResult& result, const std::string& filename) {
    if (filename.empty()) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open output file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "=== NLM Phase 6 Integration Experiment Results ===" << std::endl << std::endl;
    
    file << "=== Metrics ===" << std::endl;
    file << "Total Reward: " << result.totalReward << std::endl;
    file << "Avg Firing Rate: " << result.avgFiringRate << std::endl;
    file << "Avg Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    file << "Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    file << "Novelty Level: " << result.noveltyLevel << std::endl;
    file << "Curiosity Level: " << result.curiosityLevel << std::endl;
    file << "Dopamine Level: " << result.dopamineLevel << std::endl;
    file << std::endl;
    
    file << "=== Integration Status ===" << std::endl;
    file << "Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << std::endl;
    file << "Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << std::endl;
    file << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "true" : "false") << std::endl;
    file << "Prediction Integrated: " << (result.predictionIntegrated ? "true" : "false") << std::endl;
    file << "Development Integrated: " << (result.developmentIntegrated ? "true" : "false") << std::endl;
    file << "Checkpointing Works: " << (result.checkpointingWorks ? "true" : "false") << std::endl;
    file << "Replay Works: " << (result.replayWorks ? "true" : "false") << std::endl;
    file << std::endl;
    
    file << "=== Timing ===" << std::endl;
    file << "Start Time: " << result.startTime << std::endl;
    file << "End Time: " << result.endTime << std::endl;
    file << "Wall Clock Time: " << result.totalWallClockTime << std::endl;
    
    file.close();
    
    std::cout << "Results saved to: " << filename << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    Phase6Config config;
    std::string outputFile = "";
    bool verbose = false;
    
    // Parse command line arguments
    if (parseArgs(argc, argv, config, outputFile, verbose) != 0) {
        return 1;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Print experiment header
    printExperimentHeader(config, verbose);
    
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
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running with " << config.neuronCount << " neurons for " << config.maxSteps << " steps..." << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    printExperimentFooter(result);
    
    // Save results to file if requested
    if (!outputFile.empty()) {
        if (!saveResultsToFile(result, outputFile)) {
            std::cerr << "Warning: Failed to save results to file" << std::endl;
        }
    }
    
    return 0;
}
    
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
    config.neuronCount = 500;
    config.maxSteps = 2000;
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
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
