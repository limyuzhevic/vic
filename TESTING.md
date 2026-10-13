# NLM - Testing Guide

**Date:** 2026-10-13
**Audience:** Developers and testers
**Level:** Testing guide

---

## Table of Contents

1. [Quick Start Testing](#quick-start-testing)
2. [Testing Framework](#testing-framework)
3. [Test Categories](#test-categories)
4. [Running Tests](#running-tests)
5. [Test Configuration](#test-configuration)
6. [Test Reporting](#test-reporting)
7. [Test Troubleshooting](#test-troubleshooting)
8. [Continuous Integration](#continuous-integration)
9. [Performance Testing](#performance-testing)
10. [Load Testing](#load-testing)

---

## Quick Start Testing

### Quick Test Commands

#### Run Basic Tests
```bash
# Run unit tests
mkdir build
cd build
./nlm_test

# Or run via CTest
ctest --output-on-failure
```

#### Run Python Tests
```bash
# Run Python unit tests
cd /workspace/app
python3 -m pytest tests/ -v

# Run integration tests
python3 -m pytest tests/integration/ -v

# Run all tests
python3 -m pytest tests/ --cov=pynlm --cov-report=html
```

#### Run Example Tests
```bash
# Run example scripts
cd /workspace/app
python3 examples_comprehensive.py
python3 examples_advanced.py
python3 examples_basic.py
```

### Quick Test Summary

#### Test Command Examples
```bash
# Unit tests
./build/nlm_test

# Integration tests
./build/nlm_phase6_demo

# Python tests
python3 -m pytest tests/unit/ -v

# Performance tests
./build/nlm --benchmark

# Coverage tests
python3 -m pytest tests/ --cov=pynlm
```

## Testing Framework

### NLM Testing Architecture

#### Test Structure
```
tests/
├── unit/              # Unit tests
│   ├── test_constants.cpp     # Basic constants test
│   ├── test_error_framework.cpp # Error framework test
│   ├── test_interface_refactor.cpp # Interface compliance test
│   └── test_refactor_complete.cpp # Refactoring test
│
├── integration/        # Integration tests
│   ├── phase5_integrated.cpp     # Phase 5 integration test
│   └── phase6_integration.cpp   # Phase 6 integration test
│
├── performance/        # Performance tests
│   ├── performance_benchmark.cpp    # Benchmark test
│   └── memory_tests.cpp        # Memory test
│
└── examples/          # Example-based tests
    ├── examples_comprehensive.py # Comprehensive example test
    ├── examples_advanced.py      # Advanced example test
    └── examples_basic.py         # Basic example test
```

#### Test Types

##### Unit Tests
- **Purpose**: Test individual components and functions
- **Scope**: Small, focused tests
- **Isolation**: Each test is independent
- **Speed**: Fast execution
- **Tools**: Google Test, Catch2

##### Integration Tests
- **Purpose**: Test system integration
- **Scope**: Multiple components working together
- **Realism**: Simulate real-world usage
- **Complexity**: Higher complexity
- **Tools**: Custom test frameworks

##### Performance Tests
- **Purpose**: Measure performance characteristics
- **Scope**: System-level performance
- **Metrics**: Speed, memory, efficiency
- **Tools**: Benchmarking, profiling

##### Example Tests
- **Purpose**: Validate examples work correctly
- **Scope**: Complete example scenarios
- **Validation**: Functional correctness
- **Tools**: Python testing frameworks

### Testing Tools and Frameworks

#### C++ Testing Frameworks
```bash
# Google Test (CMake's test framework)
# Configured in CMakeLists.txt
# Build with: make nlm_test

# Catch2 (header-only testing framework)
# Can be integrated into existing code
# Benefits: Simple, lightweight, easy to use
```

#### Python Testing Frameworks
```bash
# PyTest (most common)
pip3 install pytest pytest-cov pytest-xdist

# Use for unit tests and integration tests
python3 -m pytest tests/ -v --tb=short

# Generate coverage reports
python3 -m pytest tests/ --cov=pynlm --cov-report=html --cov-report=term

# Parallel testing
python3 -m pytest tests/ -n auto -v
```

#### Performance Testing Tools
```bash
# Performance benchmarking
# Built into NLM executable
./build/nlm --benchmark --neurons=1000 --steps=1000

# Memory profiling
valgrind --tool=massif ./build/nlm_phase6_demo

# CPU profiling
perf record ./build/nlm_phase6_demo
perf report
```

## Test Categories

### Unit Tests

#### Core Functionality Tests
```bash
# Test brain initialization
# Test neuron creation
# Test synaptic connections
# Test plasticity mechanisms
# Test development stages
```

#### Component Tests
```bash
# Test memory systems
# Test neuromodulation systems
# config.py
# Test prediction systems
# Test cognition systems
```

#### Interface Tests
```bash
# Test IBrainInterface compliance
# Test IComponents compliance
# Test dependency injection
# Test factory functions
```

### Integration Tests

#### System Integration Tests
```bash
# Test Phase 6 integration
# Test memory system integration
# Test neuromodulation integration
# Test development system integration
# Test prediction system integration
```

#### Agent Integration Tests
```bash
# Test agent-world interaction
# Test sensory processing
# Test motor output
# Test reward learning
# Test curiosity behavior
```

#### Cross-System Tests
```bash
# Test memory-neuromodulation interaction
# Test development-plasticity interaction
# Test prediction-attention interaction
# Test learning-consolidation interaction
```

### Performance Tests

#### Benchmark Tests
```bash
# Neuron throughput benchmarks
# Memory efficiency benchmarks
# Simulation speed benchmarks
# Scalability benchmarks
```

#### Stress Tests
```bash
# Large-scale simulation tests
# Memory pressure tests
# CPU intensive tests
# Concurrent access tests
```

### Load Tests

#### Real-world Load Tests
```bash
# Simulate multiple agents
# Test system under heavy load
# Measure resource consumption
# Verify stability
```

#### Edge Case Tests
```bash
# Test with extreme parameters
# Test with minimal resources
# Test with corrupted data
# Test with invalid inputs
```

## Running Tests

### Standard Test Commands

#### Unit Tests
```bash
# Build and run unit tests
cd /workspace/app
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./nlm_test
```

#### Integration Tests
```bash
# Run integration tests
cd /workspace/app
./build/nlm_phase6_demo

# Run specific integration tests
cd /workspace/app
./build/nlm_test --gtest_filter="*Integration*"
```

#### Python Tests
```bash
# Install test dependencies
pip3 install pytest pytest-cov pytest-xdist

# Run all Python tests
cd /workspace/app
python3 -m pytest tests/ -v --tb=short

# Run tests with coverage
cd /workspace/app
python3 -m pytest tests/ --cov=pynlm --cov-report=html --cov-report=term

# Run tests in parallel
cd /workspace/app
python3 -m pytest tests/ -n auto -v
```

#### Example Tests
```bash
# Run comprehensive example tests
cd /workspace/app
python3 examples_comprehensive.py

# Run advanced example tests
cd /workspace/app
pythoncd /workspace/app
python3 examples_advanced.py

# Run basic example tests
cd /workspace/app
python3 examples_basic.py
```

### Test Configuration

#### Test Configuration File
```bash
cd /workspace/app

# Create test configuration
cat > test_config.json << 'EOF'
{
  "test_settings": {
    "unit_tests": {
      "enable": true,
      "verbose": true,
      "parallel": true
    },
    "integration_tests": {
      "enable": true,
      "phase6": true,
      "cross_system": true
    },
    "performance_tests": {
      "enable": true,
      "benchmarks": ["neuron_throughput", "memory_efficiency", "simulation_speed"],
      "load_tests": ["stress", "edge_cases"]
    },
    "example_tests": {
      "enable": true,
      "comprehensive": true,
      "advanced": true,
      "basic": true
    }
  },
  "benchmark_settings": {
    "neurons": 1000,
    "steps": 1000,
    "dt": 0.1,
    "warmup_steps": 100
  }
}
