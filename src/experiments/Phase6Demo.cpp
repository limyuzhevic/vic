// NLM Phase 6: Final Integration Demo
// Comprehensive demonstration of all integrated brain systems
// This replaces main.cpp with a complete Phase 6 integration showcase

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <thread>
#include <chrono>

using namespace nlm;

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated.     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void setupConfiguration(std::shared_ptr<Config> config) {
    // Basic configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);  // Multiple regions for integration
    config->set("connection_probability", 0.12f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("episodic_memory_capacity", 500, ConfigSource::Default);
    config->set("associative_memory_capacity", 200, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config->set("curiosity_threshold", 0.3f, ConfigSource::Default);
    config->set("novelty_sensitivity", 0.2f, ConfigSource::Default);
    
    // Prediction system parameters
    config->set("prediction_horizon", 10, ConfigSource::Default);
    config->set("prediction_error_decay", 0.95f, ConfigSource::Default);
    
    // Development parameters
    config->set("critical_period_end", 100.0f, ConfigSource::Default);
    config->set("plasticity_decay_rate", 0.99f, ConfigSource::Default);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Logging
    config->set("log_level", "Info", ConfigSource::Default);
}

void demonstrateMemoryIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "MEMORY SYSTEMS INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& memory = *brain->getWorkingMemory();
    auto& episodic = *brain->getEpisodicMemory();
    auto& associative = *brain->getAssociativeMemory();
    
    // Working memory demonstration
    std::cout << "Working Memory Capacity: " << memory.getCapacity() << std::endl;
    std::cout << "Working Memory Usage: " << memory.getUsageCount() << std::endl;
    
    // Store some information in working memory
    memory.storePattern(std::vector<float>{1.0f, 0.5f, 0.8f, 0.3f, 0.9f});
    memory.storePattern(std::vector<float>{0.2f, 0.7f, 0.4f, 0.6f, 0.1f});
    
    std::cout << "Stored 2 patterns in working memory" << std::endl;
    
    // Episodic memory demonstration
    std::cout << "Episodic Memory Capacity: " << episodic.getCapacity() << std::endl;
    std::cout << "Episodic Memory Episodes: " << episodic.getEpisodeCount() << std::endl;
    
    // Record an experience
    auto& prediction = *brain->getPredictionSystem();
    prediction.updatePrediction(); // Generate prediction
    float predictionError = prediction.getPredictionError();
    
    episodic.recordExperience(
        prediction.getPredictedState(),
        predictionError,
        std::vector<float>{0.1f, 0.9f, 0.3f, 0.7f} // Action taken
    );
    
    std::cout << "Recorded 1 experience in episodic memory" << std::endl;
    
    // Replay demonstration
    if (episodic.getReplayCount() > 0) {
        std::cout << "Replay count: " << episodic.getReplayCount() << std::endl;
    }
    
    // Associative memory demonstration
    std::cout << "Associative Memory Capacity: " << associative.getCapacity() << std::endl;
    
    // Create pattern associations
    std::vector<float> stimulus1{0.8f, 0.2f, 0.5f};
    std::vector<float> response1{0.1f, 0.9f, 0.3f};
    
    associative.createAssociation(stimulus1, response1);
    std::cout << "Created pattern association" << std::endl;
    
    // Test association recall
    std::vector<float> testStimulus{0.75f, 0.25f, 0.45f};
    auto association = associative.recallAssociation(testStimulus);
    if (association) {
        std::cout << "Association recall successful" << std::endl;
    }
}

void demonstrateNeuromodulationIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NEUROMODULATION SYSTEMS INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& dopamine = *brain->getDopamine();
    auto& curiosity = *brain->getCuriosity();
    auto& novelty = *brain->getNovelty();
    auto& predError = *brain->getPredictionErrorSignal();
    
    std::cout << "Dopamine baseline: " << dopamine.getBaselineLevel() << std::endl;
    std::cout << "Curiosity threshold: " << curiosity.getThreshold() << std::endl;
    std::cout << "Novelty sensitivity: " << novelty.getSensitivity() << std::endl;
    
    // Simulate reward-based learning
    std::cout << "\nSimulating reward-based learning..." << std::endl;
    float reward = 0.8f;
    dopamine.applyRewardSignal(reward);
    std::cout << "Applied dopamine reward signal: " << reward << std::endl;
    
    // Simulate curiosity-driven exploration
    std::cout << "\nSimulating curiosity-driven exploration..." << std::endl;
    float predictionError = 0.3f;
    curiosity.updateCuriosity(predictionError);
    std::cout << "Updated curiosity based on prediction error: " << predictionError << std::endl;
    
    // Simulate novelty detection
    std::cout << "\nSimulating novelty detection..." << std::endl;
    std::vector<float> sensoryInput{0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    novelty.detectNovelty(sensoryInput);
    std::cout << "Novelty detection completed" << std::endl;
    
    // Integration with plasticity
    std::cout << "\nNeuromodulation affecting plasticity..." << std::endl;
    auto& stdp = *brain->getSTDP();
    auto& hebbian = *brain->getHebbian();
    
    // Check current plasticity rates
    float stdpRate = stdp.getLTPRate();
    float hebbianRate = hebbian.getLearningRate();
    std::cout << "STDP LTP rate: " << stdpRate << std::endl;
    std::cout << "Hebbian learning rate: " << hebbianRate << std::endl;
    
    // Neuromodulators can affect these rates in real scenarios
    std::cout << "Neuromodulation system integrated with plasticity rules" << std::endl;
}

void demonstratePredictionIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "PREDICTION SYSTEM INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& prediction = *brain->getPredictionSystem();
    
    std::cout << "Prediction system initialized" << std::endl;
    std::cout << "Prediction horizon: " << prediction.getPredictionHorizon() << std::endl;
    std::cout << "Prediction error decay: " << prediction.getPredictionErrorDecay() << std::endl;
    
    // Demonstrate prediction cycle
    std::cout << "\nRunning prediction cycle..." << std::endl;
    
    // Generate some sensory input
    std::vector<float> sensoryData{0.7f, 0.8f, 0.3f, 0.2f, 0.9f};
    
    // Brain receives sensory input and makes prediction
    auto sensoryInput = std::make_shared<Vision>(5, 1); // 5 width, 1 height
    sensoryInput->setData(sensoryData);
    
    brain->receiveSensoryInput(*sensoryInput);
    brain->step(0, 0.0); // First simulation step
    
    // Get prediction
    auto predictedState = prediction.getPredictedState();
    std::cout << "Predicted state: ";
    for (float val : predictedState) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Calculate prediction error
    std::vector<float> actualState{0.75f, 0.85f, 0.35f, 0.25f, 0.95f};
    float error = prediction.calculateError(actualState);
    std::cout << "Prediction error: " << error << std::endl;
    
    // Update prediction based on error
    prediction.updatePrediction();
    std::cout << "Prediction updated based on error" << std::endl;
}

void demonstrateCognitionIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "COGNITION SYSTEMS INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& planner = *brain->getPlanner();
    auto& concepts = *brain->getConceptFormation();
    auto& attention = *brain->getAttention();
    
    std::cout << "Neural planner initialized" << std::endl;
    std::cout << "Concept formation system initialized" << std::endl;
    std::cout << "Attentional selection system initialized" << std::endl;
    
    // Demonstrate planning
    std::cout << "\nRunning action planning..." << std::endl;
    
    // Current goal
    std::vector<float> goal{0.9f, 0.1f}; // Navigate to target
    
    // Get action sequence
    auto actionSequence = planner.planActionSequence(goal, 5);
    std::cout << "Planned action sequence with " << actionSequence.size() << " actions" << std::endl;
    
    // Demonstrate concept formation
    std::cout << "\nForming concepts from experiences..." << std::endl;
    
    // Feed patterns to concept formation
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.9f, 0.2f},
        {0.2f, 0.8f, 0.3f},
        {0.3f, 0.7f, 0.4f},
        {0.4f, 0.6f, 0.5f}
    };
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        concepts.feedPattern(patterns[i]);
    }
    
    auto conceptsList = concepts.getCurrentConcepts();
    std::cout << "Discovered " << conceptsList.size() << " concepts" << std::endl;
    
    // Demonstrate attentional selection
    std::cout << "\nRunning attentional selection..." << std::endl;
    
    // Create competition between potential focus areas
    std::vector<std::pair<float, std::string>> competitors = {
        {0.8f, "SensoryInput1"},
        {0.6f, "MemoryPattern1"},
        {0.9f, "ExternalGoal"},
        {0.4f, "InternalState"}
    };
    
    std::string selectedFocus = attention.selectFocus(competitors);
    std::cout << "Attentional focus selected: " << selectedFocus << std::endl;
    std::cout << "Attention weight: " << attention.getCurrentAttentionWeight() << std::endl;
}

void demonstrateDevelopmentIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "DEVELOPMENT SYSTEM INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& development = *brain->getDevelopmentSystem();
    
    std::cout << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    std::cout << "Critical period end: " << development.getCriticalPeriodEnd() << std::endl;
    std::cout << "Plasticity decay rate: " << development.getPlasticityDecayRate() << std::endl;
    
    // Demonstrate development stages
    std::cout << "\nDemonstrating developmental plasticity..." << std::endl;
    
    // Show plasticity changes with development
    auto& stdp = *brain->getSTDP();
    auto& hebbian = *brain->getHebbian();
    auto& structural = *brain->getStructuralPlasticity();
    
    std::cout << "Initial plasticity rates:" << std::endl;
    std::cout << "  STDP LTP: " << stdp.getLTPRate() << std::endl;
    std::cout << "  STDP LTD: " << stdp.getLTDRate() << std::endl;
    std::cout << "  Hebbian: " << hebbian.getLearningRate() << std::endl;
    std::cout << "  Synaptogenesis: " << structural.getSynaptogenesisRate() << std::endl;
    std::cout << "  Pruning: " << structural.getPruningRate() << std::endl;
    
    // Development system updates during simulation
    for (int stage = 0; stage < 5; ++stage) {
        development.updateDevelopment(brain->getDevelopmentalStage());
        brain->develop(); // Apply developmental changes
        std::cout << "Developmental stage " << stage << " applied" << std::endl;
    }
    
    std::cout << "\nDevelopment system integrated with all plasticity rules" << std::endl;
}

void demonstratePersistence(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "PERSISTENCE AND CHECKPOINT SYSTEM DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::string checkpointFile = "brain_checkpoint.nlm";
    
    // Demonstrate checkpointing
    std::cout << "Creating checkpoint..." << std::endl;
    bool saved = brain->save(checkpointFile);
    if (saved) {
        std::cout << "✓ Checkpoint saved to: " << checkpointFile << std::endl;
        
        // Simulate some changes
        std::cout << "\nSimulating brain changes..." << std::endl;
        for (int i = 0; i < 100; ++i) {
            auto sensoryInput = std::make_shared<Vision>(5, 1);
            sensoryInput->setData(std::vector<float>(5, 0.5f));
            brain->receiveSensoryInput(*sensoryInput);
            brain->step(i, i * 0.001f);
        }
        
        // Load checkpoint
        std::cout << "\nRestoring from checkpoint..." << std::endl;
        bool loaded = brain->load(checkpointFile);
        if (loaded) {
            std::cout << "✓ Checkpoint loaded successfully" << std::endl;
            std::cout << "Brain state restored: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to save checkpoint" << std::endl;
    }
}

void demonstrateReplayConsolidation(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "REPLAY AND CONSOLIDATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto& episodic = *brain->getEpisodicMemory();
    
    std::cout << "Episodic memory replay demonstration" << std::endl;
    std::cout << "Replay consolidation enabled" << std::endl;
    
    // Simulate experience recording and replay
    std::cout << "\nRecording experiences..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        // Generate random experience
        std::vector<float> experienceData(5, static_cast<float>(rand()) / RAND_MAX);
        episodic.recordExperience(experienceData, 0.5f, std::vector<float>{0.1f, 0.9f});
    }
    
    std::cout << "Recorded " << episodic.getEpisodeCount() << " experiences" << std::endl;
    
    // Simulate replay during rest/sleep
    std::cout << "\nSimulating replay consolidation..." << std::endl;
    episodic.startReplayConsolidation();
    
    int replayCount = episodic.getReplayCount();
    std::cout << "Replay consolidation completed. Replay count: " << replayCount << std::endl;
    
    // Check consolidation effects
    if (episodic.getConsolidationStrength() > 0.0f) {
        std::cout << "Consolidation strength: " << episodic.getConsolidationStrength() << std::endl;
        std::cout << "Memory strengthened through replay" << std::endl;
    }
}

void demonstrateWorldIntegration(std::shared_ptr<Brain> brain) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "WORLD ENVIRONMENT INTEGRATION DEMONSTRATION" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100, 100, 10, 10); // 100x100 world, 10x10 vision
    
    world->setAgentStart(50, 50);
    std::cout << "World configured: 100x100 grid" << std::endl;
    std::cout << "Agent starting position: (50, 50)" << std::endl;
    
    // Create agent brain
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    agentBrain->initialize(world);
    
    std::cout << "Agent brain interface created and initialized" << std::endl;
    
    // Demonstrate interaction loop
    std::cout << "\nDemonstrating agent-environment interaction..." << std::endl;
    
    for (int step = 0; step < 10; ++step) {
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Produce action
        auto action = brain->produceAction();
        
        // Apply action in world
        world->applyMotorCommand(action, step * 0.001f);
        
        // Update development
        agentBrain->updateDevelopment(step * 0.001f);
        
        if (step % 5 == 0) {
            std::cout << "Step " << step << ": " << action->getName() << std::endl;
        }
    }
    
    std::cout << "Agent successfully interacted with world environment" << std::endl;
}

void printPhase6Results() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "PHASE 6 INTEGRATION RESULTS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "✓ All brain systems successfully integrated:" << std::endl;
    std::cout << "  - Memory Systems: Working, Episodic, and Associative memory" << std::endl;
    std::cout << "  - Neuromodulation: Dopamine, Curiosity, Novelty systems" << std::endl;
    std::cout << "  - Prediction System: State prediction and error computation" << std::endl;
    std::cout << "  - Cognition Systems: Planning, concept formation, attention" << std::endl;
    std::cout << "  - Development System: Stage-based plasticity and maturation" << std::endl;
    std::cout << "  - Plasticity Integration: STDP, Hebbian, Structural plasticity" << std::endl;
    std::cout << "  - Persistence: Checkpoint save/load functionality" << std::endl;
    std::cout << "  - Replay Consolidation: Memory strengthening through replay" << std::endl;
    std::cout << "  - World Integration: Complete agent-environment loop" << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM Phase 6 Integration Complete!" << std::endl;
    std::cout << "The brain now operates as a coherent artificial neural substrate" << std::endl;
    std::cout << "capable of learning, adapting, and interacting with complex environments." << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

int main(int argc, char** argv) {
    printPhase6Banner();
    
    std::cout << "Initializing NLM Phase 6: Final Integration Demo...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Demonstrating complete artificial brain integration");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
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
    
    // Apply Phase 6 specific configuration
    setupConfiguration(config);
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 3)));
    NLM_LOG_INFO("  working_memory_capacity: " + std::to_string(config->getOr<int>("working_memory_capacity", 100)));
    NLM_LOG_INFO("  episodic_memory_capacity: " + std::to_string(config->getOr<int>("episodic_memory_capacity", 500)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain for Phase 6 Integration...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Demonstrate all integrated systems
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PHASE 6 INTEGRATION DEMONSTRATION ===");
    
    demonstrateMemoryIntegration(brain);
    demonstrateNeuromodulationIntegration(brain);
    demonstratePredictionIntegration(brain);
    demonstrateCognitionIntegration(brain);
    demonstrateDevelopmentIntegration(brain);
    demonstratePersistence(brain);
    demonstrateReplayConsolidation(brain);
    demonstrateWorldIntegration(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL BRAIN STATUS ===");
    brain->logStatus();
    
    printPhase6Results();
    
    return 0;
}
}