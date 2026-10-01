// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the complete artificial brain with integrated:
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction and cognition (planning, concept formation)
// - Development and plasticity
// - Checkpointing and replay
// - Closed-loop world interaction

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
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <functional>

using namespace nlm;

// Unified experiment configuration structure
struct ExperimentConfig {
    std::string name;
    int phase = 6;  // Default to Phase 6 (Final Integration)
    bool runAllPhases = false;
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    std::string outputFile;
    bool enableVisualization = false;
    bool enableProfiling = false;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool enableReward = true;
    uint64_t regionCount = 1;
    float connectionProbability = 0.15f;
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
};

// Helper function to parse command-line arguments
ExperimentConfig parseArguments(int argc, char** argv) {
    ExperimentConfig config;
    config.name = "default";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Usage:" << std::endl;
            std::cout << "  --phase N           Run specific phase (2, 3, 4, 6)" << std::endl;
            std::cout << "  --neurons N         Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps S           Max simulation steps (default: 2000)" << std::endl;
            std::cout << "  --timestep T        Simulation timestep (default: 0.001)" << std::endl;
            std::cout << "  --seed S            Random seed (default: 42)" << std::endl;
            std::cout << "  --output FILE        Output results to file" << std::endl;
            std::cout << "  --profile           Enable performance profiling" << std::endl;
            std::cout << "  --no-visualization   Disable visualization" << std::endl;
            std::cout << "  --checkpoint FILE    Checkpoint path (default: /tmp/nlm_checkpoint.bin)" << std::endl;
            std::cout << "  --no-development     Disable development system" << std::endl;
            std::cout << "  --no-neuromod        Disable neuromodulation" << std::endl;
            std::cout << "  --no-memory         Disable memory systems" << std::endl;
            std::cout << "  --no-prediction     Disable prediction system" << std::endl;
            std::cout << "  --no-cognition      Disable cognition systems" << std::endl;
            std::cout << "  --phase2           Run Phase 2 experiments (legacy)" << std::endl;
            std::cout << "  --phase3           Run Phase 3 experiments" << std::endl;
            std::cout << "  --phase4           Run Phase 4 experiments" << std::endl;
            std::cout << "  --phase6           Run Phase 6 integration experiment (default)" << std::endl;
            std::cout << "  --all-phases       Run all experiment phases" << std::endl;
            std::cout << "  --list-experiments Show available experiment configurations" << std::endl;
            std::cout << "  --quick-benchmark   Run quick benchmark of all systems" << std::endl;
            std::cout << "  --comprehensive-test Run comprehensive integration test" << std::endl;
            std::cout << "  --memory-test       Test memory integration specifically" << std::endl;
            std::cout << "  --neuromod-test     Test neuromodulation integration" << std::endl;
            std::cout << "  --prediction-test   Test prediction system" << std::endl;
            std::cout << "  --cognition-test    Test cognition systems" << std::endl;
            std::cout << "  --checkpoint-test   Test checkpoint/save-load functionality" << std::endl;
            std::cout << "  --replay-test       Test memory replay functionality" << std::endl;
            std::exit(0);
        }
        
        if (arg.substr(0, 7) == "--phase=") {
            try {
                config.phase = std::stoi(arg.substr(7));
            } catch (...) {
                std::cerr << "Invalid phase value" << std::endl;
                std::exit(1);
            }
        } else if (arg == "--phase2") {
            config.phase = 2;
        } else if (arg == "--phase3") {
            config.phase = 3;
        } else if (arg == "--phase4") {
            config.phase = 4;
        } else if (arg == "--phase6") {
            config.phase = 6;
        } else if (arg == "--all-phases") {
            config.runAllPhases = true;
        } else if (arg == "--quick-benchmark") {
            config.phase = 6;
            config.maxSteps = 500;
            config.neuronCount = 100;
        } else if (arg == "--comprehensive-test") {
            config.phase = 6;
            config.maxSteps = 5000;
            config.neuronCount = 1000;
            config.enableDevelopment = true;
            config.enableNeuromodulation = true;
            config.enableMemory = true;
            config.enablePrediction = true;
            config.enableCognition = true;
        } else if (arg == "--memory-test") {
            config.phase = 6;
            config.maxSteps = 2000;
            config.neuronCount = 200;
        } else if (arg == "--neuromod-test") {
            config.phase = 6;
            config.maxSteps = 1000;
            config.neuronCount = 200;
        } else if (arg == "--prediction-test") {
            config.phase = 6;
            config.maxSteps = 1500;
            config.neuronCount = 300;
        } else if (arg == "--cognition-test") {
            config.phase = 6;
            config.maxSteps = 1500;
            config.neuronCount = 300;
        } else if (arg == "--checkpoint-test") {
            config.phase = 6;
            config.maxSteps = 800;
            config.neuronCount = 150;
        } else if (arg == "--replay-test") {
            config.phase = 6;
            config.maxSteps = 1200;
            config.neuronCount = 200;
        } else if (arg.substr(0, 10) == "--neurons=") {
            try {
                config.neuronCount = std::stoull(arg.substr(10));
            } catch (...) {
                std::cerr << "Invalid neuron count" << std::endl;
                std::exit(1);
            }
        } else if (arg.substr(0, 8) == "--steps=") {
            try {
                config.maxSteps = std::stoull(arg.substr(8));
            } catch (...) {
                std::cerr << "Invalid step count" << std::endl;
                std::exit(1);
            }
        } else if (arg.substr(0, 10) == "--timestep=") {
            try {
                config.timestep = std::stod(arg.substr(10));
            } catch (...) {
                std::cerr << "Invalid timestep" << std::endl;
                std::exit(1);
            }
        } else if (arg.substr(0, 7) == "--seed=") {
            try {
                config.randomSeed = std::stoull(arg.substr(7));
            } catch (...) {
                std::cerr << "Invalid seed" << std::endl;
                std::exit(1);
            }
        } else if (arg.substr(0, 9) == "--output=") {
            config.outputFile = arg.substr(9);
        } else if (arg == "--profile") {
            config.enableProfiling = true;
        } else if (arg == "--no-visualization") {
            config.enableVisualization = false;
        } else if (arg.substr(0, 13) == "--checkpoint=") {
            config.checkpointPath = arg.substr(13);
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--no-neuromod") {
            config.enableNeuromodulation = false;
        } else if (arg == "--no-memory") {
            config.enableMemory = false;
        } else if (arg == "--no-prediction") {
            config.enablePrediction = false;
        } else if (arg == "--no-cognition") {
            config.enableCognition = false;
        }
    }
    
    return config;
}

// Helper function to print configuration summary
void printConfigSummary(const ExperimentConfig& config) {
    std::cout << "=== NLM Experiment Configuration ===" << std::endl;
    std::cout << "Experiment: " << config.name << std::endl;
    std::cout << "Phase: " << config.phase << std::endl;
    if (config.runAllPhases) {
        std::cout << "Mode: ALL_PHASES (will run Phases 2, 3, 4, 6)" << std::endl;
    }
    std::cout << "Neurons: " << config.neuronCount << std::endl;
    std::cout << "Max Steps: " << config.maxSteps << std::endl;
    std::cout << "Timestep: " << config.timestep << "s" << std::endl;
    std::cout << "Random Seed: " << config.randomSeed << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "Neuromodulation: " << (config.enableNeuromodulation ? "Yes" : "No") << std::endl;
    std::cout << "Memory: " << (config.enableMemory ? "Yes" : "No") << std::endl;
    std::cout << "Prediction: " << (config.enablePrediction ? "Yes" : "No") << std::endl;
    std::cout << "Cognition: " << (config.enableCognition ? "Yes" : "No") << std::endl;
    if (!config.outputFile.empty()) {
        std::cout << "Output File: " << config.outputFile << std::endl;
    }
    std::cout << "Checkpoint Path: " << config.checkpointPath << std::endl;
    if (config.enableProfiling) {
        std::cout << "Performance Profiling: Enabled" << std::endl;
    }
    std::cout << "=================================" << std::endl;
}

// Helper function to print experiment banner
void printExperimentBanner(int phase) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase " ; std::cout << phase; std::cout << R"(: Experimental Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Performance profiler
class PerformanceProfiler {
public:
    struct Metrics {
        double totalTime = 0.0;
        double cpuTime = 0.0;
        double memoryUsage = 0.0;
        uint64_t spikeCount = 0;
        uint64_t neuronCount = 0;
        float firingRate = 0.0f;
        std::map<std::string, double> customMetrics;
    };
    
    void startTiming() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stopTiming() {
        auto endTime = std::chrono::high_resolution_clock::now();
        totalTime += std::chrono::duration<double>(endTime - startTime).count();
    }
    
    double getTotalTime() const { return totalTime; }
    
    Metrics getCurrentMetrics() const {
        Metrics m;
        m.totalTime = totalTime;
        // CPU time would need system-specific calls
        m.spikeCount = spikeCount;
        m.neuronCount = neuronCount;
        m.firingRate = firingRate;
        m.customMetrics = customMetrics;
        return m;
    }
    
    void updateSpikeCount(uint64_t count) { spikeCount = count; }
    void updateNeuronCount(uint64_t count) { neuronCount = count; }
    void updateFiringRate(float rate) { firingRate = rate; }
    void addCustomMetric(const std::string& name, double value) {
        customMetrics[name] = value;
    }
    
    void printMetrics(const std::string& title) const {
        std::cout << "=== " << title << " ===" << std::endl;
        std::cout << "Total Time: " << totalTime << "s" << std::endl;
        std::cout << "Spike Count: " << spikeCount << std::endl;
        std::cout << "Neuron Count: " << neuronCount << std::endl;
        std::cout << "Firing Rate: " << firingRate << " Hz" << std::endl;
        for (const auto& pair : customMetrics) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
        std::cout << "=====================" << std::endl;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    double totalTime = 0.0;
    uint64_t spikeCount = 0;
    uint64_t neuronCount = 0;
    float firingRate = 0.0f;
    std::map<std::string, double> customMetrics;
};

// Unified experiment runner
class UnifiedExperimentRunner {
public:
    UnifiedExperimentRunner() {
        profiler = std::make_shared<PerformanceProfiler>();
    }
    
    bool runPhase2Experiment(const ExperimentConfig& config) {
        printExperimentBanner(2);
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Implementing:" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
        std::cout << "  - Event-driven spike propagation with delays" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
        
        // Initialize components
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", config.neuronCount);
        configPtr->set("region_count", config.regionCount);
        configPtr->set("connection_probability", config.connectionProbability);
        configPtr->set("stdp_ltp_weight", config.stdpLtpWeight);
        configPtr->set("stdp_ltd_weight", config.stdpLtdWeight);
        configPtr->set("stdp_tau", config.stdpTau);
        configPtr->set("synaptogenesis_rate", config.synaptogenesisRate);
        configPtr->set("pruning_rate", config.pruningRate);
        configPtr->set("simulation_timestep", config.timestep);
        configPtr->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::Default);
        
        SimulationClock clock(config.timestep);
        auto brain = std::make_shared<Brain>(configPtr);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        profiler->startTiming();
        profiler->updateNeuronCount(config.neuronCount);
        
        // Run basic connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cerr << "No regions found!" << std::endl;
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cerr << "No neurons found!" << std::endl;
            return false;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < config.maxSteps; ++step) {
            brain->step(step, step * config.timestep);
        }
        
        profiler->stopTiming();
        profiler->updateSpikeCount(brain->getTotalSpikeCount() - initialSpikes);
        profiler->updateFiringRate(brain->getAverageFiringRate());
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "  Neuron " << i << " V=" << neurons[i]->getMembranePotential() 
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
        
        brain->logStatus();
        profiler->printMetrics("Phase 2 Results");
        return true;
    }
    
    bool runPhase6Experiment(const ExperimentConfig& config) {
        printExperimentBanner(6);
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "Testing integrated brain systems..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create Phase 6 integration experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return false;
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
        
        // Run full integration experiment
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.neuronCount;
        phase6Config.maxSteps = config.maxSteps;
        phase6Config.enableCheckpointing = true;
        phase6Config.enableReplay = true;
        phase6Config.enableDevelopment = config.enableDevelopment;
        
        auto result = experiment.run(phase6Config);
        
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
        
        profiler->addCustomMetric("total_reward", result.totalReward);
        profiler->addCustomMetric("avg_firing_rate", result.avgFiringRate);
        profiler->addCustomMetric("memory_episodes", result.memoryEpisodesStored);
        profiler->addCustomMetric("dopamine_level", result.dopamineLevel);
        profiler->printMetrics("Phase 6 Integration Results");
        
        return true;
    }
    
    bool runAllPhases(const ExperimentConfig& config) {
        std::cout << "Running all experiment phases..." << std::endl << std::endl;
        
        bool success = true;
        
        // Phase 2 (legacy)
        if (!runPhase2Experiment(config)) {
            std::cerr << "Phase 2 failed!" << std::endl;
            success = false;
        }
        
        // Phase 6 (integration)
        if (!runPhase6Experiment(config)) {
            std::cerr << "Phase 6 failed!" << std::endl;
            success = false;
        }
        
        return success;
    }
    
    std::shared_ptr<PerformanceProfiler> getProfiler() const { return profiler; }
    
private:
    std::shared_ptr<PerformanceProfiler> profiler;
};

int main(int argc, char** argv) {
    // Parse command-line arguments
    ExperimentConfig config = parseArguments(argc, argv);
    
    // Print configuration summary
    printConfigSummary(config);
    
    // Create unified experiment runner
    UnifiedExperimentRunner runner;
    
    bool success = false;
    
    if (config.runAllPhases) {
        success = runner.runAllPhases(config);
    } else {
        switch (config.phase) {
            case 2:
                success = runner.runPhase2Experiment(config);
                break;
            case 3:
                std::cout << "Phase 3 experiments are in development" << std::endl;
                success = false;
                break;
            case 4:
                std::cout << "Phase 4 experiments are in development" << std::endl;
                success = false;
                break;
            case 6:
                success = runner.runPhase6Experiment(config);
                break;
            default:
                std::cerr << "Unknown phase: " << config.phase << std::endl;
                success = false;
        }
    }
    
    // Save results to output file if specified
    if (success && !config.outputFile.empty()) {
        std::cout << "Saving results to " << config.outputFile << std::endl;
        std::ofstream file(config.outputFile);
        if (file.is_open()) {
            file << "NLM Experiment Results" << std::endl;
            file << "=======================" << std::endl;
            file << "Experiment: " << config.name << std::endl;
            file << "Phase: " << config.phase << std::endl;
            file << "Neurons: " << config.neuronCount << std::endl;
            file << "Max Steps: " << config.maxSteps << std::endl;
            file << "Total Time: " << runner.getProfiler()->getTotalTime() << "s" << std::endl;
            file << "Spike Count: " << runner.getProfiler()->getCurrentMetrics().spikeCount << std::endl;
            file << "Firing Rate: " << runner.getProfiler()->getCurrentMetrics().firingRate << " Hz" << std::endl;
            file.close();
        }
    }
    
    if (success) {
        std::cout << std::endl;
        std::cout << "=== Experiment Completed Successfully ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "=== Experiment Failed ===" << std::endl;
        return 1;
    }
}