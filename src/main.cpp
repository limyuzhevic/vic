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

void printUsage() {
    printBanner();
    std::cout << "Usage: nlm [OPTIONS]\n";
    std::cout << "\nCore Options:\n";
    std::cout << "  --config=<file>      Configuration file to load (default: configs/default.cfg)\n";
    std::cout << "  --help              Print this help message\n";
    std::cout << "  --version           Print version information\n";
    std::cout << "\nBehavior Options:\n";
    std::cout << "  --test=<test>       Run specific test (1, 2, 3, all)\n";
    std::cout << "  --verbose           Enable verbose logging (default)\n";
    std::cout << "  --quiet             Disable logging output\n";
    std::cout << "  --profile           Enable performance profiling\n";
    std::cout << "  --config-output      Output current configuration\n";
    std::cout << "  --no-defaults       Don't apply default Phase 2 configuration\n";
    std::cout << "  --steps=<n>         Number of simulation steps\n";
    std::cout << "\nAdvanced Options:\n";
    std::cout << "  --neuron-count=<n>  Number of neurons (overrides config)\n";
    std::cout << "  --region-count=<n>  Number of regions (overrides config)\n";
    std::cout << "  --seed=<n>          Random seed (overrides config)\n";
    std::cout << "  --save=<file>       Save brain state to file\n";
    std::cout << "  --load=<file>       Load brain state from file\n";
    std::cout << "\nExample:\n";
    std::cout << "  nlm --config=myconfig.cfg --verbose --test=1 --steps=5000\n";
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

void printVersion() {
    std::cout << "NLM Phase 2 - Real Neural Computation (v0.1.0)\n";
    std::cout << "Phase 2: Real LIF neuron dynamics with event-driven computation\n";
    std::cout << "STDP and Hebbian plasticity rules\n";
    std::cout << "Structural plasticity (synaptogenesis/pruning)\n";
}

void parseCommandLineArgs(int argc, char** argv, std::string& configFile, bool& verbose, bool& quiet, 
                         bool& profile, bool& configOutput, bool& noDefaults, size_t& steps,
                         size_t& testMode, std::string& saveFile, std::string& loadFile,
                         size_t& neuronCount, size_t& regionCount, uint64_t& seed) {
    configFile = "configs/default.cfg";
    verbose = true;
    quiet = false;
    profile = false;
    configOutput = false;
    noDefaults = false;
    steps = 0;  // 0 means use defaults
    testMode = 0;  // 0 means run all tests
    saveFile = "";
    loadFile = "";
    neuronCount = 0;  // 0 means use config
    regionCount = 0;  // 0 means use config
    seed = 0;  // 0 means use config
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printUsage();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(arg.find('=') + 1);
        } else if (arg == "--verbose") {
            verbose = true;
            quiet = false;
        } else if (arg == "--quiet") {
            quiet = true;
            verbose = false;
        } else if (arg == "--profile") {
            profile = true;
        } else if (arg == "--config-output") {
            configOutput = true;
        } else if (arg == "--no-defaults") {
            noDefaults = true;
        } else if (arg.find("--steps=") == 0) {
            steps = std::stoull(arg.substr(arg.find('=') + 1));
        } else if (arg.find("--test=") == 0) {
            std::string testArg = arg.substr(arg.find('=') + 1);
            if (testArg == "1") testMode = 1;
            else if (testArg == "2") testMode = 2;
            else if (testArg == "3") testMode = 3;
            else if (testArg == "all") testMode = 99;
        } else if (arg.find("--save=") == 0) {
            saveFile = arg.substr(arg.find('=') + 1);
        } else if (arg.find("--load=") == 0) {
            loadFile = arg.substr(arg.find('=') + 1);
        } else if (arg.find("--neuron-count=") == 0) {
            neuronCount = std::stoull(arg.substr(arg.find('=') + 1));
        } else if (arg.find("--region-count=") == 0) {
            regionCount = std::stoull(arg.substr(arg.find('=') + 1));
        } else if (arg.find("--seed=") == 0) {
            seed = std::stoull(arg.substr(arg.find('=') + 1));
        }
    }
}

int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool verbose = true;
    bool quiet = false;
    bool profile = false;
    bool configOutput = false;
    bool noDefaults = false;
    size_t steps = 0;
    size_t testMode = 0;
    std::string saveFile = "";
    std::string loadFile = "";
    size_t neuronCount = 0;
    size_t regionCount = 0;
    uint64_t seed = 0;
    
    parseCommandLineArgs(argc, argv, configFile, verbose, quiet, profile, configOutput, noDefaults,
                         steps, testMode, saveFile, loadFile, neuronCount, regionCount, seed);
    
    // If help or version requested, exit here
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--help" || arg == "--version") {
                return 0;
            }
        }
    }
    
    // Set logger level based on verbosity
    if (quiet) {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (!verbose) {
        consoleLogger->setLevel(LogLevel::Warning);
    }
    
    // Profile timing if requested
    auto programStart = std::chrono::high_resolution_clock::now();
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
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
    
    // Apply overrides from command line if specified
    if (seed != 0) {
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    }
    if (neuronCount != 0) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    }
    if (regionCount != 0) {
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    }
    
    // Apply default values for Phase 2 (unless --no-defaults specified)
    if (!noDefaults) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
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
    }
    
    // Output configuration if requested
    if (configOutput) {
        NLM_LOG_INFO("=== Current Configuration ===");
        if (config->has("random_seed")) {
            NLM_LOG_INFO("random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        }
        if (config->has("simulation_timestep")) {
            NLM_LOG_INFO("simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        }
        if (config->has("neuron_count")) {
            NLM_LOG_INFO("neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        }
        if (config->has("region_count")) {
            NLM_LOG_INFO("region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        }
        if (config->has("connection_probability")) {
            NLM_LOG_INFO("connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        }
        NLM_LOG_INFO("===========================");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
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
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply overrides from command line if specified
    if (seed != 0) {
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    }
    if (neuronCount != 0) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    }
    if (regionCount != 0) {
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    }
    
    // Apply default values for Phase 2 (unless --no-defaults specified)
    if (!noDefaults) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
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
    
    // Save state if requested
    if (!saveFile.empty()) {
        if (brain->save(saveFile)) {
            NLM_LOG_INFO("Brain state saved to: " + saveFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + saveFile);
        }
    }
    
    // Load state if requested
    if (!loadFile.empty()) {
        if (brain->load(loadFile)) {
            NLM_LOG_INFO("Brain state loaded from: " + loadFile);
        } else {
            NLM_LOG_ERROR("Failed to load brain state from: " + loadFile);
        }
    }
    
    // Run specified tests based on testMode
    if (testMode == 0) {
        // Run all tests (default behavior)
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
        
    } else if (testMode == 1) {
        // Run only Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
    } else if (testMode == 2) {
        // Run only Test 2: Plasticity learning experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
    } else if (testMode == 3) {
        // Run only Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (testMode == 99) {
        // Run all tests with additional profiling
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        NLM_LOG_INFO("All tests completed with profiling enabled");
    }
    
    auto programEnd = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(programEnd - programStart).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Summary ===");
    NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime) + "s");
    if (profile) {
        NLM_LOG_INFO("Profiling data would be available in full version");
    }
    
    if (testMode != 99) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Mode " + std::to_string(testMode) + " Complete ===");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    
    return 0;
}
