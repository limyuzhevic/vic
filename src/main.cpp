// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced CLI version with advanced capabilities
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Enhanced CLI with multiple modes, batch processing, monitoring, benchmarking
//
// Usage:
//   nlm --mode simple                    # Run simple demonstrations
//   nlm --mode batch --config file.cfg    # Run multiple experiments
//   nlm --mode benchmark --trials N       # Run performance benchmarks
//   nlm --mode interactive                # Start interactive console
//   nlm --mode help                       # Show help information

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
#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <fstream>

using namespace nlm;

// Command line parsing namespace
namespace cli {
    
    enum class CommandMode {
        Unknown,
        Simple,
        Batch,
        Benchmark,
        Interactive,
        Help,
        Version
    };
    
    struct CommandOptions {
        CommandMode mode = CommandMode::Unknown;
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "experiment_results.json";
        std::string metricsFile = "performance_metrics.json";
        uint64_t randomSeed = 42;
        size_t maxSteps = 1000;
        size_t trials = 1;
        bool verbose = false;
        bool realTime = false;
        bool benchmarkPerformance = false;
        std::vector<std::string> batchConfigs;
        std::string experimentName = "default_experiment";
        std::unordered_map<std::string, std::string> paramSweeps;
    };
    
    CommandOptions parseArguments(int argc, char** argv) {
        CommandOptions options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--mode" && i + 1 < argc) {
                std::string mode = argv[++i];
                if (mode == "simple") options.mode = CommandMode::Simple;
                else if (mode == "batch") options.mode = CommandMode::Batch;
                else if (mode == "benchmark") options.mode = CommandMode::Benchmark;
                else if (mode == "interactive") options.mode = CommandMode::Interactive;
                else if (mode == "help") options.mode = CommandMode::Help;
                else if (mode == "version") options.mode = CommandMode::Version;
                else options.mode = CommandMode::Unknown;
            }
            else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            }
            else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            }
            else if (arg == "--metrics" && i + 1 < argc) {
                options.metricsFile = argv[++i];
            }
            else if (arg == "--seed" && i + 1 < argc) {
                options.randomSeed = std::stoull(argv[++i]);
            }
            else if (arg == "--steps" && i + 1 < argc) {
                options.maxSteps = std::stoull(argv[++i]);
            }
            else if (arg == "--trials" && i + 1 < argc) {
                options.trials = std::stoull(argv[++i]);
            }
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--real-time" || arg == "-r") {
                options.realTime = true;
            }
            else if (arg == "--benchmark" || arg == "--perf") {
                options.benchmarkPerformance = true;
            }
            else if (arg == "--param" && i + 1 < argc) {
                std::string param = argv[++i];
                size_t pos = param.find('=');
                if (pos != std::string::npos) {
                    std::string key = param.substr(0, pos);
                    std::string value = param.substr(pos + 1);
                    options.paramSweeps[key] = value;
                }
            }
            else if (arg == "--batch-config" && i + 1 < argc) {
                options.batchConfigs.push_back(argv[++i]);
            }
            else if (arg == "--help" || arg == "-h") {
                options.mode = CommandMode::Help;
            }
            else if (arg == "--version") {
                options.mode = CommandMode::Version;
            }
            else if (arg.substr(0, 2) == "--") {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    options.paramSweeps[key] = value;
                }
            }
            else if (arg[0] == '-') {
                // Handle -key value format
                std::string key = arg.substr(1);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    options.paramSweeps[key] = argv[++i];
                }
            }
        }
        
        return options;
    }
    
    void printHelp() {
        std::cout << R"(
╔════════════════════════════════════════════════════════════════╗
║                    NLM Command Line Interface                  ║
╠════════════════════════════════════════════════════════════════╣
║                                                              ║
║  Mode Options:                                               ║
║    --mode simple              Run simple demonstrations       ║
║    --mode batch               Run batch experiments            ║
║    --mode benchmark           Run performance benchmarks       ║
║    --mode interactive         Start interactive console        ║
║    --mode help                Show this help message           ║
║    --mode version             Show version information         ║
║                                                              ║
║  Configuration:                                              ║
║    --config file.cfg          Configuration file (default:    ║
║                              configs/default.cfg)            ║
║    --batch-config cfg1.cfg    Add batch configuration file      ║
║    --output file.json         Output results file              ║
║    --metrics file.json         Performance metrics file        ║
║                                                              ║
║  Performance:                                                 ║
║    --seed 42                  Random seed (default: 42)         ║
║    --steps 1000               Max simulation steps            ║
║    --trials 1                 Number of trials (for batch mode) ║
║    --verbose, -v              Enable verbose output            ║
║    --real-time, -r            Enable real-time monitoring      ║
║    --benchmark, --perf        Enable performance benchmarking   ║
║                                                              ║
║  Parameter Sweeps:                                           ║
║    --param key=value          Parameter to sweep in batch mode  ║
║    -key value                 Shortcut for --param             ║
║                                                              ║
║  Examples:                                                    ║
║    nlm --mode simple                                        ║
║    nlm --mode batch --config experiments/batch.cfg          ║
║    nlm --mode benchmark --trials 10 --seed 123              ║
║    nlm --mode interactive --real-time                       ║
║                                                              ║
╚════════════════════════════════════════════════════════════════╝
)" << std::endl;
    }
    
    void printVersion() {
        std::cout << R"(
NLM (熙然) - Neural Learning Machine
Version 2.0.0 (Phase 2: Real Neural Computation)

Features:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Batch processing with ExperimentRunner
- Real-time monitoring and metrics collection
- Performance benchmarking
- Advanced configuration management
)" << std::endl;
    }
}

// Real-time monitoring class
class RealTimeMonitor {
public:
    RealTimeMonitor() : running_(false), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void start() {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
        NLM_LOG_INFO("Real-time monitoring started");
    }
    
    void stop() {
        running_ = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime_);
        NLM_LOG_INFO("Real-time monitoring stopped. Total duration: " + std::to_string(duration.count()) + "s");
    }
    
    void updateMetrics(const std::shared_ptr<Brain>& brain, size_t step) {
        if (!running_) return;
        
        static size_t lastStep = 0;
        static auto lastTime = std::chrono::high_resolution_clock::now();
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
        
        // Calculate metrics every 100 steps or every second
        if (step - lastStep >= 100 || elapsed.count() >= 1000) {
            float fps = 1000.0f / elapsed.count();
            float neuronActivity = static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount() * 100.0f;
            
            NLM_LOG_INFO(std::string("Step ") + std::to_string(step) + 
                        " | FPS: " + std::to_string(fps) +
                        " | Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "/" + 
                        std::to_string(brain->getTotalNeuronCount()) +
                        " | Activity: " + std::to_string(neuronActivity) + "%");
            
            lastStep = step;
            lastTime = currentTime;
        }
    }
    
private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Performance benchmark class
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string testName;
        double duration;  // seconds
        size_t steps;     // steps completed
        double fps;       // frames per second
        double memoryUsage; // MB (placeholder)
        std::unordered_map<std::string, double> metrics;
    };
    
    std::vector<BenchmarkResult> runBenchmarks(const std::shared_ptr<Brain>& brain, 
                                               const std::shared_ptr<Config>& config,
                                               size_t trials) {
        std::vector<BenchmarkResult> results;
        
        NLM_LOG_INFO("Starting performance benchmarks...");
        NLM_LOG_INFO("Running " + std::to_string(trials) + " trials");
        
        for (size_t trial = 0; trial < trials; ++trial) {
            NLM_LOG_INFO("Trial " + std::to_string(trial + 1) + "/" + std::to_string(trials));
            
            // Reset brain for each trial
            brain->reset();
            brain->initialize();
            
            // Measure performance
            auto startTime = std::chrono::high_resolution_clock::now();
            
            size_t stepsCompleted = 0;
            for (size_t step = 0; step < config->getOr<size_t>("max_benchmark_steps", 1000); ++step) {
                brain->step(step, config->getOr<double>("simulation_timestep", 0.001));
                stepsCompleted++;
                
                // Break condition
                if (brain->getFiringNeuronCount() == 0 && step > 100) {
                    break;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            double durationSec = duration.count() / 1000.0;
            double fps = (durationSec > 0) ? stepsCompleted / durationSec : 0.0;
            
            BenchmarkResult result;
            result.testName = "brain_step_benchmark";
            result.duration = durationSec;
            result.steps = stepsCompleted;
            result.fps = fps;
            result.memoryUsage = 0.0; // Placeholder - would need actual memory measurement
            result.metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            result.metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            result.metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            result.metrics["firing_rate"] = static_cast<double>(brain->getFiringNeuronCount()) / 
                                          std::max(1.0, static_cast<double>(brain->getTotalNeuronCount()));
            
            results.push_back(result);
            
            // Log individual trial result
            NLM_LOG_INFO("Trial " + std::to_string(trial + 1) + 
                        ": Duration=" + std::to_string(durationSec) + "s, " +
                        "Steps=" + std::to_string(stepsCompleted) + ", " +
                        "FPS=" + std::to_string(fps));
        }
        
        // Calculate averages
        NLM_LOG_INFO("Benchmarks completed. Calculating averages...");
        double totalDuration = 0.0, totalFps = 0.0;
        size_t totalSteps = 0;
        
        for (const auto& result : results) {
            totalDuration += result.duration;
            totalFps += result.fps;
            totalSteps += result.steps;
        }
        
        if (!results.empty()) {
            double avgDuration = totalDuration / results.size();
            double avgFps = totalFps / results.size();
            
            NLM_LOG_INFO("Benchmark Summary (Average):");
            NLM_LOG_INFO("  Duration: " + std::to_string(avgDuration) + "s");
            NLM_LOG_INFO("  Steps/sec: " + std::to_string(avgFps));
            NLM_LOG_INFO("  Total steps: " + std::to_string(totalSteps));
            
            // Save results
            saveBenchmarkResults(results, config->getOr<std::string>("benchmark_output", "benchmark_results.json"));
        }
        
        return results;
    }
    
private:
    void saveBenchmarkResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) {
        // Simple JSON serialization (placeholder for full implementation)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save benchmark results to: " + filepath);
            return;
        }
        
        file << "{\n";
        file << "  \"benchmarks\": [\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << result.testName << "\",\n";
            file << "      \"duration\": " << result.duration << ",\n";
            file << "      \"steps\": " << result.steps << ",\n";
            file << "      \"fps\": " << result.fps << ",\n";
            file << "      \"memory_mb\": " << result.memoryUsage << "\n";
            if (i < results.size() - 1) file << "    },\n";
            else file << "    }\n";
        }
        file << "  ],\n";
        file << "  \"averages\": {\n";
        double totalDuration = 0.0, totalFps = 0.0;
        size_t totalSteps = 0;
        for (const auto& result : results) {
            totalDuration += result.duration;
            totalFps += result.fps;
            totalSteps += result.steps;
        }
        size_t count = results.size();
        if (count > 0) {
            file << "    \"avg_duration\": " << (totalDuration / count) << ",\n";
            file << "    \"avg_fps\": " << (totalFps / count) << ",\n";
            file << "    \"total_steps\": " << totalSteps << "\n";
        }
        file << "  }\n";
        file << "}\n";
        file.close();
        NLM_LOG_INFO("Benchmark results saved to: " + filepath);
    }
};

// Batch experiment runner
class BatchExperimentRunner {
public:
    BatchExperimentRunner(std::shared_ptr<Config> config) : config_(config) {}
    
    std::vector<std::shared_ptr<Experiment>> runBatchExperiments(
        const std::vector<std::string>& configFiles,
        const std::string& experimentName,
        RealTimeMonitor& monitor,
        size_t trials) {
        
        NLM_LOG_INFO("Starting batch experiments: " + experimentName);
        NLM_LOG_INFO("Processing " + std::to_string(configFiles.size()) + " configuration files");
        
        std::vector<std::shared_ptr<Experiment>> allResults;
        
        for (size_t trial = 0; trial < trials; ++trial) {
            NLM_LOG_INFO("Batch trial " + std::to_string(trial + 1) + "/" + std::to_string(trials));
            
            // Create fresh brain for each trial
            auto brain = std::make_shared<Brain>(config_);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for trial " + std::to_string(trial + 1));
                continue;
            }
            
            // Load experiment configurations
            std::vector<std::shared_ptr<Config>> trialConfigs;
            for (const auto& configFile : configFiles) {
                auto trialConfig = std::make_shared<Config>(*config_);
                if (trialConfig->loadFromFile(configFile)) {
                    NLM_LOG_INFO("Loaded configuration from: " + configFile);
                    trialConfigs.push_back(trialConfig);
                } else {
                    NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
                }
            }
            
            if (trialConfigs.empty()) {
                NLM_LOG_ERROR("No valid configurations for trial " + std::to_string(trial + 1));
                continue;
            }
            
            // Run experiments for each configuration
            for (size_t i = 0; i < trialConfigs.size(); ++i) {
                auto exp = std::make_shared<Experiment>(experimentName + "_config_" + std::to_string(i));
                
                // Apply parameter sweeps
                for (const auto& param : paramSweeps_) {
                    std::string key = param.first;
                    std::string value = param.second;
                    
                    // Simple parameter application
                    if (key == "neuron_count") {
                        trialConfigs[i]->set(key, std::stoull(value), ConfigSource::Runtime);
                    } else if (key == "connection_probability") {
                        trialConfigs[i]->set(key, std::stod(value), ConfigSource::Runtime);
                    }
                    // Add more parameter types as needed
                }
                
                // Run the experiment
                runSingleExperiment(brain, trialConfigs[i], exp, monitor);
                allResults.push_back(exp);
                
                // Save individual experiment results
                saveExperimentResults(exp, "experiment_" + std::to_string(trial + 1) + 
                                    "_config_" + std::to_string(i) + ".json");
            }
        }
        
        // Save batch results
        saveBatchResults(allResults, config_->getOr<std::string>("batch_output", "batch_results.json"));
        
        return allResults;
    }
    
private:
    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, std::string> paramSweeps_;
    
    void addParameterSweep(const std::string& key, const std::string& value) {
        paramSweeps_[key] = value;
    }
    
    void runSingleExperiment(std::shared_ptr<Brain> brain, 
                            std::shared_ptr<Config> expConfig,
                            std::shared_ptr<Experiment> experiment,
                            RealTimeMonitor& monitor) {
        // Override brain config with experiment-specific config
        brain->setConfig(expConfig);
        
        // Record start state
        experiment->recordStep(0, brain->getTotalNeuronCount(), 
                              brain->getTotalSynapseCount(),
                              brain->getFiringNeuronCount(),
                              brain->getAverageFiringRate());
        
        // Run simulation
        size_t maxSteps = expConfig->getOr<size_t>("max_steps", 1000);
        for (size_t step = 0; step < maxSteps; ++step) {
            brain->step(step, expConfig->getOr<double>("simulation_timestep", 0.001));
            
            // Record metrics
            experiment->recordStep(step + 1, brain->getTotalNeuronCount(),
                                  brain->getTotalSynapseCount(),
                                  brain->getFiringNeuronCount(),
                                  brain->getAverageFiringRate());
            
            // Real-time monitoring
            monitor.updateMetrics(brain, step + 1);
            
            // Check termination conditions
            if (brain->getFiringNeuronCount() == 0 && step > 100) {
                NLM_LOG_INFO("No activity detected, terminating experiment early at step " + 
                            std::to_string(step + 1));
                break;
            }
        }
        
        // Record final metrics
        experiment->recordMetric("final_neurons", brain->getTotalNeuronCount());
        experiment->recordMetric("final_synapses", brain->getTotalSynapseCount());
        experiment->recordMetric("final_spikes", brain->getTotalSpikeCount());
        experiment->recordMetric("final_firing_rate", brain->getFiringNeuronCount() / 
                                 std::max(1.0, static_cast<double>(brain->getTotalNeuronCount())));
    }
    
    void saveExperimentResults(std::shared_ptr<Experiment> experiment, const std::string& filepath) {
        // Simple serialization (placeholder for full implementation)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save experiment results to: " + filepath);
            return;
        }
        
        file << "{\n";
        file << "  \"name\": \"" << experiment->getName() << "\",\n";
        file << "  \"seed\": " << experiment->getSeed() << ",\n";
        file << "  \"steps\": [\n";
        
        const auto& stepData = experiment->getStepData();
        for (size_t i = 0; i < stepData.size(); ++i) {
            file << "    {\n";
            file << "      \"step\": " << stepData[i].step << ",\n";
            file << "      \"neurons\": " << stepData[i].neuronCount << ",\n";
            file << "      \"synapses\": " << stepData[i].synapseCount << ",\n";
            file << "      \"firing\": " << stepData[i].firingCount << ",\n";
            file << "      \"avg_firing\": " << stepData[i].avgFiringRate << "\n";
            if (i < stepData.size() - 1) file << "    },\n";
            else file << "    }\n";
        }
        file << "  ],\n";
        
        file << "  \"metrics\": {\n";
        const auto& metrics = experiment->getMetrics();
        for (size_t i = 0; i < metrics.size(); ++i) {
            file << "    \"" << metrics[i].first << "\": " << metrics[i].second;
            if (i < metrics.size() - 1) file << ",\n";
            else file << "\n";
        }
        file << "  }\n";
        file << "}\n";
        file.close();
        NLM_LOG_INFO("Experiment results saved to: " + filepath);
    }
    
    void saveBatchResults(const std::vector<std::shared_ptr<Experiment>>& experiments, const std::string& filepath) {
        // Simple batch results aggregation
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save batch results to: " + filepath);
            return;
        }
        
        file << "{\n";
        file << "  \"total_experiments\": " << experiments.size() << ",\n";
        file << "  \"experiments\": [\n";
        
        for (size_t i = 0; i < experiments.size(); ++i) {
            const auto& exp = experiments[i];
            file << "    {\n";
            file << "      \"name\": \"" << exp->getName() << "\",\n";
            file << "      \"seed\": " << exp->getSeed() << ",\n";
            file << "      \"steps_count\": " << exp->getStepData().size() << 
                 ",\n";
            if (i < experiments.size() - 1) file << "    },\n";
            else file << "    }\n";
        }
        file << "  ]\n";
        file << "}\n";
        file.close();
        NLM_LOG_INFO("Batch results saved to: " + filepath);
    }
};

// Interactive mode
void interactiveMode() {
    std::cout << "Interactive Mode - NLM Command Console" << std::endl;
    std::cout << "Type 'help' for commands, 'exit' to quit." << std::endl;
    
    bool running = true;
    while (running) {
        std::cout << ">>> ";
        std::string command;
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            running = false;
            std::cout << "Exiting interactive mode." << std::endl;
        }
        else if (command == "help") {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  help        - Show this help" << std::endl;
            std::cout << "  version     - Show NLM version" << std::endl;
            std::cout << "  status      - Show current brain status" << std::endl;
            std::cout << "  reset       - Reset the brain" << std::endl;
            std::cout << "  run steps N - Run N simulation steps" << std::endl;
            std::cout << "  configure   - Open configuration editor" << std::endl;
            std::cout << "  exit/quit   - Exit interactive mode" << std::endl;
        }
        else if (command == "version") {
            printVersion();
        }
        else if (command == "status") {
            // Would need brain instance here
            std::cout << "Status command requires active brain instance." << std::endl;
        }
        else if (command == "reset") {
            // Would need brain instance here
            std::cout << "Reset command requires active brain instance." << std::endl;
        }
        else if (command.substr(0, 7) == "run steps") {
            // Would need brain instance here
            std::cout << "Run command requires active brain instance." << std::endl;
        }
        else if (command == "configure") {
            std::cout << "Configuration editor not yet implemented." << std::endl;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
}

void runSimpleMode(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== Simple Mode ===");
    NLM_LOG_INFO("Running basic demonstrations...");
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset and run Test 2: Plasticity learning experiment
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("Simple mode demonstration completed.");
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    cli::CommandOptions options = cli::parseArguments(argc, argv);
    
    // Handle special modes
    if (options.mode == cli::CommandMode::Help) {
        cli::printHelp();
        return 0;
    }
    
    if (options.mode == cli::CommandMode::Version) {
        cli::printVersion();
        return 0;
    }
    
    if (options.mode == cli::CommandMode::Interactive) {
        NLM_LOG_INFO("Starting in interactive mode...");
        interactiveMode();
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Apply parameter sweeps
    for (const auto& param : options.paramSweeps) {
        std::string key = param.first;
        std::string value = param.second;
        
        // Try to determine type and set accordingly
        try {
            if (key.find("count") != std::string::npos || 
                key.find("steps") != std::string::npos ||
                key.find("trials") != std::string::npos) {
                size_t intValue = std::stoull(value);
                config->set(key, static_cast<int64_t>(intValue), ConfigSource::Runtime);
            } else if (key.find("rate") != std::string::npos ||
                       key.find("probability") != std::string::npos ||
                       key.find("time") != std::string::npos) {
                double doubleValue = std::stod(value);
                config->set(key, doubleValue, ConfigSource::Runtime);
            } else {
                config->set(key, value, ConfigSource::Runtime);
            }
        } catch (...) {
            NLM_LOG_WARNING("Failed to parse parameter: " + key + " = " + value);
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", options.randomSeed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize real-time monitor if requested
    RealTimeMonitor monitor;
    if (options.realTime) {
        monitor.start();
    }
    
    // Execute based on mode
    if (options.mode == cli::CommandMode::Simple) {
        NLM_LOG_INFO("Running in simple mode...");
        runSimpleMode(config);
    }
    else if (options.mode == cli::CommandMode::Batch) {
        NLM_LOG_INFO("Running in batch mode...");
        BatchExperimentRunner batchRunner(config);
        
        // Add parameter sweeps
        for (const auto& param : options.paramSweeps) {
            batchRunner.addParameterSweep(param.first, param.second);
        }
        
        std::vector<std::string> batchConfigs;
        if (!options.batchConfigs.empty()) {
            batchConfigs = options.batchConfigs;
        } else {
            batchConfigs.push_back(options.configFile);
        }
        
        auto results = batchRunner.runBatchExperiments(
            batchConfigs, 
            options.experimentName, 
            monitor, 
            options.trials
        );
        
        NLM_LOG_INFO("Batch mode completed. Ran " + std::to_string(results.size()) + " experiments.");
    }
    else if (options.mode == cli::CommandMode::Benchmark) {
        NLM_LOG_INFO("Running in benchmark mode...");
        
        // Create brain for benchmarking
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for benchmarking!");
            return 1;
        }
        
        PerformanceBenchmark benchmark;
        auto results = benchmark.runBenchmarks(brain, config, options.trials);
        
        NLM_LOG_INFO("Benchmark mode completed.");
    }
    else {
        // Default behavior - run all demonstrations
        NLM_LOG_INFO("No mode specified, running default demonstrations...");
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run all tests
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain);
        
        brain->logStatus();
        
        NLM_LOG_INFO("Default mode completed.");
    }
    
    // Stop real-time monitoring
    if (options.realTime) {
        monitor.stop();
    }
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    
    if (options.verbose) {
        NLM_LOG_INFO("Verbose output enabled.");
        std::cout << config->summary() << std::endl;
    }
    
    return 0;
}
