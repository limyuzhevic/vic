# Enhanced NLM (Neural Learning Machine) - Python API Improvements

## Overview

This document describes the comprehensive improvements made to the NLM (熙然) neural simulation framework's Python bindings and user experience. The enhancements focus on making the powerful neural simulation capabilities more accessible to users while maintaining backward compatibility.

## Key Improvements

### 1. High-Level BrainSimulator API

#### What It Is
A simplified, intuitive interface that hides the complexity of the underlying C++ neural simulation framework while providing access to all advanced features.

#### Benefits
- **Easy to learn**: Designed for neuroscientists and researchers, not just developers
- **Less boilerplate**: No need to manually manage neurons, synapses, or plasticity
- **Intuitive patterns**: Natural language for simulation setup and execution
- **Automatic resource management**: Built-in context management for memory safety

#### Core Features

```python
import pynlm

# Create a brain with world and agent
brain = pynlm.BrainSimulator()
brain.setupWorld(width=20, height=20, visionWidth=8, visionHeight=8)
brain.enableLearning(True, True, True, True)

# Run simulation
brain.run(1000)

# Get statistics
stats = brain.getStats()
```

### 2. Configuration Presets

#### What It Is
Predefined configuration templates for common use cases, eliminating the need to understand all available configuration parameters.

#### Configuration Types

1. **Minimal Configuration**
   - Purpose: Basic testing and debugging
   - Neurons: 1000 (small for fast iteration)
   - Regions: 1
   - Connection probability: 0.1

2. **Custom Brain Configuration**
   - Purpose: Tailor simulations to specific research questions
   - Parameters: neuronCount, regionCount, connectionProb
   - Usage: `createBrainConfig(neuronCount=2000, regionCount=2, connectionProb=0.15)`

3. **Simulation Configuration**
   - Purpose: Full experiment setups with checkpointing
   - Features: Custom timestep, checkpoint directory, replay intervals

```python
# Different configuration presets
config1 = pynlm.createMinimalConfig()                    # Quick testing
config2 = pynlm.createBrainConfig(1000, 1, 0.1)         # Custom brain
config3 = pynlm.createSimulationConfig(5000, 0.001)     # Full simulation
```

### 3. Common Pattern Helpers

#### What It Is
Pre-built functions that encapsulate common simulation patterns, reducing code duplication and providing working examples.

#### Available Patterns

1. **Brain-Only Simulation**
   - Purpose: Study neural dynamics without environmental interaction
   - Use case: Testing plasticity rules, spike propagation
   - Implementation: `runBrainOnly(steps, neuronCount)`

2. **Agent-World Simulation**
   - Purpose: Study agent behavior in environment
   - Use case: Exploration, learning, decision making
   - Implementation: `runAgentWorld(steps, worldWidth, worldHeight, timestep)`

3. **Learning Experiment**
   - Purpose: Comprehensive learning studies
   - Features: Curiosity-driven exploration, reward-based learning
   - Implementation: `runLearningExperiment(steps, description)`

```python
# Common patterns
pynlm.runBrainOnly(500)                    # Just neurons
pynlm.runAgentWorld(1000)                  # Agent in world  
pynlm.runLearningExperiment(2000)          # Full learning study
```

### 4. Enhanced Command-Line Interface

#### What It Is
Professional-grade command-line tools for running simulations, managing checkpoints, and exploring configurations.

#### CLI Features

1. **Advanced Simulation Command**
   - Parameters: neurons, regions, connection probability
   - World support: configurable width, height, vision
   - Learning control: enable/disable specific subsystems
   - Output: statistics export, checkpoint saving

2. **Checkpoint Management**
   - Save brain states with metadata
   - Load checkpoints for continuation
   - List and inspect available checkpoints

3. **Experiment Runner**
   - Configuration management
   - Experiment tracking
   - Results organization

#### Usage Examples

```bash
# Run simple simulation
nlm-sim 1000

# Run with world and learning
nlm-sim 1000 --world --learn --world-width 30

# Run learning experiment
nlm-learn 500 --world-size 15 --enable-all-learning

# Manage checkpoints
nlm-checkpoint list
nlm-checkpoint save brain.bin
nlm-checkpoint load brain.bin
```

### 5. Improved Error Handling

#### What It Is
Better error messages and graceful handling of common mistakes, making debugging easier.

#### Error Handling Features

1. **Input Validation**
   - Parameter range checking
   - Type validation
   - Existence checking (files, directories)

2. **Helpful Error Messages**
   - Clear explanations of what went wrong
   - Suggestions for fixes
   - Context about expected values

3. **Graceful Degradation**
   - Fall back to safe defaults
   - Continue with reduced functionality
   - Provide alternatives when primary method fails

```python
# Error handling is transparent - no changes needed
try:
    brain = pynlm.BrainSimulator()
    brain.step(0)  # Clear error message about initialization
except Exception as e:
    print(f"Setup error: {e}")  # Clear, helpful message
```

### 6. Comprehensive Statistics Reporting

#### What It Is
Detailed statistics output that provides insights into simulation state and performance.

#### Statistics Categories

1. **Neural Activity**
   - Neuron counts (total, active, firing)
   - Spike statistics (total, recent)
   - Firing rates (average, variance)
   - Excitation/inhibition balance

2. **Learning Metrics**
   - Curiosity level (exploration drive)
   - Novelty detection (unexpected input)
   - Prediction error (difference between expected and actual)
   - Neuromodulation levels

3. **Agent Performance**
   - Position and movement statistics
   - Energy and health metrics
   - Action selection patterns
   - Environmental interaction

#### Usage

```python
brain = pynlm.BrainSimulator()
brain.run(500)

stats = brain.getStats()
print(f"Neurons: {stats['neurons_total']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Curiosity: {stats['curiosity_level']:.3f}")
print(f"Agent position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
```

### 7. Backward Compatibility

#### What It Is
All original NLM functionality preserved while adding new features, ensuring existing code continues to work.

#### Compatibility Features

1. **Original API Preserved**
   - All existing pybind11 bindings unchanged
   - Same class names and method signatures
   - Original C++ classes still accessible

2. **Seamless Migration**
   - New high-level API available alongside original
   - Gradual migration path
   - No breaking changes

3. **Advanced Access**
   - Direct access to low-level C++ API when needed
   - Original classes still available via `brain.getBrain()`

```python
# Original code still works (backward compatible)
import pynlm
brain = pynlm.BrainSimulator()  # Enhanced API
# Or use original classes
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

## Migration Guide

### For Existing Users

If you're already using the original NLM Python bindings, you can start using the enhanced features immediately:

```python
# OLD WAY (still works)
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# NEW WAY (easier, recommended)
import pynlm
brain = pynlm.BrainSimulator()
brain.setupWorld(width=20, height=20)
brain.enableLearning()
brain.run(1000)
```

### For New Users

If you're new to NLM, start with the high-level API:

```python
import pynlm

# Quick start - create and run a brain
brain = pynlm.BrainSimulator()
brain.setupWorld()
brain.enableLearning()
brain.run(1000)

# See results
stats = brain.getStats()
print(f"Simulation completed with {stats['spikes_total']} spikes")
```

## Quick Start Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and run a simple brain
brain = pynlm.BrainSimulator()
brain.run(500)

stats = brain.getStats()
print(f"Neurons: {stats['neurons_total']}")
print(f"Spikes: {stats['spikes_total']}")
```

### Example 2: Agent in World

```python
import pynlm

# Create agent with world
brain = pynlm.BrainSimulator()
brain.setupWorld(width=30, height=30, visionWidth=10, visionHeight=10)
brain.enableLearning(True, True, True, True)

# Run learning experiment
brain.run(1000)

# See curiosity-driven exploration
stats = brain.getStats()
print(f"Curiosity level: {stats['curiosity_level']:.3f}")
print(f"Agent position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
```

### Example 3: Learning Experiment

```python
import pynlm

# Run a complete learning experiment
pynlm.runLearningExperiment(
    steps=2000,
    description="Resource foraging behavior"
)
```

### Example 4: Command Line

```bash
# Run via CLI
nlm-sim 1000 --world --learn

# Run exploration demo  
nlm-explore --width 25 --height 25 --steps 500

# Run learning experiment
nlm-learn 1000 --world-size 20 --enable-all-learning
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuron_count | int | 1000 | Total number of neurons |
| region_count | int | 1 | Number of brain regions |
| connection_probability | float | 0.1 | Probability of synaptic connection |
| simulation_timestep | float | 0.001 | Simulation timestep in seconds |

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| width | float | 20.0 | World width |
| height | float | 20.0 | World height |
| visionWidth | int | 8 | Vision sensor width |
| visionHeight | int | 8 | Vision sensor height |
| agentX | float | 10.0 | Starting agent X position |
| agentY | float | 10.0 | Starting agent Y position |

### Learning Options

| Subsystem | Parameter | Default | Description |
|-----------|-----------|---------|-------------|
| Reward Modulation | rewardModulation | true | Learn from rewards |
| Structural Plasticity | structuralPlasticity | true | Grow/prune synapses |
| Development | development | true | Age-related changes |
| Curiosity | curiosity | true | Drive exploration |

## Performance Considerations

### High-Level API
- **Design Goal**: Ease of use
- **Performance**: Good for research and prototyping
- **Memory**: Automatically managed by Python garbage collection
- **Speed**: Slightly slower than direct C++ API due to binding overhead

### When to Use High-Level vs Low-Level API

#### Use High-Level API When:
- You want rapid prototyping
- You're new to NLM
- You need learning and exploration features
- You want built-in error handling
- You prefer Pythonic interfaces

#### Use Low-Level API When:
- You need maximum performance
- You're doing optimization research
- You have specific computational needs
- You're writing production systems
- You need fine-grained control

```python
# High-level (recommended for most users)
brain = pynlm.BrainSimulator()
brain.run(1000)

# Low-level (for advanced users)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
# ... C++ API usage
```

## Troubleshooting

### Common Issues and Solutions

1. **"Brain not initialized"**
   ```python
   # Problem: Calling step() without initializing
   brain = pynlm.BrainSimulator()
   brain.step(0)  # Error!
   
   # Solution:
   brain = pynlm.BrainSimulator()
   brain.setupWorld()  # This initializes
   brain.step(0)  # Now works
   ```

2. **"Cannot load file"**
   ```bash
   # Problem: Trying to load non-existent file
   nlm-checkpoint load missing.bin
   
   # Solution:
   # Make sure the file exists first
   nlm-checkpoint save brain.bin  # Create the file first
   nlm-checkpoint load brain.bin  # Then load it
   ```

3. **"No spikes generated"**
   ```python
   # Problem: Brain not receiving input
   brain = pynlm.BrainSimulator()
   brain.run(100)  # No spikes
   
   # Solution:
   brain.setupWorld()  # Add sensory input
   brain.enableLearning()  # Enable neuromodulation
   brain.run(100)  # Should now have spikes
   ```

### Getting Help

1. **Documentation**
   - Read this README for API documentation
   - Check `docs/` directory for detailed guides
   - Review `easy_usage.md` for quick start

2. **Examples**
   - Run demo.py: `python demo.py`
   - Run test_enhanced_api.py: `python test_enhanced_api.py`
   - Check CLI help: `nlm-sim --help`

3. **Community**
   - GitHub repository: https://github.com/nlm-project/nlm
   - Issues and feature requests
   - Discussion forums

## Future Improvements

### Planned Enhancements

1. **GUI Tools**
   - Neural network visualizer
   - Simulation monitor
   - Parameter explorer

2. **Advanced Features**
   - Multi-agent simulations
   - Social learning
   - Complex environments

3. **Integration**
   - Jupyter notebook support
   - Web interface
   - Mobile applications

4. **Performance**
   - GPU acceleration
   - Distributed computing
   - Optimized memory usage

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
