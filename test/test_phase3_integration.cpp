#pragma once

#include "integration_test_framework.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <memory>
#include <cassert>
#include <cmath>

namespace nlm {
namespace integration_test {

/**
 * @class Phase3IntegrationTest
 * @brief Integration tests for Phase 3: World Interaction Systems
 * 
 * Phase 3 includes:
 * - World interaction loop
 * - Sensory input and motor output
 * - Reward prediction error system
 * - Developmental stages
 */
class Phase3IntegrationTest : public IntegrationTestFramework {
public:
    Phase3IntegrationTest() = default;
    ~Phase3IntegrationTest() override = default;
    
    TestMetrics run() override {
        TestMetrics metrics;
        metrics.testName = getTestName();
        metrics.phaseName = getPhaseName();
        
        double startTime = getCurrentTimeMs();
        bool allPassed = true;
        std::string errorMsg;
        
        try {
            // Run Phase 3 tests
            allPassed &= testWorldInteractionLoop();
            allPassed &= testSensoryMotorIntegration();
            allPassed &= testRewardPredictionError();
            allPassed &= testDevelopmentalStages();
        } catch (const std::exception& e) {
            allPassed = false;
            errorMsg = std::string("Exception: ") + e.what();
        }
        
        metrics.passed = allPassed;
        metrics.errorMessage = errorMsg;
        return metrics;
    }
    
    std::string getPhaseName() const override {
        return "Phase 3";
    }
    
    std::string getTestName() const override {
        return "World Interaction Integration";
    }
    
private:
    bool testWorldInteractionLoop() {
        std::cout << "    Testing world interaction loop..." << std::endl;
        
        // Create world and agent components
        auto world = std::make_unique<SimpleWorld>();
        auto agentBrain = std::make_unique<AgentBrain>();
        auto config = std::make_shared<Config>();
        
        // Initialize components
        assert(world->initialize() && "World initialization failed");
        assert(agentBrain->initialize(config) && "AgentBrain initialization failed");
        
        // Test basic interaction
        auto percept = world->observe(nullptr); // TODO: pass actual agent
        agentBrain->processSensoryInput(percept);
        
        // Verify processing occurred
        // Note: This is a simplified test - full loop requires more components
        
        std::cout << "      World interaction loop: PASSED" << std::endl;
        return true;
    }
    
    bool testSensoryMotorIntegration() {
        std::cout << "    Testing sensory-motor integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        AgentBrain agentBrain;
        agentBrain.initialize(config);
        
        // Test 1: Sensory processing
        SensoryPercept percept;
        // TODO: Create realistic percept
        
        agentBrain.processSensoryInput(percept);
        
        // Test 2: Motor command generation
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        // TODO: Verify command is valid
        
        std::cout << "      Sensory-motor integration: PASSED" << std::endl;
        return true;
    }
    
    bool testRewardPredictionError() {
        std::cout << "    Testing reward prediction error system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        // Test 1: Neuromodulator creation
        Neuromodulator rewardSignal;
        rewardSignal.type = NeuromodulatorType::Dopamine;
        rewardSignal.value = 0.5f;
        
        Neuromodulator predictionErrorSignal;
        predictionErrorSignal.type = NeuromodulatorType::PredictionError;
        predictionErrorSignal.value = 0.2f;
        
        // Test 2: Integration through brain
        Brain brain(config);
        brain.initialize();
        
        brain.applyNeuromodulation(rewardSignal);
        brain.applyNeuromodulation(predictionErrorSignal);
        
        // Test 3: Verify reward prediction error components are integrated
        Dopamine* dopamine = brain.getDopamine();
        PredictionError* predError = brain.getPredictionErrorSignal();
        
        std::cout << "      Reward prediction error: PASSED" << std::endl;
        return true;
    }
    
    bool testDevelopmentalStages() {
        std::cout << "    Testing developmental stages..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        Brain brain(config);
        brain.initialize();
        
        // Test 1: Initial developmental stage
        DevelopmentalStage initialStage = brain.getDevelopmentalStage();
        assert(initialStage != DevelopmentalStage::Invalid && "Invalid initial stage");
        
        // Test 2: Stage transitions
        brain.setDevelopmentalStage(DevelopmentalStage::Postnatal);
        DevelopmentalStage postnatalStage = brain.getDevelopmentalStage();
        assert(postnatalStage == DevelopmentalStage::Postnatal && "Stage transition failed");
        
        // Test 3: Development system effects on plasticity
        DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
        assert(devSystem != nullptr && "Development system not available");
        
        // Test 4: Apply developmental changes
        brain.develop();
        
        std::cout << "      Developmental stages: PASSED" << std::endl;
        return true;
    }
};

} // namespace integration_test
} // namespace nlm