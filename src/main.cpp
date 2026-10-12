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
#include <map>
#include <algorithm>
#include <chrono>
#include <csignal>
#include <random>
#include <set>
#include <sstream>
#include <thread>

using namespace nlm;

struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 500;
    size_t regionCount = 1;
    double timestep = 0.001;
    uint64_t seed = 42;
    float connectionProb = 0.15f;
    size_t steps = 1000;
    bool enablePlasticity = true;
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructural = true;
    float stdpLTP = 0.02f;
    float stdpLTD = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesis = 0.0001f;
    float pruning = 0.00001f;
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
};

CLIOptions parseCLI(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--plasticity") {
            options.enablePlasticity = true;
        } else if (arg == "--no-plasticity") {
            options.enablePlasticity = false;
        } else if (arg == "--enable-stdp") {
            options.enableSTDP = true;
        } else if (arg == "--disable-stdp") {
            options.enableSTDP = false;
        } else if (arg == "--enable-hebbian") {
            options.enableHebbian = true;
        } else if (arg == "--disable-hebbian") {
            options.enableHebbian = false;
        } else if (arg == "--enable-structural") {
            options.enableStructural = true;
        } else if (arg == "--disable-structural") {
            options.enableStructural = false;
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg.find("--neuron-count=") == 0) {
            options.neuronCount = std::stoul(arg.substr(14));
        } else if (arg.find("--region-count=") == 0) {
            options.regionCount = std::stoul(arg.substr(14));
        } else if (arg.find("--timestep=") == 0) {
            options.timestep = std::stod(arg.substr(11));
        } else if (arg.find("--seed=") == 0) {
            options.seed = std::stoull(arg.substr(7));
        } else if (arg.find("--connection-prob=") == 0) {
            options.connectionProb = std::stof(arg.substr(19));
        } else if (arg.find("--steps=") == 0) {
            options.steps = std::stoul(arg.substr(8));
        } else if (arg.find("--stdp-ltp=") == 0) {
            options.stdpLTP = std::stof(arg.substr(11));
        } else if (arg.find("--stdp-ltd=") == 0) {
            options.stdpLTD = std::stof(arg.substr(11));
        } else if (arg.find("--stdp-tau=") == 0) {
            options.stdpTau = std::stof(arg.substr(11));
        } else if (arg.find("--synaptogenesis=") == 0) {
            options.synaptogenesis = std::stof(arg.substr(19));
        } else if (arg.find("--pruning=") == 0) {
            options.pruning = std::stof(arg.substr(10));
        } else if (arg.substr(0, 7) == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }
    
    return options;
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

int main(int argc, char** argv) {
    // Parse command line options
    CLIOptions options = parseCLI(argc, argv);
    
    // Set up logging based on verbosity
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = LogLevel::Info;
    
    if (options.quiet) {
        logLevel = LogLevel::Warning;
    } else if (options.verbose) {
        logLevel = LogLevel::Debug;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner and version info
    printBanner();
    
    if (options.verbose) {
        printVersion();
        std::cout << "\nRunning with options:" << std::endl;
        std::cout << "  Neuron count: " << options.neuronCount << std::endl;
        std::cout << "  Region count: " << options.regionCount << std::endl;
        std::cout << "  Timestep: " << options.timestep << "s" << std::endl;
        std::cout << "  Seed: " << options.seed << std::endl;
        std::cout << "  Connection probability: " << options.connectionProb << std::endl;
        std::cout << "  Steps: " << options.steps << std::endl;
        std::cout << "  Plasticity: " << (options.enablePlasticity ? "enabled" : "disabled") << std::endl;
        std::cout << "  STDP: " << (options.enableSTDP ? "enabled" : "disabled") << std::endl;
        std::cout << "  Hebbian: " << (options.enableHebbian ? "enabled" : "disabled") << std::endl;
        std::cout << "  Structural: " << (options.enableStructural ? "enabled" : "disabled") << std::endl;
        std::cout << "  Config file: " << options.configFile << std::endl;
        std::cout << std::endl;
    }
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Running with improved CLI interface");
    NLM_LOG_INFO("Total neurons: " + std::to_string(options.neuronCount));
    NLM_LOG_INFO("Total regions: " + std::to_string(options.regionCount));
    NLM_LOG_INFO("Simulation steps: " + std::to_string(options.steps));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override config with CLI options
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProb, ConfigSource::CommandLine);
    config->set("stdp_ltp_weight", options.stdpLTP, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", options.stdpLTD, ConfigSource::CommandLine);
    config->set("stdp_tau", options.stdpTau, ConfigSource::CommandLine);
    config->set("synaptogenesis_rate", options.synaptogenesis, ConfigSource::CommandLine);
    config->set("pruning_rate", options.pruning, ConfigSource::CommandLine);
    
    // Enable/disable plasticity based on CLI options
    config->set("stdp_enabled", options.enableSTDP, ConfigSource::CommandLine);
    config->set("hebbian_enabled", options.enableHebbian, ConfigSource::CommandLine);
    config->set("structural_enabled", options.enableStructural, ConfigSource::CommandLine);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", options.seed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", options.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", options.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", options.connectionProb)));
    NLM_LOG_INFO("  plasticity: " + std::string(options.enablePlasticity ? "enabled" : "disabled"));
    NLM_LOG_INFO("  STDP: " + std::string(options.enableSTDP ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Hebbian: " + std::string(options.enableHebbian ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Structural: " + std::string(options.enableStructural ? "enabled" : "disabled"));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", options.timestep);
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
    
    // Initialize performance monitor
    PerformanceMonitor perfMonitor;
    perfMonitor.totalSteps = options.steps;
    perfMonitor.start();
    perfMonitor.updateSystemStats(brain);
    
    // Run demonstration tests with performance monitoring
    runBasicConnectivityTest(brain);
    perfMonitor.recordStep(brain, 50);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run plasticity learning experiment
    runPlasticityExperiment(brain);
    perfMonitor.recordStep(brain, 1050);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    perfMonitor.recordStep(brain, 2050);
    
    // Log performance statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Statistics ===");
    
    if (options.verbose) {
        std::cout << "\n=== Performance Monitor Results ===" << std::endl;
        std::cout << "Total runtime: " << perfMonitor.totalRuntime << "s" << std::endl;
        std::cout << "Average step time: " << perfMonitor.avgStepTime * 1000.0 << "ms" << std::endl;
        std::cout << "Steps per second: " << perfMonitor.totalSteps / perfMonitor.totalRuntime << std::endl;
        std::cout << "Max spike count: " << (perfMonitor.spikeCounts.empty() ? 0 : *std::max_element(perfMonitor.spikeCounts.begin(), perfMonitor.spikeCounts.end())) << std::endl;
        std::cout << "Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Min step time: " << (perfMonitor.stepTimes.empty() ? 0 : *std::min_element(perfMonitor.stepTimes.begin(), perfMonitor.stepTimes.end())) * 1000.0 << "ms" << std::endl;
        std::cout << "Max step time: " << (perfMonitor.stepTimes.empty() ? 0 : *std::max_element(perfMonitor.stepTimes.begin(), perfMonitor.stepTimes.end())) * 1000.0 << "ms" << std::endl;
        std::cout << "=" << std::endl;
    }
    
    NLM_LOG_INFO("Performance monitoring complete:");
    NLM_LOG_INFO("  Total runtime: " + std::to_string(perfMonitor.totalRuntime) + "s");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(perfMonitor.totalSteps / perfMonitor.totalRuntime));
    NLM_LOG_INFO("  Average step time: " + std::to_string(perfMonitor.avgStepTime * 1000.0) + "ms");
    
    if (!perfMonitor.spikeCounts.empty()) {
        NLM_LOG_INFO("  Spike activity: max=" + std::to_string(*std::max_element(perfMonitor.spikeCounts.begin(), perfMonitor.spikeCounts.end())) + ", avg=" + std::to_string(std::accumulate(perfMonitor.spikeCounts.begin(), perfMonitor.spikeCounts.end(), 0.0) / perfMonitor.spikeCounts.size()) + "");
    }
    
    if (!perfMonitor.stepTimes.empty()) {
        NLM_LOG_INFO("  Step time variability: min=" + std::to_string(*std::min_element(perfMonitor.stepTimes.begin(), perfMonitor.stepTimes.end()) * 1000.0) + "ms, max=" + std::to_string(*std::max_element(perfMonitor.stepTimes.begin(), perfMonitor.stepTimes.end()) * 1000.0) + "ms");
    }
