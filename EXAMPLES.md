# NLM Python Bindings - Improved API Documentation

## Overview

This document describes the significant improvements made to the Python bindings for the NLM (Neural Learning Machine) framework. The Python bindings (`pynlm`) have been enhanced with an intuitive, Pythonic API that makes it easy to work with complex neural systems while maintaining access to advanced functionality for researchers and developers.

## Key Improvements

### 1. Expanded System Coverage

**Previously:** Python bindings only exposed basic core functionality (Brain, Config, AgentBrain, SimpleWorld)

**Now:** Complete coverage of all NLM systems including:

- **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
- **Prediction Systems**: PredictionSystem
- **Neuromodulation Systems**: Dopamine, Curiosity, Novelty, PredictionError
- **Development Systems**: DevelopmentSystem
- **Plasticity Systems**: STDP, Hebbian, StructuralPlasticity
- **Neural Dynamics**: SpikeSystem
- **Motor Systems**: MotorSystem

### 2. Pythonic API Design

The improved bindings follow Python best practices:

- **Descriptive method names**: `get_stage_name()` instead of cryptic C++ names
- **Intuitive parameters**: Clear, named parameters instead of positional ones
- **Helper methods**: High-level methods that abstract complex operations
- **Statistics methods**: Methods that provide useful summaries and statistics
- **Type safety**: Better type checking and error handling

### 3. Convenience Methods

Added high-level methods that simplify common operations:

#### Action System
- `is_valid()`: Check if an action is valid
- `get_parameter(index)`: Safely get action parameters
- `set_parameter(index, value)`: Safely set action parameters

#### AgentBrain
- `get_stage_name()`: Get descriptive developmental stage name
- `get_neuromodulation_summary()`: Get all neuromodulation levels in one call
- Enhanced `__repr__()` methods for better debugging

#### Memory Systems
- `createWorkingMemory()`, `createEpisodicMemory()`, `createAssociativeMemory()`: Factory methods
- `createMemorySystem()`: Create all memory systems at once
- `createDefaultMemoryConfig()`: Create reasonable default memory configuration
- `query_memory_by_location()`: Find episodes by spatial position
- `query_memory_by_action()`: Find episodes after specific actions
- `replay_memory_sequence()`: Replay stored episode sequences
- `store_association()`: Create pattern associations
- `spread_activation()`: Propagate activation through associations
- `get_memory_system_stats()`: Get statistics from all memory systems
- `createMemoryReport()`: Generate human-readable memory reports

#### Prediction System
- `get_recent_errors(count)`: Get recent prediction errors
- `get_avg_error()`: Get average prediction error
- `is_learning()`: Check if prediction system is learning

#### Neuromodulation Systems
- Enhanced type hierarchy with `Neuromodulator` base class
- `get_summary()` methods for system state overview

### 4. Better Error Handling

- More descriptive error messages
- Graceful handling of edge cases
- Methods that don't throw exceptions for expected conditions

### 5. Enhanced Documentation

- Comprehensive docstrings for all classes and methods
- Clear parameter descriptions
- Return value documentation
- Examples for complex operations

## System Details

### Memory Systems

#### Working Memory
- **Purpose**: Temporary storage of active information with neural dynamics
- **Key features**: Persistent activity, recurrent connections, competition, decay
- **Python API**:
  - `store(pattern, strength)`: Store neural activation pattern
  - `retrieve()`: Get current working memory content
  - `getNeuronActivation(neuron)`: Get activation of specific neuron
  - `getMemoryActivity()`: Get overall memory activity level

#### Episodic Memory
- **Purpose**: Store experiences as neural patterns for pattern completion
- **Key features**: Temporal ordering, spatial retrieval, relevance weighting, replay
- **Python API**:
  - `storeEpisode(episode)`: Store a new experience
  - `retrieveSimilar(pattern, max_results)`: Find similar episodes
  - `retrieveByLocation(x, y, radius, max_results)`: Find episodes by position
  - `retrieveAfterAction(action, max_results)`: Find episodes after specific actions
  - `replayEpisode(episode)`: Reactivate neural patterns
  - `getEpisodesForReplay(count)`: Get episodes suitable for replay

#### Associative Memory
- **Purpose**: Store relationships between neural patterns via synaptic connections
- **Key features**: Hebbian co-activation, pattern completion, spreading activation
- **Python API**:
  - `associate(patternA, patternB, strength)`: Create association
  - `retrieve(query_pattern, max_results)`: Find related patterns
  - `getAssociationStrength(patternA, patternB)`: Get association strength
  - `spreadActivation(cue_pattern, steps)`: Propagate activation through chains

### Prediction System

#### Key Features
- Predicts future sensory states using predictive coding
- Tracks prediction errors for learning
- Maintains confidence estimates
- Supports model training

#### Python API
- `predictNextState(current_state)`: Generate prediction
- `updatePredictions(predicted, actual)`: Update based on actual observations
- `getPredictionError()`: Get current prediction error
- `getConfidence()`: Get prediction confidence
- `getErrorHistory()`: Get historical errors
- `train(observation)`: Train prediction model

### Neuromodulation Systems

#### Dopamine
- **Role**: Reward signal and reinforcement learning
- **Functions**: Reward signaling, prediction error processing

#### Curiosity
- **Role**: Exploration motivation based on novelty and prediction error
- **Functions**: Novelty detection, exploration drive modulation

#### Novelty
- **Role**: Novelty detection for change detection
- **Functions**: Pattern comparison, novelty quantification

#### PredictionError
- **Role**: Prediction error signal for learning
- **Functions**: Error tracking, update mechanisms

### Development System

#### Key Features
- Implements developmental stages with age-based transitions
- Modulates plasticity rates according to stage
- Supports critical periods and maturation

#### Python API
- `getStage()`: Get current developmental stage
- `getStageName()`: Get descriptive stage name
- `getDevelopmentalAge()`: Get current developmental age
- `isCriticalPeriod()`: Check if in critical period
- `getCriticalPeriodProgress()`: Get critical period completion
- `getPlasticityModifier()`: Get current plasticity rate
- `advanceStage()`: Move to next developmental stage

### Plasticity Systems

#### STDP (Spike-Timing-Dependent Plasticity)
- **Role**: Hebbian learning based on spike timing
- **Parameters**: LTP weight, LTD weight, time constant

#### Hebbian
- **Role**: Standard Hebbian learning
- **Parameters**: Learning rate, maximum weight

#### Structural Plasticity
- **Role**: Creation and removal of synapses and neurons
- **Parameters**: Synaptogenesis rate, pruning rate

## Usage Examples

See `examples/nlm_examples.py` for comprehensive examples of using all the improved Python bindings.

## Integration with C++ Core

The Python bindings maintain full access to the underlying C++ neural computation engine:

- All systems are integrated into the brain loop
- Real spiking neural dynamics are preserved
- Performance optimizations are maintained
- Memory pools and event queues are still used
- SIMD vectorization and parallel processing continue to work

## Benefits for Different User Groups

### Researchers
- **Access to complex systems**: Full exposure of memory, prediction, neuromodulation systems
- **Experimental control**: Fine-grained control over system parameters
- **Data analysis**: Statistics and reporting methods for system analysis

### Developers
- **Integration ease**: Pythonic API that maps intuitively to C++ concepts
- **Extensibility**: Factory methods and builder patterns
- **Error handling**: Graceful error reporting and recovery

### Students and Educators
- **Learning curves**: Intuitive method names and clear documentation
- **Examples**: Comprehensive example code
- **Abstraction levels**: High-level convenience methods for common operations

### Advanced Users
- **Low-level access**: Direct access to all underlying systems and parameters
- **Custom integration**: Ability to create custom memory, prediction, or neuromodulation strategies
- **Performance**: Full access to high-performance neural computation

## Migration Guide

### If you were using the old API:

**Before:**
```python
# Create brain
brain = pynlm.createBrain(config)

# Access systems (if exposed)
working_memory = brain.getWorkingMemory()  # May not exist
prediction_system = brain.getPredictionSystem()  # May not exist
```

**After:**
```python
# Create brain
brain = pynlm.createBrain(config)

# Access systems (now available)
memory_systems = pynlm.createMemorySystem(brain)
working_memory = memory_systems["working_memory"]
prediction_system = brain.getPredictionSystem()  # Now available
```

### New recommended patterns:

```python
# Create all systems at once
brain = pynlm.createBrain(config)
brain.initialize()
agent_brain = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
memory_systems = pynlm.createMemorySystem(brain)
neuromodulation = pynlm.createNeuromodulationSystem()

# Work with systems using intuitive methods
working_memory.store(pattern)
agent_brain.processSensoryInput(percept)
action = agent_brain.decodeMotorCommand()

# Get statistics
stats = pynlm.get_memory_system_stats(working_memory, episodic_memory, associative_memory)
```

## Performance Considerations

- The Python bindings maintain the same high-performance characteristics as the C++ core
- Memory systems use efficient data structures and neural dynamics
- Factory methods are lightweight and avoid unnecessary object creation
- Statistics methods provide both detailed and summarized information

## Error Handling

The Python bindings provide:

- **Descriptive exceptions**: Clear error messages for debugging
- **Graceful degradation**: Methods fail gracefully when dependencies are missing
- **Type safety**: Proper type checking and conversion
- **Validation**: Input validation with helpful error messages

## Summary

The improved Python bindings transform the NLM framework from a collection of low-level C++ APIs into an accessible, Pythonic interface while maintaining full access to the underlying neural computation power. The improvements include:

- Comprehensive system coverage
- Intuitive Pythonic APIs
- Extensive convenience methods
- Better error handling
- Comprehensive documentation
- Example code
- Statistics and reporting

This makes NLM more accessible to a wider range of users while maintaining the computational power and flexibility needed for advanced research and development.
