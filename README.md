# NLM Project Documentation

This document provides an overview of the NLM (Neural Learning Machine) project improvements made according to the Phase 6 Final Audit recommendations.

## Project Overview

NLM is an experimental computational brain project that creates neural systems that learn from experience through interaction with an environment. The project implements biological brain principles using spiking neural networks and event-driven dynamics.

## Key Improvements Made

### 1. Fixed Brain Integration Issues
- **Memory Systems**: Connected working memory, episodic memory, and associative memory to the main brain loop
- **Prediction System**: Integrated prediction system with sensory processing and temporal sequence learning
- **Cognitive Systems**: Connected neural planner, concept formation, and attentional selection
- **Neuromodulation**: Enhanced dopamine, curiosity, and novelty integration with memory and attention
- **Development**: Improved developmental stage progression effects on plasticity and behavior

### 2. Enhanced Python Bindings
- **Advanced Configuration Management**: JSON/YAML file support with enhanced error handling
- **Memory System Access**: Full integration of working memory, episodic memory, and prediction systems
- **Error Handling**: Comprehensive exception handling with Pythonic error messages
- **Advanced Features**: Support for cognitive systems, neuromodulation, and development

### 3. Improved Configuration System
- **JSON/YAML Support**: Enhanced configuration file parsing with error recovery
- **Better Validation**: Configuration validation and logging
- **Advanced Set/Get Methods**: Support for complex configuration types
- **Source Tracking**: Track configuration source (file, command line, default)

### 4. New Command Line Interface
- **Easy System Interaction**: Command-line tools for brain simulation and management
- **Agent Simulation**: Complete agent-world interaction capabilities
- **Configuration Management**: Set, get, and list configuration values
- **Brain State Management**: Save and load brain states
- **Status Reporting**: Detailed brain status reporting with metrics

### 5. Enhanced Documentation
- **Beginner Guide**: Comprehensive easy-to-follow documentation with examples
- **Advanced Usage**: Detailed documentation for advanced users
- **Troubleshooting**: Common issues and solutions
- **API Reference**: Complete API documentation with code examples

## System Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Key Components

### Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Enhanced configuration management
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### Memory Systems (Phase 6 Complete)
- **Working Memory**: Persistent activity for temporary storage and competition
- **Episodic Memory**: Experience encoding and replay mechanisms
- **Associative Memory**: Experience-based pattern associations
- **Concept Formation**: Pattern discovery without labels

### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation combining novelty and prediction error
- **Novelty**: Novelty detection for exploration
- **Prediction Error**: Temporal sequence learning and error signals

### Cognitive Systems (Phase 6 Integrated)
- **Attention**: Competitive selection (not Transformer QKV)
- **Planning**: Predictive action selection and multi-step evaluation
- **Self-Model**: Sensorimotor self-awareness
- **Social Learning**: Observation and imitation capabilities

### Prediction System (Phase 6 Integrated)
- **Temporal Sequence Learning**: Predict action consequences
- **Multi-step Prediction**: Future state prediction
- **Error-based Learning**: Prediction error signals for plasticity

### Development System (Phase 6 Complete)
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
- **Plasticity Modulation**: Age-dependent learning rates
- **Structural Plasticity**: Synaptogenesis and pruning

## Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Complete Agent Simulation

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}")
```

### Using the Command Line Interface

```bash
# Run a simple brain simulation
nlm-cli simulate --steps 1000

# Run an agent in a world
nlm-cli agent --width 30 --height 30

# Get brain status
nlm-cli status --file my_brain.bin

# Manage configuration
nlm-cli config --set "brain.neuron_count" 2000
nlm-cli config list
nlm-cli config get "brain.synapse_density"
```

## Configuration Options

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Development and Building

### Prerequisites
- C++20 compiler
- CMake 3.16 or higher
- Python 3.8 or higher

### Building from Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installing Python Package

```bash
pip install pybind11 scikit-build-core pytest numpy
pip install .

# Or in development mode
pip install -e .
```

## Testing

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Python Tests

```bash
pytest tests/
```

## Performance Considerations

### Memory Usage
- Working memory: O(neuron_count / 10) memory traces
- Episodic memory: Configurable maximum episodes
- Neural ground memory: Pattern-based storage

### Time Complexity
- Neural computation: O(neuron_count × connections) per step
- Plasticity updates: O(synapse_count) per step
- Memory updates: O(active_traces) per step

### Optimization Features
- Event-driven spike processing
- Sparse connectivity
- Memory pools
- Parallel processing

## Scientific Limitations

### Biological Fidelity
- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology

### Claims
NLM does NOT claim to reproduce biological brains accurately. Current limitations include:

- No human intelligence claims
- No consciousness or sentience
- No human-like reasoning
- No genuine subjective experience

### Research Focus
Phase 6 investigates:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration results
