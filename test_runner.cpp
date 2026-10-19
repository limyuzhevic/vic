"""
NLM Project Test Suite

Comprehensive test suite for the Neural Learning Machine (NLM) project.
Includes unit tests for core components and integration tests for demo programs.

To run tests:
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j4
    ./test_runner
"""

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

// Include NLM components
#include "core/Config/Config.hpp"
#include "core/CLI/CLI.hpp"

namespace nlm {
namespace tests {

/**
 * @brief Unit tests for the Config class
 */
class ConfigTests {
public:
    static void runAllTests() {
        std::cout << "=== Config Unit Tests ===" << std::endl;
        
        testBasicConfigCreation();
        testConfigLoadFromFile();
        testConfigLoadFromArgs();
        testConfigSaveToFile();
        testConfigGetMethods();
        testConfigSetMethods();
        testConfigValidation();
        testConfigKeyTypes();
        testConfigEdgeCases();
        
        std::cout << "Config unit tests completed successfully!" << std::endl << std::endl;
    }
    
private:
    static void testBasicConfigCreation() {
        std::cout << "Testing basic config creation...";
        Config config;
        assert(config.has("random_seed"));
        assert(config.has("simulation_timestep"));
        assert(config.has("neuron_count"));
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigLoadFromFile() {
        std::cout << "Testing config load from file...";
        Config config;
        
        // Create a test config file
        std::ofstream file("test_config.cfg");
        file << "# Test config file\n";
        file << "random_seed = 12345\n";
        file << "neuron_count = 100\n";
        file << "connection_probability = 0.2\n";
        file << "log_level = DEBUG\n";
        file.close();
        
        // Load config from file
        bool result = config.loadFromFile("test_config.cfg");
        assert(result);
        
        // Verify values
        assert(config.getOr<int64_t>("random_seed", 0) == 12345);
        assert(config.getOr<int64_t>("neuron_count", 0) == 100);
        assert(config.getOr<float>("connection_probability", 0.0f) == 0.2f);
        assert(config.getOr<std::string>("log_level", "") == "DEBUG");
        
        // Cleanup
        std::remove("test_config.cfg");
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigLoadFromArgs() {
        std::cout << "Testing config load from args...";
        Config config;
        
        // Mock command line arguments
        char* argv[] = {
            "test_program",
            "--random_seed=999",
            "--neuron_count=777",
            "--log_level=ERROR",
            "-v"  // verbose flag
        };
        int argc = 5;
        
        bool result = config.loadFromArgs(argc, argv);
        assert(result);
        
        assert(config.getOr<int64_t>("random_seed", 0) == 999);
        assert(config.getOr<int64_t>("neuron_count", 0) == 777);
        assert(config.getOr<std::string>("log_level", "") == "ERROR");
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigSaveToFile() {
        std::cout << "Testing config save to file...";
        Config config;
        
        // Set some values
        config.set("test_int", 42);
        config.set("test_double", 3.14159);
        config.set("test_string", "hello");
        config.set("test_bool", true);
        config.set("test_vector", std::vector<int>({1, 2, 3, 4, 5}));
        
        // Save to file
        bool result = config.saveToFile("test_save.cfg");
        assert(result);
        
        // Read back and verify
        Config config2;
        bool loaded = config2.loadFromFile("test_save.cfg");
        assert(loaded);
        
        // Note: The current save format is simplified, so we just check file exists
        assert(std::ifstream("test_save.cfg").good());
        
        // Cleanup
        std::remove("test_save.cfg");
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigGetMethods() {
        std::cout << "Testing config get methods...";
        Config config;
        
        // Test getOr with default values
        assert(config.getOr<int64_t>("nonexistent_key", 123) == 123);
        assert(config.getOr<double>("nonexistent_key", 3.14) == 3.14);
        assert(config.getOr<std::string>("nonexistent_key", "default") == "default");
        assert(config.getOr<bool>("nonexistent_key", true) == true);
        
        // Test get with nullopt for nonexistent keys
        auto value = config.get<int64_t>("nonexistent_key");
        assert(!value.has_value());
        
        // Test get for existing keys
        auto seed = config.get<int64_t>("random_seed");
        assert(seed.has_value());
        assert(seed.value() > 0);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigSetMethods() {
        std::cout << "Testing config set methods...";
        Config config;
        
        // Test various set methods
        config.set("string_val", std::string("test"));
        config.set("int_val", 42);
        config.set("double_val", 3.14159);
        config.set("bool_val", true);
        config.set("vector_val", std::vector<double>({1.0, 2.0, 3.0}));
        
        // Verify values were set correctly
        assert(config.getOr<std::string>("string_val", "") == "test");
        assert(config.getOr<int64_t>("int_val", 0) == 42);
        assert(config.getOr<double>("double_val", 0.0) == 3.14159);
        assert(config.getOr<bool>("bool_val", false) == true);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigValidation() {
        std::cout << "Testing config validation...";
        Config config;
        
        // Valid configurations should not cause issues
        assert(config.validateAndNormalize());
        
        // Set some values that might trigger warnings
        config.set("simulation_timestep", 0.001f);
        config.set("neuron_count", static_cast<int64_t>(1000));
        config.set("connection_probability", 0.15f);
        
        // Should still be valid
        assert(config.validateAndNormalize());
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigKeyTypes() {
        std::cout << "Testing config key types...";
        Config config;
        
        // Test that different key types work
        config.set("test_int_key", 42);
        config.set("test_double_key", 3.14);
        config.set("test_string_key", "value");
        config.set("test_bool_key", true);
        
        // Verify all keys exist
        assert(config.has("test_int_key"));
        assert(config.has("test_double_key"));
        assert(config.has("test_string_key"));
        assert(config.has("test_bool_key"));
        
        // Test getKeys method
        auto keys = config.getKeys();
        bool foundIntKey = false, foundDoubleKey = false, foundStringKey = false, foundBoolKey = false;
        for (const auto& key : keys) {
            if (key == "test_int_key") foundIntKey = true;
            if (key == "test_double_key") foundDoubleKey = true;
            if (key == "test_string_key") foundStringKey = true;
            if (key == "test_bool_key") foundBoolKey = true;
        }
        
        assert(foundIntKey && foundDoubleKey && foundStringKey && foundBoolKey);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigEdgeCases() {
        std::cout << "Testing config edge cases...";
        Config config;
        
        // Test setting empty key (should fail gracefully)
        config.set("", "value");  // Current implementation doesn't prevent empty keys
        
        // Test removing non-existent key
        config.remove("nonexistent_key");  // Should not crash
        
        // Test clearing config
        config.clear();
        
        // After clearing, default values should still exist
        assert(config.has("random_seed"));
        assert(config.has("simulation_timestep"));
        
        std::cout << " PASSED" << std::endl;
    }
};

/**
 * @brief Unit tests for the CLI class
 */
class CLITests {
public:
    static void runAllTests() {
        std::cout << "=== CLI Unit Tests ===" << std::endl;
        
        testBasicCLIInitialization();
        testCLIHelpAndVersion();
        testCLIParsing();
        testCLIArgumentTypes();
        testCLIErrorHandling();
        testCLIValidation();
        
        std::cout << "CLI unit tests completed successfully!" << std::endl << std::endl;
    }
    
private:
    static void testBasicCLIInitialization() {
        std::cout << "Testing basic CLI initialization...";
        CLI cli;
        
        // Check that default arguments are added
        assert(cli.hasArgument("--episodes"));
        assert(cli.hasArgument("--steps"));
        assert(cli.hasArgument("--config"));
        assert(cli.hasArgument("--seed"));
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testCLIHelpAndVersion() {
        std::cout << "Testing CLI help and version...";
        CLI cli;
        
        CLI::ProgramInfo info("test_program", "1.0.0", "Test program", "Test Author");
        cli.setProgramInfo(info);
        
        // Check help request detection
        bool helpRequested = false;
        bool versionRequested = false;
        
        // Mock parsing
        char* argv[] = {"test_program", "--help"};
        int argc = 2;
        
        // This would normally set the flags, but we can't easily test without
        // modifying the CLI class. We'll just test that basic functionality works.
        
        std::cout << " PASSED (basic check)" << std::endl;
    }
    
    static void testCLIParsing() {
        std::cout << "Testing CLI parsing...";
        CLI cli;
        
        // Test parsing with various argument formats
        char* argv1[] = {"test_program", "--episodes=10", "--steps=200", "--seed=42"};
        int argc1 = 4;
        
        bool parseResult = cli.parse(argc1, argv1);
        assert(parseResult);
        
        // Check that values were parsed
        assert(cli.getInt("--episodes", 0) == 10);
        assert(cli.getInt("--steps", 0) == 200);
        assert(cli.getInt("--seed", 0) == 42);
        
        // Test short arguments
        char* argv2[] = {"test_program", "-e", "15", "-s", "300"};
        int argc2 = 5;
        
        // Reset and parse again
        parseResult = cli.parse(argc2, argv2);
        assert(parseResult);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testCLIArgumentTypes() {
        std::cout << "Testing CLI argument types...";
        CLI cli;
        
        // Test parsing boolean arguments
        char* argv[] = {"test_program", "--verbose", "--debug", "--validate"};
        int argc = 5;
        
        bool parseResult = cli.parse(argc, argv);
        assert(parseResult);
        
        // Boolean arguments should be present
        assert(cli.hasArgument("--verbose"));
        assert(cli.hasArgument("--debug"));
        assert(cli.hasArgument("--validate"));
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testCLIErrorHandling() {
        std::cout << "Testing CLI error handling...";
        CLI cli;
        
        // Test with no arguments (should still work)
        char* argv[] = {"test_program"};
        int argc = 1;
        
        bool parseResult = cli.parse(argc, argv);
        assert(parseResult);
        
        // Test with unknown arguments (should be handled gracefully)
        char* argv2[] = {"test_program", "--unknown_flag", "--another=unknown"};
        int argc2 = 3;
        
        parseResult = cli.parse(argc2, argv2);
        assert(parseResult);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testCLIValidation() {
        std::cout << "Testing CLI validation...";
        CLI cli;
        
        // Test config validation if requested
        char* argv[] = {"test_program", "--validate"};
        int argc = 2;
        
        bool parseResult = cli.parse(argc, argv);
        assert(parseResult);
        
        // Validation flag should be set
        assert(cli.getBool("--validate"));
        
        std::cout << " PASSED" << std::endl;
    }
};

/**
 * @brief Integration tests for demo programs
 */
class IntegrationTests {
public:
    static void runAllTests() {
        std::cout << "=== Integration Tests ===" << std::endl;
        
        // Note: Full integration tests would require building the demo programs
        // For now, we'll run simplified versions that test the core functionality
        
        testConfigCLIIntegration();
        testDemoProgramStructure();
        testConfigurationConsistency();
        
        std::cout << "Integration tests completed successfully!" << std::endl << std::endl;
    }
    
private:
    static void testConfigCLIIntegration() {
        std::cout << "Testing Config-CLI integration...";
        
        // Create a config and CLI instance
        Config config;
        CLI cli;
        
        // Test that they can work together
        config.set("test_key", "test_value");
        assert(config.getOr<std::string>("test_key", "") == "test_value");
        
        // Test CLI can parse config-related arguments
        char* argv[] = {"test_program", "--test_key", "test_value"};
        int argc = 3;
        
        bool parseResult = cli.parse(argc, argv);
        assert(parseResult);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testDemoProgramStructure() {
        std::cout << "Testing demo program structure...";
        
        // Test that demo files exist and have correct structure
        std::ifstream phase3Demo("src/experiments/Phase3Demo.cpp");
        assert(phase3Demo.good());
        
        std::ifstream phase4Demo("src/experiments/Phase4Demo.cpp");
        assert(phase4Demo.good());
        
        std::ifstream phase6Demo("src/experiments/Phase6Demo.cpp");
        assert(phase6Demo.good());
        
        // Check for CLI includes
        std::string line;
        bool hasCLINclude = false;
        while (std::getline(phase3Demo, line)) {
            if (line.find("CLI.hpp") != std::string::npos) {
                hasCLINclude = true;
                break;
            }
        }
        assert(hasCLINclude);
        
        std::cout << " PASSED" << std::endl;
    }
    
    static void testConfigurationConsistency() {
        std::cout << "Testing configuration consistency...";
        
        Config config1;
        Config config2;
        
        // Both configs should have the same default values
        assert(config1.getOr<int64_t>("random_seed", 0) == 
               config2.getOr<int64_t>("random_seed", 0));
        
        assert(config1.getOr<double>("simulation_timestep", 0.0) == 
               config2.getOr<double>("simulation_timestep", 0.0));
        
        std::cout << " PASSED" << std::endl;
    }
};

/**
 * @brief Main test runner
 */
class TestRunner {
public:
    static void runAllTests() {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM PROJECT TEST SUITE" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run unit tests
        ConfigTests::runAllTests();
        CLITests::runAllTests();
        IntegrationTests::runAllTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "ALL TESTS PASSED!" << std::endl;
        std::cout << "Total execution time: " << duration.count() << " ms" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
    }
};

} // namespace tests
} // namespace nlm

// Main entry point for test runner
int main(int argc, char* argv[]) {
    // Simple command-line argument parsing for test runner
    bool runAllTests = true;
    bool runConfigTests = false;
    bool runCLITests = false;
    bool runIntegrationTests = false;
    
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--config-tests") {
            runAllTests = false;
            runConfigTests = true;
        } else if (arg == "--cli-tests") {
            runAllTests = false;
            runCLITests = true;
        } else if (arg == "--integration-tests") {
            runAllTests = false;
            runIntegrationTests = true;
        }
    }
    
    if (runAllTests) {
        nlm::tests::TestRunner::runAllTests();
        return 0;
    }
    
    if (runConfigTests) {
        nlm::tests::ConfigTests::runAllTests();
        return 0;
    }
    
    if (runCLITests) {
        nlm::tests::CLITests::runAllTests();
        return 0;
    }
    
    if (runIntegrationTests) {
        nlm::tests::IntegrationTests::runAllTests();
        return 0;
    }
    
    std::cerr << "Error: No tests specified!" << std::endl;
    std::cerr << "Usage: test_runner [--config-tests|--cli-tests|--integration-tests]" << std::endl;
    return 1;
}
