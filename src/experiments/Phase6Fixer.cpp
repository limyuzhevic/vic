// Phase 6 Integration Issues Fixer
//
// This demonstrates the issues with Phase 6 integration and provides fixes

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace nlm;

// Simple demonstration of Phase 6 integration issues
void demonstratePhase6Issues() {
    std::cout << "=== Demonstrating Phase 6 Integration Issues ===" << std::endl;
    std::cout << std::endl;
    
    // Issue 1: Reward calculation showing 0
    std::cout << "1. Reward Calculation Issue:" << std::endl;
    std::cout << "   Current Phase 6 demo reports 0 reward" << std::endl;
    std::cout << "   This suggests integration issues with:" << std::endl;
    std::cout << "   - Reward modulation system" << std::endl;
    std::cout << "   - Action result calculation" << std::endl;
    std::cout << "   - World feedback processing" << std::endl;
    std::cout << "" << std::endl;
    
    // Issue 2: Checkpointing is stubbed
    std::cout << "2. Checkpointing Issue:" << std::endl;
    std::cout << "   Current Brain::save() returns false (stub implementation)" << std::endl;
    std::cout << "   This prevents state persistence and recovery" << std::endl;
    std::cout << "" << std::endl;
    
    // Issue 3: High activity but poor learning
    std::cout << "3. Performance Issue:" << std::endl;
    std::cout << "   Phase 6 shows ~200 spikes/step (very high)" << std::endl;
    std::cout << "   But learning is mostly saturation, not selective pattern learning" << std::endl;
    std::cout << "" << std::endl;
    
    // Issue 4: Poor configurability
    std::cout << "4. Configurability Issue:" << std::endl;
    std::cout << "   Limited control over simulation parameters" << std::endl;
    std::cout << "   Hard to tune for different behaviors" << std::endl;
    std::cout << "" << std::endl;
}

// Demonstration of how reward calculation should work
void demonstrateProperRewardCalculation() {
    std::cout << "=== Proper Reward Calculation Example ===" << std::endl;
    std::cout << std::endl;
    
    // Create a simple brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create world and agent
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    std::cout << "Running simulation with reward modulation..." << std::endl;
    
    float totalReward = 0.0f;
    uint64_t steps = 100;
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Get sensory input
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001);
        
        // This is where reward should be calculated and applied
        float reward = actionResult.reward;
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": Reward = " << reward 
                      << " (Total: " << totalReward << ")" << std::endl;
        }
    }
    
    float avgReward = totalReward / steps;
    std::cout << "" << std::endl;
    std::cout << "Average reward per step: " << avgReward << std::endl;
    std::cout << "Total reward over " << steps << " steps: " << totalReward << std::endl;
    
    if (avgReward > 0.0f) {
        std::cout << "✓ Reward calculation working properly" << std::endl;
    } else {
        std::cout << "✗ Reward calculation issue detected" << std::endl;
    }
}

// Demonstration of proper checkpointing
void demonstrateProperCheckpointing() {
    std::cout << "=== Proper Checkpointing Example ===" << std::endl;
    std::cout << std::endl;
    
    // Create brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    auto brain1 = std::make_shared<Brain>(config);
    if (!brain1->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain created with " << brain1->getTotalNeuronCount() 
              << " neurons and " << brain1->getTotalSynapseCount() 
              << " synapses" << std::endl;
    
    // Run some steps
    std::cout << "Running simulation..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    std::cout << "Saving checkpoint to " << checkpointPath << "..." << std::endl;
    
    bool saveSuccess = brain1->save(checkpointPath);
    if (saveSuccess) {
        std::cout << "✓ Checkpoint saved successfully" << std::endl;
        
        // Create new brain and load
        std::cout << "Creating new brain and loading checkpoint..." << std::endl;
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        bool loadSuccess = brain2->load(checkpointPath);
        if (loadSuccess) {
            std::cout << "✓ Checkpoint loaded successfully" << std::endl;
            
            // Verify state was restored
            std::cout << "Verifying state restoration..." << std::endl;
            std::cout << "Brain2 has " << brain2->getTotalNeuronCount() 
                      << " neurons and " << brain2->getTotalSynapseCount() 
                      << " synapses" << std::endl;
            
            // Check spike count (should be the same after restore)
            size_t spikes1 = brain1->getTotalSpikeCount();
            size_t spikes2 = brain2->getTotalSpikeCount();
            
            std::cout << "Brain1 total spikes: " << spikes1 << std::endl;
            std::cout << "Brain2 total spikes after load: " << spikes2 << std::endl;
            
            if (spikes2 > 0) {
                std::cout << "✓ Checkpointing working properly" << std::endl;
            } else {
                std::cout << "⚠ Checkpointing may have issues - no spikes after load" << std::endl;
            }
        } else {
            std::cout << "✗ Failed to load checkpoint" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to save checkpoint" << std::endl;
    }
}

// Summary of fixes needed
void printSummaryOfFixes() {
    std::cout << "=== Summary of Phase 6 Integration Fixes ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Issues to fix:" << std::endl;
    std::cout << "1. ✓ Refactor main.cpp for better separation of concerns" << std::endl;
    std::cout << "2. ✓ Implement configurable Phase 6 demo" << std::endl;
    std::cout << "3. ✗ Fix reward calculation in Phase 6 integration" << std::endl;
    std::cout << "4. ✗ Fix checkpointing implementation" << std::endl;
    std::cout << "5. ✗ Reduce activity levels for better performance" << std::endl;
    std::cout << "6. ✗ Improve configurability and parameter tuning" << std::endl;
    std::cout << "7. ✗ Add proper integration testing" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Key fixes needed:" << std::endl;
    std::cout << "- Implement proper reward calculation in Brain::produceAction()" << std::endl;
    std::cout << "- Fix CheckpointReader/CheckpointWriter implementations" << std::endl;
    std::cout << "- Add selective learning mechanisms instead of saturation" << std::endl;
    std::cout << "- Improve parameter tuning and configurability" << std::endl;
    std::cout << "- Add comprehensive integration tests" << std::endl;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM Phase 6 Integration Issues Demonstrator" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    
    // Demonstrate the issues
    demonstratePhase6Issues();
    
    // Demonstrate proper reward calculation
    std::cout << "=== Demonstrating Proper Reward Calculation ===" << std::endl;
    demonstrateProperRewardCalculation();
    std::cout << std::endl;
    
    // Demonstrate proper checkpointing
    std::cout << "=== Demonstrating Proper Checkpointing ===" << std::endl;
    demonstrateProperCheckpointing();
    std::cout << std::endl;
    
    // Print summary
    printSummaryOfFixes();
    
    return 0;
}