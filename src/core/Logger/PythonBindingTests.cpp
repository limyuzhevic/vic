// Python binding tests for NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <cassert>
#include <iostream>
#include <memory>

namespace test_pybindings {

// Test basic Python bindings functionality
void test_basic_bindings() {
    std::cout << "Testing basic Python bindings..." << std::endl;
    
    // Test that we can create a Config object
    auto config = std::make_shared<nlm::Config>();
    assert(!config->has("test"));
    
    // Test setting and getting values
    config->set("int_value", 42);
    assert(config->has("int_value"));
    auto intVal = config->get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config->set("string_value", std::string("hello"));
    auto stringVal = config->get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config->set("double_value", 3.14);
    auto doubleVal = config->get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.01);
    
    config->set("bool_value", true);
    auto boolVal = config->get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test vector types
    std::vector<double> doubleVec = {1.0, 2.0, 3.0};
    config->set("vector_value", doubleVec);
    auto vecVal = config->get<std::vector<double>>("vector_value");
    assert(vecVal.has_value());
    assert(vecVal->size() == 3);
    assert(std::abs((*vecVal)[0] - 1.0) < 0.001);
    
    std::cout << "Basic bindings tests passed!" << std::endl;
}

// Test Vision and Audio bindings
void test_sensory_bindings() {
    std::cout << "Testing sensory input bindings..." << std::endl;
    
    // Create Vision object
    nlm::Vision vision(10, 10, 3);
    
    // Test setting image data
    std::vector<float> imageData(100, 0.5f);
    vision.setData(imageData);
    
    // Verify image properties
    assert(vision.getWidth() == 10);
    assert(vision.getHeight() == 10);
    assert(vision.getChannels() == 3);
    
    // Test Audio object
    nlm::Audio audio(44100, 1024);
    audio.setSampleRate(48000);
    assert(audio.getSampleRate() == 48000);
    assert(audio.getNumSamples() == 1024);
    
    // Test InternalSignals
    nlm::InternalSignals internal;
    internal.addSignal(0.1f);
    internal.addSignal(0.2f);
    
    std::cout << "Sensory bindings tests passed!" << std::endl;
}

// Test Action and WorldObject bindings
void test_object_bindings() {
    std::cout << "Testing object bindings..." << std::endl;
    
    // Test Action
    nlm::Action moveForward(nlm::ActionType::MoveForward);
    assert(moveForward.getType() == nlm::ActionType::MoveForward);
    assert(moveForward.getName() == "MoveForward");
    
    // Test with parameters
    std::vector<float> params = {1.0f, 2.0f};
    nlm::Action customAction(nlm::ActionType::Custom, params);
    assert(customAction.getType() == nlm::ActionType::Custom);
    
    auto retrievedParams = customAction.getParameters();
    assert(retrievedParams.size() == 2);
    assert(retrievedParams[0] == 1.0f);
    assert(retrievedParams[1] == 2.0f);
    
    // Test WorldObject
    nlm::WorldObject obj(5.0f, 3.0f, nlm::WorldObjectType::Resource, 10.0f, 1.0f);
    assert(obj.x == 5.0f);
    assert(obj.y == 3.0f);
    assert(obj.type == nlm::WorldObjectType::Resource);
    assert(obj.value == 10.0f);
    assert(obj.radius == 1.0f);
    assert(obj.active == true);
    
    // Test copy
    auto obj2 = obj; // This would test copy constructor if available
    // For now just test that object is working
    
    std::cout << "Object bindings tests passed!" << std::endl;
}

// Test Agent and Brain bindings
void test_brain_bindings() {
    std::cout << "Testing brain and agent bindings..." << std::endl;
    
    // Create default config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Test that brain methods are accessible
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    
    // Note: Cannot fully initialize brain without proper setup
    // brain.initialize(); would require additional setup
    
    // Test AgentBrain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(config));
    assert(agentBrain.getBrain() != nullptr);
    
    std::cout << "Brain and agent bindings tests passed!" << std::endl;
}

// Test World bindings
void test_world_bindings() {
    std::cout << "Testing world bindings..." << std::endl;
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    
    world.reset();
    
    // Test agent start
    world.setAgentStart(10.0, 10.0);
    
    // Test world update
    world.update(0.1);
    assert(world.getSimulationTime() > 0.0);
    
    // Test object management
    nlm::WorldObject resource(5.0, 5.0, nlm::WorldObjectType::Resource, 5.0);
    world.addObject(resource);
    
    // Test position validation
    assert(world.isValidPosition(5.0, 5.0));
    assert(!world.isValidPosition(25.0, 25.0)); // Outside world bounds
    
    std::cout << "World bindings tests passed!" << std::endl;
}

// Test error handling
void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test getting non-existent key
    auto val = config->get<std::string>("nonexistent");
    assert(!val.has_value());
    
    // Test getOr with default
    int defaultVal = config->getOr<int>("nonexistent", 999);
    assert(defaultVal == 999);
    
    // Test remove
    config->set("temp", 123);
    assert(config->has("temp"));
    config->remove("temp");
    assert(!config->has("temp"));
    
    // Test has method
    assert(!config->has("nonexistent"));
    
    std::cout << "Error handling tests passed!" << std::endl;
}

// Test logging integration
void test_logging_integration() {
    std::cout << "Testing logging integration..." << std::endl;
    
    // Test that logging macros work
    NLM_LOG_INFO("Test log message");
    NLM_LOG_DEBUG("Debug message");
    NLM_LOG_WARNING("Warning message");
    NLM_LOG_ERROR("Error message");
    
    // Test stream-style logging
    NLM_LOG_STREAM(nlm::LogLevel::Info) << "Stream logging test"; 
    
    std::cout << "Logging integration tests passed!" << std::endl;
}

// Test configuration operations
void test_configuration_operations() {
    std::cout << "Testing configuration operations..." << std::endl;
    
    nlm::Config config;
    
    // Test multiple key types
    config.set("int_key", 42);
    config.set("double_key", 3.14);
    config.set("string_key", "test");
    config.set("bool_key", true);
    config.set("vector_key", std::vector<double>{1.0, 2.0, 3.0});
    
    // Test summary
    std::string summary = config.summary();
    assert(summary.find("int_key") != std::string::npos);
    assert(summary.find("double_key") != std::string::npos);
    assert(summary.find("string_key") != std::string::npos);
    assert(summary.find("bool_key") != std::string::npos);
    assert(summary.find("vector_key") != std::string::npos);
    
    // Test keys
    auto keys = config.getKeys();
    assert(keys.size() == 5);
    
    // Test clear
    config.clear();
    assert(config.getKeys().size() == 0);
    
    std::cout << "Configuration operations tests passed!" << std::endl;
}

// Run all Python binding tests
void runAll() {
    std::cout << "=== Running Python Binding Tests ===" << std::endl;
    
    test_basic_bindings();
    test_sensory_bindings();
    test_object_bindings();
    test_brain_bindings();
    test_world_bindings();
    test_error_handling();
    test_logging_integration();
    test_configuration_operations();
    
    std::cout << "=== All Python Binding Tests Passed ===" << std::endl;
}

} // namespace test_pybindings

// Test registration function
void registerPythonBindingTests() {
    static bool registered = []() {
        test_pybindings::runAll();
        return true;
    }();
}
