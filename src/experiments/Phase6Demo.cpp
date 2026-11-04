// NLM Phase 6 Comprehensive Demo
// Demonstrates all Phase 6 integration features with advanced CLI support

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace nlm;

// Demo configuration structure
struct DemoConfig {
    size_t neuronCount = 500;
    size_t maxSteps = 3000;
    double timestep = 0.001;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableCognition = true;
    std::string checkpointPath = "./checkpoints";
    
    // Performance settings
    bool enableProfiling = false;
    bool optimizedMode = true;
    
    // Debug settings
    bool verbose = true;
    bool showProgress = true;
};

// Demo results structure
struct DemoResults {
    double totalReward = 0.0;
    double avgFiringRate = 0.0;
    size_t episodesStored = 0;
    double dopamineLevel = 0.0;
    double curiosityLevel = 0.0;
    double noveltyLevel = 0.0;
    double predictionError = 0.0;
    float learnedPreference = 0.0f;
    
    // Integration verification
    bool workingMemoryIntegrated = false;
    bool episodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool cognitionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    
    // Performance metrics
    double totalWallClockTime = 0.0;
    size_t totalSpikesGenerated = 0;
    size_t totalSynapticEvents = 0;
    
    // Learning metrics
    float learningRate = 0.0f;
    float plasticityEfficiency = 0.0f;
    size_t adaptationSteps = 0;
    
    // System metrics
    float memoryUtilization = 0.0f;
    float computationalEfficiency = 0.0f;
    float integrationScore = 0.0f;
};

// Demo runner class
class Phase6Demo {
public:
    Phase6Demo(const DemoConfig& config) : config(config) {
        initializeDemo();
    }
    
    ~Phase6Demo() = default;
    
    // Run the complete demo
    DemoResults run() {
        DemoResults results;
        
        auto startWall = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== NLM Phase 6 Comprehensive Demo ===");
        NLM_LOG_INFO("Running comprehensive integration test with all Phase 6 systems...");
        
        // Initialize configuration
        auto cfg = createConfiguration();
        
        // Create brain with optimized settings
        auto brain = createOptimizedBrain(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return results;
        }
        
        // Create world
        SimpleWorld world;
        world.configure(16, 16, 16, 16);
        world.reset();
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(config.enableDevelopment);
        agent.enableDevelopment(config.enableDevelopment);
        agent.enableCuriosity(true);
        agent.enableNovelty(true);
        
        NLM_LOG_INFO("Brain, world, and agent initialized successfully");
        
        // Run simulation
        runSimulation(brain, &agent, world, results);
        
        // Test checkpointing
        if (config.enableCheckpointing) {
            testCheckpointing(brain, config.checkpointPath, results);
        }
        
        // Test replay
        if (config.enableReplay) {
            testReplay(brain, results);
        }
        
        // Calculate final metrics
        calculateDemoResults(brain, agent, results);
        
        auto endWall = std::chrono::high_resolution_clock::now();
        results.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
        
        // Verify integration
        verifyIntegration(brain, results);
        
        NLM_LOG_INFO("=== Demo Complete ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(results.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(results.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(results.episodesStored));
        NLM_LOG_INFO("Wall time: " + std::to_string(results.totalWallClockTime) + "s");
        
        return results;
    }
    
private:
    DemoConfig config;
    std::shared_ptr<Config> globalConfig;
    
    void initializeDemo() {
        NLM_LOG_INFO("Initializing Phase 6 Demo Environment...");
        
        // Setup logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Demo configuration:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
        NLM_LOG_INFO("  Max steps: " + std::to_string(config.maxSteps));
        NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Replay: " + std::string(config.enableReplay ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Neuromodulation: " + std::string(config.enableNeuromodulation ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Cognition: " + std::string(config.enableCognition ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Optimized mode: " + std::string(config.optimizedMode ? "enabled" : "disabled"));
    }
    
    std::shared_ptr<Config> createConfiguration() {
        auto cfg = std::make_shared<Config>();
        
        // Set configuration values
        cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
        cfg->set("simulation_timestep", config.timestep);
        cfg->set("connection_probability", 0.1f);
        
        // STDP parameters
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Development
        cfg->set("development_enabled", config.enableDevelopment);
        cfg->set("curiosity_enabled", config.enableNeuromodulation);
        cfg->set("novelty_enabled", config.enableNeuromodulation);
        
        // Checkpointing
        cfg->set("checkpoint_dir", config.checkpointPath);
        cfg->set("checkpoint_count", 10000);
        cfg->set("checkpoint_compressed", true);
        
        // Replay and consolidation
        cfg->set("replay_interval", 100);
        cfg->set("consolidation_interval", 1000);
        
        // Memory capacity
        cfg->set("working_memory_capacity", config.neuronCount / 10);
        cfg->set("episodic_memory_max_episodes", 1000);
        
        // Cognition
        cfg->set("planning_depth", 5);
        cfg->set("attention_excitation_strength", 1.5f);
        cfg->set("attention_inhibition_strength", 0.5f);
        
        return cfg;
    }
    
    std::shared_ptr<Brain> createOptimizedBrain(std::shared_ptr<Config> cfg) {
        // In full implementation, would create optimized brain
        // For now, use standard brain
        return std::make_shared<Brain>(cfg);
    }
    
    void runSimulation(std::shared_ptr<Brain> brain, 
                      AgentBrain* agent,
                      SimpleWorld& world,
                      DemoResults& results) {
        NLM_LOG_INFO("Starting main simulation loop...");
        
        // Simulation statistics
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        size_t synapticEvents = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Get observation
            world.update(config.timestep);
            SensoryPercept percept = world.getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Apply sensory input to brain
            brain->receiveSensoryInput(SensoryInput(percept));
            
            // Brain step
            brain->step(step, step * config.timestep);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            ActionResult actionResult = world.applyMotorCommand(cmd, step * config.timestep);
            
            // Compute reward
            float reward = actionResult.reward;
            totalReward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config.enableDevelopment) {
                agent->updateDevelopment(config.timestep);
            }
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Count synaptic events
            synapticEvents += brain->getPendingSpikeEventCount();
            
            // Progress reporting
            if (config.showProgress && step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(totalReward / (step + 1)) +
                            " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                            " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        // Store results
        results.totalReward = totalReward / config.maxSteps;
        results.avgFiringRate = totalFiringRate / config.maxSteps;
        results.totalSpikesGenerated = brain->getTotalSpikeCount();
        results.totalSynapticEvents = synapticEvents;
        
        NLM_LOG_INFO("Simulation completed: " + std::to_string(config.maxSteps) + " steps");
    }
    
    void testCheckpointing(std::shared_ptr<Brain> brain, 
                          const std::string& path,
                          DemoResults& results) {
        NLM_LOG_INFO("=== Testing Checkpoint System ===");
        
        // Save checkpoint
        if (brain->save(path)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
            
            // Create new brain and load
            auto cfg = globalConfig;
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (brain2->load(path)) {
                NLM_LOG_INFO("Checkpoint loaded successfully");
                results.checkpointingWorks = true;
                
                // Compare metrics
                size_t spikes2 = brain2->getTotalSpikeCount();
                NLM_LOG_INFO("Original brain spikes: " + std::to_string(brain->getTotalSpikeCount()));
                NLM_LOG_INFO("Reloaded brain spikes: " + std::to_string(spikes2));
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint");
            }
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    void testReplay(std::shared_ptr<Brain> brain, DemoResults& results) {
        NLM_LOG_INFO("=== Testing Memory Replay ===");
        
        auto* em = brain->getEpisodicMemory();
        if (!em) {
            NLM_LOG_ERROR("Episodic memory not available");
            return;
        }
        
        // Run some steps to create episodes
        for (int i = 0; i < 200; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check if episodes exist for replay
        size_t episodeCount = em->getEpisodeCount();
        if (episodeCount > 0) {
            NLM_LOG_INFO("[PASS] Episodes available for replay: " + std::to_string(episodeCount));
            results.episodesStored = static_cast<size_t>(episodeCount);
            
            // Get episodes for replay
            auto episodes = em->getEpisodesForReplay(3);
            if (!episodes.empty()) {
                NLM_LOG_INFO("[PASS] Replay system can retrieve episodes");
            }
        } else {
            NLM_LOG_INFO("[INFO] No episodes available for replay");
        }
    }
    
    void calculateDemoResults(std::shared_ptr<Brain> brain, 
                             AgentBrain& agent,
                             DemoResults& results) {
        NLM_LOG_INFO("Calculating demo results...");
        
        // Get memory systems
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionError();
        
        // Calculate memory utilization
        if (wm) {
            results.memoryUtilization = wm->getActiveTraces() / wm->getCapacity();
        }
        
        // Calculate neuromodulation levels
        if (dopamine) {
            results.dopamineLevel = dopamine->getLevel();
        }
        if (curiosity) {
            results.curiosityLevel = curiosity->getLevel();
        }
        if (novelty) {
            results.noveltyLevel = novelty->getLevel();
        }
        if (predictionError) {
            results.predictionError = predictionError->getError();
        }
        
        // Get learned preference
        results.learnedPreference = brain->getLearnedPreference();
        
        // Calculate learning metrics
        results.adaptationSteps = static_cast<size_t>(config.maxSteps / 10); // Estimate
        results.plasticityEfficiency = 0.85f; // Estimate
        
        // Calculate integration score
        float integrationScore = 0.0f;
        integrationScore += results.workingMemoryIntegrated ? 0.2f : 0.0f;
        integrationScore += results.episodicMemoryIntegrated ? 0.2f : 0.0f;
        integrationScore += results.neuromodulationIntegrated ? 0.2f : 0.0f;
        integrationScore += results.predictionIntegrated ? 0.2f : 0.0f;
        integrationScore += results.cognitionIntegrated ? 0.2f : 0.0f;
        
        results.integrationScore = integrationScore;
        
        // Calculate computational efficiency
        double efficiency = 1.0 / (1.0 + (results.totalWallClockTime / config.maxSteps));
        results.computationalEfficiency = static_cast<float>(efficiency);
        
        NLM_LOG_INFO("Demo results calculated:");
        NLM_LOG_INFO("  Integration score: " + std::to_string(results.integrationScore * 100) + "%");
        NLM_LOG_INFO("  Computational efficiency: " + std::to_string(results.computationalEfficiency * 100) + "%");
        NLM_LOG_INFO("  Memory utilization: " + std::to_string(results.memoryUtilization * 100) + "%");
    }
    
    void verifyIntegration(std::shared_ptr<Brain> brain, DemoResults& results) {
        NLM_LOG_INFO("=== Integration Verification ===");
        
        // Verify each system is properly integrated
        results.workingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
        results.episodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
        results.neuromodulationIntegrated = (brain->getDopamine() != nullptr && brain->getCuriosity() != nullptr);
        results.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        results.cognitionIntegrated = (brain->getPlanner() != nullptr && brain->getConceptFormation() != nullptr && brain->getAttention() != nullptr);
        results.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        NLM_LOG_INFO("Integration status:");
        NLM_LOG_INFO("  Working Memory: " + std::string(results.workingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("  Episodic Memory: " + std::string(results.episodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("  Neuromodulation: " + std::string(results.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("  Prediction: " + std::string(results.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("  Cognition: " + std::string(results.cognitionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("  Development: " + std::string(results.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    }
};

// Demo main function
int main_demo(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Comprehensive Demo ===" << std::endl;
    std::cout << "Running comprehensive integration test with enhanced CLI support." << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments for demo config
    DemoConfig config;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        }
        else if (arg.substr(0, 9) == "--neurons=") {
            config.neuronCount = std::stoul(arg.substr(9));
        }
        else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        }
        else if (arg.substr(0, 7) == "--steps=") {
            config.maxSteps = std::stoul(arg.substr(7));
        }
        else if (arg == "--checkpoint-dir" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
        else if (arg.substr(0, 20) == "--checkpoint-dir=") {
            config.checkpointPath = arg.substr(20);
        }
        else if (arg == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        }
        else if (arg == "--enable-replay") {
            config.enableReplay = true;
        }
        else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        }
        else if (arg == "--enable-neuromodulation") {
            config.enableNeuromodulation = true;
        }
        else if (arg == "--enable-cognition") {
            config.enableCognition = true;
        }
        else if (arg == "--no-progress") {
            config.showProgress = false;
        }
        else if (arg == "--quiet") {
            config.verbose = false;
            config.showProgress = false;
        }
        else if (arg == "--help" || arg == "-h") {
            printDemoHelp();
            return 0;
        }
    }
    
    // Create and run demo
    Phase6Demo demo(config);
    DemoResults results = demo.run();
    
    // Display final results
    printDemoResults(results);
    
    // Return success
    return 0;
}

// Demo help text
void printDemoHelp() {
    std::cout << "=== NLM Phase 6 Comprehensive Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --neurons N              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S                Maximum simulation steps (default: 3000)" << std::endl;
    std::cout << "  --checkpoint-dir DIR      Directory for checkpoints (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable-checkpointing    Enable checkpoint save/load" << std::endl;
    std::cout << "  --enable-replay           Enable memory replay" << std::endl;
    std::cout << "  --enable-development      Enable developmental plasticity" << std::endl;
    std::cout << "  --enable-neuromodulation   Enable neuromodulation systems" << std::endl;
    std::cout << "  --enable-cognition        Enable cognitive systems" << std::endl;
    std::cout << "  --no-progress             Suppress progress reporting" << std::endl;
    std::cout << "  --quiet                   Suppress all output except errors" << std::endl;
    std::cout << "  -h, --help               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --enable-checkpointing --enable-replay --checkpoint-dir ./simulations" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 200 --steps 2000 --no-progress" << std::endl;
    std::cout << std::endl;
}

// Print demo results
void printDemoResults(const DemoResults& results) {
    std::cout << std::endl;
    std::cout << "=== Demo Results ===" << std::endl;
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "  Total reward: " << results.totalReward << std::endl;
    std::cout << "  Average firing rate: " << results.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Episodes stored: " << results.episodesStored << std::endl;
    std::cout << "  Neuromodulation levels:" << std::endl;
    std::cout << "    Dopamine: " << results.dopamineLevel << std::endl;
    std::cout << "    Curiosity: " << results.curiosityLevel << std::endl;
    std::cout << "    Novelty: " << results.noveltyLevel << std::endl;
    std::cout << "    Prediction error: " << results.predictionError << std::endl;
    std::cout << "  Learned preference: " << results.learnedPreference << std::endl;
    std::cout << "  Integration score: " << (results.integrationScore * 100) << "%" << std::endl;
    std::cout << "  Computational efficiency: " << (results.computationalEfficiency * 100) << "%" << std::endl;
    std::cout << "  Memory utilization: " << (results.memoryUtilization * 100) << "%" << std::endl;
    std::cout << std::endl;
    std::cout << "System Integration:" << std::endl;
    std::cout << "  Working Memory: " << (results.workingMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Episodic Memory: " << (results.episodicMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Neuromodulation: " << (results.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Prediction: " << (results.predictionIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Cognition: " << (results.cognitionIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Development: " << (results.developmentIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Checkpointing: " << (results.checkpointingWorks ? "✓" : "✗") << std::endl;
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
}

// Entry point for demo
int main(int argc, char* argv[]) {
    // Redirect to demo main function
    return main_demo(argc, argv);
}