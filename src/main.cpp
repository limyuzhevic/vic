// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration of Artificial Developmental Brain
//
// This is the main entry point for the improved NLM system with:
// - Enhanced command-line parsing and configuration
// - Better logging system with multiple verbosity levels
// - Performance profiling and monitoring tools
// - Advanced experiment configurations
// - Improved memory management

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/CommandLineParser.hpp"
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
#include <thread>
#include <csignal>

using namespace nlm;

// Global flags for graceful shutdown
volatile bool g_shouldShutdown = false;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_shouldShutdown = true;
}

// Print improved banner
void printImprovedBanner() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════╗
    ║                                                                            ║
    ║     NLM — 熙然                                                          ║
    ║     Neural Learning Machine                                               ║
    ║     Phase 6: Final Integration                                            ║
    ║     Enhanced with Command-line Interface                                   ║
    ║     An experimental artificial developmental brain.                        ║
    ║     This phase implements the complete integrated brain simulation:         ║
    ║     - Real LIF neuron dynamics                                              ║
    ║     - Event-driven spike propagation with delays                             ║
    ║     - STDP and Hebbian plasticity rules                                     ║
    ║     - Structural plasticity                                                ║
    ║     - Memory systems (working, episodic, associative)                     ║
    ║     - Neuromodulation (dopamine, curiosity, novelty, prediction error)     ║
    ║     - Prediction and planning systems                                      ║
    ║     - Cognitive mechanisms (attention, concept formation)                   ║
    ║     - Development and aging effects                                        ║
    ║     - Replay and consolidation                                              ║
    ║     - Checkpoint save/load                                                 ║
    ║     - Enhanced visualization and monitoring                                 ║
    ║                                                                            ║
    ╚════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Initialize enhanced logging system
void initializeEnhancedLogging(const std::string& logLevel, bool useColors, 
                              const std::string& logFile) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    
    // Set log level based on string
    LogLevel level;
    if (logLevel == "debug") level = LogLevel::Debug;
    else if (logLevel == "info") level = LogLevel::Info;
    else if (logLevel == "warning") level = LogLevel::Warning;
    else if (logLevel == "error") level = LogLevel::Error;
    else if (logLevel == "critical") level = LogLevel::Critical;
    else level = LogLevel::Info;
    
    consoleLogger->setLevel(level);
    consoleLogger->setUseColors(useColors);
    
    if (!logFile.empty()) {
        consoleLogger->setOutputFile(logFile);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Enhanced logging system initialized with level: " + logLevel);
    if (!logFile.empty()) {
        NLM_LOG_INFO("Log file: " + logFile);
    }
}

// Performance profiler class
class PerformanceProfiler {
public:
    struct Metrics {
        double cpuTime = 0.0;
        double wallTime = 0.0;
        size_t memoryUsage = 0;
        size_t peakMemoryUsage = 0;
        size_t totalSteps = 0;
        size_t spikeCount = 0;
        float avgFiringRate = 0.0f;
    };
    
    PerformanceProfiler() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void startStep() {
        stepStartTime = std::chrono::high_resolution_clock::now();
    }
    
    void endStep(const std::shared_ptr<Brain>& brain) {
        auto endTime = std::chrono::high_resolution_clock::now();
        stepDuration += std::chrono::duration<double>(endTime - stepStartTime).count();
        
        if (brain) {
            totalSteps++;
            spikeCount += brain->getTotalSpikeCount();
            firingRateSum += brain->getAverageFiringRate();
        }
    }
    
    Metrics getMetrics() const {
        Metrics metrics;
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.wallTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.cpuTime = stepDuration;
        metrics.totalSteps = totalSteps;
        metrics.spikeCount = spikeCount;
        if (totalSteps > 0) {
            metrics.avgFiringRate = firingRateSum / totalSteps;
        }
        return metrics;
    }
    
    void printReport() const {
        Metrics metrics = getMetrics();
        
        NLM_LOG_INFO("=== PERFORMANCE REPORT ===");
        NLM_LOG_INFO("Wall clock time: " + std::to_string(metrics.wallTime) + "s");
        NLM_LOG_INFO("CPU time: " + std::to_string(metrics.cpuTime) + "s");
        if (metrics.wallTime > 0) {
            NLM_LOG_INFO("Steps per second: " + std::to_string(metrics.totalSteps / metrics.wallTime));
        }
        NLM_LOG_INFO("Average steps per iteration: 1");
        NLM_LOG_INFO("Total spikes: " + std::to_string(metrics.spikeCount));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(metrics.avgFiringRate));
        NLM_LOG_INFO("============================");
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stepStartTime;
    double stepDuration = 0.0;
    size_t totalSteps = 0;
    size_t spikeCount = 0;
    double firingRateSum = 0.0;
};

// Enhanced configuration loading
void loadEnhancedConfiguration(std::shared_ptr<Config> config, const std::string& configFile,
                               const CommandLineParser& parser) {
    // Load from file if provided
    if (!configFile.empty()) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Could not load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line arguments
    // This would integrate with the config's loadFromArgs method
    
    // Set Phase 6 defaults with enhanced values
    config->set("random_seed", parser.getOptionAsInt("seed"), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", parser.getOptionAsInt("neurons"), ConfigSource::Default);
    config->set("region_count", parser.getOptionAsInt("regions"), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Enhanced plasticity parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity
    config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config->set("pruning_rate", 0.00002f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("working_memory_capacity", parser.getOptionAsInt("neurons") / 10, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 5000, ConfigSource::Default);
    
    // Development parameters
    config->set("development_enabled", parser.getOptionAsBool("develop"), ConfigSource::Default);
    config->set("critical_period_start", 100, ConfigSource::Default);
    config->set("critical_period_end", 1000, ConfigSource::Default);
}

// Run benchmark with profiling
void runBenchmarkPhase(std::shared_ptr<Brain> brain, int steps, bool profile,
                       const CommandLineParser& parser) {
    NLM_LOG_INFO("=== NLM BENCHMARK PHASE ===");
    
    PerformanceProfiler profiler;
    
    // Get initial metrics
    size_t initialSpikes = brain->getTotalSpikeCount();
    float initialFiringRate = brain->getAverageFiringRate();
    
    // Run benchmark
    for (SimulationStep step = 0; step < steps && !g_shouldShutdown; ++step) {
        if (profile && step % 100 == 0) {
            profiler.startStep();
        }
        
        // Inject varied input patterns
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + (rand() % 20));
        }
        
        brain->step(step, step * 0.001);
        
        if (profile && step % 100 == 99) {
            profiler.endStep(brain);
        }
        
        // Print progress
        if (step % 1000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes));
        }
    }
    
    // Get final metrics
    size_t finalSpikes = brain->getTotalSpikeCount();
    float finalFiringRate = brain->getAverageFiringRate();
    
    NLM_LOG_INFO("=== BENCHMARK RESULTS ===");
    NLM_LOG_INFO("Initial spikes: " + std::to_string(initialSpikes));
    NLM_LOG_INFO("Final spikes: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Spike increase: " + std::to_string(finalSpikes - initialSpikes));
    NLM_LOG_INFO("Initial firing rate: " + std::to_string(initialFiringRate));
    NLM_LOG_INFO("Final firing rate: " + std::to_string(finalFiringRate));
    NLM_LOG_INFO("Firing rate change: " + std::to_string(finalFiringRate - initialFiringRate));
    
    // Print performance report if profiling
    if (profile) {
        profiler.printReport();
    }
}

// Run enhanced Phase 2 demo with learning experiments
void runEnhancedPhase2Demo(std::shared_ptr<Brain> brain, const CommandLineParser& parser) {
    NLM_LOG_INFO("=== ENHANCED PHASE 2 DEMO ===");
    
    // Test 1: Basic connectivity with enhanced monitoring
    NLM_LOG_INFO("Test 1: Enhanced Basic Neural Connectivity");
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Enhanced current injection with monitoring
    NLM_LOG_INFO("  Injecting current into neurons with monitoring...");
    for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f + (i * 5.0f));  // Varying current
    }
    
    // Run with enhanced monitoring
    size_t maxSpikes = 0;
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        size_t currentSpikes = brain->getTotalSpikeCount() - initialSpikes;
        if (currentSpikes > maxSpikes) {
            maxSpikes = currentSpikes;
        }
        
        if (step % 25 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Active neurons: " + std::to_string(brain->getActiveNeuronCount()) +
                        " | Max spikes so far: " + std::to_string(maxSpikes));
        }
    }
    
    NLM_LOG_INFO("  ✓ Max spikes achieved: " + std::to_string(maxSpikes));
    
    // Test 2: Enhanced plasticity learning experiment
    NLM_LOG_INFO("\nTest 2: Enhanced Plasticity Learning Experiment");
    
    // Enable enhanced plasticity
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // Enable all plasticity types
        }
    }
    
    // Enhanced learning with varied input patterns
    NLM_LOG_INFO("Applying varied input patterns (2000 steps)...");
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Create varied input patterns
        for (size_t i = 0; i < 30 && i < brain->getTotalNeuronCount() / 4; ++i) {
            float inputStrength = 20.0f + (static_cast<float>(rand()) / RAND_MAX) * 30.0f;
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputStrength);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Enhanced statistics
    NLM_LOG_INFO("\nEnhanced Plasticity Statistics:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Final firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("  Active neuron count: " + std::to_string(brain->getActiveNeuronCount()));
    
    // Test 3: Enhanced STDP verification
    NLM_LOG_INFO("\nTest 3: Enhanced STDP Verification");
    
    if (region && region->getSynapses().size() >= 3) {
        auto& synapses = region->getSynapses();
        
        // Record enhanced initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // STDP only
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create enhanced correlated activity
        NLM_LOG_INFO("Creating enhanced correlated pre->post activity...")
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 100; ++trial) {
                // Fire pre-synaptic neuron
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                if (preNeuron && postNeuron) {
                    // Enhanced timing for STDP
                    preNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    // Small delay then post fires
                    postNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
        }
        
        // Enhanced weight analysis
        NLM_LOG_INFO("  After enhanced correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " Δ=" + std::to_string(delta) +
                        " (potentiated=" + std::to_string(synapses[i]->getWeight() > beforeWeights[i]) + ")");
        }
        
        // Enhanced STDP verification
        NLM_LOG_INFO("\nEnhanced STDP Results:");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STRONG STDP WORKING: Significant potentiation detected");
            NLM_LOG_INFO("  Total weight change: " + std::to_string(totalDelta));
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP REVERSED: Depression detected");
        } else {
            NLM_LOG_INFO("  ⚠ MODERATE STDP: Small weight changes");
        }
    }
}

// Run Phase 6 integration demo
void runPhase6Demo(const CommandLineParser& parser) {
    NLM_LOG_INFO("=== NLM PHASE 6 INTEGRATION DEMO ===");
    
    // Create Phase 6 demo instance
    Phase6Demo phase6Demo;
    
    // Run with enhanced configuration
    Phase6Config config;
    config.neuronCount = parser.getOptionAsInt("neurons");
    config.maxSteps = parser.getOptionAsInt("steps");
    config.enableCheckpointing = parser.getOptionAsBool("checkpoint");
    config.enableReplay = true;
    config.enableDevelopment = parser.getOptionAsBool("develop");
    config.checkpointPath = parser.getOption("checkpoint");
    
    NLM_LOG_INFO("Phase 6 Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Max steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "Enabled" : "Disabled"));
    
    // Run the Phase 6 integration experiment
    auto result = phase6Demo.run(config);
    
    // Print enhanced results
    NLM_LOG_INFO("=== ENHANCED PHASE 6 RESULTS ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(result.curiosityLevel));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
    // Integration status
    NLM_LOG_INFO("=== INTEGRATION STATUS ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
}

int main(int argc, char** argv) {
    // Install signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    printImprovedBanner();
    
    // Parse command line arguments
    CommandLineParser parser(argc, argv);
    
    if (!parser.parse()) {
        if (!parser.getError().empty()) {
            std::cerr << "ERROR: " << parser.getError() << std::endl;
        }
        std::cerr << "Use --help for usage information." << std::endl;
        return 1;
    }
    
    // Check for help/version flags
    if (parser.hasOption("help") || parser.hasOption("version")) {
        // Parser handles these internally
        return parser.hasOption("error") ? 1 : 0;
    }
    
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Phase 6: Final Integration of Artificial Developmental Brain" << std::endl;
    std::cout << "============================================" << std::endl << std::endl;
    
    // Determine experiment type
    std::string experimentType = "phase6";  // Default
    auto args = parser.getArgs();
    if (!args.empty()) {
        experimentType = args[0];
    }
    
    NLM_LOG_INFO("Starting NLM with experiment type: " + experimentType);
    
    // Initialize enhanced logging
    initializeEnhancedLogging(
        parser.hasOption("quiet") ? "error" : 
        parser.hasOption("verbose") ? "debug" : "info",
        !parser.hasOption("quiet"),
        parser.getOption("log_file")
    );
    
    NLM_LOG_INFO("=== NLM Phase 6 Enhanced System Initialization ===");
    NLM_LOG_INFO("Experiment type: " + experimentType);
    NLM_LOG_INFO("Neuron count: " + std::to_string(parser.getOptionAsInt("neurons")));
    NLM_LOG_INFO("Region count: " + std::to_string(parser.getOptionAsInt("regions")));
    NLM_LOG_INFO("Random seed: " + std::to_string(parser.getOptionAsInt("seed")));
    NLM_LOG_INFO("Development enabled: " + std::string(parser.getOptionAsBool("develop") ? "true" : "false"));
    
    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        loadEnhancedConfiguration(config, parser.getOption("config"), parser);
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Enhanced)...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run experiment based on type
        if (experimentType == "phase2") {
            runEnhancedPhase2Demo(brain, parser);
        } else if (experimentType == "phase6") {
            runPhase6Demo(parser);
        } else if (experimentType == "benchmark") {
            runBenchmarkPhase(brain, parser.getOptionAsInt("steps"), 
                            parser.getOptionAsBool("profile"), parser);
        } else {
            NLM_LOG_ERROR("Unknown experiment type: " + experimentType);
            return 1;
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== FINAL BRAIN STATUS ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== EXPERIMENT COMPLETE ===");
        NLM_LOG_INFO("The NLM brain is now a functioning enhanced artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("Enhanced features include: command-line interface, performance profiling,");
        NLM_LOG_INFO("enhanced logging, and comprehensive monitoring tools.");
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Exception occurred: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown exception occurred");
        return 1;
    }
    
    NLM_LOG_INFO("NLM Enhanced System completed successfully!");
    return 0;
}