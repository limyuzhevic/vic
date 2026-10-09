# NLM Memory Systems Python Bindings

This document describes the Python bindings for the three neural memory systems in the NLM (Neural Learning Machine) framework.

## Overview

The NLM framework implements three advanced memory systems with neural substrate:

1. **NeuralWorkingMemory** - Persistent activity patterns for short-term storage
2. **NeuralEpisodicMemory** - Experience storage with temporal ordering
3. **NeuralAssociativeMemory** - Pattern associations and spreading activation

## Installation

These bindings are built using pybind11 and CMake. The built module `pynlm` will be installed as a Python extension module.

## Key Design Principles

### Brain Parameter Ownership
- Brain* parameters transfer ownership to the memory systems
- Memory systems assume responsibility for the Brain instance lifecycle
- Do not pass the same Brain instance to multiple memory systems

### Python API Design
- `std::vector<float>` patterns automatically converted to Python lists
- Return value policies chosen for safety and performance
- Clear separation between storage and retrieval operations
- Memory management handled by pybind11 reference counting

### Error Handling
- Null pointer checks with clear error messages
- Exception propagation to Python with meaningful diagnostics
- Graceful handling of invalid inputs

## Module Structure

### Main Classes

#### `nlm.Brain`
Central neural simulation brain with integrated memory systems.

- `getWorkingMemory()`: Get working memory reference
- `getEpisodicMemory()`: Get episodic memory reference  
- `getAssociativeMemory()`: Get associative memory reference

#### `nlm.NeuralWorkingMemory`
Persistent activity-based working memory system.

Key methods:
- `initialize(brain)`: Initialize with brain reference (ownership transfer)
- `store(pattern, strength)`: Store neural activity pattern
- `retrieve()`: Get current memory content as activity levels
- `runCompetition()`: Neural competition for selective retention
- `strengthenMemory(factor)`: Rehearse and strengthen traces

#### `nlm.NeuralEpisodicMemory`
Experience storage with neural pattern encoding.

Key methods:
- `initialize(brain)`: Initialize with brain reference (ownership transfer)
- `storeEpisode(episode)`: Store new experience
- `retrieveSimilar(pattern)`: Pattern completion retrieval
- `retrieveTemporal(start, end)`: Time-windowed retrieval
- `retrieveByLocation(x, y, radius)`: Spatial retrieval
- `replayEpisode(episode)`: Neural pattern reactivation

#### `nlm.NeuralAssociativeMemory`
Pattern association and spreading activation system.

Key methods:
- `initialize(brain)`: Initialize with brain reference (ownership transfer)
- `associate(patternA, patternB, strength)`: Create association
- `retrieve(queryPattern)`: Get associated patterns
- `spreadActivation(cuePattern)`: Propagate activation through chains
- `updateAssociation(patternA, patternB, delta)`: Adjust based on outcomes

### Supporting Classes

#### `nlm.EpisodicMemoryItem`
Represents a stored neural experience with sensory, action, and reward data.

#### Enums
- `NeuronType`: Excitatory, Inhibitory, Sensory, Motor, etc.
- `ActionType`: Movement and interaction actions
- `DevelopmentalStage`: Maturation phases
- `FiringState`: Neuron activity states

## Usage Examples

### Basic Usage

```python
import pynlm as nlm

# Create configuration and brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
```

### Neural Working Memory

```python
# Store and retrieve patterns
working_memory.store([0.1, 0.3, 0.7, 0.9], strength=1.0)
pattern = working_memory.retrieve()
print(f"Memory contains {len(pattern)} activation values")

# Neuron-specific operations
working_memory.storeToNeuron(neuron_id, 0.8)
activation = working_memory.getNeuronActivation(neuron_id)

# Competition and attention
working_memory.runCompetition()
winners = working_memory.getMemoryNeurons()

# Maintenance dynamics
working_memory.update(0.01)  # 10ms timestep
```

### Neural Episodic Memory

```python
# Create episode
episode = nlm.EpisodicMemoryItem()
episode.sensoryState = [0.5, 0.2, 0.8]
episode.positionX = 10.0
episode.positionY = 5.0
episode.action = nlm.ActionType.MoveForward
episode.reward = 1.0
episode.activeNeurons = [1001, 1002, 1003]

# Store and retrieve
episodic_memory.storeEpisode(episode)
similar = episodic_memory.retrieveSimilar([0.4, 0.2, 0.7])

# Different retrieval modes
temporal = episodic_memory.retrieveTemporal(50, 150)
location = episodic_memory.retrieveByLocation(10.0, 5.0, 2.0)
action = episodic_memory.retrieveAfterAction(nlm.ActionType.MoveForward)

# Replay for consolidation
episodic_memory.replayEpisode(similar[0] if similar else None)
```

### Neural Associative Memory

```python
# Create associations
pattern_a = [0.1, 0.2, 0.3]
pattern_b = [0.8, 0.9, 0.7]
pattern_c = [0.4, 0.5, 0.6]

associative_memory.associate(pattern_a, pattern_b, strength=0.9)
associative_memory.associate(pattern_b, pattern_c, strength=0.7)

# Retrieve and spread
results = associative_memory.retrieve(pattern_a)
activated = associative_memory.spreadActivation(pattern_a)

# Update based on outcomes
associative_memory.updateAssociation(pattern_a, pattern_b, 0.1)
```

### Complete Example

See `example_memory_bindings.py` for a complete working example demonstrating all memory systems.

## Error Handling

### Common Exceptions

- `RuntimeError`: Operations on null pointers or invalid states
- `ValueError`: Invalid parameter values
- `IndexError`: Out-of-bounds access

### Defensive Programming

```python
if brain is None:
    raise ValueError("Brain cannot be None")

if pattern and len(pattern) == 0:
    raise ValueError("Pattern cannot be empty")

# Graceful handling with None checks
if working_memory:
    # Safe to use
else:
    # Memory not initialized or unavailable
    pass
```

## Performance Considerations

### Memory Management
- Memory systems assume Brain ownership after initialization
- Avoid multiple initializations with the same Brain instance
- Clear memory when no longer needed with `clear()`

### Tensor Operations
- Patterns are copied during storage for safety
- Use mutable lists for patterns that will be modified
- Consider chunking large patterns for memory efficiency

### Competition and Updates
- `runCompetition()` and `update()` have O(n²) complexity
- Use larger timesteps for simulation efficiency
- Balance retrieval frequency with computational cost

## Integration with NLM Brain

### Lifecycle Management

```python
# Initialize memory systems with brain
brain = nlm.createBrain(config)
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain)  # Transfers ownership

# Memory automatically managed during brain lifecycle
brain.step(simulation_step)  # Updates all systems
```

### State Sharing

Memory systems share neural substrate through Brain:
- Working memory maintains neuron activation
- Episodic memory creates episode-specific neurons  
- Associative memory finds pattern neurons for association

## Testing and Verification

### Unit Tests
- Individual memory system methods
- Pattern storage and retrieval accuracy
- Competition dynamics
- Episode lifecycle

### Integration Tests
- Brain-memory system interactions
- Complete simulation steps with memory
- Memory system coordination

### Performance Tests
- Memory capacity limits
- Update frequency scaling
- Competition complexity

## Known Limitations

1. **Working Memory**: Limited by brain neuron count and capacity constraints
2. **Episodic Memory**: Uses simplified neuron encoding, not fully distributed
3. **Associative Memory**: Pattern matching based on similarity thresholds
4. **Initialization**: Memory systems require explicit Brain initialization

## Future Enhancements

1. **Distributed Representations**: Move beyond 1:1 neuron-pattern mapping
2. ** Hebbian Plasticity**: Dynamic association strengthening
3. **Consolidation Algorithms**: Sleep-like memory consolidation
4. **Pruning Mechanisms**: Remove irrelevant associations
5. **Memory Metrics**: Quantify memory quality and efficiency

## Contributing

### Adding New Methods
1. Update C++ headers with new functionality
2. Implement in corresponding .cpp files
3. Add pybind11 bindings with appropriate Python interfaces
4. Update this documentation with examples

### Fixing Issues
1. Report with specific error messages and usage context
2. Include stack traces and parameter values
3. Provide minimal reproduction examples
4. Test with different scenarios

### Performance Improvements
1. Profile memory operations
2. Optimize pattern matching algorithms
3. Reduce unnecessary copying
4. Implement lazy evaluation where appropriate

## License

This project is part of the NLM framework. See the main LICENSE file for details.

## Contact

For questions, issues, or contributions:
- GitHub repository: Check the main project repository
- Documentation: Update this file with new examples
- Issues: File bug reports with reproduction steps

---

*This documentation covers Phase 1 implementation of memory systems Python bindings.*
