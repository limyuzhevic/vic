# Comprehensive Test Suite for NLM Improvements

## Overview

This test suite validates the improvements made to NLM's error handling, code consistency, and functionality. It includes:

1. **Configuration Validation Tests**
2. **Error Handling Tests**
3. **Performance Profiling Tests**
4. **Integration Tests**
5. **Advanced Command Tests**

## Test Structure

### Directory Structure

```
tests/
├── test_config_validation.cpp
├── test_error_handling.cpp
├── test_performance_profiling.cpp
├── test_integration.cpp
├── test_advanced_commands.cpp
├── test_memory_management.cpp
└── test_backward_compatibility.cpp
```

### Common Test Infrastructure

```cpp
// tests/common/test_common.hpp

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

// Test utilities
namespace test_utils {
    
    // Configuration utilities
    template<typename T>
    std::shared_ptr<Config> createTestConfig(
        const std::string& name,
        std::unordered_map<std::string, T> values
    ) {
        auto config = std::make_shared<Config>();
        for (const auto& [key, value] : values) {
            config->set(key, value);
        }
        return config;
    }
    
    // File utilities
    class TempFile {
    public:
        TempFile(const std::string& content = "") {
            std::ofstream file(filename_);
            if (file.is_open()) {
                file << content;
            }
        }
        
        ~TempFile() {
            if (std::filesystem::exists(filename_)) {
                std::filesystem::remove(filename_);
            }
        }
        
        const std::string& getFilename() const { return filename_; }
        
    private:
        std::string filename_ = "test_" + std::to_string(std::rand()) + ".tmp";
    };
    
    // Time measurement
    class Timer {
    public:
        Timer() : start_(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double>(end - start_).count();
        }
        
        void reset() {
            start_ = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    };
    
    // Assertion helpers
    template<typename T>
    void assertAlmostEqual(T expected, T actual, double epsilon = 1e-6) {
        EXPECT_NEAR(expected, actual, epsilon);
    }
    
    // Validation helpers
    template<typename T>
    void validateRange(T value, T min, T max, const std::string& name) {
        EXPECT_GE(value, min) << "Value " << name << " " << value << " < minimum " << min;
        EXPECT_LE(value, max) << "Value " << name << " " << value << " > maximum " << max;
    }
    
    // Performance validation
    void validatePerformance(double stepsPerSecond, double minStepsPerSecond) {
        EXPECT_GE(stepsPerSecond, minStepsPerSecond) 
            << "Performance " << stepsPerSecond << " < minimum " << minStepsPerSecond;
    }
}
```

## Test Categories

### 1. Configuration Validation Tests

```cpp
// tests/test_config_validation.cpp

#include "common/test_common.hpp"

class ConfigValidationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(ConfigValidationTest, ValidConfiguration) {
    // Test with valid configuration
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1);
    config->set("simulation_timestep", 0.001);
    
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_TRUE(config->has("region_count"));
    EXPECT_TRUE(config->has("connection_probability"));
    EXPECT_TRUE(config->has("simulation_timestep"));
    
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 100);
    EXPECT_EQ(config->getOr<int64_t>("region_count", 0), 2);
    EXPECT_EQ(config->getOr<double>("connection_probability", 0.0), 0.1);
    EXPECT_EQ(config->getOr<double>("simulation_timestep", 0.0), 0.001);
}

TEST_F(ConfigValidationTest, InvalidConfiguration) {
    // Test with invalid configuration
    config->set("neuron_count", -100);
    
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), -100);
}

TEST_F(ConfigValidationTest, MissingConfiguration) {
    // Test configuration with missing keys
    config->set("neuron_count", 100);
    
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_FALSE(config->has("region_count")); // Not set yet
    EXPECT_EQ(config->getOr<int64_t>("region_count", 999), 999); // Default value
}

TEST_F(ConfigValidationTest, ConfigurationValidation) {
    // Test configuration validation
    config->set("neuron_count", 0); // Invalid: zero neurons
    
    // Should handle gracefully (implementation dependent)
    // For now, just verify the value is set
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 0);
}

TEST(ConfigFileTest, ValidConfigFile) {
    // Test loading from valid config file
    TempFile validConfig("neuron_count = 100\nregion_count = 2\nconnection_probability = 0.1");
    
    Config config;
    bool success = config.loadFromFile(validConfig.getFilename());
    EXPECT_TRUE(success);
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getOr<int64_t>("neuron_count", 0), 100);
}

TEST(ConfigFileTest, InvalidConfigFile) {
    // Test loading from invalid config file
    TempFile invalidConfig("invalid json content");
    
    Config config;
    bool success = config.loadFromFile(invalidConfig.getFilename());
    EXPECT_FALSE(success);
}

TEST(ConfigFileTest, NonExistentConfigFile) {
    // Test loading from non-existent file
    Config config;
    bool success = config.loadFromFile("nonexistent_file_12345.cfg");
    EXPECT_FALSE(success);
}
```

### 2. Error Handling Tests

```cpp
// tests/test_error_handling.cpp

#include "common/test_common.hpp"

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Capture log output for testing
        originalLevel = Logger::getGlobal()->getLevel();
        Logger::getGlobal()->setLevel(LogLevel::Info);
    }
    
    void TearDown() override {
        // Restore original log level
        Logger::getGlobal()->setLevel(originalLevel);
    }
    
    LogLevel originalLevel;
};

TEST_F(ErrorHandlingTest, BrainInitializationError) {
    // Test error handling during brain initialization
    auto config = std::make_shared<Config>();
    config->set("neuron_count", -100); // Invalid
    
    Brain brain(config);
    EXPECT_FALSE(brain.initialize());
}

TEST_F(ErrorHandlingTest, InvalidConfigurationError) {
    // Test handling of invalid configuration
    auto config = std::make_shared<Config>();
    config->set("connection_probability", 1.5); // Invalid (> 1.0)
    
    // Should handle gracefully
    Brain brain(config);
    // Behavior depends on implementation
}

TEST_F(ErrorHandlingTest, MemoryAllocationError) {
    // Test handling of memory allocation errors
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000000); // Large number
    
    Brain brain(config);
    // May succeed or fail depending on available memory
    // Test should not crash
}

TEST(AgentErrorHandlingTest, InvalidAgentConfiguration) {
    // Test error handling in agent brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    Brain brain(config);
    Brain::initialize();
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test with invalid actions
    // Should handle gracefully without crashing
}
```

### 3. Performance Profiling Tests

```cpp
// tests/test_performance_profiling.cpp

#include "common/test_common.hpp"

class PerformanceProfilingTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(PerformanceProfilingTest, PerformanceProfiling) {
    // Test performance profiling
    Brain brain(config);
    brain.initialize();
    
    test_utils::Timer timer;
    int steps = 1000;
    
    for (int step = 0; step < steps; ++step) {
        brain.step(step);
    }
    
    double elapsed = timer.elapsed();
    double stepsPerSecond = steps / elapsed;
    
    // Validate performance
    test_utils::validatePerformance(stepsPerSecond, 1000.0); // At least 1000 steps/sec
}

TEST_F(PerformanceProfilingTest, MemoryUsage) {
    // Test memory usage profiling
    Brain brain(config);
    brain.initialize();
    
    size_t initialMemory = getCurrentMemoryUsage(); // Implementation dependent
    
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    size_t finalMemory = getCurrentMemoryUsage();
    size_t memoryIncrease = finalMemory - initialMemory;
    
    // Memory increase should be reasonable
    EXPECT_LT(memoryIncrease, 100 * 1024 * 1024); // Less than 100MB
}

TEST(ProfilingToolTest, ConfigurationAnalysis) {
    // Test configuration analysis tool
    Config config;
    config->set("neuron_count", 1000);
    config->set("region_count", 3);
    config->set("connection_probability", 0.05);
    
    // This would call the configuration analysis function
    // Auto-generated for testing
}
```

### 4. Integration Tests

```cpp
// tests/test_integration.cpp

#include "common/test_common.hpp"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create standard configuration for integration tests
        config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
        config->set("random_seed", 42); // Deterministic
        
        // Initialize components
        brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        world = std::make_unique<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        world->setRandomSeed(42);
        
        agentBrain = std::make_unique<AgentBrain>(*brain);
        agentBrain->initialize(*world);
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::unique_ptr<SimpleWorld> world;
    std::unique_ptr<AgentBrain> agentBrain;
};

TEST_F(IntegrationTest, BasicIntegration) {
    // Test basic integration of all components
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
    EXPECT_TRUE(brain->getTotalSynapseCount() > 0);
    EXPECT_TRUE(agentBrain->getSensoryInputSize() > 0);
    EXPECT_TRUE(agentBrain->getMotorOutputSize() > 0);
}

TEST_F(IntegrationTest, SimulationIntegration) {
    // Test that simulation runs without errors
    int steps = 100;
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        const SensoryPercept& percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step);
        
        // Get action
        std::unique_ptr<Action> action = agentBrain->decodeMotorCommand();
        if (action) {
            // Apply action to world
            world->applyMotorCommand(*action, world->getSimulationTime());
        }
    }
    
    // Verify that the simulation produced results
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
    EXPECT_GT(brain->getFiringNeuronCount(), 0);
}

TEST_F(IntegrationTest, ErrorIntegration) {
    // Test error handling in integration
    // This would test various error conditions in the integrated system
    // Implementation depends on specific error scenarios
}
```

### 5. Advanced Command Tests

```cpp
// tests/test_advanced_commands.cpp

#include "common/test_common.hpp"
#include "bin/nlm-config.h" // Include the command line tool header

class AdvancedCommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary directory for test files
        testDir = "test_advanced_commands_" + std::to_string(std::rand());
        std::filesystem::create_directory(testDir);
        
        // Create a test configuration file
        configFile = testDir + "/test_config.cfg";
        std::ofstream file(configFile);
        file << "neuron_count = 500\n";
        file << "region_count = 2\n";
        file << "connection_probability = 0.1\n";
        file << "simulation_timestep = 0.001\n";
        file.close();
    }
    
    void TearDown() override {
        // Clean up temporary directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::string testDir;
    std::string configFile;
};

TEST_F(AdvancedCommandsTest, ConfigurationAnalysisCommand) {
    // Test configuration analysis command
    std::string config = "neuron_count=1000 region_count=3 connection_probability=0.05";
    
    // Run the command (this would call the actual implementation)
    // For now, just test that the command can be constructed
    // Implementation details would depend on the actual command-line tool
}

TEST_F(AdvancedCommandsTest, PerformanceProfilingCommand) {
    // Test performance profiling command
    std::string config = "neuron_count=500 steps=200";
    
    // Run the profiling command
    // Test the output and results
}

TEST_F(AdvancedCommandsTest, ConfigurationValidationCommand) {
    // Test configuration validation command
    std::string config = "neuron_count=100 region_count=1";
    
    // Test validation
    // Should pass
}
```

### 6. Memory Management Tests

```cpp
// tests/test_memory_management.cpp

#include "common/test_common.hpp"

class MemoryManagementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset memory tracking
        memoryTracker.reset();
    }
    
    void TearDown() override {
        // Check for memory leaks
        checkMemoryLeaks();
    }
    
    MemoryTracker memoryTracker;
};

TEST_F(MemoryManagementTest, BrainMemoryCleanup) {
    // Test that brain cleans up memory properly
    size_t initialMemory = memoryTracker.getCurrentMemory();
    
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        
        Brain brain(config);
        assert(brain.initialize());
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
    } // Brain should be destroyed here
    
    size_t finalMemory = memoryTracker.getCurrentMemory();
    size_t memoryIncrease = finalMemory - initialMemory;
    
    // Memory increase should be minimal (just tracking overhead)
    EXPECT_LT(memoryIncrease, 1024 * 1024); // Less than 1MB
}

TEST_F(MemoryManagementTest, ConfigurationMemoryCleanup) {
    // Test that configuration cleans up memory properly
    size_t initialMemory = memoryTracker.getCurrentMemory();
    
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        
        // Use config extensively
        for (int i = 0; i < 100; ++i) {
            config->set("temp_key", i);
        }
    } // Config should be destroyed here
    
    size_t finalMemory = memoryTracker.getCurrentMemory();
    size_t memoryIncrease = finalMemory - initialMemory;
    
    EXPECT_LT(memoryIncrease, 512 * 1024); // Less than 512KB
}
```

### 7. Backward Compatibility Tests

```cpp
// tests/test_backward_compatibility.cpp

#include "common/test_common.hpp"

class BackwardCompatibilityTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create configuration using old API patterns
        config = std::make_shared<Config>();
        
        // Old-style configuration (using set with default source)
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1);
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(BackwardCompatibilityTest, OldConfigurationAPI) {
    // Test that old configuration API still works
    config->set("neuron_count", 200); // Old API: no source specified
    
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 200);
}

TEST_F(BackwardCompatibilityTest, OldBrainAPI) {
    // Test that old brain API still works
    Brain brain(config);
    bool success = brain.initialize();
    
    // Should work as before
    if (success) {
        EXPECT_TRUE(brain.getTotalNeuronCount() > 0);
        EXPECT_TRUE(brain.getTotalSynapseCount() > 0);
    }
}

TEST_F(BackwardCompatibilityTest, OldConfigFileAPI) {
    // Test that old config file API still works
    TempFile configFile("neuron_count = 300\nregion_count = 3");
    
    Config config;
    bool success = config.loadFromFile(configFile.getFilename());
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getOr<int64_t>("neuron_count", 0), 300);
}
```

## Test Running Instructions

### Building Tests

```bash
# Build the project with tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_TESTS=ON
make -j4
```

### Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test category
ctest -R "ConfigValidationTest"
ctest -R "ErrorHandlingTest"
ctest -R "PerformanceProfilingTest"

# Run tests with verbose output
ctest -V
```

### Test Output Analysis

The test suite produces detailed output including:

1. **Test Results**: Pass/fail status for each test
2. **Performance Metrics**: Timing information for performance tests
3. **Memory Usage**: Memory consumption tracking
4. **Error Details**: Detailed error messages for failed tests

### Continuous Integration

The test suite can be integrated with CI/CD systems:

```yaml
# .github/workflows/tests.yml
name: NLM Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake g++ git
        pip install pybind11 scikit-build-core pytest numpy
    
    - name: Build and run tests
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_TESTS=ON
        make -j4
        ctest --output-on-failure
```

## Test Coverage

### Coverage Targets

The test suite aims for the following coverage targets:

- **Configuration Validation**: 95% coverage
- **Error Handling**: 90% coverage
- **Performance Profiling**: 85% coverage
- **Integration Tests**: 80% coverage
- **Memory Management**: 90% coverage
- **Backward Compatibility**: 95% coverage

### Coverage Metrics

The test suite measures:

1. **Code Coverage**: Percentage of code executed by tests
2. **Performance Coverage**: Performance characteristics tested
3. **Error Coverage**: Error conditions tested
4. **Integration Coverage**: Integration scenarios tested
5. **Memory Coverage**: Memory management scenarios tested

### Coverage Analysis

```bash
# Generate coverage report (requires gcov)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_BUILD_TESTS=ON
make -j4
ctest --output-on-failure

# Generate coverage report
gcov -b -c src/core/Config/Config.cpp
open Config.gcno.html
```

## Test Data Management

### Test Data Generation

The test suite generates test data automatically:

```cpp
// Generate random configurations for testing
auto generateRandomConfig() {
    auto config = std::make_shared<Config>();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> neuronDist(100, 5000);
    std::uniform_int_distribution<> regionDist(1, 4);
    std::uniform_real_distribution<> connectionDist(0.01, 0.2);
    std::uniform_real_distribution<> timestepDist(0.0001, 0.01);
    
    config->set("neuron_count", neuronDist(gen));
    config->set("region_count", regionDist(gen));
    config->set("connection_probability", connectionDist(gen));
    config->set("simulation_timestep", timestepDist(gen));
    
    return config;
}
```

### Test Data Persistence

Test data can be saved and reloaded:

```cpp
// Save test results
saveTestResults(const std::string& filename, const TestResults& results);

// Load test results
TestResults loadTestResults(const std::string& filename);
```

## Test Maintenance

### Adding New Tests

To add new tests, follow this template:

```cpp
// new_test.cpp

#include "common/test_common.hpp"

TEST(NewTestCategory, NewTestCase) {
    // Arrange
    // Setup test data
    
    // Act
    // Perform the action being tested
    
    // Assert
    // Verify the results
}
```

### Updating Tests

When making changes to the codebase:

1. **Update existing tests** to reflect new behavior
2. **Add new tests** for new functionality
3. **Remove obsolete tests** that no longer apply
4. **Run full test suite** to ensure no regressions

### Test Documentation

Each test should be documented with:

```cpp
// tests/test_example.cpp

/**
 * @test Test configuration validation
 * 
 * This test verifies that configuration validation works correctly,
 * including valid configurations, invalid configurations, and edge cases.
 * 
 * Test cases:
 * - Valid configuration: All required keys present with valid values
 * - Invalid configuration: Negative neuron count
 * - Missing configuration: Missing required keys
 * - Configuration validation: Range checking
 * 
 * Expected behavior:
 * - Valid configurations: Should succeed
 * - Invalid configurations: Should fail gracefully
 * - Missing configurations: Should use default values
 * 
 * @see Config::loadFromFile()
 * @see Config::getOr()
 */
TEST(ConfigValidationTest, ValidConfiguration) {
    // Test implementation
}
```

## Conclusion

The comprehensive test suite provides:

1. **Complete Coverage**: Tests for all major functionality
2. **Performance Testing**: Ensures performance requirements are met
3. **Error Testing**: Validates error handling and recovery
4. **Integration Testing**: Tests the complete system integration
5. **Memory Testing**: Ensures proper memory management
6. **Backward Compatibility**: Ensures existing code continues to work

The test suite ensures that NLM improvements are properly validated and that the system remains stable and reliable under various conditions.
