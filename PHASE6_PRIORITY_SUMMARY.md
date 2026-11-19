# Phase 6 Integration Priority Summary

## TL;DR: The Critical Problem

**The NLM project has sophisticated architecture but disconnected systems.** Phase 6 must focus on **integration over new features**. Currently, the project scores **43/120 (35.8%)** because systems exist but don't work together.

## Critical Integration Gaps (High Priority)

### 1. Memory Systems - COMPLETELY DISCONNECTED
- **Working Memory**: `NeuralWorkingMemory` has sophisticated mechanisms but `Brain::getWorkingMemory()` returns nullptr
- **Episodic Memory**: Implemented but never updated with experiences
- **Integration**: No connection between sensory input → working memory → episodic storage

**Action Items:**
1. Update `Brain::step()` to call working memory
2. Add episodic memory storage every 10 steps
3. Implement replay every 100 steps
4. Add consolidation every 1000 steps

### 2. Prediction System - DISCONNECTED
- **Implementation**: Full prediction, error, confidence, history
- **Reality**: Never trained or used in brain loop
- **Impact**: No prediction-based learning or adaptation

**Action Items:**
1. Connect prediction system to sensory input
2. Add prediction error training in brain loop
3. Implement prediction-based attention modulation

### 3. Cognitive Systems - ALL DISCONNECTED
- **NeuralPlanner**, **ConceptFormation**, **SelfModel**, **SocialLearning**, **SpatialRepresentation**: ALL fully implemented but never used
- **AgentBrain**: Has cognitive functions but doesn't integrate them

**Action Items:**
1. Connect NeuralPlanner to action selection in `AgentBrain::decodeMotorCommand()`
2. Add concept bias to motor decoding
3. Implement self-model agency prediction

### 4. Neuromodulation - PARTIALLY CONNECTED
- **Dopamine**: Modulates STDP only
- **ACh, NE, 5-HT**: Stub implementations
- **Missing**: Memory consolidation, attention, arousal connections

**Action Items:**
1. Implement full neuromodulator effects
2. Connect dopamine to memory consolidation
3. Implement ACh attention modulation

### 5. Persistence - NOT WORKING
- **CheckpointSystem**: Fully implemented
- **Brain::save/load**: Stubs that return false
- **Reality**: Cannot save or resume brain state

**Action Items:**
1. Implement `Brain::save()` using CheckpointSystem
2. Implement `Brain::load()` with full state restoration
3. Add automatic checkpoint management

## Current Implementation Status

### What's Working ✅:
- LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Basic sensory-motor loop (Phase 3)
- All cognitive system architectures
- All memory system architectures
- All neuromodulation systems

### What's BROKEN ❌:
- Memory integration in brain loop
- Prediction integration
- Cognitive system usage
- Full neuromodulation
- Checkpoint save/load
- Sleep/consolidation
- Reward learning

## Immediate Action Plan (Weeks 1-4)

### Week 1: Memory Integration
```cpp
// In Brain::step():
if (pImpl->workingMemory) {
    pImpl->workingMemory->update(pImpl->timestep);
}

// Add episodic storage every 10 steps
if (currentStep % 10 == 0 && pImpl->episodicMemory) {
    storeEpisodeToEpisodicMemory();
}
```

### Week 2: Prediction Integration
```cpp
// In Brain::step():
if (pImpl->predictionSystem) {
    pImpl->predictionSystem->updateWithSensory(sensoryInput);
}

// In AgentBrain::applyRewardModulation():
predictionError_ = reward - predictedReward;
if (pImpl->predictionError) {
    pImpl->predictionError->update(predictionError_);
}
```

### Week 3: Cognitive Integration
```cpp
// In AgentBrain::decodeMotorCommand():
if (brain_->getPlanner()) {
    ActionType planned = brain_->getPlanner()->planAction(lastVision_);
    // Add planner bias to motor selection
}

if (brain_->getConceptFormation()) {
    auto pattern = conceptPattern();
    size_t cid = brain_->getConceptFormation()->getMatchingConcept(pattern);
    // Add concept bias to motor selection
}
```

### Week 4: Neuromodulation & Persistence
```cpp
// Full neuromodulation implementation
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Current dopamine calculation
    predictionError_ = reward - predictedReward;
    dopamineLevel_ = predictionError_;
    
    // TODO: Add ACh, NE, 5-HT effects
    // TODO: Connect to memory consolidation
}

// Checkpoint implementation
bool Brain::save(const std::string& filepath) const {
    // Implement using CheckpointSystem
    CheckpointWriter writer;
    // Write neurons, synapses, spike history, memory states
    return writer.finalize();
}
```

## Files to Modify (Priority Order)

### Phase 1: Brain Integration (Brain.cpp)
1. **Lines 472-474**: Add working memory update
2. **Lines 583-613**: Add episodic memory storage
3. **Lines 615-619**: Add prediction system update
4. **Lines 622-630**: Add attention system update
5. **Lines 632-636**: Add concept formation update

### Phase 2: Agent Integration (AgentBrain.cpp)
1. **Lines 421-432**: Connect planner to action selection
2. **Lines 702-732**: Add concept bias
3. **Lines 742-768**: Add self-model bias
4. **Lines 769-785**: Add social bias

### Phase 3: Neuromodulation (Brain.cpp)
1. **Lines 487-507**: Add full neuromodulator effects
2. **Lines 982-990**: Connect neuromodulation to plasticity
3. **Lines 988-990**: Add neuromodulation to eligibility traces

### Phase 4: Persistence (Brain.cpp)
1. **Lines 931-1099**: Implement save() method
2. **Lines 1101-1375**: Implement load() method

## Testing Strategy

### Integration Tests (Week 5)
```cpp
// In Phase6IntegratedExperiment.hpp:
class IntegrationTest {
public:
    bool verifyIntegration();           // Check all systems connected
    bool testMemoryIntegration();       // Test memory systems
    bool testNeuromodulationIntegration(); // Test neuromodulation
    bool testPredictionIntegration();    // Test prediction systems
    bool testCheckpointing();           // Test persistence
    bool testReplay();                   // Test memory replay
};
```

### Benchmarks (Week 6)
- **Memory Retention**: Track working memory over 1000 steps
- **Learning Progress**: Measure prediction accuracy improvement
- **Integration Score**: Monitor system connections
- **Performance**: Measure computation speed

## Risk Mitigation

### High Risk: Integration Complexity
- **Mitigation**: Break into smaller, testable components
- **Approach**: Connect one system at a time
- **Testing**: Verify each integration before moving to next

### High Risk: Timeline Delays
- **Mitigation**: Use existing test framework
- **Approach**: Start with verification tests
- **Fallback**: Focus on highest-impact integrations

### High Risk: System Conflicts
- **Mitigation**: Use mock implementations for testing
- **Approach**: Test interfaces before implementation
- **Validation**: Automated integration testing

## Success Metrics

### After Week 4:
- **Memory Integration**: Working memory updated each step
- **Prediction Integration**: Prediction system connected to sensory input
- **Cognitive Integration**: Planner/concept/self-model biases active
- **Neuromodulation**: Full neuromodulator effects implemented
- **Persistence**: Checkpoint save/load working

### After Week 8:
- **Integration Score**: 80%+ from 35.8%
- **Memory Retention**: 30%+ over 1000 steps
- **Learning Progress**: Meaningful learning within 500 steps
- **Generalization**: Basic transfer to novel situations

## Quick Start Commands

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration test
./nlm_phase6_demo

# Run individual system tests
./nlm_test  # All unit tests
./nlm_phase6_test  # Integration tests
```

## Next Steps

1. **Immediately**: Modify `Brain::step()` to add memory updates
2. **This Week**: Connect prediction system to sensory input
3. **Week 2**: Add cognitive system biases in AgentBrain
4. **Week 3**: Implement full neuromodulation
5. **Week 4**: Add persistence and replay
6. **Week 5**: Run integration tests and fix issues

## Conclusion

Phase 6 is about making existing systems work together. The components exist; they just need integration. Focus on connecting systems rather than adding new ones.

**Success = Integration over Innovation**

This plan provides a clear, actionable path to transform NLM from disconnected systems into a truly integrated artificial brain.

