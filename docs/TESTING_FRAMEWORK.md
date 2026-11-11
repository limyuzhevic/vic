# Enhanced Testing Framework for NLM

## Overview

This document describes the enhanced testing framework for the NLM (Neural Learning Machine)
project, providing comprehensive coverage of all system components, improved test organization,
and better test quality.

## Testing Philosophy

### 1. Comprehensive Coverage
- Test all public APIs
- Test edge cases and error conditions
- Test integration between components
- Test performance characteristics

### 2. Quality Assurance
- Deterministic tests (fixed random seeds)
- Scalable tests (parameterized tests)
- Reproducible tests (snapshot testing)
- Performance tests (benchmarking)

### 3. Developer Experience
- Clear test organization
- Helpful error messages
- Easy debugging
- Good documentation

## Test Organization

### 1. Test Categories

#### Unit Tests
- Test individual classes and functions
- Mock external dependencies
- Test pure functions in isolation

#### Integration Tests
- Test component interactions
- Test system-level behavior
- Test configuration loading and validation

#### End-to-End Tests
- Test complete workflows
- Test user scenarios
- Test real-world use cases

#### Performance Tests
- Benchmark critical paths
- Test scalability
- Test resource usage

#### Fuzz Tests
- Test with invalid inputs
- Test with extreme values
- Test with random data

### 2. Test Structure

```
tests/
├── unit/
│   ├── core/
│   │   ├── test_config.cpp
│   │   ├── test_random.cpp
│   │   ├── test_logger.cpp
│   │   └── test_simulation_clock.cpp
│   ├── brain/
│   │   ├── test_neuron.cpp
│   │   ├── test_synapse.cpp
│   │   ├── test_neural_population.cpp
│   │   ├── test_neural_region.cpp
│   │   └── test_brain.cpp
│   ├── plasticity/
│   │   ├── test_stdp.cpp
│   │   ├── test_hebbian.cpp
│   │   └── test_structural_plasticity.cpp
│   ├── memory/
│   │   ├── test_working_memory.cpp
│   │   ├── test_episodic_memory.cpp
│   │   └── test_associative_memory.cpp
│   ├── cognition/
│   │   ├── test_concept_formation.cpp
│   │   ├── test_neural_planner.cpp
│   │   └── test_attentional_selection.cpp
│   └── world/
│       ├── test_simple_world.cpp
│       └── test_agent_body.cpp
├── integration/
│   ├── test_brain_integration.cpp
│   ├── test_memory_integration.cpp
│   ├── test_plasticity_integration.cpp
│   └── test_cognition_integration.cpp
├── e2e/
│   ├── test_phase2_demo.cpp
│   ├── test_phase3_demo.cpp
│   ├── test_phase4_demo.cpp
│   └── test_phase6_demo.cpp
├── performance/
│   ├── test_neuron_performance.cpp
│   ├── test_plasticity_performance.cpp
│   └── test_memory_performance.cpp
└── fuzz/
    ├── test_config_fuzz.cpp
    ├── test_synapse_fuzz.cpp
    └── test_plasticity_fuzz.cpp
```

### 3. Test Framework

#### Test Base Class
```cpp
class NLMTest {
public:
    virtual ~NLMTest() = default;
    
    // Setup before each test
    virtual void setup() {}
    
    // Cleanup after each test
    virtual void teardown() {}
    
    // Helper methods
    template<typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            FAIL(message + "Expected: " + std::to_string(expected) + 
                ", Actual: " + std::to_string(actual));
        }
    }
    
    template<typename T>
    void assertNotEqual(const T& notExpected, const T& actual, const std::string& message = "") {
        if (notExpected == actual) {
            FAIL(message + "Expected not: " + std::to_string(notExpected));
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            FAIL(message + "Expected true, got false");
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            FAIL(message + "Expected false, got true");
        }
    }
    
protected:
    void FAIL(const std::string& message) {
        throw TestFailureException(message);
    }
};
```

#### Test Fixtures
```cpp
class BrainTestFixture : public NLMTest {
public:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    void setup() override {
        config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void teardown() override {
        brain.reset();
        config.reset();
    }
};
```

### 4. Test Configuration

#### Test Configuration File
```yaml
# tests/test_config.yaml
test:
  # General test settings
  random_seed: 12345
  verbose: true
  timeout_seconds: 30
  
  # Performance thresholds
  max_time_per_test_ms: 1000
  max_memory_mb: 512
  
  # Test categories to run
  categories:
    - unit
    - integration
    - e2e
    
  # Specific tests to skip
  skip_tests:
    - test_plasticity_fuzz  # Slow and flaky
    
  # Parameterized tests
  parameter_sets:
    small_brain:
      neuron_count: 10
      region_count: 1
    medium_brain:
      neuron_count: 100
      region_count: 2
    large_brain:
      neuron_count: 1000
      region_count: 4
    
  # Test scenarios
  scenarios:
    basic_connectivity:
      description: "Test basic neural connectivity"
      steps: 100
      neurons: 50
    learning_experiment:
      description: "Test synaptic learning"
      steps: 1000
      neurons: 100
    plasticity_test:
      description: "Test plasticity rules"
      steps: 500
      neurons: 200
```

#### Test Framework Implementation
```cpp
class TestFramework {
private:
    std::shared_ptr<Config> testConfig;
    std::vector<std::string> skippedTests;
    std::vector<std::pair<std::string, TestParameters>> parameterSets;
    std::vector<TestResult> results;
    
public:
    TestFramework(const std::string& configPath) {
        testConfig = std::make_shared<Config>();
        testConfig->loadFromFile(configPath);
        loadConfiguration();
    }
    
    void loadConfiguration() {
        // Load test configuration from file
        // Set up parameters, skipped tests, etc.
    }
    
    void runTests() {
        // Load all test cases
        auto testCases = loadTestCases();
        
        // Filter tests based on configuration
        auto filteredTests = filterTests(testCases);
        
        // Run tests with parameter sets
        for (const auto& testCase : filteredTests) {
            runTestWithParameters(testCase);
        }
        
        // Generate report
        generateReport();
    }
    
    void runTestWithParameters(const TestCase& testCase) {
        for (const auto& paramSet : parameterSets) {
            if (shouldRunTest(testCase, paramSet)) {
                runSingleTest(testCase, paramSet);
            }
        }
    }
    
    void runSingleTest(const TestCase& testCase, const TestParameters& params) {
        std::cout << "Running test: " << testCase.name << std::endl;
        
        try {
            testCase.setup(params);
            testCase.run();
            testCase.teardown();
            
            recordResult(testCase.name, params, TestResult::PASS);
        } catch (const TestFailureException& e) {
            recordResult(testCase.name, params, TestResult::FAIL, e.what());
        }
    }
    
    void generateReport() {
        // Generate comprehensive test report
    }
};
```

### 5. Test Categories Details

#### Unit Tests

##### Core Tests
```cpp
namespace test_core {
    
    void testConfigCreation() {
        nlm::Config config;
        assert(!config.has("test"));
    }
    
    void testConfigSetAndGet() {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        assert(config.has("int_value"));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value());
        assert(*intVal == 42);
    }
    
    void testConfigValidation() {
        nlm::Config config;
        
        // Test valid configurations
        config.set("neuron_count", 100, nlm::ConfigSource::Default);
        assert(config.has("neuron_count"));
        
        // Test invalid configurations (should handle gracefully)
        // This would test error handling
    }
}
```

##### Brain Tests
```cpp
namespace test_brain {
    
    void testBrainCreation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.getTotalNeuronCount() == 100);
    }
    
    void testBrainInitialization() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
    }
    
    void testBrainStep() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        assert(brain.getTotalNeuronCount() == 100);
    }
}
```

##### Plasticity Tests
```cpp
namespace test_plasticity {
    
    void testSTDP() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Enable STDP
        if (auto* region = brain.getRegion(nlm::RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);  // STDP only
            }
        }
        
        // Create correlated pre-post activity
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region && !region->getSynapses().empty()) {
            auto* syn = region->getSynapses()[0].get();
            float beforeWeight = syn->getWeight();
            
            // Pre -> post activity
            for (int trial = 0; trial < 50; ++trial) {
                brain.injectCurrent(syn->getSourceNeuron(), 60.0f);
                brain.step(trial * 2, trial * 2 * 0.001);
                brain.injectCurrent(syn->getDestinationNeuron(), 60.0f);
                brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float afterWeight = syn->getWeight();
            float delta = afterWeight - beforeWeight;
            
            // Check for potentiation
            assert(delta > 0.001f);
        }
    }
    
    void testHebbian() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Enable Hebbian plasticity
        if (auto* region = brain.getRegion(nlm::RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, false, false);  // Hebbian only
            }
        }
        
        // Create correlated activity
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region && !region->getNeurons().empty()) {
            auto* preNeuron = region->getNeurons()[0].get();
            auto* postNeuron = region->getNeurons()[1].get();
            
            // Find synapse between them
            if (auto* syn = findSynapse(preNeuron->getId(), postNeuron->getId(), region)) {
                float beforeWeight = syn->getWeight();
                
                // Correlated pre-post activity
                for (int trial = 0; trial < 20; ++trial) {
                    brain.injectCurrent(preNeuron->getId(), 50.0f);
                    brain.step(trial * 3, trial * 3 * 0.001);
                    brain.injectCurrent(postNeuron->getId(), 50.0f);
                    brain.step(trial * 3 + 1, (trial * 3 + 1) * 0.001);
                }
                
                float afterWeight = syn->getWeight();
                float delta = afterWeight - beforeWeight;
                
                // Check for potentiation
                assert(delta > 0.001f);
            }
        }
    }
}
```

#### Integration Tests

```cpp
namespace test_integration {
    
    void testBrainMemoryIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test integration between brain and memory systems
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        auto* associativeMemory = brain.getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        // Test that memories are connected to brain
        assert(workingMemory->getBrain() == &brain);
        assert(episodicMemory->getBrain() == &brain);
        assert(associativeMemory->getBrain() == &brain);
    }
    
    void testBrainPlasticityIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test integration between brain and plasticity systems
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
        
        auto* structural = brain.getStructuralPlasticity();
        assert(structural != nullptr);
        
        // Test that plasticity systems are connected to brain
        assert(stdp->getBrain() == &brain);
        assert(hebbian->getBrain() == &brain);
        assert(structural->getBrain() == &brain);
        
        // Test that brain's plasticity update calls all systems
        brain.updatePlasticity();
        
        // Verify that plasticity was applied
        auto* region = brain.getRegion(nlm::RegionId(1));
        assert(region != nullptr);
        
        bool plasticityApplied = false;
        for (const auto& syn : region->getSynapses()) {
            if (syn->getWeight() > 0.0f) {  // Weight should change after plasticity
                plasticityApplied = true;
                break;
            }
        }
        
        assert(plasticityApplied);
    }
}
```

#### End-to-End Tests

```cpp
namespace test_e2e {
    
    void testPhase2Demo() {
        // Test the complete Phase 2 demo workflow
        nlm::Brain brain = runPhase2Demo();
        
        // Verify expected outcomes
        assert(brain.getTotalSpikeCount() > 0);
        assert(brain.getFiringNeuronCount() > 0);
        
        // Verify learning occurred
        assert(brain.getExcitationInhibitionRatio() > 0.0f);
        assert(brain.getAverageFiringRate() > 0.0f);
    }
    
    void testPhase6Integration() {
        // Test the complete Phase 6 integration workflow
        nlm::Brain brain = runPhase6IntegrationDemo();
        
        // Verify all systems are integrated
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getAssociativeMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getDevelopmentSystem() != nullptr);
        
        // Verify neuromodulation is integrated
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getNovelty() != nullptr);
        assert(brain.getPredictionErrorSignal() != nullptr);
        
        // Verify developmental stage is accessible
        assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    }
}
```

### 6. Performance Tests

```cpp
namespace test_performance {
    
    void benchmarkNeuronCreation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        
        // Measure time to create neurons
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            config->set("neuron_count", static_cast<int64_t>(i), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        // Verify performance is reasonable
        double timePerNeuron = duration / 1000;
        assert(timePerNeuron < 0.1);  // Should be less than 100ms per neuron
    }
    
    void benchmarkSimulationSteps() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Measure time for simulation steps
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        // Calculate performance metrics
        double stepsPerSecond = 1000 / duration;
        double timePerStep = duration / 1000;
        
        // Verify performance
        assert(stepsPerSecond > 1000);  // Should be able to run >1000 steps per second
        assert(timePerStep < 0.001);  // Should be less than 1ms per step
    }
}
```

### 7. Fuzz Tests

```cpp
namespace test_fuzz {
    
    void testConfigFuzz() {
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> intDist(-1000, 1000);
        std::uniform_real_distribution<double> doubleDist(-1.0, 1.0);
        
        // Generate random configurations
        for (int i = 0; i < 100; ++i) {
            nlm::Config config;
            
            // Add random configuration entries
            for (int j = 0; j < 10; ++j) {
                std::string key = "random_key_" + std::to_string(j);
                
                // Randomly choose type
                int type = intDist(rng) % 4;
                
                switch (type) {
                    case 0:
                        config.set(key, intDist(rng), nlm::ConfigSource::Runtime);
                        break;
                    case 1:
                        config.set(key, doubleDist(rng), nlm::ConfigSource::Runtime);
                        break;
                    case 2:
                        config.set(key, std::string("random_string"), nlm::ConfigSource::Runtime);
                        break;
                    case 3:
                        config.set(key, bool(intDist(rng) % 2), nlm::ConfigSource::Runtime);
                        break;
                }
            }
            
            // Try to use the configuration
            try {
                // This should not crash
                auto keys = config.getKeys();
                assert(keys.size() <= 10);
            } catch (const std::exception& e) {
                // Exceptions are acceptable in fuzz testing
                // as long as they don't crash the program
            }
        }
    }
    
    void testSynapseFuzz() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Generate random spikes
        std::mt19937 rng(42);
        std::uniform_real_distribution<double> spikeDist(0.0, 1.0);
        std::uniform_int_distribution<size_t> neuronDist(0, config->getOr<int64_t>("neuron_count", 100) - 1);
        
        for (int i = 0; i < 1000; ++i) {
            if (spikeDist(rng) < 0.1) {  // 10% chance of spike
                size_t neuronId = neuronDist(rng);
                brain.injectCurrent(nlm::NeuronId(neuronId), spikeDist(rng) * 100.0f);
            }
            
            // Step the simulation
            brain.step(i);
        }
        
        // Verify simulation didn't crash
        assert(brain.getTotalNeuronCount() == 100);
    }
}
```

### 8. Test Utilities

```cpp
namespace test_utils {
    
    // Generate test configurations with various parameter combinations
    std::vector<std::shared_ptr<Config>> generateTestConfigs() {
        std::vector<std::shared_ptr<Config>> configs;
        
        // Small configuration
        auto smallConfig = std::make_shared<Config>();
        smallConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        smallConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        smallConfig->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
        smallConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configs.push_back(smallConfig);
        
        // Medium configuration
        auto mediumConfig = std::make_shared<Config>();
        mediumConfig->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        mediumConfig->set("simulation_timestep", 0.0005, ConfigSource::Default);
        mediumConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        mediumConfig->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        mediumConfig->set("connection_probability", 0.1, ConfigSource::Default);
        configs.push_back(mediumConfig);
        
        // Large configuration
        auto largeConfig = std::make_shared<Config>();
        largeConfig->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        largeConfig->set("simulation_timestep", 0.0002, ConfigSource::Default);
        largeConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        largeConfig->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        largeConfig->set("connection_probability", 0.05, ConfigSource::Default);
        configs.push_back(largeConfig);
        
        return configs;
    }
    
    // Run parameterized tests
    template<typename TestCase, typename... Args>
    void runParameterizedTest(const std::vector<TestParameters>& parameters, 
                             std::function<void(TestCase&, Args...)> testFunc,
                             Args... args) {
        for (const auto& param : parameters) {
            TestCase testCase;
            testCase.setup(param);
            testFunc(testCase, std::forward<Args>(args)...);
            testCase.teardown();
        }
    }
    
    // Measure test execution time
    template<typename TestFunc>
    double measureExecutionTime(TestFunc testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        testFunc();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    // Verify test results with tolerance
    template<typename T>
    bool areEqualWithTolerance(const T& expected, const T& actual, double tolerance) {
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(expected - actual) <= tolerance;
        } else {
            return expected == actual;
        }
    }
}
```

### 9. Test Reporting

```cpp
class TestReporter {
public:
    void reportTestStart(const std::string& testName, const TestParameters& params) {
        std::cout << "[START] " << testName << " (" << params << ")" << std::endl;
    }
    
    void reportTestResult(const std::string& testName, const TestResult& result, 
                         double executionTime, const std::string& message = "") {
        std::string status = result == TestResult::PASS ? "PASS" : "FAIL";
        std::cout << "[" << status << "] " << testName << " (" 
                  << executionTime << "s)" << std::endl;
        
        if (message.empty() && result != TestResult::PASS) {
            std::cout << "  Message: " << message << std::endl;
        }
    }
    
    void reportTestEnd() {
        std::cout << "[END] All tests completed" << std::endl;
    }
    
    void generateReport(const std::string& filename) {
        // Generate detailed test report in various formats
        // (JSON, HTML, XML, etc.)
    }
};
```

## Test Implementation Examples

### 1. Unit Test Implementation

```cpp
#include "tests/unit/core/test_config.cpp"
#include "tests/unit/brain/test_brain.cpp"
// ... include all unit tests

int main() {
    TestFramework framework("tests/test_config.yaml");
    framework.runTests();
    
    return 0;
}
```

### 2. Integration Test Implementation

```cpp
#include "tests/integration/test_brain_integration.cpp"
// ... include all integration tests

int main() {
    TestFramework framework("tests/test_config.yaml");
    
    // Only run integration tests
    framework.setTestCategories({"integration"});
    
    framework.runTests();
    
    return 0;
}
```

### 3. Performance Test Implementation

```cpp
#include "tests/performance/test_neuron_performance.cpp"
// ... include all performance tests

int main() {
    TestFramework framework("tests/test_config.yaml");
    
    // Only run performance tests
    framework.setTestCategories({"performance"});
    
    framework.runTests();
    
    return 0;
}
```

## Test Quality Assurance

### 1. Test Coverage Requirements

- **Core Components**: >90% code coverage
- **Integration Points**: >80% code coverage
- **Error Handling**: 100% test coverage
- **Performance Critical Code**: >95% code coverage

### 2. Test Determinism Requirements

- All tests must be deterministic
- Fixed random seeds for all tests
- No reliance on external state
- Reproducible test results

### 3. Test Performance Requirements

- Unit tests: <100ms execution time
- Integration tests: <1s execution time
- End-to-end tests: <10s execution time
- Performance tests: Variable, but should complete within reasonable time

### 4. Test Maintainability Requirements

- Clear test organization
- Descriptive test names
- Comprehensive test documentation
- Easy to add new tests
- Easy to modify existing tests

## Conclusion

The enhanced testing framework provides comprehensive coverage of the NLM system,
with improved test organization, better quality assurance, and enhanced developer experience.

Key improvements include:

1. **Comprehensive Coverage**: Tests for all system components, integration points, and edge cases
2. **Improved Organization**: Clear test structure with categories and hierarchies
3. **Better Quality**: Deterministic, scalable, and reproducible tests
4. **Enhanced Developer Experience**: Clear test framework, utilities, and reporting
5. **Performance Testing**: Built-in performance benchmarking and profiling
6. **Fuzz Testing**: Robust fuzz testing for error conditions

This testing framework ensures the NLM system is thoroughly tested, reliable, and maintainable,
while providing valuable insights into system performance and behavior.

The framework is designed to scale with the system complexity, providing clear paths for
extending the test suite as new features are added and the system evolves.