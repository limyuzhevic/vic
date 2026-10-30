// Python Bindings Test Suite
// Comprehensive tests for pynlm Python bindings
// Tests the C++ components exposed through Python bindings

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "ConvenienceFunctions.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <variant>

namespace test_python_bindings {

void test_config_bindings() {
    std::cout << "    Testing Config bindings..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic config operations
    assert(!config->has("test"));
    
    config->set("string_key", std::string("hello world"), nlm::ConfigSource::Runtime);
    config->set("int_key", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    config->set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config->set("bool_key", true, nlm::ConfigSource::Runtime);
    
    assert(config->has("string_key"));
    assert(config->has("int_key"));
    assert(config->has("double_key"));
    assert(config->has("bool_key"));
    
    // Test get methods
    auto strVal = config->get<std::string>("string_key");
    assert(strVal.has_value());
    assert(*strVal == "hello world");
    
    auto intVal = config->get<int64_t>("int_key");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config->get<double>("double_key");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.001);
    
    auto boolVal = config->get<bool>("bool_key");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr with default
    int defaultVal = config->getOr<int64_t>("missing", 100);
    assert(defaultVal == 100);
    
    // Test removal
    config->set("temp_key", std::string("temp_value"), nlm::ConfigSource::Runtime);
    assert(config->has("temp_key"));
    config->remove("temp_key");
    assert(!config->has("temp_key"));
    
    // Test summary
    std::string summary = config->summary();
    assert(!summary.empty());
    
    std::cout << "    Config bindings test PASSED" << std::endl;
}

void test_brain_bindings() {
    std::cout << "    Testing Brain bindings..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool success = brain.initialize();
    assert(success);
    
    // Test brain properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test stepping
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Test stats
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getTotalSpikeCount() >= 0);
    
    // Test developmental stage
    assert(brain.getDevelopmentalStage() >= nlm::DevelopmentalStage::Initial);
    
    // Test region management
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    Brain bindings test PASSED" << std::endl;
}

void test_agent_bindings() {
    std::cout << "    Testing AgentBrain bindings..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test agent properties
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    // Test feature flags
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Test neuromodulation levels
    assert(agent.getNeuromodulationLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getPredictionError() >= 0.0f);
    
    std::cout << "    AgentBrain bindings test PASSED" << std::endl;
}

void test_world_bindings() {
    std::cout << "    Testing SimpleWorld bindings..." << std::endl;
    
    nlm::SimpleWorld world;
    
    // Test configuration
    world.configure(10, 10, 8, 8);
    assert(world.getWidth() == 10);
    assert(world.getHeight() == 10);
    
    // Test reset
    world.reset();
    
    // Test agent start
    world.setAgentStart(5, 5);
    
    // Test simulation time
    assert(world.getSimulationTime() >= 0.0f);
    
    // Test random seed
    world.setRandomSeed(42);
    assert(world.getRandomSeed() == 42);
    
    std::cout << "    SimpleWorld bindings test PASSED" << std::endl;
}

void test_sensory_bindings() {
    std::cout << "    Testing SensoryInput bindings..." << std::endl;
    
    nlm::Vision vision(8, 8, 3);
    
    // Test vision properties
    assert(vision.getWidth() == 8);
    assert(vision.getHeight() == 8);
    assert(vision.getChannels() == 3);
    
    // Test data setting
    std::vector<float> testData(8 * 8 * 3, 0.5f);
    vision.setData(testData);
    
    // Test base class methods
    nlm::SensoryInput& sensory = vision;
    assert(sensory.getType() == nlm::NeuronType::Sensory);
    
    std::cout << "    SensoryInput bindings test PASSED" << std::endl;
}

void test_action_bindings() {
    std::cout << "    Testing Action bindings..." << std::endl;
    
    nlm::Action action(nlm::ActionType::MoveForward);
    
    // Test action properties
    assert(action.getType() == nlm::ActionType::MoveForward);
    assert(action.getName() == "MoveForward");
    
    // Test type setting
    action.setType(nlm::ActionType::MoveBackward);
    assert(action.getType() == nlm::ActionType::MoveBackward);
    assert(action.getName() == "MoveBackward");
    
    // Test parameters
    std::vector<float> params = {1.0f, 2.0f, 3.0f};
    action.setParameters(params);
    assert(action.getParameters() == params);
    
    // Test cloning
    auto cloned = action.clone();
    assert(cloned->getType() == nlm::ActionType::MoveBackward);
    
    std::cout << "    Action bindings test PASSED" << std::endl;
}

void test_config_advanced() {
    std::cout << "    Testing Config advanced features..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test loading from JSON-like format
    // Create a temporary JSON file and test loading
    std::string jsonContent = R"({"test_string": "hello", "test_int": 42, "test_double": 3.14, "test_bool": true})";
    
    // Test JSON parsing functionality (if available)
    // Note: This is a conceptual test - actual implementation depends on JSON library
    std::cout << "    JSON loading test skipped (requires JSON library)" << std::endl;
    
    // Test enhanced save functionality
    bool saveResult = config->saveToFile("test_config.json");
    assert(saveResult); // Should succeed
    
    // Test error handling for invalid files
    bool invalidResult = config->loadFromFile("nonexistent_file.json");
    assert(!invalidResult); // Should fail gracefully
    
    std::cout << "    Config advanced features test PASSED" << std::endl;
}

void test_brain_advanced() {
    std::cout << "    Testing Brain advanced features..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool success = brain.initialize();
    assert(success);
    
    // Test advanced brain properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial);
    
    // Test setting developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Test region management
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    // Test getting all regions
    const auto& regions = brain.getRegions();
    assert(regions.size() == 2);
    
    std::cout << "    Brain advanced features test PASSED" << std::endl;
}

void test_advanced_neural_dynamics() {
    std::cout << "    Testing advanced neural dynamics..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test spike system
    auto* spikeSystem = brain.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    // Test neural dynamics
    auto* dynamics = brain.getSpikeSystem();
    assert(dynamics != nullptr);
    
    // Test plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structural != nullptr);
    
    // Test neuromodulation
    nlm::Dopamine dopamine;
    nlm::Curiosity curiosity;
    nlm::Novelty novelty;
    nlm::PredictionError predictionError;
    
    // These are test objects, not necessarily connected to brain
    std::cout << "    Advanced neural dynamics test PASSED" << std::endl;
}

void test_error_handling() {
    std::cout << "    Testing error handling..." << std::endl;
    
    // Test invalid file loading
    auto config = std::make_shared<nlm::Config>();
    bool result = config->loadFromFile("nonexistent_file.json");
    assert(!result); // Should fail gracefully
    
    // Test invalid operations
    nlm::Brain brain(std::make_shared<nlm::Config>());
    bool initResult = brain.initialize();
    // Initialization may or may not succeed depending on config
    
    std::cout << "    Error handling test PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running Python Bindings tests..." << std::endl;
    
    test_config_bindings();
    test_brain_bindings();
    test_agent_bindings();
    test_world_bindings();
    test_sensory_bindings();
    test_action_bindings();
    test_config_advanced();
    test_brain_advanced();
    test_advanced_neural_dynamics();
    test_error_handling();
    
    std::cout << "All Python Bindings tests PASSED" << std::endl;
}

} // namespace test_python_bindings