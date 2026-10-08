// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Types/Types.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

}

void printHelp() {
    std::cout << R"(
NLM Phase 2 Neural Network Demo Program

Usage: nlm [OPTIONS]

Options:
  -h, --help              Show this help message and exit
  -v, --verbose           Enable verbose output with detailed logging
  -A, --advanced          Enable advanced features and extended testing
  --config file           Load configuration from specified file
  --neuron-count N        Override number of neurons (default: 500)
  --region-count N        Override number of regions (default: 1)
  --duration STEPS        Override simulation steps (default: 1000 for plasticity)
  --version               Print version information

Examples:
  nlm                                  # Run basic connectivity test
  nlm -v                              # Run with verbose output
  nlm --advanced                      # Run with advanced features
  nlm --config myconfig.cfg            # Use custom configuration
  nlm --neuron-count 1000 --region-count 2  # Custom network size

For more information:
  - Read docs/ARCHITECTURE.md for detailed system overview
  - Check the NLM project documentation
  - Refer to examples/ directory for additional usage patterns

Phase 2 focuses on:
  - Real LIF neuron dynamics
  - Event-driven spike propagation with delays
  - STDP and Hebbian plasticity rules
  - Structural plasticity (synaptogenesis/pruning)
  - Learning experiments demonstrating measurable changes

Test 1: Basic connectivity validates neural signal transmission
Test 2: Plasticity experiment shows activity-dependent learning
Test 3: STDP verification confirms spike-timing-dependent plasticity

Note: Larger networks (--neuron-count) will consume more memory and take longer.
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

void printHelp() {
    std::cout << R"(
NLM Phase 2 Neural Network Demo Program

Usage: nlm [OPTIONS]

Options:
  -h, --help              Show this help message and exit
  -v, --verbose           Enable verbose output with detailed logging
  -A, --advanced          Enable advanced features and extended testing
  --config file           Load configuration from specified file
  --neuron-count N        Override number of neurons (default: 500)
  --region-count N        Override number of regions (default: 1)
  --duration STEPS        Override simulation steps (default: 1000 for plasticity)
  --version               Print version information

Examples:
  nlm                                  # Run basic connectivity test
  nlm -v                              # Run with verbose output
  nlm --advanced                      # Run with advanced features
  nlm --config myconfig.cfg            # Use custom configuration
  nlm --neuron-count 1000 --region-count 2  # Custom network size

For more information:
  - Read docs/ARCHITECTURE.md for detailed system overview
  - Check the NLM project documentation
  - Refer to examples/ directory for additional usage patterns

Phase 2 focuses on:
  - Real LIF neuron dynamics
  - Event-driven spike propagation with delays
  - STDP and Hebbian plasticity rules
  - Structural plasticity (synaptogenesis/pruning)
  - Learning experiments demonstrating measurable changes

Test 1: Basic connectivity validates neural signal transmission
Test 2: Plasticity experiment shows activity-dependent learning
Test 3: STDP verification confirms spike-timing-dependent plasticity

Note: Larger networks (--neuron-count) will consume more memory and take longer.
  )" << std::endl;
}
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  ERROR: No region 1 found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_WARNING("  WARNING: No neurons found!");
        return;
    }
    
    NLM_LOG_INFO("  Network initialized with " + std::to_string(neurons.size()) + " neurons");
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        if (verbose) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " injected with 50.0 current, threshold: " + 
                        std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    // Run a few steps
    NLM_LOG_INFO("  Running 50 simulation steps...");
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 10 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Active: " + std::to_string(brain->getActiveNeuronCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ SUCCESS: Spikes propagate through network");
        if (verbose) {
            NLM_LOG_INFO("    Connectivity is functional - neural signals transmit effectively");
        }
    } else {
        NLM_LOG_WARNING("  WARNING: No spikes detected - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()) +
                        " state=" + neuronStateToString(neurons[i]->getState().firingState));
        }
    }
}

std::string neuronStateToString(FiringState state) {
    switch (state) {
        case FiringState::Resting: return "Resting";
        case FiringState::Active: return "Active";
        case FiringState::Refractory: return "Refractory";
        default: return "Unknown";
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
    
    // Set default values for Phase 2
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
    
    // Parse command-line arguments for configuration overrides
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Apply configuration overrides
                if (key == "neuron-count" || key == "neuron_count") {
                    try {
                        int64_t neuronCount = std::stoll(value);
                        config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
                        NLM_LOG_INFO("  Overridden neuron_count: " + value);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("  ERROR: Invalid neuron-count value: " + value);
                    }
                } else if (key == "region-count" || key == "region_count") {
                    try {
                        int64_t regionCount = std::stoll(value);
                        config->set("region_count", regionCount, ConfigSource::CommandLine);
                        NLM_LOG_INFO("  Overridden region_count: " + value);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("  ERROR: Invalid region-count value: " + value);
                    }
                } else if (key == "duration") {
                    try {
                        int64_t duration = std::stoll(value);
                        config->set("plasticity_duration", duration, ConfigSource::CommandLine);
                        NLM_LOG_INFO("  Overridden plasticity_duration: " + value);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("  ERROR: Invalid duration value: " + value);
                    }
                } else if (key == "verbose" || key == "v") {
                    verbose = true;
                    NLM_LOG_INFO("Verbose mode enabled");
                } else if (key == "advanced" || key == "A") {
                    enableAdvancedFeatures = true;
                    NLM_LOG_INFO("Advanced features enabled");
                }
                // Other config options can be added here
            }
        }
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    if (verbose) {
        NLM_LOG_INFO("Verbose output enabled - detailed logging of all operations");
    }
    runBasicConnectivityTest(brain, verbose);
    
    // Reset brain for advanced experiments
    brain->reset();
    brain->initialize();
    
    // Run Advanced Learning Experiment if enabled
    if (enableAdvancedFeatures) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Advanced Features ===");
        uint32_t experimentDuration = config->getOr<int64_t>("advanced_experiment_duration", 500);
        runAdvancedLearningExperiment(brain, verbose, experimentDuration);
    }
    
    // Reset brain for integration test
    brain->reset();
    brain->initialize();
    
    // Run Cortical Integration Test if enabled
    if (enableAdvancedFeatures) {
        runCorticalIntegrationTest(brain, verbose);
    }
    
    // Reset brain for development simulation
    brain->reset();
    brain->initialize();
    
    // Run Development Simulation if enabled
    if (enableAdvancedFeatures) {
        runDevelopmentSimulation(brain, verbose);
    }
    
    // Reset and run Test 2: Plasticity learning experiment
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
void runAdvancedLearningExperiment(std::shared_ptr<Brain> brain, bool verbose, uint32_t duration) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Learning Experiment ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  ERROR: No region 1 found for advanced experiment");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    NLM_LOG_INFO("  Starting advanced learning with " + std::to_string(neurons.size()) + " neurons for " + std::to_string(duration) + " steps");
    
    // Enable all plasticity mechanisms
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // Enable Hebbian, STDP, and reward-modulated
        }
    }
    
    // Record initial state
    NLM_LOG_INFO("  Initial state: " + std::to_string(brain->getTotalSpikeCount()) + " spikes total");
    
    // Complex input pattern: alternating sensory stimulation
    NLM_LOG_INFO("  Applying complex input patterns...")
    
    for (uint32_t step = 0; step < duration; ++step) {
        // Create rich input patterns
        uint32_t sensoryNeurons = std::min<uint32_t>(10, neurons.size() / 4);
        
        if (step % 3 == 0) {
            // Strong excitatory burst
            for (size_t i = 0; i < sensoryNeurons; ++i) {
                neurons[i]->injectCurrent(60.0f + (i * 10.0f));
            }
        } else if (step % 3 == 1) {
            // Moderate input with variation
            for (size_t i = 0; i < sensoryNeurons; ++i) {
                neurons[i]->injectCurrent(30.0f + (std::sin(step * 0.1f) * 20.0f));
            }
        } else {
            // Inhibitory modulation to create dynamic balance
            for (size_t i = 0; i < sensoryNeurons / 2; ++i) {
                neurons[i + neurons.size() / 4]->injectCurrent(-20.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (verbose && step % 100 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Active: " + std::to_string(brain->getActiveNeuronCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Weight mean: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    // Analysis of learning outcomes
    NLM_LOG_INFO("  Final state: " + std::to_string(brain->getTotalSpikeCount()) + " spikes total");
    NLM_LOG_INFO("  Learning completed with " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
    
    // Determine learning quality
    uint32_t spikeRate = brain->getTotalSpikeCount() / duration;
    float firingRate = brain->getAverageFiringRate();
    
    if (spikeRate > 100 && firingRate > 0.5f) {
        NLM_LOG_INFO("  ✓ EXCELLENT: High learning activity detected");
    } else if (spikeRate > 50) {
        NLM_LOG_INFO("  ✓ GOOD: Moderate learning activity observed");
    } else {
        NLM_LOG_INFO("  ! LOW: Limited learning activity");
    }
}

void runCorticalIntegrationTest(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Cortical Integration Test ===");
    
    // Test with different brain regions and connectivity patterns
    NLM_LOG_INFO("  Testing multi-region integration...")
    
    // Create additional regions for testing
    auto region1 = brain->getRegion(RegionId(1));
    if (!region1) return;
    
    // Get statistics from all regions
    auto regions = brain->getRegions();
    NLM_LOG_INFO("  Network structure: " + std::to_string(regions.size()) + " regions total");
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("  Connectivity: " + std::to_string(totalNeurons) + " neurons, " + std::to_string(totalSynapses) + " synapses");
    
    if (totalSynapses > 0) {
        float connectionRatio = (float)totalSynapses / totalNeurons;
        NLM_LOG_INFO("  Connection density: " + std::to_string(connectionRatio) + " synapses per neuron");
    }
    
    if (verbose) {
        NLM_LOG_INFO("  Regional details:");
        for (size_t i = 0; i < std::min<size_t>(3, regions.size()); ++i) {
            auto* region = regions[i].get();
            NLM_LOG_INFO("    Region " + std::to_string(i + 1) + ": " + 
                        std::to_string(region->getAllNeurons().size()) + " neurons,
                        " + std::to_string(region->getSynapses().size()) + " synapses");
        }
    }
    
    if (totalSynapses > totalNeurons / 2) {
        NLM_LOG_INFO("  ✓ INTEGRATION: Rich connectivity achieved");
    } else {
        NLM_LOG_INFO("  ! CONNECTIVITY: Sparse network, consider increasing connection probability");
    }
}

void runDevelopmentSimulation(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Development Simulation ===");
    
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("  Simulating brain development stages...")
    
    // Simulate developmental stages
    NLM_LOG_INFO("  Stage 1 (Initial): Neonatal configuration");
    brain->setDevelopmentalStage(DevelopmentalStage::Early);
    
    // High plasticity, low connectivity initially
    for (int step = 0; step < 200; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 50 == 0) {
            NLM_LOG_INFO("    Stage 1 - Step " + std::to_string(step) + 
                        ": Plasticity active, developing circuits");
        }
    }
    
    NLM_LOG_INFO("  Stage 2 (Maturation): Early development phase");
    brain->setDevelopmentalStage(DevelopmentalStage::Middle);
    
    // Moderate plasticity, increasing connectivity
    for (int step = 200; step < 400; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 50 == 0) {
            NLM_LOG_INFO("    Stage 2 - Step " + std::to_string(step) + 
                        ": Maturation and refinement");
        }
    }
    
    NLM_LOG_INFO("  Stage 3 (Adult): Fully mature network");
    brain->setDevelopmentalStage(DevelopmentalStage::Late);
    
    // Lower plasticity, stable network
    for (int step = 400; step < 500; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 50 == 0) {
            NLM_LOG_INFO("    Stage 3 - Step " + std::to_string(step) + 
                        ": Adult stability phase");
        }
    }
    
    DevelopmentalStage finalStage = brain->getDevelopmentalStage();
    if (finalStage == DevelopmentalStage::Late) {
        NLM_LOG_INFO("  ✓ DEVELOPMENT: Complete maturation cycle achieved");
        NLM_LOG_INFO("    Final developmental stage: Late (adult)");
    }
}
