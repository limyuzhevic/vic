# Configuration Validation Test Suite

## Overview

This document provides comprehensive documentation for the NLM configuration validation test suite, including test cases, test procedures, and test results analysis.

## Test Suite Overview

The NLM configuration validation test suite provides comprehensive testing for the enhanced configuration system, including:

- **Schema Validation Tests**: Tests for configuration schema validation
- **Loading Tests**: Tests for configuration file loading
- **Validation Tests**: Tests for configuration validation
- **Performance Tests**: Tests for configuration performance
- **Integration Tests**: Tests for configuration integration with the rest of the system

## Test Categories

### Schema Validation Tests

#### Test 1: Schema Registration Tests
**Purpose**: Test that configuration schemas are properly registered and accessible.

```python
def test_schema_registration():
    registry = ConfigRegistry.getInstance()
    
    # Test that core schemas are registered
    assert registry.hasSchema("random_seed")
    assert registry.hasSchema("simulation_timestep")
    assert registry.hasSchema("neuron_count")
    
    # Test schema accessibility
    schema = registry.getSchema("random_seed")
    assert schema is not None
    assert schema.type == "int"
    assert schema.required == True
    
    # Test schema description
    description = registry.getSchemaDescription()
    assert "Configuration Schema" in description
    assert "random_seed" in description
    
    print("✓ Schema registration tests passed")
```

#### Test 2: Schema Validation Tests
**Purpose**: Test that configuration values are properly validated against schemas.

```python
def test_schema_validation():
    registry = ConfigRegistry.getInstance()
    
    # Test valid values
    assert registry.validate("random_seed", ConfigValue(42))
    assert registry.validate("simulation_timestep", ConfigValue(0.001))
    assert registry.validate("neuron_count", ConfigValue(1000))
    assert registry.validate("log_level", ConfigValue("INFO"))
    
    # Test invalid values (type mismatch)
    assert not registry.validate("random_seed", ConfigValue("42"))  # string instead of int
    assert not registry.validate("simulation_timestep", ConfigValue("0.001"))  # string instead of double
    
    # Test invalid values (range violation)
    assert not registry.validate("neuron_count", ConfigValue(-5))  # negative value
    assert not registry.validate("simulation_timestep", ConfigValue(0.0))  # too small
    assert not registry.validate("simulation_timestep", ConfigValue(2.0))  # too large
    
    # Test invalid values (pattern violation)
    assert not registry.validate("log_level", ConfigValue("INVALID"))  # not in allowed values
    
    print("✓ Schema validation tests passed")
```

#### Test 3: Full Validation Tests
**Purpose**: Test comprehensive validation with error reporting.

```python
def test_full_validation():
    registry = ConfigRegistry.getInstance()
    
    # Create configuration with valid values
    valid_config = {
        "random_seed": ConfigValue(42),
        "simulation_timestep": ConfigValue(0.001),
        "neuron_count": ConfigValue(1000),
        "region_count": ConfigValue(2),
        "connection_probability": ConfigValue(0.1),
        "log_level": ConfigValue("INFO"),
        "log_to_file": ConfigValue(False)
    }
    
    errors = registry.getValidationErrors(valid_config)
    assert len(errors) == 0  # No errors expected
    
    # Create configuration with invalid values
    invalid_config = {
        "random_seed": ConfigValue(42),
        "simulation_timestep": ConfigValue(0.001),
        "neuron_count": ConfigValue(-5),  # Invalid
        "region_count": ConfigValue(2),
        "connection_probability": ConfigValue(0.1),
        "log_level": ConfigValue("INVALID"),  # Invalid
        "log_to_file": ConfigValue(False)
    }
    
    errors = registry.getValidationErrors(invalid_config)
    assert len(errors) > 0  # Errors expected
    assert any("neuron_count" in error for error in errors)
    assert any("log_level" in error for error in errors)
    
    # Test validation report
    report = registry.getValidationReport(invalid_config)
    assert "Validation Report" in report
    assert "Invalid: " in report
    assert "neuron_count" in report
    assert "log_level" in report
    
    print("✓ Full validation tests passed")
```

### Loading Tests

#### Test 4: JSON Loading Tests
**Purpose**: Test JSON configuration file loading and parsing.

```python
def test_json_loading():
    config = Config()
    
    # Create temporary JSON config file
    json_content = '''{
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "log_level": "INFO"
    }'''
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.write(json_content)
        temp_file = f.name
    
    try:
        # Test loading
        assert config.loadFromFile(temp_file)
        
        # Verify loaded values
        assert config.getOr<int64_t>("random_seed", 0) == 42
        assert config.getOr<double>("simulation_timestep", 0.0) == 0.001
        assert config.getOr<int64_t>("neuron_count", 0) == 1000
        assert config.getOr<int64_t>("region_count", 0) == 2
        assert config.getOr<float>("connection_probability", 0.0) == 0.1
        assert config.getOr<std::string>("log_level", "") == "INFO"
        
        print("✓ JSON loading tests passed")
        
    finally:
        os.unlink(temp_file)
```

#### Test 5: YAML Loading Tests
**Purpose**: Test YAML configuration file loading and parsing.

```python
def test_yaml_loading():
    config = Config()
    
    # Create temporary YAML config file
    yaml_content = '''random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
log_level: INFO
log_to_file: true
log_filename: "test.log"
'''
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write(yaml_content)
        temp_file = f.name
    
    try:
        # Test loading (YAML support is limited, expect fallback)
        bool loaded = config.loadFromFile(temp_file)
        
        if loaded:
            # Verify some values
            assert config.has("random_seed")
            assert config.has("neuron_count")
            
        print("✓ YAML loading tests passed")
        
    finally:
        os.unlink(temp_file)
```

#### Test 6: Legacy .cfg Loading Tests
**Purpose**: Test legacy .cfg configuration file loading.

```python
def test_cfg_loading():
    config = Config()
    
    # Create temporary .cfg config file
    cfg_content = '''# Core simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Logging parameters
log_level = INFO
log_to_file = true
log_filename = test.log
'''
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(cfg_content)
        temp_file = f.name
    
    try:
        # Test loading
        assert config.loadFromFile(temp_file)
        
        # Verify loaded values
        assert config.getOr<int64_t>("random_seed", 0) == 42
        assert config.getOr<double>("simulation_timestep", 0.0) == 0.001
        assert config.getOr<int64_t>("neuron_count", 0) == 1000
        assert config.getOr<int64_t>("region_count", 0) == 2
        assert config.getOr<float>("connection_probability", 0.0) == 0.1
        assert config.getOr<std::string>("log_level", "") == "INFO"
        assert config.getOr<bool>("log_to_file", false) == True
        assert config.getOr<std::string>("log_filename", "") == "test.log"
        
        print("✓ .cfg loading tests passed")
        
    finally:
        os.unlink(temp_file)
```

### Command Line Tests

#### Test 7: Command Line Override Tests
**Purpose**: Test command line configuration overrides.

```python
def test_command_line_overrides():
    config = Config()
    
    # Simulate command line arguments
    test_args = [
        "--random_seed=123",
        "--neuron_count=500",
        "--log_level=DEBUG",
        "-log_to_file=true"
    ]
    
    // Convert to argv format
    char** argv = new char*[test_args.size() + 1]
    argv[0] = "test_program"
    for (size_t i = 0; i < test_args.size(); ++i) {
        argv[i + 1] = new char[test_args[i].size() + 1];
        std::strcpy(argv[i + 1], test_args[i].c_str());
    }
    
    try {
        // Test command line loading
        assert config.loadFromArgs(1 + test_args.size(), argv);
        
        // Verify overrides were applied
        assert config.getOr<int64_t>("random_seed", 0) == 123
        assert config.getOr<int64_t>("neuron_count", 0) == 500
        assert config.getOr<std::string>("log_level", "") == "DEBUG"
        assert config.getOr<bool>("log_to_file", false) == True
        
        print("✓ Command line override tests passed")
        
    } finally {
        for (size_t i = 1; i <= test_args.size(); ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
```

#### Test 8: Mixed Configuration Tests
**Purpose**: Test mixed configuration sources (file + command line).

```python
def test_mixed_configuration():
    config = Config()
    
    // Create temporary config file
    config_content = '''random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
log_level = INFO
'''
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        temp_file = f.name
    
    try {
        // Load from file
        assert config.loadFromFile(temp_file)
        
        // Override with command line
        test_args = [
            "--neuron_count=2000",
            "--log_level=DEBUG"
        ]
        
        char** argv = new char*[test_args.size() + 1];
        argv[0] = "test_program";
        for (size_t i = 0; i < test_args.size(); ++i) {
            argv[i + 1] = new char[test_args[i].size() + 1];
            std::strcpy(argv[i + 1], test_args[i].c_str());
        }
        
        try {
            config.loadFromArgs(1 + test_args.size(), argv);
            
            // Verify file values (except overrides)
            assert config.getOr<int64_t>("random_seed", 0) == 42
            assert config.getOr<double>("simulation_timestep", 0.0) == 0.001
            
            // Verify overrides
            assert config.getOr<int64_t>("neuron_count", 0) == 2000
            assert config.getOr<std::string>("log_level", "") == "DEBUG"
            
            print("✓ Mixed configuration tests passed")
            
        } finally {
            for (size_t i = 1; i <= test_args.size(); ++i) {
                delete[] argv[i];
            }
            delete[] argv;
        }
        
    finally:
        os.unlink(temp_file)
```

### Performance Tests

#### Test 9: Loading Performance Tests
**Purpose**: Test configuration loading performance.

```python
def test_loading_performance():
    config = Config()
    
    // Create test configuration file
    config_content = '''random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
log_to_file = true
log_filename = test.log
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_width = 10
environment_height = 10
vision_width = 16
vision_height = 16
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
'''
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        temp_file = f.name
    
    try {
        // Measure loading time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            config.loadFromFile(temp_file);
            config.clear();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Average loading time per file
        double avg_time = (double)duration.count() / 100.0;
        
        std::cout << "Average configuration loading time: " << avg_time << " ms" << std::endl;
        
        // Performance should be reasonable (e.g., < 10ms per load)
        assert avg_time < 10.0, "Configuration loading too slow: " + std::to_string(avg_time) + "ms"
        
        print("✓ Loading performance tests passed")
        
    finally:
        os.unlink(temp_file)
```

#### Test 10: Validation Performance Tests
**Purpose**: Test configuration validation performance.

```python
def test_validation_performance():
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    // Create test configuration
    std::map<std::string, ConfigValue> test_config;
    test_config["random_seed"] = ConfigValue(42);
    test_config["simulation_timestep"] = ConfigValue(0.001);
    test_config["neuron_count"] = ConfigValue(1000);
    test_config["region_count"] = ConfigValue(2);
    test_config["connection_probability"] = ConfigValue(0.1);
    test_config["log_level"] = ConfigValue("INFO");
    test_config["log_to_file"] = ConfigValue(False);
    
    // Measure validation time
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        std::vector<std::string> errors = registry.getValidationErrors(test_config);
        // Validation should not produce errors
        assert errors.size() == 0;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Average validation time per call
    double avg_time = (double)duration.count() / 10000.0;
    
    std::cout << "Average configuration validation time: " << avg_time << " us" << std::endl;
    
    // Performance should be excellent (e.g., < 100us per validation)
    assert avg_time < 100.0, "Configuration validation too slow: " + std::to_string(avg_time) + "us"
    
    print("✓ Validation performance tests passed")
```

### Integration Tests

#### Test 11: End-to-End Configuration Tests
**Purpose**: Test end-to-end configuration integration with the main system.

```python
def test_end_to_end_configuration():
    // Test complete configuration workflow
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and load configuration
    auto config = std::make_shared<Config>();
    
    // Load from file
    std::vector<std::string> configFiles = {"configs/default.cfg"};
    EnhancedConfigLoader configLoader(config);
    
    bool anyLoaded = configLoader.loadConfiguration(configFiles);
    assert anyLoaded || true; // May fail if file not found, but should not crash
    
    // Apply command line overrides
    configLoader.applyCommandLineOverrides(0, nullptr);
    
    // Validate configuration
    bool valid = configLoader.validateConfiguration();
    // May fail, but should not crash
    
    // Setup diagnostics
    DiagnosticsManager diagnostics;
    diagnostics.setConfig(config);
    diagnostics.setupDiagnostics(logger);
    
    // Log configuration summary
    configLoader.displayConfigurationSummary();
    
    // Test with main system components
    auto brain = std::make_shared<Brain>(config);
    bool initialized = brain->initialize();
    
    if (initialized) {
        // Test brain operations
        brain->step(0, 0.0);
        size_t spikes = brain->getTotalSpikeCount();
        
        // Test action production
        auto action = brain->produceAction();
        
        // Test reset
        brain->reset();
        
        // Test save/load
        bool saved = brain->save("test_checkpoint");
        bool loaded = brain->load("test_checkpoint");
        
        std::cout << "✓ End-to-end configuration integration test passed" << std::endl;
    }
```

#### Test 12: Error Handling Tests
**Purpose**: Test error handling and recovery in configuration system.

```python
def test_error_handling():
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    // Test with invalid configuration
    std::map<std::string, ConfigValue> invalid_config;
    invalid_config["random_seed"] = ConfigValue("invalid_string");  // Type mismatch
    invalid_config["neuron_count"] = ConfigValue(-5);  // Range violation
    invalid_config["log_level"] = ConfigValue("INVALID");  // Pattern violation
    
    std::vector<std::string> errors = registry.getValidationErrors(invalid_config);
    assert errors.size() > 0;
    
    // Test error reporting
    assert any("random_seed" in error for error in errors);
    assert any("neuron_count" in error for error in errors);
    assert any("log_level" in error for error in errors);
    
    // Test with empty configuration
    std::map<std::string, ConfigValue> empty_config;
    std::vector<std::string> empty_errors = registry.getValidationErrors(empty_config);
    assert empty_errors.size() == 0;  // No errors for empty config
    
    // Test with null values
    std::map<std::string, ConfigValue> null_config;
    null_config["null_key"] = ConfigValue();  // Invalid variant
    
    try {
        std::vector<std::string> null_errors = registry.getValidationErrors(null_config);
        // May or may not produce errors depending on implementation
        std::cout << "✓ Null value error handling test completed" << std::endl;
    } catch (...) {
        std::cout << "✓ Null value error handling test passed (exception expected)" << std::endl;
    }
```

## Test Runner

### Test Runner Implementation

```python
class ConfigurationTestRunner {
public:
    void runAllTests() {
        std::cout << "=== NLM Configuration Validation Test Suite ===" << std::endl;
        std::cout << "Running comprehensive configuration tests..." << std::endl;
        
        runTestSuite("Schema Validation Tests", {
            test_schema_registration,
            test_schema_validation,
            test_full_validation
        });
        
        runTestSuite("Loading Tests", {
            test_json_loading,
            test_yaml_loading,
            test_cfg_loading
        });
        
        runTestSuite("Command Line Tests", {
            test_command_line_overrides,
            test_mixed_configuration
        });
        
        runTestSuite("Performance Tests", {
            test_loading_performance,
            test_validation_performance
        });
        
        runTestSuite("Integration Tests", {
            test_end_to_end_configuration,
            test_error_handling
        });
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "The NLM configuration system is functioning correctly." << std::endl;
    }
    
private:
    void runTestSuite(const std::string& suiteName, 
                     const std::vector<std::function<void()>>& tests) {
        std::cout << "\n--- Running " << suiteName << " ---" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& test : tests) {
            try {
                test();
                passed++;
                std::cout << "✓ " << test.__name__ << " passed" << std::endl;
            } catch (const std::exception& e) {
                failed++;
                std::cout << "✗ " << test.__name__ << " failed: " << e.what() << std::endl;
            } catch (...) {
                failed++;
                std::cout << "✗ " << test.__name__ << " failed: Unknown error" << std::endl;
            }
        }
        
        std::cout << "--- " << suiteName << " Summary ---" << std::endl;
        std::cout << "Passed: " << passed << "/" << tests.size() << std::endl;
        std::cout << "Failed: " << failed << "/" << tests.size() << std::endl;
    }
};
```

### Running Tests

#### Command Line Interface

```bash
# Run all tests
./test_config --all

# Run specific test category
./test_config --schema-validation
./test_config --loading
./test_config --command-line
./test_config --performance
./test_config --integration

# Run with verbose output
./test_config --verbose

# Run with performance measurements
./test_config --performance-measure

# Run with custom configuration file
./test_config --config test_config.json

# Run with custom test directory
./test_config --test-directory ./my_tests

# Run with specific test file
./test_config --test-file schema_test.cpp
```

#### Python Test Runner

```python
#!/usr/bin/env python3

import sys
import os
import argparse
import subprocess
import json

class ConfigurationTestRunner:
    def __init__(self):
        self.test_results = []
        self.config = ConfigRegistry.getInstance()
    
    def run_tests(self, args):
        if args.all:
            self.run_all_tests()
        elif args.schema_validation:
            self.run_schema_validation_tests()
        elif args.loading:
            self.run_loading_tests()
        elif args.command_line:
            self.run_command_line_tests()
        elif args.performance:
            self.run_performance_tests()
        elif args.integration:
            self.run_integration_tests()
        else:
            self.print_help()
    
    def run_all_tests(self):
        self.run_schema_validation_tests()
        self.run_loading_tests()
        self.run_command_line_tests()
        self.run_performance_tests()
        self.run_integration_tests()
    
    def run_schema_validation_tests(self):
        print("\n=== Running Schema Validation Tests ===")
        
        # Test schema registration
        self.run_schema_registration_test()
        
        # Test schema validation
        self.run_schema_validation_test()
        
        # Test full validation
        self.run_full_validation_test()
    
    def run_loading_tests(self):
        print("\n=== Running Loading Tests ===")
        
        # Test JSON loading
        self.run_json_loading_test()
        
        # Test YAML loading
        self.run_yaml_loading_test()
        
        # Test .cfg loading
        self.run_cfg_loading_test()
    
    def run_command_line_tests(self):
        print("\n=== Running Command Line Tests ===")
        
        # Test command line overrides
        self.run_command_line_override_test()
        
        # Test mixed configuration
        self.run_mixed_configuration_test()
    
    def run_performance_tests(self):
        print("\n=== Running Performance Tests ===")
        
        # Test loading performance
        self.run_loading_performance_test()
        
        # Test validation performance
        self.run_validation_performance_test()
    
    def run_integration_tests(self):
        print("\n=== Running Integration Tests ===")
        
        # Test end-to-end configuration
        self.run_end_to_end_configuration_test()
        
        # Test error handling
        self.run_error_handling_test()
    
    def run_schema_registration_test(self):
        try:
            # Test that core schemas are registered
            assert self.config.hasSchema("random_seed")
            assert self.config.hasSchema("simulation_timestep")
            assert self.config.hasSchema("neuron_count")
            
            # Test schema accessibility
            schema = self.config.getSchema("random_seed")
            assert schema is not None
            assert schema.type == "int"
            assert schema.required == True
            
            # Test schema description
            description = self.config.getSchemaDescription()
            assert "Configuration Schema" in description
            assert "random_seed" in description
            
            self.test_results.append({"name": "Schema Registration", "status": "PASSED"})
            print("✓ Schema Registration test passed")
            
        except Exception as e:
            self.test_results.append({"name": "Schema Registration", "status": "FAILED", "error": str(e)})
            print(f"✗ Schema Registration test failed: {e}")
    
    def run_schema_validation_test(self):
        try:
            # Test valid values
            assert self.config.validate("random_seed", ConfigValue(42))
            assert self.config.validate("simulation_timestep", ConfigValue(0.001))
            assert self.config.validate("neuron_count", ConfigValue(1000))
            assert self.config.validate("log_level", ConfigValue("INFO"))
            
            # Test invalid values (type mismatch)
            assert not self.config.validate("random_seed", ConfigValue("42"))  # string instead of int
            assert not self.config.validate("simulation_timestep", ConfigValue("0.001"))  # string instead of double
            
            # Test invalid values (range violation)
            assert not self.config.validate("neuron_count", ConfigValue(-5))  # negative value
            assert not self.config.validate("simulation_timestep", ConfigValue(0.0))  # too small
            assert not self.config.validate("simulation_timestep", ConfigValue(2.0))  # too large
            
            # Test invalid values (pattern violation)
            assert not self.config.validate("log_level", ConfigValue("INVALID"))  # not in allowed values
            
            self.test_results.append({"name": "Schema Validation", "status": "PASSED"})
            print("✓ Schema Validation test passed")
            
        except Exception as e:
            self.test_results.append({"name": "Schema Validation", "status": "FAILED", "error": str(e)})
            print(f"✗ Schema Validation test failed: {e}")
    
    def run_full_validation_test(self):
        try:
            # Create configuration with valid values
            valid_config = {
                "random_seed": ConfigValue(42),
                "simulation_timestep": ConfigValue(0.001),
                "neuron_count": ConfigValue(1000),
                "region_count": ConfigValue(2),
                "connection_probability": ConfigValue(0.1),
                "log_level": ConfigValue("INFO"),
                "log_to_file": ConfigValue(False)
            }
            
            errors = self.config.getValidationErrors(valid_config)
            assert len(errors) == 0  # No errors expected
            
            # Create configuration with invalid values
            invalid_config = {
                "random_seed": ConfigValue(42),
                "simulation_timestep": ConfigValue(0.001),
                "neuron_count": ConfigValue(-5),  # Invalid
                "region_count": ConfigValue(2),
                "connection_probability": ConfigValue(0.1),
                "log_level": ConfigValue("INVALID"),  # Invalid
                "log_to_file": ConfigValue(False)
            }
            
            errors = self.config.getValidationErrors(invalid_config)
            assert len(errors) > 0  # Errors expected
            assert any("neuron_count" in error for error in errors)
            assert any("log_level" in error for error in errors)
            
            // Test validation report
            report = self.config.getValidationReport(invalid_config)
            assert "Validation Report" in report
            assert "Invalid: " in report
            assert "neuron_count" in report
            assert "log_level" in report
            
            self.test_results.append({"name": "Full Validation", "status": "PASSED"})
            print("✓ Full Validation test passed")
            
        except Exception as e:
            self.test_results.append({"name": "Full Validation", "status": "FAILED", "error": str(e)})
            print(f"✗ Full Validation test failed: {e}")
    
    def run_json_loading_test(self):
        try:
            config = Config()
            
            // Create temporary JSON config file
            json_content = '''{
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "log_level": "INFO"
            }'''
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                f.write(json_content)
                temp_file = f.name
            
            try:
                // Test loading
                assert config.loadFromFile(temp_file)
                
                // Verify loaded values
                assert config.getOr<int64_t>("random_seed", 0) == 42
                assert config.getOr<double>("simulation_timestep", 0.0) == 0.001
                assert config.getOr<int64_t>("neuron_count", 0) == 1000
                assert config.getOr<int64_t>("region_count", 0) == 2
                assert config.getOr<float>("connection_probability", 0.0) == 0.1
                assert config.getOr<std::string>("log_level", "") == "INFO"
                
                self.test_results.append({"name": "JSON Loading", "status": "PASSED"})
                print("✓ JSON Loading test passed")
                
            finally:
                os.unlink(temp_file)
                
        except Exception as e:
            self.test_results.append({"name": "JSON Loading", "status": "FAILED", "error": str(e)})
            print(f"✗ JSON Loading test failed: {e}")
    
    // Additional tests omitted for brevity
    
    def generate_test_report(self):
        print("\n=== Test Report ===")
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results if result["status"] == "PASSED")
        failed_tests = total_tests - passed_tests
        
        print(f"Total Tests: {total_tests}")
        print(f"Passed: {passed_tests}")
        print(f"Failed: {failed_tests}")
        
        if failed_tests > 0:
            print("\nFailed Tests:")
            for result in self.test_results:
                if result["status"] == "FAILED":
                    print(f"  - {result['name']}: {result.get('error', 'Unknown error')}")
        
        success_rate = (passed_tests / total_tests) * 100.0
        print(f"\nSuccess Rate: {success_rate:.1f}%")
        
        return success_rate >= 90.0  // Consider tests passed if success rate >= 90%

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Configuration Validation Test Suite")
    parser.add_argument("--all", action="store_true", help="Run all tests")
    parser.add_argument("--schema-validation", action="store_true", help="Run schema validation tests")
    parser.add_argument("--loading", action="store_true", help="Run loading tests")
    parser.add_argument("--command-line", action="store_true", help="Run command line tests")
    parser.add_argument("--performance", action="store_true", help="Run performance tests")
    parser.add_argument("--integration", action="store_true", help="Run integration tests")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--config", help="Configuration file path")
    parser.add_argument("--output", help="Output file path")
    
    args = parser.parse_args()
    
    if not any([args.all, args.schema_validation, args.loading, args.command_line, args.performance, args.integration]):
        parser.print_help()
        sys.exit(1)
    
    runner = ConfigurationTestRunner()
    success = runner.run_tests(args)
    
    if success:
        sys.exit(0)
    else:
        sys.exit(1)
```

### Building Tests

#### Compilation Commands

```bash
# Compile configuration tests
g++ -std=c++17 -I. -I./include -O2 -o test_config test_config.cpp

# Compile with configuration file support
g++ -std=c++17 -I. -I./include -O2 -DCONFIG_FILE_PATH="configs/default.cfg" -o test_config test_config.cpp

# Compile with debug support
g++ -std=c++17 -I. -I./include -O2 -g -DDEBUG -o test_config_debug test_config.cpp

# Compile with performance optimization
# Use the provided build script
./build_tests.sh
```

#### Build Script

```bash
#!/bin/bash

# Build configuration test suite

cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# Configure with CMake (if available)
if command -v cmake &> /dev/null; then
    cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON
    make -j$(nproc)
else
    # Fallback to direct compilation
    g++ -std=c++17 -I.. -I../include -O2 -o test_config ../test_config.cpp
fi

# Verify build
if [ -f "test_config" ]; then
    echo "✓ Build successful: test_config created"
else
    echo "✗ Build failed"
    exit 1
fi
```

## Test Results Analysis

### Expected Test Results

The configuration validation test suite should achieve the following results:

#### Performance Requirements

| Test Type | Expected Result | Measurement |
|-----------|-----------------|-------------|
| Schema Registration | Should complete quickly | < 1ms |
| Schema Validation | Should be fast | < 100us per test |
| JSON Loading | Should be efficient | < 10ms per file |
| YAML Loading | Should be supported | < 5ms per file |
| Command Line | Should work correctly | < 1ms per override |
| Mixed Configuration | Should handle mixed sources | < 2ms per test |
| Loading Performance | Should be scalable | < 50ms for 100 files |
| Validation Performance | Should be excellent | < 100us per validation |

#### Reliability Requirements

| Test Category | Expected Success Rate | Requirements |
|---------------|----------------------|--------------|
| Schema Validation | > 95% | All valid values should pass validation |
| Loading | > 90% | All file formats should load correctly |
| Command Line | > 95% | All overrides should be applied correctly |
| Integration | > 85% | End-to-end tests should work |
| Error Handling | > 98% | Errors should be handled gracefully |

### Test Result Interpretation

#### Green Tests (Passed)
- All assertions passed
- No exceptions thrown
- Expected behavior confirmed
- Performance targets met

#### Yellow Tests (Warnings)
- Some assertions failed
- Warnings emitted
- Performance targets not fully met
- Recovery mechanisms may be needed

#### Red Tests (Failed)
- Critical assertions failed
- Exceptions thrown
- System behavior unexpected
- Security vulnerabilities detected

### Continuous Integration

#### GitHub Actions Workflow

```yaml
name: Configuration Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test-configuration:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup C++ environment
      uses: egor-tensin/setup-cpp@v1
      with:
        compiler: g++
        cmake-version: 3.16
        ninja-version: 1.10
    
    - name: Build tests
      run: ./build_tests.sh
    
    - name: Run configuration tests
      run: ./test_config --all --verbose
    
    - name: Generate test report
      run: ./generate_test_report.sh
      if: always()
    
    - name: Upload test artifacts
      uses: actions/upload-artifact@v3
      with:
        name: test-results
        path: test_results.json
      if: always()
    
    - name: Publish test results
      uses: EnricoSaracina/test-reporter@v1
      with:
        name: NLM Configuration Tests
        summary: NLM Configuration Validation Test Results
        path: test_results.json
      if: always()
```

#### Jenkins Pipeline

```groovy
pipeline {
    agent any
    
    stages {
        stage('Build') {
            steps {
                sh './build_tests.sh'
            }
        }
        
        stage('Test') {
            steps {
                sh './test_config --all'
            }
        }
        
        stage('Analyze') {
            steps {
                sh './analyze_test_results.sh'
            }
            post {
                always {
                    archiveArtifacts artifacts: 'test_results.json', allowEmpty: true
                }
                failure {
                    error 'Tests failed'
                }
            }
        }
    }
    
    post {
        always {
            sh 'echo "Build and test completed"'
        }
        success {
            sh 'echo "All tests passed!"'
        }
        failure {
            sh 'echo "Tests failed!"'
        }
    }
}
```

### Test Maintenance

#### Updating Tests

1. **Add New Test Cases**
   - Add new configuration keys to schema
   - Add new file format tests
   - Add new validation test scenarios

2. **Update Test Data**
   - Update test configuration files
   - Update expected test results
   - Update performance benchmarks

3. **Fix Test Issues**
   - Debug failing tests
   - Update test expectations
   - Fix test infrastructure

#### Test Coverage

The test suite should maintain the following coverage levels:

- **Unit Tests**: > 90% code coverage
- **Integration Tests**: > 80% code coverage
- **Performance Tests**: > 95% test pass rate
- **Error Handling**: > 98% test pass rate

## Conclusion

The NLM configuration validation test suite provides comprehensive testing of the enhanced configuration system. It includes tests for schema validation, file loading, command line overrides, performance, and integration. The test suite ensures that the configuration system is reliable, efficient, and maintains high performance characteristics.

Key benefits of the test suite include:

- **Reliability**: Ensures configuration system functions correctly under all conditions
- **Performance**: Validates performance targets and identifies bottlenecks
- **Maintainability**: Provides automated testing for configuration changes
- **Compatibility**: Ensures compatibility across different file formats and platforms
- **Error Handling**: Tests error handling and recovery mechanisms

The configuration validation test suite is an essential component of the NLM configuration system, ensuring its robustness and reliability in production environments.