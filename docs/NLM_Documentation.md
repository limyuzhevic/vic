# NLM (Neural Learning Machine) - Documentation
# Phase 6: Final Integration - Comprehensive Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial intelligence framework that implements **brain-inspired computational systems** with spiking neural networks. Unlike traditional deep learning models, NLM focuses on **neuromorphic computing principles** including:

- **Real spiking neuron dynamics** (LIF neurons)
- **Event-driven computation** with synaptic delays
- **Integrated cognitive systems** (memory, prediction, planning)
- **Developmental processes** that adapt over time
- **Neuromodulation** for motivation and exploration

NLM represents a **systems-level approach** to artificial intelligence based on brain-inspired computational principles rather than traditional deep learning architectures.

## Architecture Overview

### Core Components

1. **Brain** - Central neural computation engine
   - LIF neuron dynamics
   - Event-driven spike propagation
   - Integrated memory, prediction, cognition
   - Neuromodulation systems

2. **Memory Systems**
   - **Working Memory**: Transient active information
   - **Episodic Memory**: Experience storage and replay
   - **Associative Memory**: Pattern associations

3. **Neuromodulation Systems**
   - **Dopamine**: Reward prediction error
   - **Curiosity**: Exploration motivation
   - **Novelty**: Novelty detection

4. **Prediction Systems**
   - Temporal sequence learning
   - Error signal computation

5. **Cognitive Systems**
   - **Neural Planner**: Action planning
   - **Concept Formation**: Pattern discovery
   - **Attention**: Selective focus

6. **Development System**
   - Plasticity rates modulated by age
   - Developmental stages

7. **AgentBrain** - Interface to world
   - Sensory transduction
   - Motor decoding
   - Reward modulation

8. **SimpleWorld** - Simulation environment
   - 2D grid world
   - Vision, touch, internal sensing
   - Action execution

## Getting Started

### Installation

NLM is installed as a Python package `pynlm` with C++ core. For development, clone the repository and build:

```bash
# Clone the repository
git clone <repo-url> nlm

# Build with scikit-build
cd nlm
pip install scikit-build-core pybind11 pytest numpy
pip install .  # or pip install -e . for development
```

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Run a few simulation steps
for i in range(100):
    brain.step(i)

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world (16x16 grid, 16 ray vision)
world.configure(16, 16, 16, 16)
world.reset()

# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(1000):
    world.update(0.001)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
```

## Building from Source

### Prerequisites

- C++ compiler with C++20 support
- CMake >= 3.16
- Python 3.8+
- pip

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with Python bindings
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build
make -j4
```

### Running Demos

The repository includes several demonstration executables:

```bash
# Phase 2: Real Neural Computation (basic demo)
./nlm

# Phase 3: Basic Environment Interaction
./nlm_phase3_demo

# Phase 4: Agent in World
./nlm_phase4_demo

# Phase 6: Integration Test (comprehensive demo)
./nlm_phase6_demo

# Performance benchmark
./nlm_benchmark
```

## Configuration

### Basic Configuration File

Create a `configs/default.cfg` file with basic settings:

```
# Core simulation
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.001
pruning_rate = 0.00001

# Neuromodulation
dopamine_baseline = 0.1

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm.log
```

### Advanced Configuration

For advanced features, use `configs/advanced_nlm.cfg` which includes:

- Detailed neuromodulation settings
- Advanced memory system configuration
- Cognitive system parameters
- Developmental stage settings
- Performance optimization options
- Debugging and monitoring features

### Command Line Configuration

Override configuration using command line arguments:

```bash
./nlm --config configs/custom.cfg
./nlm --neuron_count=500 --stdp_ltp_weight=0.02
```

## Python API Reference

### Core Classes

#### `Brain`
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step, current_time)
brain.receiveSensoryInput(input)
# ... many more methods
```

#### `AgentBrain`
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

#### `SimpleWorld`
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, vision_width, vision_height)
world.reset()
world.update(timestep)
percept = world.getSensoryPercept()
result = world.applyMotorCommand(action, time)
```

### Memory Systems

#### Working Memory
```python
working_memory = brain.getWorkingMemory()
traces = working_memory.getActiveTraces()
activation = working_memory.getNeuronActivation(neuron_id)
working_memory.storeToNeuron(neuron_id, activation)
```

#### Episodic Memory
```python
episodic_memory = brain.getEpisodicMemory()
episodes = episodic_memory.getEpisodes()
replay_episodes = episodic_memory.getEpisodesForReplay(count)
```

#### Concept Formation
```python
concept_formation = brain.getConceptFormation()
concept_count = concept_formation.getConceptCount()
concept = concept_formation.getConcept(concept_id)
```

### Neuromodulation Systems

```python
# Dopamine
dopamine = brain.getDopamine()
dopamine_level = dopamine.getLevel()

# Curiosity
curiosity = brain.getCuriosity()
curiosity_level = curiosity.getLevel()

# Novelty
novelty = brain.getNovelty()
novelty_level = novelty.getLevel()

# Prediction Error
prediction_error = brain.getPredictionErrorSignal()
error_level = prediction_error.getError()
```

### Development System

```python
development_system = brain.getDevelopmentSystem()
stage = development_system.getDevelopmentalStage()
development_system.update(brain, rng, timestep)
```

## Running Experiments

### Basic Experiment Template

```python
import pynlm
import time

def run_experiment(config_overrides=None):
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply overrides
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run episode
    total_reward = 0.0
    for step in range(1000):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        agent.applyRewardModulation(result.reward, 0.0)
        total_reward += result.reward
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Reward = {result.reward:.3f}, Total = {total_reward:.3f}")
    
    return {
        'total_reward': total_reward,
        'final_brain_state': brain,
        'final_world_state': world,
        'agent': agent
    }

# Run experiment
result = run_experiment({
    'neuron_count': 500,
    'enable_curiosity': True
})
```

### Learning Experiment

```python
def learning_experiment():
    """Demonstrates synaptic changes through experience"""
    config = pynlm.createDefaultConfig()
    
    # Enable plasticity
    config.set('stdp_ltp_weight', 0.02)
    config.set('stdp_ltd_weight', 0.015)
    config.set('plasticity_learning_rate', 0.02)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial state
    region = brain.getRegion(pynlm.RegionId(1))
    initial_weights = [syn.getWeight() for syn in region.getSynapses()]
    initial_mean = sum(initial_weights) / len(initial_weights)
    
    # Apply learning experience
    for step in range(1000):
        # Inject pattern to stimulate learning
        for i in range(20):  # 20 sensory neurons
            # Get sensory neurons and inject current
            pass
        
        brain.step(step)
    
    # Get final state
    final_weights = [syn.getWeight() for syn in region.getSynapses()]
    final_mean = sum(final_weights) / len(final_weights)
    
    print(f"Learning experiment results:")
    print(f"  Initial mean weight: {initial_mean:.4f}")
    print(f"  Final mean weight: {final_mean:.4f}")
    print(f"  Change: {final_mean - initial_mean:.4f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return final_mean - initial_mean > 0.001  # Learning occurred
```

## Advanced Features

### Phase 6 Integration

The Phase 6 integration demonstrates all major components working together:

```python
def phase6_integration_demo():
    """Comprehensive demonstration of Phase 6 integration"""
    config = pynlm.createDefaultConfig()
    
    # Enable all advanced features
    config.set('enable_development', True)
    config.set('enable_curiosity', True)
    config.set('enable_planning', True)
    config.set('enable_concept_formation', True)
    config.set('enable_episodic_memory', True)
    config.set('enable_associative_memory', True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create complex world with multiple objects
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 32, 32)  # Larger world
    world.reset()
    
    # Add resources and hazards
    for x in range(5, 15, 3):
        for y in range(5, 15, 3):
            # Add resource
            resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 1.0, 0.5)
            world.addObject(resource)
            
            # Add hazard nearby
            hazard = pynlm.WorldObject(x + 2, y + 2, pynlm.WorldObjectType.Hazard, -10.0, 0.3)
            world.addObject(hazard)
    
    # Create agent with full capabilities
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run extended episode
    total_reward = 0.0
    episodes_completed = 0
    
    for step in range(5000):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        agent.applyRewardModulation(result.reward, 0.0)
        total_reward += result.reward
        
        # Log complex information
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {result.reward:.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Working memory: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
            print(f"  Episodic memory: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
            print(f"  Planning confidence: {agent.getBrain().getPlanner().getPlanningConfidence() if agent.getBrain() and agent.getBrain().getPlanner() else 0:.3f}")
            print()
    
    return {
        'success': True,
        'total_reward': total_reward,
        'episodes_completed': episodes_completed,
        'final_state': {
            'brain_state': brain,
            'world_state': world,
            'agent_state': agent
        }
    }
```

### Memory Replay Demo

```python
def memory_replay_demo():
    """Demonstrates episodic memory replay"""
    config = pynlm.createDefaultConfig()
    
    # Configure for episodic memory
    config.set('max_steps_per_episode', 2000)
    config.set('replay_interval', 100)  # Replay every 100 steps
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Store initial state for comparison
    initial_memory_count = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
    
    # Run simulation
    for step in range(1500):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, step * 0.001)
        
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Check for memory replay
        if step % 100 == 0 and brain.getEpisodicMemory():
            replay_count = brain.getEpisodicMemory().getReplayCount()
            print(f"Step {step}: Replay count = {replay_count}")
    
    # Check final memory state
    final_memory_count = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
    
    print(f"Memory replay demo results:")
    print(f"  Initial episodes: {initial_memory_count}")
    print(f"  Final episodes: {final_memory_count}")
    print(f"  Episodes stored: {final_memory_count - initial_memory_count}")
    
    return final_memory_count - initial_memory_count > 0
```

## Building and Running Tests

### Running Python Tests

```bash
# Install test dependencies
pip install pytest numpy

# Run tests
pytest tests/

# Run specific test module
pytest tests/test_basic.py
pytest tests/test_integration.py
```

### Running C++ Tests

```bash
# Build with tests enabled
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_TESTS=ON
make -j4

# Run tests
./test_suite
```

## Troubleshooting

### Common Issues

#### Brain Initialization Fails
```
# Check that config has required values
config.set("neuron_count", 100)
config.set("region_count", 1)
config.set("simulation_timestep", 0.001)
```

#### Python Import Error
```
# Ensure Python bindings are built
pip install -e .

# Check Python environment
python -c "import pynlm; print(pynlm.__version__)"
```

#### Memory Issues
```
# Reduce network size
config.set("neuron_count", 500)

# Check memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

#### Performance Issues
```
# Reduce planning depth
agent.getBrain().getPlanner().setPlanningDepth(2)

# Increase timestep
config.set("simulation_timestep", 0.01)
```

## API Reference

### Configuration

#### `Config` class
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(argc, argv)` - Load from command line
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get value
- `getOr(key, default)` - Get value with default

### Brain Operations

#### `Brain.step(step)`
```python
brain.step(current_step)
```

#### `Brain.step(step, current_time)`
```python
brain.step(current_step, current_time)
```

#### `Brain.receiveSensoryInput(input)`
```python
brain.receiveSensoryInput(percept)
```

#### `Brain.produceAction()`
```python
action = brain.produceAction()
```

### AgentBrain Operations

#### `AgentBrain.processSensoryInput(percept)`
```python
agent.processSensoryInput(percept)
```

#### `AgentBrain.decodeMotorCommand()`
```python
action = agent.decodeMotorCommand()
```

#### `AgentBrain.applyRewardModulation(reward, predicted_reward)`
```python
agent.applyRewardModulation(reward, predicted_reward)
```

### Memory System Access

```python
# Working Memory
working_memory = brain.getWorkingMemory()
if working_memory:
    activation = working_memory.getNeuronActivation(neuron_id)
    traces = working_memory.getActiveTraces()

# Episodic Memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episodes = episodic_memory.getEpisodes()
    replay_episodes = episodic_memory.getEpisodesForReplay(count)

# Concept Formation
concept_formation = brain.getConceptFormation()
if concept_formation:
    concept_count = concept_formation.getConceptCount()
    concept = concept_formation.getConcept(concept_id)
```

## Contributing

### Code Standards

- Use `//` for single-line comments
- Use `/* ... */` for multi-line comments
- Follow Google-style C++ formatting
- Use `nullptr` instead of `NULL`
- Use `auto` when appropriate
- Prefer range-based for loops
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)

### Testing

1. Write unit tests for new functionality
2. Add integration tests for complex scenarios
3. Include performance benchmarks
4. Document test behavior and expected outcomes

### Documentation

1. Update API documentation
2. Add usage examples
3. Include configuration parameters
4. Document known limitations

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This project builds on concepts from:
- Computational neuroscience
- Neuromorphic computing
- Brain-inspired artificial intelligence
- Spiking neural networks
- Autonomous agent research

## Future Directions

### Planned Features

1. **Multi-agent systems** - Learning from other agents
2. **Language processing** - Text as additional sensory input
3. **Embodied cognition** - Physical interaction with complex environments
4. **Meta-learning** - Learning how to learn
5. **Transfer learning** - Applying knowledge across domains

### Research Areas

1. **Neural plasticity mechanisms** - How the brain maintains stability while learning
2. **Attention and focus** - Selective information processing
3. **Memory consolidation** - How memories become stable
4. **Developmental trajectories** - How systems mature over time
5. **Social learning** - Learning from others

## Contact

For questions, issues, or contributions:
- GitHub repository: <repo-url>
- Documentation: <docs-url>
- Issues: <issues-url>

## Changelog

### Version 0.1.0 (Phase 6 Complete)
- Complete integration of all brain systems
- Working memory and episodic memory operational
- Neuromodulation systems functional
- Prediction and planning systems implemented
- Development system with staged plasticity
- Python bindings with comprehensive API
- Advanced configuration system with validation

## Acknowledgments

This project represents a significant milestone in brain-inspired artificial intelligence, implementing a complete, coherent artificial brain with all major neural systems integrated and functional.

The NLM framework provides a foundation for developing autonomous agents that learn from experience through biological principles rather than traditional machine learning approaches.
