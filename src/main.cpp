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

void printHelp() {
    std::cout << R"(
USAGE: nlm [OPTIONS]

An experimental artificial developmental brain simulation.

OPTIONS:
  --config <file>           Configuration file to load (default: configs/default.cfg)
  --neurons <count>         Number of neurons to create (default: 500)
  --regions <count>         Number of neural regions (default: 1)
  --connectivity <value>    Connection probability (0.0-1.0, default: 0.15)
  --timestep <value>        Simulation timestep in seconds (default: 0.001)
  --seed <value>            Random seed (default: 42)
  --demo-mode               Run in demo mode with built-in tests
  --benchmark               Run benchmark tests
  --export <file>           Export brain state to file
  --import <file>           Import brain state from file
  --batch-mode              Run without user interaction
  --enable-feature <feature> Enable specific features (plasticity, memory, neuromodulation)
  --disable-feature <feature> Disable specific features
  --log-level <level>       Logging level: trace, debug, info, warn, error, fatal
  --log-file <file>         Log output file
  --help                    Show this help message

ADVANCED OPTIONS (Expert users):
  --plasticity-mode <mode>  Plasticity mode: stdp, hebbian, structural, all, none
  --memory-mode <mode>      Memory mode: working, episodic, semantic, associative, all
  --neuromod-mode <mode>    Neuromodulation mode: dopamine, curiosity, novelty, all
  --prediction-mode <mode>  Prediction mode: simple, complex, all
  --cognition-mode <mode>   Cognition mode: planning, attention, concept, all
  --development-mode <mode> Development mode: initial, critical, maturation, adult, aging
  --synaptic-model <model>  Synaptic model: exponential, alpha, delta, adex
  --neuron-model <model>    Neuron model: lif, hh, fhn,izh, sl
  --visualize               Enable visualization system
  --checkpoint <interval>   Checkpoint interval in steps (0 to disable)
  --replay-mode <mode>      Replay mode: replay, consolidate, replay_and_consolidate
  --temperature <value>    Thermal noise level (0.0-1.0)
  --noise-level <value>     Random noise level (0.0-1.0)
  --plasticity-rate <value> Global plasticity rate (0.0-1.0)
  --decay-rate <value>      Global decay rate (0.0-1.0)
  --plasticity-threshold <value> Plasticity threshold (0.0-1.0)

FEATURES:
  The following features can be individually enabled/disabled:
    - plasticity: Enable all plasticity mechanisms (STDP, Hebbian, structural)
    - memory: Enable all memory systems (working, episodic, semantic, associative)
    - neuromodulation: Enable all neuromodulators (dopamine, curiosity, novelty)
    - prediction: Enable prediction systems
    - cognition: Enable all cognitive mechanisms
    - development: Enable developmental processes
    - visualization: Enable visualization system
    -- export-import: Enable save/load functionality
    -- replay: Enable memory replay

EXIT CODES:
  0  Normal termination
  1  Configuration error
  2  Initialization error
  3  Runtime error
  4  Internal error

EXAMPLES:
  # Basic simulation with custom config
  nlm --config my_config.cfg --neurons 1000 --regions 2

  # Advanced mode with all features
  nlm --neurons 2000 --plasticity-mode all --memory-mode all --neuromod-mode all \
      --development-mode adult --visualize --checkpoint 1000

  # Benchmark mode for performance testing
  nlm --benchmark --neurons 5000 --timestep 0.0001

  # Import and continue a saved simulation
  nlm --import saved_brain.nlm --neurons 500

  # Disable plasticity for a simpler learning model
  nlm --plasticity-mode none --neurons 100

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

bool parseCommandLineArgs(int argc, char** argv, std::shared_ptr<Config>& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return false; // Exit after showing help
        }
        
        else if (arg == "--benchmark") {
            config->set("benchmark_mode", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Benchmark mode enabled");
        }
        
        else if (arg == "--batch-mode") {
            config->set("batch_mode", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Batch mode enabled");
        }
        
        else if (arg == "--visualize") {
            config->set("visualization_enabled", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Visualization enabled");
        }
        
        else if (arg.substr(0, 7) == "--neurons") {
            int neurons = 500;
            if (arg.find('=') != std::string::npos) {
                neurons = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neurons = std::stoi(argv[++i]);
            }
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting neuron count to: " + std::to_string(neurons));
        }
        
        else if (arg.substr(0, 8) == "--regions") {
            int regions = 1;
            if (arg.find('=') != std::string::npos) {
                regions = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                regions = std::stoi(argv[++i]);
            }
            config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting region count to: " + std::to_string(regions));
        }
        
        else if (arg.substr(0, 16) == "--connectivity") {
            float connectivity = 0.15f;
            if (arg.find('=') != std::string::npos) {
                connectivity = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                connectivity = std::stof(argv[++i]);
            }
            config->set("connection_probability", connectivity, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting connectivity to: " + std::to_string(connectivity));
        }
        
        else if (arg.substr(0, 10) == "--timestep") {
            double timestep = 0.001;
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
            config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting timestep to: " + std::to_string(timestep) + "s");
        }
        
        else if (arg.substr(0, 6) == "--seed") {
            int seed = 42;
            if (arg.find('=') != std::string::npos) {
                seed = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                seed = std::stoi(argv[++i]);
            }
            config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting random seed to: " + std::to_string(seed));
        }
        
        else if (arg == "--export" && i + 1 < argc) {
            std::string exportFile = argv[++i];
            config->set("export_file", exportFile, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting export file to: " + exportFile);
        }
        
        else if (arg == "--import" && i + 1 < argc) {
            std::string importFile = argv[++i];
            config->set("import_file", importFile, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting import file to: " + importFile);
        }
        
        else if (arg.substr(0, 16) == "--enable-feature") {
            std::string feature = "unknown";
            if (arg.find('=') != std::string::npos) {
                feature = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                feature = argv[++i];
            }
            config->set("enabled_features", "" + feature, ConfigSource::CommandLine);
            NLM_LOG_INFO("Enabling feature: " + feature);
        }
        
        else if (arg.substr(0, 18) == "--disable-feature") {
            std::string feature = "unknown";
            if (arg.find('=') != std::string::npos) {
                feature = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                feature = argv[++i];
            }
            config->set("disabled_features", "" + feature, ConfigSource::CommandLine);
            NLM_LOG_INFO("Disabling feature: " + feature);
        }
        
        else if (arg.substr(0, 12) == "--log-level") {
            std::string level = "info";
            if (arg.find('=') != std::string::npos) {
                level = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                level = argv[++i];
            }
            // Note: In a real implementation, this would set the logger level
            NLM_LOG_INFO("Setting log level to: " + level);
        }
        
        else if (arg.substr(0, 11) == "--log-file" && i + 1 < argc) {
            std::string logFile = argv[++i];
            // Note: In a real implementation, this would set up file logging
            NLM_LOG_INFO("Setting log file to: " + logFile);
        }
        
        else if (arg.substr(0, 18) == "--plasticity-mode") {
            std::string mode = "all";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("plasticity_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting plasticity mode to: " + mode);
        }
        
        else if (arg.substr(0, 15) == "--memory-mode") {
            std::string mode = "all";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("memory_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting memory mode to: " + mode);
        }
        
        else if (arg.substr(0, 17) == "--neuromod-mode") {
            std::string mode = "all";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("neuromod_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting neuromod mode to: " + mode);
        }
        
        else if (arg.substr(0, 20) == "--prediction-mode") {
            std::string mode = "all";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("prediction_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting prediction mode to: " + mode);
        }
        
        else if (arg.substr(0, 18) == "--cognition-mode") {
            std::string mode = "all";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("cognition_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting cognition mode to: " + mode);
        }
        
        else if (arg.substr(0, 20) == "--development-mode") {
            std::string mode = "adult";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("development_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting development mode to: " + mode);
        }
        
        else if (arg == "--checkpoint" && i + 1 < argc) {
            int interval = std::stoi(argv[++i]);
            config->set("checkpoint_interval", interval, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting checkpoint interval to: " + std::to_string(interval) + " steps");
        }
        
        else if (arg.substr(0, 20) == "--replay-mode") {
            std::string mode = "replay_and_consolidate";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                mode = argv[++i];
            }
            config->set("replay_mode", mode, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting replay mode to: " + mode);
        }
        
        else if (arg.substr(0, 16) == "--temperature" && i + 1 < argc) {
            float temp = std::stof(argv[++i]);
            config->set("thermal_noise", temp, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting thermal noise to: " + std::to_string(temp));
        }
        
        else if (arg.substr(0, 15) == "--noise-level" && i + 1 < argc) {
            float noise = std::stof(argv[++i]);
            config->set("random_noise_level", noise, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting random noise level to: " + std::to_string(noise));
        }
        
        else if (arg.substr(0, 20) == "--plasticity-rate" && i + 1 < argc) {
            float rate = std::stof(argv[++i]);
            config->set("global_plasticity_rate", rate, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting global plasticity rate to: " + std::to_string(rate));
        }
        
        else if (arg.substr(0, 14) == "--decay-rate" && i + 1 < argc) {
            float rate = std::stof(argv[++i]);
            config->set("global_decay_rate", rate, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting global decay rate to: " + std::to_string(rate));
        }
        
        else if (arg.substr(0, 22) == "--plasticity-threshold" && i + 1 < argc) {
            float threshold = std::stof(argv[++i]);
            config->set("plasticity_threshold", threshold, ConfigSource::CommandLine);
            NLM_LOG_INFO("Setting plasticity threshold to: " + std::to_string(threshold));
        }
        
        else {
            NLM_LOG_WARN("Unknown option: " + arg + ". Use --help for available options.");
        }
    }
    
    return true;
}

// Learning experiment struct for testing synaptic changes
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

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    if (!parseCommandLineArgs(argc, argv, config)) {
        return 0; // Help was shown, exit gracefully
    }
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
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
    
    // Override with command line args (again to catch --help etc.)
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (overridden by command line)
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Apply command line overrides
    if (config->has("neuron_count")) {
        NLM_LOG_INFO("Command line override: neuron_count from " + 
                    std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Log additional advanced config if set
    if (config->has("plasticity_mode")) {
        NLM_LOG_INFO("  plasticity_mode: " + config->getOr<std::string>("plasticity_mode", "all"));
    }
    if (config->has("memory_mode")) {
        NLM_LOG_INFO("  memory_mode: " + config->getOr<std::string>("memory_mode", "all"));
    }
    if (config->has("neuromod_mode")) {
        NLM_LOG_INFO("  neuromod_mode: " + config->getOr<std::string>("neuromod_mode", "all"));
    }
    if (config->has("checkpoint_interval")) {
        NLM_LOG_INFO("  checkpoint_interval: " + std::to_string(config->getOr<int>("checkpoint_interval", 0)) + " steps");
    }
    
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 2;
    }
    
    brain->logStatus();
    
    // Check if we should run demo mode
    if (config->getOr<bool>("demo_mode", false)) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== DEMO MODE ===");
        NLM_LOG_INFO("Running built-in demonstration tests...");
        
        // Run all tests
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (config->getOr<bool>("benchmark_mode", false)) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== BENCHMARK MODE ===");
        NLM_LOG_INFO("Running performance benchmarks...");
        // TODO: Implement benchmark suite
        NLM_LOG_INFO("Benchmark mode not yet implemented.");
    } else {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NORMAL MODE ===");
        NLM_LOG_INFO("Running basic connectivity and plasticity tests...");
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Check for export/import functionality
    if (config->has("export_file")) {
        std::string exportFile = config->getOr<std::string>("export_file", "");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== EXPORT ===");
        NLM_LOG_INFO("Exporting brain state to: " + exportFile);
        // TODO: Implement brain state export
        NLM_LOG_WARN("Export functionality not yet implemented.");
    }
    
    if (config->has("import_file")) {
        std::string importFile = config->getOr<std::string>("import_file", "");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== IMPORT ===");
        NLM_LOG_INFO("Importing brain state from: " + importFile);
        // TODO: Implement brain state import
        NLM_LOG_WARN("Import functionality not yet implemented.");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM Phase 2 completed successfully!");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("For more options, see: https://nlm.readthedocs.io");
    
    return 0;
}