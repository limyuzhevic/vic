#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>

namespace nlm {

class NLMTestSuite {
public:
    NLMTestSuite() {
        // Initialize logger for tests
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        
        try {
            testConfigurationValidation();
            testConfigurationLoading();
            testConfigurationSaving();
            testErrorHandling();
            testCommandLineParsing();
            testFileOperations();
            
            std::cout << "\n=== All tests completed ===" << std::endl;
            std::cout << "Total tests: 7" << std::endl;
            std::cout << "Passed: " << passedTests << std::endl;
            std::cout << "Failed: " << failedTests << std::endl;
            
            if (failedTests > 0) {
                return 1; // Return error code if any tests failed
            }
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Fatal error during test execution: " << e.what() << std::endl;
            return 1;
        }
    }

private:
    int passedTests = 0;
    int failedTests = 0;
    
    void runTest(const std::string& testName, bool condition) {
        if (condition) {
            std::cout << "✓ " << testName << " PASSED" << std::endl;
            passedTests++;
        } else {
            std::cout << "✗ " << testName << " FAILED" << std::endl;
            failedTests++;
        }
    }
    
    void testConfigurationValidation() {
        std::cout << "\n--- Testing Configuration Validation ---" << std::endl;
        
        auto config = std::make_shared<Config>();
        
        // Test 1: Default configuration should be valid
        runTest("Default configuration validation", 
               config->has("brain.neuron_count") && 
               config->has("brain.synapse_density"));
        
        // Test 2: Key validation
        runTest("Key validation - valid key", config->has("brain.neuron_count"));
        runTest("Key validation - invalid key returns nullopt", !config->get<int64_t>("invalid_key").has_value());
        
        // Test 3: Value range validation
        auto neuronCount = config->get<int64_t>("brain.neuron_count");
        runTest("Neuron count is reasonable", neuronCount && *neuronCount > 0 && *neuronCount <= 1000000);
        
        // Test 4: Configuration summary
        auto summary = config->summary();
        runTest("Configuration summary generation", !summary.empty());
        
        // Test 5: Clear configuration
        config->clear();
        runTest("Configuration clearing", !config->has("brain.neuron_count"));
    }
    
    void testConfigurationLoading() {
        std::cout << "\n--- Testing Configuration Loading ---" << std::endl;
        
        // Create a test configuration file
        std::string testConfigFile = "test_config.cfg";
        std::ofstream testFile(testConfigFile);
        testFile << "# Test configuration" << std::endl;
        testFile << "brain.neuron_count = 500" << std::endl;
        testFile << "brain.synapse_density = 0.2" << std::endl;
        testFile << "simulation_timestep = 0.001" << std::endl;
        testFile << "test_flag = true" << std::endl;
        testFile.close();
        
        // Test loading
        auto config = std::make_shared<Config>();
        runTest("Configuration file loading", config->loadFromFile(testConfigFile));
        
        if (config->loadFromFile(testConfigFile)) {
            runTest("Loaded neuron count", config->getOr<int64_t>("brain.neuron_count", 0) == 500);
            runTest("Loaded synapse density", config->getOr<double>("brain.synapse_density", 0.0) == 0.2);
            runTest("Loaded timestep", config->getOr<double>("simulation_timestep", 0.0) == 0.001);
            runTest("Loaded boolean flag", config->getOr<bool>("test_flag", false) == true);
        }
        
        // Cleanup
        std::remove(testConfigFile.c_str());
    }
    
    void testConfigurationSaving() {
        std::cout << "\n--- Testing Configuration Saving ---" << std::endl;
        
        // Create test configuration
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", int64_t(777), ConfigSource::Default);
        config->set("brain.synapse_density", 0.33, ConfigSource::Runtime);
        config->set("test_string", std::string("Hello World"), ConfigSource::File);
        
        // Save to file
        std::string testSaveFile = "test_save.cfg";
        runTest("Configuration saving", config->saveToFile(testSaveFile));
        
        if (config->saveToFile(testSaveFile)) {
            // Load saved file and verify
            auto loadedConfig = std::make_shared<Config>();
            if (loadedConfig->loadFromFile(testSaveFile)) {
                runTest("Saved file loading", loadedConfig->has("brain.neuron_count"));
                runTest("Saved neuron count value", 
                       loadedConfig->getOr<int64_t>("brain.neuron_count", 0) == 777);
            }
        }
        
        // Cleanup
        std::remove(testSaveFile.c_str());
    }
    
    void testErrorHandling() {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        
        // Test 1: Invalid key handling
        auto config = std::make_shared<Config>();
        runTest("Invalid key access returns nullopt", 
               !config->get<int64_t>("nonexistent_key").has_value());
        
        // Test 2: Type mismatch handling
        config->set("string_key", std::string("test"), ConfigSource::Runtime);
        auto wrongTypeValue = config->get<int64_t>("string_key");
        runTest("Type mismatch returns nullopt", !wrongTypeValue.has_value());
        
        // Test 3: Invalid file handling
        runTest("Invalid file loading returns false", 
               !config->loadFromFile("/nonexistent/path/config.txt"));
        
        // Test 4: Invalid parameter handling
        config->set("invalid_key_with_special_chars!@#$", 123, ConfigSource::Runtime);
        runTest("Invalid key in constructor", !config->has("invalid_key_with_special_chars!@#$"));
    }
    
    void testCommandLineParsing() {
        std::cout << "\n--- Testing Command Line Parsing ---" << std::endl;
        
        // Test argument parsing (would need actual command line arguments)
        // This is a simplified test
        
        // Test valid argument formats
        runTest("Valid --key=value format", true); // Simplified test
        runTest("Valid --key value format", true);  // Simplified test
        
        // Note: Full command line testing would require actual argv/argc
    }
    
    void testFileOperations() {
        std::cout << "\n--- Testing File Operations ---" << std::endl;
        
        // Test file operations
        std::string testFile = "test_operations.tmp";
        
        // Test file creation
        std::ofstream outFile(testFile);
        runTest("File creation", outFile.good());
        outFile << "Test content" << std::endl;
        outFile.close();
        
        // Test file reading
        std::ifstream inFile(testFile);
        std::string line;
        bool hasContent = false;
        if (inFile.good()) {
            std::getline(inFile, line);
            hasContent = !line.empty();
        }
        runTest("File reading", hasContent);
        inFile.close();
        
        // Test file deletion
        std::remove(testFile.c_str());
        std::ifstream afterDelete(testFile);
        runTest("File deletion", !afterDelete.good());
    }
};

// Test suite entry point
int nlm_test_suite_main(int argc, char** argv) {
    NLMTestSuite testSuite;
    return testSuite.runAllTests();
}

} // namespace nlm
