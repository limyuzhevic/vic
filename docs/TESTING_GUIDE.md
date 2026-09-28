# NLM Testing Guide

This document provides comprehensive testing guidelines and procedures for the NLM (Neurally Linked Module) project, including new features and commands introduced in Phase 2.

## Overview

The NLM project has been enhanced with several new features and commands that require comprehensive testing:

- **Advanced CLI Testing**: Tests for nlm-config, nlm-sim, nlm-analyze, nlm-viz, and nlm-dev commands
- **New Core Features Testing**: Event system, plasticity mechanisms, multi-region coordination, replay system, and development system
- **Performance Optimizations Testing**: Memory pool system, SIMD optimization, parallel processing, and cache-friendly data structures
- **User Experience Testing**: Interactive debug mode, configuration templates, performance profiling, and export/import systems
- **Integration Testing**: Phase 6 integration, backward compatibility, and system stability

## Test Structure

The NLM testing framework follows a consistent pattern:

### Test File Organization

1. **CLI Tests** (`tests/test_cli.cpp`)
   - Tests all CLI command implementations
   - Validates command-line argument parsing
   - Tests command interactions and state management

2. **Advanced Features Tests** (`tests/test_advanced_features.cpp`)
   - Tests event system with spike queues and priority handling
   - Tests homeostatic and reward-modulated plasticity
   - Tests multi-region neural coordination
   - Tests offline replay and consolidation strategies
   - Tests developmental stages and plasticity rates

3. **Performance Optimizations Tests** (`tests/test_performance_optimizations.cpp`)
   - Tests memory pool allocation/deallocation and statistics
   - Tests SIMD instruction availability and performance
   - Tests thread pool implementation and parallel operations
   - Tests cache-friendly data structures and memory locality

4. **User Experience Tests** (`tests/test_user_experience.cpp`)
   - Tests interactive debug mode and simulation control
   - Tests configuration template loading and application
   - Tests performance profiling and reporting
   - Tests export/import systems with various formats

5. **Integration Tests** (`tests/test_integration.cpp`)
   - Tests Phase 6 integration (memory, cognition, prediction, neuromodulation)
   - Tests backward compatibility with existing functionality
   - Tests system stability under various conditions
   - Tests performance improvements effectiveness

### Test Execution Pattern

Each test file follows this pattern:

```cpp
namespace test_xxx {

void testFeature1() {
    // Test implementation
    assert(condition);
    std::cout << "    testFeature1 passed" << std::endl;
}

void testFeature2() {
    // Test implementation
    assert(condition);
    std::cout << "    testFeature2 passed" << std::endl;
}

void runAll() {
    std::cout << "=== Test Category Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test each feature
    std::cout << "Running Feature 1 tests..." << std::endl;
    try {
        testFeature1();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // More tests...
    
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
    }
}

} // namespace test_xxx
```

## Test Coverage Matrix

### CLI Testing

| Command | Features Tested | Coverage |
|---------|----------------|----------|
| `nlm-config` | Templates, validation, export/import | ✅ Complete |
| `nlm-sim` | Run, pause, resume, status, save/load | ✅ Complete |
| `nlm-analyze` | Spike stats, network dynamics, memory usage, plasticity patterns | ✅ Complete |
| `nlm-viz` | Network, spikes, memory, interactions, interactive | ✅ Complete |
| `nlm-dev` | Info, profile, debug, reset | ✅ Complete |

### Core Features Testing

| Feature | Test Categories | Coverage |
|---------|----------------|----------|
| Event System | Filtering, prioritization, statistics | ✅ Complete |
| Homeostatic Plasticity | Weight balance, targets, rates | ✅ Complete |
| Reward-Modulated Plasticity | Reward signals, eligibility traces | ✅ Complete |
| Multi-Region Coordination | Region interactions, synchronization | ✅ Complete |
| Enhanced Replay System | Replay strategies, consolidation | ✅ Complete |
| Advanced Development System | Developmental stages, rates | ✅ Complete |

### Performance Optimizations Testing

| Feature | Test Categories | Coverage |
|---------|----------------|----------|
| Memory Pool System | Allocation, deallocation, statistics | ✅ Complete |
| SIMD Optimization System | Instruction availability, performance | ✅ Complete |
| Parallel Processing System | Thread pools, parallel modes | ✅ Complete |
| Cache-Friendly Data Structures | Memory locality, performance | ✅ Complete |

### User Experience Testing

| Feature | Test Categories | Coverage |
|---------|----------------|----------|
| Interactive Debug Mode | User input, simulation control | ✅ Complete |
| Configuration Templates | Template loading, application, validation | ✅ Complete |
| Performance Profiling | Profiling, reporting, export | ✅ Complete |
| Export/Import System | Formats, compression, validation | ✅ Complete |

### Integration Testing

| Test Type | Coverage |
|-----------|----------|
| Phase 6 Integration | Memory, cognition, prediction, neuromodulation |
| Backward Compatibility | Existing functionality preservation |
| System Stability | Various operating conditions |
| Performance Improvements | Optimization effectiveness |

## Example Test Scenarios

The `examples/test_scenarios/` directory contains comprehensive example scenarios demonstrating testing approaches:

1. **Basic Spike Transmission** (`scenario_examples.cpp`)
   - Tests complete spike transmission pipeline
   - Verifies synaptic plasticity updates
   - Demonstrates timing-dependent mechanisms

2. **Memory Pool Efficiency**
   - Tests lock-free memory allocation
   - Verifies cache-aligned storage
   - Measures allocation performance

3. **Event-Driven Processing**
   - Tests spike queue management
   - Validates time-bucketed delivery
   - Verifies priority event handling

4. **Parallel Neural Computation**
   - Tests thread pool implementation
   - Measures parallel speedup
   - Verifies load balancing

5. **Configuration Management**
   - Tests template application
   - Verifies export/import functionality
   - Validates error handling

6. **Interactive Debug Session**
   - Tests real-time simulation control
   - Verifies network state inspection
   - Demonstrates step-by-step execution

## Running Tests

### Prerequisites

1. Build NLM with CMake
2. Ensure all dependencies are installed
3. Verify compiler supports C++20 SIMD extensions

### Building Tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Running Tests

```bash
# Run all tests
./build/nlm

# Or run individual test components
./build/test_cli
./build/test_advanced_features
./build/test_performance_optimizations
./build/test_user_experience
./build/test_integration
```

### Performance Testing

For performance tests, ensure the simulation runs with appropriate resources:

```bash
# Run performance tests with verbose output
./build/nlm --enable-performance-logging

# Run specific performance scenarios
./build/nlm --performance-mode=stress
```

## Test Quality Standards

### Code Quality

1. **Error Handling**: All tests include proper exception handling
2. **Assertions**: Use `assert()` for critical validation
3. **Output**: Provide clear pass/fail messages with context
4. **Resource Management**: Properly clean up all allocated resources
5. **Thread Safety**: Test concurrent operations with appropriate synchronization

### Test Completeness

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions
3. **Edge Cases**: Test boundary conditions and error cases
4. **Performance Tests**: Validate optimization effectiveness
5. **Compatibility Tests**: Ensure backward compatibility

### Documentation

1. **Test Coverage**: Document what each test covers
2. **Expected Results**: Specify expected behavior
3. **Dependencies**: List required resources and prerequisites
4. **Limitations**: Note any test limitations or assumptions

## Continuous Integration

### GitHub Actions Integration

```yaml
test.yml
    - name: Run NLM Tests
      run: |
        cmake -B build
        cmake --build build
        ./build/nlm

    - name: Performance Tests
      run: |
        ./build/nlm --performance-mode=stress --verbose
```

### Jenkins Pipeline

```groovy
pipeline {
    stages {
        stage('Unit Tests') {
            steps {
                sh 'cmake -B build'
                sh 'cmake --build build'
                sh './build/nlm'
            }
        }
        stage('Performance Tests') {
            steps {
                sh './build/nlm --performance-mode=stress'
            }
        }
    }
}
```

## Test Maintenance

### Adding New Tests

1. **Follow Existing Patterns**: Use the same structure as existing tests
2. **Maintain Consistency**: Follow the same naming and formatting conventions
3. **Update Documentation**: Add documentation for new test scenarios
4. **Add to CI**: Include new tests in automated test suites

### Updating Tests

1. **Keep Tests Current**: Update tests to reflect code changes
2. **Refactor When Needed**: Refactor tests for clarity and maintainability
3. **Update Documentation**: Update test documentation as needed
4. **Maintain Coverage**: Ensure all functionality remains tested

## Troubleshooting

### Common Issues

1. **Test Failures**: Check for assertion failures and debug output
2. **Memory Issues**: Ensure proper cleanup and check for memory leaks
3. **Performance Issues**: Profile test performance and identify bottlenecks
4. **Compatibility Issues**: Verify tests work with different compiler versions

### Debugging Tips

1. **Enable Verbose Output**: Run tests with verbose flags
2. **Check Configuration**: Verify test configuration is correct
3. **Review Logs**: Check test logs for detailed output
4. **Isolate Issues**: Run individual tests to identify specific problems

## Conclusion

These comprehensive tests ensure that all new NLM features work correctly, maintain backward compatibility, and provide the expected performance improvements. The testing framework is designed to be maintainable, scalable, and robust for long-term project success.

By following this testing guide, developers can ensure that:

- All new features are thoroughly tested
- Performance optimizations are validated
- User experience is optimized
- System integration is seamless
- Backward compatibility is maintained

The NLM testing framework provides confidence that the enhanced system is ready for production use.