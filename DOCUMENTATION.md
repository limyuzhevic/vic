# NLM Documentation

This documentation covers the key improvements and new functionality in the NLM (Neural Learning Machine) framework.

## Overview

NLM is an experimental artificial developmental brain system that implements real neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

## Phase 2: Real Neural Computation (Enhanced)

### Command Line Interface

The enhanced main.cpp provides comprehensive command-line interface:

```bash
# Basic usage
./nlm

# Run specific modes
./nlm --mode demo
./nlm --mode phase2
./nlm --mode phase4
./nlm --mode phase6
./nlm --mode learning
./nlm --mode benchmark
./nlm --mode memory

# With configuration options
./nlm --neuron 1000 --steps 5000 --timestep 0.001 --config myconfig.cfg learning

# Help and version
./nlm --help
./nlm --version
```

### Configuration System

The configuration system now includes enhanced error handling and validation:

#### Key Configuration Parameters

- `random_seed`: Random number generator seed (default: 42)
- `simulation_timestep`: Time step for simulation (default: 0.001s)
- `neuron_count`: Number of neurons (default: 500)
- `region_count`: Number of brain regions (default: 1)
- `max_steps`: Maximum simulation steps (default: 1000)
- `connection_probability`: Probability of forming synapses (default: 0.15)

#### Configuration File Format

The configuration file uses a simple key=value format:

```ini
# Configuration file for NLM
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.2

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

## New Memory Systems

### NeuralAssociativeMemory

The `NeuralAssociativeMemory` class implements pattern associations through Hebbian learning:

#### Key Features

- **Bidirectional Associations**: Creates associations in both directions (A->B and B->A)
- **Strength-Based Learning**: Association strength ranges from 0.0 (no association) to 1.0 (strong association)
- **Pattern Similarity Matching**: Retrieves the most similar pattern using cosine similarity
- **Hebbian Learning Rate**: Configurable learning rate for association updates
- **Association Consolidation**: Strengthens strong associations and weakens weak ones

#### Usage Examples

**Python:**
```python
import pynlm as nlm

# Create brain and memory system
brain = nlm.createBrain()
assoc_memory = brain.getAssociativeMemory()

# Create associations between patterns
pattern1 = [0.1, 0.5, 0.9, 0.3, 0.7]
pattern2 = [0.2, 0.6, 0.8, 0.4, 0.6]
pattern3 = [0.3, 0.7, 0.7, 0.5, 0.5]

assoc_memory.createAssociation(pattern1, pattern2, strength=0.8)
assoc_memory.createAssociation(pattern2, pattern3, strength=0.6)
assoc_memory.createAssociation(pattern1, pattern3, strength=0.9)

# Retrieve associations
associated = assoc_memory.retrieveAssociation(pattern2, threshold=0.5)
print(f"Pattern associated with pattern2: {associated}")

# Check association strength
strength = assoc_memory.getAssociationStrength(pattern1, pattern3)
print(f"Association strength: {strength}")

# Get all patterns
patterns = assoc_memory.getAllPatterns()
print(f"Total patterns stored: {len(patterns)}")

# Consolidate associations
assoc_memory.consolidate(strengthenThreshold=0.7, weakenThreshold=0.2)
```

**C++:**
```cpp
#include "src/memory/NeuralAssociativeMemory.hpp"

// Create brain and memory system
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Initialize memory system
auto& assocMemory = brain->getAssociativeMemory();
assocMemory.initialize(brain.get());

// Create associations
std::vector<float> pattern1 = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
std::vector<float> pattern2 = {0.2f, 0.6f, 0.8f, 0.4f, 0.6f};
std::vector<float> pattern3 = {0.3f, 0.7f, 0.7f, 0.5f, 0.5f};

assocMemory.createAssociation(pattern1, pattern2, 0.8f);
assocMemory.createAssociation(pattern2, pattern3, 0.6f);
assocMemory.createAssociation(pattern1, pattern3, 0.9f);

// Retrieve associations
auto associated = assocMemory.retrieveAssociation(pattern2, 0.5f);
auto strength = assocMemory.getAssociationStrength(pattern1, pattern3);
```

### AttentionalSelection

The `AttentionalSelection` class implements competitive attentional focus:

#### Key Features

- **Competitive Dynamics**: Winner-take-all selection among competing neurons
- **Reward-Modulated Learning**: Strengthen attention for rewarded stimuli
- **Novelty and Urgency Detection**: Attends to novel and urgent information
- **Gain Modulation**: Controls the strength of neural processing
- **Prediction Error Integration**: Updates attention based on prediction errors

#### Usage Examples

**Python:**
```python
import pynlm as nlm

# Create agent and attentional system
brain = nlm.createBrain()
world = nlm.createSimpleWorld(100, 100, 10, 10)
agent_brain = nlm.createAgentBrain(brain, world)

# Get attentional system
attention = brain.getAttention()
attention.initialize(brain.get())

# Process sensory input
sensory_signals = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2, 0.8, 0.4, 0.6, 0.1]
available_neurons = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

selected_focus = attention.processSensoryInput(sensory_signals, available_neurons)
print(f"Selected focus: {selected_focus}")

# Update based on reward
attention.updateBasedOnReward(0.8, selected_focus)

# Get attentional gains
for neuron in selected_focus:
    gain = attention.getAttentionGain(neuron)
    print(f"Neuron {neuron} attention gain: {gain}")

# Check if something deserves attention
novelty = 0.9
urgency = 0.7
relevance = 0.5

deserves_attention = attention.deservesAttention(novelty, urgency, relevance)
print(f"Deserves attention: {deserves_attention}")
```

**C++:**
```cpp
#include "src/cognition/AttentionalSelection.hpp"

// Initialize attentional system
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
auto& attention = brain->getAttention();
attention.initialize(brain.get());

// Process sensory input
std::vector<float> sensorySignals = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
std::vector<NeuronId> availableNeurons = {0, 1, 2, 3, 4};

auto selectedFocus = attention.processSensoryInput(sensorySignals, availableNeurons);
attention.updateBasedOnReward(0.8f, selectedFocus);

// Get attentional statistics
float avgFocus = attention.getAverageAttentionalFocus();
size_t activeNeurons = attention.getActiveAttentionalNeurons();
```

## Enhanced Python Bindings

### New Utility Functions

The enhanced Python bindings provide additional utility functions:

#### Pattern Processing Utilities

```python
import pynlm as nlm

# Compute cosine similarity between patterns
similarity = nlm.patternSimilarity([0.1, 0.5, 0.9], [0.2, 0.6, 0.8])
print(f"Pattern similarity: {similarity}")

# Normalize pattern to unit length
normalized = nlm.normalizePattern([1.0, 2.0, 3.0])
print(f"Normalized pattern: {normalized}")

# Generate random pattern
random_pattern = nlm.generateRandomPattern(5, 0.0, 1.0)
print(f"Random pattern: {random_pattern}")
```

#### Factory Functions

```python
# Create default brain with optimized settings
brain = nlm.createDefaultBrain()

# Create vision sensory input
vision = nlm.createVision(10, 10, 3)

# Create action
action = nlm.createAction(nlm.ActionType.MoveForward, [1.0, 0.5])

# Create world
world = nlm.createSimpleWorld(100, 100, 10, 10)
```

#### Error Handling and Validation

The enhanced bindings include comprehensive error handling:

```python
import pynlm as nlm

# Get system information
info = nlm.getSystemInfo()
print(f"NLM Version: {info['version']}")
print(f"Features: {info['features']}")

# Set error handling behavior
nlm.setErrorHandling(throwOnError=True, verboseErrors=True)
```

## Memory Integration

### Working Memory Example

```python
import pynlm as nlm

# Create brain and working memory
brain = nlm.createBrain()
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain.get())

# Store patterns
pattern1 = [0.1, 0.5, 0.9, 0.3, 0.7]
pattern2 = [0.2, 0.6, 0.8, 0.4, 0.6]

working_memory.store(pattern1, strength=0.8)
working_memory.store(pattern2, strength=0.6)

# Retrieve patterns
retrieved = working_memory.retrieve()
print(f"Retrieved pattern: {retrieved}")

# Check if neuron is in working memory
neuron_id = 0
contains = working_memory.contains(neuron_id)
print(f"Contains neuron {neuron_id}: {contains}")

# Get pattern size
pattern_size = working_memory.getPatternSize()
print(f"Pattern size: {pattern_size}")
```

### Episodic Memory Example

```python
import pynlm as nlm

# Create brain and episodic memory
brain = nlm.createBrain()
episodic_memory = brain.getEpisodicMemory()

# Store episode
episode = {
    "timestamp": 100,
    "reward": 0.8,
    "action": nlm.ActionType.MoveForward,
    "position": (10.0, 20.0, 0.0)
}

# Store episode (using C++ interface for complex episodes)
episodic_memory.storeEpisode(episode)

# Retrieve recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(5)
for i, episode in enumerate(recent_episodes):
    print(f"Episode {i}: timestamp={episode['timestamp']}, reward={episode['reward']}")
```

## Performance and Optimization

### Benchmarking

The framework includes performance benchmarking capabilities:

```bash
# Run learning experiments with specific parameters
./nlm --neuron 1000 --steps 5000 learning

# Run benchmarks
./nlm --mode benchmark

# Run memory tests
./nlm --mode memory
```

### Performance Monitoring

The enhanced Python bindings include performance monitoring utilities:

```python
import time
import pynlm as nlm

# Time neural network operations
start_time = time.time()
brain = nlm.createBrain()
world = nlm.createSimpleWorld(50, 50, 5, 5)
end_time = time.time()

print(f"Brain creation time: {end_time - start_time:.3f} seconds")
```

## Error Handling and Validation

### Comprehensive Error Handling

The enhanced framework includes comprehensive error handling:

1. **Input Validation**: Validates pattern sizes, parameter ranges, and input formats
2. **Graceful Degradation**: Continues execution even when some components fail
3. **Detailed Error Messages**: Provides clear error messages with context
4. **Exception Safety**: Proper cleanup and resource management

### Error Categories

- **InvalidArgumentError**: Invalid function arguments or parameters
- **OutOfRangeError**: Index out of bounds or invalid ranges
- **RuntimeError**: General runtime errors
- **ConfigurationError**: Configuration file parsing errors
- **MemoryError**: Memory allocation or access errors

## Testing and Validation

### Unit Tests

The framework includes comprehensive unit tests:

```bash
# Run all tests
./test_main

# Run specific test categories
./test_main --types
./test_main --config
./test_main --neuron
./test_main --synapse
./test_main --stdp
./test_main --brain
```

### Integration Tests

Integration tests verify system connectivity:

```bash
# Run Phase 2 integration demo
./nlm --mode phase2

# Run Phase 4 cognitive experiments
./nlm --mode phase4

# Run Phase 6 integration demo
./nlm --mode phase6
```

## Migration Guide

### From Previous Versions

#### Key Changes

1. **Enhanced Configuration System**: Better error handling and validation
2. **New Memory Systems**: NeuralAssociativeMemory and enhanced AttentionalSelection
3. **Improved Python Bindings**: More comprehensive and feature-rich
4. **Better Error Handling**: Comprehensive error checking and graceful degradation
5. **Enhanced Command Line Interface**: More options and better help documentation

#### Breaking Changes

- Some internal class interfaces have been enhanced
- New error handling may raise exceptions in previously undefined cases
- Some TODO comments have been resolved with implementations

#### Migration Steps

1. **Update Configuration Files**: Use new key=value format
2. **Update Code**: Adapt to new class interfaces and methods
3. **Update Tests**: Update test files to new API
4. **Update Documentation**: Refer to this new documentation

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration File Not Found

**Solution**: Ensure configuration file exists or use default configuration.

```bash
# Use default configuration
./nlm

# Specify configuration file
./nlm --config ./myconfig.cfg
```

#### Issue: Invalid Pattern Size

**Solution**: Ensure patterns have the correct dimensions for the neural system.

```python
# Check pattern size before processing
pattern = [0.1, 0.5, 0.9]
expected_size = 5
if len(pattern) != expected_size:
    print(f"Error: Pattern size {len(pattern)} != expected {expected_size}")
    pattern = pattern + [0.0] * (expected_size - len(pattern))
```

#### Issue: Memory Usage

**Solution**: Monitor memory usage and adjust configuration accordingly.

```python
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

## Future Improvements

### Planned Enhancements

1. **Real Neural Dynamics**: Implement Hodgkin-Huxley neuron models
2. **Advanced Learning**: Implement reward prediction error and dopamine systems
3. **Enhanced Memory**: Implement more sophisticated memory consolidation
4. **Visualization**: Add real-time visualization capabilities
5. **Distributed Computing**: Enable distributed neural simulation
6. **Hardware Acceleration**: GPU support for large-scale simulations

### Research Directions

- **Neurodevelopmental Learning**: Study how developmental processes affect learning
- ** Neuromodulation**: Research the role of neuromodulators in learning
- **Predictive Coding**: Implement predictive coding architectures
- **Memory Replay**: Study memory consolidation during sleep

## Conclusion

The NLM framework has been significantly enhanced with:

1. **New Memory Systems**: NeuralAssociativeMemory and AttentionalSelection
2. **Improved Error Handling**: Comprehensive validation and graceful degradation
3. **Enhanced Python Bindings**: More features and better error handling
4. **Better Configuration System**: Enhanced error handling and validation
5. **Improved Command Line Interface**: More options and better documentation
6. **Comprehensive Testing**: Better test coverage and validation

These improvements make NLM more robust, easier to use, and more capable of supporting advanced neural computation research.
