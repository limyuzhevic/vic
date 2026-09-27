// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// Enhanced Command Line Interface with comprehensive configuration management
// Supports multiple execution modes, performance monitoring, and advanced features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace nlm;

// Command line options parser
struct CLIOptions {
    std::string mode = "demo"; // demo, test, experiment, phase6, phase4, phase3
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string logLevel = "info";
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
    bool saveState = false;
    std::string loadState;
    uint64_t steps = 1000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    uint64_t randomSeed = 42;
    double timestep = 0.001;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint.bin";
    bool help = false;
    bool version = false;
    
    // Phase 6 specific options
    bool integrationTest = false;
    bool memoryTest = false;
    bool neuromodTest = false;
    bool checkpointTest = false;
    bool replayTest = false;
    
    // Performance monitoring
    bool enableMetrics = false;
    std::string metricsOutput;
    bool profile = false;
    
    // Advanced debugging
    bool enableDebug = false;
    std::string debugOutput;
    bool traceSpikes = false;
    bool tracePlasticity = false;
    
    // Visualization options
    bool enableViz = false;
    std::string vizType = "text";
    
    void printHelp() const {
        std::cout << "NLM - Neural Learning Machine (熙然)\n";
        std::cout << "Version 0.1.0 - Phase 6: Final Integration\n\n";
        std::cout << "USAGE:\n";
        std::cout << "  nlm [OPTIONS] [ARGS]\n\n";
        std::cout << "MODES:\n";
        std::cout << "  demo           Run Phase 6 integration demo (default)\n";
        std::cout << "  test           Run unit tests\n";
        std::cout << "  experiment     Run custom experiment\n";
        std::cout << "  phase6         Run Phase 6 integration test\n";
        std::cout << "  phase4         Run Phase 4 cognition demo\n";
        std::cout << "  phase3         Run Phase 3 world interaction demo\n\n";
        std::cout << "OPTIONS:\n";
        std::cout << "  --config FILE     Configuration file (default: configs/default.cfg)\n";
        std::cout << "  --steps N         Number of simulation steps\n";
        std::cout << "  --neurons N       Number of neurons\n";
        std::cout << "  --regions N       Number of neural regions\n";
        std::cout << "  --connections P   Connection probability (0.0-1.0)\n";
        std::cout << "  --seed N          Random seed\n";
        std::cout << "  --timestep T      Simulation timestep\n";
        std::cout << "  --log LEVEL       Log level: trace, debug, info, warn, error, fatal\n";
        std::cout << "  --verbose         Enable verbose output\n";
        std::cout << "  --quiet           Suppress non-essential output\n";
        std::cout << "  --benchmark       Enable benchmark mode\n";
        std::cout << "  --save FILE       Save brain state to file\n";
        std::cout << "  --load FILE       Load brain state from file\n";
        std::cout << "  --output FILE     Save results to file\n";
        std::cout << "  --metrics FILE    Save performance metrics to file\n";
        std::cout << "  --debug FILE      Enable debugging output\n";
        std::cout << "  --checkpoint PATH Path for checkpoint saves\n";
        std::cout << "  --no-checkpoint   Disable checkpointing\n";
        std::cout << "  --no-replay       Disable replay system\n";
        std::cout << "  --no-development  Disable development system\n\n";
        std::cout << "PHASE 6 SPECIFIC OPTIONS:\n";
        std::cout << "  --integration     Run integration test only\n";
        std::cout << "  --memory-test     Test memory integration\n";
        std::cout << "  --neuromod-test   Test neuromodulation integration\n";
        std::cout << "  --checkpoint-test Test checkpoint save/load\n";
        std::cout << "  --replay-test     Test replay system\n\n";
        std::cout << "ADVANCED OPTIONS:\n";
        std::cout << "  --enable-metrics  Enable performance metrics collection\n";
        std::cout << "  --profile         Enable performance profiling\n";
        std::cout << "  --viz TYPE        Visualization: text, html, json\n";
        std::cout << "  --trace-spikes    Trace spike activity\n";
        std::cout << "  --trace-plasticity Trace plasticity events\n\n";
        std::cout << "UTILITY:\n";
        std::cout << "  --help            Show this help message\n";
        std::cout << "  --version         Show version information\n";
    }
    
    void printVersion() const {
        std::cout << "NLM v0.1.0 - Neural Learning Machine\n";
        std::cout << "Phase 6: Final Integration\n";
        std::cout << "Building an artificial developmental brain\n";
    }
    
    bool parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                help = true;
            } else if (arg == "--version") {
                version = true;
            } else if (arg == "--quiet") {
                quiet = true;
                logLevel = "error";
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg == "--benchmark") {
                benchmark = true;
            } else if (arg == "--integration") {
                integrationTest = true;
                mode = "phase6";
            } else if (arg == "--memory-test") {
                memoryTest = true;
                mode = "phase6";
            } else if (arg == "--neuromod-test") {
                neuromodTest = true;
                mode = "phase6";
            } else if (arg == "--checkpoint-test") {
                checkpointTest = true;
                mode = "phase6";
            } else if (arg == "--replay-test") {
                replayTest = true;
                mode = "phase6";
            } else if (arg == "--enable-metrics") {
                enableMetrics = true;
            } else if (arg == "--profile") {
                profile = true;
            } else if (arg == "--trace-spikes") {
                traceSpikes = true;
            } else if (arg == "--trace-plasticity") {
                tracePlasticity = true;
            } else if (arg == "--viz" && i + 1 < argc) {
                enableViz = true;
                vizType = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoull(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                neuronCount = std::stoull(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                regionCount = std::stoull(argv[++i]);
            } else if (arg == "--connections" && i + 1 < argc) {
                connectionProbability = std::stof(argv[++i]);
            } else if (arg == "--seed" && i + 1 < argc) {
                randomSeed = std::stoull(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            } else if (arg == "--log" && i + 1 < argc) {
                logLevel = argv[++i];
            } else if (arg == "--save" && i + 1 < argc) {
                saveState = true;
                outputFile = argv[++i];
            } else if (arg == "--load" && i + 1 < argc) {
                loadState = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "--metrics" && i + 1 < argc) {
                enableMetrics = true;
                metricsOutput = argv[++i];
            } else if (arg == "--debug" && i + 1 < argc) {
                enableDebug = true;
                debugOutput = argv[++i];
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                enableCheckpointing = true;
                checkpointPath = argv[++i];
            } else if (arg == "--no-checkpoint") {
                enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--no-development") {
                enableDevelopment = false;
            } else if (arg == "demo" || arg == "test" || arg == "experiment" ||
                       arg == "phase6" || arg == "phase4" || arg == "phase3") {
                mode = arg;
            } else if (arg.find("--") == 0) {
                std::cerr << "Unknown option: " << arg << std::endl;
                return false;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
};

void printBanner(const std::string& mode) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemInfo() {
    std::cout << "System Information:" << std::endl;
    std::cout << "  NLM Version: 0.1.0" << std::endl;
    std::cout << "  Build Type: Release" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Configuration: Final Integration" << std::endl;
    std::cout << std::endl;
}

void runPhase6Demo(CLIOptions& opts) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Enhanced CLI Mode with comprehensive options" << std::endl;
    std::cout << "Steps: " << opts.steps << ", Neurons: " << opts.neuronCount << std::endl;
    std::cout << std::endl;
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Configure for demo
    Phase6Config config;
    config.neuronCount = opts.neuronCount;
    config.maxSteps = opts.steps;
    config.regionCount = opts.regionCount;
    config.connectionProbability = opts.connectionProbability;
    config.enableCheckpointing = opts.enableCheckpointing;
    config.enableReplay = opts.enableReplay;
    config.enableDevelopment = opts.enableDevelopment;
    config.checkpointPath = opts.checkpointPath;
    
    std::cout << "Running Phase 6 integration experiment..." << std::endl;
    Phase6IntegrationResult result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED RESULTS ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << std::endl;
    std::cout << "=== SYSTEM INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Save results if requested
    if (!opts.outputFile.empty()) {
        std::ofstream outFile(opts.outputFile);
        if (outFile.is_open()) {
            outFile << "NLM Phase 6 Enhanced Results" << std::endl;
            outFile << "=========================" << std::endl;
            outFile << "Total Reward: " << result.totalReward << std::endl;
            outFile << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            outFile << "Memory Episodes: " << result.memoryEpisodesStored << std::endl;
            outFile << "Integration Status:" << std::endl;
            outFile << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
            outFile << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
            outFile << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
            outFile << "  Prediction: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
            outFile << "  Development: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
            outFile << "  Checkpointing: " << (result.checkpointingWorks ? "Yes" : "No") << std::endl;
            outFile << "  Replay: " << (result.replayWorks ? "Yes" : "No") << std::endl;
            outFile << "Wall Time: " << result.totalWallClockTime << "s" << std::endl;
            outFile.close();
            std::cout << "Results saved to: " << opts.outputFile << std::endl;
        }
    }
}

void runTestMode(CLIOptions& opts) {
    std::cout << "=== NLM Test Mode ===" << std::endl;
    std::cout << "Running comprehensive test suite..." << std::endl;
    
    // Run basic integration tests
    Phase6IntegratedExperiment experiment;
    
    bool allTestsPassed = true;
    
    if (opts.integrationTest || !opts.memoryTest && !opts.neuromodTest && !opts.checkpointTest && !opts.replayTest) {
        std::cout << "--- Integration Test ---" << std::endl;
        if (experiment.verifyIntegration()) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
            allTestsPassed = false;
        }
    }
    
    if (opts.memoryTest || !opts.memoryTest && !opts.neuromodTest && !opts.checkpointTest && !opts.replayTest) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        if (experiment.testMemoryIntegration()) {
            std::cout << "✓ Memory integration test PASSED" << std::endl;
        } else {
            std::cout << "✗ Memory integration test FAILED" << std::endl;
            allTestsPassed = false;
        }
    }
    
    if (opts.neuromodTest || !opts.memoryTest && !opts.neuromodTest && !opts.checkpointTest && !opts.replayTest) {
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        if (experiment.testNeuromodulationIntegration()) {
            std::cout << "✓ Neuromodulation integration test PASSED" << std::endl;
        } else {
            std::cout << "✗ Neuromodulation integration test FAILED" << std::endl;
            allTestsPassed = false;
        }
    }
    
    if (opts.checkpointTest || !opts.memoryTest && !opts.neuromodTest && !opts.checkpointTest && !opts.replayTest) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        if (experiment.testCheckpointing()) {
            std::cout << "✓ Checkpoint test PASSED" << std::endl;
        } else {
            std::cout << "✗ Checkpoint test FAILED" << std::endl;
            allTestsPassed = false;
        }
    }
    
    if (opts.replayTest || !opts.memoryTest && !opts.neuromodTest && !opts.checkpointTest && !opts.replayTest) {
        std::cout << "--- Replay Test ---" << std::endl;
        if (experiment.testReplay()) {
            std::cout << "✓ Replay test PASSED" << std::endl;
        } else {
            std::cout << "✗ Replay test FAILED" << std::endl;
            allTestsPassed = false;
        }
    }
    
    if (allTestsPassed) {
        std::cout << std::endl;
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
    }
}

void runPhase4Demo(CLIOptions& opts) {
    std::cout << "NLM Phase 4: Emerging Cognition\n";
    std::cout << "================================\n\n";
    
    // Phase 4 demo setup (similar to original but with enhanced features)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(opts.neuronCount));
    config->set("region_count", size_t(opts.regionCount));
    config->set("connection_probability", opts.connectionProbability);
    config->set("random_seed", uint64_t(opts.randomSeed));
    config->set("simulation_timestep", opts.timestep);
    
    // Create brain
    std::cout << "Initializing brain with enhanced features...\n";
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return;
    }
    
    std::cout << "Phase 4 cognition demonstration complete.\n";
    std::cout << "Brain has " << brain->getTotalNeuronCount() << " neurons and " << brain->getTotalSynapseCount() << " synapses.\n";
}

void runPhase3Demo(CLIOptions& opts) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    
    // Phase 3 demo setup (simplified for enhanced CLI)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Phase 3 world interaction demo complete.\n";
    std::cout << "Brain has " << brain->getTotalNeuronCount() << " neurons.\n";
}

int main(int argc, char* argv[]) {
    printBanner("enhanced");
    printSystemInfo();
    
    CLIOptions opts;
    
    if (!opts.parseArgs(argc, argv)) {
        std::cerr << "Error parsing arguments" << std::endl;
        return 1;
    }
    
    if (opts.help) {
        opts.printHelp();
        return 0;
    }
    
    if (opts.version) {
        opts.printVersion();
        return 0;
    }
    
    // Initialize logger with CLI-specified level
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel;
    
    if (opts.logLevel == "trace") logLevel = LogLevel::Trace;
    else if (opts.logLevel == "debug") logLevel = LogLevel::Debug;
    else if (opts.logLevel == "info") logLevel = LogLevel::Info;
    else if (opts.logLevel == "warn") logLevel = LogLevel::Warn;
    else if (opts.logLevel == "error") logLevel = LogLevel::Error;
    else if (opts.logLevel == "fatal") logLevel = LogLevel::Fatal;
    else logLevel = LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (opts.profile) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (opts.mode == "phase6") {
            runPhase6Demo(opts);
        } else if (opts.mode == "test") {
            runTestMode(opts);
        } else if (opts.mode == "phase4") {
            runPhase4Demo(opts);
        } else if (opts.mode == "phase3") {
            runPhase3Demo(opts);
        } else {
            std::cout << "Default Phase 6 demo mode\n";
            runPhase6Demo(opts);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "=== PERFORMANCE PROFILE ===" << std::endl;
        std::cout << "Total execution time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    } else {
        if (opts.mode == "phase6") {
            runPhase6Demo(opts);
        } else if (opts.mode == "test") {
            runTestMode(opts);
        } else if (opts.mode == "phase4") {
            runPhase4Demo(opts);
        } else if (opts.mode == "phase3") {
            runPhase3Demo(opts);
        } else {
            std::cout << "Default Phase 6 demo mode\n";
            runPhase6Demo(opts);
        }
    }
    
    return 0;
}