# Integration Guide for NLM Phase 6

This document provides guidance on connecting the existing NLM systems to achieve a fully integrated artificial brain.

## Overview

The NLM codebase has sophisticated individual components but most are disconnected from the main brain loop. Phase 6 focuses on **integration, not adding new features**.

## Integration Map

```
PERCEPTION
    ↓
SENSORY NEURONS
    ↓
WORKING MEMORY
    ↓
ATTENTION
    ↓
PREDICTION
    ↓
CONCEPT FORMATION
    ↓
PLANNING
    ↓
ACTION SELECTION
    ↓
SELF-MODEL
    ↓
EPISODIC MEMORY
    ↓
REPLAY & CONSOLIDATION
    ↓
DEVELOPMENT
    ↓
NEUROMODULATION
    ↓
PLASTICITY
    ↓
BEHAVIOR
```

## Priority Actions for Integration

### 1. Memory System Integration

#### Working Memory Integration
**Current State:** `WorkingMemory` class exists but is never used by the brain.

**To Integrate:**
```cpp
// In Brain::step(), add:
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing code ...
    
    // NEW: Update working memory with current neural activity
    if (auto* wm = getWorkingMemory()) {
        wm->updateNeuralTraces(getRegions());
        wm->selectAttention();  // Choose what's worth keeping
    }
    
    // ... rest of code ...
}
```

**Implementation Requirements:**
- Add `updateNeuralTraces()` method to WorkingMemory to capture neural patterns
- Add `selectAttention()` method to implement competitive selection
- Connect working memory updates to sensory input

#### Episodic Memory Integration
**Current State:** `EpisodicMemory` class exists but is never updated.

**To Integrate:**
```cpp
// In AgentBrain::processSensoryInput(), add:
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing code ...
    
    // NEW: Store experience in episodic memory
    if (auto* em = brain_->getEpisodicMemory()) {
        em->storeExperience(percept, currentStep, actionTaken);
    }
    
    // ... rest of code ...
}
```

### 2. Prediction System Integration

#### Sequence Prediction
**Current State:** `PredictionSystem` exists but is never trained.

**To Integrate:**
```cpp
// In AgentBrain::processSensoryInput(), add:
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing code ...
    
    // NEW: Update prediction system
    if (auto* pred = brain_->getPredictionSystem()) {
        pred->observe(percept, currentStep);
        pred->computePredictionError();
        pred->updateWeights();
    }
    
    // ... rest of code ...
}
```

#### Action Effect Prediction
**Current State:** `ActionConsequencePredictor` exists but is never used.

**To Integrate:**
```cpp
// In AgentBrain::applyRewardModulation(), add:
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // ... existing code ...
    
    // NEW: Update prediction based on outcome
    if (auto* pred = brain_->getPredictionSystem()) {
        pred->updateActionPredictions(actionTaken, reward);
        // Track prediction accuracy for learning
    }
    
    // ... rest of code ...
}
```

### 3. Concept Formation Integration

#### Pattern Discovery
**Current State:** `ConceptFormation` exists but never processes experiences.

**To Integrate:**
```cpp
// In a background task or periodically, add:
void Brain::consolidateLearning() {
    if (auto* cf = getConceptFormation()) {
        cf->processWorkingMemory(wm);  // Extract patterns
        cf->updateCategories(currentTime);  // Form concepts
    }
}
```

### 4. Planning Integration

#### Action Sequence Selection
**Current State:** `NeuralPlanner` exists but is never called.

**To Integrate:**
```cpp
// In AgentBrain::decodeMotorCommand(), add:
MotorCommand AgentBrain::decodeMotorCommand() {
    // ... existing code ...
    
    // NEW: Use neural planner for complex decisions
    if (isPlanningEnabled_ && auto* planner = brain_->getPlanner()) {
        planner->setCurrentState(getCurrentState());
        planner->evaluateOptions(motorNeurons);
        MotorCommand plannedAction = planner->selectAction();
        
        // Blend planned action with baseline
        if (confidence_ > planThreshold_) {
            return plannedAction;
        }
    }
    
    // ... rest of code ...
}
```

### 5. Self-Model Integration

#### Body Schema Learning
**Current State:** `SelfModel` exists but never updates.

**To Integrate:**
```cpp
// In AgentBrain::updateDevelopment(), add:
void AgentBrain::updateDevelopment(double timestep) {
    // ... existing code ...
    
    // NEW: Update self-model
    if (auto* selfModel = brain_->getSelfModel()) {
        selfModel->updateBodySchema(agentBody_);
        selfModel->updateActionEffects(actionOutcomes);
    }
    
    // ... rest of code ...
}
```

### 6. Neuromodulation Integration

#### Full Dopamine Effects
**Current State:** Dopamine only scales STDP weights.

**To Integrate:**
```cpp
// In AgentBrain::applyRewardModulation(), add:
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // ... existing code ...
    
    // NEW: Implement full dopamine effects
    if (auto* dopamine = brain_->getDopamine()) {
        // Set dopamine level based on prediction error
        dopamine->setLevel(predictionError_);
        
        // Apply dopamine to working memory consolidation
        if (auto* wm = brain_->getWorkingMemory()) {
            wm->consolidateWithDopamine(dopamineLevel_);
        }
        
        // Apply dopamine to attention modulation
        dopamine->modulateAttention(currentState);
    }
    
    // ... rest of code ...
}
```

### 7. Development Integration Beyond Plasticity

#### Development Effects on Multiple Systems
**Current State:** Development only affects plasticity rates.

**To Integrate:**
```cpp
// In AgentBrain::updateDevelopment(), add:
void AgentBrain::updateDevelopment(double timestep) {
    // ... existing code (developmental stage setting) ...
    
    // NEW: Development affects multiple systems
    if (developmentEnabled_) {
        // Stage 1-2: High plasticity, attention sampling
        if (currentStage_ == DevelopmentalStage::Initial || 
            currentStage_ == DevelopmentalStage::CriticalPeriod) {
            // Increase exploration, attention breadth
            curiosityEnabled_ = true;
            // Sample more sensory inputs
        }
        
        // Stage 3-4: Concept formation
        else if (currentStage_ == DevelopmentalStage::Maturation ||
                 currentStage_ == DevelopmentalStage::Adult) {
            // Enable concept learning
            enableConceptFormation(true);
            // Stabilize working memory
        }
        
        // All stages: memory consolidation
        if (auto* em = brain_->getEpisodicMemory()) {
            em->setConsolidationRate(plasticityModifier_);
        }
    }
    
    // ... rest of code ...
}
```

## Integration Testing Strategy

### 1. Test Individual Integrations
```cpp
// Each integration should be tested separately
TEST(WorkingMemoryIntegration) {
    // Create brain with working memory
    // Process sensory input
    // Verify working memory contains relevant patterns
}

TEST(PredictionIntegration) {
    // Create brain with prediction system
    // Present sequence
    // Verify prediction accuracy
}

TEST(ConceptIntegration) {
    // Create brain with concept formation
    // Present varied experiences
    // Verify concept formation
}
```

### 2. Test End-to-End Behavior
```cpp
TEST(IntegratedBrainBehavior) {
    // Create fully integrated brain
    // Run lifetime simulation
    // Verify coherent behavior emerges
    // Check that all systems work together
}
```

### 3. Test Developmental Progress
```cpp
TEST(DevelopmentalProgression) {
    // Create developing brain
    // Simulate multiple developmental stages
    // Verify learning progresses appropriately
    // Check that skills mature over time
}
```

## Code Examples for Integration

### Example 1: Simple Sensorimotor Loop with Memory
```python
import pynlm

# Create the integrated system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Enable all memory systems
agent.enableDevelopment(True)

# Run simulation with memory integration
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent (will store in memory)
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Motor action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and update
    reward = world.computeReward(agent.getBrain().getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0f)
    
    # Check memory status
    if step % 100 == 0:
        print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons")
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        print(f"Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")

print("Integration simulation complete!")
```

### Example 2: Planning with Neural Network
```python
import pynlm

# Create brain with planning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Main loop with planning
for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Use neural planner for complex decisions
    if step > 100:  # Give basic learning time first
        action = agent.decodeMotorCommand()
        
        # Enhanced curiosity-driven exploration
        if agent.getCuriosityLevel() > 0.3:
            # Plan multiple steps ahead
            plan = agent.getPlanner()
            if plan:
                plan.setCurrentState(brain.getRegions()[0].get())
                plan.evaluateFutures()
                action = plan.selectAction()
    
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and learn
    reward = world.computeReward(agent.getBrain().getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0f)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 200 == 0:
        print(f"Step {step}: Action {action}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}, Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Working memory: {brain.getWorkingMemory().getActiveTraces()}")

print("Advanced planning simulation complete!")
```

## Testing Integration Checklist

### For Each System Integration:
- [ ] The system is properly instantiated and connected
- [ ] Input flows from one system to the next
- [ ] Output flows from one system back to earlier systems
- [ ] The integration affects behavior meaningfully
- [ ] There are measurable improvements from the integration
- [ ] The integration doesn't break existing functionality

### For Development:
- [ ] Systems develop appropriately over time
- [ ] Learning progresses through developmental stages
- [ ] Memory consolidation improves with development
- [ ] Novelty-driven exploration peaks at appropriate times
- [ ] Skills generalize appropriately

### For End-to-End Behavior:
- [ ] The integrated system shows coherent behavior
- [ ] Memory, prediction, and action work together
- [ ] The system can adapt to new situations
- [ ] There is emergent intelligence from integration

## Priority Matrix for Integration

| System | Integration Complexity | Behavioral Impact | Priority |
|--------|---------------------|------------------|----------|
| Working Memory | Medium | High | HIGH |
| Episodic Memory | Medium | High | HIGH |
| Prediction | Medium | Medium | MEDIUM |
| Concept Formation | High | High | MEDIUM |
| Planning | High | High | MEDIUM |
| Self-Model | Medium | Medium | LOW |
| Full Neuromodulation | High | High | HIGH |
| Development Effects | Medium | High | HIGH |

## Implementation Timeline

### Phase 6a: Memory Integration (Weeks 1-2)
1. Connect working memory to sensory processing
2. Connect episodic memory to experience logging
3. Implement replay mechanism
4. Test memory retention

### Phase 6b: Prediction Integration (Weeks 3-4)
1. Connect prediction system to sensory processing
2. Implement prediction error training
3. Add prediction to attention
4. Test prediction accuracy

### Phase 6c: Full Integration (Weeks 5-6)
1. Connect all systems together
2. Implement neuromodulation effects
3. Add developmental effects beyond plasticity
4. Test end-to-end behavior

### Phase 6d: Validation (Week 7)
1. Test developmental progression
2. Test continual learning
3. Test generalization
4. Profile performance

## Expected Outcomes

### After Integration, You Should See:
1. **Memory Retention:** Experiences are stored and retrieved over time
2. **Learning Progress:** Skills improve and generalize appropriately
3. **Novelty Detection:** The system explores appropriately based on novelty
4. **Planning:** Actions are selected based on predicted outcomes
5. **Development:** Behaviors mature and change over developmental stages
6. **Coherence:** All systems work together as a unified whole

### Measuring Success:
- Memory retention curves
- Prediction accuracy improvement
- Exploration vs exploitation balance
- Developmental progression scores
- Generalization performance
- Behavioral coherence metrics

## Next Steps

1. **Start with Working Memory Integration:** This has the highest impact and is most tractable
2. **Build Integration Test Framework:** Create tests to verify each integration
3. **Implement Incrementally:** Add one system at a time and verify
4. **Profile and Optimize:** Ensure the integration doesn't break performance
5. **Validate Behavior:** Test that the integrated system shows emergent intelligence

This integration guide provides a roadmap for making the existing NLM systems work together as a coherent artificial brain. The key insight is that **Phase 6 is about integration, not adding new features.**