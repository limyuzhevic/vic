# NLM Python API - Integration Opportunities Document

## Overview

This document details the specific C++ systems that must be exposed through Python bindings to achieve Phase 6's goal of making all systems work together as a coherent artificial brain.

## Critical Integration Points (Phase 6 Mandate)

### 1. Memory Systems Integration

#### Current State (Phase 6 Final Audit)
- **WorkingMemory**: Defined but disconnected (returns nullptr)
- **EpisodicMemory**: Basic struct only, never updated
- **SemanticMemory**: Empty implementation
- **ProceduralMemory**: Empty implementation

#### Required Python API Exposure

```python
# Working Memory API
brain.getWorkingMemory().store(neuron_id, value)
brain.getWorkingMemory().retrieve(neuron_id)
brain.getWorkingMemory().contains(neuron_id)
brain.getWorkingMemory().clear()
brain.getWorkingMemory().getCapacity()
brain.getWorkingMemory().getCurrentSize()
brain.getWorkingMemory().decay(decay_rate)

# Episodic Memory API
brain.getEpisodicMemory().storeEpisode({
    'timestamp': step,
    'neurons': [neuron_id1, neuron_id2],
    'values': [0.5, 0.8],
    'metadata': 'approach resource'
})
episode = brain.getEpisodicMemory().retrieveEpisode(index)
brain.getEpisodicMemory().getRecentEpisodes(count)
brain.getEpisodicMemory().consolidate(relevance_threshold)

# Semantic Memory API
brain.getSemanticMemory().storeFact('predator_detected', 'true')
value = brain.getSemanticMemory().retrieveFact('predator_detected')
brain.getSemanticMemory().hasFact('predator_detected')
brain.getSemanticMemory().getAllFacts()

# Procedural Memory API
brain.getProceduralMemory().learnSkill('avoid_predator', [neuron_id1, neuron_id2])
skill = brain.getProceduralMemory().getSkill('avoid_predator')
brain.getProceduralMemory().getAllSkills()
brain.getProceduralMemory().updateProficiency('avoid_predator', 0.1)
```

#### C++ Integration Requirements

**Brain.hpp additions**:
```cpp
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
SemanticMemory* getSemanticMemory();
ProceduralMemory* getProceduralMemory();
```

**Brain.cpp implementation**:
```cpp
// Replace nullptr returns with actual implementations
NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    }
    return pImpl->workingMemory.get();
}

// Similar implementations for other memory systems
```

**Integration Points**:
1. **Brain::step()**: Update working memory with sensory input
2. **AgentBrain::processSensoryInput()**: Store experiences in episodic memory
3. **Development::update()**: Learn skills in procedural memory
4. **Prediction::update()**: Update semantic knowledge

### 2. Prediction System Integration

#### Current State (Phase 6 Final Audit)
- **PredictionSystem**: Implemented but disconnected (returns nullptr)
- **PredictionError**: In AgentBrain but not connected to prediction system

#### Required Python API Exposure

```python
# Prediction System API
prediction = brain.getPredictionSystem()
prediction.train(sensory_input)  # Learn forward model
prediction.setTarget(target_state)  # Set prediction target
prediction.update()  # Update prediction
prediction.getError()  # Get prediction error
prediction.getConfidence()  # Get prediction confidence
prediction.getNextStatePrediction()  # Get predicted next state
prediction.getHistory()  # Get prediction history
prediction.getModelWeights()  # Get model parameters

# Prediction Error API
error = agent.getPredictionError()
error.setPredicted(reward_prediction)
error.setActual(actual_reward)
error.getError()  # Prediction error magnitude
error.getSign()  # Positive (surprise) or negative (prediction)

# Integration API
prediction.setSensoryInput(percept)
prediction.setAction(action)
prediction.updateDevelopment(plasticity_rate)
```

#### C++ Integration Requirements

**PredictionSystem.hpp**:
```cpp
class PredictionSystem {
public:
    void train(const SensoryInput& input, const Action& action);
    void setTarget(const std::vector<float>& target);
    void update();
    float getError() const;
    float getConfidence() const;
    std::vector<float> getNextStatePrediction() const;
    std::vector<PredictionHistory> getHistory() const;
    
    // Integration APIs
    void setSensoryInput(const SensoryInput& input);
    void setAction(const Action& action);
    void updateDevelopment(float plasticityRate);
};
```

**Brain.cpp integration**:
```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing code ...
    
    // Update prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update();
    }
    
    // ... rest of code ...
}
```

**Integration Points**:
1. **Brain::step()**: Update prediction system with current state
2. **AgentBrain::applyRewardModulation()**: Learn from prediction error
3. **Development::update()**: Update prediction model with experience

### 3. Cognitive System Integration

#### Current State (Phase 6 Final Audit)
- **NeuralPlanner**: Implemented but disconnected (never instantiated)
- **ConceptFormation**: Implemented but disconnected (never used)
- **SelfModel**: Implemented but disconnected (never updated)
- **SocialLearning**: Implemented but disconnected (never observes)
- **SpatialRepresentation**: Implemented but disconnected (never used)
- **TemporalRelation**: Implemented but disconnected (never used)

#### Required Python API Exposure

```python
# Neural Planner API
planner = brain.getPlanner()
planner.setGoal('find food', priority=1.0)
planner.setConstraint('avoid_predator', threshold=0.8)
planned_action = planner.plan()
action_sequence = planner.getActionSequence()
planner.getExpectedOutcome(planned_action)

# Concept Formation API
concept = brain.getConceptFormation()
concept.extractFeatures(percept)
concept.associateFeatures('food', [sensory_feature1, sensory_feature2])
concept.getConcept('food')
concept.getAllConcepts()
concept.getConceptSimilarities()

# Self Model API
self_model = brain.getSelfModel()
self_model.update(percept, action, reward)
self_model.setBelief('agent_is_alive', True)
self_model.setGoal('survive', priority=1.0)
self_model.getBeliefs()
self_model.getGoals()
self_model.getIntentions()
self_model.getBehaviorPredictions()

# Social Learning API
social = brain.getSocialLearning()
social.observeAgent('agent_1', action_sequence)
social.learnFromObservation('agent_1', outcome)
social.getSocialKnowledge()
social.getSocialRules()

# Spatial Representation API
spatial = brain.getSpatialRepresentation()
spatial.update(position, orientation)
spatial.getDistanceTo(target_position)
spatial.getDirectionTo(target_position)
spatial.getKnownLocations()
spatial.getHomeLocation()

# Temporal Relation API
temporal = brain.getTemporalRelation()
temporal.update(temporal_context)
temporal.getTemporalOrder(event1, event2)
temporal.getDuration(event)
temporal.getCausality(event1, event2)
```

#### C++ Integration Requirements

**Brain.hpp additions**:
```cpp
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
SelfModel* getSelfModel();
SocialLearning* getSocialLearning();
SpatialRepresentation* getSpatialRepresentation();
TemporalRelation* getTemporalRelation();
```

**Brain.cpp implementation**:
```cpp
// Instantiate and connect cognitive systems in Brain::initialize()
bool Brain::initialize() {
    // ... existing initialization ...
    
    pImpl->neuralPlanner = std::make_unique<NeuralPlanner>(this);
    pImpl->conceptFormation = std::make_unique<ConceptFormation>(this);
    pImpl->selfModel = std::make_unique<SelfModel>(this);
    pImpl->socialLearning = std::make_unique<SocialLearning>(this);
    pImpl->spatialRepresentation = std::make_unique<SpatialRepresentation>(this);
    pImpl->temporalRelation = std::make_unique<TemporalRelation>(this);
    
    return true;
}
```

**Integration Points**:
1. **Brain::step()**: Update cognitive systems with current state
2. **AgentBrain::processSensoryInput()**: Feed percepts to concept formation
3. **Development::update()**: Update self model and social learning
4. **Prediction::update()**: Use spatial/temporal representations

### 4. Neuromodulation System Integration

#### Current State (Phase 6 Final Audit)
- **Dopamine**: Limited to STDP weight scaling
- **Serotonin**: Stub implementation
- **Norepinephrine**: Stub implementation
- **Acetylcholine**: Stub implementation

#### Required Python API Exposure

```python
# Dopamine API (expanded beyond STDP scaling)
dopamine = brain.getDopamine()
dopamine.setLevel(reward_prediction_error)
dopamine.applyToPlasticity()  # Affects all plasticity rules
dopamine.applyToMemoryConsolidation()  # Affects memory strength
dopamine.applyToAttention()  # Affects attention allocation
dopamine.applyToMotivation()  # Affects action selection

# Novelty API
novelty = agent.getNovelty()
novelty.setSensitivity(sensitivity)
novelty.setDecayRate(decay_rate)
detected_novelty = novelty.detect(percept)

# Curiosity API
curiosity = agent.getCuriosity()
curiosity.setLevel(curiosity_level)
curiosity.setIntegrationStrength(strength)
curiosity.getExplorationDrive()

# Acetylcholine API (implemented)
ach = brain.getAcetylcholine()
ach.setLevel(attention_level)
ach.applyToWorkingMemory(memory_strength)
ach.applyToEpisodicMemory(consolidation_rate)

# Serotonin API (implemented)
serotonin = brain.getSerotonin()
serotonin.setLevel(mood_level)
serotonin.applyToMood(mood_influence)

# Norepinephrine API (implemented)
 noradrenaline = brain.getNorepinephrine()
noradrenaline.setLevel(arousal_level)
noradrenaline.applyToArousal(arousal_effect)
```

#### C++ Integration Requirements

**Neuromodulator.hpp enhancements**:
```cpp
class Dopamine : public Neuromodulator {
public:
    void setLevel(float level);
    void applyToPlasticity();  // Affects all plasticity rules
    void applyToMemoryConsolidation();  // Working + episodic memory
    void applyToAttention();  // Attentional selection
    void applyToMotivation();  // Action selection
    
    // Enhanced beyond current STDP scaling
    void modulateHebbianLearning(bool enable);
    void modulateSTDP(bool enable);
    void modulateStructuralPlasticity(bool enable);
};

// Implement remaining neuromodulators
class Serotonin : public Neuromodulator {
public:
    void setLevel(float level);
    void applyToMood(float influence);
    void applyToSleepCycle(bool enable);
    void applyToMemoryReconsolidation(bool enable);
};

class Norepinephrine : public Neuromodulator {
public:
    void setLevel(float level);
    void applyToArousal(float effect);
    void applyToAlertness(bool enable);
    void applyToWorkingMemoryCapacity(int capacity_increase);
};

class Acetylcholine : public Neuromodulator {
public:
    void setLevel(float level);
    void applyToAttention(float strength);
    void applyToSensoryProcessing(float gain);
    void applyToMemoryEncoding(float rate);
};
```

**Integration Points**:
1. **Brain::step()**: Apply neuromodulation to all systems
2. **AgentBrain::applyRewardModulation()**: Use dopamine for reward learning
3. **Development::update()**: Use neuromodulators for developmental changes
4. **Memory::consolidate()**: Use neuromodulators for memory consolidation

### 5. Development System Integration

#### Current State (Phase 6 Final Audit)
- **DevelopmentSystem**: Only affects structural plasticity rates
- **Synaptogenesis**: Working, every 100 steps
- **Pruning**: Working, every 100 steps
- **Maturation**: Limited to rate changes

#### Required Python API Exposure

```python
# Development System API
development = brain.getDevelopmentSystem()
development.setStage(stage)  # Initial -> CriticalPeriod -> Adult -> Aging
development.update(timestep)  # Update developmental processes
development.getStage()
   1: Initial -> High plasticity, basic learning
   2: CriticalPeriod -> Enhanced sensory learning
   3: Maturation -> Specialized processing
   4: Adult -> Stable performance
   5: Aging -> Declining capabilities
   1: Synaptogenesis (new connections) 2: Pruning (connection removal)
   3: Maturation (connection strengthening)
   4: Specialization (region differentiation)
   5: Degradation (age-related decline)

development.getAge()

development.getPlasticityRate()  # Current plasticity rate
development.setPlasticityRate(rate)

development.getSynaptogenesisRate()

development.getPruningRate()

development.getMaturationRate()

development.getSpecializationRate()

development.getDegradationRate()

# Developmental Stage API
brain.setDevelopmentalStage('CriticalPeriod')
stage = brain.getDevelopmentalStage()
stage.isActive('synaptogenesis')
stage.isActive('pruning')
stage.isActive('maturation')

# Development Configuration
development.enableSynaptogenesis(True)

development.enablePruning(True)

development.enableMaturation(True)

development.enableSpecialization(True)

development.enableDegradation(True)
```

#### C++ Integration Requirements

**DevelopmentSystem.hpp**:
```cpp
class DevelopmentSystem {
public:
    void update(double timestep);
    void setStage(DevelopmentalStage stage);
    DevelopmentalStage getStage() const;
    double getAge() const;
    
    // Individual developmental processes
    void updateSynaptogenesis();
    void updatePruning();
    void updateMaturation();
    void updateSpecialization();
    void updateDegradation();
    
    // Process control
    void enableSynaptogenesis(bool enable);
    void enablePruning(bool enable);
    void enableMaturation(bool enable);
    void enableSpecialization(bool enable);
    void enableDegradation(bool enable);
    
    // Rate control
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    float getMaturationRate() const;
    float getSpecializationRate() const;
    float getDegradationRate() const;
    
    void setSynaptogenesisRate(float rate);
    // ... similar for other processes
};
```

**Brain.cpp integration**:
```cpp
void Brain::develop() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->simulationClock->getTimeStep());
    }
}
```

**Integration Points**:
1. **Brain::step()**: Call development system update
2. **Neuromodulation::apply()**: Development affects neuromodulation levels
3. **Memory::update()**: Development affects memory consolidation
4. **Prediction::update()**: Development affects prediction capabilities

### 6. Persistence Integration

#### Current State (Phase 6 Final Audit)
- **CheckpointSystem**: Fully implemented but disconnected
- **Brain::save()**: Stub implementation (always returns false)
- **Brain::load()**: Stub implementation (always returns false)

#### Required Python API Exposure

```python
# Checkpoint System API
checkpoint = CheckpointSystem()
checkpoint.saveBrain(brain, "experiment_1_checkpoint")
checkpoint.saveWorld(world, "experiment_1_checkpoint")
checkpoint.saveAgent(agent, "experiment_1_checkpoint")

# Load checkpoint
loaded_brain = checkpoint.loadBrain("experiment_1_checkpoint")
loaded_world = checkpoint.loadWorld("experiment_1_checkpoint")
loaded_agent = checkpoint.loadAgent("experiment_1_checkpoint")

# Checkpoint Manager API
manager = CheckpointManager()
manager.setDirectory("checkpoints/experiment_1")
manager.setRetentionPolicy("weekly")
manager.setMaxCheckpoints(100)

checkpoint_path = manager.createCheckpoint(brain, world, agent)
manager.restoreCheckpoint(checkpoint_path)
manager.listCheckpoints()
manager.deleteCheckpoint(checkpoint_path)

# Brain persistence (direct)
brain.save("detailed_checkpoint")
loaded_brain = Brain("config").load("detailed_checkpoint")

# World persistence
world.save("world_state")
loaded_world = SimpleWorld().load("world_state")

# Agent persistence
agent.save("agent_state")
loaded_agent = AgentBrain(brain).load("agent_state")
```

#### C++ Integration Requirements

**CheckpointSystem.hpp**:
```cpp
class CheckpointSystem {
public:
    bool saveBrain(const Brain& brain, const std::string& filepath) const;
    bool loadBrain(Brain& brain, const std::string& filepath);
    
    bool saveWorld(const SimpleWorld& world, const std::string& filepath) const;
    bool loadWorld(SimpleWorld& world, const std::string& filepath);
    
    bool saveAgent(const AgentBrain& agent, const std::string& filepath) const;
    bool loadAgent(AgentBrain& agent, const std::string& filepath);
    
    // Checkpoint management
    std::string createCheckpoint(const Brain& brain, const SimpleWorld& world, const AgentBrain& agent);
    bool restoreCheckpoint(const std::string& filepath);
    std::vector<std::string> listCheckpoints() const;
    bool deleteCheckpoint(const std::string& filepath);
};
```

**Brain.hpp implementation**:
```cpp
// Implement Brain::save() and Brain::load() using CheckpointSystem
bool Brain::save(const std::string& filepath) const {
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->saveBrain(*this, filepath);
    }
    return false;
}

bool Brain::load(const std::string& filepath) {
    if (pImpl->checkpointSystem) {
        return pImpl->checkpointSystem->loadBrain(*this, filepath);
    }
    return false;
}
```

**Integration Points**:
1. **AgentBrain::reset()**: Can restore from checkpoint instead of fresh reset
2. **Development::update()**: Can load developmental state from checkpoint
3. **Memory::update()**: Can restore episodic memory from checkpoint
4. **Prediction::update()**: Can restore prediction models from checkpoint

## Implementation Priority Matrix

| System | C++ Files to Modify | Python Files to Modify | Integration Points | Criticality |
|--------|-------------------|------------------------|-------------------|-------------|
| Memory Systems | Brain.cpp, Brain.hpp | python/bindings.cpp | Brain::step(), AgentBrain::processSensoryInput() | CRITICAL |
| Prediction System | PredictionSystem.hpp, Brain.cpp | python/bindings.cpp | Brain::step(), AgentBrain::applyRewardModulation() | CRITICAL |
| Cognitive Systems | All cognition headers, Brain.cpp | python/bindings.cpp | Brain::step(), AgentBrain::processSensoryInput() | CRITICAL |
| Neuromodulation | All neuromodulators, Brain.cpp | python/bindings.cpp | Brain::step(), Development::update() | HIGH |
| Development System | Development.hpp, Brain.cpp | python/bindings.cpp | Brain::step(), Neuromodulation::apply() | HIGH |
| Persistence | CheckpointSystem.hpp, Brain.cpp | python/bindings.cpp | AgentBrain::reset(), Development::update() | CRITICAL |

## Files to Modify (Immediate)

### High Priority (Phase 6 Immediate Integration):
1. **src/brain/Brain.cpp** - Implement all system accessors and connect cognitive systems
2. **src/brain/Brain.hpp** - Add all missing method declarations
3. **python/bindings.cpp** - Expose all C++ systems through Python

### Medium Priority (Phase 6 Implementation):
1. **src/prediction/PredictionSystem.hpp** - Ensure complete prediction system implementation
2. **src/cognition/** - Ensure all cognitive systems are complete
3. **src/neuromodulation/** - Implement missing neuromodulators (ACh, NE, 5-HT)

### Low Priority (Phase 6 Enhancement):
1. **src/performance/** - Integrate performance infrastructure
2. **src/experiments/** - Add experiment framework integration
3. **python/CMakeLists.txt** - Add any additional Python dependencies

## Expected Integration Results

After implementing these integration points:

- **Memory Integration**: Experiences stored in episodic memory, working memory supports recurrent processing
- **Prediction Integration**: Forward models learn from experience, prediction errors guide learning
- **Cognition Integration**: Planning, concept formation, self-model all influence behavior
- **Neuromodulation Integration**: All neuromodulators affect plasticity, memory, attention, motivation
- **Development Integration**: Full lifespan development with stage-specific processes
- **Persistence Integration**: Complete checkpoint system with memory replay capabilities

**Overall Result**: From **35.8% exposed** to **>80% integrated** artificial brain functionality

**Timeline**: 2-3 weeks for complete Phase 6 integration
**Risk Level**: LOW (exposing existing C++ functionality)

This document provides the complete blueprint for achieving Phase 6's goal of making NLM an integrated artificial brain rather than a disconnected collection of components.