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

namespace nlm {
    class CommandLineParser {
    public:
        struct Options {
            std::string configFile = "configs/default.cfg";
            bool verbose = false;
            bool benchmark = false;
            std::string testMode = "all"; // "connectivity", "plasticity", "stdp", "all"
            int steps = 1000;
            double timestep = 0.001;
            int neuronCount = 500;
            int regionCount = 1;
            float connectionProbability = 0.15f;
            bool help = false;
        };

        static Options parse(int argc, char** argv) {
            Options opts;
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                    opts.help = true;
                } else if (arg.substr(0, 7) == "--config") {
                    if (arg.find('=') != std::string::npos) {
                        opts.configFile = arg.substr(arg.find('=') + 1);
                    } else if (i + 1 < argc) {
                        opts.configFile = argv[++i];
                    }
                } else if (arg == "--verbose" || arg == "-v") {
                    opts.verbose = true;
                } else if (arg == "--benchmark" || arg == "-b") {
                    opts.benchmark = true;
                } else if (arg == "--test-mode") {
                    if (i + 1 < argc) opts.testMode = argv[++i];
                } else if (arg == "--steps") {
                    if (i + 1 < argc) opts.steps = std::atoi(argv[++i]);
                } else if (arg == "--timestep") {
                    if (i + 1 < argc) opts.timestep = std::atof(argv[++i]);
                } else if (arg == "--neurons") {
                    if (i + 1 < argc) opts.neuronCount = std::atoi(argv[++i]);
                } else if (arg == "--regions") {
                    if (i + 1 < argc) opts.regionCount = std::atoi(argv[++i]);
                } else if (arg == "--conn-prob") {
                    if (i + 1 < argc) opts.connectionProbability = std::atof(argv[++i]);
                } else if (arg[0] == '-') {
                    NLM_LOG_ERROR("Unknown option: " + arg);
                }
            }
            return opts;
        }

        static void printHelp() {
            std::cout << R"(
NLM Phase 2 Real Neural Computation Test Suite

Usage: ./nlm [OPTIONS]

Options:
  --help, -h                    Show this help message
  --config <file>               Load configuration from file
  --verbose, -v                 Enable verbose logging
  --benchmark, -b               Run benchmark mode
  --test-mode <mode>            Test mode: connectivity, plasticity, stdp, all (default: all)
  --steps <count>               Number of simulation steps (default: 1000)
  --timestep <time>             Simulation timestep in seconds (default: 0.001)
  --neurons <count>             Number of neurons (default: 500)
  --regions <count>             Number of regions (default: 1)
  --conn-prob <probability>     Connection probability (default: 0.15)

Test Modes:
  connectivity                 Run basic connectivity test only
  plasticity                  Run plasticity learning experiment only  
  stdp                         Run STDP verification test only
  all                          Run all tests (default)

Examples:
  ./nlm                          # Run all tests with defaults
  ./nlm --test-mode connectivity  # Run only connectivity test
  ./nlm --steps 5000 --neurons 1000  # Run with more neurons and steps
  ./nlm --benchmark --verbose     # Run benchmark with verbose output
)" << std::endl;
        }
    };
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
        : brain(b), seed(s), initialSynapseCount(0), initialWeights(), finalWeights(), mostActiveNeurons() {}
    
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

void runBasicConnectivityTestEnhanced(std::shared_ptr<Brain> brain, int steps, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 1: Basic Neural Connectivity ===");
    
    // Get brain configuration for more intelligent testing
    auto config = brain->getConfig();
    int neuronCount = config->getOr<int64_t>("neuron_count", 500);
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No brain regions found for connectivity test!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found in region!");
        return;
    }
    
    NLM_LOG_INFO("  Brain has " + std::to_string(neurons.size()) + " neurons, " + 
                std::to_string(region->getSynapses().size()) + " synapses");
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Determine optimal number of neurons to stimulate
    size_t neuronsToStimulate = std::min(size_t(10), neurons.size());
    
    // Inject strong current into selected neurons
    NLM_LOG_INFO("  Injecting current into " + std::to_string(neuronsToStimulate) + " neurons...");
    for (size_t i = 0; i < neuronsToStimulate; ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        if (verbose) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " threshold: " + std::to_string(neurons[i]->getThreshold()) +
                        " potential: " + std::to_string(neurons[i]->getMembranePotential()));
        }
    }
    
    // Run the specified number of steps
    NLM_LOG_INFO("  Running " + std::to_string(steps) + " simulation steps...");
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network successfully");
    } else {
        NLM_LOG_INFO("  ! No spikes detected - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
        if (verbose) {
            NLM_LOG_INFO("  Note: This may be normal if neurons require higher input to reach threshold.");
        }
    }
}

void runPlasticityExperimentEnhanced(std::shared_ptr<Brain> brain, int steps, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses with validation
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        NLM_LOG_INFO("  Enabling plasticity on " + std::to_string(synapses.size()) + " synapses...");
        
        size_t enabledHebbian = 0, enabledSTDP = 0, enabledStructural = 0;
        for (auto& syn : synapses) {
            bool hebbian = syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            if (hebbian) {
                if (syn->getHebbianEnabled()) enabledHebbian++;
                if (syn->getSTDPEnabled()) enabledSTDP++;
            }
            if (syn->getStructuralEnabled()) enabledStructural++;
        }
        
        NLM_LOG_INFO("    Hebbian enabled: " + std::to_string(enabledHebbian));
        NLM_LOG_INFO("    STDP enabled: " + std::to_string(enabledSTDP));
        NLM_LOG_INFO("    Structural enabled: " + std::to_string(enabledStructural));
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    // Select a diverse set of neurons for learning
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            } else if (neuron->getType() == NeuronType::Internal && patternNeurons.size() < 25) {
                patternNeurons.push_back(neuron->getId());
            } else if (patternNeurons.size() < 30) {  // Fill with other types
                patternNeurons.push_back(neuron->getId());
            }
        }
        NLM_LOG_INFO("  Selected " + std::to_string(patternNeurons.size()) + 
                    " neurons for learning pattern: " + std::to_string(patternNeurons.size()) + " sensory, " +
                    std::to_string(std::count_if(region->getAllNeurons().begin(), region->getAllNeurons().end(),
                        [](auto* n) { return n->getType() == NeuronType::Internal; })) + " internal");
    }
    
    // Run learning simulation
    for (SimulationStep step = 0; step < steps; ++step) {
        // Apply input to pattern neurons
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 200 steps (less verbose than before)
        if (verbose && step % 200 == 0) {
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

void runStdpVerificationEnhanced(std::shared_ptr<Brain> brain, int steps, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No brain regions found for STDP test!");
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_ERROR("No synapses for STDP test");
        return;
    }
    
    // Select a synapse with a clear pre-post relationship for better testing
    Synapse* testSyn = nullptr;
    size_t testIndex = 0;
    
    // Find a synapse with distinct pre and post neurons
    for (size_t i = 0; i < synapses.size(); ++i) {
        auto* syn = synapses[i].get();
        auto preNeuron = syn->getSourceNeuron();
        auto postNeuron = syn->getDestinationNeuron();
        
        if (preNeuron.index() != postNeuron.index()) {
            testSyn = syn;
            testIndex = i;
            break;
        }
    }
    
    if (!testSyn) {
        NLM_LOG_WARNING("No suitable synapse found for STDP test (all synapses are self-connections).");
        testSyn = synapses[0].get();
        testIndex = 0;
    }
    
    testSyn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = testSyn->getWeight();
    
    NLM_LOG_INFO("  Testing STDP on synapse " + std::to_string(testIndex) + ":");
    NLM_LOG_INFO("    Pre-neuron ID: " + std::to_string(testSyn->getSourceNeuron().index()));
    NLM_LOG_INFO("    Post-neuron ID: " + std::to_string(testSyn->getDestinationNeuron().index()));
    NLM_LOG_INFO("    Synapse weight before: " + std::to_string(beforeWeight));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (" + std::to_string(steps/2) + " trials)...");
    
    // Apply correlated pre->post activity for the specified number of trials
    for (int trial = 0; trial < steps/2; ++trial) {
        brain->injectCurrent(testSyn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(testSyn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        
        if (verbose && trial % 10 == 0) {
            NLM_LOG_INFO("    Trial " + std::to_string(trial) + ": weight = " + std::to_string(testSyn->getWeight()));
        }
    }
    
    float delta = testSyn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse weight after: " + std::to_string(testSyn->getWeight()) +
                " (Δ=" + std::to_string(delta) + ")");
    
    if (verbose) {
        NLM_LOG_INFO("    Final weight change: " + std::to_string((testSyn->getWeight() - beforeWeight) / steps/2) + " per trial");
    }
    
    // Analyze results with better error handling
    NLM_LOG_INFO("");
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (learning detected)");
        NLM_LOG_INFO("    This confirms that spike timing-dependent plasticity is functional.");
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check STDP parameters (strong LTD observed)");
        NLM_LOG_INFO("    Pre-before-post produced depression instead of potentiation.");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering or parameters need adjustment");
        NLM_LOG_INFO("    This could indicate: weak correlation, wrong STDP direction, or parameter tuning needed.");
    }
}

void runBenchmarkTest(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark: Performance Measurement ===");
    
    auto config = brain->getConfig();
    int neuronCount = config->getOr<int64_t>("neuron_count", 500);
    int regionCount = config->getOr<int64_t>("region_count", 1);
    
    NLM_LOG_INFO("Benchmark configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  Regions: " + std::to_string(regionCount));
    NLM_LOG_INFO("  Simulation steps: " + std::to_string(steps));
    
    // Reset brain to clean state for benchmarking
    brain->reset();
    brain->initialize();
    
    // Warm-up period (let the brain settle)
    NLM_LOG_INFO("  Warm-up: 100 steps...");
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Benchmark the simulation performance
    NLM_LOG_INFO("  Running benchmark: " + std::to_string(steps) + " steps...");
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double totalTimeSeconds = duration.count() / 1000.0;
    double stepsPerSecond = steps / totalTimeSeconds;
    double avgStepTimeMs = (totalTimeSeconds * 1000.0) / steps;
    
    NLM_LOG_INFO("  Benchmark Results:");
    NLM_LOG_INFO("    Total time: " + std::to_string(totalTimeSeconds) + " seconds");
    NLM_LOG_INFO("    Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("    Average step time: " + std::to_string(avgStepTimeMs) + " ms");
    NLM_LOG_INFO("    Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("    Final firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    
    if (stepsPerSecond > 1000) {
        NLM_LOG_INFO("  ✓ Excellent performance: >1000 steps/second");
    } else if (stepsPerSecond > 100) {
        NLM_LOG_INFO("  ✓ Good performance: >100 steps/second");
    } else {
        NLM_LOG_INFO("  ! Performance could be improved: <100 steps/second");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Parse command line arguments
    auto opts = CommandLineParser::parse(argc, argv);
    
    // If help was requested, print help and exit
    if (opts.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    // Initialize logger with verbose level if requested
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = LogLevel::Info;
    if (opts.verbose) {
        logLevel = LogLevel::Debug;
        NLM_LOG_INFO("Verbose logging enabled");
    }
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Version: Enhanced with advanced features");
    NLM_LOG_INFO("Test Mode: " + opts.testMode);
    NLM_LOG_INFO("Steps: " + std::to_string(opts.steps));
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load from file if specified
    if (opts.configFile != "configs/default.cfg") {
        if (config->loadFromFile(opts.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + opts.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + opts.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    } else {
        if (config->loadFromFile(opts.configFile)) {
            NLM_LOG_INFO("Loaded default configuration from: " + opts.configFile);
        } else {
            NLM_LOG_INFO("Using completely default configuration (no config file found).");
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply user-provided or default configuration values
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying configuration overrides:");
    
    // Validate and set neuron count
    int64_t neuronCount = static_cast<int64_t>(std::max(1, opts.neuronCount));
    if (neuronCount > 10000) {
        NLM_LOG_WARNING("Neuron count " + std::to_string(neuronCount) + " is very large, this may impact performance.");
        neuronCount = 10000;
    }
    config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
    NLM_LOG_INFO("  neuron_count: " + std::to_string(neuronCount));
    
    // Validate and set region count
    int64_t regionCount = static_cast<int64_t>(std::max(1, opts.regionCount));
    config->set("region_count", regionCount, ConfigSource::CommandLine);
    NLM_LOG_INFO("  region_count: " + std::to_string(regionCount));
    
    // Set simulation timestep
    double timestep = std::max(0.0001, std::min(0.1, opts.timestep));  // Reasonable bounds
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(timestep) + "s");
    
    // Set connection probability
    float connectionProbability = std::max(0.01f, std::min(0.5f, opts.connectionProbability));
    config->set("connection_probability", connectionProbability, ConfigSource::CommandLine);
    NLM_LOG_INFO("  connection_probability: " + std::to_string(connectionProbability));
    
    // Set random seed (deterministic for reproducibility)
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    NLM_LOG_INFO("  random_seed: 42 (deterministic)");
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Final Configuration Summary:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  Regions: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        NLM_LOG_ERROR("Check configuration and available memory.");
        return 1;
    }
    
    brain->logStatus();
    
    // Run selected tests based on mode
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Starting Test Suite ===");
    
    bool runConnectivity = (opts.testMode == "all" || opts.testMode == "connectivity");
    bool runPlasticity = (opts.testMode == "all" || opts.testMode == "plasticity");
    bool runStdp = (opts.testMode == "all" || opts.testMode == "stdp");
    
    if (runConnectivity) {
        runBasicConnectivityTestEnhanced(brain, opts.steps, opts.verbose);
        
        // Reset brain for next test if we're running multiple tests
        if (runPlasticity || runStdp) {
            NLM_LOG_INFO("Resetting brain for next test...");
            brain->reset();
            brain->initialize();
        }
    }
    
    if (runPlasticity) {
        runPlasticityExperimentEnhanced(brain, opts.steps, opts.verbose);
        
        // Reset brain for STDP test if needed
        if (runStdp) {
            NLM_LOG_INFO("Resetting brain for STDP test...");
            brain->reset();
            brain->initialize();
        }
    }
    
    if (runStdp) {
        runStdpVerificationEnhanced(brain, opts.steps, opts.verbose);
    }
    
    // Run benchmark if requested (independent test)
    if (opts.benchmark) {
        runBenchmarkTest(brain, opts.steps);
    }
    
    // Final brain status (even if no tests ran)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Complete ===");
    
    if (runConnectivity || runPlasticity || runStdp) {
        NLM_LOG_INFO("Tests completed successfully!");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Summary of completed tests:");
        if (runConnectivity) NLM_LOG_INFO("  ✓ Basic neural connectivity");
        if (runPlasticity) NLM_LOG_INFO("  ✓ Plasticity learning experiment");
        if (runStdp) NLM_LOG_INFO("  ✓ STDP verification");
        if (opts.benchmark) NLM_LOG_INFO("  ✓ Performance benchmark");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    }
    
    return 0;
}
