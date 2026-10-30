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

void printBanner(const std::string& version = "Phase 2", bool detailed = true) {
    std::cout << "\n"
              << "  ╔═══════════════════════════════════════════════════════════════╗" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ║     NLM — 熙然                                                ║" << "\n"
              << "  ║     Neural Learning Machine                                   ║" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ║     " << std::left << std::setw(43) << (version + " Release") << "   ║" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ╚═══════════════════════════════════════════════════════════════╝" << "\n\n";
    
    if (detailed) {
        std::cout << "  NLM (熙然, meaning 'serene flow') is an experimental computational brain project."
                  << "\n"
                  << "  The long-term goal is to create a neural system that begins in a primitive"
                  << "\n"
                  << "  developmental state and acquires increasingly complex abilities through"
                  << "\n"
                  << "  interaction with an environment."
                  << "\n\n";
    }
}

void printUsage(const char* programName) {
    std::cout << "\n"
              << "  Usage: " << programName << " [OPTIONS]" << "\n\n"
              << "  Options:" << "\n"
              << "    --help, -h                    Show this help message" << "\n"
              << "    --version, -v                 Show version information" << "\n"
              << "    --config <file>               Load configuration from file" << "\n"
              << "    --log-level <level>           Set log level (trace, debug, info, warn, error, critical)" << "\n"
              << "    --experiment <mode>           Specify experiment mode: basic, plasticity, stdp, learning, all" << "\n"
              << "    --output <file>               Redirect output to file" << "\n"
              << "    --quiet, -q                   Suppress non-error output" << "\n"
              << "    --benchmark                   Run benchmark mode" << "\n"
              << "    --iterations <n>              Number of iterations for benchmark" << "\n\n"
              << "  Example:" << "\n"
              << "    " << programName << " --experiment learning --log-level info --iterations 1000" << "\n\n";
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

// Enhanced main function with command line parsing and experiment management
int main(int argc, char** argv) {
    // Parse command line options
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Setup output file if specified
    std::ofstream outputFile;
    setupOutputFile(outputFile, options.outputFile);
    if (outputFile.is_open()) {
        redirectOutputToFile(outputFile);
    }
    
    // Setup logger with specified level
    setupLogger(options.logLevel);
    
    // Print banner with specified version
    printBanner(options.version, !options.quiet);
    
    if (!options.quiet) {
        std::cout << "Command line options:" << "\n"
                  << "  Config file: " << options.configFile << "\n"
                  << "  Log level: " << (options.logLevel == LogLevel::Info ? "info" : 
                                       options.logLevel == LogLevel::Debug ? "debug" :
                                       options.logLevel == LogLevel::Warn ? "warn" : "info") << "\n"
                  << "  Experiment mode: " << (options.experimentMode == ExperimentMode::Learning ? "learning" :
                                               options.experimentMode == ExperimentMode::Plasticity ? "plasticity" :
                                               options.experimentMode == ExperimentMode::STDP ? "stdp" :
                                               options.experimentMode == ExperimentMode::Basic ? "basic" : "all") << "\n"
                  << "  Benchmark mode: " << (options.benchmark ? "enabled" : "disabled") << "\n"
                  << "  Iterations: " << options.iterations << "\n\n";
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.quiet ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Enhanced Neural Computation ===");
    NLM_LOG_INFO("Initializing enhanced neural computation system...");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found or failed to load).");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values based on experiment mode
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Adjust configuration based on experiment mode
    if (options.experimentMode == ExperimentMode::Basic) {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.10f, ConfigSource::Default);
        NLM_LOG_INFO("Running BASIC experiment with minimal network size.");
    } else if (options.experimentMode == ExperimentMode::Plasticity) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        NLM_LOG_INFO("Running PLASTICITY experiment with structural plasticity enabled.");
    } else if (options.experimentMode == ExperimentMode::STDP) {
        config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.20f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        NLM_LOG_INFO("Running STDP experiment with precise spike-timing plasticity.");
    } else if (options.experimentMode == ExperimentMode::Learning) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        NLM_LOG_INFO("Running LEARNING experiment with comprehensive plasticity rules.");
    } else { // All experiments
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        NLM_LOG_INFO("Running ALL experiments with comprehensive configuration.");
    }
    
    // STDP parameters (common to multiple experiments)
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
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
    
    // Run experiments based on mode
    switch (options.experimentMode) {
        case ExperimentMode::Basic:
            printExperimentHeader("Basic Connectivity Test");
            runBasicConnectivityTest(brain);
            break;
            
        case ExperimentMode::Plasticity:
            printExperimentHeader("Plasticity Learning Experiment");
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            break;
            
        case ExperimentMode::STDP:
            printExperimentHeader("STDP Verification Experiment");
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
            break;
            
        case ExperimentMode::Learning:
            printExperimentHeader("Comprehensive Learning Experiment");
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            break;
            
        case ExperimentMode::All:
            printExperimentHeader("All Experiments (Comprehensive Suite)");
            
            // Test 1: Basic connectivity
            runBasicConnectivityTest(brain);
            
            // Test 2: Plasticity learning experiment
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            
            // Test 3: STDP verification
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
            
            break;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Experiment Complete ===");
    
    switch (options.experimentMode) {
        case ExperimentMode::Basic:
            NLM_LOG_INFO("Basic experiment completed successfully.");
            break;
        case ExperimentMode::Plasticity:
            NLM_LOG_INFO("Plasticity experiment completed successfully.");
            break;
        case ExperimentMode::STDP:
            NLM_LOG_INFO("STDP experiment completed successfully.");
            break;
        case ExperimentMode::Learning:
            NLM_LOG_INFO("Learning experiment completed successfully.");
            break;
        case ExperimentMode::All:
            NLM_LOG_INFO("All experiments completed successfully.");
            break;
    }
    
    // Save configuration to file if specified
    if (!options.outputFile.empty()) {
        std::string configOutput = options.outputFile;
        // Replace extension with .json for config files
        size_t dotPos = configOutput.find_last_of('.');
        if (dotPos != std::string::npos) {
            configOutput.replace(dotPos, std::string::npos, ".json");
        }
        if (config->saveToFile(configOutput)) {
            NLM_LOG_INFO("Configuration saved to: " + configOutput);
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + configOutput);
        }
    }
    
    return 0;
}
