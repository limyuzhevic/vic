# Unit Tests

Comprehensive unit tests for individual NLM system components, validating all new functionality improvements.

## Test Coverage

### 1. Working Memory Integration
Tests neural working memory integration with the brain processing system.

**Files:**
- `test_working_memory_integration.cpp` - Core working memory functionality
- `test_persistent_activity.cpp` - Persistent activity and maintenance mechanisms
- `test_memory_neural_dynamics.cpp` - Working memory neural dynamics

**Key Tests:**
```cpp
// Test working memory store/retrieve cycle
void testWorkingMemoryStoreRetrieve() {
    NeuralWorkingMemory memory;
    Brain* brain = ...;
    memory.initialize(brain);
    
    std::vector<float> pattern{0.5f, 0.8f, 0.3f};
    memory.store(pattern, 1.0f);
    
    auto retrieved = memory.retrieve();
    assertEqual(retrieved.size(), pattern.size());
    assertEqual(retrieved[0], pattern[0], 0.01f);
}

// Test neural maintenance connections
void testNeuralMaintenanceConnections() {
    NeuralWorkingMemory memory;
    memory.setCapacity(50);
    memory.setDecayRate(0.01f);
    
    // Test that maintenance connections persist information
    // over multiple time steps with minimal decay
}

// Test competition mechanism
void testWorkingMemoryCompetition() {
    NeuralWorkingMemory memory;
    // Store multiple patterns
    memory.store({0.9f, 0.8f, 0.7f}, 1.0f);
    memory.store({0.3f, 0.4f, 0.2f}, 1.0f);
    
    memory.runCompetition();
    
    // Verify that stronger patterns win
    assertTrue(memory.isWinning(primaryPatternNeuron));
    assertFalse(memory.isWinning(weakPatternNeuron));
}
```

### 2. Prediction System with Reward Prediction
Tests prediction mechanisms and reward prediction integration.

**Files:**
- `test_prediction_system.cpp` - Neural prediction core functionality
- `test_reward_prediction.cpp` - Reward prediction and prediction error
- `test_action_consequence.cpp` - Action-consequence learning and prediction

**Key Tests:**
```cpp
// Test prediction generation and error computation
void testPredictionGeneration() {
    NeuralPrediction predictor;
    Brain* brain = ...;
    predictor.initialize(brain);
    
    std::vector<float> sensoryState{0.5f, 0.7f, 0.9f};
    predictor.recordSensoryState(sensoryState, 0);
    
    auto prediction = predictor.generatePrediction(1);
    assertEqual(prediction.size(), sensoryState.size());
    
    float error = predictor.updateWithObservation(sensoryState, 1);
    assertLessThan(error, 0.1f); // Low prediction error
}

// Test reward prediction accuracy
void testRewardPrediction() {
    NeuralPrediction predictor;
    predictor.initialize(brain);
    
    float reward = 1.5f;
    float predictedReward = 1.3f;
    
    // Train predictor with reward history
    predictor.recordReward(reward, 0);
    
    float predicted = predictor.predictNextReward(1);
    assertAlmostEqual(predicted, reward, 0.2f);
}
```

### 3. Concept Formation with Attention Integration
Tests concept formation mechanisms and attention integration.

**Files:**
- `test_concept_formation.cpp` - Core concept formation functionality
- `test_attention_integration.cpp` - Integration with attention system
- `test_prototype_evolution.cpp` - Prototype evolution and stability

**Key Tests:**
```cpp
// Test concept creation and stability
void testConceptFormation() {
    ConceptFormation conceptForm(
        attentionSystem.getWinners(),
        workingMemory.retrieve()
    );
    
    std::vector<float> features{0.8f, 0.9f, 0.7f};
    size_t conceptId = conceptForm.presentExperience(
        features, features, 0.8f, 0
    );
    
    assertNotEqual(conceptId, 0u); // New concept created
    
    auto concept = conceptForm.getConcept(conceptId);
    assertEqual(concept->prototype.size(), features.size());
    assertEqual(concept->totalObservations, 1u);
}

// Test attention-based concept selection
void testConceptBasedOnAttention() {
    ConceptFormation conceptForm(...);
    
    // With attentional focus
    std::vector<size_t> attendedConceptIds = conceptForm.getConceptsInWorkingMemory();
    assertFalse(attendedConceptIds.empty());
    
    // Concept selection should favor attended concepts
    size_t selectedId = conceptForm.getConceptBasedOnAttention();
    assert(find(attendedConceptIds.begin(), attendedConceptIds.end(), selectedId) != attendedConceptIds.end());
}
```

### 4. World-Agent Sensory Input Integration
Tests sensory input processing and agent-world interaction.

**Files:**
- `test_sensory_integration.cpp` - Sensory input processing
- `test_agent_world_integration.cpp` - World-agent interaction
- `test_perception_decoding.cpp` - Perception and motor decoding

**Key Tests:**
```cpp
// Test sensory perception and neural injection
void testSensoryInputIntegration() {
    AgentBrain agentBrain(brain);
    SimpleWorld world;
    agentBrain.initialize(world);
    
    SensoryPercept percept;
    percept.addVisionInput({0.1f, 0.2f, 0.3f});
    percept.addTouchInput({0.5f, 0.6f});
    
    agentBrain.processSensoryInput(percept);
    
    // Verify that sensory inputs were properly injected into brain
    assertSensoryInjectionSuccessful(visionNeurons);
    assertSensoryInjectionSuccessful(touchNeurons);
}

// Test motor command decoding
void testMotorCommandDecoding() {
    AgentBrain agentBrain(brain);
    
    MotorCommand command = agentBrain.decodeMotorCommand();
    assertNotEqual(command, MotorCommand::Wait); // Should produce action
    
    // Test that different neural patterns produce appropriate commands
    // Forward neurons -> MoveForward
    // Turn neurons -> TurnLeft/TurnRight
    // Interact neurons -> Interact
}
```

### 5. Advanced Configuration Parsing and Validation
Tests configuration management and validation.

**Files:**
- `test_config_parsing.cpp` - Configuration parsing
- `test_config_validation.cpp` - Configuration validation
- `test_template_config.cpp` - Template-based configuration

**Key Tests:**
```cpp
// Test configuration validation
void testConfigurationValidation() {
    AdvancedConfig config;
    
    // Valid configuration
    assertTrue(config.validate("brain.neuron_count", 100, "int"));
    assertTrue(config.validate("brain.connection_probability", 0.1f, "float"));
    assertTrue(config.validate("brain.region_count", 1, "int"));
    
    // Invalid configuration
    assertFalse(config.validate("brain.neuron_count", "invalid", "int"));
    assertFalse(config.validate("brain.connection_probability", 1.5f, "float"));
    assertFalse(config.validate("unknown.param", 42, "int"));
}

// Test template configuration loading
void testTemplateConfiguration() {
    TemplateConfigManager manager;
    
    auto template = manager.loadTemplate("phase3_interaction");
    assertNotNull(template);
    
    auto config = manager.expandTemplate(template, {
        {"brain.neuron_count", 100},
        {"world.width", 20.0f}
    });
    
    assertEqual(config.getInt("brain.neuron_count"), 100);
    assertAlmostEqual(config.getFloat("world.width"), 20.0f);
}
```

### 6. Command-Line Interface
Tests command-line interface and configuration options.

**Files:**
- `test_cli_parsing.cpp` - CLI argument parsing
- `test_cli_execution.cpp` - CLI command execution
- `test_cli_validation.cpp` - CLI validation and error handling

**Key Tests:**
```cpp
// Test CLI argument parsing
void testCLIParsing() {
    CLIParser parser;
    
    auto args = parser.parse({
        "--config", "test_config.json",
        "--iterations", "100",
        "--verbose"
    });
    
    assertEqual(args.configFile, "test_config.json");
    assertEqual(args.iterations, 100);
    assertTrue(args.verbose);
}

// Test CLI command validation
void testCLIValidation() {
    CLIValidator validator;
    
    // Valid commands
    assertTrue(validator.validateCommand("run", {
        {"--config", "file.json"},
        {"--iterations", "50"}
    }));
    
    // Invalid commands
    assertFalse(validator.validateCommand("run", {}));
    assertFalse(validator.validateCommand("invalid_command", {}));
}
```

## Test Framework Integration

All unit tests integrate with the custom NLM testing framework:

```cpp
// Test base class for all unit tests
class UnitTestBase {
protected:
    bool setupCompleted() const { return setupComplete_; }
    void recordTestInfo(const std::string& testName, double executionTime) {
        testResults_.emplace_back(testName, executionTime, TestStatus::PASS);
    }
    
    template<typename T>
    void assertAlmostEqual(T expected, T actual, T tolerance) {
        if (std::abs(expected - actual) > tolerance) {
            throw AssertionError("Value mismatch", expected, actual);
        }
    }
    
    template<typename Container>
    void assertEqual(const Container& expected, const Container& actual) {
        if (expected != actual) {
            throw AssertionError("Container mismatch", expected, actual);
        }
    }
};
```

## Test Execution

### Running Unit Tests

```bash
# Build the test suite
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TESTING=ON ..
make

# Run all unit tests
./nlm_test_suite --suite=unit

# Run specific unit test category
./nlm_test_suite --suite=working_memory
./nlm_test_suite --suite=prediction
./nlm_test_suite --suite=concept_formation

# Run unit tests with performance profiling
./nlm_test_suite --suite=unit --profile-memory --precision=double
```

### Test Results

Unit tests generate detailed reports:
- **JSON**: Structured test results for CI/CD integration
- **HTML**: Visual test report with detailed metrics
- **Console**: Real-time progress display
- **Performance**: Detailed timing and memory usage analysis

The unit test suite ensures that all NLM improvements function correctly at the component level before integration testing.