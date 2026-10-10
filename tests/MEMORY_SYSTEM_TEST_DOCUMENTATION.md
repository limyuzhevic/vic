# Memory System Integration Test Framework

## Overview

This document describes the comprehensive memory system integration test framework for the NLM neural simulation. The framework tests all memory systems including NeuralWorkingMemory, NeuralEpisodicMemory, and NeuralAssociativeMemory, along with their integration with the Brain system.

## Test Categories

### 1. NeuralWorkingMemory Tests

#### testNeuralWorkingMemoryBasic()
**Purpose**: Test basic working memory functionality
**Description**: Verifies that working memory can store and retrieve neural patterns, handles capacity limits, and maintains memory traces.
**Expected Behavior**:
- Store method accepts vector<float> patterns and strength values
- Retrieve method returns stored activations in correct order
- Memory capacity limits are respected (configurable via setCapacity)
- Memory traces have decay behavior (configurable via setDecayRate)

#### testNeuralWorkingMemoryPersistence()
**Purpose**: Test memory persistence across simulation steps
**Description**: Ensures working memory maintains information during neural dynamics and simulation updates.
**Expected Behavior**:
- Memory capacity is set correctly during initialization
- Memory content persists across brain.step() calls
- Weak traces decay appropriately based on decay rate
- Memory traces have timestamps that track age

#### testNeuralWorkingMemoryIntegration()
**Purpose**: Test integration with Brain and sensory systems
**Description**: Verifies working memory interacts correctly with sensory input, neuron firing, and brain dynamics.
**Expected Behavior**:
- Working memory responds to sensory input through brain.receiveSensoryInput()
- Working memory stores information from firing neurons via storeToNeuron()
- Working memory updates during simulation via update() method
- Competition mechanism selects winning memory traces

### 2. NeuralEpisodicMemory Tests

#### testNeuralEpisodicMemoryBasic()
**Purpose**: Test basic episodic memory storage and retrieval
**Description**: Validates that episodic memory can store experiences and retrieve them by index.
**Expected Behavior**:
- storeEpisode() stores EpisodicMemoryItem with timestamp, sensory state, action, reward
- getEpisodeCount() returns correct number of stored episodes
- getRecentEpisodes() returns most recent episodes in correct order
- All episode fields are preserved during storage/retrieval

#### testNeuralEpisodicMemoryRetrieval()
**Purpose**: Test episodic memory retrieval mechanisms
**Description**: Tests different retrieval strategies: temporal, spatial, and action-based.
**Expected Behavior**:
- retrieveTemporal() finds episodes within specified time windows
- retrieveByLocation() finds episodes near given coordinates (within radius)
- retrieveAfterAction() finds episodes following specific actions
- Retrieval respects maxResults parameter
- Results are sorted by relevance (temporal: most recent first, location: closest first)

#### testNeuralEpisodicMemoryIntegration()
**Purpose**: Test integration with other memory systems
**Description**: Verifies episodic memory works with working memory and replay mechanisms.
**Expected Behavior**:
- replayEpisode() reactivates neural patterns associated with stored episodes
- Integration with working memory for pattern completion
- Memory consolidation based on relevance threshold
- Replay enabled/disabled via enableReplay() method

### 3. NeuralAssociativeMemory Tests

#### testNeuralAssociativeMemoryBasic()
**Purpose**: Test basic associative memory functionality
**Description**: Validates that associative memory can create and retrieve pattern associations.
**Expected Behavior**:
- associate() creates bidirectional connections between pattern vectors
- getAssociationStrength() returns correct strength values
- retrieve() returns patterns associated with query pattern
- Strength values are clamped between 0.0f and 1.0f

#### testNeuralAssociativeMemorySpreading()
**Purpose**: Test spreading activation through associative chains
**Description**: Tests how activation spreads through connected patterns in memory.
**Expected Behavior**:
- spreadActivation() returns patterns that become activated through association chains
- Activation spreads along strongest associations first
- Results respect maximum steps parameter
- visited set prevents cycles in activation spreading

#### testNeuralAssociativeMemoryIntegration()
**Purpose**: Test integration with episodic memory
**Description**: Verifies associative memory learns from episodic experiences.
**Expected Behavior**:
- associateFromExperience() creates associations from episode sensory states
- Action-to-state associations are learned automatically
- Integration with pattern completion for recall

### 4. Integration Tests

#### testMemorySystemIntegration()
**Purpose**: Test coordination between all memory systems
**Description**: Validates that NeuralWorkingMemory, NeuralEpisodicMemory, and NeuralAssociativeMemory work together.
**Expected Behavior**:
- All memory systems are initialized when Brain initializes
- Systems respond correctly to sensory input
- Memory systems interact during simulation steps
- Episodic memory consolidates old episodes
- All systems maintain consistent state

### 5. Performance Tests

#### testMemoryPerformance()
**Purpose**: Test performance characteristics of memory systems
**Description**: Measures execution time of common memory operations.
**Expected Behavior**:
- Memory storage operations complete within reasonable time (< 1 second per 1000 ops)
- Retrieval operations are efficient
- Memory systems scale appropriately with data size
- Performance is consistent across multiple runs

### 6. Error Handling Tests

#### testMemoryErrorHandling()
**Purpose**: Test robustness and error recovery
**Description**: Validates memory systems handle errors gracefully.
**Expected Behavior**:
- Empty patterns are handled without crashing
- Null episode pointers don't cause crashes
- Memory systems remain functional after errors
- Systems provide appropriate error reporting

### 7. Memory Consistency Tests

#### testMemoryConsistency()
**Purpose**: Test memory state consistency
**Description**: Verifies memory systems maintain consistent state during operations.
**Expected Behavior**:
- Pattern storage and retrieval preserve data integrity
- Memory timestamps update correctly
- System state remains consistent during simulation
- Memory content matches expected values after multiple operations

## Test Framework Features

### Template-Based Assertions
The framework uses template-based assertions to test different data types:
- assertEqual<T>() compares expected vs actual values
- assertNotEqual<T>() ensures values differ
- assertTrue()/assertFalse() boolean tests

### Performance Measurement
- startTimer()/stopTimer() measures execution time
- Results stored in TestResult structures
- Performance benchmarks generate reports

### Integration Testing
- Brain system integration tests ensure memory systems work with neural simulation
- Memory persistence tests verify data survives simulation steps
- Cross-system interaction tests validate coordinated behavior

## Building and Running Tests

### Build Instructions
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Running Tests
```bash
# Run all tests
./build/nlm_test

# Run memory system tests specifically
./build/nlm_memory_standalone

# Run memory system tests as CMake target
cmake --build build --target test_memory_system
```

### Test Output
Tests produce verbose output showing:
- Test names and status
- Timing information
- Error messages for failures
- Final summary with pass/fail counts

## Test Configuration

### Default Configuration
- Neuron count: 100-500 neurons depending on test
- Region count: 1-2 regions
- Connection probability: 0.1
- Working memory capacity: neuron_count / 10
- Episodic memory max episodes: 1000

### Performance Configuration
- Working memory decay rate: 0.01f
- Episodic memory max episodes: 1000
- Replay interval: 100 steps
- Consolidation interval: 1000 steps

## Test Verification

### Success Criteria
1. All basic functionality tests pass
2. Integration tests show proper system coordination
3. Performance tests complete within time limits
4. Error handling tests demonstrate robustness
5. Memory consistency tests verify data integrity

### Failure Detection
- Assertion failures report exact values and messages
- Performance tests report execution times
- Integration tests report system state at failure
- Memory consistency tests report corruption or inconsistencies

## Future Enhancements

### Additional Test Categories
- Stress tests for memory systems under heavy load
- Memory scaling tests with increasing data sizes
- Thread safety tests for concurrent access
- Real-world scenario tests with complex behaviors

### Advanced Testing
- Memory persistence across save/load cycles
- Memory compression and decompression tests
- Distributed memory system tests
- Cross-platform compatibility tests

## Conclusion

The Memory System Integration Test Framework provides comprehensive testing of all NLM memory systems. It ensures:
- Correct functionality of individual memory systems
- Proper integration between memory systems
- Robust error handling and recovery
- Acceptable performance characteristics
- Memory consistency and data integrity

These tests are essential for validating the neural simulation's memory capabilities and ensuring reliable operation in complex scenarios.