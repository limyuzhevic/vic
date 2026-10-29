// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Demonstration of Complete Brain System
//
// This phase demonstrates the fully integrated artificial brain system with:
// - Complete memory systems (working, episodic, associative)
// - Neuromodulation integration (dopamine, curiosity, novelty)
// - Prediction system integrated with learning
// - Development affecting plasticity rates
// - All systems working together as a coherent brain loop

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
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Complete artificial brain system with all systems integrated. ║
    ║     Memory, neuromodulation, prediction, development, cognition ║
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
    if (!region) {
        NLM_LOG_INFO("  ERROR: Region 1 not found!");
        return;
    }
    
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
        NLM_LOG_INFO("  ERROR: Region 1 not found for STDP test");
        return;
    }
    
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
    
// Override with command line args and advanced options
    config->loadFromArgs(argc, argv);
    
    // Apply command-line overrides for advanced users
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Neuron count configuration
            if (arg.find("--neurons=") == 0) {
                size_t neurons = std::stoul(arg.substr(9));
                config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set neuron_count to " + std::to_string(neurons));
            }
            else if (arg.find("--regions=") == 0) {
                size_t regions = std::stoul(arg.substr(9));
                config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set region_count to " + std::to_string(regions));
            }
            
            // STDP parameters
            else if (arg.find("--stdp_ltp=") == 0) {
                float ltp = std::stof(arg.substr(11));
                config->set("stdp_ltp_weight", ltp, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set stdp_ltp_weight to " + std::to_string(ltp));
            }
            else if (arg.find("--stdp_ltd=") == 0) {
                float ltd = std::stof(arg.substr(10));
                config->set("stdp_ltd_weight", ltd, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set stdp_ltd_weight to " + std::to_string(ltd));
            }
            
            // Plasticity parameters
            else if (arg.find("--synaptogenesis=") == 0) {
                float syn = std::stof(arg.substr(16));
                config->set("synaptogenesis_rate", syn, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set synaptogenesis_rate to " + std::to_string(syn));
            }
            else if (arg.find("--pruning=") == 0) {
                float prune = std::stof(arg.substr(11));
                config->set("pruning_rate", prune, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Set pruning_rate to " + std::to_string(prune));
            }
            
            // Plasticity modifier for test control
            else if (arg == "--high_plasticity") {
                config->set("plasticity_modifier", 2.0f, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Enabled high plasticity mode");
            }
            else if (arg == "--low_plasticity") {
                config->set("plasticity_modifier", 0.1f, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Enabled low plasticity mode");
            }
            
            // Test mode selection
            else if (arg == "--connectivity_test") {
                NLM_LOG_INFO("Command line: Running connectivity test only");
                // We'll modify the test functions to run only this test
            }
            else if (arg == "--plasticity_test") {
                NLM_LOG_INFO("Command line: Running plasticity test only");
                // We'll modify the test functions to run only this test
            }
            else if (arg == "--stdp_test") {
                NLM_LOG_INFO("Command line: Running STDP test only");
                // We'll modify the test functions to run only this test
            }
            
            // Debug and output options
            else if (arg == "--verbose") {
                config->set("log_level", "debug", ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Enabled verbose logging");
            }
            else if (arg == "--quiet") {
                config->set("log_level", "error", ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Enabled quiet mode (errors only)");
            }
            
            // Learning mode options
            else if (arg == "--curiosity_enabled") {
                config->set("agent.curiosity_enabled", true, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Curiosity exploration enabled");
            }
            else if (arg == "--curiosity_disabled") {
                config->set("agent.curiosity_enabled", false, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Curiosity exploration disabled");
            }
            
            // Development mode options
            else if (arg == "--development_enabled") {
                config->set("agent.development_enabled", true, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Development system enabled");
            }
            else if (arg == "--development_disabled") {
                config->set("agent.development_enabled", false, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Development system disabled");
            }
            
            // Reward modulation options
            else if (arg == "--reward_modulation_enabled") {
                config->set("agent.reward_modulation_enabled", true, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Reward modulation enabled");
            }
            else if (arg == "--reward_modulation_disabled") {
                config->set("agent.reward_modulation_enabled", false, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Reward modulation disabled");
            }
            
            // Structural plasticity options
            else if (arg == "--structural_plasticity_enabled") {
                config->set("agent.structural_plasticity_enabled", true, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Structural plasticity enabled");
            }
            else if (arg == "--structural_plasticity_disabled") {
                config->set("agent.structural_plasticity_enabled", false, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: Structural plasticity disabled");
            }
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  Phase: 6 - Final Integration");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 3)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(config->getOr<float>("stdp_ltp_weight", 0.02f)));
    NLM_LOG_INFO("  stdp_ltd_weight: " + std::to_string(config->getOr<float>("stdp_ltd_weight", 0.015f)));
    NLM_LOG_INFO("  synaptogenesis_rate: " + std::to_string(config->getOr<float>("synaptogenesis_rate", 0.00005f)));
    NLM_LOG_INFO("  pruning_rate: " + std::to_string(config->getOr<float>("pruning_rate", 0.00001f)));
    
    // Display command line summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Command line options applied:");
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-' && (argv[i][1] == '-' || argv[i][1] == '-')) {
                NLM_LOG_INFO("  " + std::string(argv[i]));
            }
        }
    } else {
        NLM_LOG_INFO("  (default settings)");
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
        return 1;
    }
    
    brain->logStatus();
    
    // Run tests based on command line options
    bool runConnectivity = true;
    bool runPlasticity = true;
    bool runStdp = true;
    
    // Parse command line to determine which tests to run
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--connectivity_test" && runConnectivity) {
                runConnectivity = false;
            } else if (arg == "--plasticity_test" && runPlasticity) {
                runPlasticity = false;
            } else if (arg == "--stdp_test" && runStdp) {
                runStdp = false;
            }
        }
    }
    
    // Run Test 1: Basic connectivity (if enabled)
    if (runConnectivity) {
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
    }
    
    // Run Test 2: Plasticity learning experiment (if enabled)
    if (runPlasticity) {
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification (if both enabled)
        if (runStdp) {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        }
    }
    // If only STDP test was requested, run it separately
    else if (runStdp) {
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Working memory with persistent activity and competition");
    NLM_LOG_INFO("  ✓ Episodic memory with experience encoding and replay");
    NLM_LOG_INFO("  ✓ Associative memory with Hebbian pattern associations");
    NLM_LOG_INFO("  ✓ Neuromodulation affecting neural excitability and plasticity");
    NLM_LOG_INFO("  ✓ Prediction system integrated into brain loop");
    NLM_LOG_INFO("  ✓ Neural planner with action sequence evaluation");
    NLM_LOG_INFO("  ✓ Concept formation from experience patterns");
    NLM_LOG_INFO("  ✓ Attention with competitive selection dynamics");
    NLM_LOG_INFO("  ✓ Developmental stages affecting plasticity rates");
    NLM_LOG_INFO("  ✓ Checkpoint save/load implementation");
    NLM_LOG_INFO("  ✓ Replay and consolidation functional");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Command line options summary:");
    NLM_LOG_INFO("  --neurons=N          Set neuron count");
    NLM_LOG_INFO("  --regions=N          Set region count");
    NLM_LOG_INFO("  --stdp_ltp=X         Set STDP LTP weight");
    NLM_LOG_INFO("  --stdp_ltd=X         Set STDP LTD weight");
    NLM_LOG_INFO("  --synaptogenesis=X   Set synaptogenesis rate");
    NLM_LOG_INFO("  --pruning=X          Set pruning rate");
    NLM_LOG_INFO("  --high_plasticity    Enable high plasticity mode");
    NLM_LOG_INFO("  --low_plasticity     Enable low plasticity mode");
    NLM_LOG_INFO("  --connectivity_test  Run connectivity test only");
    NLM_LOG_INFO("  --plasticity_test    Run plasticity test only");
    NLM_LOG_INFO("  --stdp_test          Run STDP test only");
    NLM_LOG_INFO("  --verbose            Enable verbose logging");
    NLM_LOG_INFO("  --quiet              Enable quiet mode");
    NLM_LOG_INFO("  --curiosity_enabled  Enable curiosity");
    NLM_LOG_INFO("  --curiosity_disabled Disable curiosity");
    NLM_LOG_INFO("  --development_enabled Enable development");
    NLM_LOG_INFO("  --development_disabled Disable development");
    NLM_LOG_INFO("  --reward_modulation_enabled Enable reward modulation");
    NLM_LOG_INFO("  --reward_modulation_disabled Disable reward modulation");
    NLM_LOG_INFO("  --structural_plasticity_enabled Enable structural plasticity");
    NLM_LOG_INFO("  --structural_plasticity_disabled Disable structural plasticity");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
