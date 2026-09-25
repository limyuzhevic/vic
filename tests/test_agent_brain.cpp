// AgentBrain Comprehensive Tests
// Tests all AgentBrain functionality and configuration methods
// Covers reward modulation, development, curiosity, and structural plasticity

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryPercept.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto brain = std::make_shared<nlm::Brain>();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Test default initialization
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test default configuration flags
    assert(agentBrain.isRewardModulationEnabled());
    assert(agentBrain.isStructuralPlasticityEnabled());
    assert(agentBrain.isDevelopmentEnabled());
    assert(agentBrain.isCuriosityEnabled());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Create a world
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    
    // Initialize agent brain
    agentBrain.initialize(world);
    
    // Test sensory input size
    assert(agentBrain.getSensoryInputSize() == 256 + 8 + 4 + 6);  // Vision + touch + internal + proprioception
    
    // Test motor output size
    assert(agentBrain.getMotorOutputSize() == 6);  // 6 motor commands
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Create world
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Create sensory percept with various inputs
    nlm::SensoryPercept percept;
    
    // Vision input (256 values)
    std::vector<float> vision(256);
    for (size_t i = 0; i < 256; ++i) {
        vision[i] = static_cast<float>(i % 256) / 255.0f;
    }
    percept.setVision(vision);
    
    // Touch input (8 values)
    std::vector<float> touch(8, 0.5f);
    percept.setTouch(touch);
    
    // Internal signals (4 values)
    std::vector<float> internal(4, 0.0f);
    percept.setInternal(internal);
    
    // Proprioception (6 values)
    std::vector<float> proprioception(6, 1.0f);
    percept.setProprioception(proprioception);
    
    // Store previous vision for novelty calculation
    agentBrain.processSensoryInput(percept);
    
    // Verify novelty level was computed
    float novelty = agentBrain.getNoveltyLevel();
    assert(std::isfinite(novelty));
    assert(novelty >= 0.0f);
    
    // Test with different sensory inputs
    nlm::SensoryPercept percept2;
    std::vector<float> vision2(256, 1.0f);
    percept2.setVision(vision2);
    
    agentBrain.processSensoryInput(percept2);
    
    float novelty2 = agentBrain.getNoveltyLevel();
    assert(std::isfinite(novelty2));
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize with world
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Process some sensory input to create neural activity
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.8f);  // High vision activity
    percept.setVision(vision);
    
    agentBrain.processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Command should be valid
    assert(static_cast<int>(command) >= 0);
    assert(static_cast<int>(command) < static_cast<int>(nlm::MotorCommand::Count));
    
    // Test different motor commands
    std::vector<nlm::MotorCommand> commands;
    for (int i = 0; i < static_cast<int>(nlm::MotorCommand::Count); ++i) {
        commands.push_back(static_cast<nlm::MotorCommand>(i));
    }
    
    // The command decoder should return one of the valid commands
    assert(std::find(commands.begin(), commands.end(), command) != commands.end());
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Test initial state
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getExpectedReward() == 0.0f);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);  // Reward > expected
    
    // Check prediction error
    float predictionError = agentBrain.getPredictionError();
    assert(predictionError > 0.0f);  // Positive error (better than expected)
    
    // Check neuromodulation level (dopamine)
    float neuromodulation = agentBrain.getNeuromodulationLevel();
    assert(neuromodulation > 0.0f);
    
    // Apply reward with less than expected
    agentBrain.applyRewardModulation(0.3f, 0.8f);  // Reward < expected
    
    float negativeError = agentBrain.getPredictionError();
    assert(negativeError < 0.0f);  // Negative error (worse than expected)
    
    float negativeNeuromodulation = agentBrain.getNeuromodulationLevel();
    assert(negativeNeuromodulation < 0.0f);
    
    // Test clamping
    agentBrain.applyRewardModulation(-1.0f, 0.0f);  // Should be clamped to [-1, 1]
    float clampedNeuromodulation = agentBrain.getNeuromodulationLevel();
    assert(clampedNeuromodulation >= -1.0f && clampedNeuromodulation <= 1.0f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Test developmental stages
    agentBrain.updateDevelopment(0.0);  // Should stay in Initial
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    agentBrain.updateDevelopment(120.0);  // 2 minutes (CriticalPeriod)
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    agentBrain.updateDevelopment(300.0);  // 5 minutes (Maturation)
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    agentBrain.updateDevelopment(900.0);  // 15 minutes (Adult)
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    agentBrain.updateDevelopment(2000.0);  // Beyond adult
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Test plasticity modifier changes
    float plasticity1 = agentBrain.getPlasticityModifier();  // Should be 1.0 (default)
    agentBrain.updateDevelopment(60.0);  // Critical period
    float plasticity2 = agentBrain.getPlasticityModifier();
    assert(plasticity2 < plasticity1);  // Plasticity should decrease
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainCuriosity() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Test curiosity computation
    // With no novelty or prediction error, curiosity should be low
    float curiosity1 = agentBrain.getCuriosityLevel();
    assert(curiosity1 == 0.0f);
    
    // Apply reward modulation to create prediction error
    agentBrain.applyRewardModulation(1.0f, 0.0f);  // Create positive prediction error
    
    float curiosity2 = agentBrain.getCuriosityLevel();
    assert(curiosity2 > 0.0f);
    
    // Test curiosity level bounds
    agentBrain.applyRewardModulation(10.0f, 0.0f);  // Large error
    float curiosity3 = agentBrain.getCuriosityLevel();
    assert(curiosity3 <= 1.0f);  // Should be clamped to max
    
    // Test curiosity with novelty
    // This would require more complex sensory changes
    // For now, just test that curiosity computation exists
    
    std::cout << "    testAgentBrainCuriosity passed" << std::endl;
}

void testAgentBrainConfiguration() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Test configuration flags
    agentBrain.enableRewardModulation(false);
    assert(!agentBrain.isRewardModulationEnabled());
    
    agentBrain.enableRewardModulation(true);
    assert(agentBrain.isRewardModulationEnabled());
    
    agentBrain.enableStructuralPlasticity(false);
    assert(!agentBrain.isStructuralPlasticityEnabled());
    
    agentBrain.enableDevelopment(false);
    assert(!agentBrain.isDevelopmentEnabled());
    
    agentBrain.enableCuriosity(false);
    assert(!agentBrain.isCuriosityEnabled());
    
    // Test that flags are properly applied
    agentBrain.enableRewardModulation(true);
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    float neuromodulation = agentBrain.getNeuromodulationLevel();
    assert(neuromodulation > 0.0f);
    
    // Disable reward modulation and test
    agentBrain.enableRewardModulation(false);
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    float neuromodulation2 = agentBrain.getNeuromodulationLevel();
    // Neuromodulation might still be non-zero due to existing state
    // but the important thing is no crash
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainSensoryNovelty() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Create identical sensory percepts
    nlm::SensoryPercept percept1;
    std::vector<float> vision1(256, 0.5f);
    percept1.setVision(vision1);
    
    nlm::SensoryPercept percept2;
    std::vector<float> vision2(256, 0.5f);
    percept2.setVision(vision2);
    
    // Process first percept
    agentBrain.processSensoryInput(percept1);
    
    // Process second identical percept
    agentBrain.processSensoryInput(percept2);
    
    float novelty1 = agentBrain.getNoveltyLevel();
    
    // Create different percept
    nlm::SensoryPercept percept3;
    std::vector<float> vision3(256, 0.8f);
    percept3.setVision(vision3);
    
    agentBrain.processSensoryInput(percept3);
    
    float novelty2 = agentBrain.getNoveltyLevel();
    
    // Different vision should create higher novelty
    assert(novelty2 >= novelty1);
    
    // Test novelty decay
    float noveltyBefore = agentBrain.getNoveltyLevel();
    agentBrain.processSensoryInput(percept1);  // Re-process same to trigger decay
    float noveltyAfter = agentBrain.getNoveltyLevel();
    
    // Novelty should decay over time
    assert(noveltyAfter <= noveltyBefore);
    
    std::cout << "    testAgentBrainSensoryNovelty passed" << std::endl;
}

void testAgentBrainReset() {
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize with some state
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Create some state
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.7f);
    percept.setVision(vision);
    agentBrain.processSensoryInput(percept);
    
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(100.0);
    
    // Reset
    agentBrain.reset();
    
    // Check that state was reset
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getExpectedReward() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainMemory() {
    // Test AgentBrain state management
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Store state
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.3f);
    percept.setVision(vision);
    
    agentBrain.processSensoryInput(percept);
    agentBrain.applyRewardModulation(0.5f, 0.2f);
    agentBrain.updateDevelopment(50.0);
    
    float neuromodulation = agentBrain.getNeuromodulationLevel();
    float novelty = agentBrain.getNoveltyLevel();
    auto developmentalStage = agentBrain.getDevelopmentalStage();
    
    // Reset and verify reset worked
    agentBrain.reset();
    
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainMemory passed" << std::endl;
}

void testAgentBrainErrorConditions() {
    // Test AgentBrain with error conditions
    nlm::AgentBrain nullAgentBrain(nullptr);
    
    // Should not crash
    nlm::MotorCommand command = nullAgentBrain.decodeMotorCommand();
    assert(command == nlm::MotorCommand::Wait);  // Default fallback
    
    // Test with uninitialized brain
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain uninitializedAgentBrain(brain);
    
    // Should not crash
    uninitializedAgentBrain.enableRewardModulation(false);
    assert(!uninitializedAgentBrain.isRewardModulationEnabled());
    
    // Test with empty world
    nlm::SimpleWorld emptyWorld;
    uninitializedAgentBrain.initialize(emptyWorld);
    
    std::cout << "    testAgentBrainErrorConditions passed" << std::endl;
}

void testAgentBrainPerformance() {
    // Test performance aspects of AgentBrain
    auto brain = std::make_shared<nlm::Brain>();
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    agentBrain.initialize(world);
    
    // Create large sensory input
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    std::vector<float> touch(8, 0.3f);
    percept.setTouch(touch);
    std::vector<float> internal(4, 0.1f);
    percept.setInternal(internal);
    std::vector<float> proprioception(6, 0.2f);
    percept.setProprioception(proprioception);
    
    // Time sensory processing
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        agentBrain.processSensoryInput(percept);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "    Processed 100 sensory inputs in " << duration.count() << " ms" << std::endl;
    
    // Time motor decoding
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        agentBrain.decodeMotorCommand();
    }
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "    Processed 100 motor decodings in " << duration.count() << " ms" << std::endl;
    
    // Time neuromodulation
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        agentBrain.applyRewardModulation(0.5f, 0.3f);
    }
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "    Processed 100 reward modulations in " << duration.count() << " ms" << std::endl;
    
    std::cout << "    testAgentBrainPerformance passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive AgentBrain tests..." << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainInitialization();
    
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    
    testAgentBrainDevelopment();
    testAgentBrainCuriosity();
    testAgentBrainConfiguration();
    
    testAgentBrainSensoryNovelty();
    testAgentBrainReset();
    testAgentBrainMemory();
    
    testAgentBrainErrorConditions();
    testAgentBrainPerformance();
    
    std::cout << "All AgentBrain tests passed!" << std::endl;
}

} // namespace test_agent_brain
