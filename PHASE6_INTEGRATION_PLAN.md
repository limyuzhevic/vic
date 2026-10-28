# NLM Phase 6 Integration Improvement Plan

## Summary
Based on the Phase 6 Final Audit, NLM has a sophisticated architecture (43/120 points = 35.8% complete) with most systems disconnected from the main brain loop. The primary goal is to **connect existing systems rather than add new features**.

## Current Status

### ✅ Working Systems (18/120 points)
- LIF neuron dynamics
- Synaptic transmission with event-driven spikes
- STDP and Hebbian plasticity
- Structural plasticity
- Embodied agent with SimpleWorld
- Neuromodulation (dopamine only, others stubs)

### ❌ Disconnected Systems (102/120 points)
- Memory systems (working, episodic, semantic, procedural)
- Prediction system
- Cognition systems (planner, concept formation, attention)
- Full neuromodulation (serotonin, norepinephrine, acetylcholine)
- Development system beyond structural plasticity
- Performance infrastructure integration
- Checkpoint persistence

## Integration Strategy

### Phase 1: Critical Connections (Priority 1)

**1.1 Memory Integration**
- Connect WorkingMemory to sensory processing in Brain::receiveSensoryInput()
- Connect EpisodicMemory to experience logging in Brain::step()
- Enable memory consolidation based on neuromodulation

**1.2 Prediction Integration**
- Connect PredictionSystem to sensory processing for predictive coding
- Use prediction error for attention modulation
- Integrate predictions into action planning

**1.3 Cognitive Integration**
- Connect NeuralPlanner to action selection
- Enable ConceptFormation from memory patterns
- Integrate AttentionalSelection for focus management

### Phase 2: Enhanced Functionality

**2.1 Neuromodulation Enhancement**
- Implement full dopamine effects (not just STDP scaling)
- Add serotonin for mood/learning modulation
- Implement acetylcholine for attention
- Add norepinephrine for arousal

**2.2 Development Integration**
- Make development affect neural excitability
- Integrate development with plasticity rules
- Connect development to attention and memory

**2.3 Performance Integration**
- Integrate memory pools into core allocation
- Use event queues for spike processing
- Enable sparse connectivity optimizations

### Phase 3: Advanced Features

**3.1 Checkpointing**
- Implement Brain::save() using CheckpointSystem
- Implement Brain::load() with state restoration

**3.2 Advanced User Features**
- Command-line interface for Phase 6 configuration
- Advanced debugging and visualization options
- Experiment configuration scripts

## Implementation Roadmap

### Week 1-2: Memory System Integration
- [ ] Integrate WorkingMemory into Brain::receiveSensoryInput()
- [ ] Connect EpisodicMemory to experience logging
- [ ] Implement memory consolidation with neuromodulation

### Week 3-4: Prediction and Cognition Integration
- [ ] Integrate PredictionSystem for predictive coding
- [ ] Connect NeuralPlanner to action selection
- [ ] Enable ConceptFormation from memory patterns

### Week 5-6: Enhanced Neuromodulation
- [ ] Implement full dopamine effects
- [ ] Add serotonin, acetylcholine, norepinephrine
- [ ] Connect neuromodulation to memory consolidation

### Week 7-8: Development and Performance
- [ ] Enhance development system integration
- [ ] Integrate performance infrastructure
- [ ] Implement checkpointing

## Technical Implementation Details

### Memory Integration Points

**WorkingMemory Integration:**
```cpp
void Brain::receiveSensoryInput(const SensoryInput& input) {
    // ... existing sensory neuron injection ...
    
    // NEW: Store to working memory
    if (pImpl->workingMemory && input.size() > 0) {
        for (size_t i = 0; i < numSensory && i < input.size(); ++i) {
            if (input[i] > 0.5f) {
                pImpl->workingMemory->storeToNeuron(
                    pImpl->sensoryNeurons[i]->getId(), 
                    input[i] / 10.0f
                );
            }
        }
    }
}
```

**EpisodicMemory Integration:**
```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... existing neural processing ...
    
    // NEW: Log experience for episodic memory
    if (pImpl->episodicMemory && currentStep % 100 == 0) {
        // Create experience snapshot
        Experience exp = createExperienceSnapshot(currentTime);
        pImpl->episodicMemory->storeExperience(exp);
    }
    
    // NEW: Replay for consolidation
    if (pImpl->episodicMemory && currentStep % pImpl->replayInterval == 0) {
        pImpl->episodicMemory->replayConsolidatedEpisodes();
    }
}
```

### Prediction Integration

```cpp
void Brain::receiveSensoryInput(const SensoryInput& input) {
    // ... existing processing ...
    
    // NEW: Predictive coding
    if (pImpl->predictionSystem) {
        auto prediction = pImpl->predictionSystem->predictNextState(input);
        auto error = pImpl->predictionSystem->computePredictionError(input, prediction);
        
        // Use prediction error to modulate attention
        if (pImpl->attention) {
            pImpl->attention->applyPredictionError(error);
        }
    }
}
```

### Cognitive Integration

```cpp
std::unique_ptr<Action> Brain::produceAction() {
    // ... existing motor selection ...
    
    // NEW: Neural planning
    if (pImpl->planner) {
        auto plan = pImpl->planner->generateActionPlan(
            pImpl->workingMemory.get(),
            pImpl->conceptFormation.get()
        );
        return plan->getSelectedAction();
    }
    
    return std::make_unique<Action>(ActionType::Wait);
}
```

## Verification

### Integration Tests
1. **Memory Integration Test**
   - Verify WorkingMemory stores sensory input
   - Verify EpisodicMemory stores experiences
   - Verify memory consolidation works

2. **Prediction Integration Test**
   - Verify prediction system is trained
   - Verify prediction error affects behavior
   - Verify confidence tracking works

3. **Cognition Integration Test**
   - Verify planner generates action sequences
   - Verify concept formation discovers patterns
   - Verify attention modulates processing

4. **Neuromodulation Integration Test**
   - Verify dopamine affects plasticity beyond scaling
   - Verify other neuromodulators work
   - Verify neuromodulation affects memory consolidation

### Performance Metrics
- **Before Integration**: 43/120 points (35.8%)
- **After Integration**: Target 80-100 points (67-83%)
- **Functionality**: Basic artificial brain with integrated memory, prediction, cognition, and neuromodulation

## Files to Modify

### Core Brain Integration (Brain.cpp)
- `Brain::receiveSensoryInput()` - Add memory integration
- `Brain::step()` - Add episodic memory and replay
- `Brain::produceAction()` - Add neural planning
- `Brain::applyNeuromodulation()` - Enhance effects

### Agent Integration (AgentBrain.cpp)
- `processSensoryInput()` - Connect to working memory
- `updateDevelopment()` - Enhance developmental effects

### System Components
- Memory implementations (NeuralWorkingMemory, NeuralEpisodicMemory)
- Prediction system implementation
- Cognitive system implementations
- Neuromodulator implementations (serotonin, acetylcholine, norepinephrine)

## Advanced Features for Experienced Users

### Command-Line Interface
```bash
# Phase 6 configuration
./nlm --phase6 \
  --neurons 1000 \
  --steps 50000 \
  --checkpoint \
  --replay \
  --development \
  --config phase6_advanced.cfg
```

### Configuration Files
- `configs/phase6_standard.cfg` - Standard settings
- `configs/phase6_advanced.cfg` - Advanced settings
- `configs/phase6_research.cfg` - Research settings

### Debugging Tools
- `--debug-mem` - Memory system debugging
- `--debug-pred` - Prediction system debugging
- `--debug-cog` - Cognitive system debugging
- `--debug-neuro` - Neuromodulation system debugging

## Timeline

- **Weeks 1-2**: Memory integration (Priority 1)
- **Weeks 3-4**: Prediction and cognition integration (Priority 1)
- **Weeks 5-6**: Enhanced neuromodulation (Priority 2)
- **Weeks 7-8**: Development, performance, checkpointing (Priority 2)
- **Weeks 9-10**: Advanced features and testing (Priority 3)

## Expected Outcome

After Phase 6 integration:
1. **Functional Artificial Brain**: Systems work together coherently
2. **Experience-Driven Learning**: Memories store and guide behavior
3. **Predictive Processing**: Brain anticipates and prepares for events
4. **Goal-Directed Behavior**: Plans emerge from concepts
5. **Adaptive Plasticity**: Neuromodulation dynamically controls learning
6. **Developmental Progression**: Brain matures naturally
7. **Persistent Learning**: Experiences consolidate and influence future behavior

The NLM brain will transition from a **neural simulator** to a **cognitive system** that learns, plans, and adapts through integrated experience processing.

---

**Primary Goal**: Make existing systems work together, not add more disconnected features.