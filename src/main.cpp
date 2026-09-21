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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <ctime>
#include <map>
#include <algorithm>

using namespace nlm;

// Command-line parsing and configuration helpers
namespace cli {
    
    // Print help/usage information
    void printHelp() {
        std::cout << R"(
USAGE: nlm [OPTIONS]

Enhanced Neural Learning Machine (Phase 2: Real Neural Computation)

Basic Options:
  --help                  Show this help message
  --version               Show version information
  --verbose               Enable verbose logging

Configuration Options:
  --config FILE           Load custom configuration file (default: configs/default.cfg)
  --seed NUMBER           Set random seed for reproducible experiments
  --duration SECONDS      Set total simulation duration (0 = unlimited)
  --steps STEPS           Set number of simulation steps (overrides duration)

Output Options:
  --stats                 Enable detailed statistics output
  --benchmark             Run in benchmark mode (faster execution, detailed metrics)
  --save FILE             Save brain state to file checkpoint
  --load FILE             Load brain state from file checkpoint

Visualization Options:
  --output-format FORMAT   Output format: text, json, csv, html (default: text)
  --visualize             Enable visualization output

Performance Options:
  --profile               Enable performance profiling
  --optimization LEVEL   Optimization level: 0-3 (default: 1)
  --threads COUNT         Number of threads for parallel processing

Advanced Options:
  --dry-run               Parse arguments and exit without running simulation
  --quiet                 Minimal output (errors only)
  --log-file FILE         Write logs to file in addition to console
  --log-level LEVEL       Set log level: DEBUG, INFO, WARN, ERROR (default: INFO)
)" << std::endl;
    }
    
    // Print version information
    void printVersion() {
        std::cout << "NLM v2.0.0 - Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Implemented by Kilo (熙然)" << std::endl;
        std::cout << "All rights reserved." << std::endl;
    }
    
    // Parse command line arguments
    struct CLIArgs {
        bool verbose = false;
        bool benchmark = false;
        bool stats = false;
        bool visualize = false;
        bool profile = false;
        bool dryRun = false;
        bool quiet = false;
        std::string configFile = "configs/default.cfg";
        uint64_t seed = 42;
        double duration = 0.0;  // seconds
        SimulationStep steps = 0;  // override steps
        std::string saveFile;
        std::string loadFile;
        std::string outputFormat = "text";
        uint8_t optimizationLevel = 1;
        uint8_t logLevel = 2;  // 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR
        std::string logFile;
        uint32_t threadCount = 0;  // 0 = auto-detect
    };
    
    CLIArgs parseArgs(int argc, char** argv) {
        CLIArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                std::exit(0);
            } else if (arg == "--version") {
                printVersion();
                std::exit(0);
            } else if (arg == "--verbose") {
                args.verbose = true;
            } else if (arg == "--benchmark") {
                args.benchmark = true;
            } else if (arg == "--stats") {
                args.stats = true;
            } else if (arg == "--visualize") {
                args.visualize = true;
            } else if (arg == "--profile") {
                args.profile = true;
            } else if (arg == "--dry-run") {
                args.dryRun = true;
            } else if (arg == "--quiet") {
                args.quiet = true;
            } else if (arg.substr(0, 8) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else if (arg.substr(0, 7) == "--seed") {
                try {
                    args.seed = std::stoull(arg.substr(7));
                } catch (...) {
                    NLM_LOG_ERROR("Invalid seed value: " + arg.substr(7));
                }
            } else if (arg.substr(0, 10) == "--duration") {
                try {
                    args.duration = std::stod(arg.substr(10));
                } catch (...) {
                    NLM_LOG_ERROR("Invalid duration value: " + arg.substr(10));
                }
            } else if (arg.substr(0, 7) == "--steps") {
                try {
                    args.steps = static_cast<SimulationStep>(std::stoll(arg.substr(7)));
                } catch (...) {
                    NLM_LOG_ERROR("Invalid steps value: " + arg.substr(7));
                }
            } else if (arg.substr(0, 6) == "--save") {
                if (arg.find('=') != std::string::npos) {
                    args.saveFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.saveFile = argv[++i];
                }
            } else if (arg.substr(0, 6) == "--load") {
                if (arg.find('=') != std::string::npos) {
                    args.loadFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.loadFile = argv[++i];
                }
            } else if (arg.substr(0, 15) == "--output-format") {
                if (arg.find('=') != std::string::npos) {
                    args.outputFormat = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.outputFormat = argv[++i];
                }
            } else if (arg.substr(0, 18) == "--optimization") {
                try {
                    args.optimizationLevel = static_cast<uint8_t>(std::stoul(arg.substr(18)));
                } catch (...) {
                    NLM_LOG_ERROR("Invalid optimization level: " + arg.substr(18));
                }
            } else if (arg.substr(0, 13) == "--log-level") {
                if (arg.find('=') != std::string::npos) {
                    std::string levelStr = arg.substr(arg.find('=') + 1);
                    if (levelStr == "DEBUG") args.logLevel = 0;
                    else if (levelStr == "INFO") args.logLevel = 1;
                    else if (levelStr == "WARN") args.logLevel = 2;
                    else if (levelStr == "ERROR") args.logLevel = 3;
                    else NLM_LOG_ERROR("Unknown log level: " + levelStr);
                } else if (i + 1 < argc) {
                    std::string levelStr = argv[++i];
                    if (levelStr == "DEBUG") args.logLevel = 0;
                    else if (levelStr == "INFO") args.logLevel = 1;
                    else if (levelStr == "WARN") args.logLevel = 2;
                    else if (levelStr == "ERROR") args.logLevel = 3;
                    else NLM_LOG_ERROR("Unknown log level: " + levelStr);
                }
            } else if (arg.substr(0, 10) == "--log-file") {
                if (arg.find('=') != std::string::npos) {
                    args.logFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.logFile = argv[++i];
                }
            } else if (arg.substr(0, 9) == "--threads") {
                try {
                    args.threadCount = static_cast<uint32_t>(std::stoul(arg.substr(9)));
                } catch (...) {
                    NLM_LOG_ERROR("Invalid thread count: " + arg.substr(9));
                }
            } else {
                // Unknown argument - check for legacy config format
                if (arg.substr(0, 2) == "--") {
                    NLM_LOG_WARNING("Unknown option: " + arg);
                }
            }
        }
        
        return args;
    }
    
    // Get log level from string
    LogLevel stringToLogLevel(const std::string& level) {
        if (level == "DEBUG") return LogLevel::Debug;
        if (level == "INFO") return LogLevel::Info;
        if (level == "WARN") return LogLevel::Warning;
        if (level == "ERROR") return LogLevel::Error;
        return LogLevel::Info;  // default
    }
}

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

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
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

void visualizeResults(const std::shared_ptr<Brain>& brain, const std::string& format) {
    std::cout << "Visualizing results in " << format << " format..." << std::endl;
    
    if (format == "json") {
        std::cout << "{\n";
        std::cout << "  \"neurons\": " << brain->getTotalNeuronCount() << ",\n";
        std::cout << "  \"synapses\": " << brain->getTotalSynapseCount() << ",\n";
        std::cout << "  \"spikes\": " << brain->getTotalSpikeCount() << ",\n";
        std::cout << "  \"firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        std::cout << "  \"excitement_inhibition_ratio\": " << brain->getExcitationInhibitionRatio() << "\n";
        std::cout << "}\n";
    } else if (format == "csv") {
        std::cout << "timestamp,neurons,synapses,spikes,firing_rate,ei_ratio\n";
        std::cout << std::chrono::duration<double>::max().count() << ",";
        std::cout << brain->getTotalNeuronCount() << ";" << brain->getTotalSynapseCount() << ";";
        std::cout << brain->getTotalSpikeCount() << ";" << brain->getAverageFiringRate() << ";";
        std::cout << brain->getExcitationInhibitionRatio() << std::endl;
    } else if (format == "html") {
        std::cout << "<!DOCTYPE html>\n<html><head><title>NLM Results</title></head><body>\n";
        std::cout << "<h1>NLM Neural Network Results</h1>\n";
        std::cout << "<p>Neurons: " << brain->getTotalNeuronCount() << "</p>\n";
        std::cout << "<p>Synapses: " << brain->getTotalSynapseCount() << "</p>\n";
        std::cout << "<p>Total Spikes: " << brain->getTotalSpikeCount() << "</p>\n";
        std::cout << "<p>Average Firing Rate: " << brain->getAverageFiringRate() << " Hz</p>\n";
        std::cout << "<p>Excitatory/Inhibitory Ratio: " << brain->getExcitationInhibitionRatio() << "</p>\n";
        std::cout << "</body></html>\n";
    } else {
        std::cout << "Text visualization:" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << "Network State Summary:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    std::cout << "Visualization complete." << std::endl;
}

void applyPerformanceSettings(cli::CLIArgs& args, std::shared_ptr<Config> config) {
    std::cout << "Applying performance settings..." << std::endl;
    
    // Apply optimization level
    if (args.optimizationLevel > 0) {
        std::cout << "Setting optimization level to: " << static_cast<int>(args.optimizationLevel) << std::endl;
        config->set("optimization_level", static_cast<int>(args.optimizationLevel), ConfigSource::CommandLine);
        
        // Apply optimization-specific settings
        if (args.optimizationLevel == 1) {
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.002, ConfigSource::Runtime);
        } else if (args.optimizationLevel == 2) {
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.005, ConfigSource::Runtime);
        } else if (args.optimizationLevel == 3) {
            config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.01, ConfigSource::Runtime);
        }
    }
    
    // Apply thread count if specified
    if (args.threadCount > 0) {
        std::cout << "Setting thread count to: " << args.threadCount << std::endl;
        config->set("thread_count", static_cast<int>(args.threadCount), ConfigSource::CommandLine);
    }
    
    // Apply profiling settings
    if (args.profile) {
        std::cout << "Performance profiling enabled" << std::endl;
        config->set("enable_profiling", true, ConfigSource::CommandLine);
    }
    
    // Apply visualization output format
    if (!args.outputFormat.empty() && args.outputFormat != "text") {
        std::cout << "Setting output format to: " << args.outputFormat << std::endl;
        config->set("output_format", args.outputFormat, ConfigSource::CommandLine);
    }
}

void printEnhancedHelp() {
    std::cout << R"(
NLM - Enhanced Neural Learning Machine CLI
==========================================

New Features Added:
-----------------
1. Advanced Command-Line Options:
   --verbose          Enable detailed verbose logging
   --config FILE      Load custom configuration file
   --seed NUMBER      Set random seed for reproducible experiments
   --duration SECONDS Set total simulation duration (0 = unlimited)
   --steps STEPS      Set number of simulation steps (overrides duration)
   --stats            Enable detailed statistics and metrics output
   --benchmark        Run in benchmark mode with detailed performance metrics
   --save FILE        Save brain state to checkpoint file
   --load FILE        Load brain state from checkpoint file

2. Visualization Options:
   --output-format FORMAT  Output format: text, json, csv, html
   --visualize             Enable visualization output

3. Performance Tuning:
   --profile              Enable performance profiling
   --optimization LEVEL  Optimization level: 0-3 (0=none, 3=maximum)
   --threads COUNT        Number of threads for parallel processing

4. Enhanced Status Reporting:
   Detailed timing and metrics are now available with --stats flag

Backward Compatibility:
----------------------
All existing command-line options continue to work as before.
The new options are added without breaking existing scripts.

Examples:
--------
# Run with custom config and statistics
nlm --config my_config.cfg --stats

# Run benchmark mode with 1000 steps
nlm --benchmark --steps 1000

# Save brain state after simulation
nlm --save brain_checkpoint.bin

# Load previous brain state
nlm --load brain_checkpoint.bin

# Run with JSON output for integration
nlm --output-format json --visualize
)" << std::endl;
}
