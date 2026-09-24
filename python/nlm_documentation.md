# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides access to a complete artificial brain simulation system. This documentation provides comprehensive examples and explanations for users of all skill levels.

## System Architecture

The NLM Python API exposes the following main components:

### Core Brain System
- **Brain**: The central neural processing unit
- **Config**: Configuration management
- **Neuron**: Individual spiking neurons
- **Synapse**: Connections between neurons

### Memory Systems
- **WorkingMemory**: Short-term memory for active information
- **EpisodicMemory**: Experience and episode storage
- **AssociativeMemory**: Pattern associations

### Prediction System
- **PredictionSystem**: Predictive coding and error computation

### Cognitive Systems
- **NeuralPlanner**: Action planning and decision making
- **ConceptFormation**: Pattern discovery and abstraction
- **AttentionalSelection**: Focus and attention control

### Neuromodulation
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Learning signal

### Sensory and Motor
- **Vision**: Visual processing
- **Audio**: Auditory processing
- **InternalSignals**: Internal state monitoring
- **Action**: Motor output
- **SimpleWorld**: Environment interface

### Agent Interface
- **AgentBrain**: Bridge between brain and world
- **SensoryPercept**: Sensory input data

## Quick Start Guide

### Example 1: Basic Neural Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

### Example 2: Complete Agent in World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 3: Working Memory Integration

```python
import pynlm

# Create brain with working memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable working memory features
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Simulation with memory integration
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check working memory state
    working_memory = brain.getWorkingMemory()
    if working_memory:
        active_traces = working_memory.getActiveTraces()
        print(f"Step {step}: {active_traces} active memory traces")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Memory-integrated simulation complete!")
```

## Advanced Usage Examples

### Example 4: Prediction System

```python
import pynlm

# Create brain with prediction system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with prediction
for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check prediction system status
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        error = prediction_system.getPredictionError()
        confidence = prediction_system.getConfidence()
        print(f"Step {step}: Prediction error={error:.3f}, confidence={confidence:.3f}")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Prediction system simulation complete!")
```

### Example 5: Concept Formation

```python
import pynlm

# Create brain with concept formation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable concept formation features
agent.enableDevelopment(True)

# Run simulation to form concepts
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check concept formation status
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        # Check for stable concepts
        for concept_id in range(1, 10):  # Check first 10 concepts
            if concept_formation.isConceptStable(concept_id):
                prototype = concept_formation.getConceptPrototype(concept_id)
                print(f"Step {step}: Stable concept {concept_id} formed with {len(prototype)} features")
                break
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Concept formation simulation complete!")
```

### Example 6: Episodic Memory

```python
import pynlm

# Create brain with episodic memory
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable episodic memory features
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with episodic memory
for step in range(400):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check episodic memory status
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        episode_count = episodic_memory.getEpisodeCount()
        print(f"Step {step}: {episode_count} episodes stored")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Episodic memory simulation complete!")
```

### Example 7: Neuromodulation Integration

```python
import pynlm

# Create brain with full neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with neuromodulation
for step in range(600):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Check neuromodulation status
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    if dopamine:
        print(f"Step {step}: Dopamine level={dopamine.getLevel():.3f}")
    if curiosity:
        print(f"Step {step}: Curiosity level={curiosity.getLevel():.3f}")
    if novelty:
        print(f"Step {step}: Novelty level={novelty.getLevel():.3f}")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Neuromodulation simulation complete!")
```

## Python API Reference

### Core Classes

#### `pynlm.createDefaultConfig()`
Creates a default configuration object.

#### `pynlm.createBrain(config)`
Creates a new brain with the specified configuration.

#### `pynlm.createSimpleWorld()`
Creates a simple 2D world for simulation.

#### `pynlm.createAgentBrain(brain)`
Creates an agent brain interface connecting the brain to a world.

### Brain Methods

#### `brain.initialize()`
Initializes the brain with configuration parameters.

#### `brain.step(step)`
Performs one simulation step with the given step number.

#### `brain.step(step, time)`
Performs one simulation step with timestamp.

#### `brain.receiveSensoryInput(input)`
Injects sensory input into the brain.

#### `brain.injectCurrent(neuron, current)`
Injects current directly into a specific neuron.

#### `brain.injectCurrentToNeurons(type, current)`
Injects current into all neurons of a specific type.

#### `brain.produceAction()`
Produces a motor action based on neural activity.

#### `brain.save(filepath)`
Saves brain state to file.

#### `brain.load(filepath)`
Loads brain state from file.

#### `brain.reset()`
Resets the brain to initial state.

### Statistics Methods

#### `brain.getTotalNeuronCount()`
Returns total number of neurons in brain.

#### `brain.getTotalSynapseCount()`
Returns total number of synapses in brain.

#### `brain.getActiveNeuronCount()`
Returns number of currently active neurons.

#### `brain.getFiringNeuronCount()`
Returns number of neurons firing in current step.

#### `brain.getTotalSpikeCount()`
Returns total spike count across all neurons.

#### `brain.getAverageFiringRate()`
Returns average firing rate across all neurons.

#### `brain.getExcitationInhibitionRatio()`
Returns excitation/inhibition balance ratio.

### Memory System Methods

#### `brain.getWorkingMemory()`
Returns working memory object.

#### `brain.getEpisodicMemory()`
Returns episodic memory object.

#### `brain.getAssociativeMemory()`
Returns associative memory object.

### Prediction System Methods

#### `brain.getPredictionSystem()`
Returns prediction system object.

#### `prediction_system.getPredictionError()`
Returns current prediction error.

#### `prediction_system.getConfidence()`
Returns prediction confidence level.

#### `prediction_system.getErrorHistory()`
Returns prediction error history.

### Cognitive System Methods

#### `brain.getPlanner()`
Returns neural planner object.

#### `brain.getConceptFormation()`
Returns concept formation object.

#### `brain.getAttention()`
Returns attentional selection object.

### Neuromodulation System Methods

#### `brain.getDopamine()`
Returns dopamine neuromodulator object.

#### `brain.getCuriosity()`
Returns curiosity neuromodulator object.

#### `brain.getNovelty()`
Returns novelty detection object.

#### `brain.getPredictionErrorSignal()`
Returns prediction error neuromodulator object.

### Agent Brain Methods

#### `agent.initialize(world)`
Initializes agent with world interface.

#### `agent.processSensoryInput(percept)`
Processes sensory input and injects into brain.

#### `agent.decodeMotorCommand()`
Decodes motor command from neural activity.

#### `agent.applyRewardModulation(reward, predicted)`
Applies reward-based learning modulation.

#### `agent.updateDevelopment(timestep)`
Updates development system.

#### `agent.enableRewardModulation(enable)`
Enables/disables reward modulation.

#### `agent.enableCuriosity(enable)`
Enables/disables curiosity.

#### `agent.enableDevelopment(enable)`
Enables/disables development.

#### `agent.enableStructuralPlasticity(enable)`
Enables/disables structural plasticity.

### Configuration Methods

#### `config.loadFromFile(filepath)`
Loads configuration from JSON file.

#### `config.loadFromArgs(argc, argv)`
Loads configuration from command line arguments.

#### `config.saveToFile(filepath)`
Saves configuration to JSON file.

#### `config.has(key)`
Checks if configuration key exists.

#### `config.getKeys()`
Returns all configuration keys.

### World Methods

#### `world.configure(width, height, visionWidth, visionHeight)`
Configures world dimensions and vision.

#### `world.reset()`
Resets world to initial state.

#### `world.update(timestep)`
Updates world by given timestep.

#### `world.applyMotorCommand(cmd, time)`
Applies motor command to world.

#### `world.getSensoryPercept()`
Gets sensory percept from world.

#### `world.getAgentBody()`
Gets agent body state.

### Data Structures

#### `SensoryInput`
Base class for sensory input.

#### `Vision`
Visual sensory input.

#### `Audio`
Auditory sensory input.

#### `InternalSignals`
Internal state sensory input.

#### `Action`
Motor action representation.

#### `ActionResult`
Result of applying action to world.

#### `WorldObject`
Object in the world.

#### `AgentBody`
Agent position and state.

#### `SensoryPercept`
Complete sensory input data.

## Advanced Topics

### Configuration System

The configuration system uses a template-based approach with multiple sources:

1. **Default Configuration**: Built-in sensible defaults
2. **File Configuration**: JSON file loading
3. **Command Line**: Override via command line arguments
4. **Runtime**: Dynamic updates during simulation

Configuration examples:

```python
config = pynlm.createDefaultConfig()

# Set custom values
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.0005)

# Save configuration
config.saveToFile("my_config.json")
```

### Development Stages

Brains go through developmental stages that affect plasticity:

- **Initial**: High plasticity, rapid learning
- **CriticalPeriod**: Modulated plasticity, sensitive learning
- **Maturation**: Stable, refined connections
- **Adult**: Maintained stability, occasional adaptation
- **Aging**: Reduced plasticity, conserved resources

```python
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```

### Memory Systems

The memory systems work together:

1. **Working Memory**: Maintains active information with competition
2. **Episodic Memory**: Stores experiences with replay and consolidation
3. **Associative Memory**: Forms pattern associations

Memory persistence can be managed through checkpoints and replay.

### Prediction and Learning

The prediction system enables predictive coding:

1. **Prediction**: Brain predicts next sensory state
2. **Error**: Compare prediction with actual input
3. **Learning**: Update internal models based on error

```python
prediction_system = brain.getPredictionSystem()
error = prediction_system.getPredictionError()
confidence = prediction_system.getConfidence()
```

### Attention and Focus

Attention controls information processing priority:

- **Bottom-up**: Stimulus-driven salience
- **Top-down**: Goal-directed bias
- **Global Competition**: Winner-take-all selection

```python
attention = brain.getAttention()
attention.applyTopDownBias(neuronId, biasStrength)
```

### Performance Monitoring

Monitor simulation performance:

```python
brain.logStatus()  # Print comprehensive status
```

## Installation and Setup

### Prerequisites

- Python 3.8+
- CMake 3.16+
- Build system with C++20 support

### Building from Source

```bash
# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install .
```

### Development Installation

```bash
# Install in development mode for easier debugging
pip install -e .
```

### Testing

```bash
# Run Python tests
pytest tests/

# Run C++ tests
./build/tests/nlm_tests
```

## Troubleshooting

### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected

### Performance Issues

- Use larger timesteps for faster simulation
- Reduce number of neurons for quick testing
- Use checkpointing to save intermediate states

### Configuration Problems

- Check JSON syntax in configuration files
- Use `config.getKeys()` to list available keys
- Use default configuration for testing

## Contributing

This is an experimental research project. Contributions are welcome for:

1. **Bug fixes**: Fix integration issues and improve stability
2. **New features**: Add new cognitive systems or neuromodulators
3. **Documentation**: Improve API documentation and examples
4. **Performance**: Optimize for faster simulation
5. **Testing**: Add comprehensive test suites

### Code Style

- Follow existing C++ coding standards
- Use consistent naming conventions
- Add comments for complex algorithms
- Include error checking and validation

### Research Contributions

- Document experimental findings
- Add new simulation paradigms
- Publish results in research venues

## License

MIT License. See LICENSE file for details.

## Acknowledgments

This project builds on decades of research in computational neuroscience and artificial intelligence. Special thanks to the research community for foundational work in:

- Spiking neural networks
- Memory systems and consolidation
- Predictive coding and brain theory
- Neuromodulation and reinforcement learning
- Developmental robotics and artificial cognition

## Future Directions

### Immediate Goals (Next 6 months)

1. **Complete Integration**: Full integration of all cognitive systems
2. **Performance Optimization**: 10x speedup for larger simulations
3. **Documentation**: Complete API documentation with examples
4. **Testing**: Comprehensive test suite with regression testing

### Medium-term Goals (1-2 years)

1. **Advanced Learning**: Continual learning without catastrophic forgetting
2. **Multi-agent**: Social learning and communication
3. **Real-world**: Benchmarks on standard cognitive tasks
4. **Hardware**: Optimized implementations for different platforms

### Long-term Goals (5+ years)

1. **General Intelligence**: Artificial general intelligence capabilities
2. **Developmental Psychology**: Reproduction of developmental trajectories
3. **Biological Plausibility**: Closer connection to biological brain mechanisms
4. **Ethical AI**: Safe and responsible artificial cognition

## Contact

For questions, issues, or collaboration:

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Research Papers: Available in docs/ directory

---

*This documentation is continuously updated as the project evolves. Check the repository for the latest information.*

Copyright 2026 NLM Research Project
