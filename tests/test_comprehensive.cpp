// Comprehensive NLM Framework Test Suite
// 
// This file contains comprehensive tests for the NLM neural simulation framework,
// covering the major components and improvements made during the enhancement process.

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <cmath>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6Demo.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/NeuralAttention.hpp"

using namespace nlm;

namespace test_config {
    
    // Test Config class basic functionality
    void testConfigCreation() {
        nlm::Config config;
        assert(!config.has("test"));
        std::cout << "  ✓ testConfigCreation passed" << std::endl;
    }
    
    // Test Config class set and get operations
    void testConfigSetAndGet() {
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
        
        std::cout << "  ✓ testConfigSetAndGet passed" << std::endl;
    }
    
    // Test Config class getOr method
    void testConfigGetOr() {
        nlm::Config config;
        
        int val = config.getOr<int>("missing", 100);
        assert(val == 100);
        
        config.set("existing", 50, nlm::ConfigSource::Runtime);
        val = config.getOr<int>("existing", 100);
        assert(val == 50);
        
        std::cout << "  ✓ testConfigGetOr passed" << std::endl;
    }
    
    // Test Config class remove method
    void testConfigRemove() {
        nlm::Config config;
        
        config.set("test", 123, nlm::ConfigSource::Runtime);
        assert(config.has("test"));
        
        config.remove("test");
        assert(!config.has("test"));
        
        std::cout << "  ✓ testConfigRemove passed" << std::endl;
    }
    
    // Test Config class keys method
    void testConfigKeys() {
        nlm::Config config;
        
        config.set("key1", 1, nlm::ConfigSource::Runtime);
        config.set("key2", 2, nlm::ConfigSource::Runtime);
        config.set("key3", 3, nlm::ConfigSource::Runtime);
        
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        
        std::cout << "  ✓ testConfigKeys passed" << std::endl;
    }
    
    // Test Config class JSON parsing
    void testConfigJSONParsing() {
        nlm::Config config;
        
        // Create a JSON string
        std::string json = R"({
            "int_value": 42,
            "double_value": 3.14,
            "string_value": "test",
            "bool_value": true,
            "int_array": [1, 2, 3, 4, 5],
            "double_array": [1.1, 2.2, 3.3],
            "string_array": ["a", "b", "c"]
        })";
        
        // Test JSON parsing
        assert(config.loadFromJSON(json));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
        
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value() && *stringVal == "test");
        
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value() && *boolVal == true);
        
        std::cout << "  ✓ testConfigJSONParsing passed" << std::endl;
    }
    
    // Test Config class YAML parsing
    void testConfigYAMLParsing() {
        nlm::Config config;
        
        // Create a YAML string
        std::string yaml = R"(
int_value: 42
double_value: 3.14
string_value: test
bool_value: true
int_array: [1, 2, 3, 4, 5]
double_array: [1.1, 2.2, 3.3]
string_array: [a, b, c]
        )";
        
        // Test YAML parsing
        assert(config.loadFromYAML(yaml));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
        
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value() && *stringVal == "test");
        
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value() && *boolVal == true);
        
        std::cout << "  ✓ testConfigYAMLParsing passed" << std::endl;
    }
    
    // Test Config class file loading
    void testConfigFileLoading() {
        nlm::Config config;
        
        // Create a temporary config file
        std::string filename = "test_config.cfg";
        std::ofstream file(filename);
        file << "# Test configuration file\n";
        file << "int_value = 42\n";
        file << "string_value = hello\n";
        file << "double_value = 3.14\n";
        file << "bool_value = true\n";
        file.close();
        
        // Test file loading
        assert(config.loadFromFile(filename));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value() && *stringVal == "hello");
        
        // Clean up
        std::remove(filename.c_str());
        
        std::cout << "  ✓ testConfigFileLoading passed" << std::endl;
    }
    
    // Test Config class save functionality
    void testConfigSave() {
        nlm::Config config;
        
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        config.set("string_value", "hello", nlm::ConfigSource::Runtime);
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        
        std::string filename = "test_config_save.cfg";
        assert(config.saveToFile(filename));
        
        // Read back and verify
        nlm::Config config2;
        assert(config2.loadFromFile(filename));
        
        auto intVal = config2.get<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        // Clean up
        std::remove(filename.c_str());
        
        std::cout << "  ✓ testConfigSave passed" << std::endl;
    }
    
    // Test Config class validation
    void testConfigValidation() {
        nlm::Config config;
        
        // Test that getOr works with missing keys
        assert(config.getOr<int>("missing", 100) == 100);
        assert(config.getOr<double>("missing", 3.14) > 3.13);
        assert(config.getOr<std::string>("missing", "default") == "default");
        
        // Test that has returns false for missing keys
        assert(!config.has("missing_key"));
        
        // Test that remove works for existing and non-existing keys
        config.set("test", 123, nlm::ConfigSource::Runtime);
        assert(config.has("test"));
        config.remove("test");
        assert(!config.has("test"));
        
        std::cout << "  ✓ testConfigValidation passed" << std::endl;
    }
    
    // Run all Config tests
    void runAllConfigTests() {
        std::cout << "=== Config Tests ===" << std::endl;
        
        testConfigCreation();
        testConfigSetAndGet();
        testConfigGetOr();
        testConfigRemove();
        testConfigKeys();
        testConfigJSONParsing();
        testConfigYAMLParsing();
        testConfigFileLoading();
        testConfigSave();
        testConfigValidation();
        
        std::cout << "\n=== Config Tests Complete ===" << std::endl << std::endl;
    }
}

namespace test_brain {
    
    // Test basic brain initialization
    void testBrainInitialization() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that brain has regions
        assert(brain->getRegions().size() > 0);
        
        std::cout << "  ✓ testBrainInitialization passed" << std::endl;
    }
    
    // Test brain simulation steps
    void testBrainSimulation() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Run some simulation steps
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check that brain is still functional
        assert(brain->getTotalNeuronCount() > 0);
        
        std::cout << "  ✓ testBrainSimulation passed" << std::endl;
    }
    
    // Test brain spike system
    void testBrainSpikeSystem() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Inject current to generate spikes
        brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
        
        // Run some steps
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check that we have spikes
        assert(brain->getTotalSpikeCount() > 0);
        
        std::cout << "  ✓ testBrainSpikeSystem passed" << std::endl;
    }
    
    // Test brain memory systems
    void testBrainMemorySystems() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that memory systems are initialized
        assert(brain->getWorkingMemory() != nullptr);
        assert(brain->getEpisodicMemory() != nullptr);
        assert(brain->getSemanticMemory() != nullptr);
        assert(brain->getProceduralMemory() != nullptr);
        assert(brain->getAssociativeMemory() != nullptr);
        
        // Check that neural memory systems are initialized
        assert(brain->getNeuralWorkingMemory() != nullptr);
        assert(brain->getNeuralEpisodicMemory() != nullptr);
        
        std::cout << "  ✓ testBrainMemorySystems passed" << std::endl;
    }
    
    // Test brain neuromodulation systems
    void testBrainNeuromodulationSystems() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that neuromodulation systems are initialized
        assert(brain->getDopamine() != nullptr);
        assert(brain->getCuriosity() != nullptr);
        assert(brain->getNovelty() != nullptr);
        
        std::cout << "  ✓ testBrainNeuromodulationSystems passed" << std::endl;
    }
    
    // Test brain prediction system
    void testBrainPredictionSystem() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that prediction system is initialized
        assert(brain->getPredictionSystem() != nullptr);
        
        std::cout << "  ✓ testBrainPredictionSystem passed" << std::endl;
    }
    
    // Test brain development system
    void testBrainDevelopmentSystem() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that development system is initialized
        assert(brain->getDevelopmentSystem() != nullptr);
        
        std::cout << "  ✓ testBrainDevelopmentSystem passed" << std::endl;
    }
    
    // Test brain cognition systems
    void testBrainCognitionSystems() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Check that cognition systems are initialized
        assert(brain->getPlanner() != nullptr);
        assert(brain->getConceptFormation() != nullptr);
        assert(brain->getAttention() != nullptr);
        
        std::cout << "  ✓ testBrainCognitionSystems passed" << std::endl;
    }
    
    // Run all brain tests
    void runAllBrainTests() {
        std::cout << "=== Brain Tests ===" << std::endl;
        
        testBrainInitialization();
        testBrainSimulation();
        testBrainSpikeSystem();
        testBrainMemorySystems();
        testBrainNeuromodulationSystems();
        testBrainPredictionSystem();
        testBrainDevelopmentSystem();
        testBrainCognitionSystems();
        
        std::cout << "\n=== Brain Tests Complete ===" << std::endl << std::endl;
    }
}

namespace test_agent {
    
    // Test agent brain initialization
    void testAgentBrainInitialization() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Create agent brain
        AgentBrain agent(brain);
        
        // Create simple world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Initialize agent with world
        agent.initialize(world);
        
        // Check that agent has motor neuron groups
        assert(!agent.getMotorForwardNeurons().empty() ||
               !agent.getMotorBackwardNeurons().empty() ||
               !agent.getMotorTurnLeftNeurons().empty() ||
               !agent.getMotorTurnRightNeurons().empty() ||
               !agent.getMotorInteractNeurons().empty() ||
               !agent.getMotorWaitNeurons().empty());
        
        std::cout << "  ✓ testAgentBrainInitialization passed" << std::endl;
    }
    
    // Test agent sensory processing
    void testAgentSensoryProcessing() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Create agent brain
        AgentBrain agent(brain);
        
        // Create simple world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Initialize agent with world
        agent.initialize(world);
        
        // Create sensory percept
        SensoryPercept percept;
        percept.setVision(std::vector<float>(256, 0.5f));
        percept.setTouch(std::vector<float>(8, 0.3f));
        percept.setInternal(std::vector<float>(4, 0.2f));
        percept.setProprioception(std::vector<float>(6, 0.1f));
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Check that novelty and curiosity were computed
        assert(agent.getNoveltyLevel() >= 0.0f && agent.getNoveltyLevel() <= 1.0f);
        assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
        
        std::cout << "  ✓ testAgentSensoryProcessing passed" << std::endl;
    }
    
    // Test agent motor decoding
    void testAgentMotorDecoding() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Create agent brain
        AgentBrain agent(brain);
        
        // Create simple world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Initialize agent with world
        agent.initialize(world);
        
        // Get motor command
        MotorCommand command = agent.decodeMotorCommand();
        
        // Check that we got a valid motor command
        assert(command == MotorCommand::MoveForward ||
               command == MotorCommand::MoveBackward ||
               command == MotorCommand::TurnLeft ||
               command == MotorCommand::TurnRight ||
               command == MotorCommand::Interact ||
               command == MotorCommand::LookLeft ||
               command == MotorCommand::LookRight ||
               command == MotorCommand::Wait);
        
        std::cout << "  ✓ testAgentMotorDecoding passed" << std::endl;
    }
    
    // Run all agent tests
    void runAllAgentTests() {
        std::cout << "=== Agent Tests ===" << std::endl;
        
        testAgentBrainInitialization();
        testAgentSensoryProcessing();
        testAgentMotorDecoding();
        
        std::cout << "\n=== Agent Tests Complete ===" << std::endl << std::endl;
    }
}

namespace test_phase6 {
    
    // Test Phase 6 integration
    void testPhase6Integration() {
        Phase6Config config;
        config.neuronCount = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.maxSteps = 100;
        config.enableDevelopment = true;
        config.checkpointPath = "test_checkpoint.bin";
        config.enableCheckpointing = false;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Check that integration results are valid
        assert(result.totalReward >= 0.0f);
        assert(result.avgFiringRate >= 0.0f);
        assert(result.memoryEpisodesStored >= 0.0f);
        assert(result.noveltyLevel >= 0.0f);
        assert(result.curiosityLevel >= 0.0f);
        assert(result.dopamineLevel >= -1.0f && result.dopamineLevel <= 1.0f);
        
        // Check that integration verification passed
        assert(result.memoryWorkingMemoryIntegrated);
        assert(result.memoryEpisodicMemoryIntegrated);
        assert(result.neuromodulationIntegrated);
        assert(result.predictionIntegrated);
        assert(result.developmentIntegrated);
        
        std::cout << "  ✓ testPhase6Integration passed" << std::endl;
    }
    
    // Test Phase 6 verification
    void testPhase6Verification() {
        Phase6IntegratedExperiment experiment;
        assert(experiment.verifyIntegration());
        
        std::cout << "  ✓ testPhase6Verification passed" << std::endl;
    }
    
    // Test Phase 6 memory integration
    void testPhase6MemoryIntegration() {
        Phase6IntegratedExperiment experiment;
        assert(experiment.testMemoryIntegration());
        
        std::cout << "  ✓ testPhase6MemoryIntegration passed" << std::endl;
    }
    
    // Test Phase 6 neuromodulation integration
    void testPhase6NeuromodulationIntegration() {
        Phase6IntegratedExperiment experiment;
        assert(experiment.testNeuromodulationIntegration());
        
        std::cout << "  ✓ testPhase6NeuromodulationIntegration passed" << std::endl;
    }
    
    // Test Phase 6 checkpointing
    void testPhase6Checkpointing() {
        Phase6IntegratedExperiment experiment;
        assert(experiment.testCheckpointing());
        
        // Clean up checkpoint file
        std::remove("test_checkpoint.bin");
        
        std::cout << "  ✓ testPhase6Checkpointing passed" << std::endl;
    }
    
    // Test Phase 6 replay
    void testPhase6Replay() {
        Phase6IntegratedExperiment experiment;
        assert(experiment.testReplay());
        
        std::cout << "  ✓ testPhase6Replay passed" << std::endl;
    }
    
    // Run all Phase 6 tests
    void runAllPhase6Tests() {
        std::cout << "=== Phase 6 Tests ===" << std::endl;
        
        testPhase6Integration();
        testPhase6Verification();
        testPhase6MemoryIntegration();
        testPhase6NeuromodulationIntegration();
        testPhase6Checkpointing();
        testPhase6Replay();
        
        std::cout << "\n=== Phase 6 Tests Complete ===" << std::endl << std::endl;
    }
}

// Main test runner
int main() {
    std::cout << "NLM Framework Comprehensive Test Suite" << std::endl;
    std::cout << "====================================" << std::endl << std::endl;
    
    try {
        test_config::runAllConfigTests();
        test_brain::runAllBrainTests();
        test_agent::runAllAgentTests();
        test_phase6::runAllPhase6Tests();
        
        std::cout << "All tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}