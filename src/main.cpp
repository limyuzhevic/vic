// NLM Main Entry Point - Command Line Interface

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
#include "utils.cpp"
#include "learning_experiment.cpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <chrono>

namespace nlm {

void printHelp() {
    std::cout << R"(
NLM - Neural Learning Machine
Usage: nlm [OPTIONS]

Options:
  --mode MODE          Demo mode: phase2, phase6, benchmark, help (default: phase2)
  --config FILE        Configuration file (default: configs/default.cfg)
  --steps N            Number of simulation steps for benchmark mode
  --neurons N          Number of neurons for benchmark mode
  --iterations N       Number of iterations for benchmark mode
  --help               Show this help message
  --version            Show version information

Demo Modes:
  phase2              Phase 2: Real Neural Computation (LIF neurons, STDP, Hebbian plasticity)
  phase6              Phase 6: Final Integration (all systems working together)
  benchmark           Performance benchmark with configurable parameters

Examples:
  nlm --mode phase2              Run Phase 2 neural computation demo
  nlm --mode phase6 --steps 5000  Run Phase 6 integration test with 5000 steps
  nlm --mode benchmark --neurons 2000 --iterations 3  Run performance benchmark

For more information, visit: https://github.com/neural-learning-machine/nlm
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration - Complete Artificial Neural Substrate" << std::endl;
    std::cout << "Built with C++20, event-driven spiking neural computation." << std::endl;
}

void runBenchmarkDemo(std::shared_ptr<Config> config, int steps, int neurons, int iterations) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Performance Benchmark ===");
    NLM_LOG_INFO("Running benchmark with " + std::to_string(neurons) + " neurons for " + 
                std::to_string(steps) + " steps over " + std::to_string(iterations) + " iterations");
    
    auto totalStart = std::chrono::high_resolution_clock::now();
    size_t totalSpikes = 0;
    float totalPerformance = 0.0f;
    
    for (int iter = 0; iter < iterations; ++iter) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Iteration " + std::to_string(iter + 1) + "/" + std::to_string(iterations));
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Update neuron count for this iteration
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain in iteration " + std::to_string(iter + 1));
            continue;
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < steps; ++step) {
            // Inject random current for activity
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount();
        totalSpikes += spikes;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        float performance = static_cast<float>(steps) / (duration.count() / 1000.0f); // steps per second
        totalPerformance += performance;
        
        NLM_LOG_INFO("  Steps: " + std::to_string(steps) + 
                    " | Spikes: " + std::to_string(spikes) + 
                    " | Time: " + std::to_string(duration.count()) + "ms" +
                    " | Performance: " + std::to_string(static_cast<int>(performance)) + " steps/sec");
    }
    
    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart);
    float avgPerformance = iterations > 0 ? totalPerformance / iterations : 0.0f;
    float totalStepsPerSec = iterations > 0 ? static_cast<float>(iterations * steps) / (totalDuration.count() / 1000.0f) : 0.0f;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("Total iterations: " + std::to_string(iterations));
    NLM_LOG_INFO("Total steps: " + std::to_string(iterations * steps));
    NLM_LOG_INFO("Total spikes: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("Average spikes per step: " + std::to_string(static_cast<float>(totalSpikes) / (iterations * steps)));
    NLM_LOG_INFO("Average iteration time: " + std::to_string(totalDuration.count() / iterations) + "ms");
    NLM_LOG_INFO("Average performance: " + std::to_string(static_cast<int>(avgPerformance)) + " steps/sec");
    NLM_LOG_INFO("Overall performance: " + std::to_string(static_cast<int>(totalStepsPerSec)) + " steps/sec");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string mode = "phase2";
    std::string configFile = "configs/default.cfg";
    int steps = 1000;
    int neurons = 500;
    int iterations = 1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg.substr(0, 7) == "--mode=") {
            mode = arg.substr(7);
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 8) == "--steps=") {
            steps = std::stoi(arg.substr(8));
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoi(argv[++i]);
        } else if (arg.substr(0, 9) == "--neurons=") {
            neurons = std::stoi(arg.substr(9));
        } else if (arg == "--neurons" && i + 1 < argc) {
            neurons = std::stoi(argv[++i]);
        } else if (arg.substr(0, 13) == "--iterations=") {
            iterations = std::stoi(arg.substr(13));
        } else if (arg == "--iterations" && i + 1 < argc) {
            iterations = std::stoi(argv[++i]);
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    printBanner();
    
    NLM_LOG_INFO("=== NLM Neural Learning Machine v0.1.0 ===");
    NLM_LOG_INFO("Phase 6: Final Integration - Complete Artificial Neural Substrate");
    NLM_LOG_INFO("Running in mode: " + mode);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set Phase 2: Real Neural Computation defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    int result = 0;
    
    if (mode == "phase2") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2: Real Neural Computation Demo ===");
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run the three Phase 2 tests
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Phase 2 Demo Complete");
        
    } else if (mode == "phase6") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 6: Final Integration Demo ===");
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run comprehensive integration test
        Phase6IntegrationTest integrationTest(brain);
        integrationTest.runAllTests();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Phase 6 Demo Complete");
        
    } else if (mode == "benchmark") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Performance Benchmark ===");
        
        // Configure for benchmark
        config->set("enable_checkpoints", false, ConfigSource::Runtime);
        config->set("enable_progress_logging", false, ConfigSource::Runtime);
        
        runBenchmarkDemo(config, steps, neurons, iterations);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Benchmark Complete");
        
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        NLM_LOG_INFO("Use --help for available modes");
        return 1;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Complete ===");
    NLM_LOG_INFO("NLM Neural Learning Machine finished successfully.");
    
    return result;
}

} // namespace nlm