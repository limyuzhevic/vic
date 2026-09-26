# NLM Phase 6 Integration Guide

This guide provides comprehensive instructions for integrating the Phase 6 NLM systems into your projects and understanding how all components work together.

## Table of Contents

1. [Overview of Phase 6 Integration](#overview-of-phase-6-integration)
2. [Brain System Integration](#brain-system-integration)
3. [Memory Systems Integration](#memory-systems-integration)
4. [Neuromodulation Integration](#neuromodulation-integration)
5. [Cognition Systems Integration](#cognition-systems-integration)
6. [Prediction System Integration](#prediction-system-integration)
7. [Development System Integration](#development-system-integration)
8. [Persistence Integration](#persistence-integration)
9. [Replay and Consolidation Integration](#replay-and-consolidation-integration)
10. [Experiment Integration](#experiment-integration)
11. [Python API Integration](#python-api-integration)
12. [Best Practices](#best-practices)
13. [Troubleshooting](#troubleshooting)

## Overview of Phase 6 Integration

The Phase 6 integration represents a fundamental shift from disconnected components to a fully integrated artificial brain. All systems now work together in a coherent loop:

```
PERCEPTION → SENSORY NEURONS → WORKING MEMORY → ATTENTION → 
CONCEPT FORMATION → PLANNING → ACTION SELECTION → MOTOR OUTPUT → 
WORLD CONSEQUENCE → REWARD/ERROR → NEUROMODULATION → 
PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN → 
CHANGED FUTURE BEHAVIOR
```

### Key Integration Principles

1. **Systems Work Together**: Each system affects and is affected by others
2. **Hierarchical Organization**: Higher-level cognition (concepts, planning) guides lower-level neural dynamics
3. **Continuous Learning**: All systems continuously update from experience
4. **Emergent Behavior**: Complex behavior emerges from simple neural interactions
5. **Developmental Trajectory**: Systems develop and adapt over time

## Brain System Integration

The Brain class is the central hub of Phase 6 integration:

### Core Components

```cpp
struct Brain::Impl {
    // All integrated systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Core neural components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
};
```

### Integration Points

1. **Memory Access**: Through Brain accessor methods
   ```cpp
   auto* workingMem = brain->getWorkingMemory();
   auto* episodicMem = brain->getEpisodicMemory();
   ```

2. **Cognition Access**: Through Brain accessor methods
   ```cpp
   auto* conceptForm = brain->getConceptFormation();
   auto* planner = brain->getPlanner();
   auto* attention = brain->getAttention();
   ```

3. **Neuromodulation Access**: Through Brain accessor methods
   ```cpp
   auto* dopamine = brain->getDopamine();
   auto* curiosity = brain->getCuriosity();
   auto* predictionError = brain->getPredictionErrorSignal();
   ```

### Integration Flow

1. **Initialization**: All systems initialized in `Brain::initialize()`
2. **Brain Loop**: 15-step integrated loop in `Brain::step()`
3. **Access**: Systems accessed through Brain accessor methods
4. **Update**: All systems updated in each brain step
5. **Interaction**: Systems interact and influence each other

## Memory Systems Integration

### Working Memory Integration

**Purpose**: Maintains active neural traces for immediate use

**Integration Details**:
- Connected to sensory processing in `Brain::receiveSensoryInput()`
- Updated in brain step (STEP 4)
- Provides competition for trace capacity
- Supplies patterns to concept formation

**Key Methods**:
```cpp
// In Brain.cpp
void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons
    // Store in working memory if significant
    if (pImpl->workingMemory && normalizedValue > 0.5f) {
        pImpl->workingMemory->storeToNeuron(...);
    }
}

// In Brain::step()
if (pImpl->workingMemory) {
    pImpl->workingMemory->update(pImpl->timestep);
}
```

### Episodic Memory Integration

**Purpose**: Stores experienced episodes for replay and consolidation

**Integration Details**:
- Updated with experiences every 10 steps
- Stores active neurons and their activations
- Triggers replay for important episodes
- Provides replay during simulation

**Key Methods**:
```cpp
// In Brain::step()
pImpl->stepsSinceLastEpisode++;
if (pImpl->stepsSinceLastEpisode >= 10) {
    pImpl->stepsSinceLastEpisode = 0;
    
    if (pImpl->episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        // ... store neuron activations, reward
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

// Replay integration
if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}
```

### Associative Memory Integration

**Purpose**: Forms connections between related patterns

**Integration Details**:
- Used by concept formation to recognize patterns
- Provides generalization capabilities
- Updated from concept formation

**Key Methods**:
```cpp
// In ConceptFormation.cpp
void ConceptFormation::updateConceptFromError(...) {
    // Use associative memory for pattern matching
    std::vector<float> similarity = associativeMemory->getSimilarity(
        oldPrototype, newPattern);
    
    // Update concept based on similarity
    if (similarity > threshold) {
        // Merge with similar concept
    }
}
```

## Neuromodulation Integration

### Dopamine Integration

**Purpose**: Reward prediction error, affects learning and neural excitability

**Integration Details**:
- Connected to reward prediction error from AgentBrain
- Affects neural excitability in all neuron populations
- Modulates plasticity factors for STDP

**Key Methods**:
```cpp
// In AgentBrain::applyRewardModulation()
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    predictionError_ = reward - predictedReward;
    dopamineLevel_ = predictionError_;
    // Apply to all synapses with eligibility traces
}

// In Brain::step()
if (pImpl->dopamine) {
    pImpl->dopamine->update(pImpl->timestep);
    
    // Apply dopamine effects on neural excitability
    float dopamineLevel = pImpl->dopamine->getLevel();
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}
```

### Curiosity Integration

**Purpose**: Drives exploration behavior based on novelty and prediction error

**Integration Details**:
- Combines novelty and prediction error
- Connected to action selection for exploration
- Affects working memory search intensity

**Key Methods**:
```cpp
// In AgentBrain.cpp
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

void AgentBrain::applyCuriosityExploration() {
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
}

// In Curiosity.cpp
void Curiosity::update(TimestepDuration dt) {
    curiosityLevel_ = noveltyLevel_ * 2.0f + predictionError_ * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Apply curiosity effects to behavior
    if (brain_ && agentBrain_) {
        if (curiosityLevel_ > explorationThreshold) {
            agentBrain_->enableCuriosityExploration(true);
        } else {
            agentBrain_->enableCuriosityExploration(false);
        }
    }
}
```

### Prediction Error Integration

**Purpose**: Signals learning opportunities, affects attention and memory

**Integration Details**:
- Connected to learning in AgentBrain
- Affects attentional focus
- Enhances working memory updating

**Key Methods**:
```cpp
// In PredictionError.cpp
void PredictionError::update(TimestepDuration dt) {
    predictionError_ = calculatePredictionError();
    
    // Apply prediction error to neuromodulation
    if (dopamine_) {
        dopamine_->signalRewardPredictionError(predictionError_);
    }
    
    // Apply prediction error to attention
    if (attention_) {
        attention_->setPredictionErrorBias(std::abs(predictionError_));
    }
    
    // Apply prediction error to memory
    if (brain_ && workingMemory_) {
        workingMemory_->setLearningRate(std::min(1.0f, std::abs(predictionError_)));
    }
}
```

## Cognition Systems Integration

### Concept Formation Integration

**Purpose**: Abstracts patterns from neural activity, forms stable prototypes

**Integration Details**:
- Receives patterns from working memory
- Updates concepts from experiences and prediction errors
- Provides prediction targets
- Merges similar concepts

**Key Methods**:
```cpp
// In Brain::step()
if (pImpl->conceptFormation && pImpl->dopamine) {
    // Extract pattern from working memory
    std::vector<float> pattern;
    // ... pattern extraction code ...
    
    // Present experience to concept formation
    float reward = pImpl->dopamine->getLevel();
    size_t conceptId = pImpl->conceptFormation->presentExperience(
        pattern, features, reward, currentStep);
}

// Concept merging
void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    auto& protoA = concepts[conceptA].prototype;
    auto& protoB = concepts[conceptB].prototype;
    
    // Create new merged prototype
    std::vector<float> mergedPrototype;
    mergedPrototype.insert(mergedPrototype.end(), protoA.begin(), protoA.end());
    mergedPrototype.insert(mergedPrototype.end(), protoB.begin(), protoB.end());
    
    // Normalize and update
    concepts[conceptA].prototype = std::move(mergedPrototype);
    concepts[conceptA].stability = (concepts[conceptA].stability + concepts[conceptB].stability) * 0.5f;
}
```

### Neural Planner Integration

**Purpose**: Generates action sequences based on predicted outcomes

**Integration Details**:
- Uses concepts as goals
- Evaluates predictions for outcome evaluation
- Connected to action selection

**Key Methods**:
```cpp
// In Brain::step()
if (pImpl->conceptFormation && conceptId > 0 && pImpl->planner) {
    auto conceptProto = pImpl->conceptFormation->getConceptPrototype(conceptId);
    pImpl->planner->setGoal(conceptProto);
    
    std::vector<ActionSequence> plans = pImpl->planner->planActions(
        currentInput, conceptProto, 
        pImpl->predictionSystem, 
        currentStep);
}

// In NeuralPlanner.cpp
void NeuralPlanner::applyAttention(const AttentionalSelection& attention) {
    // Get attentional focus
    AttentionWeights focus = attention.getCurrentFocus();
    
    // Prioritize planning for attended concepts
    for (auto& sequence : actionSequences) {
        float attentionWeight = focus.getWeightForTarget(sequence.target);
        sequence.utility *= attentionWeight;
    }
}
```

### Attention Integration

**Purpose**: Competitive selection of concepts and actions

**Integration Details**:
- Processes working memory for competition
- Integrates with concept formation
- Affects neural planning

**Key Methods**:
```cpp
// In Brain::step()
// ========== STEP 9: Update attention system ==========
if (pImpl->attention) {
    pImpl->attention->update(pImpl->timestep);
    
    // Apply attention to working memory winners
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
        pImpl->attention->processCompetition(competitors);
    }
}
```

## Prediction System Integration

### Prediction Integration

**Purpose**: Anticipates future states based on current input

**Integration Details**:
- Updated in brain step (STEP 8)
- Uses concepts as prediction targets
- Provides prediction errors for learning
- Influences attention and planning

**Key Methods**:
```cpp
// In Brain::step()
// ========== STEP 8: Update prediction system ==========
if (pImpl->predictionSystem) {
    auto currentInput = std::make_unique<InternalSignals>();
    
    // Integrate concepts as prediction targets
    if (pImpl->conceptFormation) {
        auto conceptPrototypes = pImpl->conceptFormation->getConcepts();
        float totalConceptInfluence = 0.0f;
        
        for (const auto& concept : conceptPrototypes) {
            float conceptStability = conceptFormation->getConceptStability(concept.id);
            float influence = conceptStability * (concept.totalObservations / 10.0f);
            totalConceptInfluence += influence;
            
            // Blend concept prototype with prediction
            std::vector<float> conceptInfluence = conceptFormation->getConceptInfluence(concept.id);
            if (!conceptInfluence.empty()) {
                for (size_t i = 0; i < std::min(conceptInfluence.size(), currentInput->getData().size()); ++i) {
                    currentInput->getData()[i] = currentInput->getData()[i] * (1.0f - pImpl->predictionSystem->getConceptUpdateSignal()) + 
                                                 conceptInfluence[i] * pImpl->predictionSystem->getConceptUpdateSignal() * influence;
                }
            }
        }
    }
    
    pImpl->predictionSystem->updatePredictions(*currentInput, *currentInput);
}
```

### Concept-to-Prediction Integration

**Purpose**: Use concepts to guide prediction learning

**Integration Details**:
- Concepts provide targets for prediction
- Concept stability influences prediction confidence
- Concept influence affects prediction parameters

**Key Methods**:
```cpp
// In PredictionSystem.cpp
void PredictionSystem::updateFromConcepts(const ConceptFormation& conceptFormation) {
    const auto& concepts = conceptFormation.getConcepts();
    
    for (const auto& concept : concepts) {
        if (conceptFormation.getConceptStability(concept.id) > 0.7f) {
            addConceptTarget(concept);
            
            float conceptEntropy = calculateConceptEntropy(concept.prototype);
            adjustPredictionParameters(conceptEntropy, concept.totalObservations);
        }
    }
}
```

### Prediction-to-Concept Integration

**Purpose**: Use prediction errors to update concepts

**Integration Details**:
- Prediction errors guide concept refinement
- High prediction errors trigger concept updates
- Low prediction errors reinforce concepts

**Key Methods**:
```cpp
// In ConceptFormation.cpp
void ConceptFormation::updateConceptFromError(size_t conceptId,
                                           const std::vector<float>& oldPattern,
                                           const std::vector<float>& newPattern,
                                           float predictionError) {
    if (predictionError > 0.1f && predictionError < 0.9f) {
        // Update concept prototype based on prediction error
        std::vector<float> updatedPrototype;
        for (size_t i = 0; i < oldPattern.size(); ++i) {
            float weight = 0.5f + 0.5f * predictionError;
            updatedPrototype.push_back(oldPattern[i] * (1.0f - weight) + 
                                    newPattern[i] * weight);
        }
        
        concepts[conceptId].prototype = std::move(updatedPrototype);
        concepts[conceptId].stability = std::max(0.1f, 
                                                concepts[conceptId].stability - 0.1f);
    }
}
```

## Development System Integration

### Development Integration

**Purpose**: Age-based modulation of learning and plasticity

**Integration Details**:
- Development stages affect neural excitability
- Developmental effects on attention and memory
- Development affects neuromodulation levels
- Connected to structural plasticity

**Key Methods**:
```cpp
// In Brain::step()
// ========== STEP 13: Apply development effects ==========
if (currentStep % 1000 == 0) {  // Update development every 1000 steps
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    
    // Development affects plasticity rates
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
        DevelopmentalStage stage = pImpl->developmentalStage;
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}
```

### Development Effects

**Comprehensive development effects**:

1. **Neural Excitability**: Developmental stages modulate neural responsiveness
2. **Plasticity Rules**: STDP and Hebbian plasticity affected by developmental stage
3. **Attention**: Attentional focus and competition dynamics change with development
4. **Memory**: Memory consolidation and replay affected by developmental age
5. **Neuromodulation**: Dopamine, curiosity, and other neuromodulators affected
6. **Structural Plasticity**: Synaptogenesis and pruning rates modulated

## Persistence Integration

### Checkpoint Integration

**Purpose**: Save and load brain states with full system integration

**Integration Details**:
- Checkpoint system integrated with all brain components
- Save/load of neural states, memory, and development
- Restoration of simulation state

**Key Methods**:
```cpp
// In Brain::save()
bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Configure checkpoint manager
        pImpl->checkpointManager->configure("./checkpoints", 10000, 5, true);
        
        // Set up providers for all systems
        pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
            // Fill neuron data from all regions
            // ... implementation ...
            return !data.membranePotential.empty();
        });
        
        // Similar providers for synapses, memory, etc.
        
        // Save checkpoint
        if (!pImpl->checkpointManager->saveImmediately(filepath)) {
            NLM_LOG_ERROR("Failed to save brain state");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}
```

### Load Integration

**Purpose**: Restore brain states with full system coherence

**Key Features**:
- Neural state restoration
- Memory system state restoration
- Development system state restoration
- Neuromodulation state restoration
- Prediction system state restoration

## Replay and Consolidation Integration

### Replay Integration

**Purpose**: Reactivate important experiences for memory consolidation

**Integration Details**:
- Replay during simulation for learning
- Replay during rest for consolidation
- Controlled by developmental stage
- Integrated with neuromodulation

**Key Methods**:
```cpp
// In Brain::step()
// ========== STEP 12: Replay important memories ==========
if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
    // Get episodes for replay
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}
```

### Consolidation Integration

**Purpose**: Long-term memory stabilization

**Integration Details**:
- Consolidation during rest cycles
- Developmental stage modulation
- Replay-enhanced consolidation
- Integration with working memory

**Key Methods**:
```cpp
// In Brain::step()
// ========== STEP 14: Periodic memory consolidation ==========
if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
    // Consolidate important memories, remove weak ones
    pImpl->episodicMemory->consolidate(0.3f);
}
```

## Experiment Integration

### Experiment Framework

**Purpose**: Systematic testing and validation of Phase 6 integration

**Integration Details**:
- Comprehensive experiment framework
- Metrics collection and analysis
- Validation of all system integrations
- Comparison with baseline systems

**Key Features**:

1. **Integration Verification**: Verify all systems are connected
2. **Memory Testing**: Test memory retention and consolidation
3. **Neuromodulation Testing**: Test reward learning and exploration
4. **Prediction Testing**: Test prediction accuracy and learning
5. **Development Testing**: Test developmental progression
6. **Persistence Testing**: Test save/load functionality
7. **Replay Testing**: Test memory replay mechanisms

### Experiment API

```python
# Create experiment
experiment = Phase6IntegrationExperiment()
\n# Configure experiment
config = Phase6Config()
config.neuronCount = 1000
config.maxSteps = 5000
config.enableCheckpointing = True
config.enableReplay = True
config.enableDevelopment = True
\n# Run experiment
result = experiment.run(config)
\n# Analyze results
print(f"Total reward: {result.totalReward}")
print(f"Memory episodes stored: {result.memoryEpisodesStored}")
print(f"Integration status: {result.developmentIntegrated}")
```

## Python API Integration

### Core Integration

The Python API provides direct access to all Phase 6 integrated systems:

```python
from nlm import Brain, AgentBrain, SimpleWorld, Phase6IntegrationExperiment

# Create brain with all Phase 6 features
brain = Brain({
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1
})
\nbrain.initialize()
\n# Access all integrated systems
concept_form = brain.getConceptFormation()
planner = brain.getPlanner()
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()
dopamine = brain.getDopamine()
prediction = brain.getPredictionSystem()
```

### Advanced Integration Examples

1. **Concept Formation with Planning**:
   ```python
   # Learn concepts from experience
   concept_id = concept_form.presentExperience(pattern, features, reward)
   \n   # Use concepts for planning
   plans = planner.planActions(current_input, concept_proto)
   ```

2. **Memory Replay with Prediction**:
   ```python
   # Get episodes for replay
   episodes = episodic_mem.getEpisodesForReplay(5)
   \n   # Replay affects prediction
   prediction.updateFromReplay(episodes)
   ```

3. **Neuromodulation Integration**:
   ```python
   # Use neuromodulation levels
   curiosity_level = curiosity.getLevel()
   prediction_error = prediction_error.getSignal()
   \n   # Adapt behavior based on neuromodulation
   if curiosity_level > threshold:
       enable_exploration()
   ```

## Best Practices

### 1. Understand System Dependencies

All Phase 6 systems are interdependent. Changes to one system affect others:

- Memory systems affect concept formation
- Neuromodulation affects learning and attention
- Prediction affects planning
- Development affects all system rates

### 2. Follow Integration Flow

1. **Initialize** all systems in proper order
2. **Update** systems in brain loop
3. **Access** systems through Brain accessor methods
4. **Interact** systems through shared data and interfaces

### 3. Monitor System Health

Track key indicators of system integration:

```python
# Monitor integration health
brain = getBrain()
\n# Check system accessibility
memory_working = brain.getWorkingMemory() is not None
concept_working = brain.getConceptFormation() is not None
prediction_working = brain.getPredictionSystem() is not None
\n# Check system activity
if memory_working:
    active_traces = brain.getWorkingMemory().getActiveTraces()
    print(f"Working memory active traces: {active_traces}")
```

### 4. Use Proper API Patterns

- Always access systems through Brain accessor methods
- Pass references between systems when needed
- Use callbacks and observers for system communication
- Monitor system state changes

## Troubleshooting

### Common Integration Issues

#### 1. Systems Not Connected

**Problem**: Memory systems not accessible through Brain accessor methods

**Solution**:
```python
# Check if brain was properly initialized
if brain is None or not brain.initialize():
    print("Failed to initialize brain")
    return

# Access systems through proper API
working_mem = brain.getWorkingMemory()
if working_mem is None:
    print("Working memory not initialized")
    # Check initialization code
```

#### 2. Systems Not Updating

**Problem**: Systems not updating in brain loop

**Solution**:
```python
# Check brain step execution
for step in range(1000):
    brain.step(step, step * 0.001)
    \n    # Check system states
    if concept_form.getConceptCount() > 0:
        print(f"Step {step}: Concepts formed = {concept_form.getConceptCount()}")
```

#### 3. Memory Systems Not Working

**Problem**: Memory systems not storing or retrieving data

**Solution**:
```python
# Check memory system initialization
working_mem = brain.getWorkingMemory()
if working_mem:
    # Check working memory capacity
    working_mem.setCapacity(100)
    print(f"Working memory capacity set to {working_mem.getCapacity()}")
    \n    # Test memory operations
    working_mem.storeToNeuron(123, 0.5)
    trace = working_mem.getTrace(123)
    if trace:
        print("Successfully stored and retrieved working memory trace")
```

### Integration Validation

Use the Phase 6 integration experiment to validate your setup:

```python
# Create and run integration experiment
experiment = Phase6IntegrationExperiment()
config = Phase6Config()
\n# Configure for your use case
config.neuronCount = 500
config.maxSteps = 2000
\n# Run experiment and get results
results = experiment.run(config)
\n# Check integration status
print(f"Memory working memory integrated: {results.memoryWorkingMemoryIntegrated}")
print(f"Prediction integrated: {results.predictionIntegrated}")
print(f"Development integrated: {results.developmentIntegrated}")
\n# If any integration is false, investigate the specific system
```

## Summary

Phase 6 integration creates a fully coherent artificial brain where all systems work together. Key aspects include:

1. **Centralized Access**: All systems accessed through Brain class
2. **Integrated Brain Loop**: 15-step integration with mutual system influence
3. **System Dependencies**: Each system affects and is affected by others
4. **Development Integration**: Age-based modulation of all system activities
5. **Persistence**: Full save/load capabilities for simulation state
6. **Replay and Consolidation**: Memory reinforcement and stabilization
7. **Comprehensive Testing**: Experiment framework for validation

The Phase 6 integration represents a major advance in creating artificial brains that learn, develop, and adapt through experience, with all systems working together in a coherent and biologically inspired manner.

<document>
<document>
<document>