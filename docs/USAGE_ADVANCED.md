# Modern CMake Integration - Test Framework

## Overview

This document provides comprehensive integration instructions for the modern CMake build system, focusing on the enhanced test framework and development practices.

## Test Framework Integration

### 1. Google Test Setup

The modern CMake build system integrates Google Test framework with comprehensive testing capabilities:

```cmake
# Find and configure Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test targets with Google Test integration
add_executable(nlm_test_brain tests/brain_tests.cpp)
target_link_libraries(nlm_test_brain PRIVATE nlm_core GTest::gtest GTest::gtest_main)

add_executable(nlm_test_config tests/config_tests.cpp)
target_link_libraries(nlm_test_config PRIVATE nlm_core GTest::gtest GTest::gtest_main)

add_executable(nlm_test_performance tests/performance_tests.cpp)
target_link_libraries(nlm_test_performance PRIVATE nlm_core nlm_performance GTest::gtest GTest::gtest_main)

# Integration test executable
add_executable(nlm_test_integration tests/integration_tests.cpp)
target_link_libraries(nlm_test_integration PRIVATE nlm_core nlm_agent nlm_world GTest::gtest GTest::gtest_main)

# Performance benchmark executable
add_executable(nlm_benchmark tests/benchmark.cpp)
target_link_libraries(nlm_benchmark PRIVATE nlm_core nlm_performance)
```

### 2. Unit Test Structure

Unit tests are organized into categories:

#### Brain Tests (`tests/brain_tests.cpp`)
```cpp
// Test Brain Class
class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
};

// Test Case Examples
TEST_F(BrainTest, Creation) {
    ASSERT_NE(brain, nullptr);
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getRegionCount(), 2);
}

TEST_F(BrainTest, Step) {
    for (int i = 0; i < 10; ++i) {
        brain->step(i);
    }
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
}

TEST_F(BrainTest, ActionProduction) {
    auto action = brain->produceAction();
    ASSERT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), nlm::ActionType::MotorCommand);
}
```

#### Configuration Tests (`tests/config_tests.cpp`)
```cpp
// Test Config Class
class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
    }
    
    std::shared_ptr<nlm::Config> config;
};

TEST_F(ConfigTest, BasicGetSet) {
    config->set("test_int", 42);
    EXPECT_EQ(config->get<int>("test_int"), 42);
    
    config->set("test_string", "hello");
    EXPECT_EQ(config->get<std::string>("test_string"), "hello");
}

TEST_F(ConfigTest, JsonSerialization) {
    config->set("neuron_count", 1000);
    config->set("region_count", 3);
    
    std::string json_str = config->toJson();
    auto config2 = nlm::Config::fromJson(json_str);
    
    EXPECT_EQ(config2->get<int>("neuron_count"), 1000);
    EXPECT_EQ(config2->get<int>("region_count"), 3);
}
```

#### Performance Tests (`tests/performance_tests.cpp`)
```cpp
// Performance Test Suite
class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", performance::BenchmarkConfig::default_neuron_count);
        config->set("region_count", 1);
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
};

// Performance Benchmarking Tests
TEST_F(PerformanceTest, MinimumStepsPerSecond) {
    constexpr double min_steps_per_second = 1000.0; // 1k steps/sec
    
    auto start_time = std::chrono::high_resolution_clock::now();
    const int steps = 1000;
    
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double steps_per_second = steps / duration.count();
    EXPECT_GE(steps_per_second, min_steps_per_second);
}

TEST_F(PerformanceTest, MemoryUsage) {
    // Get initial memory usage
    auto initial_stats = brain->getPerformanceStats();
    double initial_memory = initial_stats.memoryUsageMB;
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    // Get final memory usage
    auto final_stats = brain->getPerformanceStats();
    double final_memory = final_stats.memoryUsageMB;
    
    // Memory usage should not grow excessively
    EXPECT_LT(final_memory - initial_memory, 100.0); // Less than 100MB growth
}
```

### 3. Integration Tests

Integration tests verify component interactions:

#### Integration Tests (`tests/integration_tests.cpp`)
```cpp
// System Integration Tests
class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize all components
        config = std::make_shared<nlm::Config>();
        brain = std::make_shared<nlm::Brain>(config);
        world = std::make_shared<nlm::SimpleWorld>();
        agent = std::make_shared<nlm::AgentBrain>(brain);
        
        brain->initialize();
        agent->initialize(*world);
    }
    
    void TearDown() override {
        agent.reset();
        world.reset();
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::SimpleWorld> world;
    std::shared_ptr<nlm::AgentBrain> agent;
};

// Integration Test Examples
TEST_F(IntegrationTest, BrainWorldInteraction) {
    // Test data flow from brain to world
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    // Verify brain received input
    auto action = brain->produceAction();
    EXPECT_NE(action, nullptr);
    
    // Apply action to world
    world->applyMotorCommand(*action, 0.0);
    
    // Verify world updated
    auto body = world->getAgentBody();
    EXPECT_TRUE(body->x >= 0.0 && body->x <= world->getWidth());
}

TEST_F(IntegrationTest, EndToEndSimulation) {
    // Complete simulation cycle
    const int steps = 100;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(step * 0.001);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process with agent
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Produce and apply action
        auto action = brain->produceAction();
        if (action) {
            world->applyMotorCommand(*action, step * 0.001);
        }
    }
    
    // Verify system completed without errors
    EXPECT_EQ(brain->getTotalSpikeCount(), steps * 50); // Approximate spike count
}
```

### 4. Performance Benchmarks

Performance benchmarks are implemented as standalone executables:

```cpp
// tests/benchmark.cpp
#include <benchmark/benchmark.h>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

static void BM_BrainStep(benchmark::State& state) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
    }
}

BENCHMARK(BM_BrainStep)->Arg(100)->Arg(1000)->Arg(5000);

BENCHMARK_MAIN();
```

### 5. Memory Leak Detection

Memory leak detection using AddressSanitizer:

```cpp
// tests/memory_leak_test.cpp
#include <gtest/gtest.h>
#include <sanitizer/asan_interface.h>

class MemoryLeakTest : public ::testing::Test {
protected:
    void SetUp() override {
        ASAN_OPTIONS.disable_cmplr_specific=0;
    }
};

TEST_F(MemoryLeakTest, NoMemoryLeaks) {
    // Run code that might leak memory
    for (int i = 0; i < 100; ++i) {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Simulate work
        for (int j = 0; j < 10; ++j) {
            brain->step(j);
        }
    }
}
```

### 6. Cross-Platform Testing

Cross-platform testing support:

```cpp
// tests/cross_platform_test.cpp
#include <gtest/gtest.h>
#include <system/platform.h>

class CrossPlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        platform_info = nlm::getPlatformInfo();
    }
    
    nlm::PlatformInfo platform_info;
};

TEST_F(CrossPlatformTest, PlatformSpecificBehavior) {
#ifdef _WIN32
    EXPECT_TRUE(platform_info.is_windows);
#elif __APPLE__
    EXPECT_TRUE(platform_info.is_macos);
#elif __linux__
    EXPECT_TRUE(platform_info.is_linux);
#endif
}

TEST_F(CrossPlatformTest, SIMDSupport) {
    if (platform_info.has_sse4_1) {
        // Test SSE4.1 optimized code
        EXPECT_TRUE(nlm::simd::has_sse4_1);
    }
    
    if (platform_info.has_avx2) {
        // Test AVX2 optimized code
        EXPECT_TRUE(nlm::simd::has_avx2);
    }
}
```

## Build Configuration

### 1. CMake Configuration

```cmake
# Configure CMake for modern build system
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Enable testing
enable_testing()
include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Configure build options
option(NLM_PERFORMANCE "Enable performance optimizations" ON)
option(NLM_VISUALIZATION "Enable visualization support" ON)
option(NLM_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_TESTS "Build test suite" ON)
option(NLM_BENCHMARKS "Build performance benchmarks" ON)
option(NLM_COVERAGE "Enable code coverage" OFF)
option(NLM_ASAN "Enable AddressSanitizer" OFF)
option(NLM_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# Configure sanitizers
if(NLM_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

if(NLM_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# Configure compiler flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-std=c++20)

# Create test targets
add_subdirectory(tests)
```

### 2. Test CMakeLists.txt

```cmake
# tests/CMakeLists.txt

# Create test executables
add_executable(nlm_test_brain brain_tests.cpp)
target_link_libraries(nlm_test_brain PRIVATE nlm_core GTest::gtest GTest::gtest_main)

add_executable(nlm_test_config config_tests.cpp)
target_link_libraries(nlm_test_config PRIVATE nlm_core GTest::gtest GTest::gtest_main)

add_executable(nlm_test_performance performance_tests.cpp)
target_link_libraries(nlm_test_performance PRIVATE nlm_core nlm_performance GTest::gtest GTest::gtest_main)

add_executable(nlm_test_integration integration_tests.cpp)
target_link_libraries(nlm_test_integration PRIVATE nlm_core nlm_agent nlm_world GTest::gtest GTest::gtest_main)

# Create benchmark executable
add_executable(nlm_benchmark benchmark.cpp)
target_link_libraries(nlm_benchmark PRIVATE nlm_core nlm_performance)

# Add tests to CTest
add_test(NAME nlm_test_brain COMMAND nlm_test_brain)
add_test(NAME nlm_test_config COMMAND nlm_test_config)
add_test(NAME nlm_test_performance COMMAND nlm_test_performance)
add_test(NAME nlm_test_integration COMMAND nlm_test_integration)

# Add benchmarks (not part of CTest by default)
add_custom_target(benchmark_all
    COMMAND nlm_benchmark --benchmark-list
    DEPENDS nlm_benchmark
    COMMENT "Run all performance benchmarks"
)
```

### 3. Performance Benchmark Configuration

```cmake
# Configure performance benchmarks
option(NLM_BENCHMARK_NEURON_COUNTS "Neuron counts for benchmarking" "100,500,1000,2000")
option(NLM_BENCHMARK_STEPS "Steps for each benchmark" 1000)
option(NLM_BENCHMARK_REPEATS "Number of benchmark repeats" 5)

# Generate benchmark code from configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/benchmark_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark_config.cmake
    @ONLY
)
```

## Running Tests

### 1. Local Testing

```bash
# Configure and build with tests
mkdir build && cd build
cmake .. -DNLM_TESTS=ON -DNLM_PERFORMANCE=ON -DNLM_PYTHON_BINDINGS=ON
cmake --build . --target all

# Run all tests
ctest --output-on-failure

# Run specific test suites
ctest -R "test_brain"
ctest -R "test_config"
ctest -R "test_performance"
ctest -R "test_integration"

# Run benchmarks
./nlm_benchmark --benchmark-list
./nlm_benchmark --benchmark-type brain_step
```

### 2. Performance Testing

```bash
# Build with sanitizers for leak detection
cmake .. -DNLM_ASAN=ON -DNLM_UBSAN=ON
make -j$(nproc)

# Run memory leak tests
./nlm_test_brain --gtest_filter=*Leak*

# Run with memory report
ASAN_OPTIONS=detect_leaks=1 ./nlm_test_brain
```

### 3. Cross-Platform Testing

```bash
# Test on different platforms
# Linux
cmake .. -DNLM_PLATFORM_LINUX=ON
make -j$(nproc)
ctest

# macOS
cmake .. -DNLM_PLATFORM_MACOS=ON
make -j$(sysctl -n hw.ncpu)
ctest

# Windows
cmake .. -DNLM_PLATFORM_WINDOWS=ON
cmake --build . --config Release
ctest -C Release
```

### 4. Python Integration Testing

```python
# python/python_test_suite.py
import pynlm
import pytest
import json
import tempfile
import os

class TestPythonBindings:
    def test_config_creation(self):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        assert config.get("neuron_count") == 100
        
    def test_brain_creation(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        
    def test_visualization(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        vis = pynlm.createVisualization()
        vis.initialize()
        vis.setUpdateRate(30.0)
        
        assert vis.isActive()
        assert vis.getUpdateRate() == 30.0
        
        vis.close()

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
```

## Code Quality and Best Practices

### 1. Testing Best Practices

#### Test Organization
- Group related tests into test cases
- Use fixtures for setup/teardown
- Write tests that are independent and isolated
- Use assertions for verification

#### Performance Testing
- Measure execution time with consistent conditions
- Use benchmarks for performance comparisons
- Test with different input sizes
- Profile memory usage

#### Memory Safety
- Test for memory leaks with sanitizers
- Use smart pointers correctly
- Test boundary conditions

### 2. Integration with CI/CD

#### GitHub Actions

```yaml
name: NLM Tests

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: |
        cmake -B build -S . 
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
          -DNLM_TESTS=ON
          -DNLM_PERFORMANCE=ON
          -DNLM_PYTHON_BINDINGS=ON
    
    - name: Build Tests
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run Tests
      run: cd build && ctest --output-on-failure
    
    - name: Run Python Tests
      run: |
        cd python
        pip install -e .
        python -m pytest python_test_suite.py -v
```

#### Azure Pipelines

```yaml
stages:
- stage: Test
  jobs:
  - job: Linux
    pool:
      vmImage: 'ubuntu-latest'
    steps:
    - script: |
        mkdir build
        cd build
        cmake .. -DNLM_TESTS=ON -DNLM_PERFORMANCE=ON -DNLM_PYTHON_BINDINGS=ON
        cmake --build . -- -j$(nproc)
        ctest --output-on-failure
      displayName: 'Run Tests'
  
  - job: Performance
    pool:
      vmImage: 'ubuntu-latest'
    steps:
    - script: |
        mkdir build
        cd build
        cmake .. -DNLM_TESTS=ON -DNLM_PERFORMANCE=ON
        cmake --build . -- -j$(nproc)
        ./nlm_benchmark --benchmark-list
      displayName: 'Run Performance Tests'
```

### 3. Documentation and Examples

#### Test Documentation

```cpp
/**
 * @file brain_tests.cpp
 * @brief Unit tests for the Brain class
 * 
 * This file contains comprehensive unit tests for the Brain class,
 * including creation, initialization, step execution, and action
 * production tests.
 * 
 * Test Coverage:
 * - Basic Brain functionality
 * - Initialization and configuration
 * - Simulation step execution
 * - Action production and validation
 * - Performance monitoring
 */
```

#### Usage Examples

```python
#!/usr/bin/env python3
"""
Example usage of NLM Python bindings with testing.
"""

import pynlm
import json
import tempfile

def example_basic_usage():
    """Basic usage example."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
        if step % 10 == 0:
            stats = brain.getPerformanceStats()
            print(f"Step {step}: {stats.neuronsPerSecond:.0f} neurons/sec")
    
    print("Basic example completed successfully")

def example_with_visualization():
    """Example with visualization."""
    config = pynlm.createDefaultConfig()
    config.set("visualization.enabled", True)
    config.set("visualization.updateRate", 30.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    vis = pynlm.createVisualization()
    vis.initialize()
    vis.setUpdateRate(30.0)
    
    for step in range(100):
        brain.step(step)
        vis.update()
        vis.visualizeNetwork(brain)
        vis.render()
    
    vis.close()
    print("Visualization example completed successfully")

def example_configuration():
    """Example with configuration files."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("connection_probability", 0.05)
    
    # Save to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        print(f"Configuration saved to {f.name}")
    
    # Load from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(f.name)
    
    # Verify configuration
    assert config2.get<int>("neuron_count") == 500
    print("Configuration example completed successfully")
    
    # Cleanup
    os.unlink(f.name)

if __name__ == "__main__":
    example_basic_usage()
    example_with_visualization()
    example_configuration()
```

## Performance Optimization Guidelines

### 1. Build Optimization

```bash
# Build with maximum performance
cmake .. -DNLM_PERFORMANCE=ON -DNLM_USE_SIMD=ON -DNLM_THREAD_COUNT=8
make -j$(nproc)

# Build with debugging support
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ASAN=ON
make -j$(nproc)

# Build with coverage reporting
cmake .. -DNLM_COVERAGE=ON
make -j$(nproc)
```

### 2. Runtime Optimization

```cpp
// Enable performance optimizations at runtime
nlm::PerformanceOptimizer::enableSIMD();
nlm::PerformanceOptimizer::enableMultithreading(8);
nlm::PerformanceOptimizer::setOptimizationLevel(nlm::OptimizationLevel::Aggressive);

// Monitor performance
auto stats = brain->getPerformanceStats();
std::cout << "Performance: " << stats.neuronsPerSecond << " neurons/sec" << std::endl;
```

### 3. Memory Management

```cpp
// Use memory pools for better performance
nlm::MemoryPool::initialize();

// Enable compression for large simulations
nlm::SimulationCompressor::enable(true);

// Profile memory usage
nlm::MemoryProfiler::start();
// Run simulation
nlm::MemoryProfiler::stop();
auto profile = nlm::MemoryProfiler::getProfile();
```

## Future Enhancements

### 1. Advanced Testing Features

- **Property-based testing**: Use Hypothesis to test property-based assertions
- **Fuzz testing**: Integrate libFuzzer for input validation
- **Model-based testing**: Use state machine models for integration tests

### 2. Continuous Integration

- **Performance regression testing**: Automated performance monitoring
- **Compatibility testing**: Test across different compiler versions
- **Platform testing**: Test on additional platforms

### 3. Test Data Management

- **Generated test data**: Create test data programmatically
- **Test fixtures**: Manage test fixtures and state
- **Benchmark databases**: Store performance benchmarks

## Conclusion

The modern CMake build system with comprehensive test suite provides:

- **Target-based CMake**: Modern, maintainable build configuration
- **Google Test Integration**: Professional-grade unit testing
- **Performance Benchmarking**: Automated performance monitoring
- **Memory Safety**: Leak detection and validation
- **Cross-Platform Support**: Testing across different platforms
- **CI/CD Integration**: Automated testing in CI/CD pipelines

This build system ensures code quality, performance, and reliability through comprehensive testing and modern development practices.
