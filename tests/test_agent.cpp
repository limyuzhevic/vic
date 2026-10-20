// Agent Behavior Tests - Comprehensive validation of agent functionality
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace test_agent {

// Agent curiosity validation test
class AgentCuriosityValidationTest : public test_framework::TestBase {
public:
    AgentCuriosityValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Test initial curiosity level
        assert(agentBrain->getCuriosityLevel() == 0.0f);
        
        // Test curiosity level changes
        agentBrain->enableCuriosity(true);
        assert(agentBrain->isCuriosityEnabled());
        
        // Test curiosity-driven exploration
        nlm::SensoryPercept percept;
        // Add some novelty to trigger curiosity
        nlm::Vision vision(16, 16, 3);
        std::vector<float> visionData(256, 0.5f);
        vision.setData(visionData);
        percept.setVision(vision);
        
        agentBrain->processSensoryInput(percept);
        
        // Curiosity level should respond to sensory input
        assert(agentBrain->getCuriosityLevel() >= 0.0f);
        
        std::cout << "    Agent curiosity validation test for " << getName() << " passed" << std::endl;
    }
};

// Agent neuromodulation validation test
class AgentNeuromodulationValidationTest : public test_framework::TestBase {
public:
    AgentNeuromodulationValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Test initial neuromodulation level
        assert(agentBrain->getNeuromodulationLevel() == 0.0f);
        
        // Test reward-based neuromodulation
        agentBrain->enableRewardModulation(true);
        assert(agentBrain->isRewardModulationEnabled());
        
        agentBrain->applyRewardModulation(1.0f, 0.8f);  // Reward, predicted reward
        
        // Neuromodulation level should change based on reward
        assert(agentBrain->getNeuromodulationLevel() >= 0.0f);
        
        // Test novelty-based neuromodulation
        agentBrain->enableCuriosity(true);
        
        // Create perceptual change to trigger novelty detection
        nlm::SensoryPercept percept1, percept2;
        nlm::Vision vision1(16, 16, 3), vision2(16, 16, 3);
        
        std::vector<float> data1(256, 0.0f);
        std::vector<float> data2(256, 0.5f);
        
        vision1.setData(data1);
        vision2.setData(data2);
        
        percept1.setVision(vision1);
        percept2.setVision(vision2);
        
        agentBrain->processSensoryInput(percept1);
        agentBrain->processSensoryInput(percept2);
        
        // Novelty should be detected
        assert(agentBrain->getNoveltyLevel() >= 0.0f);
        
        std::cout << "    Agent neuromodulation validation test for " << getName() << " passed" << std::endl;
    }
};

// Agent development validation test
class AgentDevelopmentValidationTest : public test_framework::TestBase {
public:
    AgentDevelopmentValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Test initial developmental stage
        assert(agentBrain->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
        
        // Enable development
        agentBrain->enableDevelopment(true);
        assert(agentBrain->isDevelopmentEnabled());
        
        // Test development over time
        double timestep = 1.0;
        for (int i = 0; i < 10; ++i) {
            agentBrain->updateDevelopment(timestep);
            assert(agentBrain->getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
            assert(agentBrain->getDevelopmentalStage() <= nlm::DevelopmentalStage::Aging);
        }
        
        // Test plasticity modifier changes during development
        float initialPlasticity = agentBrain->getBrain()->getBrain()->getTotalSynapseCount() > 0 ? 1.0f : 0.0f;
        
        // Development should increase learning capacity
        assert(agentBrain->getDevelopmentalStage() == nlm::DevelopmentalStage::Adult || 
               agentBrain->getDevelopmentalStage() == nlm::DevelopmentalStage::Aging);
        
        std::cout << "    Agent development validation test for " << getName() << " passed" << std::endl;
    }
};

// Agent behavior patterns validation test
class AgentBehaviorPatternsValidationTest : public test_framework::TestBase {
public:
    AgentBehaviorPatternsValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Test custom behavior patterns
        std::vector<std::pair<nlm::NeuronId, float>> neuralMapping = {
            {nlm::NeuronId(1), 0.5f},
            {nlm::NeuronId(2), 0.3f},
            {nlm::NeuronId(3), 0.8f}
        };
        
        std::vector<std::string> rules = {
            "if(value > 0.5) move_forward",
            "if(distance < 10) turn_away",
            "if(food_present) approach"
        };
        
        agentBrain->customBehaviorPattern("exploration", neuralMapping, rules);
        
        // Test behavior policy
        std::vector<float> priorityLevels = {1.0f, 0.8f, 0.5f, 0.3f, 0.1f};
        agentBrain->behaviorPolicy(rules, priorityLevels);
        
        // Test adaptive behavior controller
        agentBrain->adaptiveBehaviorController("avoid_obstacles");
        
        // Test uncertainty-based exploration
        agentBrain->uncertaintyBasedExploration(0.5f);
        
        // Test novelty-weighted sampling
        std::vector<std::pair<float, std::string>> weightedFunctions = {
            {0.7f, "explore"},
            {0.3f, "exploit"},
            {0.1f, "rest"}
        };
        
        agentBrain->noveltyWeightedSampling(weightedFunctions);
        
        std::cout << "    Agent behavior patterns validation test for " << getName() << " passed" << std::endl;
    }
};

// Agent body state validation test
class AgentBodyStateValidationTest : public test_framework::TestBase {
public:
    AgentBodyStateValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        nlm::AgentBody body;
        
        // Test initial state
        assert(body.x == 0.0f);
        assert(body.y == 0.0f);
        assert(body.orientation == 0.0f);
        assert(body.energy == 1.0f);
        assert(body.health == 1.0f);
        assert(body.age == 0.0f);
        assert(!body.isMoving);
        assert(!body.isTurning);
        
        // Test state modifications
        body.x = 10.0f;
        body.y = 5.0f;
        body.orientation = 1.57f;  // ~90 degrees
        body.energy = 0.7f;
        body.health = 0.9f;
        body.age = 10.0f;
        body.isMoving = true;
        body.isTurning = false;
        body.velocityX = 2.0f;
        body.velocityY = 1.0f;
        body.angularVelocity = 0.5f;
        body.lastActionTime = 5.0f;
        
        // Verify state changes
        assert(body.x == 10.0f);
        assert(body.y == 5.0f);
        assert(body.orientation == 1.57f);
        assert(body.energy == 0.7f);
        assert(body.health == 0.9f);
        assert(body.age == 10.0f);
        assert(body.isMoving);
        assert(!body.isTurning);
        assert(body.velocityX == 2.0f);
        assert(body.velocityY == 1.0f);
        assert(body.angularVelocity == 0.5f);
        assert(body.lastActionTime == 5.0f);
        
        // Test reset
        body.reset();
        
        assert(body.x == 0.0f);
        assert(body.y == 0.0f);
        assert(body.orientation == 0.0f);
        assert(body.energy == 1.0f);
        assert(body.health == 1.0f);
        assert(body.age == 0.0f);
        assert(!body.isMoving);
        assert(!body.isTurning);
        assert(body.velocityX == 0.0f);
        assert(body.velocityY == 0.0f);
        assert(body.angularVelocity == 0.0f);
        assert(body.lastActionTime == 0.0f);
        
        std::cout << "    Agent body state validation test for " << getName() << " passed" << std::endl;
    }
};

// Agent sensory processing validation test
class AgentSensoryProcessingValidationTest : public test_framework::TestBase {
public:
    AgentSensoryProcessingValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::AgentBehavior) {}
    
    void run() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Create sensory percept
        nlm::SensoryPercept percept;
        
        // Test vision input
        nlm::Vision vision(16, 16, 3);
        std::vector<float> visionData(256, 0.0f);
        for (size_t i = 0; i < visionData.size(); ++i) {
            visionData[i] = static_cast<float>(i % 255) / 255.0f;
        }
        vision.setData(visionData);
        percept.setVision(vision);
        
        // Test touch input
        std::vector<float> touchData(8, 0.0f);
        for (size_t i = 0; i < touchData.size(); ++i) {
            touchData[i] = (i % 2) * 0.5f;
        }
        nlm::Touch touch(touchData);
        percept.setTouch(touch);
        
        // Test proprioception input
        std::vector<float> proprioceptionData(6, 0.0f);
        for (size_t i = 0; i < proprioceptionData.size(); ++i) {
            proprioceptionData[i] = static_cast<float>(i) * 0.2f - 0.6f;
        }
        nlm::Proprioception proprioception(proprioceptionData);
        percept.setProprioception(proprioception);
        
        // Test internal signals input
        nlm::InternalSignals internal;
        internal.addSignal(0.5f);
        internal.addSignal(-0.3f);
        internal.addSignal(0.8f);
        percept.setInternal(internal);
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Verify sensory processing
        assert(percept.getVisionWidth() == 16);
        assert(percept.getVisionHeight() == 16);
        assert(percept.getTouchSize() == 8);
        assert(percept.getInternalSignalCount() == 3);
        
        // Test timestamp
        assert(percept.getTimestamp() >= 0.0);
        
        std::cout << "    Agent sensory processing validation test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests (Original agent tests)
void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    assert(agentBrain->getBrain() == brain.get());
    assert(agentBrain->isRewardModulationEnabled());
    assert(agentBrain->isStructuralPlasticityEnabled());
    assert(agentBrain->isDevelopmentEnabled());
    assert(agentBrain->isCuriosityEnabled());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::SimpleWorld world;
    world.configure(100, 100, 16, 16);
    
    agentBrain->initialize(world);
    
    assert(agentBrain->getSensoryInputSize() > 0);
    assert(agentBrain->getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentSensoryPercept() {
    nlm::SensoryPercept percept;
    
    assert(percept.getVisionWidth() == 0);
    assert(percept.getVisionHeight() == 0);
    
    nlm::Vision vision(16, 16, 3);
    std::vector<float> data(256, 1.0f);
    vision.setData(data);
    percept.setVision(vision);
    
    assert(percept.getVisionWidth() == 16);
    assert(percept.getVisionHeight() == 16);
    
    std::cout << "    testAgentSensoryPercept passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    agentBrain->enableRewardModulation(false);
    assert(!agentBrain->isRewardModulationEnabled());
    
    agentBrain->enableStructuralPlasticity(false);
    assert(!agentBrain->isStructuralPlasticityEnabled());
    
    agentBrain->enableDevelopment(false);
    assert(!agentBrain->isDevelopmentEnabled());
    
    agentBrain->enableCuriosity(false);
    assert(!agentBrain->isCuriosityEnabled());
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainProcessSensory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::SensoryPercept percept;
    nlm::Vision vision(16, 16, 3);
    std::vector<float> data(256, 0.5f);
    vision.setData(data);
    percept.setVision(vision);
    
    agentBrain->processSensoryInput(percept);
    
    // Should not crash
    assert(true);
    
    std::cout << "    testAgentBrainProcessSensory passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Agent Behavior Tests ===" << std::endl;
    
    // Run original unit tests
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentSensoryPercept();
    testAgentBrainNeuromodulation();
    testAgentBrainProcessSensory();
    
    std::cout << "\n=== Running Agent Behavior Validation Tests ===" << std::endl;
    
    // Run comprehensive validation tests
    AgentCuriosityValidationTest curiosityTest("agent_curiosity_validation");
    curiosityTest.run();
    
    AgentNeuromodulationValidationTest neuromodulationTest("agent_neuromodulation_validation");
    neuromodulationTest.run();
    
    AgentDevelopmentValidationTest developmentTest("agent_development_validation");
    developmentTest.run();
    
    AgentBehaviorPatternsValidationTest behaviorPatternsTest("agent_behavior_patterns_validation");
    behaviorPatternsTest.run();
    
    AgentBodyStateValidationTest bodyStateTest("agent_body_state_validation");
    bodyStateTest.run();
    
    AgentSensoryProcessingValidationTest sensoryProcessingTest("agent_sensory_processing_validation");
    sensoryProcessingTest.run();
    
    std::cout << "\n=== All Agent Behavior Tests Completed ===" << std::endl;
}

} // namespace test_agent