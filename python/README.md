# NLM Python Bindings Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain project with Python bindings that provide access to the full neural simulation framework. This document provides comprehensive information about using the Python API.

## Installation

### Prerequisites
- Python 3.8+
- Build tools (CMake, compiler)
- pybind11 development headers

### Installation Methods

#### Method 1: Using setup.py (Recommended)
```bash
# Clone the repository
cd /path/to/nlm

# Install Python bindings
pip install -e .
# or for development build
python setup.py build_ext --inplace
```

#### Method 2: Using pyproject.toml with pip
```bash
cd /path/to/nlm
pip install .[dev]  # Installs with development dependencies
```

#### Method 3: Manual Installation
```bash
# Create build directory and build
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install python/
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create a default brain
brain = pynlm.createBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Create all components
world = pynlm.createWorld(width=20, height=20)
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

# Initialize world and agent
world.setAgentStart(10, 10)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input and let brain think
    agent.processSensoryInput(percept)
    brain.step(step, step * 0.001)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)
```

## API Reference

### Module-level Functions

#### `pynlm.createBrain(configPath="")`
Creates a new Brain instance with default configuration.

**Parameters:**
- `configPath` (str): Optional path to configuration file (JSON)

**Returns:**
- `Brain`: Neural simulation brain instance

**Example:**
```python
brain = pynlm.createBrain()
brain = pynlm.createBrain("my_config.json")
```

#### `pynlm.createWorld(width=16, height=16, visionWidth=5, visionHeight=5)`
Creates a new SimpleWorld instance.

**Parameters:**
- `width` (int): World width in cells
- `height` (int): World height in cells
- `visionWidth` (int): Vision sensor width
- `visionHeight` (int): Vision sensor height

**Returns:**
- `SimpleWorld`: Simulated world instance

**Example:**
```python
world = pynlm.createWorld(30, 30, 7, 7)
```

#### `pynlm.createAgentBrain(brain)`
Creates an AgentBrain instance that interfaces between brain and world.

**Parameters:**
- `brain` (Brain): The neural brain instance

**Returns:**
- `AgentBrain`: Agent interface instance

#### `pynlm.createDefaultConfig()`
Creates a configuration with sensible default values.

**Returns:**
- `Config`: Default configuration instance

#### `pynlm.createAgentBrain(brain)`
Creates an AgentBrain instance.

**Returns:**
- `AgentBrain`: Agent interface instance

### Classes

#### `Config`
Configuration management for NLM system.

**Methods:**
- `__init__()`
- `loadFromFile(filepath)` - Load from JSON file
- `loadFromArgs(argc, argv)` - Load from command line
- `saveToFile(filepath)` - Save to JSON file
- `set(key, value, source=ConfigSource.Runtime)` - Set value
- `getOr(key, defaultValue)` - Get with default
- `has(key)` - Check if key exists
- `getKeys()` - Get all keys
- `clear()` - Clear all entries
- `summary()` - Get configuration summary

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("random_seed", 123)
print(config.summary())
```

#### `Vision`
Visual sensory input (subclass of SensoryInput).

**Methods:**
- `__init__(width=0, height=0, channels=3)`
- `setData(data)` - Set pixel data (flattened array)
- `getWidth()`, `getHeight()`, `getChannels()` - Get dimensions
- `getData()` - Get pixel data

**Example:**
```python
vision = Vision(10, 10)  # 10x10 grayscale
vision.setData([0.1, 0.2, 0.3, ...])
```

#### `Audio`
Audio sensory input (subclass of SensoryInput).

**Methods:**
- `__init__(sampleRate=0, numSamples=0)`
- `setData(data)` - Set audio samples
- `setSampleRate(rate)` - Set sample rate
- `getSampleRate()`, `getNumSamples()` - Get audio info

#### `InternalSignals`
Internal neural signals (subclass of SensoryInput).

**Methods:**
- `__init__()`
- `addSignal(value)` - Add a signal value
- `clearSignals()` - Clear all signals

#### `Action`
Motor action representation.

**Methods:**
- `__init__(type=ActionType.Wait, parameters=[])`
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set parameters
- `getName()` - Get human-readable name
- `clone()` - Create copy

#### `WorldObject`
Object in the simulated world.

**Properties:**
- `x`, `y`: Position
- `radius`: Size
- `type`: WorldObjectType
- `value`: Object value
- `active`: Whether object is active

#### `AgentBody`
Physical state of the agent.

**Properties:**
- `x`, `y`: Position
- `orientation`: Direction in radians
- `velocityX`, `velocityY`: Linear velocity
- `angularVelocity`: Angular velocity
- `energy`: Energy level (0-100)
- `health`: Health level
- `age`: Age in simulation steps
- `isMoving`, `isTurning`: State flags

#### `ActionResult`
Result of executing an action.

**Properties:**
- `reward`: Reward value
- `success`: Whether action succeeded
- `message`: Additional message

#### `SensoryPercept`
Container for all sensory inputs.

**Methods:**
- `getVision()`, `setVision(vision)` - Vision input
- `getTouch()`, `setTouch(touch)` - Touch input
- `getInternal()`, `setInternal(internal)` - Internal signals
- `getProprioception()`, `setProprioception(proprioception)` - Proprioception
- `getAudio()`, `setAudio(audio)` - Audio input
- `getAllSignals()` - Get all signals

#### `SimpleWorld`
Simulated 2D world.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure dimensions
- `setAgentStart(x, y)` - Set starting position
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `getSensoryPercept()` - Get current perception
- `getAgentBody()` - Get agent state
- `applyMotorCommand(cmd, time)` - Apply action
- `addObject(obj)` - Add object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check position validity

**Convenience methods:**
- `addResource(obj)`, `addHazard(obj)`, `addWall(obj)`

#### `Brain`
Central neural simulation brain.

**Methods:**
- `__init__(config)` - Initialize with config
- `initialize()` - Initialize brain
- `step(currentStep, currentTime)` - Advance simulation
- `receiveSensoryInput(input)` - Process sensory input
- `injectCurrent(neuron, current)` - Inject current into neuron
- `injectCurrentToNeurons(type, current)` - Inject to all neurons of type
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)`, `load(filepath)` - Checkpointing
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region
- `getTotalNeuronCount()` - Get neuron count
- `getTotalSynapseCount()` - Get synapse count
- `getAverageFiringRate()` - Get firing rate
- `getTotalSpikeCount()` - Get total spikes
- `getDevelopmentalStage()` - Get current stage
- `setDevelopmentalStage(stage)` - Set stage

**Memory system access:**
- `getWorkingMemory()`, `getEpisodicMemory()`, `getSemanticMemory()`, `getProceduralMemory()`, `getAssociativeMemory()`

**Neuromodulation access:**
- `getDopamine()`, `getCuriosity()`, `getNovelty()`, `getPredictionError()`

**Cognition access:**
- `getPlanner()`, `getConceptFormation()`, `getAttention()`

**Development access:**
- `getDevelopmentSystem()`

#### `AgentBrain`
Agent interface connecting brain to world.

**Methods:**
- `__init__(brain)` - Initialize with brain
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()`, `getMotorOutputSize()` - Get sizes
- `processSensoryInput(percept)` - Process input
- `decodeMotorCommand()` - Generate action
- `applyRewardModulation(reward, predictedReward)` - Apply learning
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()` - Get stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()`, `getNoveltyLevel()`, `getPredictionError()` - Get levels
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain

**Configuration methods:**
- `enableRewardModulation(enable)`, `enableStructuralPlasticity(enable)`, `enableDevelopment(enable)`, `enableCuriosity(enable)`
- `isRewardModulationEnabled()`, `isStructuralPlasticityEnabled()`, etc.

### Exceptions

#### `NLMException`
Base exception for all NLM errors.

#### `ConfigurationError`
Raised for configuration-related errors.

#### `SimulationError`
Raised for simulation-related errors.

#### `MemoryError`
Raised for memory system errors.

**Example:**
```python
try:
    brain = pynlm.createBrain("invalid_config.json")
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### Utility Functions

#### `pynlm.normalizeVector(data, min=0.0, max=1.0)`
Normalize vector values to specified range.

#### `pynlm.vectorToString(data, delimiter=", ")`
Convert vector to string representation.

#### `pynlm.validateConfig(config)`
Validate configuration object.

#### `pynlm.isPositiveNumber(value)`
Check if value is positive.

#### `pynlm.isPositiveInt(value)`
Check if integer is positive.

### Constants

#### Module-level constants:
- `pynlm.INVALID_NEURON_ID` - Invalid neuron ID
- `pynlm.INVALID_SYNAPSE_ID` - Invalid synapse ID
- `pynlm.INVALID_REGION_ID` - Invalid region ID
- `pynlm.INVALID_POPULATION_ID` - Invalid population ID

#### Performance constants:
- `pynlm.DEFAULT_SIMULATION_TIMESTEP` - 0.001 seconds
- `pynlm.DEFAULT_RANDOM_SEED` - 42
- `pynlm.DEFAULT_NEURON_COUNT` - 100
- `pynlm.DEFAULT_REGION_COUNT` - 1

### NeuronType Enum
- `Excitatory`, `Inhibitory`, `Modulatory`
- `Sensory`, `Motor`, `Internal`

### SynapseType Enum
- `Excitatory`, `Inhibitory`, `Modulatory`
- `Electrical`, `GapJunction`

### DevelopmentalStage Enum
- `Initial`, `CriticalPeriod`, `Maturation`
- `Adult`, `Aging`

### FiringState Enum
- `Resting`, `Active`, `Refractory`, `Inhibited`

### ActionType Enum
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

### MotorCommand Enum
- `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`, `Interact`, `Wait`

### WorldObjectType Enum
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

## Advanced Usage

### Custom Agent with State Tracking

```python
import pynlm

class StatefulAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.world = world
        self.agent.initialize(world)
        self.history = []
    
    def step(self, step):
        # Store state before action
        state_before = {
            'position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'energy': self.world.getAgentBody().energy,
            'curiosity': self.agent.getCuriosityLevel(),
            'spikes': self.agent.getBrain().getTotalSpikeCount()
        }
        
        # Get sensory input and process
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Let brain think
        self.agent.getBrain().step(step, step * 0.001)
        
        # Get and apply action
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, step * 0.001)
        
        # Store state after action
        state_after = {
            'position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'energy': self.world.getAgentBody().energy,
            'curiosity': self.agent.getCuriosityLevel(),
            'spikes': self.agent.getBrain().getTotalSpikeCount()
        }
        
        self.history.append((state_before, state_after))
        return action
```

### Multi-Agent Simulation

```python
import pynlm
from threading import Thread

class MultiAgentSimulator:
    def __init__(self, world_size=20):
        self.world = pynlm.createWorld(world_size, world_size)
        self.agents = []
    
    def add_agent(self):
        brain = pynlm.createBrain()
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        self.agents.append(agent)
    
    def run_step(self, step):
        for i, agent in enumerate(self.agents):
            # Different starting positions for each agent
            body = self.world.getAgentBody()
            if body.x == 0 and body.y == 0:
                # Find a good starting position
                pass
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Step brain
            agent.getBrain().step(step, step * 0.001)
            
            # Simple agent behavior
            action = self.simple_behavior(agent, percept)
            self.world.applyMotorCommand(action, step * 0.001)
    
    def simple_behavior(self, agent, percept):
        # Simple random exploration
        import random
        actions = list(pynlm.MotorCommand)
        return random.choice(actions)
```

### Learning with Evaluation

```python
import pynlm
import time

class LearningEvaluator:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.metrics = []
    
    def run_episode(self, episode_num, max_steps=100):
        print(f"Episode {episode_num}")
        
        # Record initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getAverageFiringRate()
        
        # Run episode
        for step in range(max_steps):
            # Get observation
            percept = self.world.getSensoryPercept()
            
            # Process and think
            self.agent.processSensoryInput(percept)
            self.brain.step(step, step * 0.001)
            
            # Simple action selection
            action = self.select_action(self.agent, percept)
            result = self.world.applyMotorCommand(action, step * 0.001)
            
            # Apply reward
            self.agent.applyRewardModulation(result.reward, 0.0)
            
            # Log metrics
            if step % 20 == 0:
                self.log_step(step, initial_spikes, initial_firing)
        
        # Calculate episode statistics
        final_spikes = self.brain.getTotalSpikeCount()
        final_firing = self.brain.getAverageFiringRate()
        
        stats = {
            'episode': episode_num,
            'steps': max_steps,
            'initial_spikes': initial_spikes,
            'final_spikes': final_spikes,
            'spike_change': final_spikes - initial_spikes,
            'initial_firing': initial_firing,
            'final_firing': final_firing,
            'firing_change': final_firing - initial_firing,
            'reward': sum(self.metrics[-1]['reward'] for _ in range(max_steps // 20)),
            'curiosity': self.agent.getCuriosityLevel(),
            'time': time.time()
        }
        
        self.metrics.append(stats)
        return stats
    
    def select_action(self, agent, percept):
        # Simple epsilon-greedy action selection
        import random
        if random.random() < 0.1:  # Explore
            return random.choice(list(pynlm.MotorCommand))
        else:  # Exploit
            return agent.decodeMotorCommand()
    
    def get_summary(self):
        if not self.metrics:
            return "No metrics recorded"
        
        summary = "Learning Evaluation Summary:\n"
        summary += f"Episodes: {len(self.metrics)}\n"
        
        avg_spike_change = sum(m['spike_change'] for m in self.metrics) / len(self.metrics)
        avg_firing_change = sum(m['firing_change'] for m in self.metrics) / len(self.metrics)
        
        summary += f"Average spike change: {avg_spike_change:.2f}\n"
        summary += f"Average firing rate change: {avg_firing_change:.4f}\n"
        
        return summary
```

## Performance Tips

1. **Memory Efficiency**: Use `getDataPtr()` for large arrays to avoid copying
2. **Batching**: Process multiple simulation steps before visualization
3. **Configuration**: Pre-load configurations for repeated simulations
4. **Checkpointing**: Use `save()` and `load()` for long-running experiments
5. **Thread Safety**: Each agent should have its own brain instance

## Troubleshooting

### Common Issues

1. **ImportError: cannot import name 'pynlm'**
   - Ensure the package is installed: `pip install -e .`
   - Check that you're in the correct directory

2. **RuntimeError: Couldn't load backend** (missing OpenMP/CUDA)
   - Install required dependencies for your platform
   - Check system requirements

3. **Memory errors in large simulations**
   - Use `pynlm.createBrain()` with smaller neuron counts
   - Process simulation in chunks
   - Monitor memory usage

4. **Python bindings not found**
   - Rebuild with Python support: `python setup.py build_ext --inplace`
   - Check that pybind11 is properly installed

### Debugging

```python
import pynlm
import traceback

# Enable debug logging
pynlm.setDebugLevel(1)

try:
    brain = pynlm.createBrain()
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
    traceback.print_exc()
```

## License and Acknowledgements

This project is part of the NLM (熙然) experimental artificial developmental brain project.

License: MIT

Dependencies:
- C++20 standard library
- pybind11 for Python bindings
- Custom NLM neural simulation framework

## Support and Community

For questions, issues, or contributions:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Community discussions

## Future Enhancements

Planned Python API improvements:
1. Enhanced visualization tools
2. Advanced debugging utilities
3. Multi-threading support
4. Performance profiling tools
5. Machine learning integration
6. Web API bindings