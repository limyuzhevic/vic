# NLM Python API Reference

## Overview

This document provides comprehensive reference documentation for the NLM Python API (`pynlm` module). The module provides bindings to the C++ NLM (Neural Learning Machine) neural simulation framework.

## Module Structure

### Core Classes

#### Config
Configuration class for NLM system settings.

**Methods:**
- `__init__()` - Create empty configuration
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(args)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary string

**Usage Examples:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

#### Brain
Central neural simulation brain class.

**Methods:**
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize the brain with configuration
- `step(currentStep)` - Perform a simulation step
- `step(currentStep, currentTime)` - Perform a simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `injectCurrent(neuron, current)` - Inject current into a specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current into all neurons of a specific type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name="")` - Add a new neural region
- `getRegion(id)` - Get a region by ID
- `getRegionCount()` - Get the number of regions
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count across all regions
- `getTotalSynapseCount()` - Get total synapse count across all regions
- `getActiveNeuronCount()` - Get count of active neurons
- `getFiringNeuronCount()` - Get count of currently firing neurons
- `getAverageFiringRate()` - Get average firing rate across all neurons
- `getExcitationInhibitionRatio()` - Get excitation/inhibition balance ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get current developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get the configuration
- `logStatus()` - Log brain status

**Usage Examples:**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### AgentBrain
Agent brain interface connecting NLM brain to world.

**Methods:**
- `__init__(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain motor activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get current developmental stage
- `getNeuromodulationLevel()` - Get current neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` - Get the underlying brain
- `enableRewardModulation(enable)` - Enable reward modulation
- `enableStructuralPlasticity(enable)` - Enable structural plasticity
- `enableDevelopment(enable)` - Enable development
- `enableCuriosity(enable)` - Enable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation is enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity is enabled
- `isDevelopmentEnabled()` - Check if development is enabled
- `isCuriosityEnabled()` - Check if curiosity is enabled

**Usage Examples:**
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**
- `__init__()` - Create new simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world by timestep seconds
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get maximum energy
- `setMaxEnergy(e)` - Set maximum energy
- `getEnergyDecayRate()` - Get energy decay rate
- `setEnergyDecayRate(r)` - Set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed)` - Set random seed
- `getRandomSeed()` - Get random seed

**Usage Examples:**
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)

# Get percept
percept = world.getSensoryPercept()

# Apply motor command
action = pynlm.Action(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action, world.getSimulationTime())
```

#### SensoryInput
Base class for sensory input.

**Methods:**
- `getType()` - Get the type of sensory input
- `getData()` - Get the raw data as a vector
- `getDimensions()` - Get the dimensionality
- `getTimestamp()` - Get the timestamp
- `setTimestamp(timestamp)` - Set the timestamp

#### Vision
Vision sensory input.

**Methods:**
- `__init__()` - Create empty vision
- `__init__(width, height, channels=3)` - Create vision with dimensions
- `setData(data)` - Set vision data
- `getWidth()` - Get width
- `getHeight()` - Get height
- `getChannels()` - Get channels

#### Audio
Audio sensory input.

**Methods:**
- `__init__()` - Create empty audio
- `__init__(sampleRate, numSamples)` - Create audio with specifications
- `setData(data)` - Set audio data
- `setSampleRate(sampleRate)` - Set sample rate
- `getSampleRate()` - Get sample rate
- `getNumSamples()` - Get number of samples

#### InternalSignals
Internal signals sensory input.

**Methods:**
- `__init__()` - Create empty internal signals
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals

#### Action
Action representation for motor output.

**Methods:**
- `__init__()` - Create empty action
- `__init__(type)` - Create action with type
- `__init__(type, parameters)` - Create action with type and parameters
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get action name
- `clone()` - Clone action

#### WorldObject
World object representation.

**Properties:**
- `x` - X position
- `y` - Y position
- `radius` - Radius
- `type` - World object type
- `value` - Object value
- `active` - Active state

#### AgentBody
Agent body state.

**Properties:**
- `x` - X position
- `y` - Y position
- `orientation` - Orientation angle
- `velocityX` - X velocity
- `velocityY` - Y velocity
- `angularVelocity` - Angular velocity
- `energy` - Energy level
- `health` - Health level
- `age` - Age
- `isMoving` - Moving state
- `isTurning` - Turning state
- `lastActionTime` - Last action time

#### ActionResult
Action result from world.

**Properties:**
- `reward` - Reward value
- `success` - Success state
- `message` - Result message

#### SensoryPercept
Sensory percept data.

**Methods:**
- `getVision()` - Get vision data
- `setVision(vision)` - Set vision data
- `getVisionWidth()` - Get vision width
- `getVisionHeight()` - Get vision height
- `getTouch()` - Get touch data
- `setTouch(touch)` - Set touch data
- `getInternal()` - Get internal signals
- `setInternal(internal)` - Set internal signals
- `getProprioception()` - Get proprioception data
- `setProprioception(proprioception)` - Set proprioception data
- `getAudio()` - Get audio data
- `setAudio(audio)` - Set audio data
- `getAllSignals()` - Get all signals
- `getTimestamp()` - Get timestamp
- `setTimestamp(timestamp)` - Set timestamp

### Factory Functions

#### createDefaultConfig()
Create a default configuration.

#### createBrain(config)
Create a new brain with configuration.

#### createSimpleWorld()
Create a new simple world.

#### createAgentBrain(brain)
Create a new agent brain interface.

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

#### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction

#### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

#### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

#### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

### Constants

#### INVALID_NEURON_ID
Invalid neuron ID constant.

#### INVALID_SYNAPSE_ID
Invalid synapse ID constant.

#### INVALID_REGION_ID
Invalid region ID constant.

#### INVALID_POPULATION_ID
Invalid population ID constant.

## Advanced Usage Patterns

### 1. Configuration Management

```python
import pynlm
import json

class NLMConfigurator:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def load_from_file(self, filepath):
        """Load configuration from file."""
        self.config.loadFromFile(filepath)
    
    def save_to_file(self, filepath):
        """Save configuration to file."""
        self.config.saveToFile(filepath)
    
    def get_brain_config(self):
        """Get brain-specific configuration keys."""
        keys = self.config.getKeys()
        return [k for k in keys if k.startswith("brain.")]
    
    def validate_brain_config(self):
        """Validate brain configuration."""
        required_keys = ["brain.neuron_count", "brain.synapse_density"]
        for key in required_keys:
            if not self.config.has(key):
                raise ValueError(f"Missing required configuration: {key}")

# Usage
config_manager = NLMConfigurator()
config_manager.load_from_file("config.json")
print(config_manager.get_brain_config())
```

### 2. Agent Lifecycle Management

```python
import pynlm
import time

class NLMEnvironment:
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
        
        self.agent = None
        self.current_episode = 0
    
    def create_agent(self, brain=None):
        """Create or recreate agent."""
        if brain is None:
            brain = pynlm.createBrain(self.config)
            brain.initialize()
        
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(self.world)
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        return self.agent
    
    def run_episode(self, max_steps=1000):
        """Run a single episode."""
        self.current_episode += 1
        step = 0
        
        while step < max_steps:
            # Update world
            self.world.update(0.1)
            
            # Get and process percept
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.agent.getBrain().step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            step += 1
        
        return step
    
    def reset_environment(self):
        """Reset environment for new episode."""
        self.world.reset()
        self.agent.reset()
    
    def get_stats(self):
        """Get current environment statistics."""
        brain = self.agent.getBrain()
        return {
            "episode": self.current_episode,
            "neurons": brain.getTotalNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "development_stage": brain.getDevelopmentalStage().name,
        }

# Usage
env = NLMEnvironment()
stats = env.run_episode()
print(env.get_stats())
```

### 3. Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self):
        self.metrics = []
        self.start_time = None
    
    def start(self):
        """Start monitoring."""
        self.start_time = time.time()
        self.metrics = []
    
    def record(self, brain, agent, world, step):
        """Record current performance metrics."""
        metric = {
            "step": step,
            "timestamp": time.time() - self.start_time,
            "neurons": brain.getTotalNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "energy": world.getAgentBody().energy,
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "development_stage": brain.getDevelopmentalStage().name,
        }
        self.metrics.append(metric)
    
    def get_summary(self):
        """Get performance summary."""
        if not self.metrics:
            return None
        
        total_spikes = sum(m["spikes"] for m in self.metrics)
        total_steps = len(self.metrics)
        time_elapsed = self.metrics[-1]["timestamp"] if self.metrics else 0
        
        return {
            "total_time": time_elapsed,
            "total_steps": total_steps,
            "spikes_per_second": total_spikes / time_elapsed if time_elapsed > 0 else 0,
            "avg_firing_rate": sum(m["firing_rate"] for m in self.metrics) / total_steps,
            "peak_curiosity": max(m["curiosity"] for m in self.metrics),
            "final_development_stage": self.metrics[-1]["development_stage"] if self.metrics else None,
        }
    
    def save_metrics(self, filepath):
        """Save metrics to file."""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.metrics, f, indent=2)

# Usage
monitor = PerformanceMonitor()
monitor.start()

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    monitor.record(brain, agent, world, step)

# Get performance summary
summary = monitor.get_summary()
print(f"Performance: {summary}")
```

## Error Handling Best Practices

### 1. Configuration Validation

```python
def safe_create_brain(config_settings=None):
    """Safely create brain with configuration validation."""
    try:
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Apply custom settings if provided
        if config_settings:
            for key, value in config_settings.items():
                config.set(key, value)
        
        # Validate critical settings
        if not config.has("brain.neuron_count") or config.get("brain.neuron_count") <= 0:
            raise ValueError("Invalid neuron count")
        
        if not config.has("brain.synapse_density") or not (0 <= config.get("brain.synapse_density") <= 1):
            raise ValueError("Invalid synapse density")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
        
    except Exception as e:
        print(f"Error creating brain: {e}")
        # Return a minimal working brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain

# Usage
brain = safe_create_brain({"brain.neuron_count": 1000})
```

### 2. Agent Safety

```python
def safe_agent_operations(agent, world, percept):
    """Safely perform agent operations with error handling."""
    try:
        # Check if agent is initialized
        if not agent:
            raise RuntimeError("Agent not initialized")
        
        # Check if world is available
        if not world:
            raise RuntimeError("World not available")
        
        # Process sensory input
        if percept:
            agent.processSensoryInput(percept)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action if valid
        if action:
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return action
        
    except Exception as e:
        print(f"Error in agent operations: {e}")
        # Return safe default action
        return pynlm.Action(pynlm.ActionType.Wait)

# Usage
action = safe_agent_operations(agent, world, percept)
```

### 3. File Operation Safety

```python
def safe_save_load_operations(filepath, operation, data=None):
    """Safely perform save/load operations."""
    import os
    
    try:
        # Check if file exists for read operations
        if operation == "load" and not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Perform operation
        if operation == "save" and data:
            data.save(filepath)
            print(f"Saved to {filepath}")
        
        elif operation == "load":
            loaded_data = type(data)()
            loaded_data.load(filepath)
            print(f"Loaded from {filepath}")
            return loaded_data
        
    except Exception as e:
        print(f"Error in {operation} operation: {e}")
        return None

# Usage
saved_brain = safe_save_load_operations("brain_checkpoint.bin", "save", brain)
loaded_brain = safe_save_load_operations("brain_checkpoint.bin", "load", pynlm.Brain)
```

## Common Issues and Solutions

### 1. "Brain isn't doing anything"
**Cause:** Brain not properly initialized
**Solution:** Always call `brain.initialize()` before `brain.step()`

### 2. "Agent isn't moving"
**Cause:** Action not applied to world
**Solution:** Always call `world.applyMotorCommand()` after `agent.decodeMotorCommand()`

### 3. "Everything is 0"
**Cause:** Brain needs time to warm up
**Solution:** Increase number of steps or provide initial sensory input

### 4. "Performance is slow"
**Cause:** Too many neurons or too complex configuration
**Solution:** Use smaller configurations for testing, monitor neuron count

### 5. "Brain state not saving correctly"
**Cause:** Permission issues or uninitialized brain
**Solution:** Check write permissions, ensure brain is initialized before saving

## Best Practices

### 1. Always Initialize
- Always call `brain.initialize()` before using the brain
- Always call `agent.initialize(world)` before using agent
- Always call `world.reset()` before starting simulation

### 2. Use Context Managers
```python
class NLMContext:
    def __init__(self, config_settings=None):
        self.config = pynlm.createDefaultConfig()
        if config_settings:
            for key, value in config_settings.items():
                self.config.set(key, value)
        
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        self.create_components()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
    
    def create_components(self):
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def cleanup(self):
        # Clean up resources
        pass

# Usage
with NLMContext({"brain.neuron_count": 500}) as nlm:
    # Use components
    for step in range(100):
        nlm.world.update(0.1)
        nlm.agent.processSensoryInput(nlm.world.getSensoryPercept())
        nlm.brain.step(step)
```

### 3. Monitor and Log
- Always monitor brain statistics during simulation
- Log important events and metrics
- Implement error handling and recovery

### 4. Use Consistent Naming
- Use consistent naming for files and variables
- Follow the project's coding conventions
- Add comments for complex operations

## Migration Guide

### From C++ to Python
If you're migrating from C++ NLM code, note these differences:

| C++ | Python |
|-----|--------|
| `std::shared_ptr<Brain>` | `pynlm.createBrain(config)` |
| `Brain::step(step, time)` | `brain.step(step)` |
| `brain->getTotalNeuronCount()` | `brain.getTotalNeuronCount()` |
| Manual memory management | Automatic garbage collection |
| Exception handling | Python exceptions |

### From v0.1.0 to v0.2.0
Major changes in v0.2.0:
- Improved error handling and validation
- Better API consistency
- Enhanced documentation
- Performance optimizations
- New advanced examples

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [HOW_TO_USE.md](HOW_TO_USE.md)
- [NLM Scientific Papers](docs/SCIENCE.md)
- [Experiment Documentation](docs/EXPERIMENTS.md)

## Acknowledgments

This API reference is based on the NLM Phase 6 integration framework and reflects the current state of the Python bindings as of v0.1.0.