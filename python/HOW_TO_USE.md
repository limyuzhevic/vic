# Quick Reference for Python API

## Basic Classes

### Config
- `createDefaultConfig()` - Create empty configuration
- `createConfigFromTemplate(name)` - Create from template
- `createConfigFromDict(dict)` - Create from Python dictionary
- `validate()` - Validate configuration
- `has(key)` - Check if key exists
- `getFloat(key, default)` - Get float value
- `getInt(key, default)` - Get integer value
- `getBool(key, default)` - Get boolean value
- `set(key, value)` - Set value

### Brain
- `createBrain(config)` - Create and initialize brain
- `initialize()` - Initialize brain
- `step(step)` - Simulation step
- `getTotalNeuronCount()` - Get neuron count
- `getTotalSynapseCount()` - Get synapse count
- `getFiringNeuronCount()` - Get firing neurons
- `getAverageFiringRate()` - Get firing rate
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

### SimpleWorld
- `createWorld(width, height, visionWidth, visionHeight)` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `update(time)` - Update world
- `getSensoryPercept()` - Get percept
- `applyMotorCommand(cmd, time)` - Apply action

### AgentBrain
- `createAgentSystem(config, world)` - Create agent system
- `initialize(world)` - Initialize agent
- `processSensoryInput(percept)` - Process input
- `decodeMotorCommand()` - Get action
- `applyRewardModulation(reward, predicted)` - Learn from reward
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableCuriosity(bool)` - Enable/disable curiosity

## Advanced Functions

### Batch Operations
- `batchUpdateAgents(agents, percepts)` - Process multiple agents
- `batchInjectCurrent(brain, types, current)` - Inject to multiple types

### Performance
- `runSimulation(brain, world, steps, timestep)` - Run complete simulation
- `profileSimulation(brain, world, steps, timestep)` - Profile performance
- `stepSimulation(brain, world, steps, timestep)` - Run steps

### Multi-Agent
- `createMultiAgentSystem(configs, world)` - Create multiple agents
- `getSystemStatus(agents, world)` - Get system status

### Checkpoints
- `createCheckpoint(brain, name, filepath)` - Create checkpoint
- `restoreCheckpoint(brain, filepath)` - Restore checkpoint
- `listCheckpoints(directory)` - List checkpoints

### Configuration
- `mergeConfigs(base, override)` - Merge configurations
- `getConfigSchema()` - Get configuration schema

## Error Handling

- `withErrorHandling(func, operationName)` - Execute with error handling
- `safeExecute(func, operationName, fallback)` - Execute safely with fallback
- `getLastError()` - Get last error

## Templates

### Brain Templates
- `brainTemplate("simple", neuronCount)` - Create simple brain
- `brainTemplate("complex", neuronCount)` - Create complex brain

### World Templates
- `worldPreset("maze")` - Create maze world
- `worldPreset("open")` - Create open world

### Config Templates
- `createConfigFromTemplate("default")` - Default configuration
- `createConfigFromTemplate("explorer")` - Explorer configuration
- `createConfigFromTemplate("planner")` - Planner configuration
- `createConfigFromTemplate("minimal")` - Minimal configuration

## Console Script

After installation, you can use the `nlm` console script:

```bash
nlm --help           # Show help
nlm --demo           # Run demonstration
nlm --brain [n]      # Create brain with n neurons (default: 1000)
```

## Examples

### Basic Simulation
```python
import pynlm

config = pynlm.createConfigFromTemplate("explorer")
brain = pynlm.createValidatedBrain(config)

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = pynlm.createAgentSystem(config, world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Batch Processing
```python
import pynlm

# Create multiple agents
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
agent1 = pynlm.createAgentSystem(config, world)
agent2 = pynlm.createAgentSystem(config, world)

agents = [agent1, agent2]
percepts = [world.getSensoryPercept(), world.getSensoryPercept()]

# Process all agents at once
actions = pynlm.batchUpdateAgents(agents, percepts)

# Apply rewards
rewards = [1.0, 0.5]
pynlm.batchApplyRewards(agents, rewards)
```

### Performance Profiling
```python
import pynlm

config = pynlm.createConfigFromTemplate("complex")
brain = pynlm.createValidatedBrain(config)
world = pynlm.createSimpleWorld()

profile = pynlm.profileSimulation(brain, world, 1000)
print(f"Simulation took {profile['totalTimeMs']} ms")
print(f"Performance: {profile['stepsPerSecond']} steps/sec")
```

### Checkpointing
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save checkpoint
checkpoint_path = pynlm.createCheckpoint(brain, "my_simulation")
print(f"Checkpoint saved to: {checkpoint_path}")

# Restore checkpoint
brain.load(checkpoint_path)
```

## Version Information

- `pynlm.__version__` - Python package version
- C++ extensions require NLM library version 0.1.0+

## Testing

```bash
pytest tests/
```

## Dependencies

- Python >= 3.8
- numpy >= 1.20 (optional, for extended features)
- scikit-build-core
- pybind11
