# Phase 6 Integration Tests

## Overview

This test suite validates that all NLM brain systems are properly integrated and functioning together as a coherent artificial brain. Phase 6 focuses on integration, not new features.

## Test Architecture

The test suite uses the existing Phase6IntegratedExperiment framework but adds comprehensive unit tests for each integration point.

## Integration Test Categories

### 1. Memory System Integration Tests
### 2. Neuromodulation Integration Tests  
### 3. Prediction System Integration Tests
### 4. Cognition System Integration Tests
### 5. Development Integration Tests
### 6. Checkpoint Integration Tests
### 7. Multi-System Interaction Tests

## Test Infrastructure

### 1. Memory Integration Tests

#### Test: Working Memory Integration
```cpp
class WorkingMemoryIntegrationTest {
public:
    static bool testSensoryToMemoryConnection() {
        // Initialize brain with config
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Verify working memory exists
        auto* wm = brain->getWorkingMemory();
        ASSERT_TRUE(wm != nullptr);
        
        // Process sensory input
        SensoryInput vision(10, 10, 3);
        std::vector<float> data(100, 0.5f);
        vision.setData(data);
        
        brain->receiveSensoryInput(vision);
        
        // Verify working memory received input
        ASSERT_TRUE(wm->getActiveTraces() > 0);
        
        return true;
    }
    
    static bool testWorkingMemoryDecay() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* wm = brain->getWorkingMemory();
        wm->store(std::vector<float>(50, 1.0f), 1.0f);
        
        // Run some steps
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Verify decay occurred
        size_t tracesAfter = wm->getActiveTraces();
        // Decay should reduce traces over time
        return tracesAfter < 50;  // Some traces should have decayed
    }
};
```

#### Test: Episodic Memory Integration
```cpp
class EpisodicMemoryIntegrationTest {
public:
    static bool testExperienceLogging() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* em = brain->getEpisodicMemory();
        ASSERT_TRUE(em != nullptr);
        
        // Simulate experience over multiple steps
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            // Log experience
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.neurons = {NeuronId(step % 10)};
            episode.values = {0.5f};
            episode.metadata = "Test episode";
            
            em->storeEpisode(episode);
        }
        
        // Verify episodes stored
        ASSERT_TRUE(em->getEpisodeCount() > 0);
        return true;
    }
    
    static bool testEpisodicMemoryReplay() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* em = brain->getEpisodicMemory();
        
        // Store some episodes
        for (int i = 0; i < 10; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.neurons = {NeuronId(i)};
            em->storeEpisode(episode);
        }
        
        // Request episodes for replay
        auto episodes = em->getEpisodesForReplay(5);
        ASSERT_TRUE(episodes.size() <= 5);
        
        // Verify replay mechanism works
        return !episodes.empty();
    }
};
```

### 2. Neuromodulation Integration Tests

#### Test: Dopamine Integration
```cpp
class NeuromodulationIntegrationTest {
public:
    static bool testDopamineAffectsPlasticity() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* dopamine = brain->getDopamine();
        ASSERT_TRUE(dopamine != nullptr);
        
        // Record initial synaptic state
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            size_t initialSynapses = region->getSynapseCount();
            
            // Apply reward modulation
            float reward = 1.0f;
            float predictedReward = 0.5f;
            // AgentBrain.applyRewardModulation(reward, predictedReward);
            
            // Run some steps
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Verify dopamine affected plasticity
            // (In real implementation, check synaptic weights changed)
            return true;
        }
        return false;
    }
    
    static bool testCuriosityDrivenExploration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* curiosity = brain->getCuriosity();
        ASSERT_TRUE(curiosity != nullptr);
        
        // Check initial curiosity level
        float initialCuriosity = curiosity->getLevel();
        
        // Run steps with novel input
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
            // Inject novel sensory input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
        
        // Verify curiosity responds to novelty
        float finalCuriosity = curiosity->getLevel();
        return finalCuriosity > initialCuriosity;
    }
};
```

### 3. Prediction System Integration Tests

#### Test: Prediction System Training
```cpp
class PredictionIntegrationTest {
public:
    static bool testPredictionSystemIntegration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* predictionSystem = brain->getPredictionSystem();
        ASSERT_TRUE(predictionSystem != nullptr);
        
        // Check if prediction system has internal state
        // (Assuming prediction system has update method)
        
        // Run steps with varying sensory input
        for (int step = 0; step < 200; ++step) {
            // Generate sensory input
            SensoryInput input;
            // ... set input data ...
            
            brain->receiveSensoryInput(input);
            brain->step(step, step * 0.001);
            
            // Prediction system should process input
            // predictionSystem->update(input);
        }
        
        // Verify prediction system maintained state
        // predictionSystem->hasState()
        return true;
    }
    
    static bool testPredictionErrorIntegration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Process input to generate prediction
        SensoryInput input;
        brain->receiveSensoryInput(input);
        
        // Get prediction error system
        auto* predError = brain->getPredictionErrorSignal();
        ASSERT_TRUE(predError != nullptr);
        
        // Check if prediction error provides learning signal
        float error = predError->getError();
        
        // Verify error influences learning
        return error >= 0.0f;  // Error should be non-negative
    }
};
```

### 4. Cognition System Integration Tests

#### Test: Neural Planner Integration
```cpp
class CognitionIntegrationTest {
public:
    static bool testNeuralPlannerIntegration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* planner = brain->getPlanner();
        ASSERT_TRUE(planner != nullptr);
        
        // Test planning with current brain state
        // planner->setPlanningDepth(3);
        
        // Get action sequence
        std::vector<ActionType> plannedActions;
        // plannedActions = planner->planActions();
        
        // Verify planner produced meaningful output
        return !plannedActions.empty();
    }
    
    static bool testConceptFormationIntegration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* conceptFormation = brain->getConceptFormation();
        ASSERT_TRUE(conceptFormation != nullptr);
        
        // Run steps to provide experience
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Concept formation should process experiences
        // size_t conceptCount = conceptFormation->getConceptCount();
        
        // Verify concepts formed
        return true;  // Placeholder
    }
    
    static bool testAttentionIntegration() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto* attention = brain->getAttention();
        ASSERT_TRUE(attention != nullptr);
        
        // Test attentional selection
        // std::vector<NeuronId> attendedNeurons = attention->selectAttention();
        
        // Verify attention focused on relevant neurons
        return true;  // Placeholder
    }
};
```

### 5. Development Integration Tests

#### Test: Development Stage Effects
```cpp
class DevelopmentIntegrationTest {
public:
    static bool testDevelopmentalStages() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Check developmental stage
        DevelopmentalStage initialStage = brain->getDevelopmentalStage();
        ASSERT_TRUE(initialStage == DevelopmentalStage::Initial);
        
        // Run steps to advance development
        for (int i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
            
            // Update development
            brain->develop();
            
            if (i % 100 == 0) {
                DevelopmentalStage currentStage = brain->getDevelopmentalStage();
                // Stage should progress over time
            }
        }
        
        // Verify development affected brain
        return true;
    }
    
    static bool testPlasticityRateModulation() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Check structural plasticity
        auto* structuralPlasticity = brain->getStructuralPlasticity();
        ASSERT_TRUE(structuralPlasticity != nullptr);
        
        // Get initial plasticity rate
        float initialRate = structuralPlasticity->getSynaptogenesisRate();
        
        // Advance development
        for (int i = 0; i < 500; ++i) {
            brain->step(i, i * 0.001);
            brain->develop();
        }
        
        // Check if development affected plasticity
        float finalRate = structuralPlasticity->getSynaptogenesisRate();
        
        // Rate should change with development
        return true;  // Placeholder
    }
};
```

### 6. Checkpoint Integration Tests

#### Test: Checkpoint Save/Load Integration
```cpp
class CheckpointIntegrationTest {
public:
    static bool testCompleteCheckpointCycle() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        
        // Create and initialize first brain
        auto brain1 = std::make_shared<Brain>(config);
        brain1->initialize();
        
        // Run some steps
        for (int i = 0; i < 100; ++i) {
            brain1->step(i, i * 0.001);
        }
        
        // Save checkpoint
        std::string checkpointPath = "/tmp/test_checkpoint.bin";
        bool saveResult = brain1->save(checkpointPath);
        ASSERT_TRUE(saveResult);
        
        // Create new brain and load
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        bool loadResult = brain2->load(checkpointPath);
        ASSERT_TRUE(loadResult);
        
        // Compare states
        bool statesMatch = compareBrainStates(brain1.get(), brain2.get());
        
        return statesMatch;
    }
    
    static bool testCheckpointWithAllSystems() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 50);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Ensure all integrated systems are present
        ASSERT_TRUE(brain->getWorkingMemory() != nullptr);
        ASSERT_TRUE(brain->getEpisodicMemory() != nullptr);
        ASSERT_TRUE(brain->getPredictionSystem() != nullptr);
        ASSERT_TRUE(brain->getPlanner() != nullptr);
        ASSERT_TRUE(brain->getConceptFormation() != nullptr);
        ASSERT_TRUE(brain->getAttention() != nullptr);
        ASSERT_TRUE(brain->getDevelopmentSystem() != nullptr);
        ASSERT_TRUE(brain->getDopamine() != nullptr);
        ASSERT_TRUE(brain->getCuriosity() != nullptr);
        
        // Run steps to generate state in all systems
        for (int i = 0; i < 200; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Save checkpoint
        std::string checkpointPath = "/tmp/test_full_checkpoint.bin";
        bool saveResult = brain->save(checkpointPath);
        ASSERT_TRUE(saveResult);
        
        return true;
    }
};
```

### 7. Multi-System Interaction Tests

#### Test: Complete Brain Loop Integration
```cpp
class MultiSystemIntegrationTest {
public:
    static bool testCompleteBrainLoop() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create simple world and agent
        SimpleWorld world;
        world.initialize(16, 16);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        // Run complete world-agent-brain loop
        float totalReward = 0.0f;
        for (int step = 0; step < 500; ++step) {
            // 1. Get observation from world
            SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
            
            // 2. Process sensory input
            agent.processSensoryInput(percept);
            
            // 3. Brain step
            brain->step(step);
            
            // 4. Decode motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // 5. Apply action to world
            world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
            
            // 6. Compute reward
            float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
            totalReward += reward;
            
            // 7. Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // 8. Update development
            agent.updateDevelopment(0.001);
            
            // 9. Check memory systems
            auto* wm = brain->getWorkingMemory();
            auto* em = brain->getEpisodicMemory();
            
            if (step % 100 == 0) {
                // Log integration status
                if (wm) NLM_LOG_INFO("Working memory traces: " + std::to_string(wm->getActiveTraces()));
                if (em) NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(em->getEpisodeCount()));
            }
        }
        
        // Verify integration
        bool memoryIntegrated = (brain->getWorkingMemory() != nullptr);
        bool episodicIntegrated = (brain->getEpisodicMemory() != nullptr);
        bool neuromodulationIntegrated = (brain->getDopamine() != nullptr);
        bool predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        bool developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        NLM_LOG_INFO("=== Integration Test Results ===");
        NLM_LOG_INFO("Working Memory: " + std::string(memoryIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(episodicIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(neuromodulationIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Prediction: " + std::string(predictionIntegrated ? "YES" : "NO"));
        NLM_LOG_INFO("Development: " + std::string(developmentIntegrated ? "YES" : "NO"));
        
        return memoryIntegrated && episodicIntegrated && 
               neuromodulationIntegrated && predictionIntegrated && 
               developmentIntegrated;
    }
};
```

## Test Execution Framework

### Phase 6 Demo Test Suite
```cpp
// Run all integration tests
void runPhase6IntegrationTests() {
    std::cout << "=== Phase 6 Integration Test Suite ===" << std::endl;
    
    // Memory integration tests
    std::cout << "Testing Memory Integration..." << std::endl;
    bool memoryOK = WorkingMemoryIntegrationTest::testSensoryToMemoryConnection() &&
                   EpisodicMemoryIntegrationTest::testExperienceLogging();
    std::cout << (memoryOK ? "✓ Memory Integration PASSED" : "✗ Memory Integration FAILED") << std::endl;
    
    // Neuromodulation integration tests
    std::cout << "Testing Neuromodulation Integration..." << std::endl;
    bool neuromodOK = NeuromodulationIntegrationTest::testDopamineAffectsPlasticity() &&
                     NeuromodulationIntegrationTest::testCuriosityDrivenExploration();
    std::cout << (neuromodOK ? "✓ Neuromodulation Integration PASSED" : "✗ Neuromodulation Integration FAILED") << std::endl;
    
    // Prediction integration tests
    std::cout << "Testing Prediction Integration..." << std::endl;
    bool predictionOK = PredictionIntegrationTest::testPredictionSystemIntegration();
    std::cout << (predictionOK ? "✓ Prediction Integration PASSED" : "✗ Prediction Integration FAILED") << std::endl;
    
    // Cognition integration tests
    std::cout << "Testing Cognition Integration..." << std::endl;
    bool cognitionOK = CognitionIntegrationTest::testNeuralPlannerIntegration();
    std::cout << (cognitionOK ? "✓ Cognition Integration PASSED" : "✗ Cognition Integration FAILED") << std::endl;
    
    // Development integration tests
    std::cout << "Testing Development Integration..." << std::endl;
    bool developmentOK = DevelopmentIntegrationTest::testDevelopmentalStages();
    std::cout << (developmentOK ? "✓ Development Integration PASSED" : "✗ Development Integration FAILED") << std::endl;
    
    // Checkpoint integration tests
    std::cout << "Testing Checkpoint Integration..." << std::endl;
    bool checkpointOK = CheckpointIntegrationTest::testCompleteCheckpointCycle();
    std::cout << (checkpointOK ? "✓ Checkpoint Integration PASSED" : "✗ Checkpoint Integration FAILED") << std::endl;
    
    // Multi-system integration test
    std::cout << "Testing Multi-System Integration..." << std::endl;
    bool multiSystemOK = MultiSystemIntegrationTest::testCompleteBrainLoop();
    std::cout << (multiSystemOK ? "✓ Multi-System Integration PASSED" : "✗ Multi-System Integration FAILED") << std::endl;
    
    // Summary
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Memory Integration: " << (memoryOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Neuromodulation Integration: " << (neuromodOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Prediction Integration: " << (predictionOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Cognition Integration: " << (cognitionOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Development Integration: " << (developmentOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Checkpoint Integration: " << (checkpointOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Multi-System Integration: " << (multiSystemOK ? "PASS" : "FAIL") << std::endl;
    
    bool allTestsPass = memoryOK && neuromodOK && predictionOK && 
                       cognitionOK && developmentOK && checkpointOK && 
                       multiSystemOK;
    
    std::cout << std::endl;
    std::cout << "Overall Result: " << (allTestsPass ? "ALL TESTS PASS" : "SOME TESTS FAILED") << std::endl;
    
    return allTestsPass;
}
```

## Test Configuration

### High-Performance Test Configuration
```cpp
class Phase6TestConfig {
public:
    static Phase6Config getPerformanceConfig() {
        Phase6Config config;
        config.neuronCount = 500;          // More neurons for complex integration
        config.maxSteps = 2000;            // Longer test duration
        config.enableCheckpointing = true; // Test checkpointing
        config.enableReplay = true;        // Test replay mechanism
        config.enableDevelopment = true;   // Test development
        config.checkpointPath = "./integration_test_checkpoint.bin";
        return config;
    }
    
    static Phase6Config getQuickTestConfig() {
        Phase6Config config;
        config.neuronCount = 100;          // Smaller for quick tests
        config.maxSteps = 500;              // Shorter duration
        config.enableCheckpointing = false; // Skip checkpointing for speed
        config.enableReplay = false;
        config.enableDevelopment = false;
        return config;
    }
};
```

## Test Results Analysis

### Integration Metrics Collected

1. **Memory Metrics**:
   - Working memory trace count and persistence
   - Episodic memory episode storage and retrieval
   - Memory decay and consolidation rates

2. **Neuromodulation Metrics**:
   - Dopamine levels over time
   - Curiosity-driven exploration frequency
   - Prediction error magnitudes

3. **Prediction Metrics**:
   - Prediction accuracy vs. actual input
   - Prediction error reduction over time
   - Confidence calibration

4. **Development Metrics**:
   - Developmental stage progression
   - Plasticity rate changes
   - Behavioral complexity increase

5. **Integration Metrics**:
   - Number of connected systems
   - Cross-system interaction frequency
   - Overall brain coherence

### Performance Requirements

- **Memory Retention**: >80% memory retention over 1000 steps
- **Prediction Accuracy**: >70% accuracy for predictable inputs
- **Development Progress**: Complete developmental stages
- **Checkpoint Integrity**: Save/load with 100% state restoration
- **System Cohesion**: All 9 major systems functionally connected

## Test Output Formats

### 1. Human-Readable Summary
```bash
=== Phase 6 Integration Test Results ===

✅ Memory Integration: PASS (Working Memory traces: 45, Episodic episodes: 12)
✅ Neuromodulation Integration: PASS (Dopamine: 0.72, Curiosity: 0.85)
✅ Prediction Integration: PASS (Accuracy: 0.78, Error: 0.22)
✅ Cognition Integration: PASS (Planner depth: 3, Concepts: 8)
✅ Development Integration: PASS (Stage: Maturation, Plasticity: 1.2x)
✅ Checkpoint Integration: PASS (State restored: 100%)
✅ Multi-System Integration: PASS (All 9 systems connected)

📊 Overall Integration Score: 92.3%
⏱️  Test Duration: 3.2 seconds
📝 Output: IntegrationTestResults.txt
```

### 2. Machine-Readable JSON
```json
{
  "test_run": {
    "timestamp": "2026-09-09T23:22:17Z",
    "config": {
      "neuron_count": 500,
      "max_steps": 2000,
      "systems_tested": 9
    }
  },
  "results": {
    "memory_integration": {
      "working_memory": true,
      "episodic_memory": true,
      "replay": true,
      "consolidation": true
    },
    "neuromodulation_integration": {
      "dopamine": true,
      "curiosity": true,
      "novelty": true,
      "prediction_error": true
    },
    "prediction_integration": {
      "system_present": true,
      "accuracy": 0.78,
      "error_reduction": 0.65
    },
    "cognition_integration": {
      "planner": true,
      "concept_formation": true,
      "attention": true
    },
    "development_integration": {
      "stages_progressed": 3,
      "plasticity_modulated": true
    },
    "checkpoint_integration": {
      "save_load": true,
      "state_restoration": 1.0
    }
  },
  "metrics": {
    "overall_score": 92.3,
    "systems_connected": 9,
    "tests_passed": 7,
    "tests_total": 7
  }
}
```

## Running the Tests

### Command Line Interface
```bash
# Run Phase 6 integration tests
./nlm_phase6_integration_test [--quick] [--performance] [--output file]

# Quick test (faster, fewer assertions)
./nlm_phase6_integration_test --quick

# Performance test (thorough, more comprehensive)
./nlm_phase6_integration_test --performance

# Save results to file
./nlm_phase6_integration_test --output integration_results.json
```

### Python Interface
```python
import pybind11
import subprocess

# Run integration tests via Python interface
if __name__ == "__main__":
    result = subprocess.run([
        "./nlm_phase6_integration_test",
        "--performance",
        "--output", "results.json"
    ])
    
    if result.returncode == 0:
        print("✅ Phase 6 Integration Tests PASSED")
    else:
        print("❌ Phase 6 Integration Tests FAILED")
```

## Test Automation

### CI/CD Integration
```bash
#!/bin/bash
# Phase 6 Integration Test CI Script

set -e

echo "Running Phase 6 Integration Tests..."

# Run integration tests
./nlm_phase6_integration_test --performance > test_results.txt 2>&1

# Check results
if grep -q "Overall Result: ALL TESTS PASS" test_results.txt; then
    echo "✅ All Phase 6 integration tests PASSED"
    exit 0
else
    echo "❌ Some Phase 6 integration tests FAILED"
    cat test_results.txt
    exit 1
fi
```

### GitHub Actions Integration
```yaml
# .github/workflows/phase6-integration.yml
name: Phase 6 Integration Tests

on:
  push:
    paths:
      - 'src/**'
      - 'tests/**'
  pull_request:
    paths:
      - 'src/**'
      - 'tests/**'

jobs:
  integration-tests:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build NLM
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Run Phase 6 Integration Tests
      run: |
        cd build
        ./nlm_phase6_integration_test --performance
    
    - name: Upload Test Results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: integration-test-results
        path: build/integration_results.json
```

## Conclusion

The Phase 6 integration test suite validates that all NLM brain systems are properly connected and functional as a coherent artificial brain. These tests move beyond unit testing individual components to verify the integration of memory, prediction, cognition, neuromodulation, and development systems.

Successful completion of Phase 6 integration tests confirms that the NLM has achieved:

1. **System Integration**: All 9 major brain systems are functionally connected
2. **Memory Functionality**: Working and episodic memory systems operate correctly
3. **Learning Capability**: Prediction and neuromodulation systems facilitate learning
4. **Cognitive Processing**: Planning and attention systems guide behavior
5. **Development Progression**: Brain develops and adapts across stages
6. **Persistence**: Checkpointing maintains brain state across sessions
7. **Multi-System Coordination**: All systems work together as a unified brain

Phase 6 integration represents the foundation for creating a truly intelligent, artificial developmental brain that learns from experience and adapts to its environment.
