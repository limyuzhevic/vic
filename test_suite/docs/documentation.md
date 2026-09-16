# Documentation & Examples

Comprehensive documentation and examples for the NLM test suite, including test case documentation, usage examples, performance benchmarks, and best practices for testing.

## Test Case Documentation

### Overview of Test Cases

This section provides detailed documentation for all test cases in the NLM test suite, including their purpose, implementation details, and expected outcomes.

### Test Case Categories

#### Unit Tests
**Location:** `src/test/`

- **test_brain.cpp**: Basic brain functionality tests (creation, initialization, stepping, reset, regions, action production, status)
- **test_clock.cpp**: Simulation clock functionality
- **test_config.cpp**: Configuration system tests
- **test_main.cpp**: Main application tests
- **test_neuron.cpp**: Neuron behavior and properties
- **test_random.cpp**: Random number generator tests
- **test_stdp.cpp**: Spike-Timing-Dependent Plasticity tests
- **test_synapse.cpp**: Synapse connection and transmission tests
- **test_types.cpp**: Type system and data structure tests

#### Integration Tests
**Location:** `src/integration_test/`

- **test_phase6_integration.cpp**: Complete Phase 6 system integration
- **test_e2e_learning.cpp**: End-to-end learning scenarios
- **test_e2e_navigation.cpp**: Navigation and exploration testing
- **test_e2e_interaction.cpp**: Agent-environment interaction testing
- **test_prediction_memory_integration.cpp**: Prediction and memory system integration
- **test_attention_working_memory.cpp**: Attention and working memory interaction
- **test_neuromodulation_plasticity.cpp**: Neuromodulation and plasticity integration
- **test_performance_regression.cpp**: Performance regression testing
- **test_memory_usage.cpp**: Memory usage validation
- **test_processing_speed.cpp**: Processing speed validation

#### Advanced Features Tests
**Location:** `src/advanced_test/`

- **test_advanced_config_validation.cpp**: Advanced configuration validation
- **test_config_constraints.cpp**: Configuration constraints testing
- **test_config_security.cpp**: Configuration security and integrity
- **test_template_management.cpp**: Template lifecycle management
- **test_template_expansion.cpp**: Template variable expansion
- **test_template_inheritance.cpp**: Template inheritance and overriding
- **test_parallel_safety.cpp**: Thread safety testing
- **test_parallel_execution.cpp**: Parallel execution performance
- **test_concurrent_access.cpp**: Concurrent access testing
- **test_batch_processing.cpp**: Batch processing operations
- **test_monitoring_system.cpp**: Real-time monitoring system
- **test_performance_monitoring.cpp**: Performance monitoring
- **test_alert_system.cpp**: Alert and notification system

#### Edge Case & Stress Tests
**Location:** `src/edge_case_test/`

- **test_memory_exhaustion.cpp**: Memory exhaustion scenarios
- **test_memory_pressure.cpp**: System under memory pressure
- **test_memory_leaks.cpp**: Memory leak detection
- **test_neural_overload.cpp**: Neural network overload testing
- **test_extreme_conditions.cpp**: Extreme neural conditions
- **test_config_edge_cases.cpp**: Configuration validation edge cases
- **test_config_validation_extremes.cpp**: Extreme configuration values
- **test_config_corruption.cpp**: Configuration corruption handling
- **test_exception_safety.cpp**: Exception safety testing
- **test_error_recovery.cpp**: Error recovery mechanisms
- **test_graceful_degradation.cpp**: Graceful degradation behavior

#### Performance & Load Tests
**Location:** `src/performance_test/`

- **test_memory_usage_profiling.cpp**: Comprehensive memory usage profiling
- **test_memory_efficiency.cpp**: Memory efficiency testing
- **test_memory_patterns.cpp**: Memory access patterns testing
- **test_neural_computation_speed.cpp**: Neural computation speed testing
- **test_neural_accuracy.cpp**: Neural network accuracy validation
- **test_neural_optimization.cpp**: Neural network optimization testing
- **test_config_parsing_speed.cpp**: Configuration parsing performance
- **test_config_validation_speed.cpp**: Configuration validation speed
- **test_config_complexity.cpp**: Configuration complexity testing
- **test_scalability_neural_networks.cpp**: Neural network scalability
- **test_scalability_simulation.cpp**: Simulation scalability
- **test_scalability_memory.cpp**: Memory scalability

## Usage Examples

### Example 1: Basic Unit Test Implementation

```cpp
// Example: Basic Brain Unit Test
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_examples {

void exampleBrainCreationTest() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Test basic properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "Brain creation test passed!" << std::endl;
}

void exampleIntegrationTest() {
    // Initialize multiple components
    auto brain = std::make_shared<nlm::Brain>(config);
    NeuralWorkingMemory memory;
    ConceptFormation conceptForm;
    
    // Connect components
    memory.initialize(brain.get());
    conceptForm.initialize(brain.get());
    
    // Test integration
    std::vector<float> sensoryData{0.5f, 0.7f, 0.9f};
    memory.store(sensoryData);
    
    size_t conceptId = conceptForm.presentExperience(sensoryData, {}, 0.8f, 0);
    
    assert(conceptId > 0); // Concept created
    assert(memory.contains(conceptForm.getConceptPrototype(conceptId)[0])); // Integration successful
}

} // namespace test_examples
```

### Example 2: Configuration Template Usage

```cpp
// Example: Template-Based Configuration
#include "config/TemplateManager.hpp"
#include <iostream>

void exampleTemplateUsage() {
    TemplateManager manager;
    
    // Create phase 3 template
    Template phase3Template = manager.createTemplate("phase3_learning", {
        {"brain.neuron_count", "100"},
        {"brain.region_count", "2"},
        {"brain.learning_rate", "0.01"},
        {"environment.width", "20.0"},
        {"environment.height", "20.0"}
    });
    
    // Store template
    manager.storeTemplate(phase3Template);
    
    // Expand template with custom values
    auto config = manager.expandTemplate("phase3_learning", {
        {"brain.neuron_count", "150"},
        {"environment.width", "25.0"}
    });
    
    // Use expanded configuration
    std::cout << "Neuron count: " << config.getInt("brain.neuron_count") << std::endl;
    std::cout << "Environment width: " << config.getFloat("environment.width") << std::endl;
}
```

### Example 3: Parallel Execution Safety

```cpp
// Example: Thread-Safe Configuration Access
#include "config/ThreadSafeConfig.hpp"
#include <thread>
#include <vector>
#include <iostream>

void exampleParallelAccess() {
    ThreadSafeConfig config;
    config.setInt("neuron_count", 100);
    config.setFloat("learning_rate", 0.01f);
    
    std::vector<std::thread> threads;
    std::atomic<int> successfulReads{0};
    std::atomic<int> errors{0};
    
    // Spawn reader threads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                try {
                    auto value = config.getInt("neuron_count");
                    if (value < 0) throw std::runtime_error("Invalid value");
                    successfulReads++;
                } catch (const std::exception&) {
                    errors++;
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) t.join();
    
    std::cout << "Successful reads: " << successfulReads.load() << std::endl;
    std::cout << "Errors: " << errors.load() << std::endl;
    std::cout << "Parallel access test passed!" << std::endl;
}
```

### Example 4: Performance Benchmarking

```cpp
// Example: Performance Benchmark
#include "test/PerformanceProfiler.hpp"
#include <iostream>
#include <chrono>

void examplePerformanceBenchmark() {
    PerformanceProfiler profiler("neural_computation_benchmark");
    
    // Create test neural network
    NeuralNetwork network = createTestNetwork({100, 200, 150, 50});
    
    // Benchmark forward pass
    std::vector<float> input(100, 0.5f);
    
    profiler.startMeasurement("forward_pass");
    for (int i = 0; i < 1000; ++i) {
        auto output = network.forwardPass(input);
    }
    auto forwardPassTime = profiler.endMeasurement("forward_pass");
    
    // Benchmark backpropagation
    std::vector<float> target(50, 0.0f);
    
    profiler.startMeasurement("backpropagation");
    for (int i = 0; i < 500; ++i) {
        network.backPropagate(input, target);
    }
    auto backpropTime = profiler.endMeasurement("backpropagation");
    
    // Generate performance report
    auto report = profiler.generateReport();
    
    std::cout << "Forward pass average time: " << forwardPassTime.avgTime << " us" << std::endl;
    std::cout << "Backpropagation average time: " << backpropTime.avgTime << " us" << std::endl;
    std::cout << "Performance benchmark completed!" << std::endl;
}
```

### Example 5: Error Handling and Recovery

```cpp
// Example: Error Handling and Recovery
#include "test/ErrorRecoveryTester.hpp"
#include <iostream>

void exampleErrorHandling() {
    ErrorRecoveryTester recoveryTester;
    
    // Test recovery from different error types
    std::vector<ErrorType> errorTypes = {
        ErrorType::MemoryAllocation,
        ErrorType::NeuralNetwork,
        ErrorType::Configuration,
        ErrorType::FileSystem
    };
    
    for (const auto& errorType : errorTypes) {
        std::cout << "Testing recovery from " << errorType << "..." << std::endl;
        
        // Simulate error
        recoveryTester.simulateError(errorType);
        
        // Attempt recovery
        recoveryTester.attemptErrorRecovery(errorType);
        
        // Verify recovery
        bool isRecovered = recoveryTester.isErrorRecovered(errorType);
        
        if (isRecovered) {
            std::cout << "  Successfully recovered from " << errorType << std::endl;
        } else {
            std::cout << "  FAILED to recover from " << errorType << std::endl;
        }
    }
}
```

## Performance Benchmarks

### Benchmark Methodology

All performance benchmarks follow these guidelines:

1. **Deterministic Testing**: Tests run with fixed random seeds to ensure repeatable results
2. **Warm-up Periods**: Systems initialize before benchmarking to eliminate cold-start effects
3. **Multiple Iterations**: Each benchmark runs multiple iterations to average results
4. **Statistical Validation**: Results use statistical analysis to ensure significance
5. **Resource Isolation**: Benchmarks run in isolated environments to prevent interference

### Sample Benchmark Results

```bash
# Neural Network Performance Benchmarks
Forward Pass (1000 iterations):
  Small Network (100-200-150-50): 850 ± 45 us/iteration
  Medium Network (200-400-300-100): 2100 ± 120 us/iteration
  Large Network (500-1000-800-300): 5800 ± 320 us/iteration

Backpropagation (500 iterations):
  Small Network: 1800 ± 80 us/iteration
  Medium Network: 4200 ± 200 us/iteration
  Large Network: 11500 ± 650 us/iteration

Memory Usage:
  Small Network: 45.2 MB peak
  Medium Network: 98.7 MB peak
  Large Network: 234.5 MB peak

Configuration Parsing:
  Simple Config: 0.8 ± 0.1 ms
  Complex Config: 2.3 ± 0.2 ms
  Nested Config: 5.1 ± 0.3 ms
```

### Performance Validation Criteria

| Metric | Minimum Requirement | Measurement Method |
|--------|-------------------|-------------------|
| Forward Pass Time | < 2ms per iteration | Timer with high precision |
| Backpropagation Time | < 3ms per iteration | Timer with high precision |
| Memory Efficiency | > 70% | Memory profiler |
| Configuration Parsing | < 10ms per config | Chronometric measurement |
| Thread Safety | Zero errors | Stress testing with 100+ threads |
| Error Recovery | < 100ms recovery time | Timing recovery mechanism |

## Best Practices for Testing

### 1. Test Organization

- **Categorize Tests**: Group tests by functionality (unit, integration, performance, etc.)
- **Isolate Tests**: Ensure tests don't interfere with each other
- **Dependency Management**: Clearly document test dependencies
- **Test Data Management**: Use deterministic test data for repeatable results

### 2. Test Quality

- **Deterministic Tests**: Use fixed seeds for random operations
- **Boundary Testing**: Test edge cases and boundary conditions
- **Error Condition Testing**: Test error paths and exception handling
- **Performance Testing**: Include performance validation in all relevant tests
- **Resource Management**: Properly clean up resources after each test

### 3. Test Documentation

- **Test Purpose**: Clearly state what each test validates
- **Implementation Details**: Document test implementation approach
- **Assumptions**: List any assumptions made during test design
- **Limitations**: Document test limitations and potential issues

### 4. Test Execution

- **Automated Testing**: Integrate tests into CI/CD pipelines
- **Regular Execution**: Run tests regularly to catch regressions
- **Performance Monitoring**: Track performance metrics over time
- **Results Analysis**: Analyze test results for trends and issues

### 5. Test Maintenance

- **Update Tests**: Update tests when system changes
- **Review Tests**: Regularly review and improve test coverage
- **Test Optimization**: Optimize tests for speed and reliability
- **Documentation Updates**: Keep documentation current with test changes

## Testing Tools and Frameworks

### Profiling Tools

- **Memory Profiler**: Tracks memory allocation, usage, and leaks
- **CPU Profiler**: Measures CPU usage and performance bottlenecks
- **Timeline Profiler**: Records execution timeline for performance analysis

### Validation Tools

- **Static Analyzers**: Detect potential issues before runtime
- **Dynamic Analyzers**: Detect issues during test execution
- **Coverage Tools**: Measure test coverage of code
- **Fuzz Testers**: Test with random inputs to find edge cases

### Performance Tools

- **Benchmark Framework**: Standardized performance measurement
- **Load Testers**: Test system under various load conditions
- **Stress Testers**: Test system under extreme conditions
- **Scalability Testers**: Test system with increasing complexity

## Test Results Interpretation

### Success Criteria

**Unit Tests:**
- All tests pass without assertion failures
- No memory leaks detected
- Consistent results across multiple runs

**Integration Tests:**
- All components integrate successfully
- No data corruption or loss
- Performance meets minimum requirements

**Advanced Features Tests:**
- Thread safety verified with concurrent access
- Template expansion produces correct results
- Configuration validation catches all invalid inputs

**Edge Case & Stress Tests:**
- System remains stable under stress conditions
- Graceful degradation occurs when necessary
- Error recovery mechanisms work correctly

**Performance & Load Tests:**
- Performance metrics meet or exceed targets
- Memory usage within limits
- Scalability requirements satisfied

### Reporting Formats

- **HTML Reports**: Visual test results with detailed metrics
- **JSON Reports**: Structured data for CI/CD integration
- **XML Reports**: JUnit-compatible format for test aggregation
- **Console Reports**: Real-time progress display

The documentation & examples section provides comprehensive guidance for implementing, running, and maintaining the NLM test suite, ensuring that all tests are properly documented, executed, and maintained throughout the system lifecycle.