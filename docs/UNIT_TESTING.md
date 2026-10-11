# NLM Unit Testing Guide

## Overview

This guide provides comprehensive instructions for implementing unit tests for the NLM (Neural Network Model) system. Unit tests are essential for verifying individual components in isolation and ensuring code quality throughout development.

## Test Categories

### 1. Brain Core Functionality Tests
**File**: `tests/unit/TestBrainCore.cpp`

**Purpose**: Test core brain functionality including neuron dynamics, synaptic transmission, and plasticity.

**Key Test Cases**:
- Brain initialization and configuration
- LIF neuron dynamics (subthreshold and suprathreshold)
- Synaptic transmission and strength
- Neural population behavior
- STDP plasticity mechanisms

**Testing Approach**:
```cpp
class TestBrainCore : public TestCase {
public:
    TestBrainCore() : TestCase("BrainCore") {}
    
    void testBrainInitialization() {
        Brain brain;
        assert(brain.getRegionCount() > 0);
        assert(brain.getNeuronCount() > 0);
        assert(brain.getSynapseCount() > 0);
    }
    
    void testNeuronLIFDynamics() {
        Neuron neuron;
        // Test subthreshold integration
        neuron.integrate(0.5, 0.1);
        assert(neuron.getMembranePotential() < neuron.getThreshold());
        
        // Test suprathreshold firing
        neuron.integrate(1.0, 1.0);
        assert(neuron.shouldFire());
        neuron.fire();
        assert(neuron.getRefractoryPeriod() > 0);
    }
};
```

### 2. Memory System Tests
**File**: `tests/unit/TestMemorySystem.cpp`

**Purpose**: Test memory system functionality including storage, retrieval, and decay mechanisms.

**Key Test Cases**:
- Memory module initialization
- Short-term memory storage and retrieval
- Long-term memory pattern storage
- Memory decay over time
- Memory capacity limits

**Testing Approach**:
```cpp
class TestMemorySystem : public TestCase {
public:
    TestMemorySystem() : TestCase("MemorySystem") {}
    
    void testMemoryModuleInitialization() {
        MemoryModule module;
        assert(module.getCapacity() > 0);
        assert(module.getAvailableMemory() <= module.getCapacity());
    }
    
    void testShortTermMemory() {
        MemorySystem memory;
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
        memory.storeShortTerm(data, MemoryType::SPATIAL);
        
        auto retrieved = memory.retrieveShortTerm(MemoryType::SPATIAL);
        assert(retrieved.size() == data.size());
        assert(retrieved == data);
    }
};
```

### 3. Neuromodulation Tests
**File**: `tests/unit/TestNeuromodulation.cpp`

**Purpose**: Test neuromodulation system including dopamine, serotonin, and acetylcholine signaling.

**Key Test Cases**:
- Dopamine modulation for reward prediction
- Serotonin modulation for mood regulation
- Acetylcholine modulation for attention
- Neuromodulator interactions
- Homeostatic regulation

**Testing Approach**:
```cpp
class TestNeuromodulation : public TestCase {
public:
    TestNeuromodulation() : TestCase("Neuromodulation") {}
    
    void testDopamineModulation() {
        NeuromodulationSystem neuromod;
        DopamineNeuromodulator dopamine;
        neuromod.registerNeuromodulator(&dopamine);
        
        double reward_error = 1.0;
        double dopamine_level = neuromod.processRewardError(reward_error);
        
        assert(dopamine_level > 0.0);
        assert(dopamine_level <= 1.0);
    }
};
```

### 4. Plasticity Tests
**File**: `tests/unit/TestPlasticity.cpp`

**Purpose**: Test synaptic plasticity mechanisms including STDP, Hebbian learning, and metaplasticity.

**Key Test Cases**:
- STDP rule implementation (LTP and LTD)
- Structural plasticity
- Hebbian learning
- Oja's learning rule
- Metaplastic changes

**Testing Approach**:
```cpp
class TestPlasticity : public TestCase {
public:
    TestPlasticity() : TestCase("Plasticity") {}
    
    void testSTDPRule() {
        STDPRule stdp;
        
        // Long-term potentiation (post after pre)
        double pre_time = 0.0;
        double post_time = 10.0;
        double weight_change = stdp.calculateWeightChange(pre_time, post_time);
        
        assert(weight_change > 0.0);
        assert(weight_change <= 1.0);
        
        // Long-term depression (pre after post)
        pre_time = 10.0;
        post_time = 0.0;
        weight_change = stdp.calculateWeightChange(pre_time, post_time);
        
        assert(weight_change < 0.0);
        assert(weight_change >= -1.0);
    }
};
```

### 5. Cognitive Tests
**File**: `tests/unit/TestCognition.cpp`

**Purpose**: Test cognitive functions including attention, working memory, decision making, and learning.

**Key Test Cases**:
- Attention mechanism
- Working memory capacity
- Value-based decision making
- Reinforcement learning
- Cognitive load management

**Testing Approach**:
```cpp
class TestCognition : public TestCase {
public:
    TestCognition() : TestCase("Cognition") {}
    
    void testAttentionMechanism() {
        Attention attention;
        
        std::vector<double> features = {0.8, 0.3, 0.9, 0.1, 0.5};
        double attention_weight = attention.computeAttention(features);
        
        assert(attention_weight >= 0.0 && attention_weight <= 1.0);
        assert(attention_weight > features[0]);
    }
};
```

### 6. Sensory Tests
**File**: `tests/unit/TestSensory.cpp`

**Purpose**: Test sensory system including perception, feature extraction, and cross-modal integration.

**Key Test Cases**:
- Sensory percept creation
- Multi-sensory integration
- Feature extraction
- Sensory adaptation
- Selective attention

**Testing Approach**:
```cpp
class TestSensory : public TestCase {
public:
    TestSensory() : TestCase("Sensory") {}
    
    void testSensoryPerceptCreation() {
        SensoryPercept percept;
        
        std::vector<double> input = {0.8, 0.3, 0.6, 0.2, 0.9};
        percept.createPercept(input, SensoryModality::VISUAL);
        
        assert(percept.getModality() == SensoryModality::VISUAL);
        assert(percept.getFeatures().size() == input.size());
    }
};
```

### 7. Motor Tests
**File**: `tests/unit/TestMotor.cpp`

**Purpose**: Test motor system including command generation, control, coordination, and fatigue.

**Key Test Cases**:
- Motor command generation
- Motor learning
- Motor control (feedforward and feedback)
- Motor coordination
- Motor fatigue

**Testing Approach**:
```cpp
class TestMotor : public TestCase {
public:
    TestMotor() : TestCase("Motor") {}
    
    void testMotorCommandGeneration() {
        MotorSystem motor;
        
        std::vector<double> target_position = {1.0, 0.5, 0.2};
        std::vector<double> current_position = {0.0, 0.0, 0.0};
        
        MotorCommand command = motor.generateCommand(
            target_position, current_position, MotorType::REACHING);
        
        assert(command.magnitude > 0.0);
        assert(command.direction.size() == 3);
    }
};
```

### 8. Performance Tests
**File**: `tests/unit/TestPerformance.cpp`

**Purpose**: Test performance profiler and optimization engine functionality.

**Key Test Cases**:
- Profiler accuracy
- Optimization engine
- Memory usage optimization
- CPU benchmark
- Profile aggregation

**Testing Approach**:
```cpp
class TestPerformance : public TestCase {
public:
    TestPerformance() : TestCase("Performance") {}
    
    void testProfilerAccuracy() {
        PerformanceProfiler profiler;
        
        profiler.startProfile("computational_task");
        
        double result = 0.0;
        for (int i = 0; i < 100000; ++i) {
            result += std::sin(i * 0.1) * std::cos(i * 0.2);
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("computational_task");
        assert(metrics.duration.count() > 0);
        assert(metrics.cpu_cycles > 0);
        assert(metrics.memory_usage > 0);
        assert(std::isfinite(metrics.gflops));
    }
};
```

### 9. Configuration Tests
**File**: `tests/unit/TestConfig.cpp`

**Purpose**: Test configuration system including validation, loading, serialization, and inheritance.

**Key Test Cases**:
- Configuration system initialization
- Parameter validation
- Configuration loading from file
- Configuration serialization
- Configuration inheritance

**Testing Approach**:
```cpp
class TestConfig : public TestCase {
public:
    TestConfig() : TestCase("Config") {}
    
    void testConfigSystemInitialization() {
        ConfigSystem config;
        
        assert(config.getConfigValue("brain.neuron.count", 100) == 100);
        assert(config.getConfigValue("brain.synaptic.connectivity", 0.5) == 0.5);
        assert(config.getConfigValue("brain.learning.rate", 0.01) == 0.01);
    }
};
```

### 10. Development Tests
**File**: `tests/unit/TestDevelopment.cpp`

**Purpose**: Test development manager functionality including skill progression and adaptive learning.

**Key Test Cases**:
- Development manager
- Skill progression
- Adaptive learning
- Development benchmarks
- Cross-domain integration

**Testing Approach**:
```cpp
class TestDevelopment : public TestCase {
public:
    TestDevelopment() : TestCase("Development") {}
    
    void testDevelopmentManager() {
        DevelopmentManager dev_manager;
        
        assert(dev_manager.getCurrentStage() == LearningStage::BEGINNER);
        
        dev_manager.advanceStage();
        assert(dev_manager.getCurrentStage() == LearningStage::INTERMEDIATE);
    }
};
```

## Testing Best Practices

### 1. Test Organization
- Group related tests together
- Use descriptive test names
- Organize tests by functionality
- Maintain test hierarchy

### 2. Test Fixtures
- Create reusable test fixtures
- Set up test environment
- Clean up after tests
- Ensure test isolation

### 3. Test Data
- Use realistic test data
- Include edge cases
- Test boundary conditions
- Use parameterized data

### 4. Assertions
- Use meaningful assertions
- Provide descriptive error messages
- Check preconditions
- Validate postconditions

### 5. Performance Testing
- Measure execution time
- Monitor memory usage
- Profile CPU cycles
- Track GFLOPs

## Running Unit Tests

### Build Commands
```bash
# Build unit tests
make nlm_unit_tests

# Run unit tests
./nlm_unit_tests

# Run with verbose output
./nlm_unit_tests --verbose

# Generate test report
./nlm_unit_tests --report=html
```

### CMake Integration
```cmake
# In CMakeLists.txt
add_executable(nlm_unit_tests
    unit/TestBrainCore.cpp
    unit/TestMemorySystem.cpp
    unit/TestNeuromodulation.cpp
    unit/TestPlasticity.cpp
    unit/TestCognition.cpp
    unit/TestSensory.cpp
    unit/TestMotor.cpp
    unit/TestPerformance.cpp
    unit/TestConfig.cpp
    unit/TestDevelopment.cpp
)

target_link_libraries(nlm_unit_tests PRIVATE nlm_core)
target_compile_features(nlm_unit_tests PRIVATE cxx_std_20)
```

## Test Coverage Metrics

### Coverage Analysis
- **Statement Coverage**: Percentage of statements executed
- **Branch Coverage**: Percentage of branches taken
- **Function Coverage**: Percentage of functions called
- **Line Coverage**: Percentage of lines executed

### Performance Metrics
- **Execution Time**: Average test execution time
- **Memory Usage**: Peak memory consumption
- **CPU Cycles**: Total CPU cycles used
- **GFLOPs**: Floating-point operations per second

### Test Quality Metrics
- **Pass Rate**: Percentage of tests passed
- **Failure Rate**: Percentage of tests failed
- **Skip Rate**: Percentage of tests skipped
- **Success Rate**: (Passed / Total) * 100

## Integration with CI/CD

### GitHub Actions
```yaml
test_unit:
  runs-on: ubuntu-latest
  steps:
    - name: Build unit tests
      run: make nlm_unit_tests
    
    - name: Run unit tests
      run: ./nlm_unit_tests
    
    - name: Generate coverage report
      run: ./nlm_unit_tests --coverage
```

### GitLab CI
```yaml
test_unit:
  script:
    - make nlm_unit_tests
    - ./nlm_unit_tests
    - ./nlm_unit_tests --coverage
```

## Test Maintenance

### Adding New Tests
1. Create test file in `tests/unit/`
2. Include appropriate headers
3. Implement test cases
4. Add test to CMakeLists.txt
5. Run tests to verify

### Modifying Tests
1. Update test file
2. Run tests to verify changes
3. Update documentation
4. Commit changes

### Removing Tests
1. Remove test file
2. Update CMakeLists.txt
3. Run remaining tests
4. Update documentation

## Conclusion

Unit testing is fundamental to the NLM testing framework. The comprehensive unit testing suite ensures individual components work correctly in isolation, providing a solid foundation for integration, performance, stress, compatibility, and regression testing. Proper unit test implementation leads to higher code quality, better maintainability, and more reliable software systems.