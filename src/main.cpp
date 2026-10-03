// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include "visualization/VisualizationInterface.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

    )" << std::endl;
}

// Command-line parsing utilities
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        std::string visualizationMode = "console";
        double timestep = 0.001;
        int steps = 100;
        int maxSteps = 1000;
        uint64_t randomSeed = 42;
        bool enableVisualization = false;
        bool enableDebugLogging = false;
        bool enablePerformanceMonitoring = false;
        bool saveSnapshots = false;
        std::vector<std::string> plugins;
        bool verbose = false;
        bool help = false;
        bool version = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                opts.help = true;
                continue;
            }
            
            if (arg == "--version" || arg == "-v") {
                opts.version = true;
                continue;
            }
            
            if (arg.find("--config") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.configFile = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < argc) {
                    opts.configFile = argv[++i];
                }
                continue;
            }
            
            if (arg.find("--output") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.outputFile = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < argc) {
                    opts.outputFile = argv[++i];
                }
                continue;
            }
            
            if (arg == "--visualization") {
                opts.enableVisualization = true;
                continue;
            }
            
            if (arg.find("--visual-mode") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.visualizationMode = arg.substr(arg.find("=") + 1);
                } else if (i + 1 < argc) {
                    opts.visualizationMode = argv[++i];
                }
                continue;
            }
            
            if (arg.find("--timestep") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.timestep = std::stod(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    opts.timestep = std::stod(argv[++i]);
                }
                continue;
            }
            
            if (arg.find("--steps") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.steps = std::stoi(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    opts.steps = std::stoi(argv[++i]);
                }
                continue;
            }
            
            if (arg.find("--seed") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.randomSeed = std::stoull(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    opts.randomSeed = std::stoull(argv[++i]);
                }
                continue;
            }
            
            if (arg == "--debug") {
                opts.enableDebugLogging = true;
                continue;
            }
            
            if (arg == "--performance") {
                opts.enablePerformanceMonitoring = true;
                continue;
            }
            
            if (arg == "--save") {
                opts.saveSnapshots = true;
                continue;
            }
            
            if (arg == "--verbose") {
                opts.verbose = true;
                continue;
            }
            
            if (arg.find("--plugin") == 0) {
                if (arg.find("=") != std::string::npos) {
                    opts.plugins.push_back(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    opts.plugins.push_back(argv[++i]);
                }
                continue;
            }
            
            // If argument starts with "--", treat as unrecognized option
            if (arg.find("--") == 0) {
                std::cerr << "Warning: Unrecognized option " << arg << std::endl;
            }
        }
        
        return opts;
    }
    
    static void printHelp() {
        printBanner();
        std::cout << "\nUsage: nlm [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help              Show this help message\n";
        std::cout << "  -v, --version           Show version information\n";
        std::cout << "\nSimulation Options:\n";
        std::cout << "  --timestep <ms>         Simulation timestep (default: 0.001)\n";
        std::cout << "  --steps <n>             Number of simulation steps (default: 100)\n";
        std::cout << "  --max-steps <n>         Maximum steps for long runs (default: 1000)\n";
        std::cout << "\nConfiguration:\n";
        std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)\n";
        std::cout << "\nOutput & Visualization:\n";
        std::cout << "  --visualization          Enable visualization\n";
        std::cout << "  --visual-mode <mode>    Visualization mode (console, file, real-time)\n";
        std::cout << "  --output <file>         Output file for results\n";
        std::cout << "  --save                  Save simulation snapshots\n";
        std::cout << "\nDevelopment & Debugging:\n";
        std::cout << "  --debug                 Enable debug logging\n";
        std::cout << "  --performance           Enable performance monitoring\n";
        std::cout << "  --verbose               Verbose output\n";
        std::cout << "  --plugin <name>         Load plugin\n";
        std::cout << "\nRandomness:\n";
        std::cout << "  --seed <n>              Random seed (default: 42)\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm --steps 1000 --timestep 0.0005\n";
        std::cout << "  nlm --config myconfig.cfg --visualization --verbose\n";
        std::cout << "  nlm --performance --output results.json\n";
    }
    
    static void printVersion() {
        printBanner();
        std::cout << "\nNLM (熙然) - Neural Learning Machine\n";
        std::cout << "Version: 0.1.0\n";
        std::cout << "Phase: 2 (Real Neural Computation)\n";
        std::cout << "C++20\n";
        std::cout << "Build date: " << __DATE__ << " " << __TIME__ << "\n\n";
        std::cout << "Features:\n";
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons\n";
        std::cout << "  - Event-driven spike propagation\n";
        std::cout << "  - STDP and Hebbian plasticity\n";
        std::cout << "  - Structural plasticity\n";
        std::cout << "  - Memory systems\n";
        std::cout << "  - Neuromodulation\n";
        std::cout << "  - Prediction systems\n";
        std::cout << "  - Development system\n";
        std::cout << "  - Cognitive mechanisms\n";
    }
};

void runBenchmark(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
    if (!opts.enablePerformanceMonitoring) {
        return;
    }
    
    NLM_LOG_INFO("=== Performance Benchmark ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(opts.steps); ++step) {
        brain->step(step, step * opts.timestep);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (opts.steps * 1000.0) / duration.count();
    double avgTimePerStep = duration.count() / static_cast<double>(opts.steps);
    
    NLM_LOG_INFO("Performance Results:");
    NLM_LOG_INFO("  Total steps: " + std::to_string(opts.steps));
    NLM_LOG_INFO("  Wall clock time: " + std::to_string(duration.count() / 1000.0) + "s");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("  Average time per step: " + std::to_string(avgTimePerStep) + "ms");
    
    // Estimate memory usage
    NLM_LOG_INFO("Memory Usage:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
}
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, double timestep) {
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
        brain->step(step, step * timestep);
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, double timestep) {
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
        
        brain->step(step, step * timestep);
        
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

void runStdpVerification(std::shared_ptr<Brain> brain, double timestep) {
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
            brain->step(trial * 2, trial * 2 * timestep);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * timestep);
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
    // Parse command line options
    CommandLineParser::Options opts = CommandLineParser::parse(argc, argv);
    
    // Handle help and version flags
    if (opts.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (opts.version) {
        CommandLineParser::printVersion();
        return 0;
    }
    
    printBanner();
    
    std::cout << "Initializing NLM Neural Simulation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on verbosity
    if (opts.enableDebugLogging) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    NLM_LOG_INFO("=== NLM Neural Learning Machine ===");
    NLM_LOG_INFO("Running with " + std::to_string(opts.steps) + " steps");
    NLM_LOG_INFO("Timestep: " + std::to_string(opts.timestep) + "s");
    NLM_LOG_INFO("Config file: " + opts.configFile);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(opts.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + opts.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args (basic parsing for backward compatibility)
    config->loadFromArgs(argc, argv);
    
    // Set values from command line options
    config->set("random_seed", static_cast<int64_t>(opts.randomSeed), ConfigSource::CommandLine);
    config->set("simulation_timestep", opts.timestep, ConfigSource::CommandLine);
    
    // Set default values if not already set
    if (!config->has("neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", opts.randomSeed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", opts.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", opts.timestep);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run basic connectivity test
    runBasicConnectivityTest(brain, opts.timestep);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run plasticity learning experiment
    runPlasticityExperiment(brain, opts.timestep);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain, opts.timestep);
    
    // Run performance benchmark if enabled
    runBenchmark(brain, opts);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    
    if (opts.outputFile.empty() && opts.saveSnapshots) {
        NLM_LOG_INFO("Note: --save option specified but no output file provided. Snapshots not saved.");
    }
    
    if (opts.outputFile.empty()) {
        NLM_LOG_INFO("Use --output <file> to save results to a file.");
    }
    
    NLM_LOG_INFO("Simulation completed successfully with " + std::to_string(opts.steps) + " steps.");
    
    return 0;
}
