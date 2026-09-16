# Performance & Load Tests

Comprehensive performance and load testing for the NLM system, validating performance improvements and scalability.

## Test Coverage

### 1. Memory Usage Profiling
Tests memory usage patterns, allocation efficiency, and memory management.

**Files:**
- `test_memory_usage_profiling.cpp` - Comprehensive memory usage profiling
- `test_memory_efficiency.cpp` - Memory efficiency testing
- `test_memory_patterns.cpp` - Memory access patterns and optimization

**Key Tests:**
```cpp
// Test comprehensive memory usage profiling
void testMemoryUsageProfiling() {
    MemoryProfiler profiler;
    
    // Profile system startup
    profiler.startProfile("system_startup");
    System system(config);
    profiler.endProfile("system_startup");
    
    // Profile steady state operation
    profiler.startProfile("steady_state");
    for (int step = 0; step < 10000; ++step) {
        system.step(step);
    }
    profiler.endProfile("steady_state");
    
    // Profile shutdown
    profiler.startProfile("shutdown");
    system.shutdown();
    profiler.endProfile("shutdown");
    
    // Generate memory profile report
    auto report = profiler.generateReport();
    
    // Analyze memory usage patterns
    assertMemoryUsageWithinLimits(report.getPeakMemoryUsage(), 1024 * 1024 * 1024); // 1GB limit
    assertMemoryEfficiency(report.getMemoryEfficiency(), 0.8f); // 80% efficiency minimum
    
    // Verify no memory leaks
    assertNoMemoryLeaks(report.getLeakReport());
}

// Test memory efficiency
void testMemoryEfficiency() {
    MemoryEfficiencyTester efficiencyTester;
    
    // Test with different data structures
    std::vector<std::string> dataStructures = {
        "std::vector", "std::deque", "std::list", 
        "std::unordered_map", "std::map", "custom_neural_structures"
    };
    
    for (const auto& structure : dataStructures) {
        // Create test dataset
        auto dataset = efficiencyTester.createTestDataset(structure, 10000);
        
        // Measure memory usage
        auto memoryUsage = efficiencyTester.measureMemoryUsage(dataset);
        
        // Calculate efficiency metrics
        float memoryPerElement = memoryUsage / dataset.size();
        float allocationEfficiency = efficiencyTester.calculateAllocationEfficiency(dataset);
        
        // Verify efficiency targets
        assertLessThan(memoryPerElement, 100.0f, "memory_per_element"); // < 100 bytes per element
        assertGreaterThan(allocationEfficiency, 0.7f, "allocation_efficiency"); // > 70% efficiency
    }
}
```

### 2. Neural Computation Performance
Tests neural network computation speed, accuracy, and optimization.

**Files:**
- `test_neural_computation_speed.cpp` - Neural computation speed testing
- `test_neural_accuracy.cpp` - Neural network accuracy validation
- `test_neural_optimization.cpp` - Neural network optimization testing

**Key Tests:**
```cpp
// Test neural computation speed
void testNeuralComputationSpeed() {
    NeuralSpeedTester speedTester;
    
    // Create test neural network
    NeuralNetwork network = speedTester.createTestNetwork(
        {100, 200, 150, 50}, // Layer sizes
        1000 // Number of neurons
    );
    
    // Test forward pass speed
    std::vector<float> input(100, 0.5f);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        auto output = network.forwardPass(input);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    float avgTimePerPass = duration.count() / 1000.0f;
    
    // Verify speed targets
    assertLessThan(avgTimePerPass, 1000.0f, "avg_time_per_forward_pass_us"); // < 1ms per pass
    
    // Test backpropagation speed
    std::vector<float> target(50, 0.0f);
    
    startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 500; ++i) {
        network.backPropagate(input, target);
    }
    endTime = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    float avgTimePerBackprop = duration.count() / 500.0f;
    
    assertLessThan(avgTimePerBackprop, 2000.0f, "avg_time_per_backprop_us"); // < 2ms per backprop
}

// Test neural network accuracy
void testNeuralAccuracy() {
    NeuralAccuracyTester accuracyTester;
    
    // Create training dataset
    std::vector<TrainingExample> trainingSet = 
        accuracyTester.generateTrainingData(10000);
    
    // Create neural network
    NeuralNetwork network = accuracyTester.createNetwork(
        {100, 200, 150, 50}, 10 // Architecture and 10 output classes
    );
    
    // Train network
    auto startTime = std::chrono::high_resolution_clock::now();
    network.train(trainingSet, 100, 0.01f);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto trainingTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Test accuracy on validation set
    auto validationSet = accuracyTester.createValidationSet(2000);
    auto accuracy = accuracyTester.calculateAccuracy(network, validationSet);
    
    // Verify accuracy targets
    assertGreaterThan(accuracy, 0.85f, "classification_accuracy"); // > 85% accuracy
    assertLessThan(trainingTime.count() / trainingSet.size(), 0.1f, "time_per_training_example_s"); // < 0.1s per example
}
```

### 3. Configuration Parsing Speed
Tests configuration parsing efficiency and validation speed.

**Files:**
- `test_config_parsing_speed.cpp` - Configuration parsing performance
- `test_config_validation_speed.cpp` - Configuration validation speed
- `test_config_complexity.cpp` - Configuration complexity testing

**Key Tests:**
```cpp
// Test configuration parsing speed
void testConfigurationParsingSpeed() {
    ConfigSpeedTester speedTester;
    
    // Create test configuration files
    std::vector<std::string> configFiles;
    for (int i = 0; i < 100; ++i) {
        configFiles.push_back(speedTester.createTestConfigFile(i));
    }
    
    // Test parsing speed
    auto startTime = std::chrono::high_resolution_clock::now();
    for (const auto& file : configFiles) {
        auto config = speedTester.parseConfigFile(file);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    float avgTimePerConfig = duration.count() / configFiles.size();
    
    // Verify speed targets
    assertLessThan(avgTimePerConfig, 5.0f, "avg_time_per_config_ms"); // < 5ms per config
    
    // Clean up
    for (const auto& file : configFiles) {
        std::remove(file.c_str());
    }
}

// Test configuration validation speed
void testConfigurationValidationSpeed() {
    ConfigValidationSpeedTester validationTester;
    
    // Create test configurations
    std::vector<Config> testConfigs;
    for (int i = 0; i < 1000; ++i) {
        Config config;
        config.setInt("param1", i % 100);
        config.setFloat("param2", 0.1f * (i % 10));
        config.setBool("param3", i % 2 == 0);
        testConfigs.push_back(config);
    }
    
    // Test validation speed
    auto startTime = std::chrono::high_resolution_clock::now();
    for (const auto& config : testConfigs) {
        bool isValid = validationTester.validateConfig(config);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    float avgTimePerValidation = duration.count() / testConfigs.size();
    
    // Verify speed targets
    assertLessThan(avgTimePerValidation, 0.1f, "avg_time_per_validation_ms"); // < 0.1ms per validation
}
```

### 4. Simulation Scalability
Tests system scalability with increasing simulation size and complexity.

**Files:**
- `test_scalability_neural_networks.cpp` - Neural network scalability
- `test_scalability_simulation.cpp` - Simulation scalability
- `test_scalability_memory.cpp` - Memory scalability

**Key Tests:**
```cpp
// Test neural network scalability
void testNeuralNetworkScalability() {
    NeuralScalabilityTester scalabilityTester;
    
    // Test with different network sizes
    std::vector<std::vector<size_t>> networkSizes = {
        {50, 100, 50},    // Small
        {200, 400, 200},  // Medium
        {500, 1000, 500}, // Large
        {1000, 2000, 1000} // Very large
    };
    
    for (const auto& sizes : networkSizes) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto network = scalabilityTester.createNetwork(sizes);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Calculate scalability metrics
        size_t totalNeurons = std::accumulate(sizes.begin(), sizes.end(), 0);
        float neuronsPerMs = totalNeurons / duration.count();
        
        // Verify scalability targets
        assertGreaterThan(neuronsPerMs, 100.0f, "neurons_per_ms"); // > 100 neurons per ms
        
        // Test with increasing input sizes
        for (size_t inputSize = 10; inputSize <= 1000; inputSize *= 10) {
            std::vector<float> input(inputSize, 0.5f);
            
            startTime = std::chrono::high_resolution_clock::now();
            auto output = network.forwardPass(input);
            endTime = std::chrono::high_resolution_clock::now();
            
            duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            assertLessThan(duration.count(), 100.0f, "forward_pass_time_us"); // < 100us
        }
    }
}

// Test simulation scalability
void testSimulationScalability() {
    SimulationScalabilityTester scalabilityTester;
    
    // Test with different simulation sizes
    std::vector<size_t> simulationSizes = {100, 500, 1000, 5000, 10000};
    
    for (size_t size : simulationSizes) {
        // Create simulation environment
        auto environment = scalabilityTester.createSimulationEnvironment(size);
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < size; ++step) {
            environment.step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        // Calculate scalability metrics
        float stepsPerSecond = size / duration.count();
        
        // Verify scalability targets
        assertGreaterThan(stepsPerSecond, 100.0f, "steps_per_second"); // > 100 steps per second
        
        // Test memory usage scalability
        auto memoryUsage = scalabilityTester.measureMemoryUsage(environment);
        float memoryPerStep = memoryUsage / size;
        
        assertLessThan(memoryPerStep, 1000.0f, "memory_per_step_bytes"); // < 1KB per step
    }
}
```

## Test Framework Integration

Performance and load tests integrate with advanced profiling and benchmarking frameworks:

```cpp
// Performance test base class
class PerformanceTestBase {
protected:
    void setupPerformanceEnvironment() {
        testEnv_.enablePerformanceProfiling();
        testEnv_.enableMemoryProfiling();
        testEnv_.setPerformancePrecision(Precision::High);
        testEnv_.enableBenchmarking();
    }
    
    template<typename T>
    void assertPerformanceTarget(T actual, T target, 
                                 const std::string& metricName,
                                 const std::string& testName) {
        if (actual > target) {
            throw PerformanceTargetExceededError(
                "Performance target exceeded for " + testName + "." + metricName + ": " +
                std::to_string(actual) + " > " + std::to_string(target)
            );
        }
    }
    
    PerformanceMetrics capturePerformanceMetrics() {
        PerformanceMetrics metrics;
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        metrics.cpuUsage = testEnv_.getCPUUsage();
        metrics.memoryUsage = testEnv_.getMemoryUsage();
        metrics.diskIO = testEnv_.getDiskIO();
        return metrics;
    }
};
```

## Test Execution

### Running Performance & Load Tests

```bash
# Build performance and load tests
mkdir -p build && cd build
cmake -DCMAKE_BUILD_PERFORMANCE_TESTS=ON -DCMAKE_BUILD_LOAD_TESTS=ON ..
make

# Run all performance and load tests
./nlm_test_suite --suite=performance --suite=load

# Run specific test categories
./nlm_test_suite --suite=memory_usage
./nlm_test_suite --suite=neural_computation
./nlm_test_suite --suite=config_parsing
./nlm_test_suite --suite=simulation_scalability
```

### Performance Test Configuration

```bash
# Performance and load test options
./nlm_test_suite --suite=performance --suite=load \
    --enable-memory-profiling \
    --enable-neural-benchmarking \
    --enable-config-parsing-tests \
    --enable-simulation-scaling \
    --performance-threshold=0.95 \
    --memory-limit=8 \
    --iteration-count=1000 \
    --enable-continuous-monitoring
```

The performance and load test suite validates that all NLM system improvements meet performance targets and scale effectively with increasing complexity, ensuring optimal performance in production environments.