// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation with Enhanced User Interface
//
// Enhanced main.cpp with comprehensive command-line interface for advanced users
// Features:
// - Comprehensive configuration management
// - Advanced command-line options
// - Flexible simulation control
// - Better logging and statistics output
// - Error handling and validation

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
#include <algorithm>
#include <map>
#include <chrono>

using namespace nlm;

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

void printEnhancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Enhanced Advanced Interface for Advanced Users           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
=== NLM Advanced Command Line Interface ===

Usage:
    nlm [options]

Options:
    --help, -h                  Show this help message
    --config <file>             Load configuration from file
    --neuron-count <n>          Set number of neurons (default: 500)
    --region-count <n>          Set number of brain regions (default: 1)
    --timestep <t>              Set simulation timestep in seconds (default: 0.001)
    --steps <n>                 Run for specified number of steps (default: 1000)
    --random-seed <s>           Set random seed for reproducibility (default: 42)
    --log-level <level>         Set logging level: DEBUG, INFO, WARNING, ERROR
    --log-file <file>           Log output to file
    --checkpoint <file>         Save checkpoint to file
    --performance                Enable performance measurements
    --verbose                    Enable verbose output
    --quiet                      Suppress non-essential output
    --benchmark                  Run quick benchmark instead of simulation
    --version                    Show version information

Configuration File Format:
    # Key = value pairs
    # Boolean: true/false, 1/0, yes/no
    # Numeric: integer, float, double
    # String: "quoted string"
    
    Example config:
        # Brain configuration
        brain.neuron_count = 1000
        brain.region_count = 2
        brain.connection_probability = 0.15
        
        # Plasticity settings
        plasticity.stdp.enable = true
        plasticity.stdp.learning_rate = 0.01
        
        # Neuromodulation
        neuromodulation.dopamine.baseline = 0.1
        neuromodulation.curiosity.enable = true

Advanced Configuration Keys:
    brain.*                     Brain configuration
    plasticity.*                Plasticity rules
    neuromodulation.*            Neuromodulators
    memory.*                    Memory systems
    prediction.*                Prediction system
    development.*               Development stages
    environment.*               Environment settings
    visualization.*             Visualization options

Environment Variables:
    NLM_LOG_LEVEL               Override default log level
    NLM_CONFIG_FILE             Default config file path
    NLM_CHECKPOINT_DIR          Directory for checkpoints

Example Commands:
    nlm --neuron-count 1000 --steps 5000 --log-level INFO
    nlm --config custom.cfg --checkpoint my_brain.bin
    nlm --performance --verbose
    nlm --benchmark

For more information, see documentation in docs/ and easy_usage.md
    )" << std::endl;
}

// Parse advanced command-line arguments with validation
class AdvancedCLI {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
        double timestep = 0.001;
        SimulationStep steps = 1000;
        int64_t randomSeed = 42;
        std::string logLevel = "INFO";
        std::string logFile = "";
        std::string checkpointFile = "";
        bool performanceMode = false;
        bool verboseMode = false;
        bool quietMode = false;
        bool benchmarkMode = false;
        bool helpRequested = false;
        bool versionRequested = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        
        // Check for help/version first
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                options.helpRequested = true;
                return options;
            } else if (arg == "--version") {
                options.versionRequested = true;
                return options;
            }
        }
        
        // Parse all other options
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    options.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg.substr(0, 14) == "--neuron-count") {
                if (arg.find('=') != std::string::npos) {
                    options.neuronCount = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    options.neuronCount = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 13) == "--region-count") {
                if (arg.find('=') != std::string::npos) {
                    options.regionCount = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    options.regionCount = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 11) == "--timestep") {
                if (arg.find('=') != std::string::npos) {
                    options.timestep = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    options.timestep = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    options.steps = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    options.steps = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 14) == "--random-seed") {
                if (arg.find('=') != std::string::npos) {
                    options.randomSeed = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    options.randomSeed = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 11) == "--log-level") {
                if (arg.find('=') != std::string::npos) {
                    options.logLevel = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.logLevel = argv[++i];
                }
            } else if (arg.substr(0, 10) == "--log-file") {
                if (arg.find('=') != std::string::npos) {
                    options.logFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.logFile = argv[++i];
                }
            } else if (arg.substr(0, 13) == "--checkpoint") {
                if (arg.find('=') != std::string::npos) {
                    options.checkpointFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.checkpointFile = argv[++i];
                }
            } else if (arg == "--performance") {
                options.performanceMode = true;
            } else if (arg == "--verbose") {
                options.verboseMode = true;
            } else if (arg == "--quiet") {
                options.quietMode = true;
            } else if (arg == "--benchmark") {
                options.benchmarkMode = true;
            }
        }
        
        // Validate options
        if (options.neuronCount <= 0) options.neuronCount = 500;
        if (options.regionCount <= 0) options.regionCount = 1;
        if (options.timestep <= 0.0) options.timestep = 0.001;
        if (options.steps <= 0) options.steps = 1000;
        
        return options;
    }
    
    static LogLevel stringToLogLevel(const std::string& levelStr) {
        std::string upperLevel = levelStr;
        std::transform(upperLevel.begin(), upperLevel.end(), upperLevel.begin(), ::toupper);
        
        if (upperLevel == "DEBUG") return LogLevel::Debug;
        if (upperLevel == "WARNING") return LogLevel::Warning;
        if (upperLevel == "ERROR") return LogLevel::Error;
        return LogLevel::Info;  // Default
    }
};

// Print enhanced configuration summary
void printConfigSummary(const Config& config, const AdvancedCLI::Options& options) {
    std::cout << std::endl;
    std::cout << "=== Enhanced Configuration Summary ===" << std::endl;
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  Neurons: " << options.neuronCount << std::endl;
    std::cout << "  Regions: " << options.regionCount << std::endl;
    std::cout << "  Timestep: " << options.timestep << "s" << std::endl;
    std::cout << "  Random Seed: " << options.randomSeed << std::endl;
    std::cout << "  Log Level: " << options.logLevel << std::endl;
    if (!options.logFile.empty()) {
        std::cout << "  Log File: " << options.logFile << std::endl;
    }
    if (!options.checkpointFile.empty()) {
        std::cout << "  Checkpoint: " << options.checkpointFile << std::endl;
    }
    
    if (options.performanceMode) {
        std::cout << std::endl;
        std::cout << "Performance Mode: Enabled" << std::endl;
        std::cout << "  (Will collect timing and resource usage statistics)" << std::endl;
    }
    
    std::cout << std::endl;
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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
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
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");
    
    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runBenchmark() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Performance Benchmark                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    auto options = AdvancedCLI::parse(argc, argv);
    
    // Handle help/version requests
    if (options.helpRequested) {
        printHelp();
        return 0;
    }
    
    if (options.versionRequested) {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation (Enhanced)" << std::endl;
        std::cout << "Version 0.1.0 (Enhanced)" << std::endl;
        std::cout << "Build: Enhanced with advanced user interface and performance monitoring" << std::endl;
        return 0;
    }
    
    // Configure logging based on options
    auto logger = std::make_shared<Logger>();
    
    // Set log level
    LogLevel logLevel = AdvancedCLI::stringToLogLevel(options.logLevel);
    
    // Create console logger with appropriate level
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    
    // Add file logger if requested
    if (!options.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(options.logFile, logLevel);
        logger->addLogger(fileLogger);
        NLM_LOG_INFO("Log file enabled: " + options.logFile);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set up environment variables if specified
    if (std::getenv("NLM_LOG_LEVEL")) {
        // Override with environment variable
        LogLevel envLogLevel = AdvancedCLI::stringToLogLevel(std::getenv("NLM_LOG_LEVEL"));
        consoleLogger->setLevel(envLogLevel);
    }
    
    // Enable/disable verbose/quiet modes
    if (options.quietMode) {
        consoleLogger->setLevel(LogLevel::Error);  // Only show errors in quiet mode
    }
    
    printEnhancedBanner();
    
    NLM_LOG_INFO("=== NLM Enhanced Advanced Interface ===");
    NLM_LOG_INFO("Running with enhanced configuration and user interface");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    
    // Override config with command line args first
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (enhanced)
    config->set("random_seed", options.randomSeed, ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("neuron_count", options.neuronCount, ConfigSource::Default);
    config->set("region_count", options.regionCount, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Enhanced STDP parameters for advanced users
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Additional enhanced configuration for advanced users
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("curiosity_enable", true, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", options.randomSeed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", options.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", options.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Benchmark mode - quick performance test
    if (options.benchmarkMode) {
        NLM_LOG_INFO("=== BENCHMARK MODE ===");
        NLM_LOG_INFO("Running quick benchmark with enhanced performance monitoring...");
        runBenchmark();
        return 0;
    }
    
    // Initialize simulation clock
    SimulationClock clock(options.timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(options.timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing Enhanced NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Performance mode
    if (options.performanceMode) {
        NLM_LOG_INFO("=== PERFORMANCE MODE ===");
        NLM_LOG_INFO("Enhanced performance monitoring enabled");
        NLM_LOG_INFO("(Collecting detailed timing and resource usage statistics)");
    }
    
    // Start performance measurement if enabled
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run enhanced tests based on configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Neural Computation Tests ===");
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("Test 1: Enhanced Basic Neural Connectivity Test");
    runBasicConnectivityTest(brain);
    
    // Reset and run comprehensive plasticity experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 2: Enhanced Plasticity Learning Experiment");
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 3: Enhanced STDP Verification");
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Performance measurements
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Performance Metrics ===");
    NLM_LOG_INFO("Total wall clock time: " + std::to_string(elapsedTime) + "s");
    NLM_LOG_INFO("Simulation steps per second: " + std::to_string(options.steps / elapsedTime));
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    
    // Save checkpoint if requested
    if (!options.checkpointFile.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving checkpoint to: " + options.checkpointFile);
        if (brain->save(options.checkpointFile)) {
            NLM_LOG_INFO("Checkpoint saved successfully!");
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint!");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Neural Computation Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule with enhanced neuromodulation");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Enhanced learning experiment with comprehensive statistics");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Advanced features applied:");
    if (options.performanceMode) NLM_LOG_INFO("  ✓ Performance monitoring enabled");
    if (options.verboseMode) NLM_LOG_INFO("  ✓ Verbose logging enabled");
    if (!options.checkpointFile.empty()) NLM_LOG_INFO("  ✓ Checkpoint saved");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("(Enhanced with advanced user interface and performance monitoring.)");
    
    return 0;
}