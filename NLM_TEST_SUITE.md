# NLM Test Suite

## Overview

This document describes the NLM (Neural Learning Machine) test suite, which validates the improved functionality and addresses the issues identified during the project analysis. The test suite focuses on the key improvements:

1. **Build System Improvements**: New executable options and integration
2. **Command System**: Enhanced CLI and configuration management
3. **Phase Demos**: Phase 3, 4, 5, and 6 demonstrations
4. **Integration**: Brain system connectivity and functionality
5. **Configuration Management**: Runtime configuration and validation

## Test Categories

### 1. Build System Tests
- **Executable Availability**: Verify all demo executables are built correctly
- **CMake Integration**: Test CMake configuration and build options
- **Installation Tests**: Validate installation targets and file structure

### 2. Command System Tests
- **Help System**: Context-sensitive help functionality
- **Argument Parsing**: Command-line argument parsing
- **Configuration Management**: Runtime configuration handling
- **Interactive Mode**: REPL functionality

### 3. Demo Tests
- **Phase 3 Demo**: World interaction and agent behavior
- **Phase 4 Demo**: Cognitive mechanism emergence
- **Phase 5 Demo**: Advanced experimental features
- **Phase 6 Demo**: Full integration verification

### 4. Integration Tests
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty integration
- **Prediction Systems**: Neural prediction and error signaling
- **Development System**: Neural development and maturation

### 5. Configuration Tests
- **File-based Configuration**: INI file parsing and loading
- **Runtime Configuration**: Dynamic configuration modification
- **Validation**: Configuration validation and error handling
- **Environment-based Configuration**: Environment-specific settings

## Test Structure

### 1. Unit Tests

#### Build System Tests
```cpp
// Test that all demo executables are properly linked
TEST(BuildSystemTests, DemoExecutablesExist) {
    // Verify executable existence
    EXPECT_TRUE(std::filesystem::exists("./nlm_phase3_demo"));
    EXPECT_TRUE(std::filesystem::exists("./nlm_phase4_demo"));
    EXPECT_TRUE(std::filesystem::exists("./nlm_phase5_demo"));
    EXPECT_TRUE(std::filesystem::exists("./nlm_phase6_demo"));
    EXPECT_TRUE(std::filesystem::exists("./nlm"));
}

// Test CMake configuration
TEST(BuildSystemTests, CMakeConfiguration) {
    // Test that CMake defines are correctly set
    EXPECT_EQ(CMAKE_BUILD_TYPE, "Release");
    EXPECT_TRUE(HAS_PYTHON_BINDINGS);
    EXPECT_TRUE(CPP_STANDARD_20);
}
```

#### Command System Tests
```cpp
// Test help system
TEST(CommandSystemTests, HelpFunctionality) {
    // Test help for different commands
    std::string helpOutput = executeCommand("./nlm --help");
    EXPECT_TRUE(helpOutput.find("NLM Neural Learning Machine") != std::string::npos);
    EXPECT_TRUE(helpOutput.find("usage:") != std::string::npos);
}

// Test argument parsing
TEST(CommandSystemTests, ArgumentParsing) {
    // Test different argument combinations
    std::string result1 = executeCommand("./nlm_phase3_demo 10 500");
    std::string result2 = executeCommand("./nlm_phase4_demo 100");
    
    // Verify results are as expected
    EXPECT_TRUE(result1.find("Simulation complete") != std::string::npos);
    EXPECT_TRUE(result2.find("Experiment complete") != std::string::npos);
}
```

#### Configuration Tests
```cpp
// Test file-based configuration
TEST(ConfigurationTests, FileBasedConfiguration) {
    // Create a temporary config file
    std::string configContent = R"(
[brain]
neuron_count = 500
region_count = 2

[simulation]
steps_per_episode = 1000
" )";
    
    std::string configFile = "test_config.ini";
    writeFile(configFile, configContent);
    
    // Test loading configuration
    Config config;
    bool loaded = config.loadFromFile(configFile);
    EXPECT_TRUE(loaded);
    EXPECT_EQ(config.get<int64_t>("neuron_count"), 500);
    EXPECT_EQ(config.get<int64_t>("region_count"), 2);
    
    cleanupFile(configFile);
}

// Test runtime configuration
TEST(ConfigurationTests, RuntimeConfiguration) {
    Config config;
    
    // Set configuration values
    config.set("brain.neuron_count", 1000);
    config.set("brain.connection_probability", 0.15f);
    
    // Verify values
    EXPECT_EQ(config.getOr<int64_t>("brain.neuron_count", 0), 1000);
    EXPECT_EQ(config.getOr<float>("brain.connection_probability", 0.0f), 0.15f);
}
```

### 2. Integration Tests

#### Brain System Integration Tests
```cpp
// Test memory systems integration
TEST(BrainSystemTests, MemoryIntegration) {
    // Create brain with configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Verify memory systems are integrated
    EXPECT_TRUE(brain->getWorkingMemory() != nullptr);
    EXPECT_TRUE(brain->getEpisodicMemory() != nullptr);
    EXPECT_TRUE(brain->getAssociativeMemory() != nullptr);
    
    // Test memory functionality
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
}

// Test neuromodulation integration
TEST(BrainSystemTests, NeuromodulationIntegration) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Verify neuromodulation systems are integrated
    EXPECT_TRUE(brain->getDopamine() != nullptr);
    EXPECT_TRUE(brain->getCuriosity() != nullptr);
    EXPECT_TRUE(brain->getNovelty() != nullptr);
    
    // Test neuromodulation functionality
    for (int step = 0; step < 50; ++step) {
        // Inject sensory activity to trigger neuromodulation
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->step(step, step * 0.001);
    }
    
    EXPECT_GT(brain->getAverageFiringRate(), 0.0f);
}
```

#### Prediction and Cognition Integration Tests
```cpp
// Test prediction system integration
TEST(PredictionCognitionTests, PredictionSystem) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Verify prediction system is integrated
    EXPECT_TRUE(brain->getPredictionSystem() != nullptr);
    
    // Test prediction functionality
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Verify prediction errors are being computed
    float predictionError = brain->getPredictionError();
    EXPECT_GE(predictionError, 0.0f);
}

// Test cognition system integration
TEST(PredictionCognitionTests, CognitionSystems) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Verify cognition systems are integrated
    EXPECT_TRUE(brain->getPlanner() != nullptr);
    EXPECT_TRUE(brain->getConceptFormation() != nullptr);
    EXPECT_TRUE(brain->getAttention() != nullptr);
    
    // Test cognition functionality
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    EXPECT_GT(brain->getAverageFiringRate(), 0.0f);
}
```

### 3. Demo Tests

#### Phase 3 Demo Tests
```cpp
// Test Phase 3 demo functionality
TEST(Phase3DemoTests, WorldInteraction) {
    std::string command = "./nlm_phase3_demo 5 200";
    std::string output = executeCommand(command);
    
    // Verify demo runs successfully
    EXPECT_TRUE(output.find("Phase 3 demonstration complete") != std::string::npos);
    EXPECT_TRUE(output.find("Learning") != std::string::npos);
}

// Test Phase 3 demo with different parameters
TEST(Phase3DemoTests, ParameterVariants) {
    // Test with different neuron counts
    std::vector<std::pair<int, int>> testCases = {
        {5, 100},
        {10, 200},
        {20, 500}
    };
    
    for (const auto& testCase : testCases) {
        std::string command = "./nlm_phase3_demo " + 
                              std::to_string(testCase.first) + " " +
                              std::to_string(testCase.second);
        std::string output = executeCommand(command);
        EXPECT_TRUE(output.find("demonstration complete") != std::string::npos);
    }
}
```

#### Phase 4 Demo Tests
```cpp
// Test Phase 4 demo functionality
TEST(Phase4DemoTests, CognitiveEmergence) {
    std::string command = "./nlm_phase4_demo 50";
    std::string output = executeCommand(command);
    
    // Verify demo runs successfully
    EXPECT_TRUE(output.find("Phase 4 experiments complete") != std::string::npos);
    EXPECT_TRUE(output.find("Prediction") != std::string::npos);
}

// Test Phase 4 demo with different trial counts
TEST(Phase4DemoTests, TrialVariants) {
    std::vector<int> trialCounts = {10, 25, 50};
    
    for (int trials : trialCounts) {
        std::string command = "./nlm_phase4_demo " + std::to_string(trials);
        std::string output = executeCommand(command);
        EXPECT_TRUE(output.find("experiments complete") != std::string::npos);
    }
}
```

#### Phase 5 Demo Tests
```cpp
// Test Phase 5 demo functionality
TEST(Phase5DemoTests, AdvancedFeatures) {
    std::string command = "./nlm_phase5_demo --demo-type 1 --neurons 2000";
    std::string output = executeCommand(command);
    
    // Verify demo runs successfully
    EXPECT_TRUE(output.find("Phase 5 Demo Complete") != std::string::npos);
    EXPECT_TRUE(output.find("Lifelong Learning") != std::string::npos);
}

// Test Phase 5 demo with different demo types
TEST(Phase5DemoTests, DemoTypes) {
    std::vector<std::pair<int, std::string>> demoTypes = {
        {0, "Basic Demo"},
        {1, "Lifelong Learning"},
        {2, "Scaling Demo"},
        {3, "Social Learning"}
    };
    
    for (const auto& demoType : demoTypes) {
        std::string command = "./nlm_phase5_demo --demo-type " + 
                              std::to_string(demoType.first) + " --neurons 1000";
        std::string output = executeCommand(command);
        EXPECT_TRUE(output.find("Demo Complete") != std::string::npos);
        EXPECT_TRUE(output.find(demoType.second) != std::string::npos);
    }
}
```

#### Phase 6 Demo Tests
```cpp
// Test Phase 6 demo functionality
TEST(Phase6DemoTests, IntegrationTest) {
    std::string command = "./nlm_phase6_demo";
    std::string output = executeCommand(command);
    
    // Verify demo runs successfully
    EXPECT_TRUE(output.find("INTEGRATION STATUS") != std::string::npos);
    EXPECT_TRUE(output.find("Phase 6 Complete") != std::string::npos);
}

// Test Phase 6 demo with custom configuration
TEST(Phase6DemoTests, CustomConfiguration) {
    std::string command = "./nlm_phase6_demo --config custom_config.ini";
    std::string output = executeCommand(command);
    
    // Should handle missing config file gracefully
    EXPECT_TRUE(output.find("ERROR") != std::string::npos || 
                output.find("using default configuration") != std::string::npos);
}
```

### 4. Advanced Feature Tests

#### Performance Monitoring Tests
```cpp
// Test performance monitoring
TEST(AdvancedFeaturesTests, PerformanceMonitoring) {
    PerformanceMonitor monitor;
    monitor.start();
    
    // Run simulation
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    monitor.stop();
    
    // Verify metrics are collected
    PerformanceMetrics metrics = monitor.getCurrentMetrics();
    EXPECT_GT(metrics.avgStepTimeMs, 0.0);
    EXPECT_GE(metrics.realTimeFactor, 0.0);
}

// Test hardware detection
TEST(AdvancedFeaturesTests, HardwareDetection) {
    PerformanceMonitor monitor;
    HardwareInfo info = monitor.getHardwareInfo();
    
    EXPECT_FALSE(info.cpuModel.empty());
    EXPECT_GT(info.cpuCores, 0);
    EXPECT_GE(info.systemMemoryGB, 1);
}
```

#### Configuration Template Tests
```cpp
// Test configuration template management
TEST(AdvancedFeaturesTests, ConfigurationTemplates) {
    ConfigurationTemplateManager manager;
    
    // Create a test template
    Config baseConfig;
    baseConfig.set("brain.neuron_count", 500);
    baseConfig.set("brain.connection_probability", 0.1f);
    
    std::string templateName = manager.createTemplate(
        "Test Template",
        "A test configuration template",
        baseConfig,
        {"test", "development"}
    );
    
    // Load the template
    Config loadedConfig;
    bool loaded = manager.loadTemplate(templateName, loadedConfig);
    EXPECT_TRUE(loaded);
    EXPECT_EQ(loadedConfig.getOr<int64_t>("brain.neuron_count", 0), 500);
    
    // Clean up
    manager.deleteTemplate(templateName);
}
```

## Test Execution

### 1. Running Tests

#### Command Line
```bash
# Run all tests
make test

# Run specific test category
make test-brain
make test-command
make test-demo
make test-configuration

# Run tests with verbose output
make test-verbose

# Run tests with custom options
ctest -R "test_name" -V
```

#### Build System
```bash
# Build with test support
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j4

# Run tests
./nlm_test
```

### 2. Test Output

Test output should include:

1. **Test Results Summary**: Pass/fail counts and durations
2. **Detailed Test Information**: Test names, descriptions, and execution details
3. **Error Messages**: Detailed error messages for failed tests
4. **Performance Metrics**: Execution time and resource usage
5. **Configuration Information**: System configuration used for tests

### 3. Test Logging

Test logging should include:

1. **Test Execution Log**: When tests start and finish
2. **Performance Log**: Test performance metrics
3. **Error Log**: Detailed error information for failures
4. **Configuration Log**: Test configuration and environment

## Test Results Reporting

### 1. Test Report Format

```json
{
    "test_summary": {
        "total_tests": 100,
        "passed_tests": 95,
        "failed_tests": 5,
        "skipped_tests": 0,
        "total_time_seconds": 120.5
    },
    "test_categories": {
        "build_system": {
            "total": 20,
            "passed": 20,
            "failed": 0,
            "time": 15.2
        },
        "command_system": {
            "total": 15,
            "passed": 14,
            "failed": 1,
            "time": 10.8
        },
        "integration": {
            "total": 25,
            "passed": 25,
            "failed": 0,
            "time": 35.1
        },
        "demo": {
            "total": 30,
            "passed": 30,
            "failed": 0,
            "time": 40.0
        },
        "configuration": {
            "total": 10,
            "passed": 10,
            "failed": 0,
            "time": 5.4
        },
        "advanced_features": {
            "total": 10,
            "passed": 9,
            "failed": 1,
            "time": 8.9
        }
    },
    "failed_tests": [
        {
            "test_name": "CommandSystemTests.ArgumentParsing",
            "error_message": "Expected argument not found",
            "stack_trace": "..."
        }
    ],
    "system_info": {
        "compiler": "GCC 11.2",
        "build_type": "Release",
        "cpu_cores": 8,
        "memory_gb": 16
    }
}
```

### 2. Test Result Interpretation

**Passing Tests**: All test requirements met
**Failing Tests**: Test requirements not met, investigate cause
**Skipped Tests**: Test not applicable in current environment
**Error Tests**: Test encountered unexpected error, investigate

### 3. Test Quality Metrics

1. **Test Coverage**: Percentage of code covered by tests
2. **Test Accuracy**: Correctness of test assertions
3. **Test Performance**: Execution time of tests
4. **Test Reliability**: Consistency of test results

## Continuous Integration

### 1. CI/CD Pipeline

The test suite should be integrated into a CI/CD pipeline:

1. **Build Stage**: Compile the project with tests
2. **Test Stage**: Run the test suite
3. **Coverage Stage**: Measure test coverage
4. **Reporting Stage**: Generate test reports
5. **Deployment Stage**: Deploy if tests pass

### 2. GitHub Actions Example

```yaml
name: NLM Tests

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Environment
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake python3
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
    
    - name: Build
      run: |
        cd build
        make -j4
    
    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Upload Test Results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results
        path: build/test-results.xml
```

### 3. Jenkins Pipeline Example

```groovy
pipeline {
    agent any
    
    stages {
        stage('Build') {
            steps {
                sh 'mkdir -p build'
                sh 'cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON'
                sh 'cd build && make -j4'
            }
        }
        
        stage('Test') {
            steps {
                sh 'cd build && ctest --output-on-failure'
            }
        }
        
        stage('Coverage') {
            steps {
                sh 'cd build && make gcov'
                sh 'cd build && gcovr --exclude "*.cpp" --exclude "*/tests/*" --report-xml coverage.xml'
            }
        }
    }
    
    post {
        always {
            archiveArtifacts artifacts: 'build/test-results.xml, build/coverage.xml', allowEmptyArchive: true
        }
    }
}
```

## Maintenance and Updates

### 1. Test Maintenance

- **Regular Updates**: Update tests when functionality changes
- **Test Coverage**: Ensure tests cover all critical functionality
- **Performance Testing**: Regularly test performance characteristics
- **Compatibility Testing**: Test with different compilers and platforms

### 2. Test Documentation

- **Test Documentation**: Document test purposes and expected behavior
- **Test Categories**: Organize tests into logical categories
- **Test Dependencies**: Document test dependencies and requirements
- **Test Examples**: Provide examples of test execution and interpretation

### 3. Test Improvement

- **Test Efficiency**: Optimize test performance and reduce execution time
- **Test Reliability**: Improve test reliability and reduce flaky tests
- **Test Scalability**: Ensure tests scale with system size
- **Test Maintainability**: Keep tests maintainable and readable

## Conclusion

The NLM test suite provides comprehensive validation of the improved functionality:

1. **Build System**: Verifies correct executable construction and linking
2. **Command System**: Validates CLI and configuration management
3. **Integration**: Confirms brain system connectivity and functionality
4. **Demos**: Ensures all demo executables work correctly
5. **Configuration**: Tests configuration management and validation
6. **Advanced Features**: Validates performance monitoring and advanced tools

The test suite ensures:

- **Quality**: All functionality meets quality standards
- **Reliability**: System behaves correctly under various conditions
- **Performance**: System performs within expected parameters
- **Compatibility**: System works across different environments
- **Maintainability**: System is easy to maintain and extend

Regular test execution and maintenance ensure the NLM system remains reliable and provides the enhanced user experience intended by the improvements.