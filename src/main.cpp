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
    if (!region) {
        NLM_LOG_ERROR("  ERROR: No region 1 found");
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test (found " + 
                    std::to_string(synapses.size()) + ", need 5)");
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
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (Δ=" + 
                    std::to_string(totalDelta) + ")");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters (Δ=" + 
                    std::to_string(totalDelta) + ")");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering (Δ=" + 
                    std::to_string(totalDelta) + ")");
    }
}

void printUsage(const char* programName) {
    std::cout << R"(
NLM - Neural Learning Machine Phase 2
Usage: " << programName << " [options]

Options:
  --config FILE            Load configuration from FILE (default: configs/default.cfg)
  --help, -h               Show this help message and exit
  --version                Show version information and exit

  Testing Options:
    --test1                  Run basic connectivity test only
    --test2                  Run plasticity experiment only
    --test3                  Run STDP verification only
    --test-all              Run all tests (default)

  Output Options:
    --quiet, -q             Minimal output
    --verbose, -v           Detailed output (default)
    --no-banner             Suppress banner
    --save BRIGHTFILE       Save brain state to BRIGHTFILE after simulation
    --load BRIGHTFILE       Load brain state from BRIGHTFILE before simulation

  Advanced Options:
    --neurons N             Number of neurons (default: 500)
    --timestep T            Simulation timestep in seconds (default: 0.001)
    --seed S                Random seed for reproducibility (default: 42)
    --steps STEPS           Number of simulation steps (default: 1000)
    --region-count R        Number of brain regions (default: 1)
    --connection-prob P     Connection probability (default: 0.15)

  Configuration File Options (can be combined with --config):
    --set KEY=VALUE         Override configuration key
    --get KEY               Get configuration value

Examples:
  " << programName << " --test2 --verbose --save=brain.bin
  " << programName << " --test1 --quiet --load=brain.bin
  " << programName << " --set=neuron_count=2000 --set=plasticity_learning_rate=0.005

For more information, visit: https://github.com/yourorg/nlm
)" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    bool showHelp = false;
    bool showVersion = false;
    std::string configFile = "configs/default.cfg";
    std::string saveFile = "";
    std::string loadFile = "";
    bool test1Only = false;
    bool test2Only = false;
    bool test3Only = false;
    bool quietMode = false;
    bool verboseMode = true;
    bool noBanner = false;
    int64_t neuronCount = 500;
    double timestep = 0.001;
    uint64_t seed = 42;
    int steps = 1000;
    int64_t regionCount = 1;
    float connectionProb = 0.15f;
    std::vector<std::string> overrideKeys;
    std::vector<std::string> getKeys;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version") {
            showVersion = true;
        } else if (arg == "--no-banner") {
            noBanner = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
            verboseMode = false;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg == "--test1") {
            test1Only = true;
        } else if (arg == "--test2") {
            test2Only = true;
        } else if (arg == "--test3") {
            test3Only = true;
        } else if (arg == "--test-all") {
            // All tests enabled by default
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--save" && i + 1 < argc) {
            saveFile = argv[++i];
        } else if (arg == "--load" && i + 1 < argc) {
            loadFile = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoll(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            timestep = std::stod(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoi(argv[++i]);
        } else if (arg == "--region-count" && i + 1 < argc) {
            regionCount = std::stoll(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            connectionProb = std::stof(argv[++i]);
        } else if (arg == "--set" && i + 1 < argc) {
            overrideKeys.push_back(argv[++i]);
        } else if (arg == "--get" && i + 1 < argc) {
            getKeys.push_back(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information.\n";
            return 1;
        }
    }
    
    // Handle help/version
    if (showHelp) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (showVersion) {
        std::cout << "NLM Phase 2 - Neural Learning Machine" << std::endl;
        std::cout << "Version: 2.0" << std::endl;
        std::cout << "Built: " << __DATE__ << " " << __TIME__ << std::endl;
        return 0;
    }
    
    if (!noBanner) {
        printBanner();
    }
    
    if (quietMode) {
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    } else {
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
// Override with command line args (legacy --config --set --get support)
    config->loadFromArgs(argc, argv);
    
    // Apply command-line overrides (new style)
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", connectionProb, ConfigSource::CommandLine);
    
    // Apply legacy command-line key-value pairs
    for (const auto& overridePair : overrideKeys) {
        size_t pos = overridePair.find('=');
        if (pos != std::string::npos) {
            std::string key = overridePair.substr(0, pos);
            std::string value = overridePair.substr(pos + 1);
            
            // Simple type conversion based on common types
            if (key.find("int") != std::string::npos || 
                key.find("count") != std::string::npos ||
                key.find("seed") != std::string::npos) {
                try {
                    int64_t intVal = std::stoll(value);
                    config->set(key, intVal, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse int value for key: " + key);
                }
            } else if (key.find("float") != std::string::npos || 
                       key.find("rate") != std::string::npos ||
                       key.find("probability") != std::string::npos ||
                       key.find("weight") != std::string::npos) {
                try {
                    float floatVal = std::stof(value);
                    config->set(key, floatVal, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse float value for key: " + key);
                }
            } else if (key.find("double") != std::string::npos || 
                       key.find("timestep") != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    config->set(key, doubleVal, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse double value for key: " + key);
                }
            } else {
                // Try to parse as string if no specific type found
                config->set(key, value, ConfigSource::CommandLine);
            }
        } else {
            NLM_LOG_WARNING("Invalid key=value pair: " + overridePair);
        }
    }
    
    // Handle --get queries
    for (const auto& getKey : getKeys) {
        if (config->has(getKey)) {
            if (std::is_same<int64_t, decltype(config->getOr<int64_t>(getKey, 0))>::value) {
                std::cout << getKey << " = " << config->getOr<int64_t>(getKey, 0) << std::endl;
            } else if (std::is_same<double, decltype(config->getOr<double>(getKey, 0.0))>::value) {
                std::cout << getKey << " = " << config->getOr<double>(getKey, 0.0) << std::endl;
            } else if (std::is_same<float, decltype(config->getOr<float>(getKey, 0.0f))>::value) {
                std::cout << getKey << " = " << config->getOr<float>(getKey, 0.0f) << std::endl;
            } else {
                std::cout << getKey << " = " << config->getOr<std::string>(getKey, "") << std::endl;
            }
        } else {
            std::cout << getKey << " = (not set)" << std::endl;
        }
    }
    
    // Save configuration if --save specified
    if (!saveFile.empty()) {
        if (config->saveToFile(saveFile)) {
            NLM_LOG_INFO("Configuration saved to: " + saveFile);
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + saveFile);
        }
    }
    
    // Load brain state if --load specified
    if (!loadFile.empty()) {
        if (brain->load(loadFile)) {
            NLM_LOG_INFO("Brain state loaded from: " + loadFile);
        } else {
            NLM_LOG_ERROR("Failed to load brain state from: " + loadFile);
        }
    }
    
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
    
    // Run tests based on command line options
    if (!test1Only && !test2Only && !test3Only) {
        // Run all tests (default)
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
    } else {
        // Run only specified tests
        if (test1Only) {
            runBasicConnectivityTest(brain);
        }
        
        if (test2Only) {
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
        }
        
        if (test3Only) {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        }
    }
    
    // Save brain state if requested
    if (!saveFile.empty()) {
        if (brain->save(saveFile)) {
            NLM_LOG_INFO("Brain state saved to: " + saveFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + saveFile);
        }
    }
}
