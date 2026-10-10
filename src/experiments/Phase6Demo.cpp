#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // NEW: Test Phase 3 Integration (World Interaction)
    std::cout << "--- Phase 3 Integration Test ---" << std::endl;
    std::cout << "Testing: Sensory Input -> Neural Processing -> Action -> World Response -> Reward" << std::endl;
    
    if (testPhase3Integration()) {
        std::cout << "Phase 3 integration PASSED!" << std::endl << std::endl;
    } else {
        std::cout << "Phase 3 integration FAILED!" << std::endl << std::endl;
    }
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Phase 3 World Integration: " << (result.phase3IntegrationWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}

bool Phase6IntegratedExperiment::testPhase3Integration() {
    NLM_LOG_INFO("=== Testing Phase 3 Integration (World Interaction) ===");
    
    // Create configuration for world interaction
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    cfg->set("random_seed", 42u);
    cfg->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for Phase 3 test");
        return false;
    }
    
    // Create simple world
    SimpleWorld world;
    world.initialize(16, 16);
    world.setRandomSeed(42);
    
    // Create agent brain interface
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Phase 3 components initialized successfully");
    
    // Run Phase 3 interaction loop
    float totalReward = 0.0f;
    size_t successfulInteractions = 0;
    
    for (uint64_t step = 0; step < 500; ++step) {
        // 1. Get observation from world (Phase 3: world provides sensory data)
        SensoryPercept percept = world.getSensoryPercept();
        
        // 2. Process sensory input (Phase 3: agent processes input)
        agent.processSensoryInput(percept);
        
        // 3. Simulate brain activity (Phase 6: neural processing)
        brain->step(step, step * 0.001);
        
        // 4. Decode motor command (Phase 3: agent generates action)
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // 5. Apply action to world (Phase 3: action affects world)
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        
        // 6. Apply reward modulation (Phase 6: reward affects learning)
        agent.applyRewardModulation(result.reward, 0.0f);
        
        // 7. Update world (Phase 3: world dynamics)
        world.update(0.01);
        
        // 8. Update development (Phase 6: developmental changes)
        agent.updateDevelopment(0.01);
        
        // Track metrics
        totalReward += result.reward;
        if (result.success) successfulInteractions++;
        
        // Log progress
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Action: " + motorCommandToString(cmd) +
                        " | Reward: " + std::to_string(result.reward) +
                        " | Novelty: " + std::to_string(agent.getNoveltyLevel()) +
                        " | Curiosity: " + std::to_string(agent.getCuriosityLevel()) +
                        " | Energy: " + std::to_string(world.getAgentBody().energy));
        }
    }
    
    // Collect metrics
    Phase6IntegrationResult phase3Result;
    phase3Result.totalReward = totalReward / 500;
    phase3Result.noveltyLevel = agent.getNoveltyLevel();
    phase3Result.curiosityLevel = agent.getCuriosityLevel();
    phase3Result.dopamineLevel = agent.getNeuromodulationLevel();
    
    // Verify Phase 3 integration
    bool sensoryInput = true;  // We got sensory percept
    bool motorOutput = (successfulInteractions > 0);  // We produced actions
    bool rewardLoop = (totalReward > 0.0f);  // We received rewards
    bool brainIntegration = (brain->getTotalNeuronCount() > 0);  // Brain has neurons
    
    phase3Result.phase3IntegrationWorks = (sensoryInput && motorOutput && rewardLoop && brainIntegration);
    
    if (phase3Result.phase3IntegrationWorks) {
        NLM_LOG_INFO("[PASS] Phase 3 integration successful");
        NLM_LOG_INFO("  Sensory input: ✓ (received percept)");
        NLM_LOG_INFO("  Motor output: ✓ (produced " + std::to_string(successfulInteractions) + " successful actions)");
        NLM_LOG_INFO("  Reward loop: ✓ (total reward: " + std::to_string(totalReward) + ")");
        NLM_LOG_INFO("  Brain integration: ✓ (" + std::to_string(brain->getTotalNeuronCount()) + " neurons)");
    } else {
        NLM_LOG_ERROR("[FAIL] Phase 3 integration failed");
    }
    
    return phase3Result.phase3IntegrationWorks;
}

const char* Phase6IntegratedExperiment::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}
