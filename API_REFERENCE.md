# NLM Python API - Comprehensive Reference

This document provides a complete reference for the NLM Python API, including:
- Core classes and their methods
- Advanced usage patterns
- Performance optimization techniques
- Debugging and troubleshooting
- Example implementations

## Overview

The NLM Python API provides Python bindings for the Neural Learning Machine (NLM) C++ framework. The API exposes the complete functionality of the artificial brain simulation, including:

- **Brain Simulation**: Core neural computation with LIF neurons and event-driven dynamics
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Cognition**: Planning, concept formation, and attentional selection
- **Development**: Developmental stages and structural plasticity
- **Environment**: World simulation with vision, touch, and other sensory modalities
- **Agent Interface**: Integration between brain and environment

## Core API Reference

### 1. Configuration Class

The `Config` class manages brain configuration parameters.

#### Creation
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Custom configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
```

#### Methods
| Method | Description | Example |
|--------|-------------|---------|
| `set(key, value, source)` | Set configuration value | `config.set("brain.neuron_count", 2000)` |
| `get(key)` | Get configuration value | `config.get("brain.neuron_count")` |
| `getOr(key, default)` | Get value or return default | `config.getOr("missing", 100)` |
| `has(key)` | Check if key exists | `config.has("brain.neuron_count")` |
| `loadFromFile(filepath)` | Load configuration from file | `config.loadFromFile("config.json")` |
| `saveToFile(filepath)` | Save configuration to file | `config.saveToFile("config.json")` |
| `clear()` | Clear all configuration entries | `config.clear()` |

#### Common Configuration Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |

### 2. Brain Class

The `Brain` class represents the artificial neural system.

#### Creation
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Methods
| Method | Description | Example |
|--------|-------------|---------|
| `initialize()` | Initialize brain with configuration | `brain.initialize()` |
| `step(step_number)` | Perform one simulation step | `brain.step(step)` |
| `step(step_number, timestamp)` | Perform step with timestamp | `brain.step(step, time)` |
| `receiveSensoryInput(input)` | Inject sensory data | `brain.receiveSensoryInput(visual_input)` |
| `injectCurrent(neuron, current)` | Inject current into specific neuron | `brain.injectCurrent(neuron_id, 5.0)` |
| `injectCurrentToNeurons(type, current)` | Inject current to all neurons of type | `brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)` |
| `produceAction()` | Generate motor action from neural activity | `action = brain.produceAction()` |
| `reset()` | Reset brain state | `brain.reset()` |
| `save(filepath)` | Save brain state to file | `brain.save("brain_checkpoint.bin")` |
| `load(filepath)` | Load brain state from file | `brain.load("brain_checkpoint.bin")` |
| `addRegion(name)` | Add new brain region | `region_id = brain.addRegion("cortex")` |
| `getRegion(id)` | Get region by ID | `region = brain.getRegion(region_id)` |
| `getTotalNeuronCount()` | Get total neuron count | `count = brain.getTotalNeuronCount()` |
| `getTotalSynapseCount()` | Get total synapse count | `count = brain.getTotalSynapseCount()` |
| `getFiringNeuronCount()` | Get currently firing neurons | `count = brain.getFiringNeuronCount()` |
| `getAverageFiringRate()` | Get average firing rate | `rate = brain.getAverageFiringRate()` |
| `getExcitationInhibitionRatio()` | Get E/I balance | `ratio = brain.getExcitationInhibitionRatio()` |
| `getTotalSpikeCount()` | Get total spike count | `count = brain.getTotalSpikeCount()` |
| `getDevelopmentalStage()` | Get current developmental stage | `stage = brain.getDevelopmentalStage()` |
| `setDevelopmentalStage(stage)` | Set developmental stage | `brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)` |
| `logStatus()` | Print brain status information | `brain.logStatus()` |

### 3. AgentBrain Class

The `AgentBrain` class connects the brain to the world, handling sensory and motor functions.

#### Creation
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Methods
| Method | Description | Example |
|--------|-------------|---------|
| `initialize(world)` | Initialize with world | `agent.initialize(world)` |
| `getSensoryInputSize()` | Get expected sensory input size | `size = agent.getSensoryInputSize()` |
| `getMotorOutputSize()` | Get expected motor output size | `size = agent.getMotorOutputSize()` |
| `processSensoryInput(percept)` | Process sensory percept and inject into brain | `agent.processSensoryInput(percept)` |
| `decodeMotorCommand()` | Decode brain activity into motor command | `action = agent.decodeMotorCommand()` |
| `applyRewardModulation(reward, predicted)` | Apply reward-based neuromodulation | `agent.applyRewardModulation(reward, predicted)` |
| `updateDevelopment(timestep)` | Update development system | `agent.updateDevelopment(timestep)` |
| `getCuriosityLevel()` | Get curiosity level | `curiosity = agent.getCuriosityLevel()` |
| `getNoveltyLevel()` | Get novelty level | `novelty = agent.getNoveltyLevel()` |
| `getNeuromodulationLevel()` | Get neuromodulation level | `dopamine = agent.getNeuromodulationLevel()` |
| `getPredictionError()` | Get prediction error | `error = agent.getPredictionError()` |
| `reset()` | Reset agent for new episode | `agent.reset()` |
| `enableRewardModulation(enable)` | Enable/disable reward modulation | `agent.enableRewardModulation(True)` |
| `enableCuriosity(enable)` | Enable/disable curiosity | `agent.enableCuriosity(True)` |
| `enableDevelopment(enable)` | Enable/disable development | `agent.enableDevelopment(True)` |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `agent.enableStructuralPlasticity(True)` |

### 4. SimpleWorld Class

The `SimpleWorld` class represents the simulated environment.

#### Creation
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
```

#### Methods
| Method | Description | Example |
|--------|-------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions | `world.configure(20, 20, 8, 8)` |
| `reset()` | Reset world to initial state | `world.reset()` |
| `setAgentStart(x, y)` | Set agent starting position | `world.setAgentStart(10.0, 10.0)` |
| `update(timestep)` | Update world state | `world.update(0.1)` |
| `applyMotorCommand(cmd, time)` | Apply motor command | `result = world.applyMotorCommand(action, time)` |
| `getSensoryPercept()` | Get current sensory percept | `percept = world.getSensoryPercept()` |
| `getAgentBody()` | Get agent body state | `body = world.getAgentBody()` |
| `addObject(obj)` | Add object to world | `world.addObject(world_obj)` |
| `isValidPosition(x, y)` | Check if position is valid | `valid = world.isValidPosition(x, y)` |
| `getWidth()`, `getHeight()` | Get world dimensions | `width = world.getWidth()` |

### 5. Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons  
- `Modulatory` - Neuromodulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `Interact` - Interact with object

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period for learning
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### 6. Sensory Input Classes

#### Vision
```python
vision = pynlm.Vision(width=8, height=8)
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0])
```

#### Audio
```python
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData([0.5, 0.6, 0.7, ...])
```

### 7. Action Class
```python
from pynlm import ActionType, Action

# Create action
action = Action(ActionType.MoveForward)
action.setType(ActionType.MoveForward)
action.setParameters([1.0, 0.5])  # Custom parameters

# Get action info
action_type = action.getType()
parameters = action.getParameters()
name = action.getName()
```

## Advanced Usage Patterns

### Complete Agent Simulation
```python
def run_complete_agent_simulation(steps=1000):
    """Run a complete agent simulation with all subsystems enabled"""
    import pynlm
    
    # 1. Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.15)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,
        visionWidth=10, visionHeight=10
    )
    world.reset()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all subsystems for advanced simulation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 5. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # 6. Return final statistics
    return {
        'total_reward': agent.getNeuromodulationLevel(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'development_stage': agent.getDevelopmentalStage()
    }

# Run the simulation
results = run_complete_agent_simulation(1000)
print(f"Simulation completed: {results}")
```

### Brain Configuration Management
```python
def create_custom_configuration():
    """Create and save a custom brain configuration"""
    import pynlm
    import json
    
    # Create base configuration
    config = pynlm.createDefaultConfig()
    
    # Modify parameters for specific use case
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    config.set("brain.synapse_density", 0.2)
    config.set("brain.connection_probability", 0.15)
    
    # Neuromodulation settings
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Development settings
    config.set("development.critical_period_multiplier", 2.0)
    config.set("development.maturation_rate", 0.8)
    
    # Save configuration
    config.saveToFile("custom_brain_config.json")
    
    # Load configuration
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("custom_brain_config.json")
    
    # Verify values
    neuron_count = new_config.getOr("brain.neuron_count", 0)
    print(f"Loaded neuron count: {neuron_count}")
    
    return config

# Use custom configuration
custom_config = create_custom_configuration()
brain = pynlm.createBrain(custom_config)
brain.initialize()
```

### Performance Monitoring
```python
def monitor_brain_performance(duration_seconds=60, output_file="performance.json"):
    """Monitor brain performance over time"""
    import pynlm
    import time
    import json
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable monitoring
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Monitoring data
    monitoring_data = []
    start_time = time.time()
    
    # Run monitoring
    steps_per_second = 10  # One step every 0.1 seconds
    total_steps = int(duration_seconds * steps_per_second)
    
    for step in range(total_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics every second
        if step % steps_per_second == 0:
            metric = {
                'step': step,
                'time': time.time() - start_time,
                'firing_neutrons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dopamine': agent.getNeuromodulationLevel(),
                'development_stage': agent.getDevelopmentalStage()
            }
            monitoring_data.append(metric)
    
    # Calculate summary
    if monitoring_data:
        avg_firing = sum(m['firing_neutrons'] for m in monitoring_data) / len(monitoring_data)
        avg_spikes = sum(m['total_spikes'] for m in monitoring_data) / len(monitoring_data)
        
        summary = {
            'duration': duration_seconds,
            'total_steps': total_steps,
            'steps_per_second': total_steps / duration_seconds,
            'average_firing_rate': avg_firing,
            'average_spike_rate': avg_spikes,
            'start_time': monitoring_data[0]['time'],
            'end_time': monitoring_data[-1]['time'],
            'monitoring_data': monitoring_data
        }
        
        # Save to file
        with open(output_file, 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Performance monitoring completed for {duration_seconds} seconds")
        print(f"Average firing rate: {avg_firing:.1f} neurons")
        print(f"Average spike rate: {avg_spikes:.1f} spikes/second")
        print(f"Data saved to {output_file}")
        
        return summary
    
    return None

# Run performance monitoring
performance = monitor_brain_performance(60, "brain_performance.json")
```

### Debugging and Validation
```python
def debug_brain_simulation(verbose=False):
    """Debug brain simulation and validate internal state"""
    import pynlm
    import json
    
    print("Starting brain simulation debug...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    for step in range(20):
        brain.step(step)
        
        if verbose:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            print(f"Step {step}: {firing} firing neurons, {total_spikes} total spikes")
    
    # Validate internal state
    print("Validating internal state...")
    regions = brain.getRegions()
    print(f"Found {len(regions)} brain regions")
    
    total_neurons = sum(region.getTotalNeuronCount() for region in regions)
    total_synapses = sum(region.getSynapseCount() for region in regions)
    
    print(f"Validation results:")
    print(f"  - Total neurons: {total_neurons} (expected: {brain.getTotalNeuronCount()})")
    print(f"  - Total synapses: {total_synapses} (expected: {brain.getTotalSynapseCount()})")
    
    if total_neurons != brain.getTotalNeuronCount():
        print(f"  - WARNING: Neuron count mismatch!")
    
    if total_synapses != brain.getTotalSynapseCount():
        print(f"  - WARNING: Synapse count mismatch!")
    
    # Generate debug output
    debug_data = {
        'brain_config': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount()
        },
        'regions': []
    }
    
    for region in regions:
        region_info = {
            'id': region.getId().index(),
            'name': region.getName(),
            'neuron_count': region.getTotalNeuronCount(),
            'synapse_count': region.getSynapseCount()
        }
        debug_data['regions'].append(region_info)
    
    # Save debug data
    debug_file = "brain_debug.json"
    with open(debug_file, 'w') as f:
        json.dump(debug_data, f, indent=2)
    
    print(f"Debug data saved to {debug_file}")
    print("Debug simulation completed!")
    
    return debug_data

# Run debugging
debug_results = debug_brain_simulation(verbose=True)
```

## Performance Optimization Tips

### 1. Configuration Optimization
- Use smaller brain sizes for testing
- Adjust connection probability based on network size
- Enable/disable subsystems as needed

### 2. Memory Management
- Use `reset()` method to clear state instead of creating new objects
- Monitor memory usage with performance monitoring
- Consider checkpointing for long simulations

### 3. Simulation Efficiency
- Run multiple steps in batch when possible
- Use appropriate simulation timestep
- Monitor and adjust based on performance metrics

### 4. Advanced Configuration Options
```python
# Advanced configuration for large-scale simulations
config.set("brain.neuron_count", 5000)  # Large brain
config.set("brain.region_count", 8)     # Multiple regions
config.set("simulation_timestep", 0.01) # Faster simulation
config.set("checkpoint_dir", "./checkpoints")  # Regular checkpointing
```

## Troubleshooting Guide

### Common Issues and Solutions

#### 1. "Brain not initialized"
```python
# Solution: Always call initialize() before using brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Critical!
brain.step(0)  # Now safe to use
```

#### 2. "Agent has no world"
```python
# Solution: Initialize agent with world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Critical!
agent.processSensoryInput(percept)  # Now safe
```

#### 3. "Performance too slow"
```python
# Solution: Optimize configuration and monitoring
config.set("brain.neuron_count", 500)  # Reduce for speed
config.set("simulation_timestep", 0.01)  # Increase timestep
# Enable performance monitoring
```

#### 4. "Configuration not loading"
```python
# Solution: Check file path and format
config = pynlm.createDefaultConfig()
result = config.loadFromFile("valid_config.json")  # Should return True
if not result:
    print("Failed to load configuration")
```

### Error Handling Best Practices
```python
def safe_simulation_run(steps=100):
    """Run simulation with error handling"""
    import pynlm
    
    try:
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        # Run simulation
        for step in range(steps):
            try:
                brain.step(step)
            except Exception as e:
                print(f"Error at step {step}: {e}")
                break
        
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return False

# Run safe simulation
success = safe_simulation_run(100)
if success:
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## Examples Directory

The examples directory contains various implementations:

### 1. Basic Examples
- `basic_brain.py` - Simple brain creation and simulation
- `agent_basic.py` - Basic agent simulation
- `world_basic.py` - World simulation examples

### 2. Advanced Examples
- `agent_advanced.py` - Advanced agent with all subsystems
- `performance_monitor.py` - Performance monitoring implementation
- `brain_checkpointing.py` - Brain state checkpointing
- `development_simulation.py` - Development stage simulation

### 3. Complete Workflows
- `learning_loop.py` - Complete learning environment
- `simulation_suite.py` - Suite of different simulations
- `benchmark.py` - Performance benchmarks

## Installation and Setup

### Prerequisites
```bash
# Install Python dependencies
pip install pynlm numpy matplotlib

# Build and install NLM C++ library (if needed)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

### Basic Usage
```bash
# Run a simple simulation
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(10):
    brain.step(step)
print(f'Final firing rate: {brain.getAverageFiringRate()}')
"

# Run with agent
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print('Agent simulation completed!')
"
```

### Command Line Interface

The NLM CLI provides additional tools:
```bash
# Install CLI
touch /workspace/app/bin/nlm-cli
chmod +x /workspace/app/bin/nlm-cli

# Run commands
nlm-cli run --steps 100
nlm-cli run-agent --steps 500
nlm-cli config get brain.neuron_count
nlm-cli test --quick
nlm-cli test-integration
```

## API Evolution and Backward Compatibility

### Version Notes
- **Version 1.0**: Initial release with basic functionality
- **Version 1.1**: Added advanced subsystems (curiosity, development)
- **Version 1.2**: Enhanced error handling and performance monitoring
- **Version 1.3**: Added serialization and checkpointing
- **Version 1.4**: Improved documentation and examples

### Backward Compatibility
- All existing code should continue to work
- New features are additive, not breaking
- Deprecated methods are marked for future removal
- Enhanced error handling provides better debugging

### Migration Guide
For users upgrading from older versions:

```python
# Old code (should still work)
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# New code (with additional features)
import pynlm

# Create configuration with modern API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain with enhanced error handling
brain = pynlm.createBrain(config)
if not brain.initialize():
    raise RuntimeError("Brain initialization failed")

# Use agent with all subsystems enabled
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

## Future Enhancements

### Planned Features
1. **Enhanced Visualization**: Interactive brain visualization
2. **Cloud Integration**: Distributed simulation capabilities
3. **Machine Learning Integration**: Learning from external data
4. **Real-time Control**: Interactive control of brain parameters
5. **Multi-agent Systems**: Simulation of multiple interacting agents

### Research Areas
1. **Neuromodulation Modeling**: Enhanced dopamine and curiosity systems
2. **Memory Architectures**: Advanced memory consolidation algorithms
3. **Development Models**: More realistic developmental trajectories
4. **Social Learning**: Multi-agent learning and sharing
5. **Hardware Acceleration**: GPU acceleration for large-scale simulations

## Contact and Support

For questions, issues, or feature requests:
- Documentation: https://nlm.readthedocs.io
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Discord Community: https://discord.gg/nlm
- Email: support@nlm-project.org

## License

This documentation is provided under the MIT License, same as the NLM project.

---

*Last updated: 2024-01-15*
*Version: 1.4.0*
*For the latest documentation, visit: https://nlm.readthedocs.io*
