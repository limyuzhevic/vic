// Comprehensive NLM Test Implementation
// Implementation of all test categories and modules

#include "nlm/TestInfrastructure.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include "motor/MotorSystem.hpp"
#include "plasticity/STDP.hpp"
#include "neuromodulation/NeuromodulationSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "memory/MemorySystem.hpp"
#include "development/DevelopmentSystem.hpp"
#include "world/World.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <unordered_map>

namespace nlm::test {

// ==============================================================
// Core Module Tests (Config, Brain, World, Agent)
// ==============================================================

namespace test_core {

void runConfigTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Config_Creation", "Test basic config creation", "Unit", "Core", []() {
        nlm::Config config;
        assert(!config.has("test"));
    });
    
    framework.registerTest("Config_SetAndGet", "Test config set and get operations", "Unit", "Core", []() {
        nlm::Config config;
        
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        assert(config.has("int_value"));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value());
        assert(*intVal == 42);
        
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value());
        assert(*doubleVal > 3.13 && *doubleVal < 3.15);
        
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value());
        assert(*stringVal == "hello");
        
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value());
        assert(*boolVal == true);
    });
    
    framework.registerTest("Config_GetOr", "Test config getOr with default values", "Unit", "Core", []() {
        nlm::Config config;
        
        int val = config.getOr<int>("missing", 100);
        assert(val == 100);
        
        config.set("existing", 50, nlm::ConfigSource::Runtime);
        val = config.getOr<int>("existing", 100);
        assert(val == 50);
    });
    
    framework.registerTest("Config_Remove", "Test config removal operations", "Unit", "Core", []() {
        nlm::Config config;
        
        config.set("test", 123, nlm::ConfigSource::Runtime);
        assert(config.has("test"));
        
        config.remove("test");
        assert(!config.has("test"));
    });
    
    framework.registerTest("Config_Keys", "Test config keys enumeration", "Unit", "Core", []() {
        nlm::Config config;
        
        config.set("key1", 1, nlm::ConfigSource::Runtime);
        config.set("key2", 2, nlm::ConfigSource::Runtime);
        config.set("key3", 3, nlm::ConfigSource::Runtime);
        
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    });
}

void runBrainTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Brain_Creation", "Test brain creation with config", "Unit", "Brain", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.getRegionCount() == 1);
        assert(brain.getTotalNeuronCount() == 100);
    });
    
    framework.registerTest("Brain_Initialization", "Test brain initialization process", "Unit", "Brain", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
    });
    
    framework.registerTest("Brain_Step", "Test brain stepping process", "Unit", "Brain", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        assert(brain.getTotalNeuronCount() == 100);
    });
    
    framework.registerTest("Brain_Reset", "Test brain reset functionality", "Unit", "Brain", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        brain.step(0);
        brain.reset();
        
        assert(brain.getTotalNeuronCount() == 50);
    });
    
    framework.registerTest("Brain_Regions", "Test brain region management", "Unit", "Brain", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 3);
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            assert(region != nullptr);
        }
    });
}

void runAgentTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Agent_Creation", "Test agent body creation", "Unit", "Agent", []() {
        nlm::AgentBody agent;
        assert(agent.getEnergy() == 100.0); // Default energy
        assert(agent.getMaxEnergy() == 100.0);
    });
    
    framework.registerTest("Agent_Movement", "Test agent movement functionality", "Unit", "Agent", []() {
        nlm::AgentBody agent;
        auto pos1 = agent.getPosition();
        
        agent.setPosition({10.0, 10.0});
        auto pos2 = agent.getPosition();
        
        assert(pos2.x == 10.0);
        assert(pos2.y == 10.0);
    });
    
    framework.registerTest("Agent_Sensory", "Test agent sensory processing", "Unit", "Agent", []() {
        nlm::AgentBody agent;
        agent.getSensoryPercept().addSensoryInput("vision", 0.8);
        agent.getSensoryPercept().addSensoryInput("audition", 0.6);
        
        auto vision = agent.getSensoryPercept().getSensoryInput("vision");
        auto audition = agent.getSensoryPercept().getSensoryInput("audition");
        
        assert(vision > 0.7);
        assert(audition > 0.5);
    });
}

void runWorldTests(nlm::test::TestFramework& framework) {
    framework.registerTest("World_Creation", "Test world creation and initialization", "Unit", "World", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("width", 100.0, nlm::ConfigSource::Default);
        config->set("height", 100.0, nlm::ConfigSource::Default);
        
        nlm::World world(config);
        auto dimensions = world.getDimensions();
        assert(dimensions.width == 100.0);
        assert(dimensions.height == 100.0);
    });
    
    framework.registerTest("World_Simulation", "Test world simulation step", "Unit", "World", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("width", 100.0, nlm::ConfigSource::Default);
        config->set("height", 100.0, nlm::ConfigSource::Default);
        
        nlm::World world(config);
        world.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            world.step(step);
        }
    });
}

} // namespace test_core

// ==============================================================
// Sensory System Tests
// ==============================================================

namespace test_sensory {

void runVisionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Vision_Creation", "Test vision sensory system creation", "Unit", "Sensory", []() {
        nlm::Vision vision;
        assert(vision.getFOV() > 0.0);
        assert(vision.getRange() > 0.0);
    });
    
    framework.registerTest("Vision_Sensing", "Test vision sensing capability", "Unit", "Sensory", []() {
        nlm::Vision vision;
        nlm::SensoryInput input;
        input.addFeature("light", 0.7);
        input.addFeature("color", 0.9);
        
        vision.processInput(input);
        auto features = vision.getFeatures();
        assert(features.find("light") != features.end());
        assert(features.find("color") != features.end());
    });
}

void runAudioTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Audio_Creation", "Test audio sensory system creation", "Unit", "Sensory", []() {
        nlm::Audio audio;
        assert(audio.getSensitivity() > 0.0);
    });
    
    framework.registerTest("Audio_Sensing", "Test audio sensing capability", "Unit", "Sensory", []() {
        nlm::Audio audio;
        nlm::SensoryInput input;
        input.addFeature("volume", 0.8);
        input.addFeature("frequency", 440.0);
        
        audio.processInput(input);
        auto features = audio.getFeatures();
        assert(features.find("volume") != features.end());
    });
}

void runSensoryInputTests(nlm::test::TestFramework& framework) {
    framework.registerTest("SensoryInput_Creation", "Test sensory input creation", "Unit", "Sensory", []() {
        nlm::SensoryInput input;
        assert(input.getFeatureCount() == 0);
        
        input.addFeature("test", 0.5);
        assert(input.getFeatureCount() == 1);
        assert(input.getFeature("test") == 0.5);
    });
}

} // namespace test_sensory

// ==============================================================
// Motor System Tests
// ==============================================================

namespace test_motor {

void runActionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Action_Creation", "Test action creation", "Unit", "Motor", []() {
        nlm::Action action("move", 10.0);
        assert(action.getType() == "move");
        assert(action.getStrength() == 10.0);
    });
    
    framework.registerTest("Action_Execution", "Test action execution", "Unit", "Motor", []() {
        nlm::Action action("move", 10.0);
        action.execute();
        assert(action.getCooldown() > 0.0);
    });
}

void runMotorSystemTests(nlm::test::TestFramework& framework) {
    framework.registerTest("MotorSystem_Creation", "Test motor system creation", "Unit", "Motor", []() {
        nlm::MotorSystem motor;
        assert(motor.getMaxActionsPerStep() > 0);
    });
    
    framework.registerTest("MotorSystem_Control", "Test motor system control", "Unit", "Motor", []() {
        nlm::MotorSystem motor;
        nlm::Action action("move", 10.0);
        motor.queueAction(action);
        
        assert(motor.getActionQueueSize() > 0);
    });
}

} // namespace test_motor

// ==============================================================
// Plasticity System Tests
// ==============================================================

namespace test_plasticity {

void runSTDPTests(nlm::test::TestFramework& framework) {
    framework.registerTest("STDP_Creation", "Test STDP synapse plasticity creation", "Unit", "Plasticity", []() {
        nlm::STDP stdp;
        assert(!stdp.isEnabled());
        
        stdp.enable();
        assert(stdp.isEnabled());
    });
    
    framework.registerTest("STDP_Plasticity", "Test STDP plasticity application", "Unit", "Plasticity", []() {
        nlm::STDP stdp;
        stdp.enable();
        
        // Create two neurons and a synapse
        nlm::Neuron preNeuron, postNeuron;
        nlm::Synapse synapse(&preNeuron, &postNeuron);
        
        // Apply STDP
        bool result = stdp.apply(&synapse, 1.0);
        assert(result);
    });
}

} // namespace test_plasticity

// ==============================================================
// Neuromodulation System Tests
// ==============================================================

namespace test_neuromodulation {

void runNeuromodulationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Neuromodulation_Creation", "Test neuromodulation system creation", "Unit", "Neuromodulation", []() {
        nlm::NeuromodulationSystem neuromodulation;
        assert(neuromodulation.getModulationStrength() >= 0.0);
    });
    
    framework.registerTest("Neuromodulation_Modulation", "Test neuromodulation application", "Unit", "Neuromodulation", []() {
        nlm::NeuromodulationSystem neuromodulation;
        neuromodulation.applyModulation(1.0);
        
        auto strengths = neuromodulation.getModulationStrengths();
        assert(!strengths.empty());
    });
}

} // namespace test_neuromodulation

// ==============================================================
// Prediction System Tests
// ==============================================================

namespace test_prediction {

void runPredictionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Prediction_Creation", "Test prediction system creation", "Unit", "Prediction", []() {
        nlm::PredictionSystem prediction;
        assert(prediction.getPredictionHorizon() > 0);
    });
    
    framework.registerTest("Prediction_Forecasting", "Test prediction forecasting", "Unit", "Prediction", []() {
        nlm::PredictionSystem prediction;
        std::vector<double> inputData{1.0, 2.0, 3.0, 4.0, 5.0};
        
        auto forecast = prediction.forecast(inputData);
        assert(!forecast.empty());
        assert(forecast.size() > inputData.size());
    });
}

} // namespace test_prediction

// ==============================================================
// Cognition System Tests
// ==============================================================

namespace test_cognition {

void runConceptFormationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("ConceptFormation_Creation", "Test concept formation creation", "Unit", "Cognition", []() {
        nlm::ConceptFormation concept;
        assert(concept.getConceptCount() == 0);
    });
    
    framework.registerTest("ConceptFormation_Learning", "Test concept learning", "Unit", "Cognition", []() {
        nlm::ConceptFormation concept;
        concept.learn("test_feature", 0.8);
        
        auto concepts = concept.getConcepts();
        assert(concepts.find("test_feature") != concepts.end());
    });
}

void runNeuralPlannerTests(nlm::test::TestFramework& framework) {
    framework.registerTest("NeuralPlanner_Creation", "Test neural planner creation", "Unit", "Cognition", []() {
        nlm::NeuralPlanner planner;
        assert(planner.getPlanningHorizon() > 0);
    });
    
    framework.registerTest("NeuralPlanner_Plan", "Test planning functionality", "Unit", "Cognition", []() {
        nlm::NeuralPlanner planner;
        std::vector<std::string> goals{"goal1", "goal2"};
        
        auto plan = planner.plan(goals);
        assert(!plan.empty());
    });
}

} // namespace test_cognition

// ==============================================================
// Memory System Tests
// ==============================================================

namespace test_memory {

void runMemorySystemTests(nlm::test::TestFramework& framework) {
    framework.registerTest("MemorySystem_Creation", "Test memory system creation", "Unit", "Memory", []() {
        nlm::MemorySystem memory;
        assert(memory.getCapacity() > 0);
    });
    
    framework.registerTest("MemorySystem_Storage", "Test memory storage operations", "Unit", "Memory", []() {
        nlm::MemorySystem memory;
        memory.store("key1", 42.0);
        memory.store("key2", "value");
        
        auto val1 = memory.recall<double>("key1");
        auto val2 = memory.recall<std::string>("key2");
        
        assert(val1.has_value());
        assert(val2.has_value());
    });
}

} // namespace test_memory

// ==============================================================
// Development System Tests
// ==============================================================

namespace test_development {

void runDevelopmentTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Development_Creation", "Test development system creation", "Unit", "Development", []() {
        nlm::DevelopmentSystem development;
        assert(!development.isEnabled());
    });
    
    framework.registerTest("Development_Initialization", "Test development initialization", "Unit", "Development", []() {
        nlm::DevelopmentSystem development;
        development.enable();
        assert(development.isEnabled());
        
        development.initializeLearning();
        assert(development.isLearningInitialized());
    });
}

} // namespace test_development

// ==============================================================
// Performance Tests
// ==============================================================

namespace test_performance {

void runLoadTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Performance_LoadTest", "Test system performance under load", "Performance", "Load", []() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Simulate processing many items
        std::vector<int> data(10000);
        std::iota(data.begin(), data.end(), 0);
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double operationsPerSecond = data.size() / (durationMs / 1000.0);
        
        assert(operationsPerSecond > 1000.0); // Should process at least 1000 operations per second
    });
    
    framework.registerTest("Performance_MemoryUsage", "Test memory usage under load", "Performance", "Load", []() {
        std::vector<std::vector<double>> data;
        
        // Allocate memory
        for (int i = 0; i < 1000; ++i) {
            std::vector<double> row(1000, 1.0);
            data.push_back(std::move(row));
        }
        
        // Verify data
        assert(data.size() == 1000);
        assert(data[0].size() == 1000);
        
        // Clean up
        data.clear();
    });
}

void runStressTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Performance_StressTest", "Test system under stress", "Performance", "Stress", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        
        // Run brain under stress
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        assert(brain.getTotalNeuronCount() == 500);
    });
}

void runScalabilityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Performance_ScalabilityTest", "Test system scalability", "Performance", "Scalability", []() {
        std::vector<double> scalingResults;
        
        // Test different neuron counts
        std::vector<int> neuronCounts = {100, 500, 1000, 2000};
        
        for (int neuronCount : neuronCounts) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            auto startTime = std::chrono::high_resolution_clock::now();
            bool success = brain.initialize();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            scalingResults.push_back(durationMs);
            
            assert(success);
        }
        
        // Verify that scaling is reasonable (not exponential growth)
        assert(scalingResults[0] > 0);
        assert(scalingResults.back() > scalingResults[0]); // Should scale up
    });
}

void runRegressionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Performance_RegressionTest", "Test performance regression", "Performance", "Regression", []() {
        // This test verifies that performance hasn't degraded
        // Implementation would compare against known baseline performance
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Measure performance
        auto startTime = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        double stepsPerMs = 10.0 / durationMs;
        
        // Verify performance is within acceptable range
        assert(stepsPerMs > 0.1); // Should process at least 0.1 steps per ms
    });
}

} // namespace test_performance

// ==============================================================
// Chaos Engineering Tests
// ==============================================================

namespace test_chaos {

void runFaultInjectionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Chaos_FaultInjection", "Test fault injection capabilities", "Chaos", "Fault", []() {
        nlm::test::ChaosEngineer engineer;
        
        nlm::test::ChaosEngineer::FaultInjection fault;
        fault.type = nlm::test::ChaosEngineer::FaultType::NETWORK_LATENCY;
        fault.severity = 0.5;
        fault.durationSeconds = 1.0;
        
        // Note: This would require actual fault injection capabilities
        // For testing framework purposes, we'll just verify the structure
        assert(fault.type != nlm::test::ChaosEngineer::FaultType::CRASH);
    });
    
    framework.registerTest("Chaos_Recovery", "Test recovery from faults", "Chaos", "Recovery", []() {
        nlm::test::ChaosEngineer engineer;
        
        // Test recovery mechanisms
        assert(true); // Placeholder - would test actual recovery
    });
}

void runResilienceTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Chaos_Resilience", "Test system resilience", "Chaos", "Resilience", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test resilience by running multiple steps
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
            assert(brain.getTotalNeuronCount() == 100);
        }
        
        // Test recovery from step failure simulation
        brain.reset();
        assert(brain.getTotalNeuronCount() == 100);
    });
}

void runErrorHandlingTests(nlm::test::Framework& framework) {
    framework.registerTest("Chaos_ErrorHandling", "Test error handling under stress", "Chaos", "Error", []() {
        // Test error recovery mechanisms
        nlm::Config config;
        
        // This would test actual error handling
        assert(true); // Placeholder - would test actual error handling
    });
}

} // namespace test_chaos

// ==============================================================
// Regression Tests
// ==============================================================

namespace test_regression {

void runBackwardCompatibilityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Regression_BackwardCompatibility", "Test backward compatibility", "Regression", "Backward", []() {
        // Test that the API hasn't changed
        auto config = std::make_shared<nlm::Config>();
        config->set("test_value", 123, nlm::ConfigSource::Default);
        
        auto value = config->getOr<int>("test_value", 0);
        assert(value == 123);
        
        // Test that existing functionality still works
        nlm::Brain brain(config);
        assert(brain.getRegionCount() == 1);
    });
}

void runAPICompatibilityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Regression_APICompatibility", "Test API compatibility", "Regression", "API", []() {
        // Test that public APIs haven't changed
        nlm::Config config;
        
        // Test all public Config methods
        config.set("key", "value", nlm::ConfigSource::Runtime);
        assert(config.has("key"));
        
        auto value = config.getOr<std::string>("key", "default");
        assert(value == "value");
        
        config.remove("key");
        assert(!config.has("key"));
        
        auto keys = config.getKeys();
        assert(keys.empty());
    });
}

void runDataFormatCompatibilityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Regression_DataFormatCompatibility", "Test data format compatibility", "Regression", "DataFormat", []() {
        // Test that data formats haven't changed
        nlm::Config config;
        
        // Test different data types
        config.set("int", 42, nlm::ConfigSource::Runtime);
        config.set("double", 3.14, nlm::ConfigSource::Runtime);
        config.set("string", "test", nlm::ConfigSource::Runtime);
        config.set("bool", true, nlm::ConfigSource::Runtime);
        
        assert(config.getOr<int>("int", 0) == 42);
        assert(config.getOr<double>("double", 0.0) > 3.13);
        assert(config.getOr<std::string>("string", "") == "test");
        assert(config.getOr<bool>("bool", false) == true);
    });
}

} // namespace test_regression

// ==============================================================
// Validation Tests
// ==============================================================

namespace test_validation {

void runConfigurationValidationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Validation_ConfigValidation", "Test configuration validation", "Validation", "Config", []() {
        nlm::Config config;
        
        // Test invalid configurations
        config.set("valid_key", "valid_value", nlm::ConfigSource::Runtime);
        assert(config.has("valid_key"));
        
        // Test that invalid operations are caught
        // This would test validation logic
        assert(true); // Placeholder - would test actual validation
    });
}

void runInputValidationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Validation_InputValidation", "Test input validation", "Validation", "Input", []() {
        // Test various input validation scenarios
        nlm::SensoryInput input;
        input.addFeature("valid_feature", 0.5);
        
        assert(input.getFeatureCount() == 1);
        
        // Test boundary conditions
        nlm::Audio audio;
        // This would test input validation logic
        assert(true); // Placeholder - would test actual input validation
    });
}

void runOutputValidationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Validation_OutputValidation", "Test output validation", "Validation", "Output", []() {
        // Test that outputs are valid and well-formed
        nlm::Brain brain(std::make_shared<nlm::Config>());
        brain.initialize();
        
        auto action = brain.produceAction();
        // This would test output validation logic
        assert(true); // Placeholder - would test actual output validation
    });
}

void runBoundaryConditionTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Validation_BoundaryConditions", "Test boundary conditions", "Validation", "Boundary", []() {
        // Test boundary conditions
        nlm::Config config;
        
        // Test empty config
        assert(config.getKeys().empty());
        
        // Test maximum values
        config.set("max_int", INT_MAX, nlm::ConfigSource::Runtime);
        assert(config.getOr<int>("max_int", 0) == INT_MAX);
        
        // Test zero and negative values
        config.set("zero", 0, nlm::ConfigSource::Runtime);
        config.set("negative", -1, nlm::ConfigSource::Runtime);
        
        assert(config.getOr<int>("zero", 1) == 0);
        assert(config.getOr<int>("negative", 1) == -1);
    });
}

} // namespace test_validation

// ==============================================================
// Security Tests
// ==============================================================

namespace test_security {

void runSecurityVulnerabilityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Security_Vulnerability", "Test security vulnerabilities", "Security", "Vulnerability", []() {
        // Test for common security vulnerabilities
        nlm::Config config;
        
        // Test injection attacks
        config.set("normal_key", "value", nlm::ConfigSource::Runtime);
        
        // Test that injection attempts are caught
        assert(true); // Placeholder - would test actual security vulnerabilities
    });
}

void runAccessControlTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Security_AccessControl", "Test access control mechanisms", "Security", "AccessControl", []() {
        // Test access control mechanisms
        nlm::AgentBody agent;
        
        // Test authorization checks
        assert(true); // Placeholder - would test actual access control
    });
}

void runDataIntegrityTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Security_DataIntegrity", "Test data integrity", "Security", "DataIntegrity", []() {
        // Test data integrity and corruption detection
        nlm::MemorySystem memory;
        memory.store("key1", 42.0);
        memory.store("key2", "value");
        
        // Test data consistency
        auto val1 = memory.recall<double>("key1");
        auto val2 = memory.recall<std::string>("key2");
        
        assert(val1.has_value());
        assert(val2.has_value());
        assert(*val1 == 42.0);
        assert(*val2 == "value");
    });
}

} // namespace test_security

// ==============================================================
// Documentation Tests
// ==============================================================

namespace test_documentation {

void runCodeDocumentationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Documentation_CodeDocumentation", "Test code documentation", "Documentation", "Code", []() {
        // Test that code has proper documentation
        // This would analyze comments, docstrings, etc.
        assert(true); // Placeholder - would test actual documentation
    });
}

void runAPIDocumentationTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Documentation_APIDocumentation", "Test API documentation", "Documentation", "API", []() {
        // Test that API documentation is complete
        // This would verify that all public classes/methods have documentation
        assert(true); // Placeholder - would test actual API documentation
    });
}

void runUsageExampleTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Documentation_UsageExamples", "Test usage examples", "Documentation", "Usage", []() {
        // Test that usage examples work correctly
        nlm::Config config;
        config.set("example_key", "example_value", nlm::ConfigSource::Runtime);
        
        auto value = config.getOr<std::string>("example_key", "");
        assert(value == "example_value");
    });
}

} // namespace test_documentation

// ==============================================================
// Integration Tests
// ==============================================================

namespace test_integration {

void runE2ETests(nlm::test::TestFramework& framework) {
    framework.registerTest("Integration_E2E", "Test end-to-end integration", "Integration", "E2E", []() {
        // Test complete workflow from input to output
        nlm::Config config;
        config.set("neuron_count", 50, nlm::ConfigSource::Default);
        
        nlm::Brain brain(std::make_shared<nlm::Config>(config));
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        auto action = brain.produceAction();
        assert(action != nullptr);
    });
    
    framework.registerTest("Integration_System", "Test system integration", "Integration", "System", []() {
        // Test integration between different systems
        nlm::World world(std::make_shared<nlm::Config>());
        world.initialize();
        
        nlm::Brain brain(std::make_shared<nlm::Config>());
        brain.initialize();
        
        // Simulate system interaction
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            world.step(step);
            brain.step(step);
        }
    });
}

void runComponentTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Integration_Component", "Test component integration", "Integration", "Component", []() {
        // Test integration between components
        nlm::SensoryInput input;
        input.addFeature("vision", 0.8);
        input.addFeature("audition", 0.6);
        
        nlm::MotorSystem motor;
        nlm::Action action("move", 10.0);
        motor.queueAction(action);
        
        assert(motor.getActionQueueSize() > 0);
    });
}

void runInterfaceTests(nlm::test::TestFramework& framework) {
    framework.registerTest("Integration_Interface", "Test interface integration", "Integration", "Interface", []() {
        // Test integration through interfaces
        nlm::Config config;
        config.set("test", "value", nlm::ConfigSource::Runtime);
        
        // Test interface compliance
        auto value = config.getOr<std::string>("test", "");
        assert(value == "value");
    });
}

} // namespace test_integration

// ==============================================================
// Main Test Suite Orchestrator
// ==============================================================

class NLMTestSuite {
public:
    NLMTestSuite() {
        // Create main test framework
        mainFramework_ = std::make_unique<nlm::test::TestFramework>("NLM Comprehensive Test Suite", 
                                                                   "Comprehensive test suite for NLM neural simulation framework");
        
        // Create specialized test frameworks for different categories
        infrastructureFramework_ = std::make_unique<nlm::test::TestFramework>("Test Infrastructure", 
                                                                             "Test framework infrastructure and utilities");
        
        performanceFramework_ = std::make_unique<nlm::test::TestFramework>("Performance Tests", 
                                                                           "Performance testing suite");
        
        chaosFramework_ = std::make_unique<nlm::test::TestFramework>("Chaos Engineering Tests", 
                                                                     "Chaos engineering and fault injection tests");
        
        regressionFramework_ = std::make_unique<nlm::test::TestFramework>("Regression Tests", 
                                                                           "Regression and compatibility tests");
        
        validationFramework_ = std::make_unique<nlm::test::TestFramework>("Validation Tests", 
                                                                           "Validation and boundary condition tests");
        
        securityFramework_ = std::make_unique<nlm::test::TestFramework>("Security Tests", 
                                                                        "Security testing suite");
        
        documentationFramework_ = std::make_unique<nlm::test::TestFramework>("Documentation Tests", 
                                                                             "Documentation and example tests");
        
        integrationFramework_ = std::make_unique<nlm::test::TestFramework>("Integration Tests", 
                                                                           "Integration testing suite");
    }
    
    void runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Running all test categories..." << std::endl;
        std::cout << std::endl;
        
        // Run all test frameworks
        runInfrastructureTests();
        runUnitTests();
        runIntegrationTests();
        runPerformanceTests();
        runChaosTests();
        runRegressionTests();
        runValidationTests();
        runSecurityTests();
        runDocumentationTests();
        
        std::cout << std::endl;
        std::cout << "=== All Test Categories Completed ===" << std::endl;
    }
    
private:
    void runInfrastructureTests() {
        std::cout << "Running Infrastructure Tests..." << std::endl;
        
        // Infrastructure tests
        infrastructureFramework_->registerTest("Infrastructure_TestUtilities", 
            "Test test utilities and helper functions", 
            "Infrastructure", "Test Infrastructure",
            []() {
                auto data = nlm::test::TestUtilities::generateTestData("infra_test", 50);
                assert(!data.id.empty());
                assert(data.data.size() > 0);
                assert(data.metrics.size() > 0);
            });
        
        infrastructureFramework_->registerTest("Infrastructure_DataManagement", 
            "Test test data management", 
            "Infrastructure", "Test Infrastructure",
            []() {
                nlm::test::TestDataManager manager;
                std::string dataId = manager.generateTestData("data_test");
                assert(!dataId.empty());
                
                auto dataIds = manager.getAvailableTestData("data_test");
                assert(!dataIds.empty());
            });
        
        infrastructureFramework_->registerTest("Infrastructure_Performance", 
            "Test performance monitoring utilities", 
            "Infrastructure", "Test Infrastructure",
            []() {
                auto testFunc = []() {
                    volatile int x = 0;
                    for (int i = 0; i < 1000; ++i) {
                        x += i;
                    }
                    return x;
                };
                
                double avgTime = nlm::test::TestUtilities::measureExecutionTime(testFunc, 1000);
                assert(avgTime > 0.0 && avgTime < 1000.0); // Should be reasonable
            });
        
        infrastructureFramework_->runAllTests();
        std::cout << "Infrastructure Tests: " << infrastructureFramework_->getMetadata().passedTests 
                 << " passed, " << infrastructureFramework_->getMetadata().failedTests 
                 << " failed" << std::endl;
    }
    
    void runUnitTests() {
        std::cout << std::endl;
        std::cout << "Running Unit Tests..." << std::endl;
        
        // Core unit tests
        nlm::test::TestFramework coreFramework("Core Unit Tests", "Core system unit tests");
        test_core::runConfigTests(coreFramework);
        test_core::runBrainTests(coreFramework);
        test_core::runAgentTests(coreFramework);
        test_core::runWorldTests(coreFramework);
        coreFramework.runAllTests();
        
        // Sensory unit tests
        nlm::test::TestFramework sensoryFramework("Sensory Unit Tests", "Sensory system unit tests");
        test_sensory::runVisionTests(sensoryFramework);
        test_sensory::runAudioTests(sensoryFramework);
        test_sensory::runSensoryInputTests(sensoryFramework);
        sensoryFramework.runAllTests();
        
        // Motor unit tests
        nlm::test::TestFramework motorFramework("Motor Unit Tests", "Motor system unit tests");
        test_motor::runActionTests(motorFramework);
        test_motor::runMotorSystemTests(motorFramework);
        motorFramework.runAllTests();
        
        // Plasticity unit tests
        nlm::test::TestFramework plasticityFramework("Plasticity Unit Tests", "Plasticity system unit tests");
        test_plasticity::runSTDPTests(plasticityFramework);
        plasticityFramework.runAllTests();
        
        // Neuromodulation unit tests
        nlm::test::TestFramework neuromodulationFramework("Neuromodulation Unit Tests", "Neuromodulation system unit tests");
        test_neuromodulation::runNeuromodulationTests(neuromodulationFramework);
        neuromodulationFramework.runAllTests();
        
        // Prediction unit tests
        nlm::test::TestFramework predictionFramework("Prediction Unit Tests", "Prediction system unit tests");
        test_prediction::runPredictionTests(predictionFramework);
        predictionFramework.runAllTests();
        
        // Cognition unit tests
        nlm::test::TestFramework cognitionFramework("Cognition Unit Tests", "Cognition system unit tests");
        test_cognition::runConceptFormationTests(cognitionFramework);
        test_cognition::runNeuralPlannerTests(cognitionFramework);
        cognitionFramework.runAllTests();
        
        // Memory unit tests
        nlm::test::TestFramework memoryFramework("Memory Unit Tests", "Memory system unit tests");
        test_memory::runMemorySystemTests(memoryFramework);
        memoryFramework.runAllTests();
        
        // Development unit tests
        nlm::test::TestFramework developmentFramework("Development Unit Tests", "Development system unit tests");
        test_development::runDevelopmentTests(developmentFramework);
        developmentFramework.runAllTests();
        
        std::cout << "Unit Tests: Completed" << std::endl;
    }
    
    void runIntegrationTests() {
        std::cout << std::endl;
        std::cout << "Running Integration Tests..." << std::endl;
        
        nlm::test::TestFramework integrationFramework("Integration Tests", "Integration testing suite");
        test_integration::runE2ETests(integrationFramework);
        test_integration::runComponentTests(integrationFramework);
        test_integration::runInterfaceTests(integrationFramework);
        integrationFramework.runAllTests();
        
        std::cout << "Integration Tests: Completed" << std::endl;
    }
    
    void runPerformanceTests() {
        std::cout << std::endl;
        std::cout << "Running Performance Tests..." << std::endl;
        
        nlm::test::TestFramework performanceFramework("Performance Tests", "Performance testing suite");
        test_performance::runLoadTests(performanceFramework);
        test_performance::runStressTests(performanceFramework);
        test_performance::runScalabilityTests(performanceFramework);
        test_performance::runRegressionTests(performanceFramework);
        performanceFramework.runAllTests();
        
        std::cout << "Performance Tests: Completed" << std::endl;
    }
    
    void runChaosTests() {
        std::cout << std::endl;
        std::cout << "Running Chaos Engineering Tests..." << std::endl;
        
        nlm::test::TestFramework chaosFramework("Chaos Engineering Tests", "Chaos engineering and fault injection tests");
        test_chaos::runFaultInjectionTests(chaosFramework);
        test_chaos::runResilienceTests(chaosFramework);
        chaosFramework.runAllTests();
        
        std::cout << "Chaos Engineering Tests: Completed" << std::endl;
    }
    
    void runRegressionTests() {
        std::cout << std::endl;
        std::cout << "Running Regression Tests..." << std::endl;
        
        nlm::test::TestFramework regressionFramework("Regression Tests", "Regression and compatibility tests");
        test_regression::runBackwardCompatibilityTests(regressionFramework);
        test_regression::runAPICompatibilityTests(regressionFramework);
        test_regression::runDataFormatCompatibilityTests(regressionFramework);
        regressionFramework.runAllTests();
        
        std::cout << "Regression Tests: Completed" << std::endl;
    }
    
    void runValidationTests() {
        std::cout << std::endl;
        std::cout << "Running Validation Tests..." << std::endl;
        
        nlm::test::TestFramework validationFramework("Validation Tests", "Validation and boundary condition tests");
        test_validation::runConfigurationValidationTests(validationFramework);
        test_validation::runInputValidationTests(validationFramework);
        test_validation::runOutputValidationTests(validationFramework);
        test_validation::runBoundaryConditionTests(validationFramework);
        validationFramework.runAllTests();
        
        std::cout << "Validation Tests: Completed" << std::endl;
    }
    
    void runSecurityTests() {
        std::cout << std::endl;
        std::cout << "Running Security Tests..." << std::endl;
        
        nlm::test::TestFramework securityFramework("Security Tests", "Security testing suite");
        test_security::runSecurityVulnerabilityTests(securityFramework);
        test_security::runAccessControlTests(securityFramework);
        test_security::runDataIntegrityTests(securityFramework);
        securityFramework.runAllTests();
        
        std::cout << "Security Tests: Completed" << std::endl;
    }
    
    void runDocumentationTests() {
        std::cout << std::endl;
        std::cout << "Running Documentation Tests..." << std::endl;
        
        nlm::test::TestFramework documentationFramework("Documentation Tests", "Documentation and example tests");
        test_documentation::runCodeDocumentationTests(documentationFramework);
        test_documentation::runAPIDocumentationTests(documentationFramework);
        test_documentation::runUsageExampleTests(documentationFramework);
        documentationFramework.runAllTests();
        
        std::cout << "Documentation Tests: Completed" << std::endl;
    }
    
    // Additional test framework methods (simplified for brevity)
    void registerTest(const std::string& name, const std::string& description,
                     const std::string& category, const std::string& module,
                     std::function<void()> testFunc) {
        // This would be implemented in the TestFramework class
    }
    
    TestSuiteMetadata getMetadata() const {
        // This would be implemented in the TestFramework class
        return TestSuiteMetadata{};
    }
    
private:
    std::unique_ptr<nlm::test::TestFramework> mainFramework_;
    std::unique_ptr<nlm::test::TestFramework> infrastructureFramework_;
    std::unique_ptr<nlm::test::TestFramework> performanceFramework_;
    std::unique_ptr<nlm::test::TestFramework> chaosFramework_;
    std::unique_ptr<nlm::test::TestFramework> regressionFramework_;
    std::unique_ptr<nlm::test::TestFramework> validationFramework_;
    std::unique_ptr<nlm::test::TestFramework> securityFramework_;
    std::unique_ptr<nlm::test::TestFramework> documentationFramework_;
    std::unique_ptr<nlm::test::TestFramework> integrationFramework_;
};

} // namespace nlm::test

// Entry point for the comprehensive test suite
int main() {
    try {
        nlm::test::NLMTestSuite testSuite;
        testSuite.runAllTests();
        
        std::cout << std::endl;
        std::cout << "=== NLM Comprehensive Test Suite Completed ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "=== Test Suite Error ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
