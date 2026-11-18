#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <sstream>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all systems into a coherent loop.    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << "Available modes:" << std::endl;
    std::cout << "  demo       Run complete Phase 6 integration demo" << std::endl;
    std::cout << "  benchmark   Run benchmark test" << std::endl;
    std::cout << "  phase2      Run Phase 2 neural computation demo" << std::endl;
    std::cout << "  phase3      Run Phase 3 world interaction demo" << std::endl;
    std::cout << "  phase4      Run Phase 4 cognition demo" << std::endl;
    std::cout << "  phase5      Run Phase 5 extensibility demo" << std::endl;
    std::cout << "  phase6      Run Phase 6 full integration demo" << std::endl;
    std::cout << "  phase6_text Run Phase 6 text-as-a-sense demo" << std::endl;
    std::cout << "  test        Run unit tests" << std::endl;
    std::cout << "  --version    Show version information" << std::endl;
    std::cout << "  --help       Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Configuration options:" << std::endl;
    stdtype_t set("  nlm --demo --neurons 500 --steps 1000" << std::endl;
    std::cout << "  --neurons N      Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps N        Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --timestep t      Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --seed S         Random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint      Enable checkpointing" << std::endl;
    std::cout << "  --replay          Enable memory replay" << std::endl;
    std::cout << "  --develop         Enable brain development" << std::endl;
    std::cout << "  --reward          Enable reward modulation" << std::endl;
    std::cout << "  --curiosity       Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --novelty         Enable novelty detection" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Benchmark options:" << std::endl;
    std::cout << "  --benchmark-mode MODE  Benchmark mode: performance|memory|plasticity" << std::endl;
    std::cout << "  --benchmark-duration T  Benchmark duration in seconds (default: 60)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Display options:" << std::endl;
    std::cout << "  --verbose         Verbose output" << std::endl;
    std::cout << "  --quiet           Quiet output (errors only)" << std::endl;
    std::cout << "  --profile         Enable performance profiling" << std::endl;
    std::cout << "" << std::endl;
}

void printVersion() {
    std::cout << "NLM v0.1.0 (Phase 6 Final Integration)" << std::endl;
    std::cout << "Built with C++20, pybind11 for Python bindings" << std::endl;
    std::cout << "Copyright (c) 2026 NLM Research Project" << std::endl;
    std::cout << "" << std::endl;
}

// Performance profiling utilities
struct Profiler {
    std::unordered_map<std::string, double> timestamps;
    std::unordered_map<std::string, size_t> counters;
    
    void start(const std::string& name) {
        timestamps[name] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void stop(const std::string& name) {
        double end = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        double duration = end - timestamps[name];
        std::cout << "  [PROF] " << name << ": " << duration << "s" << std::endl;
    }
    
    void increment(const std::string& name) {
        counters[name]++;
    }
    
    void report() {
        std::cout << "=== Performance Profile ===" << std::endl;
        for (const auto& [name, duration] : timestamps) {
            std::cout << "  " << name << ": " << duration << "s (avg)" << std::endl;
        }
        std::cout << "  Counters:" << std::endl;
        for (const auto& [name, count] : counters) {
            std::cout << "    " << name << ": " << count << std::endl;
        }
    }
};

// Configuration parser
class CLIConfig {
public:
    std::string mode;
    size_t neurons = 500;
    size_t steps = 1000;
    double timestep = 0.001;
    uint64_t seed = 42;
    bool enableCheckpoint = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    bool enableReward = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool verbose = false;
    bool quiet = false;
    bool profile = false;
    std::string benchmarkMode;
    double benchmarkDuration = 60.0;
    std::string configFile;
    
    bool parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            else if (arg == "--version") {
                printVersion();
                return false;
            }
            else if (arg == "--verbose") {
                verbose = true;
            }
            else if (arg == "--quiet") {
                quiet = true;
            }
            else if (arg == "--profile") {
                profile = true;
            }
            else if (arg == "--benchmark") {
                mode = "benchmark";
            }
            else if (arg == "--demo") {
                mode = "demo";
            }
            else if (arg == "--phase2") {
                mode = "phase2";
            }
            else if (arg == "--phase3") {
                mode = "phase3";
            }
            else if (arg == "--phase4") {
                mode = "phase4";
            }
            else if (arg == "--phase5") {
                mode = "phase5";
            }
            else if (arg == "--phase6") {
                mode = "phase6";
            }
            else if (arg == "--phase6_text") {
                mode = "phase6_text";
            }
            else if (arg == "--test") {
                mode = "test";
            }
            else if (arg == "--neurons" && i + 1 < argc) {
                neurons = std::stoull(argv[++i]);
            }
            else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoull(argv[++i]);
            }
            else if (arg == "--timestep" && i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
            else if (arg == "--seed" && i + 1 < argc) {
                seed = std::stoull(argv[++i]);
            }
            else if (arg == "--checkpoint") {
                enableCheckpoint = true;
            }
            else if (arg == "--replay") {
                enableReplay = true;
            }
            else if (arg == "--develop") {
                enableDevelopment = true;
            }
            else if (arg == "--reward") {
                enableReward = true;
            }
            else if (arg == "--curiosity") {
                enableCuriosity = true;
            }
            else if (arg == "--novelty") {
                enableNovelty = true;
            }
            else if (arg == "--benchmark-mode" && i + 1 < argc) {
                benchmarkMode = argv[++i];
            }
            else if (arg == "--benchmark-duration" && i + 1 < argc) {
                benchmarkDuration = std::stod(argv[++i]);
            }
            else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            }
            else {
                if (mode.empty()) mode = "demo";
            }
        }
        
        // Default mode if not specified
        if (mode.empty()) mode = "demo";
        
        return true;
    }
    
    void printConfig() {
        if (quiet) return;
        
        std::cout << "=== NLM Configuration ===" << std::endl;
        std::cout << "Mode: " << mode << std::endl;
        std::cout << "Neurons: " << neurons << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        std::cout << "Timestep: " << timestep << "s" << std::endl;
        std::cout << "Random seed: " << seed << std::endl;
        std::cout << "Checkpoint: " << (enableCheckpoint ? "enabled" : "disabled") << std::endl;
        std::cout << "Replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "Development: " << (enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << "Reward modulation: " << (enableReward ? "enabled" : "disabled") << std::endl;
        std::cout << "Curiosity: " << (enableCuriosity ? "enabled" : "disabled") << std::endl;
        std::cout << "Novelty detection: " << (enableNovelty ? "enabled" : "disabled") << std::endl;
        if (!configFile.empty()) {
            std::cout << "Config file: " << configFile << std::endl;
        }
        std::cout << "" << std::endl;
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    CLIConfig config;
    if (!config.parseArgs(argc, argv)) {
        return 0;  // Help or version requested
    }
    
    // Apply configuration
    config.printConfig();
    
    Profiler profiler;
    if (config.profile) {
        std::cout << "Starting performance profiler..." << std::endl;
        profiler.start("total");
    }
    
    int result = 0;
    
    try {
        if (config.mode == "demo") {
            std::cout << "=== NLM Phase 6 Unified Demo ===" << std::endl;
            
            Phase6IntegratedExperiment experiment;
            
            // Quick integration verification
            std::cout << "Running integration verification..." << std::endl;
            bool integrationOK = experiment.verifyIntegration();
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                result = 1;
            }
            
            // Test individual systems
            if (config.verbose && integrationOK) {
                std::cout << "Testing memory integration..." << std::endl;
                experiment.testMemoryIntegration();
                
                std::cout << "Testing neuromodulation integration..." << std::endl;
                experiment.testNeuromodulationIntegration();
                
                std::cout << "Testing checkpointing..." << std::endl;
                experiment.testCheckpointing();
                
                std::cout << "Testing replay..." << std::endl;
                experiment.testReplay();
            }
            
            if (integrationOK && result == 0) {
                Phase6Config phase6Config;
                phase6Config.neuronCount = static_cast<uint32_t>(config.neurons);
                phase6Config.maxSteps = static_cast<uint64_t>(config.steps);
                phase6Config.enableCheckpointing = config.enableCheckpoint;
                phase6Config.enableReplay = config.enableReplay;
                phase6Config.enableDevelopment = config.enableDevelopment;
                
                std::cout << "Running full integration experiment..." << std::endl;
                auto phaseStart = std::chrono::high_resolution_clock::now();
                auto experimentResult = experiment.run(phase6Config);
                auto phaseEnd = std::chrono::high_resolution_clock::now();
                
                double duration = std::chrono::duration<double>(phaseEnd - phaseStart).count();
                
                std::cout << "=== FINAL RESULTS ===" << std::endl;
                std::cout << "Total reward: " << experimentResult.totalReward << std::endl;
                std::cout << "Avg firing rate: " << experimentResult.avgFiringRate << std::endl;
                std::cout << "Episodes stored: " << experimentResult.memoryEpisodesStored << std::endl;
                std::cout << "Dopamine level: " << experimentResult.dopamineLevel << std::endl;
                std::cout << "Wall clock time: " << duration << "s" << std::endl;
                std::cout << "" << std::endl;
                
                std::cout << "=== INTEGRATION STATUS ===" << std::endl;
                std::cout << "Working Memory: " << (experimentResult.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                std::cout << "Episodic Memory: " << (experimentResult.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                std::cout << "Neuromodulation: " << (experimentResult.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                std::cout << "Prediction: " << (experimentResult.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                std::cout << "Development: " << (experimentResult.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                std::cout << "Checkpointing: " << (experimentResult.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
                std::cout << "" << std::endl;
                
                if (!experimentResult.memoryWorkingMemoryIntegrated || 
                    !experimentResult.memoryEpisodicMemoryIntegrated ||
                    !experimentResult.neuromodulationIntegrated ||
                    !experimentResult.predictionIntegrated ||
                    !experimentResult.developmentIntegrated) {
                    result = 1;
                }
            }
        }
        
        else if (config.mode == "benchmark") {
            std::cout << "=== NLM Benchmark ===" << std::endl;
            std::cout << "Benchmark mode: " << config.benchmarkMode << std::endl;
            std::cout << "Duration: " << config.benchmarkDuration << "s" << std::endl;
            
            // Create configuration
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", static_cast<int64_t>(config.neurons));
            cfg->set("simulation_timestep", config.timestep);
            
            // Create brain
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "ERROR: Failed to initialize brain for benchmark!" << std::endl;
                result = 1;
            }
            else {
                // Run benchmark
                auto start = std::chrono::high_resolution_clock::now();
                size_t totalSteps = static_cast<size_t>(config.benchmarkDuration / config.timestep);
                
                for (size_t step = 0; step < totalSteps; ++step) {
                    brain->step(step, step * config.timestep);
                    
                    if (config.profile && step % 100 == 0) {
                        std::cout << "  Step " << step << "/" << totalSteps << " | "
                                 << "Spikes: " << brain->getTotalSpikeCount() << " | "
                                 << "Firing: " << brain->getFiringNeuronCount() << " | "
                                 << "Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                    }
                }
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(end - start).count();
                
                std::cout << "=== Benchmark Results ===" << std::endl;
                std::cout << "Duration: " << duration << "s" << std::endl;
                std::cout << "Total steps: " << totalSteps << std::endl;
                std::cout << "Steps per second: " << totalSteps / duration << std::endl;
                std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
                std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
                
                if (config.profile) {
                    profiler.report();
                }
            }
        }
        
        else {
            std::cerr << "ERROR: Unknown mode: " << config.mode << std::endl;
            printHelp();
            result = 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        result = 1;
    }
    catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        result = 1;
    }
    
    if (config.profile) {
        profiler.stop("total");
        profiler.report();
    }
    
    if (quiet) {
        std::cout << "NLM completed with result: " << result << std::endl;
    }
    
    return result;
}
