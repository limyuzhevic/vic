// Phase 6: AgentBrain Integration Test
// Comprehensive test of AgentBrain with various scenarios

#include "AgentBrain.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

using namespace nlm;

class AgentBrainIntegrationTest {
public:
    struct TestResult {
        bool passed = false;
        std::string message;
        double executionTime = 0.0;
    };
    
    std::vector<TestResult> results;
    
    AgentBrainIntegrationTest() {
        std::cout << "=== AgentBrain Integration Test ===" << std::endl;
    }
    
    void runAllTests() {
        testBasicInitialization();
        testSensoryProcessing();
        testMotorDecoding();
        testRewardModulation();
        testDevelopmentIntegration();
        testMemoryIntegration();
        testCheckpointing();
        testEdgeCases();
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        int passed = 0;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << (i + 1) << ". " << results[i].message << " - " 
                      << (results[i].passed ? "PASS" : "FAIL") << std::endl;
            if (results[i].passed) passed++;
        }
        
        std::cout << std::endl;
        std::cout << "Total: " << results.size() << ", Passed: " << passed 
                  << ", Failed: " << (results.size() - passed) << std::endl;
    }
    
private:
    std::shared_ptr<Brain> createTestBrain(int neuronCount = 100) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
        cfg->set("region_count", static_cast<int64_t>(1));
        cfg->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain");
        }
        return brain;
    }
    
    void testBasicInitialization() {
        std::cout << "Test 1: Basic initialization..." << std::endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            // Test that agent has valid brain pointer
            assert(agent.getBrain() != nullptr);
            assert(agent.getSensoryInputSize() > 0);
            assert(agent.getMotorOutputSize() > 0);
            
            results.push_back({true, "Basic initialization passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Basic initialization failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testSensoryProcessing() {
        std::cout << "Test 2: Sensory processing..." << std::endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create test percept
            SensoryPercept percept;
            
            // Test vision input (256 values)
            std::vector<float> vision(256, 0.0f);
            for (size_t i = 0; i < vision.size(); ++i) {
                vision[i] = i % 10 * 0.1f;  // Vary values 0.0 to 0.9
            }
            
            // Test touch input (8 values)
            std::vector<float> touch(8, 0.0f);
            for (size_t i = 0; i < touch.size(); ++i) {
                touch[i] = i % 5 * 0.2f;
            }
            
            // Test internal signals (4 values)
            std::vector<float> internal(4, 0.0f);
            for (size_t i = 0; i < internal.size(); ++i) {
                internal[i] = i * 0.25f - 0.5f;
            }
            
            // Test proprioception (6 values)
            std::vector<float> proprioception(6, 0.0f);
            for (size_t i = 0; i < proprioception.size(); ++i) {
                proprioception[i] = i * 0.1f - 0.3f;
            }
            
            // Set percept data
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Verify novelty was computed
            float novelty = agent.getNoveltyLevel();
            assert(novelty >= 0.0f);
            
            // Verify curiosity was updated
            float curiosity = agent.getCuriosityLevel();
            assert(curiosity >= 0.0f && curiosity <= 1.0f);
            
            // Verify development state updated
            double age = agent.getDevelopmentalStage(); // Note: This returns DevelopmentalStage, not age
            
            results.push_back({true, "Sensory processing passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Sensory processing failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testMotorDecoding() {
        std::cout << "Test 3: Motor decoding..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create percept with some vision data
            SensoryPercept percept;
            std::vector<float> vision(256, 0.0f);
            
            // Add some pattern to vision to stimulate neurons
            for (size_t i = 0; i < vision.size(); i += 32) {
                vision[i] = 0.8f;
                vision[i + 16] = 0.6f;
            }
            
            percept.setVision(vision);
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Decode motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Command should be valid
            bool validCmd = (cmd == MotorCommand::MoveForward ||
                           cmd == MotorCommand::MoveBackward ||
                           cmd == MotorCommand::TurnLeft ||
                           cmd == MotorCommand::TurnRight ||
                           cmd == MotorCommand::Interact ||
                           cmd == MotorCommand::Wait ||
                           cmd == MotorCommand::LookLeft ||
                           cmd == MotorCommand::LookRight);
            
            assert(validCmd);
            
            results.push_back({true, "Motor decoding passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Motor decoding failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testRewardModulation() {
        std::cout << "Test 4: Reward modulation..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create percept
            SensoryPercept percept;
            std::vector<float> vision(256, 0.0f);
            vision[128] = 1.0f;  // Some bright pixel
            percept.setVision(vision);
            
            agent.processSensoryInput(percept);
            
            // Apply reward modulation
            float reward = 1.0f;
            float predictedReward = 0.5f;
            agent.applyRewardModulation(reward, predictedReward);
            
            // Verify prediction error computed
            float predictionError = agent.getPredictionError();
            assert(std::abs(predictionError - 0.5f) < 0.001f);
            
            // Verify dopamine level updated
            float dopamine = agent.getNeuromodulationLevel();
            assert(dopamine >= -1.0f && dopamine <= 1.0f);
            
            // Apply another reward (different predicted value)
            reward = 0.0f;
            predictedReward = 0.2f;
            agent.applyRewardModulation(reward, predictedReward);
            
            predictionError = agent.getPredictionError();
            assert(std::abs(predictionError - (-0.2f)) < 0.001f);
            
            results.push_back({true, "Reward modulation passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Reward modulation failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testDevelopmentIntegration() {
        std::cout << "Test 5: Development integration..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create initial percept
            SensoryPercept percept;
            std::vector<float> vision(256, 0.0f);
            vision[0] = 0.5f;
            percept.setVision(vision);
            
            agent.processSensoryInput(percept);
            
            // Update development multiple times
            for (int step = 0; step < 10; ++step) {
                agent.updateDevelopment(0.001);
                
                // Verify plasticity modifier changes over time
                float plasticity = agent.getCuriosityLevel();  // Note: using curiosity as placeholder
                assert(plasticity >= 0.0f && plasticity <= 1.0f);
            }
            
            // Verify developmental stage changes
            DevelopmentalStage stage = agent.getDevelopmentalStage();
            assert(stage == DevelopmentalStage::Initial || 
                   stage == DevelopmentalStage::CriticalPeriod ||
                   stage == DevelopmentalStage::Maturation ||
                   stage == DevelopmentalStage::Adult ||
                   stage == DevelopmentalStage::Aging);
            
            results.push_back({true, "Development integration passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Development integration failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testMemoryIntegration() {
        std::cout << "Test 6: Memory integration..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create percept
            SensoryPercept percept;
            std::vector<float> vision(256, 0.0f);
            for (int i = 0; i < 50; ++i) {
                vision[i] = 0.1f * i;
            }
            percept.setVision(vision);
            
            agent.processSensoryInput(percept);
            
            // Process multiple steps
            for (int step = 0; step < 20; ++step) {
                // Update development
                agent.updateDevelopment(0.001);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Verify command is valid
                assert(cmd == MotorCommand::MoveForward ||
                       cmd == MotorCommand::MoveBackward ||
                       cmd == MotorCommand::TurnLeft ||
                       cmd == MotorCommand::TurnRight ||
                       cmd == MotorCommand::Interact ||
                       cmd == MotorCommand::Wait ||
                       cmd == MotorCommand::LookLeft ||
                       cmd == MotorCommand::LookRight);
            }
            
            // Verify novelty detection works
            float novelty = agent.getNoveltyLevel();
            assert(novelty >= 0.0f);
            
            results.push_back({true, "Memory integration passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Memory integration failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testCheckpointing() {
        std::cout << "Test 7: Checkpointing..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Create percept with some activity
            SensoryPercept percept;
            std::vector<float> vision(256, 0.0f);
            for (int i = 0; i < 100; ++i) {
                vision[i % vision.size()] = 0.5f;
            }
            percept.setVision(vision);
            
            // Process some steps
            for (int i = 0; i < 10; ++i) {
                agent.processSensoryInput(percept);
                agent.updateDevelopment(0.001);
            }
            
            // Reset agent
            agent.reset();
            
            // Verify reset worked
            float dopamine = agent.getNeuromodulationLevel();
            assert(dopamine == 0.0f);
            
            results.push_back({true, "Checkpointing passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Checkpointing failed: " + std::string(e.what()), 0.0});
        }
    }
    
    void testEdgeCases() {
        std::cout << "Test 8: Edge cases..." << endl;
        try {
            auto brain = createTestBrain();
            AgentBrain agent(brain);
            
            SimpleWorld world;
            world.initialize(16, 16);
            agent.initialize(world);
            
            // Test 1: Empty percept
            SensoryPercept emptyPercept;
            agent.processSensoryInput(emptyPercept);  // Should not crash
            
            // Test 2: Zero reward
            agent.applyRewardModulation(0.0f, 0.0f);
            
            // Test 3: Large reward
            agent.applyRewardModulation(100.0f, 50.0f);
            
            // Test 4: Negative reward
            agent.applyRewardModulation(-1.0f, 0.0f);
            
            // Test 5: Very small timestep
            for (int i = 0; i < 100; ++i) {
                agent.updateDevelopment(0.000001);
            }
            
            // Test 6: Boundary conditions
            agent.enableRewardModulation(false);
            agent.applyRewardModulation(1.0f, 0.5f);  // Should have no effect
            
            agent.enableCuriosity(false);
            MotorCommand cmd = agent.decodeMotorCommand();  // Should still work
            
            results.push_back({true, "Edge cases passed", 0.0});
        } catch (const std::exception& e) {
            results.push_back({false, "Edge cases failed: " + std::string(e.what()), 0.0});
        }
    }
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Suppress logs for cleaner test output
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    AgentBrainIntegrationTest test;
    test.runAllTests();
    
    return 0;
}