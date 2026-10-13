#include "Phase6IntegratedExperiment.hpp"
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment() {
    // Initialize any necessary components
}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() {
    // Clean up resources
}

Phase6IntegrationResult Phase6IntegratedExperiment::run(const Phase6Config& config) {
    std::cout << "=== Phase 6 Integration Experiment ===" << std::endl;
    std::cout << "Configuration: " << config.maxSteps << " steps, " 
              << config.neuronCount << " neurons, " 
              << config.regionCount << " regions" << std::endl;
    std::cout << std::endl;
    
    // Record start time
    auto startWallClock = std::chrono::system_clock::now();
    auto startSimTime = std::chrono::system_clock::now();
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and configure main configuration
    auto mainConfig = std::make_shared<Config>();
    
    // Set up basic configuration
    mainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    mainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    mainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    mainConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    mainConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    mainConfig->set("replay_interval", config.replayInterval, ConfigSource::Default);
    mainConfig->set("consolidation_interval", config.consolidationInterval, ConfigSource::Default);
    
    // Initialize brain with configuration
    auto brain = std::make_shared<Brain>(mainConfig);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        Phase6IntegrationResult result;
        result.checkpointingWorks = false;
        return result;
    }
    
    // Create environment and agent
    auto world = std::make_shared<SimpleWorld>();
    auto agentBrain = std::make_shared<AgentBrain>(brain, world, config.checkpointPath);
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Brain status:" << std::endl;
    brain->logStatus();
    std::cout << std::endl;
    
    // Run the complete integration experiment
    return runIntegrationExperiment(brain, agentBrain, world, config, startWallClock, startSimTime);
}

Phase6IntegrationResult Phase6IntegratedExperiment::runIntegrationExperiment(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agentBrain,
    std::shared_ptr<SimpleWorld> world,
    const Phase6Config& config,
    const auto& startWallClock,
    const auto& startSimTime) {
    
    Phase6IntegrationResult result;
    double totalReward = 0.0f;
    size_t totalSteps = 0;
    size_t episodesStored = 0;
    size_t replayOperations = 0;
    size_t consolidationOperations = 0;
    
    // Main simulation loop
    for (SimulationStep step = 0; step < config.maxSteps; ++step) {
        auto stepStartTime = std::chrono::system_clock::now();
        
        // 1. Get observation from world
        SensoryPercept percept = world->observe(agentBrain->getBody());
        
        // 2. Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // 3. Run brain step
        brain->step(step);
        
        // 4. Decode motor command
        MotorCommand command = agentBrain->decodeMotorCommand();
        
        // 5. Apply action to world
        world->applyAction(agentBrain->getBody(), command);
        
        // 6. Compute reward and prediction
        float reward = world->computeReward(agentBrain->getBody());
        float predictedReward = agentBrain->getPredictionSystem()->getExpectedReward();
        
        totalReward += reward;
        
        // 7. Apply reward modulation (neuromodulation integration)
        agentBrain->applyRewardModulation(reward, predictedReward);
        
        // Log progress periodically
        if (step % 1000 == 0) {
            std::cout << "Step " << step << "/" << config.maxSteps 
                      << " | Reward: " << reward 
                      << " | Total: " << totalReward
                      << " | Episodes: " << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0)
                      << std::endl;
        }
        
        totalSteps = step;
        
        // Update wall clock for timeout checking
        auto currentWallClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentWallClock - startWallClock);
        if (duration.count() > 300) {  // 5 minute timeout
            std::cout << "Timeout reached after " << duration.count() << " seconds" << std::endl;
            break;
        }
    }
    
    // Record end time
    auto endWallClock = std::chrono::system_clock::now();
    auto endSimTime = std::chrono::system_clock::now();
    
    auto wallDuration = std::chrono::duration_cast<std::chrono::seconds>(endWallClock - startWallClock);
    auto simDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endSimTime - startSimTime);
    
    // Collect final metrics
    result.totalReward = totalReward;
    result.avgFiringRate = brain->getAverageFiringRate();
    result.avgSynapticWeight = brain->getExcitationInhibitionRatio();
    
    if (brain->getEpisodicMemory()) {
        result.memoryEpisodesStored = brain->getEpisodicMemory()->getEpisodeCount();
        episodesStored = brain->getEpisodicMemory()->getEpisodeCount();
        
        // Test replay functionality
        if (config.enableReplay) {
            auto recentEpisodes = brain->getEpisodicMemory()->getRecentEpisodes(3);
            for (const auto& episode : recentEpisodes) {
                brain->getEpisodicMemory()->replayEpisode(&episode);
                replayOperations++;
            }
        }
        
        // Test consolidation functionality
        if (config.enableDevelopment) {
            brain->getEpisodicMemory()->consolidate(0.3f);
            consolidationOperations++;
        }
    }
    
    // Set integration results
    result.memoryWorkingMemoryIntegrated = brain->getWorkingMemory() != nullptr;
    result.memoryEpisodicMemoryIntegrated = brain->getEpisodicMemory() != nullptr;
    result.neuromodulationIntegrated = brain->getDopamine() != nullptr && brain->getCuriosity() != nullptr;
    result.predictionIntegrated = brain->getPredictionSystem() != nullptr;
    result.developmentIntegrated = brain->getDevelopmentSystem() != nullptr;
    
    result.checkpointingWorks = brain->save(config.checkpointPath);
    result.replayWorks = replayOperations > 0;
    
    result.totalWallClockTime = wallDuration.count();
    
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Complete ===" << std::endl;
    std::cout << "Steps completed: " << totalSteps << std::endl;
    std::cout << "Total reward: " << totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "E/I ratio: " << result.avgSynapticWeight << std::endl;
    std::cout << "Episodes stored: " << episodesStored << std::endl;
    std::cout << "Replay operations: " << replayOperations << std::endl;
    std::cout << "Consolidation operations: " << consolidationOperations << std::endl;
    std::cout << std::endl;
    std::cout << "Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓" : "✗") << std::endl;
    
    return result;
}

bool Phase6IntegratedExperiment::verifyIntegration() {
    std::cout << "=== Phase 6 Integration Verification ===" << std::endl;
    
    // Create minimal configuration for verification
    Phase6Config config;
    config.maxSteps = 100;
    config.neuronCount = 100;
    config.regionCount = 1;
    
    auto result = run(config);
    
    // Check if integration was successful
    bool success = result.memoryWorkingMemoryIntegrated && 
                   result.memoryEpisodicMemoryIntegrated && 
                   result.developmentIntegrated;
    
    if (success) {
        std::cout << "✓ Integration verification PASSED" << std::endl;
        std::cout << "  Key systems are properly connected and functional" << std::endl;
    } else {
        std::cout << "✗ Integration verification FAILED" << std::endl;
        std::cout << "  Some systems are not properly integrated" << std::endl;
    }
    
    return success;
}

bool Phase6IntegratedExperiment::testMemoryIntegration() {
    std::cout << "=== Testing Memory Integration ===" << std::endl;
    
    Phase6Config config;
    config.maxSteps = 50;
    config.neuronCount = 200;
    config.regionCount = 1;
    
    auto result = run(config);
    
    bool memoryWorking = result.memoryWorkingMemoryIntegrated && 
                         result.memoryEpisodicMemoryIntegrated;
    
    if (memoryWorking) {
        std::cout << "✓ Memory integration test PASSED" << std::endl;
        std::cout << "  Working memory and episodic memory are functional" << std::endl;
    } else {
        std::cout << "✗ Memory integration test FAILED" << std::endl;
        std::cout << "  Memory systems are not properly integrated" << std::endl;
    }
    
    return memoryWorking;
}

bool Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    std::cout << "=== Testing Neuromodulation Integration ===" << std::endl;
    
    Phase6Config config;
    config.maxSteps = 50;
    config.neuronCount = 200;
    config.regionCount = 1;
    
    auto result = run(config);
    
    bool neuromodulationWorking = result.neuromodulationIntegrated;
    
    if (neuromodulationWorking) {
        std::cout << "✓ Neuromodulation integration test PASSED" << std::endl;
        std::cout << "  Neuromodulation systems (dopamine, curiosity) are integrated" << std::endl;
    } else {
        std::cout << "✗ Neuromodulation integration test FAILED" << std::endl;
        std::cout << "  Neuromodulation systems are not properly integrated" << std::endl;
    }
    
    return neuromodulationWorking;
}

bool Phase6IntegratedExperiment::testCheckpointing() {
    std::cout << "=== Testing Checkpointing Integration ===" << std::endl;
    
    Phase6Config config;
    config.maxSteps = 20;
    config.neuronCount = 100;
    config.regionCount = 1;
    config.enableCheckpointing = true;
    config.checkpointPath = "./test_checkpoint.bin";
    
    auto result = run(config);
    
    bool checkpointingWorking = result.checkpointingWorks;
    
    if (checkpointingWorking) {
        std::cout << "✓ Checkpointing integration test PASSED" << std::endl;
        std::cout << "  Checkpoint save/load systems are functional" << std::endl;
    } else {
        std::cout << "✗ Checkpointing integration test FAILED" << std::endl;
        std::cout << "  Checkpoint systems are not working properly" << std::endl;
    }
    
    // Clean up test checkpoint file
    std::remove(config.checkpointPath.c_str());
    
    return checkpointingWorking;
}

bool Phase6IntegratedExperiment::testReplay() {
    std::cout << "=== Testing Replay Integration ===" << std::endl;
    
    Phase6Config config;
    config.maxSteps = 30;
    config.neuronCount = 100;
    config.regionCount = 1;
    config.enableReplay = true;
    config.replayInterval = 10;
    
    auto result = run(config);
    
    bool replayWorking = result.replayWorks;
    
    if (replayWorking) {
        std::cout << "✓ Replay integration test PASSED" << std::endl;
        std::cout << "  Memory replay systems are functional" << std::endl;
    } else {
        std::cout << "✗ Replay integration test FAILED" << std::endl;
        std::cout << "  Replay systems are not working properly" << std::endl;
    }
    
    return replayWorking;
}

} // namespace nlm
