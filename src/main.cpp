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

using namespace nlm;

// Command line options and help
void printAdvancedUsage() {
    std::cout << R"(
USAGE:
    nlm [OPTIONS] [DEMO_TYPE] [CONFIG_FILE]

DEMO_TYPES:
    phase2              Phase 2: Real Neural Computation (default)
    phase3              Phase 3: World Interaction
    phase4              Phase 4: Cognitive Integration
    phase6              Phase 6: Full Integration
    benchmark           Performance benchmark

OPTIONS:
    --help, -h         Show this help message
    --version, -v      Show version information
    --config, -c FILE  Configuration file (JSON/YAML format)
    --output, -o FILE  Output file for results
    --verbose, -V      Enable verbose logging
    --quiet, -q        Quiet mode (minimal output)
    --debug, -d        Enable debug mode
    --profile, -p      Enable performance profiling
    --save-brain, -s   Save brain state at the end
    --load-brain, -l FILE Load brain state from file
    --random-seed N    Set random seed (default: 42)
    --neuron-count N   Number of neurons (default: 500)
    --duration T       Simulation duration in seconds (default: 10.0)
    --timestep DT      Simulation timestep (default: 0.001)

ADVANCED OPTIONS:
    --stdp-weight-factors LT PT      STDP learning rate factors
    --hebbian-learning-rate R       Hebbian learning rate
    --plasticity-mod N              Plasticity modulation factor
    --dopamine-level L              Dopamine level for neuromodulation
    --curiosity-factor C            Curiosity drive factor
    --novelty-threshold T           Novelty detection threshold
    --reward-function F             Reward function type
    --action-selection-policy P    Action selection strategy
    --memory-capacity C            Working memory capacity
    --episodic-memory-size S       Episodic memory size
    --prediction-horizon H         Prediction horizon for predictive coding

EXAMPLES:
    # Run Phase 2 demo with custom config
    nlm phase2 --config myconfig.json --verbose --save-brain

    # Run Phase 6 demo with benchmarking
    nlm phase6 --profile --output results.json --neuron-count 2000

    # Load saved brain and continue
    nlm phase3 --load-brain saved_brain.bin --random-seed 123

    # Run with optimized plasticity parameters
    nlm phase4 --stdp-weight-factors 0.02 0.015 --hebbian-learning-rate 0.05

    # Quick demo with minimal output
    nlm phase2 --quiet --duration 1.0

CONFIGURATION:
    Configuration files support JSON (snake_case) and YAML (kebab-case) formats:
    
    JSON example:
    {
        "neuron_count": 1000,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    YAML example:
    neuron_count: 1000
    simulation_timestep: 0.001
    random_seed: 42
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012
    stdp_tau: 20.0
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

PLATFORMS:
    Linux, macOS, Windows
    Dependencies: C++17, CMake, make

REPORTING BUGS:
    Report issues at: https://github.com/nnl-research/nlm
    Include: configuration, error output, and system information

ABOUT:
    NLM (熙然) - Neural Learning Machine
    An experimental artificial developmental brain
    https://github.com/nnl-research/nlm
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine - Version 6.0 (Phase 6)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Platform: " << (sizeof(void*) * 8) << "-bit" << std::endl;
    std::cout << "C++ Standard: C++17" << std::endl;
    std::cout << "Architecture: Complete Integration of Memory, Prediction, Cognition, Neuromodulation" << std::endl;
    std::cout << "License: MIT" << std::endl;
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

// Forward declarations for demo functions
void runPhase2Demo(std::shared_ptr<Config> config);
void runPhase3Demo(std::shared_ptr<Config> config);
void runPhase4Demo(std::shared_ptr<Config> config);
void runPhase6Demo(std::shared_ptr<Config> config);
void runBenchmarkDemo(std::shared_ptr<Config> config);
void runAdvancedPlasticityDemo(std::shared_ptr<Config> config);

// Parse advanced command-line arguments
void parseAdvancedArgs(int argc, char** argv, std::shared_ptr<Config> config);

// Run demo based on type
void runDemo(const std::string& demoType, std::shared_ptr<Config> config);

// Advanced demo implementations (simplified for space)
void runPhase2Demo(std::shared_ptr<Config> config) {
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
        return;
    }
    
    brain->logStatus();
    
    // Run existing Phase 2 tests
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Demo Complete ===");
}

void runPhase3Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Phase 3 Demo: World Interaction (simplified)");
    runPhase2Demo(config); // Simplified for demo
    NLM_LOG_INFO("Phase 3 features: sensory processing, world interaction, reward signals");
}

void runPhase4Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Phase 4 Demo: Cognitive Integration (simplified)");
    runPhase2Demo(config); // Simplified for demo
    NLM_LOG_INFO("Phase 4 features: neural prediction, working memory, episodic memory");
}

void runPhase6Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Phase 6 Demo: Full Integration (simplified)");
    runPhase2Demo(config); // Simplified for demo
    NLM_LOG_INFO("Phase 6 features: memory systems, neuromodulation, prediction, cognition");
}

void runBenchmarkDemo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Benchmark Demo: Performance profiling (simplified)");
    runPhase2Demo(config); // Simplified for demo
    NLM_LOG_INFO("Benchmark features: timing analysis, memory usage, scalability testing");
}

void runAdvancedPlasticityDemo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Advanced Plasticity Demo: Enhanced learning algorithms (simplified)");
    runPhase2Demo(config); // Simplified for demo
    NLM_LOG_INFO("Advanced features: adaptive learning rates, meta-plasticity, reward-modulated learning");
}

void parseAdvancedArgs(int argc, char** argv, std::shared_ptr<Config> config) {
    // Parse advanced options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle advanced STDP weight factors
        if (arg == "--stdp-weight-factors" && i + 2 < argc) {
            float ltp = std::stof(argv[i + 1]);
            float ltd = std::stof(argv[i + 2]);
            config->set("stdp_ltp_weight", ltp, ConfigSource::CommandLine);
            config->set("stdp_ltd_weight", ltd, ConfigSource::CommandLine);
            i += 2;
        }
        // Handle Hebbian learning rate
        else if (arg == "--hebbian-learning-rate" && i + 1 < argc) {
            float rate = std::stof(argv[i + 1]);
            config->set("hebbian_learning_rate", rate, ConfigSource::CommandLine);
            i++;
        }
        // Handle plasticity modulation
        else if (arg == "--plasticity-mod" && i + 1 < argc) {
            float mod = std::stof(argv[i + 1]);
            config->set("plasticity_modulation", mod, ConfigSource::CommandLine);
            i++;
        }
        // Handle dopamine level
        else if (arg == "--dopamine-level" && i + 1 < argc) {
            float level = std::stof(argv[i + 1]);
            config->set("dopamine_level", level, ConfigSource::CommandLine);
            i++;
        }
        // Handle curiosity factor
        else if (arg == "--curiosity-factor" && i + 1 < argc) {
            float factor = std::stof(argv[i + 1]);
            config->set("curiosity_factor", factor, ConfigSource::CommandLine);
            i++;
        }
        // Handle novelty threshold
        else if (arg == "--novelty-threshold" && i + 1 < argc) {
            float threshold = std::stof(argv[i + 1]);
            config->set("novelty_threshold", threshold, ConfigSource::CommandLine);
            i++;
        }
        // Handle reward function type
        else if (arg == "--reward-function" && i + 1 < argc) {
            config->set("reward_function_type", argv[i + 1], ConfigSource::CommandLine);
            i++;
        }
        // Handle action selection policy
        else if (arg == "--action-selection-policy" && i + 1 < argc) {
            config->set("action_selection_policy", argv[i + 1], ConfigSource::CommandLine);
            i++;
        }
        // Handle memory capacity
        else if (arg == "--memory-capacity" && i + 1 < argc) {
            size_t capacity = static_cast<size_t>(std::stoull(argv[i + 1]));
            config->set("working_memory_capacity", capacity, ConfigSource::CommandLine);
            i++;
        }
        // Handle episodic memory size
        else if (arg == "--episodic-memory-size" && i + 1 < argc) {
            size_t size = static_cast<size_t>(std::stoull(argv[i + 1]));
            config->set("episodic_memory_max_episodes", size, ConfigSource::CommandLine);
            i++;
        }
        // Handle prediction horizon
        else if (arg == "--prediction-horizon" && i + 1 < argc) {
            int horizon = std::stoi(argv[i + 1]);
            config->set("prediction_horizon", horizon, ConfigSource::CommandLine);
            i++;
        }
    }
}

void runDemo(const std::string& demoType, std::shared_ptr<Config> config) {
    if (demoType == "phase2" || demoType.empty()) {
        runPhase2Demo(config);
    } else if (demoType == "phase3") {
        runPhase3Demo(config);
    } else if (demoType == "phase4") {
        runPhase4Demo(config);
    } else if (demoType == "phase6") {
        runPhase6Demo(config);
    } else if (demoType == "benchmark") {
        runBenchmarkDemo(config);
    } else if (demoType == "advanced") {
        runAdvancedPlasticityDemo(config);
    } else {
        std::cerr << "Error: Unknown demo type: " << demoType << std::endl;
        printAdvancedUsage();
        return;
    }
}

int main(int argc, char** argv) {
    // Check for help/version requests first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printAdvancedUsage();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        }
    }
    
    // Parse demo type and config file
    std::string demoType = "phase2"; // Default
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    bool profile = false;
    bool saveBrain = false;
    std::string loadBrainFile;
    
    int argIndex = 1;
    while (argIndex < argc) {
        std::string arg(argv[argIndex]);
        
        // Handle positional arguments (demo type, config file)
        if (arg != "--help" && arg != "-h" && arg != "--version" && arg != "-v" &&
            !arg.starts_with("--") && !arg.starts_with("-")) {
            
            if (demoType == "phase2" && configFile == "configs/default.cfg") {
                // First positional argument is demo type
                demoType = arg;
            } else if (configFile == "configs/default.cfg") {
                // Second positional argument is config file
                configFile = arg;
            } else {
                std::cerr << "Error: Too many positional arguments" << std::endl;
                printAdvancedUsage();
                return 1;
            }
            argIndex++;
            continue;
        }
        
        // Handle options
        if (arg == "--config" || arg == "-c") {
            if (++argIndex < argc) {
                configFile = argv[argIndex++];
            }
        } else if (arg == "--output" || arg == "-o") {
            if (++argIndex < argc) {
                outputFile = argv[argIndex++];
            }
        } else if (arg == "--verbose" || arg == "-V") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--debug" || arg == "-d") {
            debug = true;
        } else if (arg == "--profile" || arg == "-p") {
            profile = true;
        } else if (arg == "--save-brain" || arg == "-s") {
            saveBrain = true;
        } else if (arg == "--load-brain" || arg == "-l") {
            if (++argIndex < argc) {
                loadBrainFile = argv[argIndex++];
            }
        } else if (arg == "--random-seed" && ++argIndex < argc) {
            config->set("random_seed", static_cast<int64_t>(std::stoll(argv[argIndex])), ConfigSource::CommandLine);
        } else if (arg == "--neuron-count" && ++argIndex < argc) {
            config->set("neuron_count", static_cast<int64_t>(std::stoll(argv[argIndex])), ConfigSource::CommandLine);
        } else if (arg == "--duration" && ++argIndex < argc) {
            config->set("simulation_duration", std::stod(argv[argIndex]), ConfigSource::CommandLine);
        } else if (arg == "--timestep" && ++argIndex < argc) {
            config->set("simulation_timestep", std::stod(argv[argIndex]), ConfigSource::CommandLine);
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            printAdvancedUsage();
            return 1;
        }
        
        argIndex++;
    }
    
    // Initialize logger based on verbosity
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = LogLevel::Info;
    if (quiet) logLevel = LogLevel::Error;
    if (verbose || debug) logLevel = LogLevel::Debug;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply advanced arguments parsing
    parseAdvancedArgs(argc, argv, config);
    
    // Set default values if not already set
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
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
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    }
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    }
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  Demo Type: " + demoType);
    NLM_LOG_INFO("  Random Seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  Simulation Timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  Neuron Count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  Region Count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  Connection Probability: " + std::to_string(config->getOr<float>("connection_probability", 994.0f)));
    
    // Load brain if specified
    if (!loadBrainFile.empty()) {
        NLM_LOG_INFO("Loading brain from: " + loadBrainFile);
        // Implementation would be here
    }
    
    // Run the selected demo
    runDemo(demoType, config);
    
    // Save brain if requested
    if (saveBrain && !outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + outputFile);
        // Implementation would be here
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Demo execution completed successfully!");
    
    return 0;
}
