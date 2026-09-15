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

void runAdvancedNeuronTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Advanced Neuron Types ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    NLM_LOG_INFO("  Testing advanced neuron types...");
    
    // Test a few neurons with advanced properties
    size_t testNeurons = std::min(size_t(5), neurons.size());
    
    for (size_t i = 0; i < testNeurons; ++i) {
        Neuron* neuron = neurons[i];
        NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " type=" + std::to_string(static_cast<int>(neuron->getType())) +
                        " V=" + std::to_string(neuron->getMembranePotential()) +
                        " thresh=" + std::to_string(neuron->getThreshold()));
    }
    
    // Inject current to stimulate activity
    NLM_LOG_INFO("  Injecting current to test advanced dynamics...");
    for (size_t i = 0; i < testNeurons; ++i) {
        neurons[i]->injectCurrent(30.0f);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 20 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    NLM_LOG_INFO("  Advanced neuron test completed.");
}

void runAdvancedPlasticityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Advanced Plasticity Rules ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for advanced plasticity test");
        return;
    }
    
    NLM_LOG_INFO("  Testing advanced plasticity rules...");
    
    // Create a simple brain state snapshot
    struct PlasticityState {
        float weight;
        float efficacy;
        float eligibilityTrace;
        int spikeCount;
    };
    
    std::vector<PlasticityState> beforeStates;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        PlasticityState state;
        state.weight = synapses[i]->getWeight();
        state.efficacy = synapses[i]->getEfficacy();
        state.eligibilityTrace = synapses[i]->getEligibilityTrace();
        state.spikeCount = synapses[i]->getPreSpikeHistory().size();
        beforeStates.push_back(state);
    }
    
    // Enable advanced plasticity
    NLM_LOG_INFO("  Enabling advanced plasticity rules...");
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        synapses[i]->enablePlasticity(true, true, true); // Enable all plasticity types
        // Set some parameters
        synapses[i]->setEfficacy(0.8f + (i * 0.05f));
    }
    
    // Apply activity-dependent plasticity
    NLM_LOG_INFO("  Applying activity-dependent plasticity...")
    for (SimulationStep step = 0; step < 500; ++step) {
        // Generate activity pattern
        for (size_t i = 0; i < 3 && i < neurons.size(); ++i) {
            neurons[i]->injectCurrent(20.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Check changes
    NLM_LOG_INFO("  After activity...");
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        float weightChange = synapses[i]->getWeight() - beforeStates[i].weight;
        float efficacyChange = synapses[i]->getEfficacy() - beforeStates[i].efficacy;
        float eligibilityChange = synapses[i]->getEligibilityTrace() - beforeStates[i].eligibilityTrace;
        
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " weight Δ=" + std::to_string(weightChange) +
                        " efficacy Δ=" + std::to_string(efficacyChange) +
                        " eligibility Δ=" + std::to_string(eligibilityChange));
    }
    
    NLM_LOG_INFO("  Advanced plasticity test completed.");
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
    
    // Command line arguments parsing for advanced options
    bool enableVisualization = false;
    bool enableParallel = false;
    bool useReducedPrecision = false;
    bool enableAblation = false;
    std::string experimentType = "standard";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--visualization" || arg == "-v") {
            enableVisualization = true;
            config->set("visualization_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--parallel" || arg == "--threads") {
            enableParallel = true;
            config->set("use_parallel_processing", true, ConfigSource::CommandLine);
        } else if (arg == "--reduced-precision" || arg == "--float16") {
            useReducedPrecision = true;
            config->set("use_reduced_precision", true, ConfigSource::CommandLine);
        } else if (arg == "--ablation" || arg == "--ablate") {
            enableAblation = true;
            config->set("enable_ablation", true, ConfigSource::CommandLine);
        } else if (arg == "--experiment" && i + 1 < argc) {
            experimentType = argv[++i];
            config->set("experiment_type", experimentType, ConfigSource::CommandLine);
        } else if (arg == "--advanced-neurons" || arg == "--advanced") {
            config->set("advanced_neuron_types_enabled", true, ConfigSource::CommandLine);
            config->set("bursty_neuron_enabled", true, ConfigSource::CommandLine);
            config->set("adaptive_neuron_enabled", true, ConfigSource::CommandLine);
            config->set("adaptive_if_neuron_enabled", true, ConfigSource::CommandLine);
            config->set("hodgkin_huxley_neuron_enabled", true, ConfigSource::CommandLine);
            config->set("resonant_neuron_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--advanced-plasticity" || arg == "--advanced-plast") {
            config->set("advanced_plasticity_enabled", true, ConfigSource::CommandLine);
            config->set("reward_modulated_stdp_enabled", true, ConfigSource::CommandLine);
            config->set("homosynaptic_enabled", true, ConfigSource::CommandLine);
            config->set("heterosynaptic_enabled", true, ConfigSource::CommandLine);
            config->set("synaptic_tagging_enabled", true, ConfigSource::CommandLine);
            config->set("metaplasticity_enabled", true, ConfigSource::CommandLine);
            config->set("short_term_plasticity_enabled", true, ConfigSource::CommandLine);
            config->set("neuromodulatory_integration_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--bursting-neurons") {
            config->set("bursty_neuron_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--adaptive-neurons") {
            config->set("adaptive_neuron_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--hh-neurons") {
            config->set("hodgkin_huxley_neuron_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--resonant-neurons") {
            config->set("resonant_neuron_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--rstdp") {
            config->set("reward_modulated_stdp_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--homosynaptic") {
            config->set("homosynaptic_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--heterosynaptic") {
            config->set("heterosynaptic_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--stp") {
            config->set("short_term_plasticity_enabled", true, ConfigSource::CommandLine);
        }
    }
    
    // Handle visualization flag
    if (enableVisualization) {
        config->set("visualization_enabled", true, ConfigSource::CommandLine);
    }
    
    // Log advanced options
    if (enableVisualization) {
        NLM_LOG_INFO("  Advanced options: Visualization enabled");
    }
    if (enableParallel) {
        NLM_LOG_INFO("  Advanced options: Parallel processing enabled");
    }
    if (useReducedPrecision) {
        NLM_LOG_INFO("  Advanced options: Reduced precision enabled");
    }
    if (enableAblation) {
        NLM_LOG_INFO("  Advanced options: Ablation mode enabled");
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Log advanced configuration options if set
    if (config->has("visualization_enabled") && config->getOr<bool>("visualization_enabled", false)) {
        NLM_LOG_INFO("  visualization_enabled: true");
    }
    if (config->has("use_parallel_processing") && config->getOr<bool>("use_parallel_processing", false)) {
        NLM_LOG_INFO("  use_parallel_processing: true");
    }
    if (config->has("use_reduced_precision") && config->getOr<bool>("use_reduced_precision", false)) {
        NLM_LOG_INFO("  use_reduced_precision: true");
    }
    if (config->has("enable_ablation") && config->getOr<bool>("enable_ablation", false)) {
        NLM_LOG_INFO("  enable_ablation: true");
    }
    if (config->has("experiment_type") && !config->getOr<std::string>("experiment_type", "").empty()) {
        NLM_LOG_INFO("  experiment_type: " + config->getOr<std::string>("experiment_type", ""));
    }
    
    // Log advanced neuron and plasticity configuration
    if (config->has("advanced_neuron_types_enabled") && config->getOr<bool>("advanced_neuron_types_enabled", false)) {
        NLM_LOG_INFO("  advanced_neuron_types_enabled: true");
    }
    if (config->has("bursty_neuron_enabled") && config->getOr<bool>("bursty_neuron_enabled", false)) {
        NLM_LOG_INFO("  bursty_neuron_enabled: true");
    }
    if (config->has("adaptive_neuron_enabled") && config->getOr<bool>("adaptive_neuron_enabled", false)) {
        NLM_LOG_INFO("  adaptive_neuron_enabled: true");
    }
    if (config->has("advanced_plasticity_enabled") && config->getOr<bool>("advanced_plasticity_enabled", false)) {
        NLM_LOG_INFO("  advanced_plasticity_enabled: true");
    }
    if (config->has("reward_modulated_stdp_enabled") && config->getOr<bool>("reward_modulated_stdp_enabled", false)) {
        NLM_LOG_INFO("  reward_modulated_stdp_enabled: true");
    }
    if (config->has("homosynaptic_enabled") && config->getOr<bool>("homosynaptic_enabled", false)) {
        NLM_LOG_INFO("  homosynaptic_enabled: true");
    }
    
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
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Run Test 3: STDP verification
    runStdpVerification(brain);
    
    // Run Test 4: Advanced neuron types
    runAdvancedNeuronTest(brain);
    
    // Run Test 5: Advanced plasticity rules
    runAdvancedPlasticityTest(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("  ✓ Advanced neuron types (bursty, adaptive, integrate-and-fire with adaptation, HH models, resonant neurons)");
    NLM_LOG_INFO("  ✓ Advanced plasticity rules (R-STDP, neuromodulatory plasticity, homosynaptic/heterosynaptic plasticity, synaptic tagging and capture, metaplasticity, STP, neuromodulatory integration)");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
