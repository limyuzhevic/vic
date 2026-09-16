# Integration Tests

Comprehensive integration tests validating system component interactions and phase 6 system integration.

## Test Coverage

### 1. Phase 6 Integration Tests
Tests complete system integration following Phase 6 system improvements.

**Files:**
- `test_phase6_integration.cpp` - Complete system integration testing
- `test_phase6_components.cpp` - Phase 6 specific component interactions
- `test_phase6_reproduction.cpp` - Reproduction of Phase 6 results

**Key Tests:**
```cpp
// Test complete Phase 6 system integration
void testPhase6SystemIntegration() {
    Phase6IntegratedExperiment experiment;
    
    // Configure system with Phase 6 improvements
    auto config = experiment.createPhase6Config();
    
    // Initialize complete system
    experiment.initialize(config);
    
    // Run experiment simulation
    for (int step = 0; step < 1000; ++step) {
        experiment.step(step);
        
        // Verify system remains stable
        assertSystemHealth(experiment.getSystemHealth());
        
        // Check for emergent behaviors
        if (step % 100 == 0) {
            assertEmergentBehaviors(experiment.getCurrentState());
        }
    }
    
    // Verify Phase 6 performance targets
    assertPerformanceTargets(experiment.getPerformanceMetrics());
}

// Test working memory with concept formation integration
void testWorkingMemoryConceptFormationIntegration() {
    ConceptFormation conceptForm(workingMemory.getWinners(), workingMemory.retrieve());
    
    // Process sequence of experiences
    for (int step = 0; step < 100; ++step) {
        std::vector<float> sensoryState = getSensoryState(step);
        std::vector<float> features = extractFeatures(sensoryState);
        
        size_t conceptId = conceptForm.presentExperience(
            sensoryState, features, 0.5f + (step % 10) * 0.05f, step
        );
        
        // Store concept in working memory for quick access
        if (conceptId > 0) {
            conceptForm.storeConceptInWorkingMemory(conceptId);
        }
        
        // Verify concept stability over time
        assertConceptStability(conceptForm, conceptId, step);
    }
}
```

### 2. End-to-End Tests
Simulates real-world scenarios and complete agent behaviors.

**Files:**
- `test_e2e_learning.cpp` - End-to-end learning scenarios
- `test_e2e_navigation.cpp` - Navigation and exploration scenarios
- `test_e2e_interaction.cpp` - Agent-environment interaction scenarios

**Key Tests:**
```cpp
// Test end-to-end learning scenario
void testEndToEndLearning() {
    Agent agent(brain);
    Environment environment;
    
    // Initialize agent and environment
    agent.initialize(environment);
    
    // Simulate learning episode
    for (int episode = 0; episode < 50; ++episode) {
        // Reset agent for new episode
        agent.reset();
        
        // Run episode until completion
        bool episodeComplete = false;
        for (int step = 0; step < 500 && !episodeComplete; ++step) {
            // Get sensory input
            auto percept = agent.getSensoryInput();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Generate motor command
            auto action = agent.generateAction();
            
            // Apply action to environment
            episodeComplete = environment.applyAction(action);
            
            // Get reward and apply neuromodulation
            float reward = environment.getReward();
            agent.applyRewardModulation(reward, agent.getPredictedReward());
            
            // Update development system
            agent.updateDevelopment(0.1f);
        }
        
        // Verify learning progress
        assertLearningProgress(episode, agent.getNeuromodulationLevel());
    }
}

// Test navigation and exploration behavior
void testNavigationAndExploration() {
    Agent agent(brain);
    Environment environment;
    agent.initialize(environment);
    
    // Track exploration metrics
    std::vector<Position> visitedPositions;
    float totalDistance = 0.0f;
    int novelObservations = 0;
    
    for (int step = 0; step < 1000; ++step) {
        Position currentPos = environment.getAgentPosition();
        
        // Check for novel positions (exploration)
        if (!isPositionVisited(visitedPositions, currentPos)) {
            visitedPositions.push_back(currentPos);
            novelObservations++;
        }
        
        // Move agent
        auto action = agent.generateAction();
        Position nextPos = environment.getNextPosition(action);
        totalDistance += distance(currentPos, nextPos);
        
        // Apply action
        environment.applyAction(action);
        
        // Update agent state
        agent.update(0.1f);
        
        // Verify exploration efficiency
        assertExplorationEfficiency(
            step, visitedPositions.size(), totalDistance, novelObservations
        );
    }
}
```

### 3. Cross-Component Interaction Tests
Tests interactions between different system components.

**Files:**
- `test_prediction_memory_integration.cpp` - Prediction and memory integration
- `test_attention_working_memory.cpp` - Attention and working memory interaction
- `test_neuromodulation_plasticity.cpp` - Neuromodulation and plasticity integration

**Key Tests:**
```cpp
// Test prediction and working memory integration
void testPredictionWorkingMemoryIntegration() {
    NeuralPrediction predictor(brain);
    NeuralWorkingMemory memory(brain);
    
    predictor.initialize(brain);
    memory.initialize(brain);
    
    // Process sequence of sensory states
    for (int step = 0; step < 100; ++step) {
        std::vector<float> sensoryState = getSensoryState(step);
        
        // Record in predictor
        predictor.recordSensoryState(sensoryState, step);
        
        // Generate prediction
        auto prediction = predictor.generatePrediction(step + 1);
        
        // Store predicted state in working memory
        memory.store(prediction, 0.9f);
        
        // Update predictor with actual observation
        float error = predictor.updateWithObservation(sensoryState, step + 1);
        
        // Verify integration quality
        assertPredictionMemoryIntegration(error, memory.getMemoryActivity());
    }
}

// Test attention and working memory interaction
void testAttentionWorkingMemoryIntegration() {
    AttentionalSelection attention(brain);
    NeuralWorkingMemory memory(brain);
    
    attention.initialize(brain);
    memory.initialize(brain);
    
    std::vector<NeuronId> competitors = getRelevantNeurons();
    
    for (int step = 0; step < 100; ++step) {
        // Apply bottom-up salience
        for (NeuronId neuron : competitors) {
            attention.applyBottomUpSalience(neuron, getSalience(neuron, step));
        }
        
        // Process competition
        std::vector<NeuronId> winners = attention.processCompetition(
            competitors, 0.5f
        );
        
        // Apply attentional focus
        attention.focusOnRegion(getCurrentRegion(step));
        
        // Store winners in working memory
        for (NeuronId winner : winners) {
            memory.storeToNeuron(winner, attention.getExcitationFor(winner));
        }
        
        // Update systems
        attention.update(0.1f);
        memory.update(0.1f);
        
        // Verify integration
        assertAttentionMemoryIntegration(
            attention.getWinners(), memory.getMemoryNeurons()
        );
    }
}
```

### 4. Performance Regression Tests
Ensures performance improvements are maintained.

**Files:**
- `test_performance_regression.cpp` - Performance regression testing
- `test_memory_usage.cpp` - Memory usage validation
- `test_processing_speed.cpp` - Processing speed validation

**Key Tests:**
```cpp
// Test performance regression with comprehensive metrics
void testPerformanceRegression() {
    PerformanceProfiler profiler;
    
    // Baseline measurements
    profiler.startMeasurement("system_startup");
    auto config = createPhase6Config();
    profiler.stopMeasurement("system_startup");
    
    profiler.startMeasurement("initialization");
    System system(config);
    system.initialize();
    profiler.stopMeasurement("initialization");
    
    profiler.startMeasurement("steady_state");
    for (int step = 0; step < 1000; ++step) {
        system.step(step);
    }
    profiler.stopMeasurement("steady_state");
    
    profiler.startMeasurement("teardown");
    system.shutdown();
    profiler.stopMeasurement("teardown");
    
    // Analyze performance metrics
    auto metrics = profiler.getMetrics();
    
    // Check for regressions
    assertNoPerformanceRegression(metrics, {
        {"system_startup", 1.0f},
        {"initialization", 2.0f},
        {"steady_state", 1.5f},
        {"teardown", 1.0f}
    });
    
    // Generate performance report
    profiler.generateReport("performance_regression_report.html");
}

// Test memory usage patterns
void testMemoryUsage() {
    MemoryProfiler memoryProfiler;
    
    // Measure memory usage during different phases
    for (int phase = 0; phase < 3; ++phase) {
        memoryProfiler.startMeasurement();
        
        // Run system in specific phase
        switch (phase) {
            case 0: runLearningPhase(); break;
            case 1: runMaintenancePhase(); break;
            case 2: runRecoveryPhase(); break;
        }
        
        memoryProfiler.stopMeasurement();
        
        // Check memory limits
        assertMemoryUsageWithinLimits(
            memoryProfiler.getCurrentUsage(),
            getMemoryLimit(phase)
        );
        
        // Force garbage collection
        if (phase < 2) {
            memoryProfiler.forceGarbageCollection();
        }
    }
}
```

## Test Framework Integration

Integration tests leverage advanced testing framework capabilities:

```cpp
// Integration test base class
class IntegrationTestBase {
protected:
    bool setupComponents() {
        // Initialize all components needed for integration
        bool success = initializeComponents();
        setupComplete_ = success;
        return success;
    }
    
    void verifySystemState() {
        // Verify that all components are in consistent state
        for (auto& component : components_) {
            assertComponentConsistency(component);
        }
    }
    
    void cleanupComponents() {
        // Ensure proper cleanup of all components
        for (auto& component : components_) {
            component->cleanup();
        }
    }
};
```

## Test Execution

### Running Integration Tests

```bash
# Build integration tests
mkdir -p build && cd build
cmake -DCMAKE_BUILD_INTEGRATION_TESTS=ON ..
make

# Run all integration tests
./nlm_test_suite --suite=integration

# Run specific integration test category
./nlm_test_suite --suite=phase6
./nlm_test_suite --suite=e2e
./nlm_test_suite --suite=cross_component
./nlm_test_suite --suite=performance_regression
```

### Integration Test Configuration

```bash
# Integration test options
./nlm_test_suite --suite=integration \
    --iterations=500 \
    --load-level=5 \
    --precision=double \
    --memory-limit=8 \
    --enable-performance-validation \
    --enable-stress-testing
```

The integration test suite ensures that all NLM system improvements work together correctly, maintaining system stability and performance while delivering expected emergent behaviors.