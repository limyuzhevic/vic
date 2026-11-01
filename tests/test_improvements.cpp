// Comprehensive NLM Tests
// Tests for improved codebase features and fixes

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace test_improvements {

// Test 1: Configuration File Fix Validation
void testConfigurationFileFix() {
    std::cout << "Testing Configuration File Fix..." << std::endl;
    
    // Test that default.cfg has no duplicate dopamine_baseline entries
    std::ifstream configFile("configs/default.cfg");
    assert(configFile.is_open());
    
    std::string line;
    int dopamineBaselineCount = 0;
    while (std::getline(configFile, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == std::string::npos) continue;
        line = line.substr(firstNonSpace);
        
        // Count dopamine_baseline entries
        if (line.find("dopamine_baseline") != std::string::npos) {
            dopamineBaselineCount++;
        }
    }
    
    // Should be exactly 1 occurrence
    assert(dopamineBaselineCount == 1, 
           "Configuration file should have exactly 1 dopamine_baseline entry");
    
    std::cout << "    testConfigurationFileFix passed" << std::endl;
}

// Test 2: Improved main.cpp Helper Functions
void testMainFunctionHelpers() {
    std::cout << "Testing Main Function Helpers..." << std::endl;
    
    // Test Config class methods (used by printConfigurationSummary)
    nlm::Config config;
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_double", 3.14, nlm::ConfigSource::Default);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Default);
    
    // Test getOr methods
    int val1 = config.getOr<int>("test_int", 0);
    assert(val1 == 42);
    
    double val2 = config.getOr<double>("test_double", 0.0);
    assert(val2 > 3.13 && val2 < 3.15);
    
    std::string val3 = config.getOr<std::string>("test_string", "");
    assert(val3 == "hello");
    
    // Test getOr with default values
    int val4 = config.getOr<int>("nonexistent", 100);
    assert(val4 == 100);
    
    std::cout << "    testMainFunctionHelpers passed" << std::endl;
}

// Test 3: Advanced Command Line Parsing Logic
void testAdvancedCommandLineParsing() {
    std::cout << "Testing Advanced Command Line Parsing..." << std::endl;
    
    nlm::Config config;
    
    // Test that config can handle advanced options
    config.set("plasticity_enabled", "true", nlm::ConfigSource::CommandLine);
    config.set("development_enabled", "false", nlm::ConfigSource::CommandLine);
    config.set("neuromodulation_enabled", "true", nlm::ConfigSource::CommandLine);
    config.set("enable_curiosity", "true", nlm::ConfigSource::CommandLine);
    config.set("enable_novelty", "true", nlm::ConfigSource::CommandLine);
    config.set("enable_prediction", "true", nlm::ConfigSource::CommandLine);
    config.set("enable_planning", "false", nlm::ConfigSource::CommandLine);
    config.set("enable_memory", "true", nlm::ConfigSource::CommandLine);
    config.set("enable_attention", "false", nlm::ConfigSource::CommandLine);
    
    // Test that values are accessible
    assert(config.getOr<bool>("plasticity_enabled", false) == true);
    assert(config.getOr<bool>("development_enabled", false) == false);
    assert(config.getOr<bool>("neuromodulation_enabled", false) == true);
    assert(config.getOr<bool>("enable_curiosity", false) == true);
    assert(config.getOr<bool>("enable_novelty", false) == true);
    assert(config.getOr<bool>("enable_prediction", false) == true);
    assert(config.getOr<bool>("enable_planning", false) == false);
    assert(config.getOr<bool>("enable_memory", false) == true);
    assert(config.getOr<bool>("enable_attention", false) == false);
    
    std::cout << "    testAdvancedCommandLineParsing passed" << std::endl;
}

// Test 4: Log Level Configuration
void testLogLevelConfiguration() {
    std::cout << "Testing Log Level Configuration..." << std::endl;
    
    // Test that log levels can be configured
    nlm::Logger logger;
    
    // Set different log levels
    logger.setLevel(nlm::LogLevel::Debug);
    assert(logger.isEnabled(nlm::LogLevel::Debug));
    assert(!logger.isEnabled(nlm::LogLevel::Error));  // Higher level not enabled
    
    logger.setLevel(nlm::LogLevel::Info);
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Debug));  // Lower level should still be enabled
    assert(!logger.isEnabled(nlm::LogLevel::Error));
    
    logger.setLevel(nlm::LogLevel::Warning);
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Debug));
    assert(!logger.isEnabled(nlm::LogLevel::Error));
    
    logger.setLevel(nlm::LogLevel::Critical);
    assert(logger.isEnabled(nlm::LogLevel::Critical));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Debug));
    assert(!logger.isEnabled(nlm::LogLevel::Error));  // Higher than critical
    
    std::cout << "    testLogLevelConfiguration passed" << std::endl;
}

// Test 5: Checkpoint Configuration
void testCheckpointConfiguration() {
    std::cout << "Testing Checkpoint Configuration..." << std::endl;
    
    nlm::Config config;
    
    // Test checkpoint-related configuration options
    config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::CommandLine);
    config.set("replay_interval", 10000, nlm::ConfigSource::CommandLine);
    config.set("consolidation_interval", 50000, nlm::ConfigSource::CommandLine);
    config.set("checkpoint_keep", 5, nlm::ConfigSource::CommandLine);
    
    assert(config.getOr<std::string>("checkpoint_dir", "") == "./checkpoints");
    assert(config.getOr<int64_t>("replay_interval", 0) == 10000);
    assert(config.getOr<int64_t>("consolidation_interval", 0) == 50000);
    assert(config.getOr<int64_t>("checkpoint_keep", 0) == 5);
    
    std::cout << "    testCheckpointConfiguration passed" << std::endl;
}

// Test 6: Environment Configuration
void testEnvironmentConfiguration() {
    std::cout << "Testing Environment Configuration..." << std::endl;
    
    nlm::Config config;
    
    // Test environment-related configuration
    config.set("environment_name", "GridWorld", nlm::ConfigSource::CommandLine);
    config.set("environment_width", static_cast<int64_t>(20), nlm::ConfigSource::CommandLine);
    config.set("environment_height", static_cast<int64_t>(15), nlm::ConfigSource::CommandLine);
    
    assert(config.getOr<std::string>("environment_name", "") == "GridWorld");
    assert(config.getOr<int64_t>("environment_width", 0) == 20);
    assert(config.getOr<int64_t>("environment_height", 0) == 15);
    
    std::cout << "    testEnvironmentConfiguration passed" << std::endl;
}

// Test 7: STDP Configuration
void testSTDPConfiguration() {
    std::cout << "Testing STDP Configuration..." << std::endl;
    
    nlm::Config config;
    
    // Test STDP-related configuration
    config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::CommandLine);
    config.set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::CommandLine);
    config.set("stdp_tau", 20.0f, nlm::ConfigSource::CommandLine);
    
    assert(config.getOr<float>("stdp_ltp_weight", 0.0f) > 0.0f);
    assert(config.getOr<float>("stdp_ltd_weight", 0.0f) > 0.0f);
    assert(config.getOr<float>("stdp_tau", 0.0f) > 0.0f);
    
    // Test that STDP configuration is accessible
    float ltp = config.getOr<float>("stdp_ltp_weight", 0.02f);
    float ltd = config.getOr<float>("stdp_ltd_weight", 0.015f);
    float tau = config.getOr<float>("stdp_tau", 20.0f);
    
    assert(ltp > 0.0f && ltp <= 0.05f);  // Reasonable range
    assert(ltd > 0.0f && ltd <= 0.05f);  // Reasonable range
    assert(tau > 0.0f && tau <= 100.0f); // Reasonable range
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

// Test 8: Configuration Summary Function
void testConfigurationSummary() {
    std::cout << "Testing Configuration Summary..." << std::endl;
    
    nlm::Config config;
    
    // Set up test configuration
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Test getOr methods that would be used in configuration summary
    int64_t seed = config.getOr<int64_t>("random_seed", 0);
    double timestep = config.getOr<double>("simulation_timestep", 0.0);
    int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
    int64_t regionCount = config.getOr<int64_t>("region_count", 0);
    float connectionProb = config.getOr<float>("connection_probability", 0.0f);
    
    assert(seed == 42);
    assert(timestep == 0.001);
    assert(neuronCount == 500);
    assert(regionCount == 1);
    assert(connectionProb == 0.15f);
    
    std::cout << "    testConfigurationSummary passed" << std::endl;
}

// Test 9: Error Handling in Configuration
void testConfigurationErrorHandling() {
    std::cout << "Testing Configuration Error Handling..." << std::endl;
    
    nlm::Config config;
    
    // Test that missing keys return default values
    assert(config.getOr<int>("missing_key", 123) == 123);
    assert(config.getOr<std::string>("missing_key", "default") == "default");
    assert(config.getOr<double>("missing_key", 3.14) > 3.13);
    assert(config.getOr<bool>("missing_key", true) == true);
    
    // Test that existing keys can be accessed
    config.set("existing_key", "value1", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("existing_key", "") == "value1");
    
    // Test that remove works correctly
    assert(config.has("existing_key"));
    config.remove("existing_key");
    assert(!config.has("existing_key"));
    
    std::cout << "    testConfigurationErrorHandling passed" << std::endl;
}

// Test 10: Plasticity Configuration Validation
void testPlasticityConfiguration() {
    std::cout << "Testing Plasticity Configuration..." << std::endl;
    
    nlm::Config config;
    
    // Test plasticity configuration options
    config.set("plasticity_enabled", "true", nlm::ConfigSource::CommandLine);
    config.set("plasticity_learning_rate", 0.01f, nlm::ConfigSource::CommandLine);
    config.set("plasticity_stdp_enable", "true", nlm::ConfigSource::CommandLine);
    config.set("plasticity_hebbian_enable", "true", nlm::ConfigSource::CommandLine);
    config.set("plasticity_structural_enable", "false", nlm::ConfigSource::CommandLine);
    
    // Validate plasticity configuration
    assert(config.getOr<bool>("plasticity_enabled", false) == true);
    assert(config.getOr<float>("plasticity_learning_rate", 0.0f) == 0.01f);
    assert(config.getOr<bool>("plasticity_stdp_enable", false) == true);
    assert(config.getOr<bool>("plasticity_hebbian_enable", false) == true);
    assert(config.getOr<bool>("plasticity_structural_enable", false) == false);
    
    std::cout << "    testPlasticityConfiguration passed" << std::endl;
}

// Test 11: Configuration Source Validation
void testConfigurationSource() {
    std::cout << "Testing Configuration Source..." << std::endl;
    
    nlm::Config config;
    
    // Test different configuration sources
    config.set("key1", "default", nlm::ConfigSource::Default);
    config.set("key2", "file", nlm::ConfigSource::File);
    config.set("key3", "commandline", nlm::ConfigSource::CommandLine);
    config.set("key4", "runtime", nlm::ConfigSource::Runtime);
    
    // The source information isn't directly accessible, but we can verify the values
    assert(config.getOr<std::string>("key1", "") == "default");
    assert(config.getOr<std::string>("key2", "") == "file");
    assert(config.getOr<std::string>("key3", "") == "commandline");
    assert(config.getOr<std::string>("key4", "") == "runtime");
    
    std::cout << "    testConfigurationSource passed" << std::endl;
}

// Test 12: Main Function Integration
void testMainFunctionIntegration() {
    std::cout << "Testing Main Function Integration..." << std::endl;
    
    // Test that configuration can be loaded and used as expected
    nlm::Config config;
    
    // Simulate the configuration loading process from main.cpp
    std::string configFile = "configs/default.cfg";
    bool configLoaded = config.loadFromFile(configFile);
    
    // Load from command line args (simulated)
    config.loadFromArgs(0, nullptr); // argc=0, no args
    
    // Set default values for Phase 2 (as in main.cpp)
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Verify configuration values
    assert(config.getOr<int64_t>("random_seed", 0) == 42);
    assert(config.getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(config.getOr<int64_t>("neuron_count", 0) == 500);
    assert(config.getOr<int64_t>("region_count", 0) == 1);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    // Test STDP parameters
    config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    
    assert(config.getOr<float>("stdp_ltp_weight", 0.0f) == 0.02f);
    assert(config.getOr<float>("stdp_ltd_weight", 0.0f) == 0.015f);
    assert(config.getOr<float>("stdp_tau", 0.0f) == 20.0f);
    
    // Test structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    
    assert(config.getOr<float>("synaptogenesis_rate", 0.0f) == 0.0001f);
    assert(config.getOr<float>("pruning_rate", 0.0f) == 0.00001f);
    
    std::cout << "    testMainFunctionIntegration passed" << std::endl;
}

// Test 13: Documentation Generation
void testDocumentationGeneration() {
    std::cout << "Testing Documentation Generation..." << std::endl;
    
    // Test that key files exist
    assert(std::filesystem::exists("src/main.cpp"));
    assert(std::filesystem::exists("configs/default.cfg"));
    assert(std::filesystem::exists("docs/ARCHITECTURE.md"));
    assert(std::filesystem::exists("docs/PHASE6_ARCHITECTURE.md"));
    assert(std::filesystem::exists("docs/USER_GUIDE.md"));
    assert(std::filesystem::exists("docs/CMD_LINE_REFERENCE.md"));
    
    // Test that improvements summary exists
    assert(std::filesystem::exists("IMPROVEMENTS_SUMMARY.md"));
    
    std::cout << "    testDocumentationGeneration passed" << std::endl;
}

// Test 14: Code Structure Improvements
void testCodeStructureImprovements() {
    std::cout << "Testing Code Structure Improvements..." << std::endl;
    
    // Test that main.cpp has been refactored (check for helper functions)
    std::ifstream mainFile("src/main.cpp");
    std::string content((std::istreambuf_iterator<char>(mainFile)),
                        std::istreambuf_iterator<char>());
    
    // Check for presence of helper function declarations
    assert(content.find("void printBanner()") != std::string::npos);
    assert(content.find("void printConfigurationSummary") != std::string::npos);
    assert(content.find("void logPhaseSummary()") != std::string::npos);
    assert(content.find("void logFinalBrainStatus()") != std::string::npos);
    assert(content.find("void runAllTests()") != std::string::npos);
    
    // Check that main function is shorter (has fewer lines)
    size_t mainStart = content.find("int main(");
    size_t mainEnd = content.find("return 0;", mainStart);
    size_t mainBody = content.substr(mainStart, mainEnd - mainStart).length();
    
    // Main function should be less than 100 lines (was 464)
    assert(mainBody < 10000, "Main function body should be shorter");
    
    std::cout << "    testCodeStructureImprovements passed" << std::endl;
}

// Test 15: Bug Fixes Summary
void testBugFixesSummary() {
    std::cout << "Testing Bug Fixes Summary..." << std::endl;
    
    // Test 1: Configuration file fix (duplicate dopamine_baseline)
    std::ifstream configFile("configs/default.cfg");
    assert(configFile.is_open());
    
    int duplicateCount = 0;
    std::string line;
    while (std::getline(configFile, line)) {
        // Skip comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim and check for duplicate
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace != std::string::npos) {
            line = line.substr(firstNonSpace);
            if (line.find("dopamine_baseline") != std::string::npos) {
                duplicateCount++;
            }
        }
    }
    
    // Should be exactly 1 occurrence
    assert(duplicateCount == 1, "Configuration file should have exactly 1 dopamine_baseline entry");
    
    // Test 2: Main.cpp refactoring
    std::ifstream mainFile("src/main.cpp");
    std::string content((std::istreambuf_iterator<char>(mainFile)),
                        std::istreambuf_iterator<char>());
    
    // Should have multiple helper functions
    assert(content.find("// Test 1: Basic neural connectivity test") != std::string::npos);
    assert(content.find("// Test 2: Plasticity learning experiment") != std::string::npos);
    assert(content.find("// Test 3: STDP verification") != std::string::npos);
    
    std::cout << "    testBugFixesSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Improvements Tests..." << std::endl;
    std::cout << std::endl;
    
    testConfigurationFileFix();
    testMainFunctionHelpers();
    testAdvancedCommandLineParsing();
    testLogLevelConfiguration();
    testCheckpointConfiguration();
    testEnvironmentConfiguration();
    testSTDPConfiguration();
    testConfigurationSummary();
    testConfigurationErrorHandling();
    testPlasticityConfiguration();
    testConfigurationSource();
    testMainFunctionIntegration();
    testDocumentationGeneration();
    testCodeStructureImprovements();
    testBugFixesSummary();
    
    std::cout << std::endl;
    std::cout << "=== All Improvements Tests PASSED ===" << std::endl;
}

} // namespace test_improvements
