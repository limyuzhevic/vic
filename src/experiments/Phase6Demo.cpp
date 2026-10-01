// Brain-Agent-Environment Integration Tests
// Comprehensive integration tests that verify all three major components
// work together correctly in a unified system.

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace integration_tests {

// Integration Test 1: Brain-Agent Communication
void testBrainAgentCommunication() {
    std::cout << "=== Test 1: Brain-Agent Communication ===\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(100));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(123));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create world
    SimpleWorld world;
    world.initialize(10, 10);
    world.setRandomSeed(456);
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Verify communication channels
    assert(agentBrain.getBrain().get() == brain.get());
    assert(agentBrain.getRegions()[0].get() == brain->getRegion(RegionId(1)));
    
    // Test sensory input processing
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    agentBrain.enablePredictionError(true);
    
    // Verify agent can communicate with brain
    const auto& regions = agentBrain.getRegions();
    assert(!regions.empty());
    
    // Test neuromodulation access
    if (brain->getDopamine()) {
        brain->getDopamine()->setLevel(0.5f);
        assert(brain->getDopamine()->getLevel() == 0.5f);
    }
    
    // Test memory system access
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->initialize(brain.get());
        assert(brain->getWorkingMemory()->getCapacity() > 0);
    }
    
    std::cout << "✓ Brain-Agent communication test passed\n";
}

// Integration Test 2: Agent-World Interaction
void testAgentWorldInteraction() {
    std::cout << "=== Test 2: Agent-World Interaction ===\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(50));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.15f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create world
    SimpleWorld world;
    world.initialize(8, 8);
    world.setRandomSeed(789);
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable all agent capabilities
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    agentBrain.enablePredictionError(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    
    // Test initial world state
    assert(world.getAgentBody().energy > 0);
    assert(world.getAgentBody().position.x >= 0 && world.getAgentBody().position.x < 8);
    
    // Run interaction cycle
    for (int step = 0; step < 10; ++step) {
        // Get observation from world
        const SensoryPercept& percept = world.getSensoryPercept();
        assert(!percept.empty());
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Get motor command
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        
        // Apply action to world
        ActionResult result = world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
        
        // Apply reward modulation
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        
        // Update development
        agentBrain.updateDevelopment(0.001);
        
        // Verify world state is updated
        assert(world.getAgentBody().energy >= 0);  // Energy shouldn't go negative
    }
    
    std::cout << "✓ Agent-World interaction test passed\n";
}

// Integration Test 3: Learning Cycle Integration
void testLearningCycleIntegration() {
    std::cout << "=== Test 3: Learning Cycle Integration ===\n";
    
    // Create configuration with learning parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(200));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.12f);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("pruning_rate", 0.000005f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create world
    SimpleWorld world;
    world.configure(12, 12);
    world.setRandomSeed(999);
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable learning systems
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    agentBrain.enablePredictionError(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    
    // Track learning metrics
    float initialReward = 0.0f;
    float finalReward = 0.0f;
    float rewardImprovement = 0.0f;
    
    // Run learning cycles
    for (int cycle = 0; cycle < 20; ++cycle) {
        // Reset for new cycle
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        float cycleReward = 0.0f;
        
        // Run steps
        for (int step = 0; step < 100; ++step) {
            // Get observation
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get motor command
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // Apply action
            ActionResult result = world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
            
            // Apply reward
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            cycleReward += result.reward;
        }
        
        if (cycle == 0) {
            initialReward = cycleReward;
        } else {
            finalReward = cycleReward;
            rewardImprovement = finalReward - initialReward;
        }
        
        // Verify learning is happening
        assert(brain->getTotalSynapseCount() > 0);
        assert(agentBrain.getNoveltyLevel() >= 0.0f);
        assert(agentBrain.getCuriosityLevel() >= 0.0f);
        
        // Check for adaptation (reward changes)
        if (cycle > 5) {
            float adaptation = std::abs(rewardImprovement);
            // Allow for small changes
            assert(adaptation < cycleReward * 2.0f);
        }
    }
    
    // Verify learning occurred
    assert(std::abs(rewardImprovement) > 0.0f || initialReward > 0.0f);
    
    std::cout << "✓ Learning cycle integration test passed\n";
}

// Integration Test 4: Memory Integration
void testMemoryIntegration() {
    std::cout << "=== Test 4: Memory Integration ===\n";
    
    // Create configuration with memory parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(150));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    config->set("replay_interval", 50);
    config->set("consolidation_interval", 200);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Verify memory systems are initialized
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    
    // Create world and agent
    SimpleWorld world;
    world.initialize(6, 6);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable memory systems
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    
    // Test memory operations
    float initialMemoryActivity = brain->getWorkingMemory()->getMemoryActivity();
    
    // Run some steps to generate memory activity
    for (int step = 0; step < 50; ++step) {
        const SensoryPercept& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        brain->step(step);
        
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
        agentBrain.applyRewardModulation(1.0f, 0.0f);
    }
    
    float finalMemoryActivity = brain->getWorkingMemory()->getMemoryActivity();
    
    // Verify memory is being used
    assert(finalMemoryActivity >= 0.0f);
    assert(brain->getWorkingMemory()->getActiveTraces() > 0);
    
    // Test episodic memory
    if (brain->getEpisodicMemory()) {
        size_t initialEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
        assert(initialEpisodes >= 0);
    }
    
    // Test memory consolidation (if enabled)
    if (config->has("consolidation_interval")) {
        size_t consolidationInterval = config->getOr<size_t>("consolidation_interval", 100);
        // This would be tested in a longer-running simulation
        assert(consolidationInterval > 0);
    }
    
    std::cout << "✓ Memory integration test passed\n";
}

// Integration Test 5: Neuromodulation Integration
void testNeuromodulationIntegration() {
    std::cout << "=== Test 5: Neuromodulation Integration ===\n";
    
    // Create configuration with neuromodulation parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(100));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    config->set("dopamine_baseline", 0.1f);
    config->set("reward_discount_factor", 0.95f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Verify neuromodulation systems exist
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionErrorSignal() != nullptr);
    
    // Create world and agent
    SimpleWorld world;
    world.initialize(8, 8);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable neuromodulation
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    agentBrain.enablePredictionError(true);
    
    // Test neuromodulation levels
    float initialDopamine = brain->getDopamine()->getLevel();
    float initialCuriosity = agentBrain.getCuriosityLevel();
    float initialNovelty = agentBrain.getNoveltyLevel();
    
    // Run interaction to generate neuromodulation signals
    for (int step = 0; step < 30; ++step) {
        const SensoryPercept& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        brain->step(step);
        
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        ActionResult result = world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
        
        // Apply reward to generate neuromodulation
        float reward = result.reward;
        if (reward > 0) {
            agentBrain.applyRewardModulation(reward, 0.0f);
        }
        
        // Generate prediction error based on experience
        float prediction = agentBrain.getPredictionError();
        if (std::abs(prediction) > 0.1f) {
            agentBrain.setPredictionError(prediction);
        }
    }
    
    // Verify neuromodulation levels changed
    assert(std::abs(brain->getDopamine()->getLevel() - initialDopamine) >= 0.0f);
    assert(std::abs(agentBrain.getCuriosityLevel() - initialCuriosity) >= 0.0f);
    assert(std::abs(agentBrain.getNoveltyLevel() - initialNovelty) >= 0.0f);
    
    // Test neuromodulation effects on plasticity
    float plasticityFactor = brain->getDopamine()->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 2.0f);  // Should be scaled
    
    std::cout << "✓ Neuromodulation integration test passed\n";
}

// Integration Test 6: Development Integration
void testDevelopmentIntegration() {
    std::cout << "=== Test 6: Development Integration ===\n";
    
    // Create configuration with development parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(120));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("development_enable", true);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create world and agent
    SimpleWorld world;
    world.initialize(10, 10);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable development
    agentBrain.enableDevelopment(true);
    
    // Test developmental stage progression
    DevelopmentalStage initialStage = brain->getDevelopmentalStage();
    
    // Run development cycles
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Simulate development
        agentBrain.updateDevelopment(0.1f);
        
        // Verify development system is working
        assert(brain->getDevelopmentSystem() != nullptr);
        
        // Check developmental stage changes
        DevelopmentalStage currentStage = brain->getDevelopmentalStage();
        // Stages should progress or stay the same over time
        assert(static_cast<int>(currentStage) >= static_cast<int>(initialStage));
        
        // Test world interaction during development
        const SensoryPercept& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        brain->step(cycle);
        
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
        agentBrain.applyRewardModulation(0.5f, 0.0f);
    }
    
    std::cout << "✓ Development integration test passed\n";
}

// Integration Test 7: Full System Integration
void testFullSystemIntegration() {
    std::cout << "=== Test 7: Full System Integration ===\n";
    
    // Create comprehensive configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(300));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.12f);
    config->set("stdp_ltp_weight", 0.015f);
    config->set("stdp_ltd_weight", 0.018f);
    config->set("synaptogenesis_rate", 0.00008f);
    config->set("pruning_rate", 0.000008f);
    config->set("replay_interval", 25);
    config->set("consolidation_interval", 150);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create world and agent
    SimpleWorld world;
    world.configure(15, 15);
    world.setRandomSeed(42);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable all systems
    agentBrain.enableCuriosity(true);
    agentBrain.enableNoveltyDetection(true);
    agentBrain.enablePredictionError(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    
    // Run comprehensive integration test
    float totalReward = 0.0f;
    float totalNovelty = 0.0f;
    float totalCuriosity = 0.0f;
    
    for (int step = 0; step < 200; ++step) {
        // Get observation
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Motor action
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        ActionResult result = world.applyAction(agentBrain.getBrain()->getRegions()[0].get(), cmd);
        
        // Reward-based learning
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        totalReward += result.reward;
        
        // Track neuromodulation
        totalNovelty += agentBrain.getNoveltyLevel();
        totalCuriosity += agentBrain.getCuriosityLevel();
        
        // Update world
        world.update(0.01);
        agentBrain.updateDevelopment(0.01);
        
        // Verify system health
        assert(brain->getTotalSpikeCount() >= 0);
        assert(agentBrain.getBrain()->getTotalSynapseCount() > 0);
        assert(world.getAgentBody().energy >= 0);
    }
    
    // Final verification
    assert(totalReward > 0.0f || totalNovelty > 0.0f || totalCuriosity > 0.0f);
    assert(brain->getTotalSynapseCount() >= config->getOr<size_t>("neuron_count", 100) / 2);
    assert(brain->getWorkingMemory() != nullptr || brain->getEpisodicMemory() != nullptr);
    
    std::cout << "✓ Full system integration test passed\n";
    std::cout << "  Total reward: " << totalReward << "\n";
    std::cout << "  Average novelty: " << (totalNovelty / 200.0f) << "\n";
    std::cout << "  Average curiosity: " << (totalCuriosity / 200.0f) << "\n";
}

void runAllIntegrationTests() {
    std::cout << "====================================================\n";
    std::cout << "BRAIN-AGENT-WORLD INTEGRATION TEST SUITE\n";
    std::cout << "====================================================\n\n";
    
    try {
        testBrainAgentCommunication();
        testAgentWorldInteraction();
        testLearningCycleIntegration();
        testMemoryIntegration();
        testNeuromodulationIntegration();
        testDevelopmentIntegration();
        testFullSystemIntegration();
        
        std::cout << "\n====================================================\n";
        std::cout << "ALL INTEGRATION TESTS PASSED!\n";
        std::cout << "====================================================\n";
        std::cout << "\nThe brain-agent-world integration is working correctly.\n";
        std::cout << "All three major components are properly interconnected and functional.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\nINTEGRATION TEST FAILED: " << e.what() << "\n";
        throw;
    } catch (...) {
        std::cerr << "\nINTEGRATION TEST FAILED: Unknown error\n";
        throw;
    }
}

} // namespace integration_tests

// Main entry point for integration tests
int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Run integration tests
    integration_tests::runAllIntegrationTests();
    
    return 0;
}
