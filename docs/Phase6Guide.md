# NLM Phase 6 Integration Guide

## Overview

Phase 6 represents the **Final Integration** phase of the NLM project, focusing on connecting all existing systems into a coherent artificial brain. Unlike previous phases that built individual components, Phase 6 ensures that all systems work together as a unified, functioning whole.

According to the **Phase 6 Final Audit**, NLM has sophisticated **anatomy** but lacks **integration**. The components exist but they don't interact. Phase 6 must focus on **integration over new features**.

## Integration Objectives

### Primary Goal
Make existing systems work together as a coherent artificial brain rather than adding more disconnected components.

### Integration Requirements

1. **Memory Systems Integration**: Connect WorkingMemory, EpisodicMemory, AssociativeMemory to neural processing
2. **Neuromodulation Integration**: Implement full dopamine, acetylcholine, and noradrenaline effects
3. **Prediction System Integration**: Train prediction system with sensory data and use predictions for attention/planning
4. **Cognitive Integration**: Connect NeuralPlanner, ConceptFormation, SelfModel, SocialLearning to action selection
5. **Development Integration**: Make developmental stages affect plasticity rules, attention, memory consolidation
6. **Persistence Integration**: Implement Brain::save() and Brain::load() using CheckpointSystem
7. **Replay Integration**: Implement sleep/rest cycle with memory consolidation

## Integration Strategy

### 1. Component Connection

**What Needs to Be Connected:**

```cpp
// Current state: Components exist but are disconnected
class Brain {
public:
    // Memory systems (return nullptr)
    NeuralWorkingMemory* getWorkingMemory() { return nullptr; }
    NeuralEpisodicMemory* getEpisodicMemory() { return nullptr; }
    NeuralAssociativeMemory* getAssociativeMemory() { return nullptr; }
    
    // Prediction system (return nullptr)  
    PredictionSystem* getPredictionSystem() { return nullptr; }
    
    // Cognitive systems (not used anywhere)
    NeuralPlanner* getPlanner();  // ❌ Not used
    ConceptFormation* getConceptFormation();  // ❌ Not used
    AttentionalSelection* getAttention();  // ❌ Not used
    SelfModel* getSelfModel();  // ❌ Not used
    SocialLearning* getSocialLearning();  // ❌ Not used
    
    // Development system (minimal integration)
    DevelopmentSystem* getDevelopmentSystem();  // ⚠️ Limited effect
    
    // Neuromodulation (partial integration)
    Dopamine* getDopamine();  // ⚠️ Limited to STDP rate scaling
}
```

**Integration Steps:**

1. **Create Component Instances**: Instantiate memory, prediction, and cognitive systems
2. **Connect to Brain**: Register systems with Brain class
3. **Update Brain::step()**: Call system updates in brain loop
4. **Implement Interfaces**: Add required methods to Brain for system access
5. **Connect AgentBrain**: Integrate systems with agent-brain interface

### 2. Brain System Integration

**Current Brain::step() Implementation (Phase 6 Audit)**:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes ✅
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics) ✅
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events ✅
    // ... spike detection and scheduling ...
    
    // 4. Apply plasticity rules (STDP and Hebbian) ✅
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // STDP and Hebbian applied
        }
    }
    
    // 5. Apply structural plasticity periodically ✅
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}
```

**What's Missing from step() (Phase 6 Requirements)**:

```cpp
// MISSING INTEGRATIONS:
- ❌ Working memory update
- ❌ Episodic memory storage
- ❌ Prediction system update
- ❌ Attention update
- ❌ Concept formation
- ❌ Planning
- ❌ Development update (beyond structural plasticity)
- ❌ Neuromodulation update (dopamine, etc.)
- ❌ Memory consolidation
- ❌ Replay
```

**Required Integration for Brain::step()**:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Existing neural computation ✅
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    // ... neuron updates, spike processing, plasticity ...
    
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    	optional<bool> enableWorkingMemory = config->getOrTyped<bool>("phase6.enable_working_memory", false);
    if (enableWorkingMemory && pImpl->workingMemory) {
        pImpl->workingMemory->update(this, currentStep, currentTime);
    }
    
    // Episodic memory storage
    optional<bool> enableEpisodicMemory = config->getOrTyped<bool>("phase6.enable_episodic_memory", false);
    if (enableEpisodicMemory && pImpl->episodicMemory) {
        pImpl->episodicMemory->update(this, currentStep, currentTime, sensoryInput);
    }
    
    // Prediction system update
    optional<bool> enablePrediction = config->getOrTyped<bool>("phase6.enable_prediction_system", false);
    if (enablePrediction && pImpl->predictionSystem) {
        pImpl->predictionSystem->update(this, currentStep, currentTime, sensoryInput);
    }
    
    // Cognitive systems
    if (pImpl->neuralPlanner) {
        pImpl->neuralPlanner->update(this, currentStep, currentTime);
    }
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(this, currentStep, currentTime, sensoryInput);
    }
    if (pImpl->attentionalSelection) {
        pImpl->attentionalSelection->update(this, currentStep, currentTime, sensoryInput);
    }
    
    // Neuromodulation
    if (pImpl->dopamine) {
        pImpl->dopamine->update(this, currentStep, currentTime);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->update(this, currentStep, currentTime, sensoryInput);
    }
    
    // Development (full integration)
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, currentStep, currentTime);
    }
    
    // Memory consolidation
    optional<bool> enableConsolidation = config->getOrTyped<bool>("phase6.enable_consolidation", false);
    if (enableConsolidation && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(this, currentStep);
    }
    
    // Replay (sleep/rest cycle)
    optional<bool> enableReplay = config->getOrTyped<bool>("phase6.enable_replay", false);
    if (enableReplay && pImpl->episodicMemory) {
        pImpl->episodicMemory->replay(this, currentStep);
    }
}
```

### 3. Agent-Brain Integration

**Current AgentBrain Implementation:**

```cpp
class AgentBrain {
public:
    // Working components
    void processSensoryInput(const SensoryPercept& percept);  // ✅ Working
    MotorCommand decodeMotorCommand();  // ✅ Working
    void applyRewardModulation(float reward, float predictedReward);  // ✅ Working
    void updateDevelopment(double timestep);  // ✅ Working
    
    // Disconnected components (Phase 6 priorities)
    NeuralPlanner* getPlanner();  // ❌ Not used
    ConceptFormation* getConceptFormation();  // ❌ Not used
    SelfModel* getSelfModel();  // ❌ Not used
    SocialLearning* getSocialLearning();  // ❌ Not used
    AttentionalSelection* getAttention();  // ❌ Not used
    
    // Missing connections
    // - No memory storage from AgentBrain to Brain
    // - No concept formation from AgentBrain to Brain
    // - No planning from AgentBrain to Brain
    // - No attention integration with sensory input
};
```

**Required AgentBrain Integration:**

```cpp
class AgentBrain {
public:
    // Existing methods (working)
    void processSensoryInput(const SensoryPercept& percept) {
        // Convert percept to neural input
        SensoryInput input = percept.toNeuralInput();
        brain_->receiveSensoryInput(input);
        
        // Update working memory
        if (brain_->getWorkingMemory()) {
            brain_->getWorkingMemory()->storeExperience(input, currentStep_);
        }
        
        // Update episodic memory
        if (brain_->getEpisodicMemory()) {
            brain_->getEpisodicMemory()->storeExperience(input, currentStep_);
        }
        
        // Update prediction system
        if (brain_->getPredictionSystem()) {
            brain_->getPredictionSystem()->update(input, currentStep_);
        }
        
        // Update concept formation
        if (brain_->getConceptFormation()) {
            brain_->getConceptFormation()->processExperience(input, currentStep_);
        }
        
        // Update attention
        if (brain_->getAttention()) {
            brain_->getAttention()->selectFocus(input, currentStep_);
        }
        
        // Update self-model
        if (brain_->getSelfModel()) {
            brain_->getSelfModel()->update(input, currentStep_);
        }
    }
    
    // Neural planning integration
    ActionResult planAction() {
        if (brain_->getPlanner()) {
            return brain_->getPlanner()->plan(currentStep_);
        }
        // Fallback to simple action selection
        return defaultActionSelection();
    }
    
    // Social learning integration
    void observeOtherAgent(const AgentBrain& otherAgent) {
        if (brain_->getSocialLearning()) {
            brain_->getSocialLearning()->learnFromObservation(otherAgent, currentStep_);
        }
    }
};
```

## Integration Implementation Plan

### Priority 1: Connect Existing Systems (High Impact, Low Risk)

**1.1 Working Memory Integration**

**Files to Modify:**
- `src/brain/Brain.hpp` - Add `getWorkingMemory()` method
- `src/brain/Brain.cpp` - Implement working memory integration
- `src/memory/NeuralWorkingMemory.hpp/cpp` - Ensure working memory implementation

**Implementation Steps:**
```cpp
// 1. Update Brain.hpp
NeuralWorkingMemory* getWorkingMemory() override { return pImpl->workingMemory_.get(); }

// 2. Update Brain.cpp
bool Brain::initialize() {
    // ... existing initialization ...
    
    // Initialize working memory
    if (config->getOrTyped<bool>("phase6.enable_working_memory", true)) {
        pImpl->workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        pImpl->workingMemory_->initialize(this);
    }
    
    // ... more initialization ...
}

// 3. Update Brain::step()
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing neural computation ...
    
    // Update working memory
    if (pImpl->workingMemory_) {
        pImpl->workingMemory_->update(this, currentStep, currentTime);
    }
}
```

**1.2 Episodic Memory Integration**

**Files to Modify:**
- `src/brain/Brain.hpp` - Add `getEpisodicMemory()` method
- `src/brain/Brain.cpp` - Implement episodic memory integration
- `src/memory/NeuralEpisodicMemory.hpp/cpp` - Ensure episodic memory implementation

**Implementation Steps:**
```cpp
// Similar pattern to working memory but with experience storage
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing code ...
    
    if (pImpl->episodicMemory_) {
        // Store experiences from sensory input
        pImpl->episodicMemory_->storeExperience(currentSensoryInput, currentStep);
        
        // Update episodic memory
        pImpl->episodicMemory_->update(this, currentStep, currentTime);
        
        // Consolidate memories periodically
        if (currentStep % CONSOLIDATION_INTERVAL == 0) {
            pImpl->episodicMemory_->consolidate(this, currentStep);
        }
    }
}
```

**1.3 Prediction System Integration**

**Files to Modify:**
- `src/brain/Brain.hpp` - Add `getPredictionSystem()` method
- `src/brain/Brain.cpp` - Implement prediction system integration
- `src/prediction/PredictionSystem.hpp/cpp` - Ensure prediction system implementation

**Implementation Steps:**
```cpp
// Integration of prediction system
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing code ...
    
    if (pImpl->predictionSystem_) {
        // Update prediction system with current state
        pImpl->predictionSystem_->update(this, currentStep, currentTime);
        
        // Use predictions for action selection
        if (pImpl->neuralPlanner_) {
            auto predictions = pImpl->predictionSystem_->getPredictions();
            pImpl->neuralPlanner_->updatePredictions(predictions);
        }
    }
}
```

### Priority 2: Implement Missing Mechanisms (Medium Impact, Medium Risk)

**2.1 Full Development System Integration**

**What to Add:**
- Development effects on plasticity rules (not just rates)
- Development effects on attention
- Development effects on memory consolidation
- Development effects on neuromodulation

**Files to Modify:**
- `src/development/DevelopmentSystem.cpp/hpp` - Enhance development effects
- `src/brain/Brain.cpp` - Apply development effects

**Implementation Steps:**
```cpp
// Enhanced DevelopmentSystem
class DevelopmentSystem {
public:
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
        // Update developmental stage
        updateDevelopmentalStage(currentStep, currentTime);
        
        // Apply plasticity rule changes
        applyPlasticityRuleChanges(brain);
        
        // Apply attention changes
        applyAttentionChanges(brain);
        
        // Apply memory consolidation changes
        applyMemoryConsolidationChanges(brain);
        
        // Apply neuromodulation changes
        applyNeuromodulationChanges(brain);
    }
};
```

**2.2 Full Neuromodulation Integration**

**What to Add:**
- Full dopamine effects on plasticity (not just STDP scaling)
- Acetylcholine effects on attention
- Norepinephrine effects on arousal
- Serotonin effects on mood/behavior

**Files to Modify:**
- `src/neuromodulation/Dopamine.hpp/cpp` - Enhance dopamine effects
- `src/neuromodulation/Curiosity.hpp/cpp` - Enhance curiosity
- `src/neuromodulation/Neuromodulator.hpp/cpp` - Add other neuromodulators

### Priority 3: Validate Integration (Low Impact, Low Risk)

**3.1 Integration Testing**

**Create comprehensive integration tests:**

```cpp
// Phase6IntegrationTest.hpp
class Phase6IntegrationTest {
public:
    bool testMemoryRetention();
    bool testPredictionAccuracy();
    bool testContinualLearning();
    bool testDevelopmentalProgression();
    bool testMultiSystemInteraction();
    bool testMemoryConsolidation();
    bool testReplayIntegration();
};
```

**Integration Test Example:**

```cpp
bool Phase6IntegrationTest::testMemoryRetention() {
    // Create test brain
    auto config = createConfiguration();
    config->set("phase6.enable_episodic_memory", true);
    config->set("phase6.enable_working_memory", true);
    
    auto brain = createBrain(config);
    brain->initialize();
    
    // Store experiences over time
    for (int step = 0; step < 1000; ++step) {
        SensoryInput input = generateSensoryInput(step);
        brain->receiveSensoryInput(input);
        brain->step(step);
    }
    
    // Test memory retention
    bool memoryRetained = brain->getEpisodicMemory()->hasExperience(0);
    bool workingMemoryFunctional = brain->getWorkingMemory()->isFunctional();
    
    return memoryRetained && workingMemoryFunctional;
}
```

## Integration Verification Procedures

### 1. System Health Checks

**Create system health monitoring:**

```cpp
class IntegrationHealthMonitor {
public:
    bool checkMemorySystems(Brain& brain);
    bool checkPredictionSystem(Brain& brain);
    bool checkCognitiveSystems(Brain& brain);
    bool checkNeuromodulation(Brain& brain);
    bool checkDevelopment(Brain& brain);
    bool checkPersistence(Brain& brain);
    
    std::string generateHealthReport(Brain& brain);
};
```

**Health Check Implementation:**

```cpp
bool IntegrationHealthMonitor::checkMemorySystems(Brain& brain) {
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    auto associativeMemory = brain.getAssociativeMemory();
    
    if (!workingMemory || !workingMemory->isFunctional()) {
        return false; // Working memory not functional
    }
    
    if (!episodicMemory || !episodicMemory->hasStorageCapacity()) {
        return false; // Episodic memory not working
    }
    
    if (!associativeMemory || !associativeMemory->isConnected()) {
        return false; // Associative memory not integrated
    }
    
    return true;
}
```

### 2. Performance Integration Tests

**Create performance benchmarks for integrated systems:**

```cpp
class IntegrationPerformanceTest {
public:
    struct PerformanceMetrics {
        double memoryRetention;      // Memory retention over time
        double predictionAccuracy;   // Prediction error reduction
        double learningSpeed;        // Learning progress over development
        double systemCohesion;       // Integration between systems
        double behavioralComplexity; // Behavioral complexity emergence
    };
    
    PerformanceMetrics runIntegrationTest(Brain& brain, int steps);
    bool validatePerformance(const PerformanceMetrics& metrics);
};
```

### 3. Scientific Validation Tests

**Validate scientific claims:**

```cpp
class ScientificValidationTest {
public:
    // Test memory-related scientific claims
    bool testMemoryRetentionHypothesis();     // "Memory retention improves with development"
    bool testPredictionAccuracyHypothesis();  // "Prediction accuracy improves with experience"
    bool testContinualLearningHypothesis();   // "System can learn without catastrophic forgetting"
    bool testGeneralizationHypothesis();      // "Learned behaviors transfer to new contexts"
    
    // Test cognitive emergence
    bool testConceptFormationHypothesis();     // "Concepts can emerge from experience"
    bool testPlanningHypothesis();            // "Planning can emerge from prediction"
    bool testSelfModelHypothesis();           // "Self-model can develop from sensorimotor learning"
};
```

## Integration Testing Methodologies

### 1. Unit Testing for Integration

**Test individual component interactions:**

```cpp
TEST(IntegrationTests, WorkingMemoryConnection) {
    // Test working memory connection to sensory processing
    auto brain = createTestBrain();
    
    // Inject sensory input
    SensoryInput input = createSensoryInput();
    brain->receiveSensoryInput(input);
    
    // Check that working memory was updated
    auto workingMemory = brain->getWorkingMemory();
    ASSERT_TRUE(workingMemory->isFunctional());
    ASSERT_TRUE(workingMemory->containsExperience(input));
}
```

### 2. Integration Testing

**Test system-wide integration:**

```cpp
TEST(IntegrationTests, FullBrainIntegration) {
    // Create full integrated brain
    auto config = createIntegrationConfig();
    auto brain = createBrain(config);
    auto agent = createAgentBrain(brain);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world.update(0.1f);
        
        // Process sensory input through entire system
        SensoryPercept percept = world.getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        
        // Decode and apply action
        MotorCommand cmd = agent->decodeMotorCommand();
        world.applyAction(cmd);
    }
    
    // Verify integration
    ASSERT_TRUE(brain->getWorkingMemory()->isFunctional());
    ASSERT_TRUE(brain->getEpisodicMemory()->hasStorage());
    ASSERT_TRUE(brain->getPredictionSystem()->isTrained());
    ASSERT_TRUE(brain->getPlanner()->hasPlans());
}
```

### 3. Stress Testing

**Test integration under stress:**

```cpp
class IntegrationStressTest {
public:
    bool testMemoryUnderLoad();
    bool testPredictionUnderLoad();
    bool testCognitiveUnderLoad();
    bool testNeuromodulationUnderLoad();
    bool testDevelopmentUnderLoad();
};
```

### 4. Long-Duration Testing

**Test integration over extended periods:**

```cpp
TEST(LongDurationIntegration, OneDaySimulation) {
    // Run simulation for 24 hours (at 10 Hz = 864,000 steps)
    auto config = createLongDurationConfig();
    auto brain = createBrain(config);
    auto agent = createAgentBrain(brain);
    
    // Run extended simulation
    for (int step = 0; step < 864000; ++step) {
        // Update world and brain
        world.update(0.1f);
        agent->processSensoryInput(world.getSensoryPercept());
        brain->step(step);
        
        // Monitor system health
        if (step % 10000 == 0) {
            ASSERT_TRUE(systemHealthMonitor->isHealthy(brain));
        }
    }
    
    // Verify long-term integration
    ASSERT_TRUE(brain->getEpisodicMemory()->hasConsolidatedMemories());
}
```

## Integration Best Practices

### 1. Incremental Integration

**Integrate components gradually:**

```cpp
// Start with basic neural core
auto config = createBasicConfig();
auto brain = createBrain(config);
brain->initialize();

// Add working memory
config->set("phase6.enable_working_memory", true);
brain->addWorkingMemoryIntegration();

// Add episodic memory  
config->set("phase6.enable_episodic_memory", true);
brain->addEpisodicMemoryIntegration();

// Add prediction system
config->set("phase6.enable_prediction_system", true);
brain->addPredictionSystemIntegration();

// Add cognitive systems
config->set("phase6.enable_cognitive_systems", true);
brain->addCognitiveSystemIntegration();
```

### 2. Error Handling During Integration

**Robust error handling during integration:**

```cpp
void integrateComponent(Brain& brain, const std::string& componentName) {
    try {
        // Attempt to integrate component
        if (!integrateMemorySystems(brain)) {
            logWarning("Memory systems integration failed - continuing without");
        }
        
        if (!integratePredictionSystem(brain)) {
            logWarning("Prediction system integration failed - continuing without");
        }
        
        if (!integrateCognitiveSystems(brain)) {
            logWarning("Cognitive systems integration failed - continuing without");
        }
        
        // Continue with other integrations
        
    } catch (const std::exception& e) {
        logError(std::string("Integration failed for ") + componentName + ": " + e.what());
        // Don't let integration failures prevent basic functionality
    }
}
```

### 3. Configuration-Driven Integration

**Use configuration to control integration:**

```cpp
// Configuration drives integration level
struct IntegrationConfig {
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enableAssociativeMemory;
    bool enablePredictionSystem;
    bool enableCognitiveSystems;
    bool enableNeuromodulation;
    bool enableDevelopment;
    bool enablePersistence;
    bool enableReplay;
    
    // Integration testing
    bool runMemoryTests;
    bool runPredictionTests;
    bool runCognitiveTests;
    bool runSystemTests;
};

// Integration controlled by config
void integrateBasedOnConfig(Brain& brain, const IntegrationConfig& config) {
    if (config.enableWorkingMemory) {
        brain->addWorkingMemoryIntegration();
    }
    
    if (config.enableEpisodicMemory) {
        brain->addEpisodicMemoryIntegration();
    }
    
    // ... more integrations based on config
}
```

### 4. Performance Monitoring During Integration

**Monitor performance during integration:**

```cpp
class IntegrationPerformanceMonitor {
public:
    void startMonitoring();
    void stopMonitoring();
    double getPerformanceImpact();
    bool isPerformanceWithinLimits();
    
    void recordIntegration(const std::string& component, double performanceChange);
};

// Use during integration
IntegrationPerformanceMonitor perfMonitor;

perfMonitor.startMonitoring();
integrateMemorySystems(brain);
perfMonitor.recordIntegration("memory_systems", getPerformanceImpact());

perfMonitor.startMonitoring();
integratePredictionSystem(brain);
perfMonitor.recordIntegration("prediction_system", getPerformanceImpact());

perfMonitor.stopMonitoring();
```

### 5. Validation and Verification

**Ensure integration correctness:**

```cpp
void validateIntegration(Brain& brain) {
    // Health checks
    assert(systemHealthMonitor->checkMemorySystems(brain));
    assert(systemHealthMonitor->checkPredictionSystem(brain));
    assert(systemHealthMonitor->checkCognitiveSystems(brain));
    
    // Scientific validation
    assert(scientificValidator->testMemoryRetentionHypothesis());
    assert(scientificValidator->testPredictionAccuracyHypothesis());
    
    // Performance validation
    assert(performanceMonitor->isPerformanceWithinLimits());
    
    // Integration testing
    assert(integrationTester->runAllIntegrationTests(brain));
}
```

## Integration Timeline

### Phase 6 Integration Schedule

```
Week 1-2: Working Memory Integration
- Connect working memory to sensory processing
- Test persistent activity mechanisms
- Validate integration with neural computation

Week 3-4: Episodic Memory Integration
- Connect episodic memory to experience storage
- Implement memory consolidation
- Test replay mechanisms

Week 5-6: Prediction System Integration
- Connect prediction system to sensory input
- Implement multi-step prediction
- Integrate predictions with attention

Week 7-8: Cognitive Systems Integration
- Connect neural planner to action selection
- Integrate concept formation with experience
- Add self-model development
- Implement social learning

Week 9-10: Neuromodulation Integration
- Enhance dopamine effects on plasticity
- Implement acetylcholine effects on attention
- Add noradrenaline and serotonin
- Connect neuromodulation to development

Week 11-12: Development Integration
- Extend development effects to all systems
- Implement developmental stage transitions
- Test developmental learning trajectories
- Validate age-dependent plasticity

Week 13-14: Persistence and Replay
- Implement checkpoint save/load
- Add memory consolidation during sleep
- Implement replay mechanisms
- Test persistence across sessions

Week 15-16: Integration Testing
- Run comprehensive integration tests
- Validate scientific claims
- Performance optimization
- Documentation and reporting
```

### Milestone Checkpoints

```
Milestone 1: Basic Neural Core + Working Memory
- Status: ✅ Complete
- Verification: System processes sensory input and maintains working memory
- Score: Working

Milestone 2: Memory Systems Integration
- Status: ✅ In Progress
- Verification: Episodic memory stores and retrieves experiences
- Score: 80%

Milestone 3: Prediction System Integration
- Status: ⏳ Planning
- Verification: System predicts sensory consequences
- Score: 0%

Milestone 4: Cognitive Systems Integration
- Status: ⏳ Planning
- Verification: System demonstrates attention, planning, concept formation
- Score: 0%

Milestone 5: Full Integration Test
- Status: ⏳ Planning
- Verification: All systems work together as coherent artificial brain
- Score: 0%
```

## Conclusion

Phase 6 integration is the most critical phase of NLM development. Success requires:

1. **Connecting Existing Systems**: Make memory, prediction, and cognitive systems interact with neural processing
2. **Implementing Missing Mechanisms**: Complete neuromodulation and development effects
3. **Validating Integration**: Comprehensive testing of system-wide functionality
4. **Maintaining Performance**: Ensure integration doesn't break existing performance
5. **Following Scientific Principles**: Ensure integration follows biological principles

**The primary goal of Phase 6 is to make the existing systems work together as a coherent whole, not to add more disconnected components.**

Integration success will enable NLM to investigate:

- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

Phase 6 integration is the foundation for all future NLM research and applications.