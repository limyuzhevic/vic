# AgentBrain Refactoring Plan

## Overview
AgentBrain.cpp is currently 350 lines and violates the Single Responsibility Principle. This file handles multiple concerns:
1. Neuron group initialization and distribution
2. Sensory input processing and integration
3. Motor command decoding and selection
4. Reward modulation and learning
5. Developmental state management
6. Neuromodulation level tracking
7. State reset and cleanup

## Refactoring Strategy

### 1. SensoryProcessor Class
**Responsibility:** Handle all sensory input processing
**Methods:**
- `void processVisionInput()` - Process visual sensory data
- `void processTouchInput()` - Process touch/collision data
- `void processInternalInput()` - Process internal physiological signals
- `void processProprioceptionInput()` - Process body position signals
- `void computeNovelty()` - Calculate novelty detection
- `void updateCuriosity()` - Update curiosity levels

### 2. MotorDecoder Class
**Responsibility:** Handle motor command decoding and selection
**Methods:**
- `void calculateMotorActivity()` - Compute activity levels in motor groups
- `void selectAction()` - Select optimal motor command
- `void applyCuriosity()` - Add curiosity-based exploration

### 3. RewardProcessor Class
**Responsibility:** Handle reward-based learning and modulation
**Methods:**
- `void computePredictionError()` - Calculate prediction error
- `void updateDopamineSignal()` - Apply dopamine modulation
- `void modulatePlasticity()` - Adjust plasticity based on reward

### 4. DevelopmentManager Class
**Responsibility:** Handle developmental state and progression
**Methods:**
- `void updateDevelopmentalAge()` - Advance developmental timeline
- `void updatePlasticityModifier()` - Adjust plasticity with development
- `void manageStructuralPlasticity()` - Control structural changes
- `void updateDevelopmentalStage()` - Set brain developmental stage

### 5. NeuromodulationManager Class
**Responsibility:** Handle neuromodulator levels and states
**Methods:**
- `void updateDopamineLevel()` - Manage dopamine signals
- `void updateCuriosityLevel()` - Manage curiosity levels
- `void updateNoveltyLevel()` - Manage novelty detection
- `void resetAllNeuromodulators()` - Reset neuromodulator states

### 6. AgentStateManager Class
**Responsibility:** Handle agent state initialization and reset
**Methods:**
- `void initializeNeurons()` - Set up neuron groups
- `void initializeSensoryGroups()` - Configure sensory input groups
- `void initializeMotorGroups()` - Configure motor output groups
- `void reset()` - Reset all agent state

## Implementation Plan

### Phase 1: Header Refactoring
1. Create new header files for each class
2. Define interfaces and method signatures
3. Maintain backward compatibility through AgentBrain class

### Phase 2: Implementation
1. Implement SensoryProcessor with all sensory processing methods
2. Implement MotorDecoder with action selection logic
3. Implement RewardProcessor with reward-based learning
4. Implement DevelopmentManager with developmental progression
5. Implement NeuromodulationManager with neuromodulator states
6. Implement AgentStateManager with initialization logic

### Phase 3: Integration
1. Update AgentBrain class to use the new classes
2. Ensure all existing functionality is preserved
3. Run comprehensive tests

## Benefits
1. **Improved Maintainability:** Each class has a single, clear responsibility
2. **Better Testability:** Individual components can be tested in isolation
3. **Enhanced Readability:** Code is easier to understand and navigate
4. **Easier Extension:** New features can be added without modifying existing code
5. **Reduced Complexity:** AgentBrain.cpp will be significantly shorter and more manageable

## Testing Strategy
1. Unit tests for each new class
2. Integration tests for AgentBrain class
3. Performance regression testing
4. Memory leak detection
5. Functional equivalence testing

## Files to Modify
1. src/agent/AgentBrain.hpp - Add includes for new classes
2. src/agent/AgentBrain.cpp - Refactor to use new classes
3. Create src/agent/SensoryProcessor.hpp/.cpp
4. Create src/agent/MotorDecoder.hpp/.cpp
5. Create src/agent/RewardProcessor.hpp/.cpp
6. Create src/agent/DevelopmentManager.hpp/.cpp
7. Create src/agent/NeuromodulationManager.hpp/.cpp
8. Create src/agent/AgentStateManager.hpp/.cpp

## Timeline
- **Week 1:** Design and initial implementation of SensoryProcessor
- **Week 2:** Implement MotorDecoder
- **Week 3:** Implement RewardProcessor
- **Week 4:** Implement DevelopmentManager and NeuromodulationManager
- **Week 5:** Implement AgentStateManager
- **Week 6:** Integrate with AgentBrain class and test
- **Week 7:** Final testing and optimization

## Technical Considerations
1. **Neuron Pointer Management:** Handle neuron pointer cleanup properly
2. **Neural Network Integration:** Ensure proper brain-neuron communication
3. **Performance:** Minimize overhead from additional abstractions
4. **Memory Management:** Proper cleanup of allocated neuron groups
5. **Thread Safety:** Consider thread safety for concurrent access

## Future Extensions
1. **Behavior Trees:** Add behavior tree support for complex action selection
2. **Emotion System:** Integrate emotion-based behavior modulation
3. **Motivation System:** Add higher-level motivational states
4. **Social Behavior:** Add multi-agent interaction capabilities
5. **Adaptive Architecture:** Self-modifying behavior based on experience