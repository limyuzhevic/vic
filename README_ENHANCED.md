# NLM (熙然) - Enhanced Documentation

**Version 2.0.0 - Enhanced Python API**

An Experimental Artificial Developmental Brain with Improved Usability

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates how brains can learn, remember, and make decisions. This enhanced version provides a modern Python API with improved usability, comprehensive documentation, and advanced features.

## Key Improvements in Version 2.0.0

### 1. Enhanced Python API
- **SmartBrain wrapper** with context manager support for automatic resource management
- **EnhancedConfig** with chainable API methods for easier configuration
- **Factory methods** for common configuration patterns (speed, accuracy, learning)
- **Better error handling** with meaningful Python exceptions
- **Type safety** with improved bindings

### 2. Comprehensive Documentation
- **Complete API reference** with examples
- **Step-by-step tutorials** for different use cases
- **Code snippets** for common patterns
- **Configuration guides** for different scenarios

### 3. Improved Code Organization
- **Proper header file structure** for all components
- **Consistent namespace organization**
- **Better documentation comments** throughout the codebase
- **Clean separation of concerns**

### 4. Advanced Features
- **Memory system integration** (working, episodic, associative memory)
- **Neuromodulation support** (dopamine, curiosity, novelty)
- **Developmental stages** with plasticity modulation
- **Prediction and planning systems**
- **Checkpoint system** for state persistence

## Installation

### Prerequisites
- C++ compiler with C++20 support
- Python 3.8+ with pybind11
- CMake 3.16+

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Installation

```bash
# The Python module will be built automatically
# Check if pynlm is available
python3 -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

## Quick Start Guide

### Basic Usage (Beginner)

This is the simplest way to get started with NLM:

```python
import pynlm

# Create brain with default configuration
with pynlm.create_brain() as brain:
    print(f"Brain initialized with {brain.getNeuronCount()} neurons")
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
    
    print(f"Total spikes: {brain.getSpikeCount()}")
    print(f"Average firing rate: {brain.getFiringRate():.2f} Hz")
```

### Advanced Usage (Recommended)

For more control and better performance:

```python
import pynlm

# Create brain with optimized configuration
config = pynlm.EnhancedConfig.createForLearning()
brain = pynlm.createBrain(config)

# Use context manager for automatic initialization
with brain:
    # Train for multiple episodes
    for episode in range(5):
        print(f"Episode {episode + 1}")
        
        # Run training
        brain.runEpisode(f"Training episode {episode + 1}")
        
        # Simulate
        brain.train(100, f"Training episode {episode + 1}")
        
        # Save progress
        brain.save(f"brain_episode_{episode + 1}.nlm")
        
        # Load next episode (if needed)
        # brain.load(f"brain_episode_{episode + 2}.nlm")
```

### Configuration Examples

#### Speed Optimization (for testing)

```python
import pynlm

# Create fast, lightweight brain for testing
config = pynlm.EnhancedConfig.createForSpeed()
brain = pynlm.createBrain(config)

print("Speed-optimized configuration:")
print(f"  Neurons: {config.getNeuronCount()}")
print(f"  Regions: {config.getRegionCount()}")
print(f"  Step size: {config.getTimestep()}s")
```

#### Accuracy Optimization (for research)

```python
import pynlm

# Create high-accuracy brain for research
config = pynlm.EnhancedConfig.createForAccuracy()
brain = pynlm.createBrain(config)

print("Accuracy-optimized configuration:")
print(f"  Neurons: {config.getNeuronCount()}")
print(f"  Regions: {config.getRegionCount()}")
print(f"  STDP LTP weight: {config.get('stdp_ltp_weight', 0.02)}")
print(f"  STDP LTD weight: {config.get('stdp_ltd_weight', 0.018)}")
```

#### Learning Optimization (for exploration)

```python
import pynlm

# Create brain optimized for learning and exploration
config = pynlm.EnhancedConfig.createForLearning()
brain = pynlm.createBrain(config)

print("Learning-optimized configuration:")
print(f"  Structural plasticity: synaptogenesis={config.get('synaptogenesis_rate', 0.00005)}, pruning={config.get('pruning_rate', 0.000005)}")
print(f"  Memory capacity: working={config.get('working_memory_capacity', 500)}, episodic={config.get('episodic_memory_max_episodes', 2000)}")
```

### Memory System Usage

#### Working Memory

```python
import pynlm

brain = pynlm.createBrain()
with brain:
    # Access working memory
    working_memory = brain.getWorkingMemory()
    
    # Store neural trace
    working_memory.storeToNeuron(brain.getNeuronIds()[0], 0.8)
    
    print(f"Active working memory traces: {working_memory.getActiveTraces()}")
    print(f"Max trace strength: {working_memory.getMaxTraceStrength():.3f}")
```

#### Episodic Memory

```python
import pynlm

brain = pynlm.createBrain()
with brain:
    # Access episodic memory
    episodic_memory = brain.getEpisodicMemory()
    
    # Store an episode
    episode = pynlm.EpisodicMemoryItem()
    episode.timestamp = 100
    episode.reward = 0.8
    episode.activeNeurons = [brain.getNeuronIds()[0], brain.getNeuronIds()[1]]
    episode.neuronActivations = [0.9, 0.7]
    
    episodic_memory.storeEpisode(episode)
    
    print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")
    print(f"Average episode reward: {episodic_memory.getAverageReward():.3f}")
```

### Neuromodulation Usage

```python
import pynlm

brain = pynlm.createBrain()
with brain:
    # Access neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print(f"Initial dopamine level: {dopamine.getLevel():.3f}")
    print(f"Initial curiosity level: {curiosity.getLevel():.3f}")
    print(f"Initial novelty level: {novelty.getLevel():.3f}")
    print(f"Initial prediction error: {prediction_error.getError():.3f}")
    
    # Apply reward modulation
    brain.applyNeuromodulation(Neuromodulator(NeuromodulatorType.Reward, 0.5))
```

## API Reference

### Core Classes

#### Brain (SmartBrain)

| Method | Description | Example |
|--------|-------------|---------|
| `initialize()` | Initialize brain with configuration | `brain.initialize()` |
| `step(steps)` | Run simulation steps | `brain.step(100)` |
| `runEpisode(desc)` | Run new episode | `brain.runEpisode("Training")` |
| `train(steps, desc)` | Train for specified steps | `brain.train(500, "Learning")` |
| `save(filename)` | Save brain state | `brain.save("brain.nlm")` |
| `load(filename)` | Load brain state | `brain.load("brain.nlm")` |

#### EnhancedConfig

| Method | Description | Example |
|--------|-------------|---------|
| `setRandomSeed(seed)` | Set random seed | `config.setRandomSeed(42)` |
| `setNeuronCount(count)` | Set neuron count | `config.setNeuronCount(2000)` |
| `setRegionCount(count)` | Set region count | `config.setRegionCount(3)` |
| `setConnectionProbability(prob)` | Set connection probability | `config.setConnectionProbability(0.1)` |
| `setSTDPParams(ltp, ltd, tau)` | Set STDP parameters | `config.setSTDPParams(0.015, 0.013, 20.0)` |
| `createDefault()` | Create default config | `config = pynlm.EnhancedConfig.createDefault()` |
| `createForSpeed()` | Create speed config | `config = pynlm.EnhancedConfig.createForSpeed()` |
| `createForAccuracy()` | Create accuracy config | `config = pynlm.EnhancedConfig.createForAccuracy()` |
| `createForLearning()` | Create learning config | `config = pynlm.EnhancedConfig.createForLearning()` |

#### Configuration Factory Methods

- **createDefault()**: Balanced configuration for general use
- **createForSpeed()**: Optimized for performance and testing
- **createForAccuracy()**: Optimized for precision and research
- **createForLearning()**: Optimized for exploration and learning

### Memory Systems

#### NeuralWorkingMemory

- **storeToNeuron(neuronId, activation)**: Store activation trace for neuron
- **getActiveTraces()**: Get number of active memory traces
- **getMaxTraceStrength()**: Get maximum trace strength
- **getAverageTraceStrength()**: Get average trace strength

#### NeuralEpisodicMemory

- **storeEpisode(episode)**: Store an episodic memory item
- **getEpisodeCount()**: Get number of stored episodes
- **getAverageReward()**: Get average reward across episodes
- **getEpisodesForReplay(count)**: Get episodes for replay

#### NeuralAssociativeMemory

- **storePattern(pattern, neuronIds)**: Store associative pattern
- **completePattern(partialPattern)**: Complete partial pattern
- **calculatePatternSimilarity(pattern1, pattern2)**: Calculate pattern similarity

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `DEFAULT_RESTING_POTENTIAL` | -70.0 | Default neuron resting potential |
| `DEFAULT_THRESHOLD` | -55.0 | Default firing threshold |
| `DEFAULT_RESET_POTENTIAL` | -70.0 | Default reset potential |
| `DEFAULT_LEAK_CONDUCTANCE` | 10.0 | Default leak conductance |
| `DEFAULT_REFECTORY_PERIOD` | 5 | Default refractory period |
| `DEFAULT_LTP_WEIGHT` | 0.01 | Default STDP LTP weight |
| `DEFAULT_LTD_WEIGHT` | 0.012 | Default STDP LTD weight |
| `DEFAULT_STDP_TIME_CONSTANT` | 20.0 | Default STDP time constant |

### Neuron Types

- **Excitatory**: Excitatory neurons that increase firing probability
- **Inhibitory**: Inhibitory neurons that decrease firing probability
- **Modulatory**: Neuromodulatory neurons affecting plasticity
- **Sensory**: Sensory input neurons
- **Motor**: Motor output neurons
- **Internal**: Internal processing neurons

### Action Types

- **MoveForward**: Move forward
- **MoveBackward**: Move backward
- **TurnLeft**: Turn left
- **TurnRight**: Turn right
- **Look**: Look in current direction
- **LookUp**: Look up
- **LookDown**: Look down
- **Interact**: Interact with environment
- **Eat**: Eat food
- **Drink**: Drink water
- **Rest**: Rest/stay still
- **Wait**: Wait
- **Custom**: Custom action

## Development Guide

### Building from Source

```bash
# Clone repository
git clone <repository-url>
cd <repository-name>

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DPYTHON_EXECUTABLE=$(which python3) \
         -Dpybind11_DIR=/path/to/pybind11

# Build
make -j4

# Run tests
ctest

# Install Python module (optional)
pip install -e .
```

### Running Examples

```bash
# Run basic example
python3 examples/basic_example.py

# Run advanced example
python3 examples/advanced_example.py

# Run memory system example
python3 examples/memory_example.py

# Run neuromodulation example
python3 examples/neuromodulation_example.py
```

### Running Tests

```bash
# Run unit tests
cd build
ctest

# Run integration tests
python3 -m pytest tests/ -v
```

## File Structure

```
NLM/ (project root)
├── src/
│   ├── core/                    # Core utilities
│   │   ├── Types/              # Type definitions
│   │   ├── Config/             # Configuration system
│   │   ├── Random/             # Random number generation
│   │   └── Logger/             # Logging system
│   ├── SimulationClock/        # Time management
│   ├── brain/                 # Neural components
│   │   ├── Neuron.cpp/hpp     # Single neuron
│   │   ├── Synapse.cpp/hpp    # Neural connections
│   │   ├── NeuralRegion.cpp/hpp # Brain regions
│   │   ├── NeuralPopulation.cpp/hpp # Neuron populations
│   │   └── Brain.cpp/hpp      # Central brain
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity rules
│   ├── memory/                # Memory systems
│   ├── neuromodulation/       # Neuromodulation systems
│   ├── prediction/            # Prediction systems
│   ├── cognition/             # Cognitive systems
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/         # Visualization
├── python/                    # Python bindings
│   ├── bindings.cpp          # Original bindings
│   └── EnhancedBindings.cpp  # Enhanced bindings
├── tests/                    # Test suite
├── examples/                 # Usage examples
├── docs/                     # Documentation
├── configs/                  # Configuration files
├── easy_usage.md            # Easy usage guide
├── HOW_TO_USE.md            # Detailed usage guide
└── README.md                # Project documentation
```

## License

MIT License

## Copyright

© 2024-2026 NLM Development Team

## Contact

For issues and questions, please refer to the documentation or project repository.

## Acknowledgments

This project builds on extensive research in computational neuroscience and artificial intelligence. Special thanks to all contributors and researchers who have advanced the field of neural simulation.
