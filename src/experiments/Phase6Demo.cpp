#include "Phase6Demo.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>

namespace nlm {

Phase6Demo::Phase6Demo() : logger_(Logger::getInstance()) {}

Phase6Demo::~Phase6Demo() = default;

Phase6DemoResult Phase6Demo::run(const Phase6DemoConfig& config) {
    Phase6DemoResult result;
    auto startWall = std::chrono::high_resolution_clock::now();
    
    logger_.setLevel(Logger::Level::Info);
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        logger_.logError("Failed to initialize brain");
        return result;
    }
    
    // Create simple world
    SimpleWorld world;
    world.initialize(config.worldWidth, config.worldHeight);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(config.enableDevelopment);
    agent.enableDevelopment(config.enableDevelopment);
    agent.enableCuriosity(true);
    
    logger_.logInfo("Brain and agent initialized successfully");
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Get observation
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        if (config.enableDevelopment) {
            agent.updateDevelopment(0.001);
        }
        
        // Collect metrics
        totalFiringRate += brain->getAverageFiringRate();
        if (brain->getFiringNeuronCount() > 0) firingCount++;
        
        // Periodic status
        if (step % 1000 == 0) {
            logger_.logInfo("Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(totalReward / (step + 1)) +
                           " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                           " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                               brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Collect final metrics
    result.totalReward = totalReward / config.maxSteps;
    result.avgFiringRate = totalFiringRate / config.maxSteps;
    result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
        static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
    result.noveltyLevel = agent.getNoveltyLevel();
    result.curiosityLevel = agent.getCuriosityLevel();
    result.dopamineLevel = agent.getNeuromodulationLevel();
    
    // Test checkpointing
    if (config.enableCheckpointing) {
        logger_.logInfo("Testing checkpoint save/load...");
        std::string path = "/tmp/nlm_checkpoint_test.bin";
        if (brain->save(path)) {
            logger_.logInfo("Checkpoint saved successfully");
            
            // Create new brain and load
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (brain2->load(path)) {
                logger_.logInfo("Checkpoint loaded successfully");
                result.checkpointWorks = true;
            } else {
                logger_.logError("Failed to load checkpoint");
            }
        } else {
            logger_.logError("Failed to save checkpoint");
        }
    }
    
    // Test replay
    if (config.enableReplay) {
        logger_.logInfo("Testing replay system...");
        if (brain->getEpisodicMemory() && brain->getEpisodicMemory()->getEpisodeCount() > 0) {
            auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(3);
            if (!episodes.empty()) {
                result.replayWorks = true;
                logger_.logInfo("[PASS] Replay system works");
            }
        }
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    
    // Verify integration
    result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    return result;
}

} // namespace nlm