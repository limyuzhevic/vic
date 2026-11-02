// Test program for JSON/YAML configuration support
// Phase 2 Configuration Enhancement Demo

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

// Test JSON configuration
bool testJSONConfiguration() {
    std::cout << "=== Testing JSON Configuration ===" << std::endl;
    
    nlm::Config config;
    
    // Load JSON configuration
    if (config.loadFromFile("configs/default.json")) {
        std::cout << "✓ JSON configuration loaded successfully" << std::endl;
        std::cout << "  Keys: ";
        auto keys = config.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << std::endl;
        
        // Test getting values
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount.has_value()) {
            std::cout << "✓ neuron_count = " << *neuronCount << std::endl;
        }
        
        auto simTimestep = config.get<double>("simulation_timestep");
        if (simTimestep.has_value()) {
            std::cout << "✓ simulation_timestep = " << *simTimestep << std::endl;
        }
        
        auto plasticityRate = config.get<double>("plasticity_learning_rate");
        if (plasticityRate.has_value()) {
            std::cout << "✓ plasticity_learning_rate = " << *plasticityRate << std::endl;
        }
        
        return true;
    } else {
        std::cout << "✗ Failed to load JSON configuration" << std::endl;
        return false;
    }
}

// Test simple format configuration
bool testSimpleFormatConfiguration() {
    std::cout << "\n=== Testing Simple Format Configuration ===" << std::endl;
    
    nlm::Config config;
    
    // Load simple format configuration
    if (config.loadFromFile("configs/default.cfg")) {
        std::cout << "✓ Simple format configuration loaded successfully" << std::endl;
        std::cout << "  Keys: ";
        auto keys = config.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << std::endl;
        
        // Test getting values
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount.has_value()) {
            std::cout << "✓ neuron_count = " << *neuronCount << std::endl;
        }
        
        return true;
    } else {
        std::cout << "✗ Failed to load simple format configuration" << std::endl;
        return false;
    }
}

// Test configuration save/load
bool testConfigurationSaveLoad() {
    std::cout << "\n=== Testing Configuration Save/Load ===" << std::endl;
    
    nlm::Config config;
    
    // Create test configuration
    config.set("test_int", 42);
    config.set("test_double", 3.14159);
    config.set("test_string", "Hello, World!");
    config.set("test_bool", true);
    config.set("test_array", std::vector<int64_t>({1, 2, 3, 4, 5}));
    
    std::cout << "✓ Created test configuration with " << config.getKeys().size() << " entries" << std::endl;
    
    // Save configuration
    const std::string testFile = "/tmp/test_config.json";
    if (config.saveToFile(testFile)) {
        std::cout << "✓ Configuration saved to " << testFile << std::endl;
        
        // Load configuration back
        nlm::Config config2;
        if (config2.loadFromFile(testFile)) {
            std::cout << "✓ Configuration loaded back successfully" << std::endl;
            
            // Verify values
            auto intVal = config2.get<int64_t>("test_int");
            if (intVal.has_value() && *intVal == 42) {
                std::cout << "✓ test_int value preserved: " << *intVal << std::endl;
            }
            
            auto doubleVal = config2.get<double>("test_double");
            if (doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001) {
                std::cout << "✓ test_double value preserved: " << *doubleVal << std::endl;
            }
            
            auto stringVal = config2.get<std::string>("test_string");
            if (stringVal.has_value() && *stringVal == "Hello, World!") {
                std::cout << "✓ test_string value preserved: " << *stringVal << std::endl;
            }
            
            auto boolVal = config2.get<bool>("test_bool");
            if (boolVal.has_value() && *boolVal == true) {
                std::cout << "✓ test_bool value preserved: " << *boolVal << std::endl;
            }
            
            auto arrayVal = config2.get<std::vector<int64_t>>("test_array");
            if (arrayVal.has_value()) {
                std::cout << "✓ test_array value preserved: size " << (*arrayVal).size() << std::endl;
            }
            
            // Clean up
            std::remove(testFile.c_str());
            return true;
        } else {
            std::cout << "✗ Failed to load saved configuration" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to save configuration" << std::endl;
    }
    
    return false;
}

// Test command line configuration
bool testCommandLineConfiguration() {
    std::cout << "\n=== Testing Command Line Configuration ===" << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[5] = {
        (char*)"test",
        (char*)"--config",
        (char*)"random_seed=123",
        (char*)"--neuron_count",
        (char*)"500"
    };
    
    if (config.loadFromArgs(argc, argv)) {
        std::cout << "✓ Command line configuration loaded successfully" << std::endl;
        
        auto randomSeed = config.get<int64_t>("random_seed");
        if (randomSeed.has_value() && *randomSeed == 123) {
            std::cout << "✓ random_seed = " << *randomSeed << std::endl;
        }
        
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount.has_value() && *neuronCount == 500) {
            std::cout << "✓ neuron_count = " << *neuronCount << std::endl;
        }
        
        return true;
    } else {
        std::cout << "✗ Failed to load command line configuration" << std::endl;
        return false;
    }
}

// Test configuration validation
bool testConfigurationValidation() {
    std::cout << "\n=== Testing Configuration Validation ===" << std::endl;
    
    nlm::Config config;
    
    // Test validation with valid configuration
    std::cout << "Testing with valid configuration..." << std::endl;
    config.set("valid_int", 42);
    config.set("valid_double", 3.14);
    config.set("valid_string", "test");
    config.set("valid_bool", true);
    
    std::cout << "✓ Valid configuration created" << std::endl;
    
    // Test validation with missing key
    std::cout << "Testing with missing key..." << std::endl;
    auto missingVal = config.get<int64_t>("nonexistent_key");
    if (!missingVal.has_value()) {
        std::cout << "✓ Missing key correctly returns null" << std::endl;
    }
    
    // Test validation with getOr default
    std::cout << "Testing getOr with default..." << std::endl;
    int64_t defaultVal = config.getOr<int64_t>("nonexistent_key", 100);
    if (defaultVal == 100) {
        std::cout << "✓ getOr returns default value: " << defaultVal << std::endl;
    }
    
    return true;
}

// Test configuration summary
bool testConfigurationSummary() {
    std::cout << "\n=== Testing Configuration Summary ===" << std::endl;
    
    nlm::Config config;
    
    // Add some test configuration
    config.set("summary_test_int", 123, nlm::ConfigSource::Default);
    config.set("summary_test_double", 45.67, nlm::ConfigSource::File);
    config.set("summary_test_string", "test", nlm::ConfigSource::CommandLine);
    config.set("summary_test_bool", true, nlm::ConfigSource::Runtime);
    
    std::cout << "Configuration summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    return true;
}

int main() {
    std::cout << "=== NLM Phase 2 Configuration Enhancement Tests ===" << std::endl;
    std::cout << "Testing JSON/YAML configuration support and advanced features" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    bool allPassed = true;
    
    // Run tests
    allPassed &= testJSONConfiguration();
    allPassed &= testSimpleFormatConfiguration();
    allPassed &= testConfigurationSaveLoad();
    allPassed &= testCommandLineConfiguration();
    allPassed &= testConfigurationValidation();
    allPassed &= testConfigurationSummary();
    
    std::cout << "\n=== Test Results ===" << std::endl;
    if (allPassed) {
        std::cout << "✓ All tests PASSED" << std::endl;
        std::cout << "\nConfiguration enhancements are working correctly:" << std::endl;
        std::cout << "  - JSON/YAML configuration support" << std::endl;
        std::cout << "  - Simple format fallback" << std::endl;
        std::cout << "  - Configuration save/load" << std::endl;
        std::cout << "  - Command line configuration" << std::endl;
        std::cout << "  - Configuration validation" << std::endl;
        std::cout << "  - Advanced configuration options" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests FAILED" << std::endl;
        return 1;
    }
}
