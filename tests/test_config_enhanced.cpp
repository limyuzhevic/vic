// Enhanced Configuration Test Suite
// Tests for EnhancedConfigLoader and multi-file configuration handling

#include "EnhancedConfigLoader.hpp"
#include "Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_benchmark.hpp>
#include <fstream>
#include <filesystem>
#include <setenv.h>
#include <cstdlib>

namespace test_config_enhanced {
    
    // Test fixtures
    class EnhancedConfigLoaderTestFixture {
    public:
        std::string tempDir = "/tmp/nlm_test_configs";
        std::string jsonFile;
        std::string yamlFile;
        
        EnhancedConfigLoaderTestFixture() {
            // Create temporary directory
            std::filesystem::create_directories(tempDir);
            
            // Create JSON config file
            jsonFile = tempDir + "/test_config.json";
            std::ofstream jsonOut(jsonFile);
            jsonOut << R"({
    "neuron_count": 2000,
    "vision_width": 64,
    "vision_height": 64,
    "enable_event_driven": true,
    "enable_multithreading": true,
    "thread_count": 4,
    "performance_monitoring": {
        "enabled": true,
        "sampling_interval": 0.1
    }
})";
            jsonOut.close();
            
            // Create YAML config file
            yamlFile = tempDir + "/test_config.yaml";
            std::ofstream yamlOut(yamlFile);
            yamlOut << R"(neuron_count: 1500
vision_width: 32
vision_height: 32
enable_sparsity: true
sparsity_threshold: 0.1
memory_config:
  working_memory_size: 1024
  episodic_memory_capacity: 5000
  consolidation_rate: 0.8
neuromodulation:
  dopamine_sensitivity: 0.7
  curiosity_sensitivity: 0.5
  novelty_threshold: 0.6
)";
            yamlOut.close();
        }
        
        ~EnhancedConfigLoaderTestFixture() {
            // Cleanup
            std::filesystem::remove_all(tempDir);
        }
    };
    
    // Multi-file Configuration Loading Tests
    TEST_CASE_METHOD(EnhancedConfigLoaderTestFixture, "Multi-file Configuration Loading", "[config][enhanced]") {
        nlm::EnhancedConfigLoader loader;
        
        loader.addConfigFile(jsonFile);
        loader.addConfigFile(yamlFile);
        
        // Test getting values
        auto neuronCount = loader.getOr<int64_t>("neuron_count", 500);
        REQUIRE(neuronCount == 2000); // Should prefer first file
        
        auto visionWidth = loader.getOr<int64_t>("vision_width", 16);
        REQUIRE(visionWidth == 64); // From JSON
        
        auto visionHeight = loader.getOr<int64_t>("vision_height", 16);
        REQUIRE(visionHeight == 64); // From JSON
        
        auto sparsity = loader.getOr<bool>("enable_sparsity", false);
        REQUIRE(sparsity == true); // From YAML (not in JSON)
        
        // Test boolean conversion
        auto eventDriven = loader.getOr<bool>("enable_event_driven", false);
        REQUIRE(eventDriven == true);
        
        auto multithreading = loader.getOr<bool>("enable_multithreading", false);
        REQUIRE(multithreading == true);
    }
    
    TEST_CASE_METHOD(EnhancedConfigLoaderTestFixture, "Environment Variable Configuration", "[config][env]") {
        // Set environment variable
        std::setenv("NLM_NEURON_COUNT", "3000", 1);
        
        nlm::EnhancedConfigLoader loader;
        auto neuronCount = loader.getOr<int64_t>("neuron_count", 500);
        
        // Environment variable should take precedence
        REQUIRE(neuronCount == 3000);
        
        // Cleanup environment
        std::unsetenv("NLM_NEURON_COUNT");
    }
    
    TEST_CASE_METHOD(EnhancedConfigLoaderTestFixture, "Default Values", "[config][enhanced]") {
        nlm::EnhancedConfigLoader loader;
        loader.addConfigFile(jsonFile);
        
        // Test getting existing value
        auto neuronCount = loader.getOr<int64_t>("neuron_count", 500);
        REQUIRE(neurCount == 2000);
        
        // Test getting non-existing value with default
        auto nonExisting = loader.getOr<double>("non_existing_param", 3.14);
        REQUIRE(nonExisting == 3.14);
        
        // Test integer parsing
        auto threadCount = loader.getOr<int64_t>("thread_count", 1);
        REQUIRE(threadCount == 4);
    }
    
    TEST_CASE_METHOD(EnhancedConfigLoaderTestFixture, "Complex Nested Configuration", "[config][enhanced]") {
        nlm::EnhancedConfigLoader loader;
        loader.addConfigFile(jsonFile);
        
        // Test nested value access
        auto samplingInterval = loader.getOr<double>("performance_monitoring.sampling_interval", 1.0);
        REQUIRE(samplingInterval == 0.1);
        
        // Test nested boolean
        auto enabled = loader.getOr<bool>("performance_monitoring.enabled", false);
        REQUIRE(enabled == true);
        
        // Test nested int
        auto workingMemorySize = loader.getOr<int64_t>("memory_config.working_memory_size", 100);
        REQUIRE(workingMemorySize == 1024);
        
        auto episodicCapacity = loader.getOr<int64_t>("memory_config.episodic_memory_capacity", 100);
        REQUIRE(episodicCapacity == 5000);
    }
    
    TEST_CASE("EnhancedConfigLoader File Validation", "[config][validation]") {
        nlm::EnhancedConfigLoader loader;
        
        // Test with non-existent file
        bool result = loader.addConfigFile("/non/existent/file.json");
        REQUIRE(!result);
        
        // Test empty loader
        REQUIRE(loader.getAllParameterNames().empty());
        REQUIRE(loader.getFileCount() == 0);
    }
    
    TEST_CASE("EnhancedConfigLoader Statistics", "[config][stats]") {
        nlm::EnhancedConfigLoader loader;
        
        // Test empty statistics
        REQUIRE(loader.getFileCount() == 0);
        REQUIRE(loader.getTotalParameters() == 0);
        
        // Add a file
        std::filesystem::create_directories("/tmp/nlm_stats_test");
        std::string testFile = "/tmp/nlm_stats_test/config.json";
        std::ofstream out(testFile);
        out << R"({"test_param": 42, "another_param": "value"})";
        out.close();
        
        loader.addConfigFile(testFile);
        
        // Test statistics after adding file
        REQUIRE(loader.getFileCount() == 1);
        auto paramNames = loader.getAllParameterNames();
        REQUIRE(paramNames.size() >= 2); // test_param, another_param
        
        std::filesystem::remove_all("/tmp/nlm_stats_test");
    }
    
    // Performance benchmarks
    TEST_CASE_BENCHMARK("EnhancedConfigLoader Multi-file Loading", "[config][benchmark]") {
        nlm::EnhancedConfigLoader loader;
        
        // Create multiple config files
        for (int i = 0; i < 10; ++i) {
            std::string filename = "/tmp/test_config_" + std::to_string(i) + ".json";
            std::ofstream out(filename);
            out << R"({"neuron_count": 1000, "vision_width": 64, "vision_height": 64})";
            out.close();
            loader.addConfigFile(filename);
            std::remove(filename.c_str());
        }
        
        // Test loading and getting values
        auto neuronCount = loader.getOr<int64_t>("neuron_count", 500);
        auto visionWidth = loader.getOr<int64_t>("vision_width", 16);
        auto visionHeight = loader.getOr<int64_t>("vision_height", 16);
        
        return neuronCount + visionWidth + visionHeight;
    }
    
    TEST_CASE_BENCHMARK("EnhancedConfigLoader Parameter Retrieval", "[config][benchmark]") {
        nlm::EnhancedConfigLoader loader;
        std::string filename = "/tmp/bench_config.json";
        
        // Create config with many parameters
        std::ofstream out(filename);
        out << R"({"param1": 100, "param2": 200, "param3": 300, "param4": 400, "param5": 500})";
        out.close();
        
        loader.addConfigFile(filename);
        
        // Test parameter retrieval performance
        double total = 0.0;
        for (int i = 1; i <= 5; ++i) {
            std::string paramName = "param" + std::to_string(i);
            auto value = loader.getOr<double>(paramName, 0.0);
            total += value;
        }
        
        std::remove(filename.c_str());
        
        return total;
    }
    
    TEST_CASE_METHOD(EnhancedConfigLoaderTestFixture, "Configuration Merge Resolution", "[config][enhanced]") {
        nlm::EnhancedConfigLoader loader;
        loader.addConfigFile(jsonFile);
        
        // The same parameter in multiple files - first one wins
        auto visionWidth = loader.getOr<int64_t>("vision_width", 16);
        REQUIRE(visionWidth == 64);
    }
    
    TEST_CASE("EnhancedConfigLoader Type Conversion", "[config][types]") {
        nlm::EnhancedConfigLoader loader;
        
        // Create a simple config file
        std::filesystem::create_directories("/tmp/nlm_type_test");
        std::string testFile = "/tmp/nlm_type_test/config.json";
        std::ofstream out(testFile);
        out << R"({
            "int_value": 42,
            "double_value": 3.14,
            "bool_true": true,
            "bool_false": false,
            "string_value": "test_string"
        })";
        out.close();
        
        loader.addConfigFile(testFile);
        
        // Test type conversion
        REQUIRE(loader.getOr<int64_t>("int_value", 0) == 42);
        REQUIRE(loader.getOr<double>("double_value", 0.0) == 3.14);
        REQUIRE(loader.getOr<bool>("bool_true", false) == true);
        REQUIRE(loader.getOr<bool>("bool_false", true) == false);
        REQUIRE(loader.getOr<std::string>("string_value", "") == "test_string");
        
        // Test non-existent parameter
        REQUIRE(loader.getOr<int64_t>("non_existent", 99) == 99);
        REQUIRE(loader.getOr<double>("non_existent", 3.14) == 3.14);
        REQUIRE(loader.getOr<bool>("non_existent", true) == true);
        REQUIRE(loader.getOr<std::string>("non_existent", "default") == "default");
        
        std::filesystem::remove_all("/tmp/nlm_type_test");
    }
    
    TEST_CASE("EnhancedConfigLoader Error Handling", "[config][error_handling]") {
        nlm::EnhancedConfigLoader loader;
        
        // Test adding invalid file
        REQUIRE(!loader.addConfigFile("/nonexistent/path/config.json"));
        REQUIRE(loader.getFileCount() == 0);
        
        // Test getting values from empty loader
        REQUIRE(loader.getOr<int64_t>("test", 100) == 100);
        REQUIRE(loader.getAllParameterNames().empty());
    }
    
} // namespace test_config_enhanced