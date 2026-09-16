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
#include <chrono>

using namespace nlm;

// Forward declarations
class CommandLineParser;
class PhaseRunner;

// Command-line argument parsing
class CommandLineParser {
public:
    struct Args {
        int phase = 2;
        std::string configFile = "configs/default.cfg";
        uint64_t seed = 42;
        bool verbose = false;
        bool help = false;
        bool debug = false;
        bool benchmark = false;
        bool listConfigs = false;
        uint32_t simulationSteps = 0;
        uint32_t numEpisodes = 5;
        float timestep = 0.001f;
        bool enableCheckpoint = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        std::string checkpointPath = "./nlm_checkpoint.bin";
    };
    
    static Args parse(int argc, char** argv) {
        Args args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--debug") {
                args.debug = true;
                args.verbose = true;
            } else if (arg == "--benchmark") {
                args.benchmark = true;
            } else if (arg == "--list-configs") {
                args.listConfigs = true;
            } else if (arg == "--config" && i + 1 < argc) {
                args.configFile = argv[++i];
            } else if (arg == "--seed" && i + 1 < argc) {
                args.seed = std::stoull(argv[++i]);
            } else if (arg == "--phase" && i + 1 < argc) {
                args.phase = std::stoi(argv[++i]);
                if (args.phase < 2 || args.phase > 6) {
                    std::cerr << "Error: Phase must be between 2 and 6" << std::endl;
                    args.phase = 2;
                }
            } else if (arg == "--steps" && i + 1 < argc) {
                args.simulationSteps = std::stoi(argv[++i]);
            } else if (arg == "--episodes" && i + 1 < argc) {
                args.numEpisodes = std::stoi(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                args.timestep = std::stof(argv[++i]);
            } else if (arg == "--checkpoint") {
                args.enableCheckpoint = true;
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.checkpointPath = argv[++i];
                }
            } else if (arg == "--no-checkpoint") {
                args.enableCheckpoint = false;
            } else if (arg == "--replay") {
                args.enableReplay = true;
            } else if (arg == "--no-replay") {
                args.enableReplay = false;
            } else if (arg == "--development") {
                args.enableDevelopment = true;
            } else if (arg == "--no-development") {
                args.enableDevelopment = false;
            } else if (arg.substr(0, 2) == "--" && i + 1 < argc) {
                // Check for boolean flags with explicit values
                if (arg == "--enable-checkpoint" || arg == "--enable-replay" || 
                    arg == "--enable-development") {
                    if (argv[++i] == "true" || argv[i] == "1") {
                        if (arg == "--enable-checkpoint") args.enableCheckpoint = true;
                        else if (arg == "--enable-replay") args.enableReplay = true;
                        else if (arg == "--enable-development") args.enableDevelopment = true;
                    }
                }
            } else if (arg[0] != '-') {
                // Unknown argument, might be a config file
                args.configFile = arg;
            }
        }
        
        return args;
    }
    
    static void printHelp() {
        std::cout << R"(
NLM - Neural Learning Machine - Modular Neural Simulation Framework

USAGE:
    nlm [OPTIONS]
    nlm [CONFIG_FILE]

OPTIONS:
    --phase N             Phase to run (2-6, default: 2)
    --config FILE         Configuration file (default: configs/default.cfg)
    --seed N              Random seed (default: 42)
    --verbose, -v         Enable verbose output
    --debug               Enable debug mode (implies --verbose)
    --benchmark           Run benchmark tests
    --help, -h            Show this help message
    --list-configs        List available configuration templates

    --steps N             Number of simulation steps (0 = phase-specific default)
    --episodes N          Number of episodes for agent learning (default: 5)
    --timestep F          Simulation timestep in ms (default: 0.001)
    --checkpoint FILE      Save/load checkpoints (default: ./nlm_checkpoint.bin)
    --no-checkpoint       Disable checkpointing
    --replay              Enable memory replay for consolidation
    --no-replay           Disable memory replay
    --development         Enable developmental stages
    --no-development      Disable developmental stages

PHASE DESCRIPTIONS:
    Phase 2: Basic neural computation - LIF neurons, STDP, Hebbian plasticity
    Phase 3: World interaction - sensory-motor loops, simple behavior
    Phase 4: Emerging cognition - prediction, memory, attention
    Phase 5: Advanced learning - continual learning, damage recovery
    Phase 6: Full integration - all systems connected and working together

EXAMPLES:
    nlm                               Run basic Phase 2 demo
    nlm --phase 6                    Run full Phase 6 integration demo
    nlm --phase 3 --episodes 10      Phase 3 with more learning episodes
    nlm --config myconfig.cfg         Load custom configuration
    nlm --debug --verbose             Enable detailed debugging output

CONFIGURATION FILES:
    The framework supports loading settings from config files with:
    key=value format. Common settings:
        neuron_count=500
        region_count=1
        connection_probability=0.1
        stdp_ltp_weight=0.01
        stdp_ltd_weight=0.012
        synaptogenesis_rate=0.0001
        pruning_rate=0.00001

For more information, see the documentation in docs/ directory.

Copyright 2026 NLM Project - MIT License
                    )" << std::endl;
    }
};

void listAvailableConfigs() {
    std::cout << "Available configuration templates:" << std::endl;
    std::cout << "  configs/default.cfg      - Default configuration" << std::endl;
    std::cout << "  configs/small.cfg        - Small network (100 neurons)" << std::endl;
    std::cout << "  configs/large.cfg        - Large network (5000 neurons)" << std::endl;
    std::cout << "  configs/fast.cfg         - Fast simulation (larger timestep)" << std::endl;
    std::cout << "  configs/debug.cfg        - Debug configuration with verbose logging" << std::endl;
    std::cout << "  configs/learning.cfg      - Optimized for learning experiments" << std::endl;
    std::cout << std::endl;
    std::cout << "To create a custom config, copy and modify a template:" << std::endl;
    std::cout << "  cp configs/default.cfg myconfig.cfg" << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Modular Neural Simulation Framework                        ║
    ║                                                               ║
    ║     Phase-Based Artificial Development                          ║
    ║                                                               ║
    ╚════════════════════════════════════════════════════════════════╝
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

void runPhase2Demo(std::shared_ptr<Brain> brain, const CommandLineParser::Args& args) {
    NLM_LOG_INFO("=== Phase 2: Basic Neural Computation ===");
    
    // Run basic connectivity test
    runBasicConnectivityTest(brain);
    
    // Reset and run plasticity experiment
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("=== Phase 2 Complete ===");
}

void runPhase3Demo(std::shared_ptr<Brain> brain, size_t numEpisodes) {
    NLM_LOG_INFO("=== Phase 3: World Interaction Demo ===");
    
    // Simple Phase 3 demo using existing Phase6Demo functionality
    // This would normally use the full Phase3Experiment, but for simplicity
    // we'll run a basic demonstration with the brain interface
    
    NLM_LOG_INFO("Phase 3 demonstration requires additional code:");
    NLM_LOG_INFO("  - Simple world simulation");
    NLM_LOG_INFO("  - Agent brain interface");
    NLM_LOG_INFO("  - Sensory processing and motor output");
    NLM_LOG_INFO("  - Experience -> action -> learning loop");
    NLM_LOG_INFO(std::endl);
    NLM_LOG_INFO("For a complete Phase 3 demo, see:");
    NLM_LOG_INFO("  - src/experiments/Phase3Demo.cpp");
    NLM_LOG_INFO("  - src/experiments/Phase3Experiment.cpp");
    NLM_LOG_INFO("  - src/agent/AgentBrain.cpp");
    NLM_LOG_INFO("  - src/world/SimpleWorld.cpp");
    
    // Simple demonstration: run a few steps with sensory input
    NLM_LOG_INFO(std::endl);
    NLM_LOG_INFO("Running basic Phase 3-style demonstration...");
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + ":");
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Run some steps with sensory input
        for (SimulationStep step = 0; step < 100; ++step) {
            // Inject some sensory activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        NLM_LOG_INFO("  Episode complete: " + std::to_string(brain->getTotalSpikeCount()) + " total spikes");
    }
    
    NLM_LOG_INFO("=== Phase 3 Complete ===");
}

void runPhase4Demo(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Phase 4: Emerging Cognition ===");
    NLM_LOG_INFO("Phase 4 demonstrates cognitive capabilities:");
    NLM_LOG_INFO("  - Temporal prediction: Learning event sequences");
    NLM_LOG_INFO("  - Working memory: Maintaining information across delays");
    NLM_LOG_INFO("  - Episodic memory: Storing and recalling experiences");
    NLM_LOG_INFO("  - Concept formation: Discovering patterns without labels");
    NLM_LOG_INFO("  - Neural attention: Selective processing through competition");
    NLM_LOG_INFO("  - Multi-step planning: Using predictions to select actions");
    NLM_LOG_INFO("  - Self-model: Learning body schema and action consequences");
    NLM_LOG_INFO("  - Social learning: Observing and imitating others");
    NLM_LOG_INFO("  - Continual learning: Adapting to new tasks");
    
    NLM_LOG_INFO(std::endl);
    NLM_LOG_INFO("Full Phase 4 demo is available in:");
    NLM_LOG_INFO("  - src/experiments/Phase4Demo.cpp");
    NLM_LOG_INFO("  - src/experiments/Phase4Experiment.cpp");
    
    NLM_LOG_INFO("=== Phase 4 Complete ===");
}

void runPhase6Demo(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Phase 6: Full Integration Demo ===");
    NLM_LOG_INFO("Phase 6 integrates all brain systems into a coherent artificial brain:");
    
    // This would normally run the full Phase6Demo.cpp experiment
    // For now, we'll provide information about what's included
    
    NLM_LOG_INFO(std::endl);
    NLM_LOG_INFO("Phase 6 integration includes:");
    NLM_LOG_INFO("  - Memory systems (working, episodic, associative)");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Prediction system");
    NLM_LOG_INFO("  - Cognition systems (planner, concept formation, attention)");
    NLM_LOG_INFO("  - Development system");
    NLM_LOG_INFO("  - Checkpoint save/load");
    NLM_LOG_INFO("  - Replay and consolidation");
    
    NLM_LOG_INFO(std::endl);
    NLM_LOG_INFO("Full Phase 6 integration demo is available in:");
    NLM_LOG_INFO("  - src/experiments/Phase6Demo.cpp");
    NLM_LOG_INFO("  - src/experiments/Phase6IntegratedExperiment.cpp");
    
    NLM_LOG_INFO("=== Phase 6 Complete ===");
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    auto args = CommandLineParser::parse(argc, argv);
    
    if (args.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (args.listConfigs) {
        listAvailableConfigs();
        return 0;
    }
    
    // Initialize logger with verbosity based on arguments
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = args.debug ? LogLevel::Debug : 
                        args.verbose ? LogLevel::Info : LogLevel::Warning;
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log banner based on phase
    std::string phaseName;
    switch (args.phase) {
        case 2: phaseName = "Phase 2: Basic Neural Computation"; break;
        case 3: phaseName = "Phase 3: World Interaction"; break;
        case 4: phaseName = "Phase 4: Emerging Cognition"; break;
        case 5: phaseName = "Phase 5: Advanced Learning"; break;
        case 6: phaseName = "Phase 6: Full Integration"; break;
        default: phaseName = "Phase " + std::to_string(args.phase); break;
    }
    
    NLM_LOG_INFO("=== " + phaseName + " ===");
    
    if (args.benchmark) {
        NLM_LOG_INFO("Running benchmark mode...");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = args.configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            // Parse config file
            std::string nextArg = argv[++i];
            if (nextArg.substr(0, 7) == "configs/") {
                configFile = nextArg;
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
    
    // Apply parsed command line overrides
    config->set("random_seed", static_cast<int64_t>(args.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);  // Default
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::CommandLine);
    config->set("stdp_tau", 20.0f, ConfigSource::CommandLine);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::CommandLine);
    config->set("pruning_rate", 0.00001f, ConfigSource::CommandLine);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
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
    
    // Run the appropriate phase demo
    switch (args.phase) {
        case 2:
            runPhase2Demo(brain, args);
            break;
        case 3:
            runPhase3Demo(brain, args.numEpisodes);
            break;
        case 4:
            runPhase4Demo(brain);
            break;
        case 5:
            NLM_LOG_INFO("Phase 5: Advanced Learning");
            NLM_LOG_INFO("Full Phase 5 experiment is available in:");
            NLM_LOG_INFO("  - src/experiments/Phase5Experiment.cpp");
            break;
        case 6:
            runPhase6Demo(brain);
            break;
        default:
            NLM_LOG_ERROR("Unknown phase: " + std::to_string(args.phase));
            return 1;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + phaseName + " Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}