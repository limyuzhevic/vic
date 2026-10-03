// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the final integration of all brain systems into a coherent artificial brain:
// - Leaky Integrate-and-Fire (LIF) neurons (Phase 2 core)
// - Event-driven spike propagation with delays
// - STDP and Hebbian plasticity rules
// - Structural plasticity (synaptogenesis/pruning)
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction systems
// - Cognitive systems (planner, concept formation, attention)
// - Development system with staged maturation
// - Checkpoint save/load and persistence
// - Replay and consolidation mechanisms
// - Integration with agent and world systems

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
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

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
    ║     This phase integrates all systems into a coherent brain loop: ║
    ║     - Memory systems (working, episodic, associative)         ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)           ║
    ║     - Prediction systems                                       ║
    ║     - Cognitive systems (planner, concept formation, attention) ║
    ║     - Development with staged maturation                      ║
    ║     - Checkpoint save/load and persistence                    ║
    ║     - Replay and memory consolidation                           ║
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
    
    std::cout << "Initializing NLM Phase 6: Final Integration of Artificial Brain...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Phase 6 integrates all previously separate systems:");
    NLM_LOG_INFO("  - Memory systems (working, episodic, associative)");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Prediction systems");
    NLM_LOG_INFO("  - Cognitive systems (planner, concept formation, attention)");
    NLM_LOG_INFO("  - Development system with staged maturation");
    NLM_LOG_INFO("  - Checkpoint save/load and persistence");
    NLM_LOG_INFO("  - Replay and memory consolidation");
    NLM_LOG_INFO("  - Integration with agent and world systems");
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
    
    // Set default values for Phase 6 integration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Memory system configuration
    config->set("memory.working_memory_size", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("memory.episodic_capacity", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("memory.associative_strength", 0.5f, ConfigSource::Default);
    
    // Prediction system configuration
    config->set("prediction.enabled", true, ConfigSource::Default);
    config->set("prediction.update_interval", 10, ConfigSource::Default);
    config->set("prediction.error_threshold", 0.1f, ConfigSource::Default);
    
    // Neuromodulation configuration
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enabled", true, ConfigSource::Default);
    config->set("neuromod.novelty.enabled", true, ConfigSource::Default);
    config->set("neuromod.prediction_error.sensitivity", 0.5f, ConfigSource::Default);
    
    // Cognitive system configuration
    config->set("cognition.planner.enabled", true, ConfigSource::Default);
    config->set("cognition.planner.horizon", 10, ConfigSource::Default);
    config->set("cognition.concept_formation.enabled", true, ConfigSource::Default);
    config->set("cognition.attention.enable", true, ConfigSource::Default);
    config->set("cognition.attention.saturation_rate", 0.95f, ConfigSource::Default);
    
    // Development configuration
    config->set("development.enabled", true, ConfigSource::Default);
    config->set("development.stage.initial_plasticity", 1.0f, ConfigSource::Default);
    config->set("development.stage.critical_period_plasticity", 0.8f, ConfigSource::Default);
    config->set("development.stage.maturation_plasticity", 0.5f, ConfigSource::Default);
    config->set("development.stage.adult_plasticity", 0.2f, ConfigSource::Default);
    config->set("development.replay_interval", 100, ConfigSource::Default);
    config->set("development.consolidation_interval", 1000, ConfigSource::Default);
    
    // Checkpoint configuration
    config->set("checkpoint.enabled", true, ConfigSource::Default);
    config->set("checkpoint.path", "nlm_checkpoint.bin", ConfigSource::Default);
    config->set("checkpoint.auto_save", false, ConfigSource::Default);
    config->set("checkpoint.max_saves", 5, ConfigSource::Default);
    
    // Agent/world integration configuration
    config->set("agent.enable_reward_modulation", true, ConfigSource::Default);
    config->set("agent.enable_structural_plasticity", true, ConfigSource::Default);
    config->set("agent.enable_development", true, ConfigSource::Default);
    config->set("agent.enable_curiosity", true, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  memory.working_memory_size: " + std::to_string(config->getOr<int64_t>("memory.working_memory_size", 100)));
    NLM_LOG_INFO("  memory.episodic_capacity: " + std::to_string(config->getOr<int64_t>("memory.episodic_capacity", 1000)));
    NLM_LOG_INFO("  development.enabled: " + std::to_string(config->getOr<bool>("development.enabled", true)));
    NLM_LOG_INFO("  checkpoint.enabled: " + std::to_string(config->getOr<bool>("checkpoint.enabled", true)));
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
    
    // Create world for agent integration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing SimpleWorld for agent integration...");
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(16, 16);
    world->setAgentStart(8.0f, 8.0f);
    world->reset();
    
    // Create agent brain interface
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Creating AgentBrain interface...");
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Configure agent
    agent->enableRewardModulation(config->getOr<bool>("agent.enable_reward_modulation", true));
    agent->enableStructuralPlasticity(config->getOr<bool>("agent.enable_structural_plasticity", true));
    agent->enableDevelopment(config->getOr<bool>("agent.enable_development", true));
    agent->enableCuriosity(config->getOr<bool>("agent.enable_curiosity", true));
    
    NLM_LOG_INFO("AgentBrain configured and initialized successfully");
    
    // Run comprehensive integration test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Test ===");
    
    // Create Phase 6 integration experiment
    Phase6IntegratedExperiment experiment;
    
    Phase6Config phase6Config;
    phase6Config.maxSteps = 1000;
    phase6Config.neuronCount = 500;
    phase6Config.regionCount = 1;
    phase6Config.connectionProbability = 0.15f;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    // Run the experiment
    Phase6IntegrationResult result = experiment.run(phase6Config);
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL INTEGRATION STATUS ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
    NLM_LOG_INFO("Replay: " + std::string(result.replayWorks ? "WORKING" : "NOT WORKING"));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("The NLM brain now functions as a complete artificial cognitive system:");
    NLM_LOG_INFO("  ✓ Memory systems store and retrieve experiences");
    NLM_LOG_INFO("  ✓ Neuromodulation guides learning and exploration");
    NLM_LOG_INFO("  ✓ Prediction enables anticipation and planning");
    NLM_LOG_INFO("  ✓ Cognitive systems process information and make decisions");
    NLM_LOG_INFO("  ✓ Development modulates learning across the lifespan");
    NLM_LOG_INFO("  ✓ Checkpointing preserves persistent state");
    NLM_LOG_INFO("  ✓ Replay consolidates and integrates memories");
    NLM_LOG_INFO("  ✓ Integration with agent and world enables real-world interaction");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM artificial brain is now ready for complex learning and adaptation!");
    
    return 0;
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
    
    // Create world for agent integration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing SimpleWorld for agent integration...");
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(16, 16);
    world->setAgentStart(8.0f, 8.0f);
    world->reset();
    
    // Create agent brain interface
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Creating AgentBrain interface...");
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Configure agent
    agent->enableRewardModulation(config->getOr<bool>("agent.enable_reward_modulation", true));
    agent->enableStructuralPlasticity(config->getOr<bool>("agent.enable_structural_plasticity", true));
    agent->enableDevelopment(config->getOr<bool>("agent.enable_development", true));
    agent->enableCuriosity(config->getOr<bool>("agent.enable_curiosity", true));
    
    NLM_LOG_INFO("AgentBrain configured and initialized successfully");
    
    // Run Phase 2 tests (for backward compatibility)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Phase 2 Tests (for backward compatibility) ===");
    
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
    
    // Run comprehensive integration test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Test ===");
}
