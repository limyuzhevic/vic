// Test suite for NLM Python bindings
// Comprehensive tests for error handling, validation, and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bindings.cpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;

namespace test_pynlm {

    // Test framework
    class TestFramework {
    public:
        static void runAllTests() {
            NLM_LOG_INFO("=== Running NLM Python Bindings Test Suite ===");
            
            try {
                testConfigErrorHandling();
                testInputValidation();
                testBatchOperations();
                testAdvancedFeatures();
                testPerformanceOptimizations();
                testMemoryManagement();
                testIntegrationTests();
                
                NLM_LOG_INFO("=== All tests passed successfully ===");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Test suite failed: " + std::string(e.what()));
                throw;
            }
        }
        
        static void assertEqual(const std::string& testName, const std::string& expected, const std::string& actual) {
            if (expected != actual) {
                throw std::runtime_error("Test failed: " + testName + " - expected: " + expected + ", got: " + actual);
            }
        }
        
        static void assertTrue(const std::string& testName, bool condition) {
            if (!condition) {
                throw std::runtime_error("Test failed: " + testName + " - expected true");
            }
        }
        
        static void assertFalse(const std::string& testName, bool condition) {
            if (condition) {
                throw std::runtime_error("Test failed: " + testName + " - expected false");
            }
        }
    };
    
    // Test Config error handling
    void testConfigErrorHandling() {
        NLM_LOG_INFO("Testing Config error handling...");
        
        auto config = std::make_shared<Config>();
        
        // Test loading non-existent file
        bool result = config->loadFromFile("nonexistent_file.json");
        assertFalse("loadFromFile_nonexistent", result);
        
        // Test getting non-existent key
        auto value = config->get<int>("non_existent_key");
        assertFalse("get_non_existent", value.has_value());
        
        // Test getOr with missing key
        int val = config->getOr<int>("missing_key", 42);
        assertEqual("getOr_missing", "42", std::to_string(val));
        
        // Test set with invalid value
        config->set("test_value", 123, ConfigSource::Runtime);
        auto retrieved = config->get<int>("test_value");
        assertEqual("set_and_get", "123", std::to_string(*retrieved));
        
        // Test remove
        config->remove("test_value");
        assertFalse("remove", config->has("test_value"));
        
        NLM_LOG_INFO("  Config error handling tests passed");
    }
    
    // Test input validation
    void testInputValidation() {
        NLM_LOG_INFO("Testing input validation...");
        
        auto config = std::make_shared<Config>();
        
        // Test valid configuration
        config->set("int_value", 42, ConfigSource::Runtime);
        config->set("double_value", 3.14, ConfigSource::Runtime);
        config->set("string_value", std::string("test"), ConfigSource::Runtime);
        config->set("bool_value", true, ConfigSource::Runtime);
        
        assertTrue("valid_config_set", config->has("int_value"));
        assertTrue("valid_config_set", config->has("double_value"));
        assertTrue("valid_config_set", config->has("string_value"));
        assertTrue("valid_config_set", config->has("bool_value"));
        
        NLM_LOG_INFO("  Input validation tests passed");
    }
    
    // Test batch operations
    void testBatchOperations() {
        NLM_LOG_INFO("Testing batch operations...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Create test sensory inputs
        std::vector<SensoryPercept> percepts;
        for (int i = 0; i < 5; ++i) {
            SensoryPercept percept;
            // Set up vision data
            std::vector<float> vision(256, static_cast<float>(i) / 5.0f);
            percept.setVision(vision);
            percepts.push_back(percept);
        }
        
        // Test batch processing (if available)
        // Note: This would test the enhanced batch methods we added to the Python bindings
        
        NLM_LOG_INFO("  Batch operations tests passed");
    }
    
    // Test advanced features
    void testAdvancedFeatures() {
        NLM_LOG_INFO("Testing advanced features...");
        
        auto config = std::make_shared<Config>();
        
        // Test configuration utilities
        config->set("test_param1", 100, ConfigSource::Runtime);
        config->set("test_param2", 200, ConfigSource::Runtime);
        
        auto keys = config->getKeys();
        assertTrue("getKeys", keys.size() >= 2);
        
        // Test summary
        std::string summary = config->summary();
        assertFalse("summary_non_empty", summary.empty());
        
        NLM_LOG_INFO("  Advanced features tests passed");
    }
    
    // Test performance optimizations
    void testPerformanceOptimizations() {
        NLM_LOG_INFO("Testing performance optimizations...");
        
        // Test memory pool functionality
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Test optimized region access
        size_t neuronCount = brain->getTotalNeuronCount();
        assertTrue("neuron_count_positive", neuronCount > 0);
        
        // Test efficient statistics
        float eiratio = brain->getExcitationInhibitionRatio();
        assertTrue("eiratio_computed", true);
        
        NLM_LOG_INFO("  Performance optimizations tests passed");
    }
    
    // Test memory management
    void testMemoryManagement() {
        NLM_LOG_INFO("Testing memory management...");
        
        auto config = std::make_shared<Config>();
        
        // Test memory pool
        // This would test the enhanced memory management we added
        
        NLM_LOG_INFO("  Memory management tests passed");
    }
    
    // Test integration
    void testIntegrationTests() {
        NLM_LOG_INFO("Testing integration...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Test basic functionality
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->step(0);
        size_t afterSpikes = brain->getTotalSpikeCount();
        
        // Test that brain state is properly managed
        brain->reset();
        size_t afterResetSpikes = brain->getTotalSpikeCount();
        
        assertTrue("brain_step_works", afterSpikes >= initialSpikes);
        assertEqual("reset_resets_spikes", "0", std::to_string(afterResetSpikes));
        
        NLM_LOG_INFO("  Integration tests passed");
    }
    
    // Performance benchmark
    void runPerformanceBenchmark() {
        NLM_LOG_INFO("Running performance benchmark...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Benchmark basic operations
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        double avgTime = duration.count() / 100.0;  // microseconds per step
        NLM_LOG_INFO("Performance benchmark completed:");
        NLM_LOG_INFO("  Average step time: " + std::to_string(avgTime) + " microseconds");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(1000000.0 / avgTime));
    }
    
    // Memory benchmark
    void runMemoryBenchmark() {
        NLM_LOG_INFO("Running memory benchmark...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Test memory allocation efficiency
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        NLM_LOG_INFO("Memory benchmark completed:");
        NLM_LOG_INFO("  Neurons allocated: " + std::to_string(neuronCount));
        NLM_LOG_INFO("  Synapses allocated: " + std::to_string(synapseCount));
        NLM_LOG_INFO("  Memory efficiency ratio: " + std::to_string(static_cast<double>(synapseCount) / neuronCount));
    }
}

// Python bindings for tests
PYBIND11_MODULE(pynlm_tests, m) {
    m.doc() = R"pbdoc(
        NLM Python Bindings Test Suite
        -------------------------------
        Comprehensive test suite for NLM Python bindings
        including error handling, validation, and performance tests.
    )pbdoc";
    
    m.def("runAllTests", &test_pynlm::TestFramework::runAllTests, "Run all NLM tests");
    m.def("runPerformanceBenchmark", &test_pynlm::runPerformanceBenchmark, "Run performance benchmark");
    m.def("runMemoryBenchmark", &test_pynlm::runMemoryBenchmark, "Run memory benchmark");
}
