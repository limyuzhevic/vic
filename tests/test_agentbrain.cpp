// AgentBrain Test Suite
// Comprehensive tests for AgentBrain and its modular components

#include "AgentBrain.hpp"
#include "SensoryTransduction.hpp"
#include "MotorDecoder.hpp"
#include "Neuromodulator.hpp"
#include "Developer.hpp"
#include "ExplorationAgent.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <memory>
#include <vector>

namespace test_agentbrain {
    
    // Test fixtures
    class AgentBrainTestFixture {
    public:
        std::shared_ptr<nlm::Config> config_;
        std::shared_ptr<nlm::Brain> brain_;
        std::unique_ptr<nlm::AgentBrain> agentBrain_;
        
        AgentBrainTestFixture() {
            config_ = std::make_shared<nlm::Config>();
            config_->setVisionDimensions(32, 32);
            config_->setTouchCount(12);
            config_->setInternalSignalCount(6);
            config_->setProprioceptionCount(8);
            config_->setMotorActionCount(8);
            
            brain_ = std::make_shared<nlm::Brain>(config_);
            REQUIRE(brain_->initialize());
            
            agentBrain_ = std::make_unique<nlm::AgentBrain>(brain_, config_);
        }
    };
    
    // Basic functionality tests
    TEST_CASE_METHOD(AgentBrainTestFixture, "AgentBrain Construction", "[agentbrain][basic]") {
        REQUIRE(agentBrain_ != nullptr);
        REQUIRE(agentBrain_->getBrain() != nullptr);
        REQUIRE(agentBrain_->isRewardModulationEnabled());
        REQUIRE(agentBrain_->isDevelopmentEnabled());
        REQUIRE(agentBrain_->isCuriosityEnabled());
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "AgentBrain Initialization", "[agentbrain][basic]") {
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(agentBrain_->getMotorOutputSize() == 8);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "SensoryTransduction Component", "[agentbrain][sensory]") {
        nlm::SensoryTransduction st(config_);
        
        // Test vision processing
        std::vector<float> vision(32 * 32, 0.5f);
        nlm::SensoryPercept percept;
        percept.vision = vision;
        
        REQUIRE_NOTHROW(st.processSensoryInput(percept));
        
        REQUIRE(st.getVisionWidth() == 32);
        REQUIRE(st.getVisionHeight() == 32);
        REQUIRE(st.getSensoryInputSize() > 0);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "MotorDecoder Component", "[agentbrain][motor]") {
        // Need exploration agent for motor decoder
        auto explorationAgent = std::make_shared<nlm::ExplorationAgent>(config_);
        explorationAgent->initialize(brain_);
        
        nlm::MotorDecoder md(config_);
        md.initialize(brain_, nlm::AgentBody{}, explorationAgent);
        
        REQUIRE(md.getMotorOutputSize() == 8);
        
        // Test motor command decoding
        nlm::MotorCommand cmd = md.decodeMotorCommand();
        REQUIRE(cmd == nlm::MotorCommand::Wait || cmd == nlm::MotorCommand::MoveForward ||
                cmd == nlm::MotorCommand::MoveBackward || cmd == nlm::MotorCommand::TurnLeft ||
                cmd == nlm::MotorCommand::TurnRight || cmd == nlm::MotorCommand::Interact);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "Neuromodulator Component", "[agentbrain][neuromodulation]") {
        nlm::Neuromodulator nm(config_);
        nm.initialize(brain_);
        
        REQUIRE(nlm::NeuromodulationLevel::None <= nm.getNeuromodulationLevel() &&
                nm.getNeuromodulationLevel() <= nlm::NeuromodulationLevel::Maximum);
        
        // Test reward modulation
        agentBrain_->applyRewardModulation(1.0f, 0.8f);
        float neuromodLevel = agentBrain_->getNeuromodulationLevel();
        REQUIRE(neuromodLevel >= 0.0f);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "Developer Component", "[agentbrain][development]") {
        nlm::Developer developer(config_);
        developer.initialize(brain_);
        
        REQUIRE(agentBrain_->getDevelopmentalStage() == nlm::DevelopmentalStage::Juvenile);
        
        agentBrain_->updateDevelopment(0.01);
        REQUIRE(agentBrain_->getDevelopmentalStage() == nlm::DevelopmentalStage::Juvenile);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "ExplorationAgent Component", "[agentbrain][curiosity]") {
        nlm::ExplorationAgent ea(config_);
        ea.initialize(brain_);
        
        REQUIRE(agentBrain_->getCuriosityLevel() == 0.0f);
        
        // Test curiosity update
        ea.update(0.01, 0.5f); // stimulus, novelty
        REQUIRE(agentBrain_->getCuriosityLevel() > 0.0f);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "Configuration Integration", "[agentbrain][config]") {
        nlm::SimpleWorld world(128, 128);
        agentBrain_->initialize(world);
        
        // Verify configuration integration
        REQUIRE(agentBrain_->getSensoryInputSize() == 464); // 128*128 + touch + internal + proprioception
        REQUIRE(agentBrain_->getMotorOutputSize() == 8);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "AgentBrain Reset", "[agentbrain][basic]") {
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        // Verify initial state
        REQUIRE(agentBrain_->getNeuromodulationLevel() == 0.0f);
        
        // Reset and verify
        agentBrain_->reset();
        REQUIRE(agentBrain_->getNeuromodulationLevel() == 0.0f);
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "Configuration Flag Control", "[agentbrain][config]") {
        // Test flag control
        agentBrain_->enableRewardModulation(false);
        REQUIRE(!agentBrain_->isRewardModulationEnabled());
        
        agentBrain_->enableDevelopment(false);
        REQUIRE(!agentBrain_->isDevelopmentEnabled());
        
        agentBrain_->enableCuriosity(false);
        REQUIRE(!agentBrain_->isCuriosityEnabled());
        
        // Re-enable
        agentBrain_->enableRewardModulation(true);
        REQUIRE(agentBrain_->isRewardModulationEnabled());
    }
    
    TEST_CASE_METHOD(AgentBrainTestFixture, "Complete AgentBrain Integration", "[agentbrain][integration]") {
        nlm::SimpleWorld world(64, 64);
        agentBrain_->initialize(world);
        
        // Create a complete sensory percept
        nlm::SensoryPercept percept;
        percept.vision = std::vector<float>(64 * 64, 0.5f);
        percept.touch = std::vector<float>(12, 0.0f);
        percept.internal = std::vector<float>(6, 0.0f);
        percept.proprioception = std::vector<float>(8, 0.0f);
        
        // Process sensory input
        agentBrain_->processSensoryInput(percept);
        
        // Verify all components are working
        REQUIRE(agentBrain_->getSensoryInputSize() == 310);
        REQUIRE(agentBrain_->getMotorOutputSize() == 8);
        REQUIRE(agentBrain_->getNeuromodulationLevel() >= 0.0f);
        REQUIRE(agentBrain_->getCuriosityLevel() >= 0.0f);
        REQUIRE(agentBrain_->getNoveltyLevel() >= 0.0f);
        
        // Get motor command
        nlm::MotorCommand cmd = agentBrain_->decodeMotorCommand();
        REQUIRE(cmd != nlm::MotorCommand::Invalid);
    }
    
    // Performance benchmarks
    BENCHMARK("AgentBrain Construction", "[agentbrain][benchmark]") {
        auto config = std::make_shared<nlm::Config>();
        config->setVisionDimensions(32, 32);
        config->setTouchCount(12);
        config->setInternalSignalCount(6);
        config->setProprioceptionCount(8);
        config->setMotorActionCount(8);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        return nlm::AgentBrain(brain, config);
    }
    
    BENCHMARK("AgentBrain Sensory Processing", "[agentbrain][benchmark]") {
        auto config = std::make_shared<nlm::Config>();
        config->setVisionDimensions(32, 32);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        nlm::AgentBrain agentBrain(brain, config);
        nlm::SimpleWorld world(32, 32);
        agentBrain.initialize(world);
        
        nlm::SensoryPercept percept;
        percept.vision = std::vector<float>(32 * 32, 0.5f);
        
        for (int i = 0; i < 100; ++i) {
            agentBrain.processSensoryInput(percept);
        }
        
        return agentBrain.getSensoryInputSize();
    }
    
} // namespace test_agentbrain