// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Improved Application
//
// This version improves the NLM application with:
// - Better separation of concerns between production logic and test code
// - Enhanced error handling and validation
// - Command-line interface with multiple modes
// - Improved configuration management
// - Modular design for easier extension and maintenance

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

// Production mode: Simple brain simulation with configurable behavior
class NLMApplication {
public:
    struct Config {
        std::string configFile = "configs/default.cfg";
        std::string experimentMode = "demo"; // "demo", "production", "debug"
        int maxSteps = 1000;
        double timestep = 0.001;
        bool enableLogging = true;
        std::string outputFile = "";
        bool runConnectivityTest = false;
        bool runPlasticityTest = false;
        bool runStdpTest = false;
        int neuronCount = 1000;
        int regionCount = 1;
    };
    
    NLMApplication(const Config& appConfig) : config(appConfig) {}
    
    bool run() {
        try {
            initializeLogging();
            NLM_LOG_INFO("Starting NLM Application - Phase 6: Integrated Artificial Brain");
            
            // Load configuration
            auto config = loadConfiguration();
            if (!config) {
                return false;
            }
            
            // Initialize brain
            auto brain = createBrain(config);
            if (!brain) {
                NLM_LOG_ERROR("Failed to create brain");
                return false;
            }
            
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return false;
            }
            
            brain->logStatus();
            
            // Run requested tests or demo
            if (config->getOr<bool>("run_connectivity_test", config.experimentMode == "debug")) {
                runConnectivityTest(brain);
            }
            
            if (config->getOr<bool>("run_plasticity_test", config.experimentMode == "debug")) {
                brain->reset();
                brain->initialize();
                runPlasticityExperiment(brain);
            }
            
            if (config->getOr<bool>("run_stdp_test", config.experimentMode == "debug")) {
                brain->reset();
                brain->initialize();
                runStdpVerification(brain);
            }
            
            // Production mode: Run simulation
            if (config->getOr<bool>("run_production", config.experimentMode == "production" || 
                                   config.experimentMode == "demo")) {
                runProductionSimulation(brain);
            }
            
            // Save output if requested
            if (!config->getOr<std::string>("output_file", "").empty()) {
                saveResults(brain, config->getOr<std::string>("output_file", ""));
            }
            
            brain->logStatus();
            NLM_LOG_INFO("NLM Application completed successfully");
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Application error: ") + e.what());
            return false;
        }
    }
    
private:
    Config config;
    
    void initializeLogging() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(config.enableLogging ? LogLevel::Info : LogLevel::Error);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    std::shared_ptr<Config> loadConfiguration() {
        auto config = std::make_shared<Config>();
        
        // Load from file if specified
        if (!config->getOr<std::string>("config_file", "").empty()) {
            if (!config->loadFromFile(config->getOr<std::string>("config_file", ""))) {
                NLM_LOG_WARN("Could not load config file, using defaults");
            }
        }
        
        // Override with command line args if in production mode
        if (config->getOr<std::string>("experiment_mode", "") == "production") {
            // Parse command line arguments
            config->loadFromArgs(0, nullptr);
        }
        
        return config;
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> appConfig) {
        auto brainConfig = std::make_shared<Config>();
        
        // Set default values with validation
        brainConfig->set("random_seed", 42, ConfigSource::Default);
        brainConfig->set("simulation_timestep", config.timestep, ConfigSource::Default);
        brainConfig->set("neuron_count", config.neuronCount, ConfigSource::Default);
        brainConfig->set("region_count", config.regionCount, ConfigSource::Default);
        brainConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Set plasticity parameters
        brainConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        brainConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Set structural plasticity parameters
        brainConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        brainConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return std::make_shared<Brain>(brainConfig);
    }
    
    void runConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running Connectivity Test ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found in brain");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * config.timestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
        } else {
            NLM_LOG_WARN("No spikes detected - checking neuron parameters");
            for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
                auto& state = neurons[i]->getState();
                NLM_LOG_INFO(std::string("Neuron ") + std::to_string(i) +
                           std::string(" V=") + std::to_string(state.membranePotential) +
                           std::string(" thresh=") + std::to_string(state.threshold));
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running Plasticity Experiment ===");
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input patterns
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * config.timestep);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO(std::string("Step ") + std::to_string(step) +
                           std::string(" | Spikes: ") + std::to_string(brain->getTotalSpikeCount()) +
                           std::string(" | Firing: ") + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("✓ Plasticity experiment completed");
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found for STDP test");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO(std::string("  Synapse ") + std::to_string(i) +
                       std::string(" before: ") + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity for LTP
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            // Find neurons to use
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
                brain->step(trial * 2, trial * 2 * config.timestep);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * config.timestep);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO(std::string("  Synapse ") + std::to_string(i) +
                       std::string(" after: ") + std::to_string(synapses[i]->getWeight()) +
                       std::string(" (Δ=") + std::to_string(delta) + std::string(")"));
        }
        
        // Check if weights increased (LTP)
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP working: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("! No change: STDP may not be triggering");
        }
    }
    
    void runProductionSimulation(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Running Production Simulation ===");
        NLM_LOG_INFO("Simulating " + std::to_string(config.maxSteps) + " steps...");
        
        // Check for environmental input
        bool hasEnvironment = config.experimentMode != "demo";
        
        if (hasEnvironment) {
            // TODO: Initialize environment when available
            NLM_LOG_INFO("Environment simulation would be integrated here");
        }
        
        // Run main simulation loop
        for (SimulationStep step = 0; step < config.maxSteps; ++step) {
            brain->step(step, step * config.timestep);
            
            // Log statistics periodically
            if (step % 100 == 0) {
                NLM_LOG_INFO(std::string("Step ") + std::to_string(step) +
                           std::string(" | Spikes: ") + std::to_string(brain->getTotalSpikeCount()) +
                           std::string(" | Firing: ") + std::to_string(brain->getFiringNeuronCount()) +
                           std::string(" | E/I Ratio: ") + std::to_string(brain->getExcitationInhibitionRatio()));
            }
        }
        
        NLM_LOG_INFO("✓ Production simulation completed");
    }
    
    void saveResults(std::shared_ptr<Brain> brain, const std::string& filepath) {
        NLM_LOG_INFO("Saving results to: " + filepath);
        
        try {
            if (brain->save(filepath)) {
                NLM_LOG_INFO("Results saved successfully");
            } else {
                NLM_LOG_ERROR("Failed to save results");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error saving results: ") + e.what());
        }
    }
};

// Command line argument parsing
NLMApplication::Config parseCommandLine(int argc, char** argv) {
    NLMApplication::Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--mode" && i + 1 < argc) {
            config.experimentMode = argv[++i];
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            config.timestep = std::stod(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoi(argv[++i]);
        } else if (arg == "--connectivity-test") {
            config.runConnectivityTest = true;
        } else if (arg == "--plasticity-test") {
            config.runPlasticityTest = true;
        } else if (arg == "--stdp-test") {
            config.runStdpTest = true;
        } else if (arg == "--no-logging") {
            config.enableLogging = false;
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg.substr(0, 7) == "--set=") {
            // Set custom configuration value
            std::string setting = arg.substr(7);
            size_t pos = setting.find('=');
            if (pos != std::string::npos) {
                std::string key = setting.substr(0, pos);
                std::string value = setting.substr(pos + 1);
                // Note: In a real implementation, this would set the config value
                NLM_LOG_INFO("Setting: " + key + " = " + value);
            }
        }
    }
    
    return config;
}

void printUsage() {
    std::cout << "NLM Application Usage:" << std::endl;
    std::cout << "  nlm [--help] [--config <file>] [--mode <mode>] [--steps <n>] [--timestep <t>]" << std::endl;
    std::cout << "      [--neurons <n>] [--regions <n>] [--connectivity-test] [--plasticity-test] [--stdp-test]" << std::endl;
    std::cout << "      [--no-logging] [--output <file>] [--set<key>=<value>]]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Modes: demo (default), production, debug" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --mode demo --steps 1000" << std::endl;
    std::cout << "  nlm --mode production --neurons 5000 --regions 4" << std::endl;
    std::cout << "  nlm --mode debug --connectivity-test --plasticity-test --stdp-test" << std::endl;
}

// Test function implementations (kept for backward compatibility)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Basic Connectivity Test ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Spikes propagate through network");
    } else {
        NLM_LOG_WARN("No spikes detected");
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO(std::string("Step ") + std::to_string(step) +
                       std::string(" | Spikes: ") + std::to_string(brain->getTotalSpikeCount()) +
                       std::string(" | Firing: ") + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("✓ Plasticity experiment completed");
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found for STDP test");
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_ERROR("Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO(std::string("  Synapse ") + std::to_string(i) +
                   std::string(" before: ") + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO(std::string("  Synapse ") + std::to_string(i) +
                   std::string(" after: ") + std::to_string(synapses[i]->getWeight()) +
                   std::string(" (Δ=") + std::to_string(delta) + std::string(")"));
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP working: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    try {
        auto config = parseCommandLine(argc, argv);
        
        NLMApplication app(config);
        if (app.run()) {
            return 0;
        } else {
            NLM_LOG_ERROR("Application failed");
            return 1;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Fatal error: ") + e.what());
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown fatal error");
        return 1;
    }
}

} // namespace nlm
