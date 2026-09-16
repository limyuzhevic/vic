# Advanced Features Tests

Comprehensive tests for advanced NLM system features including configuration validation, template-based configuration, parallel execution, batch processing, and real-time monitoring.

## Test Coverage

### 1. Advanced Configuration Validation
Tests advanced configuration parsing, validation, and error handling.

**Files:**
- `test_advanced_config_validation.cpp` - Advanced validation rules
- `test_config_constraints.cpp` - Configuration constraints and validation
- `test_config_security.cpp` - Configuration security and integrity

**Key Tests:**
```cpp
// Test advanced configuration validation rules
void testAdvancedConfigurationValidation() {
    AdvancedConfigValidator validator;
    
    // Test type validation
    assertTrue(validator.validateType("neuron_count", 100, "int"));
    assertTrue(validator.validateType("connection_probability", 0.15f, "float"));
    assertTrue(validator.validateType("model_name", "resnet", "string"));
    assertTrue(validator.validateType("enabled_features", std::vector<std::string>{"attention", "prediction"}, "array"));
    
    // Test range validation
    assertTrue(validator.validateRange("neuron_count", 100, 1000, 10000));
    assertTrue(validator.validateRange("learning_rate", 0.001f, 0.1f, 0.01f));
    assertTrue(validator.validateRange("temperature", 0.1f, 2.0f, 1.0f));
    
    // Test constraint validation
    assertTrue(validator.validateConstraint("max_memory_mb", 1024, "min(512)"));
    assertTrue(validator.validateConstraint("max_memory_mb", 1024, "max(2048)"));
    assertTrue(validator.validateConstraint("sparsity_level", 0.95f, "range(0.8, 0.99)"));
    
    // Test dependency validation
    assertTrue(validator.validateDependencies({
        {"feature_attention", true},
        {"feature_prediction", true}
    }, {"feature_working_memory", true})); // Should pass: dependencies met
    
    assertFalse(validator.validateDependencies({
        {"feature_attention", true},
        {"feature_prediction", true}
    }, {"feature_working_memory", false})); // Should fail: dependency not met
}

// Test configuration security and integrity
void testConfigurationSecurity() {
    ConfigurationSecurityManager security;
    
    // Test sensitive data masking
    ConfigSection section;
    section.addValue("api_key", "secret_key_123");
    section.addValue("database_password", "db_password_456");
    section.addValue("public_setting", "public_value");
    
    auto masked = security.maskSensitiveData(section);
    assertNotEqual(masked.getValue("api_key"), "secret_key_123");
    assertNotEqual(masked.getValue("database_password"), "db_password_456");
    assertEqual(masked.getValue("public_setting"), "public_value");
    
    // Test configuration integrity
    std::string configHash = security.computeConfigHash(section);
    assertFalse(configHash.empty());
    
    // Test configuration tampering detection
    ConfigSection modified = section;
    modified.addValue("tampered_field", "should_be_detected");
    assertTrue(security.detectTampering(section, modified));
}
```

### 2. Template-Based Configuration
Tests template configuration management and expansion.

**Files:**
- `test_template_management.cpp` - Template lifecycle management
- `test_template_expansion.cpp` - Template variable expansion
- `test_template_inheritance.cpp` - Template inheritance and overriding

**Key Tests:**
```cpp
// Test template management operations
void testTemplateManagement() {
    TemplateManager manager;
    
    // Create templates
    Template phase3Template = manager.createTemplate("phase3_learning", {
        {"brain.neuron_count", "100"},
        {"brain.region_count", "2"},
        {"brain.learning_rate", "0.01"},
        {"environment.width", "20.0"},
        {"environment.height", "20.0"}
    });
    
    Template phase4Template = manager.createTemplate("phase4_cognitive", {
        {"brain.neuron_count", "200"},
        {"brain.region_count", "4"},
        {"brain.learning_rate", "0.005"},
        {"brain.use_attention", "true"},
        {"brain.use_prediction", "true"}
    });
    
    // Store templates
    assertTrue(manager.storeTemplate(phase3Template));
    assertTrue(manager.storeTemplate(phase4Template));
    
    // Retrieve templates
    auto retrievedPhase3 = manager.getTemplate("phase3_learning");
    auto retrievedPhase4 = manager.getTemplate("phase4_cognitive");
    
    assertNotNull(retrievedPhase3);
    assertNotNull(retrievedPhase4);
    assertEqual(retrievedPhase3->name, "phase3_learning");
    
    // List all templates
    auto templates = manager.listTemplates();
    assertGreaterThan(templates.size(), 1u);
}

// Test template inheritance and inheritance hierarchy
void testTemplateInheritance() {
    TemplateManager manager;
    
    // Create base template
    Template baseTemplate = manager.createTemplate("base", {
        {"brain.neuron_count", "100"},
        {"brain.learning_rate", "0.01"},
        {"environment.width", "10.0"}
    });
    
    // Create child template with inheritance
    Template childTemplate = manager.createTemplate("phase3", {
        {"brain.region_count", "2"},
        {"brain.use_attention", "true"}
    });
    
    // Set inheritance relationship
    assertTrue(manager.setTemplateInheritance("phase3", "base"));
    
    // Expand child template (should inherit from base)
    auto expanded = manager.expandTemplate("phase3", {
        {"brain.neuron_count", "150"} // Override base value
    });
    
    assertEqual(expanded.getInt("brain.neuron_count"), 150); // Overridden
    assertEqual(expanded.getInt("brain.region_count"), 2);    // Child value
    assertEqual(expanded.getFloat("environment.width"), 10.0f); // Inherited
    assertEqual(expanded.getBool("brain.use_attention"), true);   // Child value
}
```

### 3. Parallel Execution Tests
Tests thread safety and parallel execution capabilities.

**Files:**
- `test_parallel_safety.cpp` - Thread safety and race condition testing
- `test_parallel_execution.cpp` - Parallel execution performance
- `test_concurrent_access.cpp` - Concurrent access to shared resources

**Key Tests:**
```cpp
// Test thread safety of configuration access
void testParallelConfigurationAccess() {
    ThreadSafeConfig config;
    
    // Initialize config
    config.setInt("neuron_count", 100);
    config.setFloat("learning_rate", 0.01f);
    
    std::atomic<int> successfulReads{0};
    std::atomic<int> successfulWrites{0};
    std::atomic<int> errors{0};
    
    // Multiple reader threads
    std::vector<std::thread> readers;
    for (int i = 0; i < 10; ++i) {
        readers.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                try {
                    auto value = config.getInt("neuron_count");
                    assertGreaterThanOrEqual(value, 0);
                    successfulReads++;
                } catch (const std::exception&) {
                    errors++;
                }
            }
        });
    }
    
    // Multiple writer threads
    std::vector<std::thread> writers;
    for (int i = 0; i < 5; ++i) {
        writers.emplace_back([&]() {
            for (int j = 0; j < 50; ++j) {
                try {
                    config.setInt("neuron_count", 100 + j);
                    successfulWrites++;
                } catch (const std::exception&) {
                    errors++;
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& t : readers) t.join();
    for (auto& t : writers) t.join();
    
    // Verify no errors occurred
    assertEqual(errors.load(), 0);
    assertGreaterThan(successfulReads.load(), 0);
    assertGreaterThan(successfulWrites.load(), 0);
}

// Test parallel neural network operations
void testParallelNeuralOperations() {
    ParallelNeuralProcessor processor;
    
    // Create test data
    std::vector<std::vector<float>> inputData(100, std::vector<float>(50, 0.0f));
    std::vector<std::vector<float>> expectedOutput(100, std::vector<float>(10, 0.0f));
    
    // Initialize with random data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    for (auto& row : inputData) {
        for (auto& val : row) val = dis(gen);
    }
    
    for (auto& row : expectedOutput) {
        for (auto& val : row) val = dis(gen);
    }
    
    // Process in parallel
    auto startTime = std::chrono::high_resolution_clock::now();
    auto results = processor.processInParallel(inputData);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Verify results
    assertEqual(results.size(), inputData.size());
    
    // Calculate processing time
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Verify performance meets requirements
    float processingTimePerInput = duration.count() / inputData.size();
    assertLessThan(processingTimePerInput, 10.0f); // Less than 10ms per input
}
```

### 4. Batch Processing Tests
Tests batch processing capabilities and efficiency.

**Files:**
- `test_batch_processing.cpp` - Batch processing operations
- `test_batch_memory_management.cpp` - Batch memory management
- `test_batch_performance.cpp` - Batch performance optimization

**Key Tests:**
```cpp
// Test batch configuration processing
void testBatchConfigurationProcessing() {
    BatchConfigProcessor processor;
    
    // Create batch of configurations
    std::vector<Config> configBatch;
    for (int i = 0; i < 1000; ++i) {
        Config config;
        config.setInt("neuron_count", 50 + (i % 100));
        config.setFloat("learning_rate", 0.001f * ((i % 10) + 1));
        config.setBool("use_attention", i % 3 == 0);
        configBatch.push_back(config);
    }
    
    // Process batch
    auto startTime = std::chrono::high_resolution_clock::now();
    auto results = processor.validateAndProcessBatch(configBatch);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Verify results
    assertEqual(results.size(), configBatch.size());
    
    // Check validation results
    for (size_t i = 0; i < results.size(); ++i) {
        if (configBatch[i].getInt("neuron_count") < 50 || 
            configBatch[i].getInt("neuron_count") > 149) {
            assertFalse(results[i].isValid);
        } else {
            assertTrue(results[i].isValid);
        }
    }
    
    // Calculate batch processing time
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    float avgTimePerConfig = duration.count() / configBatch.size();
    assertLessThan(avgTimePerConfig, 1.0f); // Less than 1ms per config
}

// Test batch neural network training
void testBatchNeuralTraining() {
    BatchNeuralTrainer trainer;
    
    // Create training data batches
    std::vector<TrainingExample> trainingBatch;
    for (int i = 0; i < 1000; ++i) {
        TrainingExample example;
        example.input = std::vector<float>(100, 0.0f);
        example.target = std::vector<float>(10, 0.0f);
        
        // Fill with random data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        for (auto& val : example.input) val = dis(gen);
        for (auto& val : example.target) val = dis(gen);
        
        trainingBatch.push_back(example);
    }
    
    // Train model with batch
    auto startTime = std::chrono::high_resolution_clock::now();
    trainer.trainBatch(trainingBatch);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Verify training completed
    assertTrue(trainer.isTrainingComplete());
    
    // Calculate training time
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    float examplesPerSecond = trainingBatch.size() / duration.count();
    assertGreaterThan(examplesPerSecond, 100.0f); // More than 100 examples per second
}
```

### 5. Real-time Monitoring Tests
Tests real-time monitoring and system observability features.

**Files:**
- `test_monitoring_system.cpp` - Monitoring system functionality
- `test_performance_monitoring.cpp` - Performance monitoring
- `test_alert_system.cpp` - Alert and notification system

**Key Tests:**
```cpp
// Test monitoring system data collection
void testMonitoringDataCollection() {
    RealTimeMonitor monitor;
    
    // Start monitoring
    monitor.startMonitoring();
    
    // Collect metrics for a period
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Get collected metrics
    auto metrics = monitor.getMetrics();
    
    // Verify metrics collection
    assertFalse(metrics.empty());
    
    // Check for expected metric types
    bool hasCpuUsage = false;
    bool hasMemoryUsage = false;
    bool hasNeuralActivity = false;
    
    for (const auto& metric : metrics) {
        if (metric.name == "cpu_usage") hasCpuUsage = true;
        if (metric.name == "memory_usage") hasMemoryUsage = true;
        if (metric.name == "neural_activity") hasNeuralActivity = true;
    }
    
    assertTrue(hasCpuUsage);
    assertTrue(hasMemoryUsage);
    assertTrue(hasNeuralActivity);
    
    // Stop monitoring
    monitor.stopMonitoring();
}

// Test performance alerting system
void testPerformanceAlerting() {
    PerformanceAlertManager alertManager;
    
    // Configure alert thresholds
    AlertThreshold cpuThreshold{AlertType::Performance, "cpu_usage", 0.9f, AlertSeverity::Warning};
    AlertThreshold memoryThreshold{AlertType::Memory, "memory_usage", 0.8f, AlertSeverity::Critical};
    AlertThreshold neuralThreshold{AlertType::Neural, "neural_activity", 1.5f, AlertSeverity::Warning};
    
    alertManager.addThreshold(cpuThreshold);
    alertManager.addThreshold(memoryThreshold);
    alertManager.addThreshold(neuralThreshold);
    
    // Test alert generation
    PerformanceData metrics;
    metrics.cpuUsage = 0.95f;    // Should trigger CPU alert
    metrics.memoryUsage = 0.3f;  // Should not trigger memory alert
    metrics.neuralActivity = 1.2f; // Should not trigger neural alert
    
    auto alerts = alertManager.checkAlerts(metrics);
    
    assertEqual(alerts.size(), 1);
    assertEqual(alerts[0].name, "cpu_usage");
    assertEqual(alerts[0].severity, AlertSeverity::Warning);
}
```

## Test Framework Integration

Advanced features tests integrate with specialized testing frameworks:

```cpp
// Advanced test base class
class AdvancedTestBase {
protected:
    void setupAdvancedEnvironment() {
        // Configure advanced testing environment
        testEnv_.enableParallelExecution();
        testEnv_.enableMemoryProfiling();
        testEnv_.enablePerformanceMonitoring();
        testEnv_.setLoadLevel(TestLoadLevel::High);
    }
    
    template<typename T>
    void assertPerformanceWithinLimit(T actual, T limit, const std::string& metricName) {
        if (actual > limit) {
            throw PerformanceAssertionError(
                "Performance limit exceeded for " + metricName,
                actual, limit
            );
        }
    }
    
    void cleanupAdvancedEnvironment() {
        testEnv_.disableAllFeatures();
        testEnv_.cleanupResources();
    }
};
```

## Test Execution

### Running Advanced Features Tests

```bash
# Build advanced features tests
mkdir -p build && cd build
cmake -DCMAKE_BUILD_ADVANCED_TESTS=ON ..
make

# Run all advanced features tests
./nlm_test_suite --suite=advanced

# Run specific advanced test category
./nlm_test_suite --suite=config_validation
./nlm_test_suite --suite=template_configuration
./nlm_test_suite --suite=parallel_execution
./nlm_test_suite --suite=batch_processing
./nlm_test_suite --suite=real_time_monitoring
```

### Advanced Test Configuration

```bash
# Advanced features test options
./nlm_test_suite --suite=advanced \
    --config-validation-only \
    --enable-parallel-testing \
    --enable-batch-testing \
    --enable-monitoring \
    --performance-threshold=0.9 \
    --memory-limit=16 \
    --iteration-count=1000
```

The advanced features test suite validates that all NLM system improvements function correctly in complex, real-world scenarios, ensuring robustness, performance, and reliability of advanced system capabilities.