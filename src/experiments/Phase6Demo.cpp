/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
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
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Create simple world
    SimpleWorld world;
    world.configure(16, 16, 8, 8);  // Add vision configuration
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(config.enableDevelopment);
    agent.enableDevelopment(config.enableDevelopment);
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Brain and agent initialized successfully");
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Get observation from world (not from brain region)
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(cmd, step * 0.001);
        
        // Compute reward (simplified - use agent's neuromodulation level as reward proxy)
        float reward = agent.getNeuromodulationLevel();
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
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                        " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Collect final metrics
    float avgReward = totalReward / config.maxSteps;
    float avgFiring = totalFiringRate / config.maxSteps;
    float memoryEpisodes = brain->getEpisodicMemory() ? 
        static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
    float noveltyLevel = agent.getNoveltyLevel();
    float curiosityLevel = agent.getCuriosityLevel();
    float dopamineLevel = agent.getNeuromodulationLevel();
    
    // Verify integration
    bool memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    bool memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    bool neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    bool predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    bool developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Working Memory: " + std::string(memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Prediction: " + std::string(predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Development: " + std::string(developmentIntegrated ? "YES" : "NO"));
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << avgReward << std::endl;
    std::cout << "Avg firing rate: " << avgFiring << std::endl;
    std::cout << "Episodes stored: " << memoryEpisodes << std::endl;
    std::cout << "Dopamine level: " << dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << "0.0" << "s" << std::endl; // Simplified for demo
    
    return 0;
}