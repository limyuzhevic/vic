// Comprehensive test for enhanced NLM Python bindings
#include "nlm_bindings.hpp"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <iostream>
#include <cassert>

using namespace pybind11::literals;

void test_pythonic_features() {
    std::cout << "Testing enhanced Python bindings..." << std::endl;
    
    // Test 1: Enhanced ID classes
    {
        std::cout << "Test 1: Enhanced ID classes" << std::endl;
        auto neuron_id = nlm::NeuronId(42);
        
        assert(neuron_id.getValue() == 42);
        assert(neuron_id.getIndex() == 42);
        assert(neuron_id.isValid());
        
        auto invalid_id = nlm::NeuronId::invalid();
        assert(!invalid_id.isValid());
        
        // Test batch creation
        auto batch = nlm::NeuronId::batch(5, 10);
        assert(batch.size() == 5);
        assert(batch[0].getValue() == 10);
        assert(batch[4].getValue() == 14);
        
        // Test range creation
        auto range = nlm::NeuronId::range(5, 10);
        assert(range.size() == 5);
        assert(range[0].getValue() == 5);
        assert(range[4].getValue() == 9);
        
        std::cout << "✓ ID classes test passed" << std::endl;
    }
    
    // Test 2: Context manager support
    {
        std::cout << "Test 2: Context manager support" << std::endl;
        auto config = nlm::python_bindings::createDefaultConfig();
        auto brain = nlm::python_bindings::createBrain(config);
        auto world = nlm::python_bindings::createWorld();
        auto session = nlm::SimulationSession(brain, world);
        
        // Test context manager functionality
        session.enter();
        assert(session.is_active_context());
        
        session.step(1.0);
        
        session.shutdown();
        assert(!session.is_active_context());
        
        std::cout << "✓ Context manager test passed" << std::endl;
    }
    
    // Test 3: Builder pattern
    {
        std::cout << "Test 3: Builder pattern" << std::endl;
        auto builder = nlm::ActionBuilder();
        
        auto action = builder
            .setType(nlm::ActionType::MoveForward)
            .addParameter(0.5f)
            .addParameter(1.0f)
            .build();
        
        assert(action.getType() == nlm::ActionType::MoveForward);
        assert(action.getParameters().size() == 2);
        assert(action.getParameters()[0] == 0.5f);
        assert(action.getParameters()[1] == 1.0f);
        
        // Test reset
        auto reset_builder = builder.reset();
        auto reset_action = reset_builder.setType(nlm::ActionType::Wait).build();
        assert(reset_action.getType() == nlm::ActionType::Wait);
        
        std::cout << "✓ Builder pattern test passed" << std::endl;
    }
    
    // Test 4: Enhanced Config with validation
    {
        std::cout << "Test 4: Enhanced Config with validation" << std::endl;
        auto config = nlm::python_bindings::createDefaultConfig();
        
        // Test validation
        assert(nlm::ConfigValidator::validateSimulationParameters(config));
        
        // Test to_dict extraction
        config.set("test_key", "test_value");
        auto dict = nlm::ConfigValidator::extractSettings(config, {"test_key"});
        assert(dict["test_key"].cast<std::string>() == "test_value");
        
        std::cout << "✓ Enhanced Config test passed" << std::endl;
    }
    
    // Test 5: Serialization
    {
        std::cout << "Test 5: Serialization" << std::endl;
        auto neuron_id = nlm::NeuronId(123);
        
        std::string json = nlm::Serializer::toJson(neuron_id);
        assert(json.find("123") != std::string::npos);
        
        auto loaded_id = nlm::Serializer::fromJson<nlm::NeuronId>(json);
        assert(loaded_id.getValue() == 123);
        
        std::cout << "✓ Serialization test passed" << std::endl;
    }
    
    // Test 6: Statistics calculation
    {
        std::cout << "Test 6: Statistics calculation" << std::endl;
        std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        
        auto stats = nlm::python_bindings::calculateStatistics(data);
        assert(stats["mean"] == 3.0);
        assert(stats["count"] == 5.0);
        assert(stats["min"] == 1.0);
        assert(stats["max"] == 5.0);
        
        std::cout << "✓ Statistics test passed" << std::endl;
    }
    
    std::cout << "\n✅ All Pythonic features tests passed!" << std::endl;
}

int main() {
    try {
        test_pythonic_features();
        std::cout << "\n🎉 All enhanced NLM Python binding tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with error: " << e.what() << std::endl;
        return 1;
    }
}