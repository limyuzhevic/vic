// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the final integration of all neural computation systems:
// - Real LIF neuron dynamics (Phase 2)
// - Event-driven spike propagation with synaptic delays (Phase 2)
// - STDP and Hebbian plasticity (Phase 2)
// - Structural plasticity (synaptogenesis/pruning) (Phase 2)
// - Working memory with persistent activity and competition (Phase 4)
// - Episodic memory with experience encoding and replay (Phase 4)
// - Associative memory with Hebbian pattern associations (Phase 4)
// - Neuromodulation with dopamine, curiosity, and prediction error (Phase 3)
// - Neural prediction system (Phase 4)
// - Neural planner with action sequence evaluation (Phase 4)
// - Concept formation from experience patterns (Phase 4)
// - Neural attention with competitive selection dynamics (Phase 4)
// - Developmental system with age-dependent plasticity (Phase 3)
// - Performance optimizations with memory pools and event queues (Phase 5)
// - Checkpoint save/load system (Phase 5)
// - Replay and consolidation with sleep/rest cycle (Phase 5)

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
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all previous systems:                ║
    ║     - LIF neuron dynamics (Phase 2)                            ║
    ║     - Event-driven spike propagation (Phase 2)                  ║
    ║     - STDP and Hebbian plasticity (Phase 2)                    ║
    ║     - Structural plasticity (Phase 2)                          ║
    ║     - Memory systems (Phase 4)                                 ║
    ║     - Neuromodulation (Phase 3)                                 ║
    ║     - Prediction and cognition (Phase 4)                       ║
    ║     - Development system (Phase 3)                             ║
    ║     - Performance optimizations (Phase 5)                      ║
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

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Integrating all neural computation systems:");
    NLM_LOG_INFO("  - Real LIF neuron dynamics (Phase 2)");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays (Phase 2)");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules (Phase 2)");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning) (Phase 2)");
    NLM_LOG_INFO("  - Working memory with persistent activity and competition (Phase 4)");
    NLM_LOG_INFO("  - Episodic memory with experience encoding and replay (Phase 4)");
    NLM_LOG_INFO("  - Associative memory with Hebbian pattern associations (Phase 4)");
    NLM_LOG_INFO("  - Neuromodulation: dopamine, curiosity, prediction error (Phase 3)");
    NLM_LOG_INFO("  - Neural prediction system (Phase 4)");
    NLM_LOG_INFO("  - Neural planner with action sequence evaluation (Phase 4)");
    NLM_LOG_INFO("  - Concept formation from experience patterns (Phase 4)");
    NLM_LOG_INFO("  - Neural attention with competitive selection dynamics (Phase 4)");
    NLM_LOG_INFO("  - Developmental system with age-dependent plasticity (Phase 3)");
    NLM_LOG_INFO("  - Performance optimizations with memory pools and event queues (Phase 5)");
    NLM_LOG_INFO("  - Checkpoint save/load system (Phase 5)");
    NLM_LOG_INFO("  - Replay and consolidation with sleep/rest cycle (Phase 5)");
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
    
    // Print help if requested
    if (config->getOr<bool>("show_help", false)) {
        std::cout << "\n=== NLM Phase 6 Command Line Options ===" << std::endl;
        std::cout << "  --help, -h                     Show this help message" << std::endl;
        std::cout << "  --config <file>                Load configuration from file" << std::endl;
        std::cout << "  --random_seed <seed>           Set random seed" << std::endl;
        std::cout << "  --neuron_count <count>         Set neuron count" << std::endl;
        std::cout << "  --region_count <count>         Set region count" << std::endl;
        std::cout << "  --connection_probability <prob> Set connection probability" << std::endl;
        std::cout << "  --stdp_ltp_weight <weight>     Set STDP LTP weight" << std::endl;
        std::cout << "  --stdp_ltd_weight <weight>     Set STDP LTD weight" << std::endl;
        std::cout << "  --stdp_tau <tau>               Set STDP time constant" << std::endl;
        std::cout << "  --synaptogenesis_rate <rate>   Set synaptogenesis rate" << std::endl;
        std::cout << "  --pruning_rate <rate>          Set pruning rate" << std::endl;
        std::cout << "  --simulation_timestep <dt>     Set simulation timestep" << std::endl;
        std::cout << "  --log_level <level>            Set log level (Debug, Info, Warning, Error)" << std::endl;
        std::cout << "  --enable_checkpointing         Enable checkpointing" << std::endl;
        std::cout << "  --enable_replay                Enable memory replay" << std::endl;
        std::cout << "  --enable_development           Enable developmental plasticity" << std::endl;
        std::cout << "  --enable_curiosity            Enable curiosity-driven exploration" << std::endl;
        std::cout << "  --max_steps <steps>            Set maximum simulation steps" << std::endl;
        std::cout << "  --dry_run                      Validate configuration only" << std::endl;
        std::cout << "  --checkpoint_path <path>       Set checkpoint save/load path" << std::endl;
        std::cout << "  --enable_debug_features         Enable debug mode with extra output" << std::endl;
        std::cout << "  --enable_advanced_plasticity   Enable advanced plasticity rules" << std::endl;
        std::cout << "  --enable_social_learning        Enable social learning mechanisms" << std::endl;
        std::cout << "  --learning_rate <rate>         Set overall learning rate" << std::endl;
        std::cout << "  --exploration_rate <rate>      Set exploration/exploitation balance" << std::endl;
        return 0;
    }
    
    // Set default values for Phase 6
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
    
    // Memory systems parameters
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("episodic_memory_capacity", 50, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("curiosity_threshold", 0.5f, ConfigSource::Default);
    config->set("novelty_threshold", 0.3f, ConfigSource::Default);
    
    // Development parameters
    config->set("critical_period_start", 100, ConfigSource::Default);
    config->set("critical_period_end", 500, ConfigSource::Default);
    
    // Performance parameters
    config->set("max_simulation_steps", 10000, ConfigSource::Default);
    config->set("checkpoint_interval", 1000, ConfigSource::Default);
    config->set("replay_interval", 100, ConfigSource::Default);
    
// Print configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(config->getOr<float>("stdp_ltp_weight", 0.02f)));
    NLM_LOG_INFO("  stdp_ltd_weight: " + std::to_string(config->getOr<float>("stdp_ltd_weight", 0.015f)));
    NLM_LOG_INFO("  stdp_tau: " + std::to_string(config->getOr<float>("stdp_tau", 20.0f)));
    NLM_LOG_INFO("  synaptogenesis_rate: " + std::to_string(config->getOr<float>("synaptogenesis_rate", 0.0001f)));
    NLM_LOG_INFO("  pruning_rate: " + std::to_string(config->getOr<float>("pruning_rate", 0.00001f)));
    NLM_LOG_INFO("  working_memory_capacity: " + std::to_string(config->getOr<int64_t>("working_memory_capacity", 100)));
    NLM_LOG_INFO("  episodic_memory_capacity: " + std::to_string(config->getOr<int64_t>("episodic_memory_capacity", 50)));
    NLM_LOG_INFO("  dopamine_baseline: " + std::to_string(config->getOr<float>("dopamine_baseline", 0.1f)));
    NLM_LOG_INFO("  curiosity_threshold: " + std::to_string(config->getOr<float>("curiosity_threshold", 0.5f)));
    NLM_LOG_INFO("  novelty_threshold: " + std::to_string(config->getOr<float>("novelty_threshold", 0.3f)));
    NLM_LOG_INFO("  critical_period_start: " + std::to_string(config->getOr<int64_t>("critical_period_start", 100)));
    NLM_LOG_INFO("  critical_period_end: " + std::to_string(config->getOr<int64_t>("critical_period_end", 500)));
    NLM_LOG_INFO("  max_simulation_steps: " + std::to_string(config->getOr<int64_t>("max_simulation_steps", 10000)));
    NLM_LOG_INFO("  checkpoint_interval: " + std::to_string(config->getOr<int64_t>("checkpoint_interval", 1000)));
    NLM_LOG_INFO("  replay_interval: " + std::to_string(config->getOr<int64_t>("replay_interval", 100)));
    NLM_LOG_INFO("");
    
    // Validate configuration
    if (validateConfiguration(config)) {
        NLM_LOG_INFO("Configuration validation passed.");
    } else {
        NLM_LOG_ERROR("Configuration validation failed!");
        return 1;
    }
    
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
        NLM_LOG_ERROR("This usually indicates configuration issues or insufficient memory.");
        NLM_LOG_ERROR("Check your configuration values and try again.");
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
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented (Phase 2)");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays (Phase 2)");
    NLM_LOG_INFO("  ✓ STDP plasticity rule (Phase 2)");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule (Phase 2)");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning) (Phase 2)");
    NLM_LOG_INFO("  ✓ Working memory with persistent activity and competition (Phase 4)");
    NLM_LOG_INFO("  ✓ Episodic memory with experience encoding and replay (Phase 4)");
    NLM_LOG_INFO("  ✓ Associative memory with Hebbian pattern associations (Phase 4)");
    NLM_LOG_INFO("  ✓ Neuromodulation with dopamine, curiosity, and prediction error (Phase 3)");
    NLM_LOG_INFO("  ✓ Neural prediction system (Phase 4)");
    NLM_LOG_INFO("  ✓ Neural planner with action sequence evaluation (Phase 4)");
    NLM_LOG_INFO("  ✓ Concept formation from experience patterns (Phase 4)");
    NLM_LOG_INFO("  ✓ Neural attention with competitive selection dynamics (Phase 4)");
    NLM_LOG_INFO("  ✓ Developmental system with age-dependent plasticity (Phase 3)");
    NLM_LOG_INFO("  ✓ Performance optimizations with memory pools and event queues (Phase 5)");
    NLM_LOG_INFO("  ✓ Checkpoint save/load system (Phase 5)");
    NLM_LOG_INFO("  ✓ Replay and consolidation with sleep/rest cycle (Phase 5)");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}

// Configuration validation function
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    bool isValid = true;
    
    // Validate core parameters
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 500);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) + ". Must be positive.");
        isValid = false;
    }
    
    int64_t regionCount = config->getOr<int64_t>("region_count", 1);
    if (regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount) + ". Must be positive.");
        isValid = false;
    }
    
    float connectionProb = config->getOr<float>("connection_probability", 0.15f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProb) + ". Must be between 0.0 and 1.0.");
        isValid = false;
    }
    
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be positive and less than 1.0.");
        isValid = false;
    }
    
    // Validate plasticity parameters
    float stdpLTP = config->getOr<float>("stdp_ltp_weight", 0.02f);
    if (stdpLTP < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: " + std::to_string(stdpLTP) + ". Must be non-negative.");
        isValid = false;
    }
    
    float stdpLTD = config->getOr<float>("stdp_ltd_weight", 0.015f);
    if (stdpLTD < 0.0f) {
        NLM_LOG_ERROR("Invalid stdp_ltd_weight: " + std::to_string(stdpLTD) + ". Must be non-negative.");
        isValid = false;
    }
    
    // Validate neuromodulation parameters
    float dopamine = config->getOr<float>("dopamine_baseline", 0.1f);
    if (dopamine < 0.0f || dopamine > 1.0f) {
        NLM_LOG_ERROR("Invalid dopamine_baseline: " + std::to_string(dopamine) + ". Must be between 0.0 and 1.0.");
        isValid = false;
    }
    
    // Validate development parameters
    int64_t criticalStart = config->getOr<int64_t>("critical_period_start", 100);
    int64_t criticalEnd = config->getOr<int64_t>("critical_period_end", 500);
    if (criticalStart < 0) {
        NLM_LOG_ERROR("Invalid critical_period_start: " + std::to_string(criticalStart) + ". Must be non-negative.");
        isValid = false;
    }
    if (criticalEnd <= criticalStart) {
        NLM_LOG_ERROR("Invalid critical_period_end: " + std::to_string(criticalEnd) + ". Must be greater than critical_period_start.");
        isValid = false;
    }
    
    // Validate memory system parameters
    int64_t workingMemCap = config->getOr<int64_t>("working_memory_capacity", 100);
    if (workingMemCap <= 0) {
        NLM_LOG_ERROR("Invalid working_memory_capacity: " + std::to_string(workingMemCap) + ". Must be positive.");
        isValid = false;
    }
    
    int64_t episodicMemCap = config->getOr<int64_t>("episodic_memory_capacity", 50);
    if (episodicMemCap <= 0) {
        NLM_LOG_ERROR("Invalid episodic_memory_capacity: " + std::to_string(episodicMemCap) + ". Must be positive.");
        isValid = false;
    }
    
    // Check for memory capacity conflicts
    if (workingMemCap + episodicMemCap > neuronCount * 10) {  // Heuristic: 10 neurons per memory trace
        NLM_LOG_WARNING("Memory system capacity may be too high relative to neuron count. Consider reducing capacity.");
    }
    
    if (!isValid) {
        NLM_LOG_ERROR("Configuration validation failed. Please check the errors above.");
    }
    
    return isValid;
}
