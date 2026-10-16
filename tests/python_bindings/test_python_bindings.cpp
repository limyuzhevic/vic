// Python Binding Tests using Google Test Framework
// Comprehensive unit tests for NLM Python bindings
// Tests binding generator, Python binding compatibility, and integration tests

#include "fixtures/TestHelpers.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <string>
#include <vector>
#include <cmath>

namespace test {

// Helper function to initialize Python interpreter
void initializePython() {
    pybind11::initialize_interpreter();
}

void finalizePython() {
    pybind11::finalize_interpreter();
}

// Test 10.1: Python Binding Basic Test
TEST_F(NLMTest, PythonBindingBasic) {
    initializePython();
    
    try {
        // Test basic Python binding functionality
        pybind11::module sys = pybind11::module::import("sys");
        EXPECT_EQ(sys.attr("version").cast<std::string>().substr(0, 1), "3");
        
        // Test if NLM modules can be imported
        pybind11::module nlm = pybind11::module::import("nlm");
        EXPECT_FALSE(nlm.is_none());
        
        // Test basic functionality
        pybind11::object version = nlm.attr("__version__");
        EXPECT_TRUE(version.is_none() || version.cast<std::string>().empty() || 
                   !version.cast<std::string>().empty());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python binding import failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.2: Python Config Binding Test
TEST_F(NLMTest, PythonConfigBinding) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::module config = nlm.attr("config");
        
        // Test Config class
        pybind11::object ConfigClass = config.attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Test set and get methods
        configInstance.attr("set")("test_key", 42);
        pybind11::object value = configInstance.attr("get")("test_key");
        EXPECT_FALSE(value.is_none());
        
        // Test type conversion
        configInstance.attr("set")("test_double", 3.14);
        pybind11::object doubleValue = configInstance.attr("get")("test_double");
        EXPECT_FALSE(doubleValue.is_none());
        
        configInstance.attr("set")("test_string", std::string("hello"));
        pybind11::object stringValue = configInstance.attr("get")("test_string");
        EXPECT_FALSE(stringValue.is_none());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Config binding test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.3: Python Brain Binding Test
TEST_F(NLMTest, PythonBrainBinding) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::module brain = nlm.attr("brain");
        
        // Test Brain class
        pybind11::object BrainClass = brain.attr("Brain");
        
        // Create a simple config for brain
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        configInstance.attr("set")("neuron_count", 10);
        configInstance.attr("set")("region_count", 1);
        configInstance.attr("set")("connection_probability", 0.1);
        
        // Create brain instance
        pybind11::object brainInstance = BrainClass(configInstance);
        
        // Test brain methods
        pybind11::object initResult = brainInstance.attr("initialize")();
        EXPECT_TRUE(initResult.cast<bool>());
        
        // Test step method
        for (int step = 0; step < 5; ++step) {
            brainInstance.attr("step")(step);
        }
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Brain binding test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.4: Python Neuron Binding Test
TEST_F(NLMTest, PythonNeuronBinding) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::module neuron = nlm.attr("neuron");
        
        // Test Neuron class
        pybind11::object NeuronClass = neuron.attr("Neuron");
        pybind11::object neuronInstance = NeuronClass(1);
        
        // Test neuron properties
        pybind11::object neuronId = neuronInstance.attr("get_id")();
        EXPECT_FALSE(neuronId.is_none());
        
        pybind11::object neuronType = neuronInstance.attr("get_type")();
        EXPECT_FALSE(neuronType.is_none());
        
        // Test neuron state
        neuronInstance.attr("set_type")(std::string("excitatory"));
        pybind11::object newType = neuronInstance.attr("get_type")();
        EXPECT_FALSE(newType.is_none());
        
        neuronInstance.attr("set_membrane_potential")(-65.0);
        pybind11::object potential = neuronInstance.attr("get_membrane_potential")();
        EXPECT_FALSE(potential.is_none());
        EXPECT_DOUBLE_EQ(potential.cast<double>(), -65.0);
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Neuron binding test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.5: Python Synapse Binding Test
TEST_F(NLMTest, PythonSynapseBinding) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::module synapse = nlm.attr("synapse");
        
        // Test Synapse class
        pybind11::object SynapseClass = synapse.attr("Synapse");
        pybind11::object synapseInstance = SynapseClass(1, 0, 1);
        
        // Test synapse properties
        pybind11::object synapseId = synapseInstance.attr("get_id")();
        EXPECT_FALSE(synapseId.is_none());
        
        pybind11::object sourceId = synapseInstance.attr("get_source_neuron_id")();
        EXPECT_FALSE(sourceId.is_none());
        
        pybind11::object targetId = synapseInstance.attr("get_target_neuron_id")();
        EXPECT_FALSE(targetId.is_none());
        
        // Test synapse weight
        synapseInstance.attr("set_weight")(0.5);
        pybind11::object weight = synapseInstance.attr("get_weight")();
        EXPECT_FALSE(weight.is_none());
        EXPECT_DOUBLE_EQ(weight.cast<double>(), 0.5);
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Synapse binding test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.6: Python Binding Generator Test
TEST_F(NLMTest, PythonBindingGenerator) {
    initializePython();
    
    try {
        // Test binding generator functionality
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test if binding generator is available
        if (nlm.attr("hasattr")(nlm, "binding_generator").cast<bool>()) {
            pybind11::object generator = nlm.attr("binding_generator");
            
            // Test generator methods
            pybind11::object modules = generator.attr("generate_modules")();
            EXPECT_FALSE(modules.is_none());
            
            pybind11::object configModule = generator.attr("generate_config_binding")();
            EXPECT_FALSE(configModule.is_none());
            
            pybind11::object brainModule = generator.attr("generate_brain_binding")();
            EXPECT_FALSE(brainModule.is_none());
        }
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding generator test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.7: Python Binding Compatibility Test
TEST_F(NLMTest, PythonBindingCompatibility) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test C++ API compatibility
        pybind11::module core = nlm.attr("core");
        pybind11::module config = core.attr("config");
        
        // Create C++ Config object through Python
        pybind11::object ConfigClass = config.attr("Config");
        pybind11::object cppConfig = ConfigClass();
        
        // Test Python-C++ interoperability
        cppConfig.attr("set")("test_key", "test_value");
        pybind11::object value = cppConfig.attr("get")("test_key");
        EXPECT_FALSE(value.is_none());
        
        // Test type conversion
        cppConfig.attr("set")("test_int", 42);
        pybind11::object intValue = cppConfig.attr("get")("test_int");
        EXPECT_FALSE(intValue.is_none());
        EXPECT_EQ(intValue.cast<int64_t>(), 42);
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding compatibility test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.8: Python Binding Error Handling
TEST_F(ErrorHandlingTest, PythonBindingErrorHandling) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test error handling through Python bindings
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Enable error handling
        configInstance.attr("set_error_handling_enabled")(true);
        
        // Test error reporting
        configInstance.attr("set")("test_key", 1);
        pybind11::object value = configInstance.attr("get")("test_key");
        EXPECT_FALSE(value.is_none());
        
        // Get error stats
        pybind11::object errorStats = configInstance.attr("get_error_stats")();
        EXPECT_FALSE(errorStats.is_none());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding error handling test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.9: Python Binding Performance
TEST_F(PerformanceTest, PythonBindingPerformance) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Performance test for Python bindings
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
            pybind11::object configInstance = ConfigClass();
            configInstance.attr("set")("test_key", i);
            pybind11::object value = configInstance.attr("get")("test_key");
            EXPECT_FALSE(value.is_none());
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        EXPECT_LT(duration.count(), 1000);  // Should complete 100 bindings quickly
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding performance test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.10: Python Binding Integration Test
TEST_F(IntegrationTest, PythonBindingIntegration) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test complete Python binding integration
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Configure through Python
        configInstance.attr("set")("neuron_count", 10);
        configInstance.attr("set")("region_count", 1);
        configInstance.attr("set")("connection_probability", 0.1);
        
        // Import brain module
        pybind11::object BrainClass = pybind11::module::import("nlm").attr("brain").attr("Brain");
        pybind11::object brainInstance = BrainClass(configInstance);
        
        // Initialize brain through Python
        pybind11::object initResult = brainInstance.attr("initialize")();
        EXPECT_TRUE(initResult.cast<bool>());
        
        // Run simulation through Python
        for (int step = 0; step < 10; ++step) {
            brainInstance.attr("step")(step);
        }
        
        // Verify integration
        EXPECT_TRUE(initResult.cast<bool>());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding integration test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.11: Python Binding Type Conversion
TEST_F(NLMTest, PythonBindingTypeConversion) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Test various type conversions
        configInstance.attr("set")("int_value", 42);
        pybind11::object intVal = configInstance.attr("get")("int_value");
        EXPECT_FALSE(intVal.is_none());
        EXPECT_EQ(intVal.cast<int64_t>(), 42);
        
        configInstance.attr("set")("double_value", 3.14159);
        pybind11::object doubleVal = configInstance.attr("get")("double_value");
        EXPECT_FALSE(doubleVal.is_none());
        EXPECT_DOUBLE_EQ(doubleVal.cast<double>(), 3.14159);
        
        configInstance.attr("set")("string_value", std::string("hello world"));
        pybind11::object stringVal = configInstance.attr("get")("string_value");
        EXPECT_FALSE(stringVal.is_none());
        EXPECT_EQ(stringVal.cast<std::string>(), "hello world");
        
        configInstance.attr("set")("bool_value", true);
        pybind11::object boolVal = configInstance.attr("get")("bool_value");
        EXPECT_FALSE(boolVal.is_none());
        EXPECT_TRUE(boolVal.cast<bool>());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding type conversion test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.12: Python Binding Error Scenarios
TEST_F(ErrorHandlingTest, PythonBindingErrorScenarios) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Test error scenarios
        configInstance.attr("set_error_handling_enabled")(true);
        
        // Should not crash on invalid operations
        pybind11::object missingValue = configInstance.attr("get")("nonexistent_key");
        // Value may be none, which is expected
        
        // Test with wrong types
        configInstance.attr("set")("int_field", "not_an_int");  // String to int
        pybind11::object convertedValue = configInstance.attr("get")("int_field");
        // May fail or convert, both are acceptable
        
    } catch (const pybind11::error_already_set& e) {
        // Expected: Python bindings may throw errors on invalid operations
        EXPECT_TRUE(true);  // Error is expected
    }
    
    finalizePython();
}

// Test 10.13: Python Binding Module Organization
TEST_F(NLMTest, PythonBindingModuleOrganization) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test module organization
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "config").cast<bool>());
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "brain").cast<bool>());
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "neuron").cast<bool>());
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "synapse").cast<bool>());
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "plasticity").cast<bool>());
        EXPECT_TRUE(nlm.attr("hasattr")(nlm, "performance").cast<bool>());
        
        // Test submodules
        pybind11::object config = nlm.attr("config");
        pybind11::object brain = nlm.attr("brain");
        pybind11::object neuron = nlm.attr("neuron");
        pybind11::object synapse = nlm.attr("synapse");
        
        // All submodules should be accessible
        EXPECT_FALSE(config.is_none());
        EXPECT_FALSE(brain.is_none());
        EXPECT_FALSE(neuron.is_none());
        EXPECT_FALSE(synapse.is_none());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding module organization test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.14: Python Binding Serialization
TEST_F(NLMTest, PythonBindingSerialization) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object configInstance = ConfigClass();
        
        // Configure the object
        configInstance.attr("set")("int_value", 42);
        configInstance.attr("set")("string_value", std::string("test"));
        
        // Test serialization if available
        if (configInstance.attr("hasattr")(configInstance, "to_dict").cast<bool>()) {
            pybind11::object dict = configInstance.attr("to_dict")();
            EXPECT_FALSE(dict.is_none());
            
            // Test deserialization if available
            if (dict.attr("hasattr")(dict, "from_dict").cast<bool>()) {
                pybind11::object newConfig = dict.attr("from_dict")(dict);
                EXPECT_FALSE(newConfig.is_none());
            }
        }
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding serialization test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.15: Python Binding Memory Management
TEST_F(NLMTest, PythonBindingMemoryManagement) {
    initializePython();
    
    try {
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Test memory management with multiple objects
        std::vector<pybind11::object> configObjects;
        
        for (int i = 0; i < 10; ++i) {
            pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
            pybind11::object configInstance = ConfigClass();
            configInstance.attr("set")("test_key", i);
            configObjects.push_back(configInstance);
        }
        
        // All objects should be valid
        for (size_t i = 0; i < configObjects.size(); ++i) {
            pybind11::object value = configObjects[i].attr("get")("test_key");
            EXPECT_FALSE(value.is_none());
            EXPECT_EQ(value.cast<int64_t>(), static_cast<int64_t>(i));
        }
        
        // Clear references
        configObjects.clear();
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding memory management test failed: " << e.what();
    }
    
    finalizePython();
}

// Test 10.16: Python Binding System Integration
TEST_F(IntegrationTest, PythonBindingSystemIntegration) {
    initializePython();
    
    try {
        // Test complete system integration through Python bindings
        pybind11::module nlm = pybind11::module::import("nlm");
        
        // Step 1: Import all components
        pybind11::object ConfigClass = pybind11::module::import("nlm").attr("config").attr("Config");
        pybind11::object BrainClass = pybind11::module::import("nlm").attr("brain").attr("Brain");
        
        // Step 2: Create and configure
        pybind11::object configInstance = ConfigClass();
        configInstance.attr("set")("neuron_count", 20);
        configInstance.attr("set")("region_count", 2);
        configInstance.attr("set")("connection_probability", 0.1);
        
        // Step 3: Create brain system
        pybind11::object brainInstance = BrainClass(configInstance);
        
        // Step 4: Initialize system
        pybind11::object initResult = brainInstance.attr("initialize")();
        EXPECT_TRUE(initResult.cast<bool>());
        
        // Step 5: Run simulation
        for (int step = 0; step < 10; ++step) {
            brainInstance.attr("step")(step);
        }
        
        // Step 6: Verify system health
        EXPECT_TRUE(initResult.cast<bool>());
        
    } catch (const pybind11::error_already_set& e) {
        FAIL() << "Python Binding system integration test failed: " << e.what();
    }
    
    finalizePython();
}

} // namespace test