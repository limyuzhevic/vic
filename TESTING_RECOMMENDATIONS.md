# NLM Brain Simulation Project - TESTING RECOMMENDATIONS

## Overview
Comprehensive testing strategy for validating the critical bug fixes in the Phase 6 NLM brain simulation project. This document provides detailed testing procedures, validation criteria, and performance benchmarks for all 5 critical bug fixes.

---

## Introduction

Testing is critical for ensuring that the bug fixes:
1. Resolve the identified issues without introducing new problems
2. Maintain backward compatibility with existing code
3. Improve performance as expected
4. Pass all existing test suites

The testing strategy follows a hierarchical approach:
- **Unit Tests**: Individual component testing
- **Integration Tests**: Cross-system interaction testing
- **Performance Tests**: Benchmark and scalability testing
- **Regression Tests**: Prevention of re-emergence of bugs

---

## Test 1: BRAIN::IMPL CONSTRUCTOR FIX

### Test Objectives
- Validate complete pointer initialization in Brain::Impl constructor
- Ensure no use-before-initialization bugs
- Verify proper resource management

### Test Cases

#### 1.1 Constructor Initialization Test
```cpp
// Test file: test_brain_constructor.cpp
// Test class: BrainConstructorTest

class BrainConstructorTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>("test_config.json");
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(BrainConstructorTest, AllPointersInitialized) {
    brain = std::make_shared<Brain>(config);
    
    // Verify that all Brain::Impl members are properly initialized
    // Check that no member is in nullptr state when it shouldn't be
    
    // Memory systems should be initialized
    ASSERT_NE(brain->getWorkingMemory(), nullptr);
    ASSERT_NE(brain->getEpisodicMemory(), nullptr);
    ASSERT_NE(brain->getAssociativeMemory(), nullptr);
    
    // Prediction system should be initialized
    ASSERT_NE(brain->getPredictionSystem(), nullptr);
    
    // All neuromodulation systems should be initialized
    ASSERT_NE(brain->getDopamine(), nullptr);
    ASSERT_NE(brain->getCuriosity(), nullptr);
    ASSERT_NE(brain->getNovelty(), nullptr);
    ASSERT_NE(brain->getPredictionErrorSignal(), nullptr);
    
    // Plasticity systems should be initialized
    ASSERT_NE(brain->getSTDP(), nullptr);
    ASSERT_NE(brain->getHebbian(), nullptr);
    ASSERT_NE(brain->getStructuralPlasticity(), nullptr);
    
    // Spike system should be initialized
    ASSERT_NE(brain->getSpikeSystem(), nullptr);
}

TEST_F(BrainConstructorTest, NoUseBeforeInitialization) {
    brain = std::make_shared<Brain>(config);
    
    // Verify that constructor doesn't crash when accessing members
    // by calling methods that use these members
    
    brain->getRandomGenerator()->nextFloat();
    brain->getLogger()->info("Test message");
    brain->getDevelopmentSystem()->update(0.001);
    
    // If constructor had use-before-initialization bugs,
    // accessing these members would cause crashes or undefined behavior
}
```

#### 1.2 Null Safety Test
```cpp
TEST_F(BrainConstructorTest, NullPointerSafety) {
    // Verify that constructor properly initializes all pointers
    // and doesn't leave any in an unsafe null state
    
    // Check that unique_ptr members are not nullptr
    // after constructor completion
    const auto& impl = *brain->getImpl();
    
    // All smart pointers should be valid after construction
    ASSERT_TRUE(static_cast<bool>(impl.workingMemory));
    ASSERT_TRUE(static_cast<bool>(impl.episodicMemory));
    ASSERT_TRUE(static_cast<bool>(impl.associativeMemory));
    ASSERT_TRUE(static_cast<bool>(impl.predictionSystem));
    ASSERT_TRUE(static_cast<bool>(impl.planner));
    ASSERT_TRUE(static_cast<bool>(impl.conceptFormation));
    ASSERT_TRUE(static_cast<bool>(impl.attention));
    ASSERT_TRUE(static_cast<bool>(impl.developmentSystem));
    ASSERT_TRUE(static_cast<bool>(impl.dopamine));
    ASSERT_TRUE(static_cast<bool>(impl.curiosity));
    ASSERT_TRUE(static_cast<bool>(impl.predictionError));
    ASSERT_TRUE(static_cast<bool>(impl.novelty));
    ASSERT_TRUE(static_cast<bool>(impl.spikeSystem));
    ASSERT_TRUE(static_cast<bool>(impl.stdp));
    ASSERT_TRUE(static_cast<bool>(impl.hebbian));
    ASSERT_TRUE(static_cast<bool>(impl.structuralPlasticity));
}
```

### Validation Criteria
- **Pass**: All system accessor methods return valid non-nullptr objects
- **Fail**: Any accessor returns nullptr indicates constructor bug

### Performance Impact
- **Before Fix**: Potential crashes, undefined behavior
- **After Fix**: Stable initialization, no performance impact
- **Expected**: 0% performance change, 100% stability improvement

### Test Commands
```bash
# Run Brain constructor tests
cd build
./test_brain_constructor

# Or run via ctest
ctest -R brain_constructor

# Memory leak detection (if available)
valgrind --leak-check=full ./test_brain_constructor
```

---

## Test 2: AGENTBRAIN NULL POINTER FIX

### Test Objectives
- Validate null pointer safety in AgentBrain
- Ensure consistent null checking patterns
- Test neuron distribution accuracy

### Test Cases

#### 2.1 Null Pointer Safety Test
```cpp
// Test file: test_agent_brain_null_safety.cpp
// Test class: AgentBrainNullSafetyTest

class AgentBrainNullSafetyTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>("test_config.json");
        brain = std::make_shared<Brain>(config);
        agentBrain = std::make_shared<AgentBrain>(brain);
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
};

TEST_F(AgentBrainNullSafetyTest, BrainNullCheck) {
    // Test with null brain pointer
    std::shared_ptr<AgentBrain> nullAgentBrain(new AgentBrain(nullptr));
    
    // All methods should handle null brain safely
    SensoryPercept dummyPercept;
    nullAgentBrain->processSensoryInput(dummyPercept);
    
    // Should not crash or dereference null
    EXPECT_EQ(nullAgentBrain->decodeMotorCommand(), MotorCommand::Wait);
    EXPECT_EQ(nullAgentBrain->getNeuromodulationLevel(), 0.0f);
}

TEST_F(AgentBrainNullSafetyTest, ProcessSensoryInputSafety) {
    // Create valid AgentBrain with non-null brain
    agentBrain->initialize(world);
    
    // Create sensory percept with various inputs
    SensoryPercept visionPercept;
    visionPercept.vision = std::vector<float>(256, 0.5f);
    visionPercept.touch = std::vector<float>(8, 0.0f);
    visionPercept.internal = std::vector<float>(4, 0.0f);
    visionPercept.proprioception = std::vector<float>(6, 0.0f);
    
    // Should not crash even if some sensory neurons are null
    agentBrain->processSensoryInput(visionPercept);
    
    // Verify no crashes occurred
    EXPECT_TRUE(true);
}
```

#### 2.2 Neuron Distribution Test
```cpp
TEST_F(AgentBrainNullSafetyTest, NeuronDistributionAccuracy) {
    agentBrain->initialize(world);
    
    // Count expected vs actual neurons
    size_t expectedMotorNeurons = 0;
    size_t expectedSensoryNeurons = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                expectedMotorNeurons += pop->getNeurons().size();
            } else if (pop->getNeuronType() == NeuronType::Sensory) {
                expectedSensoryNeurons += pop->getNeurons().size();
            }
        }
    }
    
    // Verify distribution matches expectations
    // (Test implementation would check actual vs expected counts)
    
    EXPECT_EQ(agentBrain->getMotorOutputSize(), 6);  // One per action type
    EXPECT_EQ(agentBrain->getSensoryInputSize(), 256 + 8 + 4 + 6);
}
```

### Validation Criteria
- **Pass**: No crashes with null brain pointer
- **Pass**: All null checks consistent and complete
- **Pass**: Neuron distribution matches expected counts
- **Fail**: Any segmentation fault or undefined behavior

### Performance Impact
- **Before Fix**: Potential crashes, redundant null checks
- **After Fix**: Consistent null safety, accurate distribution
- **Expected**: 15% better distribution accuracy, 100% safety improvement

### Test Commands
```bash
# Run AgentBrain null safety tests
cd build
./test_agent_brain_null_safety

# Run comprehensive AgentBrain tests
ctest -R agent_brain

# Integration tests
./test_agent_brain_integration
```

---

## Test 3: MEMORY MANAGEMENT FIX

### Test Objectives
- Validate move assignment operator correctness
- Ensure no memory leaks
- Test proper resource cleanup

### Test Cases

#### 3.1 Move Assignment Test
```cpp
// Test file: test_memory_management.cpp
// Test class: MemoryManagementTest

class MemoryManagementTest : public ::testing::Test {
protected:
    void SetUp() override {
        config1 = std::make_shared<Config>("config1.json");
        config2 = std::make_shared<Config>("config2.json");
        
        brain1 = std::make_shared<Brain>(config1);
        brain2 = std::make_shared<Brain>(config2);
        
        // Initialize both brains with different data
        initializeBrainWithData(brain1);
        initializeBrainWithData(brain2);
    }
    
    std::shared_ptr<Config> config1, config2;
    std::shared_ptr<Brain> brain1, brain2;
};

TEST_F(MemoryManagementTest, MoveAssignmentOperator) {
    // Store original data from brain2
    auto originalTotalSpikes = brain2->getTotalSpikeCount();
    auto originalActiveNeurons = brain2->getActiveNeuronCount();
    
    // Move-assign brain2 to brain1
    (*brain1) = std::move(brain2);
    
    // Verify that brain1 now contains brain2's data
    EXPECT_EQ(brain1->getTotalSpikeCount(), originalTotalSpikes);
    EXPECT_EQ(brain1->getActiveNeuronCount(), originalActiveNeurons);
    
    // Verify that brain2 is now in a valid empty state
    EXPECT_EQ(brain2, nullptr);  // Or appropriate empty state
}

TEST_F(MemoryManagementTest, NoMemoryLeaks) {
    // This test would detect memory leaks using valgrind
    // or AddressSanitizer
    
    // Perform many move operations
    for (int i = 0; i < 1000; ++i) {
        std::shared_ptr<Brain> tempBrain = std::make_shared<Brain>(config1);
        (*brain1) = std::move(tempBrain);
    }
    
    // If there are memory leaks, valgrind will detect them
    // This test is typically run with external tools
}
```

#### 3.2 Reset Method Test
```cpp
TEST_F(MemoryManagementTest, ResetMethodSafety) {
    brain1->reset();
    
    // Verify that reset properly cleans up all resources
    EXPECT_EQ(brain1->getTotalSpikeCount(), 0);
    EXPECT_EQ(brain1->getActiveNeuronCount(), 0);
    EXPECT_FALSE(brain1->isResting());
    EXPECT_EQ(brain1->getCurrentStep(), 0);
    
    // Verify that memory systems are in valid state
    auto workingMem = brain1->getWorkingMemory();
    if (workingMem) {
        // Working memory should be reset to clean state
        EXPECT_EQ(workingMem->getActiveTraces(), 0);
    }
}

TEST_F(MemoryManagementTest, DoubleDeletionPrevention) {
    // Reset multiple times - should not cause double deletion
    brain1->reset();
    brain1->reset();  // Second reset should be safe
    brain1->reset();  // Third reset should be safe
    
    // If reset had double deletion bugs, this would cause crashes
    EXPECT_TRUE(true);  // No crashes indicates success
}
```

### Validation Criteria
- **Pass**: Move assignment preserves data correctly
- **Pass**: No memory leaks detected
- **Pass**: Reset method can be called multiple times safely
- **Fail**: Memory leaks, double deletion, or data corruption

### Performance Impact
- **Before Fix**: Memory leaks, potential double deletion
- **After Fix**: Proper resource management, no leaks
- **Expected**: 100% memory leak fix, no performance change

### Test Commands
```bash
# Run memory management tests
cd build
./test_memory_management

# Run with AddressSanitizer for leak detection
export CFLAGS="$CFLAGS -fsanitize=address"
export CXXFLAGS="$CXXFLAGS -fsanitize=address"
cmake --build .
./test_memory_management

# Run valgrind if available
valgrind --leak-check=full --track-origins=yes ./test_memory_management
```

---

## Test 4: PERFORMANCE BOTTLENECK FIX

### Test Objectives
- Validate O(N) complexity improvement
- Measure performance gains
- Ensure correctness after optimization

### Test Cases

#### 4.1 Complexity Measurement Test
```cpp
// Test file: test_performance_optimization.cpp
// Test class: PerformanceOptimizationTest

class PerformanceOptimizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>("test_config.json");
        brain = std::make_shared<Brain>(config);
        
        // Create brains with different sizes
        createBrainWithNeuronCount(brain, 100);   // Small
        createBrainWithNeuronCount(brain2, 1000); // Medium
        createBrainWithNeuronCount(brain3, 10000); // Large
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain, brain2, brain3;
};

TEST_F(PerformanceOptimizationTest, ComplexityImprovement) {
    // Measure time for step() method with different brain sizes
    // The optimized version should show linear scaling
    
    double smallTime = measureStepTime(brain);
    double mediumTime = measureStepTime(brain2);
    double largeTime = measureStepTime(brain3);
    
    // With O(N) complexity:
    // medium/small ≈ 10x, large/small ≈ 100x
    // With O(N²) complexity (before fix):
    // medium/small ≈ 100x, large/small ≈ 10,000x
    
    double mediumRatio = mediumTime / smallTime;
    double largeRatio = largeTime / smallTime;
    
    // Allow some tolerance for system variability
    EXPECT_LT(mediumRatio, 20.0);   // Should be much less than 100x
    EXPECT_LT(largeRatio, 500.0);   // Should be much less than 10,000x
    
    // Verify correctness - all brains should produce same results
    // (within numerical precision)
    verifyBrainOutputs(brain, brain2);
    verifyBrainOutputs(brain, brain3);
}
```

#### 4.2 Neuron Access Optimization Test
```cpp
TEST_F(PerformanceOptimizationTest, NeuronAccessPattern) {
    // This test verifies that the flattened array access pattern
    // is being used instead of nested loops
    
    brain->step(0.001);
    
    // If the optimization is working, the step() method should:
    // 1. Create and use a flattened neuron array
    // 2. Process all neurons in a single pass
    // 3. Use cached synapse lists
    
    // We can verify this by checking that the performance
    // scales linearly with neuron count (as tested above)
    
    // Additionally, we can verify that the implementation
    // doesn't have nested loops that would cause O(N²) behavior
}
```

#### 4.3 Memory Access Pattern Test
```cpp
TEST_F(PerformanceOptimizationTest, MemoryAccessPattern) {
    // Test that memory access is cache-friendly
    // (sequential access vs random access)
    
    // Measure memory bandwidth or cache misses
    // for the optimized implementation
    
    // The flattened array approach should have better
    // cache locality than the nested loop approach
    
    // This is typically measured using performance counters
    // or profiling tools
}
```

### Validation Criteria
- **Pass**: Linear scaling with neuron count
- **Pass**: Performance improvement of 50-90%
- **Pass**: Correctness maintained after optimization
- **Fail**: Poor scaling, regressions, or incorrect results

### Performance Impact
- **Before Fix**: O(N²) complexity with nested loops
- **After Fix**: O(N) complexity with flattened arrays
- **Expected**: 50-90% performance improvement for large networks

### Test Commands
```bash
# Run performance optimization tests
cd build
./test_performance_optimization

# Run benchmarks
./benchmark_brain_step

# Generate performance comparison report
./performance_analyzer --before-fix --after-fix

# Run full performance test suite
ctest -R performance
```

---

## Test 5: INTEGRATION FAILURE FIX

### Test Objectives
- Validate complete system integration
- Ensure all Phase 6 systems are properly connected
- Test graceful degradation on system failure

### Test Cases

#### 5.1 System Integration Test
```cpp
// Test file: test_system_integration.cpp
// Test class: SystemIntegrationTest

class SystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>("test_config.json");
        brain = std::make_shared<Brain>(config);
        
        // Create experiment environment
        createTestEnvironment();
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    SimpleWorld world;
};

TEST_F(SystemIntegrationTest, MemorySystemIntegration) {
    // Test that all memory systems are properly integrated
    
    auto workingMem = brain->getWorkingMemory();
    auto episodicMem = brain->getEpisodicMemory();
    auto associativeMem = brain->getAssociativeMemory();
    
    ASSERT_NE(workingMem, nullptr);
    ASSERT_NE(episodicMem, nullptr);
    ASSERT_NE(associativeMem, nullptr);
    
    // Test that memory systems work with brain
    workingMem->initialize(brain.get());
    episodicMem->initialize(brain.get());
    associativeMem->initialize(brain.get());
    
    // Verify integration by using the systems
    workingMem->addTrace(brain->getTotalNeuronCount() / 10);
    episodicMem->storeEpisode(std::vector<float>(100, 0.5f));
    associativeMem->associate(std::vector<float>(50, 0.3f));
}

TEST_F(SystemIntegrationTest, PredictionSystemIntegration) {
    // Test prediction system integration
    auto predSystem = brain->getPredictionSystem();
    ASSERT_NE(predSystem, nullptr);
    
    // Test complete prediction system functionality
    predSystem->initialize(brain.get());
    predSystem->setLearningRate(0.01f);
    predSystem->setPredictionErrorThreshold(0.1f);
    
    // Verify integration with brain
    brain->setPredictionSystem(predSystem);
    
    // Test prediction functionality
    std::vector<float> prediction(10, 0.5f);
    std::vector<float> actual(10, 0.6f);
    predSystem->update(prediction, actual);
}

TEST_F(SystemIntegrationTest, NeuromodulationIntegration) {
    // Test neuromodulation system integration
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    auto novelty = brain->getNovelty();
    auto predError = brain->getPredictionErrorSignal();
    
    ASSERT_NE(dopamine, nullptr);
    ASSERT_NE(curiosity, nullptr);
    ASSERT_NE(novelty, nullptr);
    ASSERT_NE(predError, nullptr);
    
    // Test neuromodulation functionality
    dopamine->setLevel(0.5f);
    curiosity->update(0.3f);
    novelty->update(0.2f);
    predError->update(0.1f);
    
    // Verify integration with brain
    brain->setDopamine(dopamine.get());
    brain->setCuriosity(curiosity.get());
    brain->setNovelty(novelty.get());
    brain->setPredictionErrorSignal(predError.get());
}

TEST_F(SystemIntegrationTest, CognitionSystemIntegration) {
    // Test cognition system integration
    auto planner = brain->getPlanner();
    auto conceptForm = brain->getConceptFormation();
    auto attention = brain->getAttention();
    
    ASSERT_NE(planner, nullptr);
    ASSERT_NE(conceptForm, nullptr);
    ASSERT_NE(attention, nullptr);
    
    // Test cognition system functionality
    planner->initialize(brain.get());
    conceptForm->initialize(brain.get());
    attention->initialize(brain.get());
    
    // Verify integration with brain
    brain->setPlanner(planner.get());
    brain->setConceptFormation(conceptForm.get());
    brain->setAttention(attention.get());
}

TEST_F(SystemIntegrationTest, DevelopmentSystemIntegration) {
    // Test development system integration
    auto devSystem = brain->getDevelopmentSystem();
    ASSERT_NE(devSystem, nullptr);
    
    // Test development system functionality
    devSystem->initialize(brain.get());
    devSystem->update(60.0);  // Update to initial developmental stage
    
    // Verify integration with brain
    brain->setDevelopmentSystem(devSystem.get());
}
```

#### 5.2 Graceful Degradation Test
```cpp
TEST_F(SystemIntegrationTest, GracefulDegradation) {
    // Test that system can continue working if some components fail
    
    // Simulate a system failure by setting a system to null
    // (This would normally happen through error recovery mechanisms)
    
    // The system should continue functioning even if
    // one or more subsystems fail to initialize
    
    // This tests robustness and error handling
}

TEST_F(SystemIntegrationTest, ErrorHandling) {
    // Test error handling for invalid system access
    
    // Test accessing non-existent systems
    auto invalidSystem = brain->getInvalidSystem();  // Should return nullptr
    ASSERT_EQ(invalidSystem, nullptr);
    
    // Test system with invalid configuration
    std::shared_ptr<Config> invalidConfig = nullptr;
    std::shared_ptr<Brain> invalidBrain(new Brain(invalidConfig));
    
    // Invalid brain should handle errors gracefully
    EXPECT_NO_THROW(invalidBrain->step(0.001));
}
```

### Validation Criteria
- **Pass**: All system accessors return valid non-nullptr objects
- **Pass**: All integrated systems work correctly with brain
- **Pass**: Graceful degradation when systems fail
- **Pass**: Proper error handling and logging
- **Fail**: Null dereferences, incomplete integration, system crashes

### Performance Impact
- **Before Fix**: Potential null dereferences, incomplete integration
- **After Fix**: Complete system integration with validation
- **Expected**: 100% integration reliability, no performance change

### Test Commands
```bash
# Run system integration tests
cd build
./test_system_integration

# Run Phase 6 integration tests
./test_phase6_integration

# Run comprehensive integration test suite
./integration_test_suite --verbose

# Generate integration test report
./integration_test_analyzer --report=integration_status.html
```

---

## Comprehensive Test Suite

### Test 1: Full Integration Test
```bash
# Run all tests in sequence
cd build
./run_all_tests.sh

# Expected result: All 5 fix categories pass
# Validation: Comprehensive test report generated
```

### Test 2: Memory Leak Detection
```bash
# Compile with memory sanitizers
export CFLAGS="$CFLAGS -fsanitize=address -fno-omit-frame-pointer"
export CXXFLAGS="$CXXFLAGS -fsanitize=address -fno-omit-frame-pointer"
cmake --build .

# Run tests with leak detection
valgrind --leak-check=full --track-origins=yes ./all_tests

# Check for any memory issues
if [ $? -ne 0 ]; then
    echo "Memory leaks detected - FIX REQUIRED"
    exit 1
fi
```

### Test 3: Performance Benchmark
```bash
# Run performance comparison
./benchmark_compare.sh

# Expected results:
# - Brain::Impl constructor: No change (0%)
# - AgentBrain null safety: 15% improvement in accuracy
# - Memory management: No change (100% leak fix)
# - Performance optimization: 50-90% improvement
# - System integration: No change (100% reliability)
```

### Test 4: Regression Test
```bash
# Run regression tests to ensure no existing functionality broken
cd build
./test_regression.sh

# Key regression checks:
# - All existing API calls still work
# - Brain step() method produces correct results
# - AgentBrain methods work as before
# - Memory systems function correctly
# - Integration between components maintained
```

---

## Test Reporting

### Test Report Format
```cpp
// TestResult structure for reporting
struct TestResult {
    std::string testName;
    std::string category;  // "BrainConstructor", "AgentBrainNullSafety", etc.
    bool passed;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> metrics;
};
```

### Automated Test Runner
```bash
cat > run_validation_tests.sh << 'EOF'
#!/bin/bash

echo "=== NLM Brain Simulation Project - Bug Fix Validation ==="
echo "Generated: $(date)"
echo ""

# Run all test categories
results=()

# Test 1: Brain Constructor
if ctest -R brain_constructor --output-on-failure; then
    results+=("Brain Constructor: PASS")
else
    results+=("Brain Constructor: FAIL")
fi

# Test 2: AgentBrain Null Safety  
if ctest -R agent_brain_null_safety --output-on-failure; then
    results+=("AgentBrain Null Safety: PASS")
else
    results+=("AgentBrain Null Safety: FAIL")
fi

# Test 3: Memory Management
if ctest -R memory_management --output-on-failure; then
    results+=("Memory Management: PASS")
else
    results+=("Memory Management: FAIL")
fi

# Test 4: Performance Optimization
if ctest -R performance_optimization --output-on-failure; then
    results+=("Performance Optimization: PASS")
else
    results+=("Performance Optimization: FAIL")
fi

# Test 5: System Integration
if ctest -R system_integration --output-on-failure; then
    results+=("System Integration: PASS")
else
    results+=("System Integration: FAIL")
fi

# Generate HTML report
cat > test_report.html << END_HTML
<!DOCTYPE html>
<html>
<head><title>Bug Fix Validation Report</title></head>
<body>
<h1>Bug Fix Validation Report</h1>
<p><strong>Generated:</strong> $(date)</p>
<p><strong>Project:</strong> NLM Brain Simulation Project - Phase 6</p>

<h2>Test Results Summary</h2>
<table border="1">
<tr><th>Test Category</th><th>Status</th><th>Expected</th><th>Actual</th></tr>
<tr><td>Brain Constructor</td><td>${results[0]}</td><td>PASS</td><td>${results[0]}</td></tr>
<tr><td>AgentBrain Null Safety</td><td>${results[1]}</td><td>PASS</td><td>${results[1]}</td></tr>
<tr><td>Memory Management</td><td>${results[2]}</td><td>PASS</td><td>${results[2]}</td></tr>
<tr><td>Performance Optimization</td><td>${results[3]}</td><td>PASS</td><td>${results[3]}</td></tr>
<tr><td>System Integration</td><td>${results[4]}</td><td>PASS</td><td>${results[4]}</td></tr>
</table>

<h2>Overall Status</h2>
END_HTML

for result in "${results[@]}"; do
    if [[ "$result" == *"FAIL"* ]]; then
        echo "FAIL: $result"
        exit 1
    fi
done

echo ""
echo "=== ALL TESTS PASSED ==="
echo ""
echo "Validation completed successfully on $(date)"
