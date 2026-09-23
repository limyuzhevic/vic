# NLM Testing Guide

## Overview

The NLM (Neural Learning Machine) testing framework provides comprehensive tools for testing neural simulation systems. It includes unit tests for individual components, integration tests for system behavior, and performance benchmarks.

### Testing Philosophy

NLM's testing approach follows several key principles:

1. **Deterministic Testing**: All tests use fixed random seeds for reproducible results
2. **Component Testing**: Individual components are tested in isolation
3. **Integration Testing**: System components are tested together
4. **Performance Testing**: Performance is measured and optimized
5. **Regression Testing**: Changes don't break existing functionality
6. **Configuration Testing**: Different configurations are tested

## Testing Architecture

### Test Components

NLM's testing framework consists of several key components:

1. **Unit Tests**: Test individual classes and functions
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete system behavior
4. **Performance Tests**: Measure and optimize performance
5. **Configuration Tests**: Test different configuration options
6. **Regression Tests**: Prevent functionality regression

### Test Structure

#### Test Files

The NLM testing framework is organized as follows:

```
tests/
├── test_types.cpp          # Type system tests
├── test_random.cpp         # Random number generator tests
├── test_config.cpp         # Configuration system tests
├── test_clock.cpp          # Simulation clock tests
├── test_neuron.cpp         # Neuron dynamics tests
├── test_synapse.cpp        # Synapse and plasticity tests
├── test_stdp.cpp           # STDP mechanism tests
├── test_brain.cpp          # Brain integration tests
└── test_main.cpp           # Main test runner
```

#### Test Organization

- **Type System**: Tests for NeuronId, SynapseId, RegionId, etc.
- **Random Number Generator**: Tests for deterministic random number generation
- **Configuration System**: Tests for JSON configuration, command line parsing
- **Simulation Clock**: Tests for time management and simulation control
- **Neuron Dynamics**: Tests for LIF neuron implementation
- **Synapse and Plasticity**: Tests for synaptic connections and learning
- **STDP**: Tests for spike-timing-dependent plasticity
- **Brain Integration**: Tests for complete brain functionality

### Test Framework

#### Test Classes

```cpp
// Base test class
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run() = 0;
    virtual bool isEnabled() const { return true; }
};

// Test result structure
struct TestResult {
    std::string name;
    std::string description;
    bool passed;
    std::string message;
    double execution_time;
};
```

#### Test Runner

```cpp
class TestRunner {
public:
    TestRunner();
    void registerTest(std::unique_ptr<TestCase> test);
    bool runTest(const std::string& testName);
    bool runAllTests();
    std::vector<TestResult> getResults() const;
    void clearResults();
    void setTestFilter(const std::string& filter);
    void enableAllTests();
    void disableAllTests();
    void enableTest(const std::string& testName);
    void disableTest(const std::string& testName);
    bool isTestEnabled(const std::string& testName) const;
};
```

## Running Tests

### Building Tests

#### Standard Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (include tests)
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=ON

# Build all targets
make -j$(nproc)
```

#### Debug Build

```bash
# For development and debugging
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
make -j$(nproc)
```

### Running Tests

#### Basic Test Execution

```bash
# Run all tests
ctest --output-on-failure

# Run tests with verbose output
ctest -V

# Run specific test
ctest -R test_neuron

# Run tests matching pattern
ctest -E "neuron|synapse"
```

#### Test Targets

```bash
# Run specific test executable
./nlm_test

# Run test with specific options
./nlm_test --test basic_connectivity,plasticity
./nlm_test --list-tests
./nlm_test --help
```

#### Test Output Formats

```bash
# Human-readable output
ctest --output-on-failure

# XML output for CI/CD
ctest --output-on-failure --output-junit test-results.xml

# JSON output (if supported)
ctest --output-json test-results.json
```

### Test Command Line Options

#### Common Options

| Option | Description |
|--------|-------------|
| `--test=name` | Run specific test by name |
| `--test=name1,name2` | Run multiple tests by comma-separated list |
| `--list-tests` | List all available tests and exit |
| `--help` | Show help message |
| `--version` | Show version information |

#### Test Selection Examples

```bash
# Run all tests
./nlm_test

# Run specific test
./nlm_test --test basic_connectivity

# Run multiple tests
./nlm_test --test basic_connectivity,plasticity,stdp_verification

# Run no tests (quick check)
./nlm_test --test none

# List all tests
./nlm_test --list-tests
```

## Unit Tests

### Test Types

#### Type System Tests

```cpp
// Test type definitions and operations
class TestTypeSystem : public TestCase {
public:
    std::string getName() const override { return "type_system"; }
    std::string getDescription() const override { 
        return "Test type system (NeuronId, SynapseId, etc.)"; 
    }
    
    bool run() override {
        // Test NeuronId operations
        NeuronId id1;
        NeuronId id2(42);
        
        EXPECT_EQ(id1.value, 0);
        EXPECT_EQ(id2.value, 42);
        EXPECT_TRUE(id1 != id2);
        EXPECT_TRUE(id1 == NeuronId(0));
        
        // Test other type operations...
        return true;
    }
};
```

#### Random Number Generator Tests

```cpp
// Test deterministic random number generation
class TestRandomGenerator : public TestCase {
public:
    std::string getName() const override { return "random_generator"; }
    std::string getDescription() const override { 
        return "Test random number generator with seeds"; 
    }
    
    bool run() override {
        Random rng1(42);
        Random rng2(42);
        
        // Same seed should produce same sequence
        EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
        EXPECT_EQ(rng1.gaussian(0.0, 1.0), rng2.gaussian(0.0, 1.0));
        
        // Different seeds should produce different sequences
        Random rng3(123);
        EXPECT_NE(rng1.uniformInt(0, 100), rng3.uniformInt(0, 100));
        
        return true;
    }
};
```

#### Configuration Tests

```cpp
// Test configuration system
class TestConfiguration : public TestCase {
public:
    std::string getName() const override { return "configuration"; }
    std::string getDescription() const override { 
        return "Test configuration system (JSON, command line)"; 
    }
    
    bool run() override {
        auto config = std::make_shared<Config>();
        
        // Test basic operations
        config->set("test.key", "value");
        EXPECT_TRUE(config->has("test.key"));
        EXPECT_EQ(config->getOr("test.key", "default"), "value");
        
        // Test file operations
        config->saveToFile("test_config.json");
        auto config2 = std::make_shared<Config>();
        EXPECT_TRUE(config2->loadFromFile("test_config.json"));
        EXPECT_EQ(config2->getOr("test.key", "default"), "value");
        
        // Clean up
        std::remove("test_config.json");
        
        return true;
    }
};
```

#### Simulation Clock Tests

```cpp
// Test simulation clock
class TestSimulationClock : public TestCase {
public:
    std::string getName() const override { return "simulation_clock"; }
    std::string getDescription() const override { 
        return "Test simulation clock and time management"; 
    }
    
    bool run() override {
        SimulationClock clock(0.001);
        
        EXPECT_DOUBLE_EQ(clock.getTimestep(), 0.001);
        EXPECT_EQ(clock.getCurrentStep(), 0);
        EXPECT_DOUBLE_EQ(clock.getCurrentTime(), 0.0);
        
        // Test stepping
        clock.step();
        EXPECT_EQ(clock.getCurrentStep(), 1);
        EXPECT_DOUBLE_EQ(clock.getCurrentTime(), 0.001);
        
        clock.step(100);
        EXPECT_EQ(clock.getCurrentStep(), 101);
        EXPECT_DOUBLE_EQ(clock.getCurrentTime(), 0.101);
        
        return true;
    }
};
```

#### Neuron Tests

```cpp
// Test neuron dynamics
class TestNeuron : public TestCase {
public:
    std::string getName() const override { return "neuron"; }
    std::string getDescription() const override { 
        return "Test Leaky Integrate-and-Fire neuron dynamics"; 
    }
    
    bool run() override {
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        
        // Create brain with minimal neurons
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get neurons
        auto region = brain->getRegion(RegionId(0));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        Neuron* neuron = neurons[0].get();
        
        // Test resting state
        EXPECT_DOUBLE_EQ(neuron->getMembranePotential(), -70.0);
        EXPECT_EQ(neuron->getFiringState(), FiringState::Resting);
        
        // Test current injection
        neuron->injectCurrent(100.0f);
        neuron->update(0.001);
        EXPECT_TRUE(neuron->getMembranePotential() > -70.0);
        
        return true;
    }
};
```

#### Synapse Tests

```cpp
// Test synaptic connections and plasticity
class TestSynapse : public TestCase {
public:
    std::string getName() const override { return "synapse"; }
    std::string getDescription() const override { 
        return "Test synaptic connections and plasticity"; 
    }
    
    bool run() override {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get synapses
        auto region = brain->getRegion(RegionId(0));
        if (!region) return false;
        
        auto synapses = region->getSynapses();
        if (synapses.empty()) return false;
        
        Synapse* synapse = synapses[0].get();
        
        // Test initial properties
        float initial_weight = synapse->getWeight();
        EXPECT_TRUE(initial_weight >= 0.0f && initial_weight <= 1.0f);
        
        // Test plasticity
        synapse->enablePlasticity(true, true, true);
        EXPECT_TRUE(synapse->isPlastic());
        
        // Test weight change
        float new_weight = synapse->getWeight() + 0.1f;
        synapse->setWeight(new_weight);
        EXPECT_EQ(synapse->getWeight(), new_weight);
        
        return true;
    }
};
```

#### STDP Tests

```cpp
// Test Spike-Timing-Dependent Plasticity
class TestSTDP : public TestCase {
public:
    std::string getName() const override { return "stdp_verification"; }
    std::string getDescription() const override { 
        return "Test STDP mechanism (pre-before-post potentiation)"; 
    }
    
    bool run() override {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 50);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01);
        config->set("plasticity.stdp.tau_plus", 20.0);
        config->set("plasticity.stdp.tau_minus", 20.0);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get neurons and synapses
        auto region = brain->getRegion(RegionId(0));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        auto synapses = region->getSynapses();
        
        if (neurons.size() < 2 || synapses.empty()) return false;
        
        Neuron* pre_neuron = neurons[0].get();
        Neuron* post_neuron = neurons[1].get();
        Synapse* synapse = synapses[0].get();
        
        // Record initial weight
        float initial_weight = synapse->getWeight();
        
        // Enable STDP only
        synapse->enablePlasticity(false, true, false);
        
        // Create pre-before-post spike timing (LTP)
        pre_neuron->injectCurrent(60.0f);
        brain->step(0, 0.0);
        
        post_neuron->injectCurrent(60.0f);
        brain->step(1, 0.001);
        
        // Check for potentiation
        float final_weight = synapse->getWeight();
        float delta = final_weight - initial_weight;
        
        EXPECT_TRUE(delta > 0.0f);  // Weight should increase
        
        return true;
    }
};
```

### Test Categories

| Test Category | Description | Location |
|---------------|-------------|----------|
| `type_system` | Type system tests (NeuronId, SynapseId, etc.) | `test_types.cpp` |
| `random_generator` | Random number generator tests | `test_random.cpp` |
| `configuration` | Configuration system tests | `test_config.cpp` |
| `simulation_clock` | Simulation clock tests | `test_clock.cpp` |
| `neuron` | Neuron dynamics tests | `test_neuron.cpp` |
| `synapse` | Synapse and plasticity tests | `test_synapse.cpp` |
| `stdp_verification` | STDP mechanism tests | `test_stdp.cpp` |
| `brain_integration` | Brain integration tests | `test_brain.cpp` |

## Integration Tests

### System Integration Tests

```cpp
// Test complete system integration
class TestSystemIntegration : public TestCase {
public:
    std::string getName() const override { return "system_integration"; }
    std::string getDescription() const override { 
        return "Test complete brain system integration"; 
    }
    
    bool run() override {
        // Create full system configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("plasticity.enable", true);
        config->set("neuromodulation.enable", true);
        config->set("development.enable", true);
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test basic connectivity
        if (brain->getTotalNeuronCount() == 0) {
            return false;
        }
        
        // Test plasticity
        auto region = brain->getRegion(RegionId(0));
        if (!region) {
            return false;
        }
        
        bool plasticity_enabled = false;
        for (const auto& synapse : region->getSynapses()) {
            if (synapse->isPlastic()) {
                plasticity_enabled = true;
                break;
            }
        }
        
        if (!plasticity_enabled) {
            return false;
        }
        
        // Test neuromodulation
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        if (brain->getDevelopmentalStage() != DevelopmentalStage::CriticalPeriod) {
            return false;
        }
        
        return true;
    }
};
```

### Test Phases

#### Phase 2: Neural Computation Tests

```cpp
class TestPhase2 : public TestCase {
public:
    std::string getName() const override { return "phase2_neural_computation"; }
    std::string getDescription() const override { 
        return "Test Phase 2: Real neural computation (LIF, plasticity)"; 
    }
    
    bool run() override {
        // Test LIF neuron dynamics
        // Test event-driven spike propagation
        // Test STDP and Hebbian plasticity
        // Test structural plasticity
        
        return true;  // Implementation depends on specific tests
    }
};
```

#### Phase 3: World Interaction Tests

```cpp
class TestPhase3 : public TestCase {
public:
    std::string getName() const override { return "phase3_world_interaction"; }
    std::string getDescription() const override { 
        return "Test Phase 3: World interaction loop"; 
    }
    
    bool run() override {
        // Test sensory input and output
        // Test reward prediction error
        // Test neuromodulation integration
        // Test developmental stages
        
        return true;  // Implementation depends on specific tests
    }
};
```

#### Phase 4: Cognitive Tests

```cpp
class TestPhase4 : public TestCase {
public:
    std::string getName() const override { return "phase4_cognition"; }
    std::string getDescription() const override { 
        return "Test Phase 4: Emerging cognitive capabilities"; 
    }
    
    bool run() override {
        // Test prediction system
        // Test working memory
        // Test episodic memory
        // Test concept formation
        // Test attention mechanism
        // Test planning capabilities
        
        return true;  // Implementation depends on specific tests
    }
};
```

## Performance Tests

### Performance Measurement

```cpp
// Performance test class
class PerformanceTest : public TestCase {
public:
    std::string getName() const override { return "performance_benchmark"; }
    std::string getDescription() const override { 
        return "Measure and benchmark simulation performance"; 
    }
    
    bool run() override {
        // Measure simulation speed
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run simulation
        int steps = 1000;
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Calculate performance metrics
        double steps_per_second = steps / (duration.count() / 1000.0);
        size_t spikes_per_second = brain->getTotalSpikeCount() / duration.count();
        
        NLM_LOG_INFO("Performance metrics:");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(steps_per_second));
        NLM_LOG_INFO("  Spikes per second: " + std::to_string(spikes_per_second));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        
        return true;
    }
};
```

### Memory Usage Tests

```cpp
// Memory usage test
class TestMemoryUsage : public TestCase {
public:
    std::string getName() const override { return "memory_usage"; }
    std::string getDescription() const override { 
        return "Test memory usage and efficiency"; 
    }
    
    bool run() override {
        // Measure memory before simulation
        auto start_memory = getMemoryUsage();
        
        // Run simulation
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        // Measure memory after simulation
        auto end_memory = getMemoryUsage();
        
        // Check for memory leaks
        double memory_increase = end_memory - start_memory;
        if (memory_increase > 100.0) {  // 100MB threshold
            NLM_LOG_WARNING("High memory usage: " + std::to_string(memory_increase) + " MB increase");
        }
        
        return true;
    }
    
private:
    double getMemoryUsage() {
        // Implementation depends on platform
        // For Linux: read from /proc/self/status
        // For Windows: use Windows API
        // For macOS: use task_info
        return 0.0;
    }
};
```

## Configuration Testing

### Configuration Space Testing

```cpp
// Test different configuration combinations
class TestConfigurationSpace : public TestCase {
public:
    std::string getName() const override { return "configuration_space"; }
    std::string getDescription() const override { 
        return "Test different configuration combinations"; 
    }
    
    bool run() override {
        // Test different neuron counts
        std::vector<int> neuron_counts = {10, 100, 1000, 5000};
        
        for (int count : neuron_counts) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", count);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return false;
            }
            
            // Verify neuron count
            if (brain->getTotalNeuronCount() != count) {
                return false;
            }
        }
        
        return true;
    }
};
```

## Test Automation

### Continuous Integration

#### GitHub Actions Example

```yaml
name: NLM Tests

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential
        pip install pybind11 scikit-build-core pytest numpy
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure
        ./nlm_test --output-junit test-results.xml
    
    - name: Upload test results
      if: always()
      uses: actions/upload-artifact@v2
      with:
        name: test-results
        path: build/test-results.xml
```

#### Jenkins Pipeline Example

```groovy
pipeline {
    agent any
    
    stages {
        stage('Checkout') {
            steps {
                git url: 'https://github.com/nlm-project/nlm.git'
            }
        }
        
        stage('Build') {
            steps {
                sh '''
                mkdir build
                cd build
                cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
                make -j$(nproc)
                '''
            }
        }
        
        stage('Test') {
            steps {
                sh '''
                cd build
                ctest --output-on-failure
                ./nlm_test
                '''
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build/test-results.xml', allowEmpty: true
                }
            }
        }
    }
}
```

### Test Reporting

#### Test Report Generation

```cpp
// Generate comprehensive test report
class TestReportGenerator {
public:
    static void generateReport(const std::vector<TestResult>& results, 
                              const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "NLM Test Report" << std::endl;
        file << "===============" << std::endl << std::endl;
        
        for (const auto& result : results) {
            file << "Test: " << result.name << std::endl;
            file << "Description: " << result.description << std::endl;
            file << "Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
            file << "Execution time: " << result.execution_time << " ms" << std::endl;
            if (!result.message.empty()) {
                file << "Message: " << result.message << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
    }
};
```

## Test Debugging

### Debugging Techniques

#### Debug Information Collection

```cpp
// Collect debug information during test failures
class DebugInfoCollector {
public:
    static void collectDebugInfo(const std::string& testName, 
                                const std::shared_ptr<Brain>& brain) {
        std::stringstream debug_file;
        debug_file << "debug_" << testName << "_" << 
            std::chrono::system_clock::now().time_since_epoch().count() << ".log";
        
        std::ofstream file(debug_file.str());
        if (!file.is_open()) {
            return;
        }
        
        file << "Debug Information for Test: " << testName << std::endl;
        file << "==============================" << std::endl << std::endl;
        
        file << "Brain Configuration:" << std::endl;
        auto config = brain->getConfig();
        if (config) {
            file << "  Neuron count: " << config->getOr("neuron_count", 0) << std::endl;
            file << "  Region count: " << config->getOr("region_count", 0) << std::endl;
            file << "  Timestep: " << config->getOr("simulation_timestep", 0.0) << std::endl;
        }
        
        file << std::endl << "Brain Statistics:" << std::endl;
        file << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        file << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        file.close();
        
        NLM_LOG_INFO("Debug information saved to: " + debug_file.str());
    }
};
```

### Common Test Issues

#### Issue 1: Random Seed Problems

**Problem**: Tests produce different results due to random number generation.

**Solution**: Use fixed random seeds in tests.

```cpp
void setRandomSeedForTests() {
    srand(42);  // Fixed seed for reproducible tests
    // Set random seed in configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
}
```

#### Issue 2: Configuration File Not Found

**Problem**: Configuration files are not found in test environment.

**Solution**: Use absolute paths or create test configuration files.

```cpp
bool testWithDefaultConfig() {
    // Test with default configuration
    auto config = std::make_shared<Config>();
    
    // Check if configuration is valid
    if (!config->has("neuron_count")) {
        return false;
    }
    
    return true;
}

bool testWithFileConfig() {
    // Create temporary configuration file for test
    std::ofstream file("test_config.json");
    file << "{\"neuron_count\": 100}";
    file.close();
    
    auto config = std::make_shared<Config>();
    bool loaded = config->loadFromFile("test_config.json");
    
    // Clean up
    std::remove("test_config.json");
    
    return loaded;
}
```

#### Issue 3: Memory Leaks

**Problem**: Memory usage increases during tests.

**Solution**: Check for memory leaks using memory profilers.

```bash
# Use valgrind for memory leak detection
valgrind --leak-check=full ./nlm_test

# Use AddressSanitizer (clang/gcc)
g++ -g -fsanitize=address -o nlm_test test_files/*.cpp
./nlm_test
```

## Contributing Tests

### Writing New Tests

#### Test Structure

```cpp
// Test structure guidelines
class TestNewFeature : public TestCase {
public:
    std::string getName() const override {
        return "new_feature_test";
    }
    
    std::string getDescription() const override {
        return "Test the new feature implementation";
    }
    
    bool run() override {
        // Test setup
        setupTestEnvironment();
        
        // Test the feature
        bool feature_works = testFeatureImplementation();
        
        // Test cleanup
        cleanupTestEnvironment();
        
        return feature_works;
    }
};
```

#### Test Best Practices

1. **Isolation**: Each test should be independent
2. **Determinism**: Use fixed seeds for reproducible results
3. **Clean-up**: Clean up resources after tests
4. **Validation**: Validate test inputs and outputs
5. **Error Handling**: Handle test errors gracefully
6. **Performance**: Measure and optimize test performance
7. **Documentation**: Document test purpose and expected behavior

### Adding Tests to the Project

#### Test File Organization

```bash
# Test file organization
/tests/
├── test_types.cpp              # Type system tests
├── test_random.cpp             # Random number generator tests
├── test_config.cpp             # Configuration system tests
├── test_clock.cpp              # Simulation clock tests
├── test_neuron.cpp             # Neuron dynamics tests
├── test_synapse.cpp            # Synapse and plasticity tests
├── test_stdp.cpp               # STDP mechanism tests
├── test_brain.cpp              # Brain integration tests
└── test_main.cpp               # Main test runner
```

#### Adding New Test Categories

```cpp
// Add new test categories as needed
class TestNewCategory : public TestCase {
public:
    std::string getName() const override {
        return "new_category_test";
    }
    
    std::string getDescription() const override {
        return "Test new category functionality";
    }
    
    bool run() override {
        // Test implementation for new category
        return true;
    }
};
```

#### Updating Test Runner

```cpp
// Update test runner to include new tests
void TestRunner::registerAllTests() {
    // Register existing tests
    registerTest(std::make_unique<TestTypeSystem>());
    registerTest(std::make_unique<TestRandomGenerator>());
    registerTest(std::make_unique<TestConfiguration>());
    registerTest(std::make_unique<TestSimulationClock>());
    registerTest(std::make_unique<TestNeuron>());
    registerTest(std::make_unique<TestSynapse>());
    registerTest(std::make_unique<TestSTDP>());
    registerTest(std::make_unique<TestBrain>());
    
    // Register new tests
    registerTest(std::make_unique<TestNewCategory>());
    registerTest(std::make_unique<TestSystemIntegration>());
    registerTest(std::make_unique<PerformanceTest>());
}
```

## Test Maintenance

### Test Maintenance Tasks

#### Regular Test Maintenance

1. **Update Tests**: Update tests when code changes
2. **Fix Tests**: Fix failing tests
3. **Add Tests**: Add tests for new functionality
4. **Remove Tests**: Remove obsolete tests
5. **Optimize Tests**: Improve test performance
6. **Document Tests**: Document test purpose and behavior

#### Test Lifecycle Management

```cpp
class TestLifecycleManager {
public:
    void setupTests() {
        // Initialize test environment
        // Create test data
        // Set up test configurations
    }
    
    void teardownTests() {
        // Clean up test environment
        // Remove temporary files
        // Release test resources
    }
    
    void handleTestFailure(const TestResult& result) {
        // Collect debug information
        // Generate test failure report
        // Update test statistics
    }
    
    void analyzeTestResults(const std::vector<TestResult>& results) {
        // Calculate test statistics
        // Identify test failures
        // Generate recommendations
    }
};
```

## Testing Guidelines

### Coding Standards

#### Test Code Style

```cpp
// Test code style guidelines
// 1. Use descriptive test names
// 2. Use descriptive variable names
// 3. Use meaningful assertions
// 4. Add comments for complex tests
// 5. Use try-catch blocks for error handling
// 6. Validate test inputs and outputs
// 7. Clean up resources after tests

// Example good test
TEST(NeuronTest, BasicProperties) {
    Neuron neuron;
    neuron.setMembranePotential(-70.0);
    
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -70.0);
    EXPECT_EQ(neuron.getFiringState(), FiringState::Resting);
    
    // Clean up
    neuron.reset();
}
```

#### Test Naming Conventions

```cpp
// Test naming conventions
class TestTypeSystem : public TestCase {
public:
    std::string getName() const override {
        return "type_system_initialization";  // Descriptive name
    }
    
    std::string getDescription() const override {
        return "Test type system initialization and basic operations";  // Clear description
    }
    
    bool run() override {
        // Test implementation
    }
};
```

### Test Documentation

#### Test Documentation Guidelines

```cpp
// Test documentation guidelines
class TestNeuron : public TestCase {
public:
    std::string getName() const override {
        return "neuron_dynamics_test";
    }
    
    std::string getDescription() const override {
        return "Test Leaky Integrate-and-Fire neuron dynamics including "
               "membrane potential integration, spike generation, "
               "refractory period, and current injection effects. "
               "This test validates the core neural computation "
               "capabilities of the NLM system.";
    }
    
    bool run() override {
        // Test implementation
        // ...
    }
};
```

## Conclusion

The NLM testing framework provides comprehensive tools for testing neural simulation systems. It includes:

- **Unit Tests**: Test individual components
- **Integration Tests**: Test component interactions
- **System Tests**: Test complete system behavior
- **Performance Tests**: Measure and optimize performance
- **Configuration Tests**: Test different configuration options
- **Regression Tests**: Prevent functionality regression

Key takeaways:

1. **Comprehensive Coverage**: Test all aspects of the system
2. **Deterministic Testing**: Use fixed seeds for reproducible results
3. **Automated Testing**: Integrate testing into CI/CD pipeline
4. **Performance Testing**: Measure and optimize performance
5. **Configuration Testing**: Test different configurations
6. **Error Handling**: Handle test errors gracefully
7. **Documentation**: Document test purpose and behavior
8. **Maintenance**: Regularly maintain and update tests

The NLM testing framework ensures the reliability, performance, and correctness of the neural simulation system across different use cases and configurations.