# NLM API Reference

This document provides comprehensive reference information for NLM's API, including:
- Core classes and their methods
- Configuration options
- Python bindings
- CLI commands

## Core Classes

### Brain Class

#### Creation and Initialization
```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Control
```python
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state
```

#### Input/Output
```python
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()            # Get motor action
```

#### Statistics
```python
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
```

#### State Management
```python
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

#### Regions
```python
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### Config Class

#### Creation
```python
config = pynlm.createDefaultConfig()
```

#### Query
```python
if config.has("brain.neuron_count"):
    keys = config.getKeys()
```

#### Modify
```python
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
```

#### File I/O
```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

#### Creation and Initialization
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing
```python
agent.processSensoryInput(percept)
```

#### Motor Decoding
```python
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation
```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development
```python
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

#### Subsystem Enable/Disable
```python
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
```

#### Statistics
```python
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

#### Creation
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### Control
```python
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

#### Agent Interaction
```python
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)
```

#### State Access
```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

#### World Objects
```python
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### Properties
```python
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

## Enumerations

### Neuron Types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
```

### World Object Types
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.0005 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.01 | Structural plasticity growth rate |
| `plasticity.structural.pruning_rate` | float | 0.005 | Structural plasticity pruning rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.scale` | float | 1.5 | Curiosity modulation scale |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.scale` | float | 1.2 | Novelty detection scale |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental stages |
| `development.critical_period_duration` | float | 500.0 | Duration of critical period (ms) |
| `development.maturation_rate` | float | 0.01 | Rate of maturation |

## Python API Functions

### Core Functions
```python
pynlm.__version__                    # Get NLM version
pynlm.createDefaultConfig()          # Create default configuration
pynlm.createBrain(config)            # Create brain
pynlm.createSimpleWorld()            # Create simple world
pynlm.createAgentBrain(brain)       # Create agent
pynlm.WorldObject(x, y, type, value) # Create world object
pynlm.NeuronType.Excitatory         # Neuron type enum
pynlm.ActionType.MoveForward        # Action type enum
```

### Configuration Functions
```python
config.getKeys()                    # Get all configuration keys
config.has(key)                     # Check if key exists
config.get(key)                     # Get configuration value
config.set(key, value)              # Set configuration value
config.loadFromFile(filename)       # Load configuration from file
config.saveToFile(filename)         # Save configuration to file
```

### Brain Functions
```python
brain.getTotalNeuronCount()         # Get total neuron count
brain.getTotalSynapseCount()        # Get total synapse count
brain.getFiringNeuronCount()        # Get firing neuron count
brain.getAverageFiringRate()        # Get average firing rate
brain.getExcitationInhibitionRatio()  # Get E/I ratio
brain.getDevelopmentalStage()      # Get developmental stage
brain.getCurrentTime()              # Get current simulation time
brain.save(filename)                # Save brain state
brain.load(filename)                # Load brain state
brain.reset()                       # Reset brain state
brain.step(step)                    # Run simulation step
brain.step(step, time)               # Run simulation step with time
brain.receiveSensoryInput(data)     # Receive sensory input
brain.produceAction()                # Produce motor action
```

### Agent Functions
```python
agent.getCuriosityLevel()           # Get curiosity level
agent.getNoveltyLevel()              # Get novelty level
agent.getPredictionError()           # Get prediction error
agent.getNeuromodulationLevel()      # Get neuromodulation level
agent.getCurrentReward()             # Get current reward
agent.decodeMotorCommand()          # Decode motor command
agent.processSensoryInput(percept)   # Process sensory input
agent.applyRewardModulation(reward, predicted)  # Apply reward modulation
agent.updateDevelopment(timestep)    # Update development
agent.enableRewardModulation(enabled)  # Enable/disable reward modulation
agent.enableStructuralPlasticity(enabled)  # Enable/disable structural plasticity
agent.enableDevelopment(enabled)    # Enable/disable development
agent.enableCuriosity(enabled)       # Enable/disable curiosity
```

### World Functions
```python
world.getWidth()                    # Get world width
world.getHeight()                   # Get world height
world.getAgentPosition()            # Get agent position
world.getSensoryPercept()           # Get sensory percept
world.getAgentBody()                # Get agent body
world.getSimulationTime()           # Get simulation time
world.getMaxEnergy()                # Get max energy
world.setMaxEnergy(value)           # Set max energy
world.update(timestep)               # Update world
world.reset()                       # Reset world
world.setAgentStart(x, y)           # Set agent start position
world.applyMotorCommand(action, time)  # Apply motor command
world.setRandomSeed(seed)           # Set random seed
world.isValidPosition(x, y)         # Check if position is valid
world.addObject(object)             # Add world object
world.removeObject(x, y)            # Remove world object
world.getDistanceToHazard(x, y)     # Get distance to hazard
```

## CLI Commands

### Core Commands
```bash
./nlm --help                         # Show help
./nlm --version                      # Show version
./nlm --run --steps N               # Run simulation
./nlm --config CONFIG_FILE          # Load configuration
./nlm --save CHECKPOINT_FILE         # Save brain state
./nlm --load CHECKPOINT_FILE         # Load brain state
```

### Advanced CLI Commands
```bash
./nlm --sweep-config CONFIG_FILE --output OUTPUT_FILE --steps N  # Parameter sweep
./nlm --load-config CONFIG_FILE --profile PROFILE --save-config OUTPUT_FILE  # Configuration management
./nlm --compare-results FILE1 FILE2 FILE3 --output COMPARISON_FILE --metrics METRICS  # Result comparison
./nlm --create-batch BATCH_CONFIG --output-dir OUTPUT_DIR --parallel  # Batch processing
./nlm --help-advanced                    # Show advanced help
```

## File Formats

### Configuration Files

#### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true
        }
    }
}
```

#### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  v_thresh: -50.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    scale: 1.5
  novelty:
    enable: true
    scale: 1.2
```

### Checkpoint Files

#### Binary Checkpoint Format
NLM checkpoint files are binary files that contain:
- Brain state (neuron membrane potentials, synaptic weights)
- Simulation state (time, step count)
- Configuration (brain parameters, plasticity settings)

#### Loading/Saving
```python
brain.save("checkpoint.nlm")  # Save to binary format
brain.load("checkpoint.nlm")  # Load from binary format
```

## Error Handling

### Common Errors

| Error | Description | Solution |
|-------|-------------|----------|
| "Brain not initialized" | Brain not initialized before use | Call `brain.initialize()` first |
| "Invalid configuration" | Invalid configuration key or value | Check configuration keys and values |
| "File not found" | Configuration or checkpoint file not found | Verify file paths and permissions |
| "Memory allocation failed" | Out of memory | Reduce simulation size or increase system resources |
| "Invalid action" | Invalid motor command | Check action type and value |

### Error Handling Patterns
```python
try:
    brain.initialize()
    brain.step(0)
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

## Performance Considerations

### Memory Optimization
- Use appropriate neuron counts for simulation size
- Configure synaptic density to balance connectivity and performance
- Use memory pools for large simulations

### Performance Optimization
- Enable parallel processing for large simulations
- Use SIMD optimizations for neural computations
- Configure appropriate timesteps for balance between accuracy and speed

### File I/O Optimization
- Use binary formats for checkpoint files
- Compress large result files
- Use efficient serialization for configuration files

## Conclusion

This API reference provides comprehensive documentation for NLM's API, including:

- Core classes and their methods
- Configuration options
- Python bindings
- CLI commands
- File formats
- Error handling
- Performance considerations

By following this documentation, users can effectively use NLM's API to create and run neural network simulations, experiments, and applications.