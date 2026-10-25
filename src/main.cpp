// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the complete integrated artificial brain with:
// - All memory systems integrated (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system with sensory prediction and error computation
// - Cognition systems (neural planner, concept formation, attention)
// - Development stages affecting plasticity rates
// - Checkpoint save/load with compression
// - Replay and consolidation during sleep/rest cycles
// - Complete closed-loop world interaction
//
// Enhanced NLM Phase 6 with comprehensive CLI for advanced users
// and multi-mode operation including debugging, visualization, and experimentation.

#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>

using namespace nlm;

// Forward declarations
namespace nlm {
    namespace cli {
        void showHelp(const std::string& programName);
        bool parseArguments(int argc, char* argv[], std::map<std::string, std::string>& args);
        void printBanner();
        void printVersion();
        void printConfigInfo();
        bool handleCommand(const std::string& cmd, const std::map<std::string, std::string>& args);
        bool runDemoMode(const std::map<std::string, std::string>& args);
        bool runDebugMode(const std::map<std::string, std::string>& args);
        bool runExperimentMode(const std::map<std::string, std::string>& args);
        bool runVisualizationMode(const std::map<std::string, std::string>& args);
        bool runPluginMode(const std::map<std::string, std::string>& args);
        bool runBenchmarkMode(const std::map<std::string, std::string>& args);
        bool runIntegrationTest(const std::map<std::string, std::string>& args);
    }
}

// ===== CLI COMMAND IMPLEMENTATIONS =====

void nlm::cli::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void nlm::cli::printVersion() {
    std::cout << "NLM Phase 6 - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6: Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Complete integrated artificial brain system" << std::endl;
}

void nlm::cli::printConfigInfo() {
    std::cout << "=== NLM Phase 6 Configuration ===" << std::endl;
    std::cout << "All memory systems integrated:" << std::endl;
    std::cout << "  - Working Memory (neural persistent activity)" << std::endl;
    std::cout << "  - Episodic Memory (experience storage)" << std::endl;
    std::cout << "  - Associative Memory (pattern associations)" << std::endl;
    std::cout << std::endl;
    std::cout << "Neuromodulation systems:" << std::endl;
    std::cout << "  - Dopamine (reward prediction error)" << std::endl;
    std::cout << "  - Curiosity (exploration motivation)" << std::endl;
    std::cout << "  - Novelty (novelty detection)" << std::endl;
    std::cout << "  - Prediction Error (sensory prediction)" << std::endl;
    std::cout << std::endl;
    std::cout << "Cognition systems:" << std::endl;
    std::cout << "  - Neural Planner (action planning)" << std::endl;
    std::cout << "  - Concept Formation (pattern discovery)" << std::endl;
    std::cout << "  - Attentional Selection (competitive selection)" << std::endl;
    std::cout << std::endl;
    std::cout << "Development system with stage-based plasticity:" << std::endl;
    std::cout << "  - Initial (high plasticity)" << std::endl;
    std::cout << "  - Critical Period" << std::endl;
    std::cout << "  - Maturation" << std::endl;
    std::cout << "  - Adult (stable)" << std::endl;
}

void nlm::cli::showHelp(const std::string& programName) {
    printBanner();
    std::cout << std::endl;
    std::cout << "USAGE: " << programName << " [MODE] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES (choose one):" << std::endl;
    std::cout << "  demo          - Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  debug          - Debug mode with detailed logging" << std::endl;
    std::cout << "  experiment     - Run custom experiment" << std::endl;
    std::cout << "  visualization  - Interactive visualization" << std::endl;
    std::cout << "  plugin         - Plugin development mode" << std::endl;
    std::cout << "  benchmark      - Performance benchmarking" << std::endl;
    std::cout << "  integration    - Integration verification tests" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --help, -h     Show this help message" << std::endl;
    std::cout << "  --version, -v  Show version information" << std::endl;
    std::cout << "  --config FILE  Load configuration from FILE" << std::endl;
    std::cout << "  --steps N      Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --neurons N    Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions N    Number of regions (default: 1)" << std::endl;
    std::cout << "  --seed VAL     Random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint FILE  Enable checkpointing, save to FILE" << std::endl;
    std::cout << "  --replay        Enable memory replay" << std::endl;
    std::cout << "  --develop       Enable development stages" << std::endl;
    std::cout << "  --output FILE   Save results to FILE" << std::endl;
    std::cout << "  --verbose, -V  Enable verbose logging" << std::endl;
    std::cout << "  --quiet, -q    Minimal output" << std::endl;
    std::cout << std::endl;
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --memory-capacity N  Working memory capacity" << std::endl;
    std::cout << "  --plasticity-mod MOD  Plasticity modulation factor" << std::endl;
    std::cout << "  --reward-weight W     Reward weight for learning" << std::endl;
    std::cout << "  --curiosity-threshold T  Curiosity threshold" << std::endl;
    std::cout << "  --attention-strength S  Attention system strength" << std::endl;
    std::cout << "  --sensory-gain G      Sensory input gain" << std::endl;
    std::cout << "  --motor-response R    Motor response rate" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  " << programName << " demo --neurons 500 --steps 2000" << std::endl;
    std::cout << "  " << programName << " debug --config myconfig.cfg --verbose" << std::endl;
    std::cout << "  " << programName << " integration --steps 5000" << std::endl;
    std::cout << "  " << programName << " benchmark --neurons 2000 --steps 10000" << std::endl;
    std::cout << std::endl;
    printConfigInfo();
}

bool nlm::cli::parseArguments(int argc, char* argv[], std::map<std::string, std::string>& args) {
    bool valid = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if ((arg == "--help" || arg == "-h")) {
            // Help is handled separately
            continue;
        } else if ((arg == "--version" || arg == "-v")) {
            // Version is handled separately
            continue;
        } else if (arg.substr(0, 2) == "--") {
            // Long option: --key=value or --key value
            size_t pos = arg.find('=');
            std::string key, value;
            
            if (pos != std::string::npos) {
                key = arg.substr(2, pos - 2);
                value = arg.substr(pos + 1);
            } else {
                key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    value = "true";
                }
            }
            
            // Convert key to lowercase for consistency
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            args[key] = value;
        } else if (arg[0] == '-') {
            // Short option: -k value
            std::string key = arg.substr(1);
            std::string value;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            } else {
                value = "true";
            }
            
            // Convert key to lowercase for consistency
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            args[key] = value;
        } else {
            // Positional argument (mode)
            std::string mode = arg;
            std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
            args["mode"] = mode;
        }
    }
    
    // Set default mode if not specified
    if (args.find("mode") == args.end()) {
        args["mode"] = "demo";
    }
    
    return valid;
}

bool nlm::cli::handleCommand(const std::string& cmd, const std::map<std::string, std::string>& args) {
    std::string mode = cmd;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    if (mode == "demo") {
        return runDemoMode(args);
    } else if (mode == "debug") {
        return runDebugMode(args);
    } else if (mode == "experiment") {
        return runExperimentMode(args);
    } else if (mode == "visualization") {
        return runVisualizationMode(args);
    } else if (mode == "plugin") {
        return runPluginMode(args);
    } else if (mode == "benchmark") {
        return runBenchmarkMode(args);
    } else if (mode == "integration") {
        return runIntegrationTest(args);
    } else {
        std::cerr << "ERROR: Unknown mode: " << mode << std::endl;
        std::cerr << "Use --help for available modes" << std::endl;
        return false;
    }
}

bool nlm::cli::runDemoMode(const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Phase 6 Demo Mode ===" << std::endl;
    
    // Parse arguments with defaults
    size_t neurons = 1000;
    size_t steps = 1000;
    size_t regions = 1;
    int seed = 42;
    std::string configFile;
    bool enableCheckpoint = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    std::string outputFile;
    
    // Parse arguments
    auto it = args.find("neurons");
    if (it != args.end()) neurons = std::stoul(it->second);
    
    it = args.find("steps");
    if (it != args.end()) steps = std::stoul(it->second);
    
    it = args.find("regions");
    if (it != args.end()) regions = std::stoul(it->second);
    
    it = args.find("seed");
    if (it != args.end()) seed = std::stoi(it->second);
    
    it = args.find("config");
    if (it != args.end()) configFile = it->second;
    
    it = args.find("checkpoint");
    if (it != args.end()) {
        enableCheckpoint = true;
        // Could parse checkpoint file path
    }
    
    it = args.find("replay");
    if (it != args.end()) enableReplay = true;
    
    it = args.find("develop");
    if (it != args.end()) enableDevelopment = true;
    
    it = args.find("output");
    if (it != args.end()) outputFile = it->second;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neurons << std::endl;
    std::cout << "  Regions: " << regions << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    std::cout << "  Seed: " << seed << std::endl;
    std::cout << "  Checkpoint: " << (enableCheckpoint ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", neurons);
    cfg->set("region_count", regions);
    cfg->set("random_seed", static_cast<int64_t>(seed));
    cfg->set("simulation_timestep", 0.001);
    cfg->set("connection_probability", 0.1f);
    
    // Create and run Phase 6 demo
    Phase6Demo demo;
    
    if (enableCheckpoint) {
        std::cout << "Checkpointing enabled, but need Phase6Config for checkpoint path" << std::endl;
        // Would need to modify Phase6Demo to support checkpointing
    }
    
    // Run integration verification first
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = demo.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Run individual system tests
    std::cout << "Testing individual systems..." << std::endl;
    demo.testMemoryIntegration();
    std::cout << std::endl;
    demo.testNeuromodulationIntegration();
    std::cout << std::endl;
    demo.testCheckpointing();
    std::cout << std::endl;
    demo.testReplay();
    std::cout << std::endl;
    
    // Run full integration experiment if requested
    if (enableDevelopment || enableReplay || enableCheckpoint) {
        std::cout << "Running full integration experiment..." << std::endl;
        
        Phase6Config config;
        config.neuronCount = neurons;
        config.maxSteps = steps;
        config.enableCheckpointing = enableCheckpoint;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        
        if (!outputFile.empty()) {
            config.checkpointPath = outputFile;
        }
        
        auto result = demo.run(config);
        
        std::cout << "=== DEMO RESULTS ===" << std::endl;
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
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    return true;
}

bool nlm::cli::runDebugMode(const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Phase 6 Debug Mode ===" << std::endl;
    
    // Parse arguments
    bool verbose = args.find("verbose") != args.end();
    bool quiet = args.find("quiet") != args.end();
    std::string configFile;
    
    auto it = args.find("config");
    if (it != args.end()) configFile = it->second;
    
    if (verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
        // Enable debug logging
    }
    
    if (quiet) {
        std::cout << "Quiet mode - minimal output" << std::endl;
    }
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("random_seed", static_cast<int64_t>(42));
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    brain->logStatus();
    
    // Run debug steps
    std::cout << "Running debug simulation..." << std::endl;
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                     << "Spikes: " << brain->getTotalSpikeCount() << " "
                     << "Active: " << brain->getActiveNeuronCount() << " "
                     << "WorkingMem: " << (brain->getWorkingMemory() ? 
                         brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    std::cout << "Debug mode complete" << std::endl;
    return true;
}

bool nlm::cli::runIntegrationTest(const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "Testing individual systems..." << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    experiment.testReplay();
    
    std::cout << std::endl;
    std::cout << "=== INTEGRATION TEST COMPLETE ===" << std::endl;
    return true;
}

bool nlm::cli::runBenchmarkMode(const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Phase 6 Benchmark Mode ===" << std::endl;
    
    // Parse benchmark parameters
    size_t neurons = 1000;
    size_t steps = 1000;
    
    auto it = args.find("neurons");
    if (it != args.end()) neurons = std::stoul(it->second);
    
    it = args.find("steps");
    if (it != args.end()) steps = std::stoul(it->second);
    
    std::cout << "Running benchmark with " << neurons << " neurons, " << steps << " steps" << std::endl;
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", neurons);
    cfg->set("region_count", 1);
    cfg->set("random_seed", static_cast<int64_t>(42));
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Run benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            std::cout << "Step " << step << "/" << steps << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    double stepsPerSecond = steps / duration;
    double neuronsPerSecond = (neurons * steps) / duration;
    
    std::cout << std::endl;
    std::cout << "=== BENCHMARK RESULTS ===" << std::endl;
    std::cout << "Total steps: " << steps << std::endl;
    std::cout << "Duration: " << duration << "s" << std::endl;
    std::cout << "Steps per second: " << stepsPerSecond << std::endl;
    std::cout << "Neurons processed per second: " << neuronsPerSecond << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return true;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Check for help/version flags
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--help" || arg == "-h") || (arg == "--version" || arg == "-v")) {
            showHelp(argv[0]);
            return 0;
        }
    }
    
    // Parse arguments
    std::map<std::string, std::string> args;
    if (!nlm::cli::parseArguments(argc, argv, args)) {
        std::cerr << "ERROR: Failed to parse arguments" << std::endl;
        return 1;
    }
    
    // Handle help/version
    if (args.find("help") != args.end() || args.find("version") != args.end()) {
        showHelp(argv[0]);
        return 0;
    }
    
    // Extract mode from args
    std::string mode = args["mode"];
    
    // Remove mode from args to avoid conflicts
    std::map<std::string, std::string> filteredArgs;
    for (const auto& pair : args) {
        if (pair.first != "mode") {
            filteredArgs[pair.first] = pair.second;
        }
    }
    
    // Execute the requested mode
    return nlm::cli::handleCommand(mode, filteredArgs) ? 0 : 1;
}