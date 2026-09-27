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
#include "debug/AdvancedCLI.hpp"
#include "debug/DebugManager.hpp"
#include "debug/Profiler.hpp"
#include "debug/Visualizer.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>

using namespace nlm;

// Enhanced logging configuration
void setupEnhancedLogging(const nlm::AdvancedCLIOptions& options) {
    auto logger = nlm::Logger::getGlobal();
    
    // Set log level
    nlm::LogLevel logLevel;
    if (options.logging.logLevel == "debug") {
        logLevel = nlm::LogLevel::Debug;
    } else if (options.logging.logLevel == "warning") {
        logLevel = nlm::LogLevel::Warning;
    } else if (options.logging.logLevel == "error") {
        logLevel = nlm::LogLevel::Error;
    } else {
        logLevel = nlm::LogLevel::Info; // default
    }
    
    logger->setLevel(logLevel);
    
    // Add file logger if specified
    if (options.logging.logFile) {
        auto fileLogger = std::make_shared<nlm::ConsoleLogger>(logLevel);
        fileLogger->setOutputFile(*options.logging.logFile);
        logger->addLogger(fileLogger);
    }
    
    // Enable event logging
    if (options.logging.eventLog) {
        auto eventLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(eventLogger);
    }
}

// Initialize debugging system
void setupDebugSystem(const nlm::AdvancedCLIOptions& options) {
    static std::shared_ptr<nlm::DebugManager> debugManager = std::make_shared<nlm::DebugManager>();
    nlm::Logger::getGlobal()->addLogger(std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info));
    
    // Configure debug level
    debugManager->setDebugLevel(options.debug.debugLevel);
    
    // Enable tracing
    if (!options.debug.traceTargets.empty()) {
        debugManager->enableTracing(options.debug.traceTargets);
    }
    
    // Enable profiling
    if (!options.debug.profileTargets.empty()) {
        for (const auto& target : options.debug.profileTargets) {
            if (target == "simulation") {
                debugManager->enableProfiling({"simulation"});
            } else if (target == "memory") {
                debugManager->enableProfiling({"memory"});
            } else if (target == "neuromodulation") {
                debugManager->enableProfiling({"neuromodulation"});
            }
        }
    }
    
    // Set up checkpointing
    if (options.debug.checkpointFile) {
        debugManager->saveDebugState(*options.debug.checkpointFile);
    }
}

// Initialize performance profiler
void setupProfiler(const nlm::AdvancedCLIOptions& options) {
    static std::shared_ptr<nlm::Profiler> profiler = std::make_shared<nlm::Profiler>();
    
    // Start profiling if measurement targets are specified
    if (!options.performance.measurementTargets.empty()) {
        std::vector<nlm::ProfileTarget> targets;
        for (const auto& target : options.performance.measurementTargets) {
            if (target == "cpu") targets.push_back(nlm::ProfileTarget::CPU);
            else if (target == "memory") targets.push_back(nlm::ProfileTarget::Memory);
            else if (target == "gpu") targets.push_back(nlm::ProfileTarget::GPU);
            else if (target == "network") targets.push_back(nlm::ProfileTarget::Network);
        }
        
        if (!targets.empty()) {
            profiler->startProfiling(targets);
        }
    }
    
    // Enable benchmark mode
    if (options.performance.benchmarkMode != "none") {
        profiler->startBenchmark(options.performance.benchmarkMode);
    }
}

// Initialize visualization system
void setupVisualization(const nlm::AdvancedCLIOptions& options) {
    static std::shared_ptr<nlm::Visualizer> visualizer = std::make_shared<nlm::Visualizer>();
    
    // Configure visualization
    if (options.export.liveViz) {
        visualizer->enableLiveMode();
        visualizer->setVisualizationPort(options.export.vizPort);
    }
    
    visualizer->setUpdateInterval(1.0 / options.export.vizUpdateInterval);
    
    // Set up export formats
    if (options.export.exportNetworkFormat != "none") {
        std::string networkFile = options.export.exportNetworkPath.value_or("network." + options.export.exportNetworkFormat);
        // Network export would be handled here
    }
    
    if (options.export.exportSpikesFormat != "none") {
        std::string spikesFile = options.export.exportSpikesPath.value_or("spikes." + options.export.exportSpikesFormat);
        // Spikes export would be handled here
    }
    
    if (options.export.exportMemoryFormat != "none") {
        std::string memoryFile = options.export.exportMemoryPath.value_or("memory." + options.export.exportMemoryFormat);
        // Memory export would be handled here
    }
    
    if (options.export.exportBehaviorFormat != "none") {
        std::string behaviorFile = options.export.exportBehaviorPath.value_or("behavior." + options.export.exportBehaviorFormat);
        // Behavior export would be handled here
    }
}

// Export results
void exportResults(const nlm::AdvancedCLIOptions& options, const std::shared_ptr<nlm::Brain>& brain) {
    if (!brain) return;
    
    // Export network
    if (options.export.exportNetworkFormat != "none") {
        try {
            std::string networkData = brain->exportNetwork(options.export.exportNetworkFormat);
            if (!options.export.exportNetworkPath) {
                // Write to default file
                std::ofstream file("network." + options.export.exportNetworkFormat);
                if (file.is_open()) {
                    file << networkData;
                    NLM_LOG_INFO("Exported network to network." + options.export.exportNetworkFormat);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to export network: " + std::string(e.what()));
        }
    }
    
    // Export spikes
    if (options.export.exportSpikesFormat != "none") {
        try {
            std::string spikesData = brain->exportSpikes(options.export.exportSpikesFormat);
            if (!options.export.exportSpikesPath) {
                std::ofstream file("spikes." + options.export.exportSpikesFormat);
                if (file.is_open()) {
                    file << spikesData;
                    NLM_LOG_INFO("Exported spikes to spikes." + options.export.exportSpikesFormat);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to export spikes: " + std::string(e.what()));
        }
    }
    
    // Export memory
    if (options.export.exportMemoryFormat != "none") {
        try {
            std::string memoryData = brain->exportMemory(options.export.exportMemoryFormat);
            if (!options.export.exportMemoryPath) {
                std::ofstream file("memory." + options.export.exportMemoryFormat);
                if (file.is_open()) {
                    file << memoryData;
                    NLM_LOG_INFO("Exported memory to memory." + options.export.exportMemoryFormat);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to export memory: " + std::string(e.what()));
        }
    }
    
    // Export behavior
    if (options.export.exportBehaviorFormat != "none") {
        try {
            std::string behaviorData = brain->exportBehavior(options.export.exportBehaviorFormat);
            if (!options.export.exportBehaviorPath) {
                std::ofstream file("behavior." + options.export.exportBehaviorFormat);
                if (file.is_open()) {
                    file << behaviorData;
                    NLM_LOG_INFO("Exported behavior to behavior." + options.export.exportBehaviorFormat);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to export behavior: " + std::string(e.what()));
        }
    }
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
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation with Enhanced CLI...\n" << std::endl;
    
    // Initialize enhanced CLI parser
    nlm::AdvancedCLI cli;
    
    // Parse command line arguments
    if (!cli.parse(argc, argv)) {
        std::cerr << "Error parsing command line arguments" << std::endl;
        return 1;
    }
    
    // Get parsed options
    const auto& options = cli.getOptions();
    
    // Validate options
    auto validationErrors = cli.validate();
    if (!validationErrors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cerr << "  - " << error << std::endl;
        }
        return 1;
    }
    
    // Print help if requested
    if (cli.hasOption("--help") || cli.hasOption("-h")) {
        std::cout << cli.getHelpText() << std::endl;
        return 0;
    }
    
    // Print version if requested
    if (cli.hasOption("--version") || cli.hasOption("-v")) {
        std::cout << "NLM v2.0+ Advanced CLI" << std::endl;
        return 0;
    }
    
    // Initialize logger with enhanced logging options
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    setupEnhancedLogging(options);
    
    NLM_LOG_INFO("=== NLM Phase 2: Enhanced Real Neural Computation ===");
    NLM_LOG_INFO("Phase 2.1: Advanced Configuration and CLI Parsing");
    
    // Display configuration summary
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  Max steps: " + std::to_string(options.simulation.maxSteps));
    NLM_LOG_INFO("  Timestep: " + std::to_string(options.simulation.timestep) + "s");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(options.simulation.neuronCount));
    NLM_LOG_INFO("  Region count: " + std::to_string(options.simulation.regionCount));
    NLM_LOG_INFO("  Temperature: " + std::to_string(options.simulation.temperature));
    NLM_LOG_INFO("  Learning rate: " + std::to_string(options.simulation.learningRate));
    NLM_LOG_INFO("  Plasticity enabled: " + std::to_string(options.simulation.plasticityEnabled));
    NLM_LOG_INFO("  Neuromodulation enabled: " + std::to_string(options.simulation.neuromodulationEnabled));
    
    // Initialize debugging system
    setupDebugSystem(options);
    
    // Initialize performance profiler
    setupProfiler(options);
    
    // Initialize visualization system
    setupVisualization(options);
    
    // Load configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Try to load from multiple config files (--config1, --config2, etc.)
    std::vector<std::string> configFiles = options.configFiles.configFiles;
    std::string legacyConfigFile = options.legacyConfigFile;
    
    bool configLoaded = false;
    if (!configFiles.empty()) {
        // Load first config file
        for (const auto& file : configFiles) {
            if (config->loadFromFile(file)) {
                NLM_LOG_INFO("Loaded configuration from: " + file);
                configLoaded = true;
                break;
            }
        }
    }
    
    if (!configLoaded && !legacyConfigFile.empty()) {
        if (config->loadFromFile(legacyConfigFile)) {
            NLM_LOG_INFO("Loaded legacy configuration from: " + legacyConfigFile);
            configLoaded = true;
        }
    }
    
    if (!configLoaded) {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply advanced configuration format options
    for (const auto& [key, value] : options.configFormat.setOptions) {
        config->set(key, value, nlm::ConfigSource::CommandLine);
    }
    
    for (const auto& option : options.configFormat.enableOptions) {
        if (option == "plasticity") {
            config->set("plasticity_enabled", true, nlm::ConfigSource::CommandLine);
        } else if (option == "neuromodulation") {
            config->set("neuromodulation_enabled", true, nlm::ConfigSource::CommandLine);
        } else if (option == "development") {
            config->set("development_enabled", true, nlm::ConfigSource::CommandLine);
        }
    }
    
    for (const auto& [key, value] : options.configFormat.overrideOptions) {
        config->set(key, value, nlm::ConfigSource::CommandLine);
    }
    
    // Override with command line args (using advanced CLI)
    cli.applyToConfig(config);
    
    // Set default values for Phase 2 (overridden by CLI if specified)
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("simulation_timestep", options.simulation.timestep, nlm::ConfigSource::CommandLine);
    config->set("neuron_count", options.simulation.neuronCount, nlm::ConfigSource::CommandLine);
    config->set("region_count", options.simulation.regionCount, nlm::ConfigSource::CommandLine);
    config->set("connection_probability", options.simulation.connectionProbability, nlm::ConfigSource::CommandLine);
    
    // STDP parameters
    config->set("stdp_ltp_weight", options.simulation.stdpLtpWeight, nlm::ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", options.simulation.stdpLtdWeight, nlm::ConfigSource::CommandLine);
    config->set("stdp_tau", options.simulation.stdpTau, nlm::ConfigSource::CommandLine);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", options.simulation.synaptogenesisRate, nlm::ConfigSource::CommandLine);
    config->set("pruning_rate", options.simulation.pruningRate, nlm::ConfigSource::CommandLine);
    
    // Memory system parameters
    config->set("working_memory_size", options.memory.workingMemorySize, nlm::ConfigSource::CommandLine);
    config->set("episodic_memory_size", options.memory.episodicMemorySize, nlm::ConfigSource::CommandLine);
    config->set("memory_replay", options.memory.memoryReplay, nlm::ConfigSource::CommandLine);
    config->set("replay_frequency", static_cast<int64_t>(options.memory.replayFrequency), nlm::ConfigSource::CommandLine);
    
    // Neuromodulation parameters
    config->set("dopamine_level", options.neuromodulation.dopamineLevel, nlm::ConfigSource::CommandLine);
    config->set("curiosity_level", options.neuromodulation.curiosityLevel, nlm::ConfigSource::CommandLine);
    config->set("novelty_threshold", options.neuromodulation.noveltyThreshold, nlm::ConfigSource::CommandLine);
    config->set("reward_scale", options.neuromodulation.rewardScale, nlm::ConfigSource::CommandLine);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Advanced Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    nlm::SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Start performance profiler
    if (!options.performance.measurementTargets.empty()) {
        nlm::Profiler::getGlobal()->startProfiling();
    }
    
    // Initialize brain with enhanced configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain with Enhanced Configuration...");
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Take initial performance sample
    nlm::Profiler::getGlobal()->takeSample(0, "initialization");
    
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
    
    // Stop performance profiler
    if (!options.performance.measurementTargets.empty()) {
        nlm::Profiler::getGlobal()->stopProfiling();
    }
    
    // Export results if requested
    exportResults(options, brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Display performance statistics
    if (!options.performance.measurementTargets.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Performance Statistics ===");
        auto profileStats = nlm::Profiler::getGlobal()->getProfileStats();
        NLM_LOG_INFO("Total samples: " + std::to_string(profileStats.totalSamples));
        NLM_LOG_INFO("Average CPU usage: " + std::to_string(profileStats.averageCpuUsage) + "%");
        NLM_LOG_INFO("Average memory usage: " + std::to_string(profileStats.averageMemoryUsage) + " MB");
        NLM_LOG_INFO("Peak memory usage: " + std::to_string(profileStats.peakMemoryUsage) + " MB");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2.1 Complete ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("")
    
    return 0;
}
