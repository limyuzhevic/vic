/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>

using namespace nlm;

/**
 * Parse command line arguments for enhanced CLI
 */
struct DemoArgs {
    std::string mode = "run"; // run, quick, analyze, benchmark, config
    std::string configFile = "configs/default.cfg";
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool verbose = false;
    std::string outputFile;
};

DemoArgs parseArgs(int argc, char* argv[]) {
    DemoArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--mode" && i + 1 < argc) {
            args.mode = argv[++i];
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            args.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            args.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--no-checkpointing") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            args.enableReplay = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg == "--help") {
            printHelp();
            exit(0);
        }
    }
    
    return args;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Demo Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --mode <mode>           Demo mode (run, quick, analyze, benchmark, config)" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neuron-count <count>  Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <count>         Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  --no-checkpointing       Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              Disable replay system" << std::endl;
    std::cout << "  --no-development         Disable development system" << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << "  --output <file>         Save results to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Special Commands:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
}

void runQuickIntegration(Phase6IntegratedExperiment& experiment, bool verbose) {
    std::cout << "--- Quick Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        exit(1);
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    if (verbose) {
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
    }
}

void runAnalysis(Phase6IntegratedExperiment& experiment, bool verbose) {
    std::cout << "--- Network Analysis ---" << std::endl;
    
    // Create a minimal brain for analysis
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for analysis" << std::endl;
        return;
    }
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Analyze network
    std::cout << "Network Analysis Results:" << std::endl;
    std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  E/I Balance: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Developmental Stage: " << brain->getDevelopmentalStage() << std::endl;
    
    if (verbose) {
        std::cout << "  Working Memory Traces: " << (brain->getWorkingMemory() ? 
                                                   brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        std::cout << "  Episodic Episodes: " << (brain->getEpisodicMemory() ? 
                                               brain->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
    }
}

void runBenchmark(Phase6IntegratedExperiment& experiment, DemoArgs& args) {
    std::cout << "--- Performance Benchmark ---" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", args.neuronCount);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmark" << std::endl;
        return;
    }
    
    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    for (uint64_t step = 0; step < args.maxSteps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0 && args.verbose) {
            std::cout << "  Step " << step << "/" << args.maxSteps 
                     << " | Firing: " << brain->getFiringNeuronCount() 
                     << " | Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double>(end - start);
    double stepsPerSecond = args.maxSteps / duration.count();
    double msPerStep = duration.count() * 1000.0 / args.maxSteps;
    
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Total Steps: " << args.maxSteps << std::endl;
    std::cout << "  Total Time: " << duration.count() << "s" << std::endl;
    std::cout << "  Steps per Second: " << stepsPerSecond << std::endl;
    std::cout << "  Milliseconds per Step: " << msPerStep << std::endl;
    std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
}

void runConfigAnalysis(DemoArgs& args) {
    std::cout << "--- Configuration Analysis ---" << std::endl;
    
    std::cout << "Current Configuration:" << std::endl;
    std::cout << "  Mode: " << args.mode << std::endl;
    std::cout << "  Neuron Count: " << args.neuronCount << std::endl;
    std::cout << "  Max Steps: " << args.maxSteps << std::endl;
    std::cout << "  Config File: " << args.configFile << std::endl;
    std::cout << "  Checkpointing: " << (args.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (args.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (args.enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << "  Verbose: " << (args.verbose ? "enabled" : "disabled") << std::endl;
    
    if (!args.outputFile.empty()) {
        std::cout << "  Output File: " << args.outputFile << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse arguments
    DemoArgs args = parseArgs(argc, argv);
    
    // Print banner
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run based on mode
    if (args.mode == "quick") {
        runQuickIntegration(experiment, args.verbose);
    } else if (args.mode == "analyze") {
        runAnalysis(experiment, args.verbose);
    } else if (args.mode == "benchmark") {
        runBenchmark(experiment, args);
    } else if (args.mode == "config") {
        runConfigAnalysis(args);
        return 0;
    } else { // run (default)
        // Run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
        if (args.verbose) {
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
        }
        
        // Run full experiment with configured settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = args.neuronCount;
        config.maxSteps = args.maxSteps;
        config.enableCheckpointing = args.enableCheckpointing;
        config.enableReplay = args.enableReplay;
        config.enableDevelopment = args.enableDevelopment;
        config.checkpointPath = args.outputFile.empty() ? "./checkpoint_test.bin" : args.outputFile;
        
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
        
        // Save results to file if requested
        if (!args.outputFile.empty()) {
            // In a real implementation, this would save to a JSON or binary file
            std::cout << "Results would be saved to: " << args.outputFile << std::endl;
        }
    }
    
    return 0;
}
