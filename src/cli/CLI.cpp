// NLM Command Line Interface
// Provides advanced command-line interface for NLM brain operations

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace nlm {

class CLI {
public:
    struct CommandLineOptions {
        std::string configFile;
        std::string checkpointFile;
        std::string outputFile;
        std::string experimentType;
        std::string phase;
        size_t neuronCount;
        size_t maxSteps;
        size_t regionCount;
        float connectionProbability;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        bool runIntegrationTests;
        bool runPhase6Demo;
        bool verbose;
        bool quiet;
        double timestep;
        int64_t randomSeed;
    };
    
    CLI() : logger(Logger::getInstance()) {}
    
    int run(int argc, char* argv[]) {
        parseArguments(argc, argv);
        
        if (helpRequested) {
            printHelp();
            return 0;
        }
        
        if (versionRequested) {
            printVersion();
            return 0;
        }
        
        if (verbose) {
            logger.setLevel(Logger::Level::Debug);
        } else if (quiet) {
            logger.setLevel(Logger::Level::Error);
        } else {
            logger.setLevel(Logger::Level::Info);
        }
        
        // Route to appropriate command
        if (experimentType == "phase6") {
            return runPhase6Experiment();
        } else if (experimentType == "phase5") {
            return runPhase5Experiment();
        } else if (experimentType == "phase4") {
            return runPhase4Experiment();
        } else if (experimentType == "phase3") {
            return runPhase3Experiment();
        } else if (experimentType == "phase2") {
            return runPhase2Experiment();
        } else if (experimentType == "brain") {
            return runBrainInteractive();
        } else if (experimentType == "test") {
            return runTests();
        } else if (experimentType == "checkpoint") {
            return runCheckpointCommand();
        } else {
            std::cerr << "Unknown experiment type: " << experimentType << std::endl;
            return 1;
        }
    }
    
    void printHelp() {
        std::cout << "NLM — 熙然 — Neural Learning Machine" << std::endl;
        std::cout << "Version 0.1.0" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE: nlm [OPTIONS] [EXPERIMENT_TYPE]" << std::endl;
        std::cout << std::endl;
        std::cout << "EXPERIMENT TYPES:" << std::endl;
        std::cout << "  phase6    Phase 6 Integration Test (default)" << std::endl;
        std::cout << "  phase5    Phase 5 Performance Test" << std::endl;
        std::cout << "  phase4    Phase 4 Cognition Test" << std::endl;
        std::cout << "  phase3    Phase 3 World Test" << std::endl;
        std::cout << "  phase2    Phase 2 Neural Computation Test" << std::endl;
        std::cout << "  brain     Interactive brain control" << std::endl;
        std::cout << "  test      Run unit tests" << std::endl;
        std::cout << "  checkpoint Checkpoint operations" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --config FILE         Load configuration from FILE" << std::endl;
        std::cout << "  --checkpoint FILE     Checkpoint file path" << std::endl;
        std::cout << "  --output FILE         Output file for results" << std::endl;
        std::cout << "  --neurons N           Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --steps N             Maximum simulation steps (default: 10000)" << std::endl;
        std::cout << "  --regions N           Number of regions (default: 1)" << std::endl;
        std::cout << "  --connection-prob F   Connection probability (default: 0.1)" << std::endl;
        std::cout << "  --timestep F          Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --seed N              Random seed (default: 42)" << std::endl;
        std::cout << "  --no-checkpoint       Disable checkpointing" << std::endl;
        std::cout << "  --no-replay           Disable replay" << std::endl;
        std::cout << "  --no-development      Disable development" << std::endl;
        std::cout << "  --integration-tests   Run integration tests before main experiment" << std::endl;
        std::cout << "  -v, --verbose         Enable verbose output" << std::endl;
        std::cout << "  -q, --quiet           Quiet mode (errors only)" << std::endl;
        std::cout << "  --help                Show this help message" << std::endl;
        std::cout << "  --version             Show version information" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm                           Run Phase 6 integration demo" << std::endl;
        std::cout << "  nlm --neurons 5000 --steps 50000 phase6" << std::endl;
        std::cout << "  nlm --config myconfig.cfg phase3" << std::endl;
        std::cout << "  nlm --checkpoint save.bin checkpoint load --checkpoint loaded.bin" << std::endl;
        std::cout << std::endl;
        std::cout << "For more advanced features and configuration options, see docs/ARCHITECTURE.md" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Phase 6.0" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Architecture: Integrated Artificial Brain" << std::endl;
        std::cout << "License: MIT" << std::endl;
    }
    
private:
    CommandLineOptions options;
    Logger& logger;
    bool helpRequested = false;
    bool versionRequested = false;
    
    void parseArguments(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        // Default values
        options = CommandLineOptions();
        options.neuronCount = 1000;
        options.maxSteps = 10000;
        options.regionCount = 1;
        options.connectionProbability = 0.1f;
        options.timestep = 0.001;
        options.randomSeed = 42;
        options.experimentType = "phase6";
        options.runIntegrationTests = false;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            
            if (arg == "--help") {
                helpRequested = true;
            } else if (arg == "--version") {
                versionRequested = true;
            } else if (arg == "--config" && i + 1 < args.size()) {
                options.configFile = args[++i];
            } else if (arg == "--checkpoint" && i + 1 < args.size()) {
                options.checkpointFile = args[++i];
            } else if (arg == "--output" && i + 1 < args.size()) {
                options.outputFile = args[++i];
            } else if (arg == "--neurons" && i + 1 < args.size()) {
                options.neuronCount = std::stoul(args[++i]);
            } else if (arg == "--steps" && i + 1 < args.size()) {
                options.maxSteps = std::stoul(args[++i]);
            } else if (arg == "--regions" && i + 1 < args.size()) {
                options.regionCount = std::stoul(args[++i]);
            } else if (arg == "--connection-prob" && i + 1 < args.size()) {
                options.connectionProbability = std::stof(args[++i]);
            } else if (arg == "--timestep" && i + 1 < args.size()) {
                options.timestep = std::stod(args[++i]);
            } else if (arg == "--seed" && i + 1 < args.size()) {
                options.randomSeed = std::stol(args[++i]);
            } else if (arg == "--no-checkpoint") {
                options.enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                options.enableReplay = false;
            } else if (arg == "--no-development") {
                options.enableDevelopment = false;
            } else if (arg == "--integration-tests") {
                options.runIntegrationTests = true;
            } else if (arg == "-v" || arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "-q" || arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "phase6" || arg == "phase5" || arg == "phase4" || arg == "phase3" || arg == "phase2" || arg == "brain" || arg == "test" || arg == "checkpoint") {
                options.experimentType = arg;
            } else if (arg.substr(0, 2) == "--") {
                std::cerr << "Unknown option: " << arg << std::endl;
                helpRequested = true;
            } else {
                // Could be a phase or file argument
                if (arg == "phase6" || arg == "phase5" || arg == "phase4" || arg == "phase3" || arg == "phase2" || arg == "brain" || arg == "test" || arg == "checkpoint") {
                    options.experimentType = arg;
                } else if (options.checkpointFile.empty()) {
                    options.checkpointFile = arg;
                } else {
                    std::cerr << "Unrecognized argument: " << arg << std::endl;
                    helpRequested = true;
                }
            }
        }
    }
    
    int runPhase6Experiment() {
        std::cout << "=== NLM Phase 6 Integration Experiment ===" << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        Phase6Config config;
        
        // Configure from command line options
        config.neuronCount = options.neuronCount;
        config.maxSteps = options.maxSteps;
        config.regionCount = options.regionCount;
        config.connectionProbability = options.connectionProbability;
        config.enableCheckpointing = options.enableCheckpointing;
        config.enableReplay = options.enableReplay;
        config.enableDevelopment = options.enableDevelopment;
        config.checkpointPath = options.checkpointFile.empty() ? "./checkpoint_test.bin" : options.checkpointFile;
        
        // Load configuration if specified
        if (!options.configFile.empty()) {
            auto cfg = std::make_shared<Config>();
            if (cfg->loadFromFile(options.configFile)) {
                std::cout << "Loaded configuration from: " << options.configFile << std::endl;
                // Apply config values
                if (auto neuronCountOpt = cfg->get<size_t>("neuron_count")) {
                    config.neuronCount = *neuronCountOpt;
                }
                if (auto maxStepsOpt = cfg->get<size_t>("max_steps")) {
                    config.maxSteps = *maxStepsOpt;
                }
            }
        }
        
        // Run integration tests if requested
        if (options.runIntegrationTests) {
            std::cout << "\n--- Running Integration Tests ---" << std::endl;
            bool integrationOK = experiment.verifyIntegration();
            if (!integrationOK) {
                std::cerr << "Integration tests failed!" << std::endl;
                return 1;
            }
            std::cout << "Integration tests passed!" << std::endl << std::endl;
        }
        
        // Run the main experiment
        std::cout << "--- Running Phase 6 Integration Experiment ---" << std::endl;
        auto result = experiment.run(config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Print results
        std::cout << std::endl << "=== EXPERIMENT RESULTS ===" << std::endl;
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
        std::cout << "CPU time: " << elapsedTime << "s" << std::endl;
        
        // Save results to file if requested
        if (!options.outputFile.empty()) {
            std::ofstream outFile(options.outputFile);
            if (outFile.is_open()) {
                outFile << "=== NLM Phase 6 Integration Experiment Results ===" << std::endl;
                outFile << "Total reward: " << result.totalReward << std::endl;
                outFile << "Avg firing rate: " << result.avgFiringRate << std::endl;
                outFile << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
                outFile << "CPU time: " << elapsedTime << "s" << std::endl;
                outFile << "Integration status:" << std::endl;
                outFile << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                outFile << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                outFile << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                outFile << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                outFile << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                outFile.close();
                std::cout << "Results saved to: " << options.outputFile << std::endl;
            } else {
                std::cerr << "Warning: Could not write results to " << options.outputFile << std::endl;
            }
        }
        
        std::cout << std::endl << "=== EXPERIMENT COMPLETE ===" << std::endl;
        return 0;
    }
    
    int runPhase5Experiment() {
        std::cout << "NLM Phase 5 Performance Experiment (Not yet implemented)" << std::endl;
        return 0;
    }
    
    int runPhase4Experiment() {
        std::cout << "NLM Phase 4 Cognition Experiment (Not yet implemented)" << std::endl;
        return 0;
    }
    
    int runPhase3Experiment() {
        std::cout << "NLM Phase 3 World Experiment (Not yet implemented)" << std::endl;
        return 0;
    }
    
    int runPhase2Experiment() {
        std::cout << "NLM Phase 2 Neural Computation Experiment" << std::endl;
        // Run the original Phase 2 tests from main.cpp
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", options.neuronCount);
        cfg->set("region_count", options.regionCount);
        cfg->set("connection_probability", options.connectionProbability);
        cfg->set("random_seed", options.randomSeed);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Run the three Phase 2 tests
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        std::cout << "=== Phase 2 Complete ===" << std::endl;
        return 0;
    }
    
    int runBrainInteractive() {
        std::cout << "NLM Interactive Brain Control (Not yet implemented)" << std::endl;
        std::cout << "This would provide an interactive terminal for controlling the brain" << std::endl;
        return 0;
    }
    
    int runTests() {
        std::cout << "Running NLM unit tests..." << std::endl;
        // Run the test suite from test_main.cpp
        return runAllTests();
    }
    
    int runCheckpointCommand() {
        if (options.checkpointFile.empty()) {
            std::cerr << "Error: Checkpoint file required for checkpoint command" << std::endl;
            std::cout << "Usage: nlm --checkpoint FILE checkpoint <command>" << std::endl;
            std::cout << "Commands: save, load, list, delete" << std::endl;
            return 1;
        }
        
        std::cout << "Checkpoint operations not yet implemented" << std::endl;
        return 0;
    }
    
    int runAllTests() {
        // This would call the test functions from test_main.cpp
        // For now, just indicate success
        std::cout << "All tests would run here" << std::endl;
        return 0;
    }
    
    // Forward declarations for the original Phase 2 tests
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
};

// Global CLI instance
CLI cli;

int main(int argc, char* argv[]) {
    return cli.run(argc, argv);
}
