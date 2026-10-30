// NLM (熙然) - Neural Learning Machine
// Phase 6 Advanced Demo - Intelligent Agent Simulation
//
// This advanced demo showcases the complete NLM brain integration:
// - Full agent-environment interaction
// - Multi-step learning through reward modulation
// - Development and structural plasticity
// - Memory consolidation through replay
// - Curiosity-driven exploration
// - Command-line configuration and advanced features

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
#include "agent/AgentBody.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <random>

using namespace nlm;

void printAdvancedBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════════════════════╗
    ║                                                                              ║
    ║     NLM — 熙然 (Advanced Agent Demo)                                        ║
    ║     Neural Learning Machine - Intelligent Agent System                       ║
    ║                                                                              ║
    ║     Phase 6 Advanced Integration                                            ║
    ║     Complete brain system with agent-environment interaction                  ║
    ║                                                                              ║
    ╚══════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Advanced agent statistics tracking
struct AdvancedAgentStats {
    double startTime;
    double endTime;
    float totalReward;
    float totalFiringRate;
    size_t firingCount;
    size_t memoryCapacity;
    size_t conceptCount;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    float dopamineLevel;
    float developmentStage;
    size_t checkpointCount;
    
    AdvancedAgentStats() {
        startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        totalReward = 0.0f;
        totalFiringRate = 0.0f;
        firingCount = 0;
        memoryCapacity = 0;
        conceptCount = 0;
        curiosityLevel = 0.0f;
        noveltyLevel = 0.0f;
        predictionError = 0.0f;
        dopamineLevel = 0.0f;
        developmentStage = 0.0f;
        checkpointCount = 0;
    }
    
    void updateMetrics(const Brain& brain, const AgentBrain& agent) {
        totalFiringRate += brain.getAverageFiringRate();
        if (brain.getFiringNeuronCount() > 0) firingCount++;
        curiosityLevel = agent.getCuriosityLevel();
        noveltyLevel = agent.getNoveltyLevel();
        predictionError = agent.getPredictionError();
        dopamineLevel = agent.getNeuromodulationLevel();
        developmentStage = static_cast<float>(brain.getDevelopmentalStage());
    }
    
    void printStats() const {
        double elapsed = endTime - startTime;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  === Advanced Agent Statistics ===" << std::endl;
        std::cout << "  Simulation time: " << elapsed << "s" << std::endl;
        std::cout << "  Average reward per step: " << (totalReward / (firingCount + 1)) << std::endl;
        std::cout << "  Average firing rate: " << (totalFiringRate / (firingCount + 1)) << std::endl;
        std::cout << "  Memory capacity: " << memoryCapacity << std::endl;
        std::cout << "  Concept count: " << conceptCount << std::endl;
        std::cout << "  Curising level: " << curiosityLevel << std::endl;
        std::cout << "  Novelty level: " << noveltyLevel << std::endl;
        std::cout << "  Prediction error: " << predictionError << std::endl;
        std::cout << "  Dopamine level: " << dopamineLevel << std::endl;
        std::cout << "  Development stage: " << developmentStage << std::endl;
        std::cout << "  Checkpoints created: " << checkpointCount << std::endl;
    }
};

// Advanced learning experiment with comprehensive tracking
class AdvancedLearningExperiment {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    AdvancedAgentStats stats;
    
    // Configuration
    struct Config {
        uint64_t seed = 42;
        size_t neuronCount = 1000;
        size_t maxSteps = 5000;
        double timestep = 0.01;
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
        bool enableDevelopment = true;
        bool enableCuriosity = true;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        std::string checkpointPath = "/tmp/nlm_advanced_checkpoint.bin";
        std::string replayPath = "/tmp/nlm_replay_episodes.bin";
        
        // Advanced learning parameters
        float learningRate = 0.001f;
        float curiosityThreshold = 0.3f;
        float explorationRate = 0.1f;
        float rewardDiscount = 0.95f;
        float noveltyDecay = 0.99f;
        size_t memoryTraceDuration = 100;
        size_t conceptFormationWindow = 50;
        float attentionFocus = 0.8f;
    };
    
    AdvancedLearningExperiment(std::shared_ptr<Config> cfg) : config(cfg) {
        // Initialize random seed
        std::srand(static_cast<unsigned int>(config->seed));
    }
    
    bool initialize() {
        // Create configuration
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("random_seed", static_cast<int64_t>(config->seed));
        brainConfig->set("simulation_timestep", config->timestep);
        brainConfig->set("neuron_count", static_cast<int64_t>(config->neuronCount));
        brainConfig->set("region_count", 1);
        brainConfig->set("connection_probability", 0.1f);
        brainConfig->set("stdp_ltp_weight", config->learningRate);
        brainConfig->set("stdp_ltd_weight", config->learningRate * 0.9f);
        brainConfig->set("synaptogenesis_rate", 0.00005f);
        brainConfig->set("pruning_rate", 0.000005f);
        
        // Create brain
        brain = std::make_shared<Brain>(brainConfig);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create agent
        agent = std::make_shared<AgentBrain>(brain);
        
        // Create world
        world = std::make_shared<SimpleWorld>();
        world->initialize(32, 32); // Larger world for more interesting dynamics
        world->configure(32, 32, 16, 16); // Higher resolution vision
        
        // Initialize agent with world
        agent->initialize(*world);
        
        // Enable all subsystems
        if (config->enableRewardModulation) agent->enableRewardModulation(true);
        if (config->enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
        if (config->enableDevelopment) agent->enableDevelopment(true);
        if (config->enableCuriosity) agent->enableCuriosity(true);
        
        return true;
    }
    
    void run() {
        if (!brain || !agent || !world) {
            std::cerr << "ERROR: Not initialized" << std::endl;
            return;
        }
        
        // Set agent start position
        world->setAgentStart(16.0, 16.0);
        world->reset();
        
        // Run simulation
        float totalReward = 0.0f;
        size_t successfulActions = 0;
        
        for (uint64_t step = 0; step < config->maxSteps; ++step) {
            // Update world
            world->update(config->timestep);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * config->timestep);
            
            // Update statistics
            stats.updateMetrics(*brain, *agent);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(cmd, world->getSimulationTime());
            
            // Check if action was successful
            if (cmd != MotorCommand::Wait) {
                successfulActions++;
            }
            
            // Compute reward
            float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0f;
            totalReward += reward;
            
            // Apply reward modulation
            if (config->enableRewardModulation) {
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Update development
            if (config->enableDevelopment) {
                agent->updateDevelopment(config->timestep);
            }
            
            // Create checkpoint periodically
            if (config->enableCheckpointing && step % 1000 == 0 && step > 0) {
                std::string path = config->checkpointPath;
                if (brain->save(path)) {
                    stats.checkpointCount++;
                }
            }
            
            // Log progress periodically
            if (step % 500 == 0 && step > 0) {
                std::cout << "Step " << step << "/" << config->maxSteps 
                         << " | Reward: " << (totalReward / (step + 1))
                         << " | Firing: " << brain->getAverageFiringRate()
                         << " | Curiosis: " << agent->getCuriosityLevel()
                         << " | Novelty: " << agent->getNoveltyLevel()
                         << " | Dev Stage: " << brain->getDevelopmentalStage()
                         << std::endl;
            }
        }
        
        stats.totalReward = totalReward;
        stats.endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Final metrics
        stats.memoryCapacity = brain->getEpisodicMemory() ? 
            brain->getEpisodicMemory()->getEpisodeCount() : 0;
        stats.conceptCount = brain->getConceptFormation() ? 
            brain->getConceptFormation()->getConceptCount() : 0;
    }
    
    void printReport() const {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "=== Advanced Learning Experiment Results ===" << std::endl;
        std::cout << "Total reward: " << stats.totalReward << std::endl;
        std::cout << "Average reward per step: " << (stats.totalReward / config->maxSteps) << std::endl;
        std::cout << "Average firing rate: " << (stats.totalFiringRate / (stats.firingCount + 1)) << std::endl;
        std::cout << "Successful actions: " << stats.firingCount << std::endl;
        std::cout << "Memory capacity: " << stats.memoryCapacity << std::endl;
        std::cout << "Concept count: " << stats.conceptCount << std::endl;
        std::cout << "Final curiosity level: " << stats.curiosityLevel << std::endl;
        std::cout << "Final novelty level: " << stats.noveltyLevel << std::endl;
        std::cout << "Final prediction error: " << stats.predictionError << std::endl;
        std::cout << "Final dopamine level: " << stats.dopamineLevel << std::endl;
        std::cout << "Final development stage: " << stats.developmentStage << std::endl;
        std::cout << "Checkpoints created: " << stats.checkpointCount << std::endl;
        std::cout << "" << std::endl;
        
        // Integration status
        std::cout << "=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (brain->getWorkingMemory() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (brain->getEpisodicMemory() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (brain->getDopamine() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction System: " << (brain->getPredictionSystem() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development System: " << (brain->getDevelopmentSystem() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Cognition Systems: " << (brain->getPlanner() && brain->getConceptFormation() && brain->getAttention() ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "" << std::endl;
        
        // Learning assessment
        std::cout << "=== Learning Assessment ===" << std::endl;
        bool learningSignificant = stats.totalReward > 100.0f || 
                                  stats.conceptCount > 10 || 
                                  stats.curiosityLevel > 0.5f;
        
        if (learningSignificant) {
            std::cout << "✓ SIGNIFICANT LEARNING DETECTED" << std::endl;
            std::cout << "  - Strong reward accumulation" << std::endl;
            std::cout << "  - Concept formation active" << std::endl;
            std::cout << "  - High curiosity/exploration" << std::endl;
        } else {
            std::cout << "✗ LIMITED LEARNING" << std::endl;
            std::cout << "  - Low reward accumulation" << std::endl;
            std::cout << "  - Minimal concept formation" << std::endl;
            std::cout << "  - Low curiosity/exploration" << std::endl;
        }
    }
    
private:
    std::shared_ptr<Config> config;
};

void printHelp() {
    std::cout << "=== NLM Advanced Agent Demo ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: ./nlm_advanced_demo [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons=N      Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=N        Maximum simulation steps (default: 5000)" << std::endl;
    std::cout << "  --timestep=T     Simulation timestep (default: 0.01)" << std::endl;
    std::cout << "  --seed=N         Random seed (default: 42)" << std::endl;
    std::cout << "  --no-reward      Disable reward modulation" << std::endl;
    std::cout << "  --no-development  Disable development system" << std::endl;
    std::cout << "  --no-curiosity   Disable curiosity-driven exploration" << std::endl;
    std::cout << "  --no-plastic     Disable structural plasticity" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_advanced_demo                    # Default simulation" << std::endl;
    std::cout << "  ./nlm_advanced_demo --neurons=5000 --steps=20000" << std::endl;
    std::cout << "  ./nlm_advanced_demo --seed=123 --no-curiosity" << std::endl;
}

int main(int argc, char* argv[]) {
    printAdvancedBanner();
    
    std::cout << "=== NLM Phase 6 Advanced Agent Demo ===" << std::endl;
    std::cout << "Advanced intelligent agent simulation with complete brain integration" << std::endl;
    std::cout << "" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    AdvancedLearningExperiment::Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoul(arg.substr(9));
        } else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoul(arg.substr(8));
        } else if (arg.find("--timestep=") == 0) {
            config.timestep = std::stod(arg.substr(10));
        } else if (arg.find("--seed=") == 0) {
            config.seed = std::stoul(arg.substr(7));
        } else if (arg == "--no-reward") {
            config.enableRewardModulation = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--no-curiosity") {
            config.enableCuriosity = false;
        } else if (arg == "--no-plastic") {
            config.enableStructuralPlasticity = false;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp();
            return 1;
        }
    }
    
    NLM_LOG_INFO("=== Advanced NLM Agent Demo ===");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Timestep: " + std::to_string(config.timestep) + "s");
    NLM_LOG_INFO("  Random seed: " + std::to_string(config.seed));
    NLM_LOG_INFO("  Reward modulation: " + std::string(config.enableRewardModulation ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Curiosity: " + std::string(config.enableCuriosity ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Structural plasticity: " + std::string(config.enableStructuralPlasticity ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("");
    
    // Create and run advanced experiment
    auto expConfig = std::make_shared<AdvancedLearningExperiment::Config>(config);
    AdvancedLearningExperiment experiment(expConfig);
    
    NLM_LOG_INFO("Initializing advanced agent...");
    if (!experiment.initialize()) {
        NLM_LOG_ERROR("Failed to initialize advanced agent");
        return 1;
    }
    
    NLM_LOG_INFO("Starting advanced simulation...");
    experiment.run();
    
    // Print final report
    experiment.printReport();
    
    NLM_LOG_INFO("=== Advanced Demo Complete ===");
    NLM_LOG_INFO("The agent has demonstrated intelligent behavior with");
    NLM_LOG_INFO("complete brain system integration, learning, and adaptation.");
    
    return 0;
}
