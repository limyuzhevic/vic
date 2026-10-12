# NLM Cross-System Integration Tests

## Overview

This directory contains comprehensive integration tests for the NLM (Neural Learning Machine) brain systems.
These tests validate the integration of all brain systems and ensure proper cross-system functionality.

## Test Architecture

The integration tests are organized into the following categories:

### 1. Integration Test Framework
- **IntegrationTest.hpp/cpp**: Base class for all integration tests
- **BrainSystemTester.hpp/cpp**: Comprehensive brain system testing
- **IntegrationTestUtils.hpp**: Common test utilities
- **IntegrationTestAssertions.hpp**: Specialized test assertions
- **ExperimentRunnerIntegration.hpp/cpp**: Test experiment runner

### 2. Memory System Integration Tests
- **MemoryIntegrationTester.hpp/cpp**: Working, episodic, and associative memory integration
- Tests memory consolidation across brain regions
- Validates memory-neuromodulation interactions

### 3. Neuromodulation Integration Tests
- **NeuromodulationIntegrationTester.hpp/cpp**: Dopamine, curiosity, novelty, and prediction error integration
- Tests reward-based learning with working memory
- Validates neuromodulation-driven action selection

### 4. Prediction System Integration Tests
- **PredictionIntegrationTester.hpp/cpp**: Forward, value, behavioral, and meta-prediction integration
- Tests prediction with sensory processing
- Validates prediction error-driven learning

### 5. Cognition System Integration Tests
- **CognitionIntegrationTester.hpp/cpp**: Concept formation, attention, planning, and self-model integration
- Tests cognitive processes with memory systems
- Validates cognition-neuromodulation interactions

### 6. Development System Integration Tests
- **DevelopmentIntegrationTester.hpp/cpp**: Plasticity, critical periods, structural plasticity, and adaptive development
- Tests developmental processes across systems
- Validates development-memory interactions

### 7. Cross-System Workflow Tests
- Complete brain loop integration tests
- Memory-development interaction tests
- Neuromodulation-motivation integration tests
- Prediction-behavior integration tests

## Test Data

### Configuration
- **integration_test_config.hpp**: Test configuration settings

### Brain States
- **brain_states.json**: Various brain states for testing (initial, young adult, adult, critical period, aging)

### Experiment Scenarios
- **experiment_scenarios.json**: Different test scenarios and configurations

### Test Cases
- **integration_test_cases.json**: Detailed test cases with parameters and expected outcomes

## Test Implementation Strategy

### Phase 1: Integration Test Framework
- Create base integration test class with common functionality
- Implement test utilities for brain system interaction
- Create specialized assertions for neural system testing
- Set up test data and scenarios

### Phase 2: Memory System Integration Tests
- Test working memory with neural processing
- Test episodic memory with brain regions
- Test associative memory with pattern completion
- Test memory consolidation across regions

### Phase 3: Neuromodulation Integration Tests
- Test reward prediction error with working memory
- Test curiosity with episodic memory exploration
- Test novelty detection with prediction systems
- Test dopamine modulation with action selection

### Phase 4: Prediction System Integration Tests
- Test forward prediction with sensory processing
- Test value prediction with reward systems
- Test behavioral prediction with motor output
- Test meta-prediction with cognitive evaluation

### Phase 5: Cognition System Integration Tests
- Test concept formation with episodic memory retrieval
- Test attention selection with prediction errors
- Test planning with action selection constraints
- Test self-model with social learning integration

### Phase 6: Development System Integration Tests
- Test developmental plasticity with learning processes
- Test critical period effects with neural development
- Test structural plasticity with synaptic remodeling
- Test adaptive development across the lifespan

### Phase 7: Cross-System Workflow Tests
- Test complete brain loop integration
- Test memory consolidation during development
- Test neuromodulation-driven learning
- Test prediction with behavioral adaptation

## Running Integration Tests

### Prerequisites
- CMake 3.16 or higher
- C++20 compiler
- Google Test framework

### Build Instructions

1. Configure CMake:
```bash
cmake -B build -S .
```

2. Build the integration tests:
```bash
cmake --build build
```

3. Run integration tests:
```bash
cd build
./nlm_integration_test
```

### Test Options

The integration test framework supports various test options:

- **Test filtering**: Run specific test categories (memory, neuromodulation, prediction, cognition, development)
- **Test scenarios**: Execute predefined test scenarios
- **Performance monitoring**: Monitor and report test performance metrics
- **Result reporting**: Generate detailed test reports and statistics

## Test Results

The integration test framework provides comprehensive test reporting:

- **Test counts**: Total, passed, and failed test statistics
- **Execution time**: Performance metrics for each test
- **Integration scores**: Cross-system integration quality metrics
- **Error reporting**: Detailed error messages and stack traces
- **Progress tracking**: Real-time test execution progress

## Test Quality Assurance

### Test Coverage
- **Unit integration tests**: Test individual system integrations
- **Cross-system tests**: Test multiple system interactions
- **End-to-end tests**: Test complete brain loop integration
- **Performance tests**: Test integration with performance optimizations
- **Stress tests**: Test integration under load conditions
- **Regression tests**: Test integration with existing functionality

### Validation Criteria

1. **Memory System Integration**
   - Working memory maintains neural activity patterns
   - Episodic memory links experiences across regions
   - Associative memory enables pattern completion
   - Memory consolidation transfers memories between systems

2. **Neuromodulation System Integration**
   - Dopamine modulates working memory plasticity
   - Curiosity drives exploration behavior
   - Novelty enhances prediction learning
   - Prediction error guides action selection

3. **Prediction System Integration**
   - Forward prediction integrates with sensory processing
   - Value prediction integrates with reward systems
   - Behavioral prediction integrates with motor output
   - Meta-prediction integrates with cognitive evaluation

4. **Cognition System Integration**
   - Concept formation uses episodic memory
   - Attention selection uses prediction errors
   - Planning constrains action selection
   - Self-model integrates with social learning

5. **Development System Integration**
   - Plasticity maturation affects memory systems
   - Critical periods enhance learning
   - Structural plasticity modifies neural connectivity
   - Adaptive development optimizes system performance

6. **Cross-System Workflow Integration**
   - Complete brain loop functions properly
   - Memory consolidation occurs during development
   - Neuromodulation drives learning
   - Prediction guides behavioral adaptation

## Integration Test Examples

### Memory Integration Test
```cpp
TEST_F(MemoryIntegrationTester, WorkingMemoryWithNeuralProcessing) {
    // Test that working memory integrates with neural processing
    // Working memory stores neural activity patterns
    // Neural processing updates working memory representations
    // Integration verified by cross-correlation of memory and neural activity
}
```

### Neuromodulation Integration Test
```cpp
TEST_F(NeuromodulationIntegrationTester, RewardWithWorkingMemory) {
    // Test that reward signals modulate working memory updates
    // Dopamine affects working memory plasticity
    // Integration verified by dopamine-dependent memory changes
}
```

### Prediction Integration Test
```cpp
TEST_F(PredictionIntegrationTester, ForwardPredictionWithSensory) {
    // Test that forward prediction integrates with sensory processing
    // Prediction errors modulate sensory representations
    // Integration verified by prediction error signals affecting sensory processing
}
```

## Goal

Create comprehensive integration tests that ensure all NLM brain systems work together correctly, validating the complete integration achieved through the Phase 6 implementation and ensuring no regressions in functionality.

## File Structure

```
src/test/
├── integration/
│   ├── IntegrationTest.hpp
│   ├── IntegrationTest.cpp
│   ├── BrainSystemTester.hpp
│   ├── BrainSystemTester.cpp
│   ├── MemoryIntegrationTester.hpp
│   ├── MemoryIntegrationTester.cpp
│   ├── NeuromodulationIntegrationTester.hpp
│   ├── NeuromodulationIntegrationTester.cpp
│   ├── PredictionIntegrationTester.hpp
│   ├── PredictionIntegrationTester.cpp
│   ├── CognitionIntegrationTester.hpp
│   ├── CognitionIntegrationTester.cpp
│   ├── DevelopmentIntegrationTester.hpp
│   ├── DevelopmentIntegrationTester.cpp
│   ├── ExperimentRunnerIntegration.hpp
│   ├── ExperimentRunnerIntegration.cpp
│   ├── IntegrationTestUtils.hpp
│   ├── IntegrationTestAssertions.hpp
├── data/
│   ├── integration_test_config.hpp
│   ├── brain_states.json
│   ├── experiment_scenarios.json
│   └── integration_test_cases.json
└── CMakeLists.txt
```

## References

- NLM System Architecture Documentation
- Phase 6 Implementation Reports
- Integration Test Requirements Specification
- Cross-System Workflow Documentation