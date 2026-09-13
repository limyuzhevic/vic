# NLM - Advanced Usage Examples and Feature Documentation

## Overview

This document provides advanced usage examples and detailed documentation for the NLM (Neural Learning Machine) system. It covers topics beyond basic usage, including configuration management, development features, and performance optimization.

## Configuration Management

### JSON Configuration

NLM now supports JSON configuration files with full type support:

#### Configuration File Format

A JSON configuration file can include:

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "simulation_timestep": 0.001,
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "checkpoint_dir": "./checkpoints",
    "random_seed": 42,
    "development_enabled": true,
    "reward_modulation_enabled": true,
    "structural_plasticity_enabled": true,
    "curiosity_enabled": true,
    "novelty_detection_enabled": true
}
```

#### Loading and Saving Configuration

```python
import pynlm

# Load configuration from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Save configuration to JSON file
config.saveToFile("my_config.json")

# Get specific configuration values
neuron_count = config.getOr<int64_t>("neuron_count", 1000)
connection_probability = config.getOr<double>("connection_probability", 0.15)
development_enabled = config.getOr<bool>("development_enabled", true)

# Check if configuration has specific values
if config.has("custom_parameter"):
    value = config.get<std::string>("custom_parameter")
```

### Command Line Configuration

NLM also supports command line configuration:

```bash
./nlm --neuron_count=2000 --connection_probability=0.2 --development_enabled=true
```

Or with spaces:
```bash
./nlm --neuron_count 2000 --connection_probability 0.2 --development_enabled true
```

## Development Features

### Development Stages

NLM supports multiple developmental stages:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Moderately high plasticity
3. **Maturation**: Reduced plasticity
4. **Adult**: Low plasticity, stable behavior
5. **Aging**: Very low plasticity, declining performance

#### Development Progress Monitoring

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor development progress over time
for step in range(10000):
    brain.step(step)
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: Developmental stage = {stage}")
    
    # Get developmental age
    developmental_stage = agent.getDevelopmentalStage()
    print(f"Stage: {developmental_stage}, Age: {agent.getAge()}")
    
    if stage == pynlm.DevelopmentalStage.Adult:
        print("Brain has reached adult stage")
        break
```

### Plasticity Configuration

Control different plasticity mechanisms:

```python
config = pynlm.createDefaultConfig()

# STDP parameters
config.set("stdp_ltp_weight", 0.01f)  # Long-term potentiation weight
config.set("stdp_ltd_weight", 0.012f)  # Long-term depression weight
config.set("stdp_tau", 20.0f)        # STDP time constant

# Structural plasticity
config.set("synaptogenesis_rate", 0.0001f)  # New synapse formation
config.set("pruning_rate", 0.00001f)      # Synapse elimination

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)

brain = pynlm.createBrain(config)
```

## Memory Systems

### Working Memory

Working memory maintains active information across steps:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Process sensory input
for step in range(1000):
    brain.step(step)
    
    # Working memory should maintain traces
    working_memory = brain.getWorkingMemory()
    if working_memory:
        active_traces = working_memory.getActiveTraces()
        print(f"Step {step}: Working memory traces = {active_traces}")
```

### Episodic Memory

Episodic memory stores experiences:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)
    
    # Episodic memory stores episodes
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        episode_count = episodic_memory.getEpisodeCount()
        print(f"Stored {episode_count} episodes")
```

## Performance Features

### Checkpoint and Save/Load

NLM supports checkpointing for persistence:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Save checkpoint
if brain.save("checkpoint.bin"):
    print("Checkpoint saved successfully")
    
# Later, load checkpoint
brain2 = pynlm.createBrain(config)
brain2.initialize()
if brain2.load("checkpoint.bin"):
    print("Checkpoint loaded successfully")
    # brain2 continues from saved state
```

### Performance Monitoring

Track system performance:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor performance over time
stats = []
for step in range(5000):
    brain.step(step)
    
    # Collect statistics
    stat = {
        'step': step,
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'spike_count': brain.getTotalSpikeCount(),
        'working_memory': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
        'episodic_memory': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
    }
    stats.append(stat)
    
    # Log status periodically
    if step % 1000 == 0:
        brain.logStatus()
```

## Advanced Agent Features

### Neuromodulation

Control neuromodulation systems:

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get neuromodulation levels
print(f"Dopamine level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
```

### Curiosity-Driven Exploration

Curiosity drives exploration behavior:

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation with curiosity
for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Curiosity affects action selection
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.3:
        print(f"Step {step}: High curiosity ({curiosity:.2f}) - likely exploration")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Error Handling and Logging

### Robust Error Handling

NLM provides comprehensive error handling:

```python
config = pynlm.createDefaultConfig()

# Try to load non-existent file
try:
    if not config.loadFromFile("nonexistent.json"):
        print("Warning: Could not load configuration from nonexistent.json")
        print("Using default configuration")
except Exception as e:
    print(f"Error loading configuration: {e}")

# Safe access to configuration values
if config.has("optional_param"):
    value = config.get<std::string>("optional_param")
    if value.has_value():
        print(f"Optional parameter: {value.value()}")
```

### Logging Levels

Configure logging levels:

```python
import pynlm

# Enable different logging levels
logger = pynlm.Logger()
logger.setLevel(pynlm.LogLevel::Info)

# Create brain and enable logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Log status will now show informational messages
brain.logStatus()
```

## Advanced Experimentation

### Parameter Tuning

Experiment with different parameter combinations:

```python
import pynlm

# Experiment with different plasticity parameters
def run_experiment(ltp_weight, ltd_weight, synaptogenesis_rate):
    config = pynlm.createDefaultConfig()
    config.set("stdp_ltp_weight", ltp_weight)
    config.set("stdp_ltd_weight", ltd_weight)
    config.set("synaptogenesis_rate", synaptogenesis_rate)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Collect results
    result = {
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'plasticity_effectiveness': calculate_plasticity_effect(brain)
    }
    
    return result

# Run multiple experiments
experiments = [
    (0.01, 0.012, 0.0001),
    (0.02, 0.024, 0.0002),
    (0.005, 0.006, 0.00005)
]

for ltp, ltd, syn_rate in experiments:
    result = run_experiment(ltp, ltd, syn_rate)
    print(f"LTp: {ltp}, LTD: {ltd}, Syn rate: {syn_rate}")
    print(f"  Result: {result}")
```

### Batch Processing

Process multiple simulations in parallel:

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def run_single_simulation(params):
    config = pynlm.createDefaultConfig()
    
    # Set parameters from params dict
    for key, value in params.items():
        if isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, str):
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(params.get('steps', 1000)):
        brain.step(step)
    
    return {
        'params': params,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount()
    }

# Define parameter combinations
parameter_sets = [
    {'neuron_count': 500, 'connection_probability': 0.1, 'steps': 500},
    {'neuron_count': 1000, 'connection_probability': 0.15, 'steps': 1000},
    {'neuron_count': 2000, 'connection_probability': 0.2, 'steps': 2000}
]

# Run simulations in parallel
with ThreadPoolExecutor(max_workers=3) as executor:
    results = list(executor.map(run_single_simulation, parameter_sets))

# Process results
for result in results:
    print(f"Params: {result['params']}")
    print(f"  Final firing rate: {result['final_firing_rate']}")
    print(f"  Total spikes: {result['total_spikes']}")
```

## Migration Guide

### From Phase 1 to Phase 2+

If upgrading from Phase 1, note these changes:

1. **Configuration Format**: JSON configuration files replace key=value format
2. **Memory Systems**: Working and episodic memory are now integrated
3. **Neuromodulation**: Enhanced neuromodulation systems
4. **Development**: Full developmental stages with age-based plasticity
5. **Error Handling**: Better error handling and logging

#### Migration Steps

1. Update configuration files to JSON format
2. Update code to use new memory system APIs
3. Add neuromodulation to agent configuration
4. Implement development stage tracking
5. Add comprehensive error handling

## API Reference

### Config Class

| Method | Description |
|--------|-------------|
| `loadFromFile(filepath)` | Load configuration from JSON or key=value file |
| `saveToFile(filepath)` | Save configuration to JSON file |
| `loadFromArgs(argc, argv)` | Load from command line arguments |
| `getOr<T>(key, defaultValue)` | Get value with default if missing |
| `has(key)` | Check if configuration has key |
| `remove(key)` | Remove configuration entry |
| `getKeys()` | Get all configuration keys |
| `clear()` | Clear all configuration |
| `summary()` | Get configuration summary |

### Brain Class

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize brain with configuration |
| `step(step, time)` | Run brain simulation step |
| `receiveSensoryInput(input)` | Inject sensory input |
| `injectCurrent(neuron, current)` | Inject current into specific neuron |
| `produceAction()` | Get motor action from brain |
| `reset()` | Reset brain state |
| `save(filepath)` | Save brain state to file |
| `load(filepath)` | Load brain state from file |
| `logStatus()` | Log brain status |

### AgentBrain Class

| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize agent with world interface |
| `processSensoryInput(percept)` | Process sensory percepts |
| `decodeMotorCommand()` | Get motor command from brain |
| `applyRewardModulation(reward, predictedReward)` | Apply reward signals |
| `enableRewardModulation(enable)` | Enable/disable reward modulation |
| `enableDevelopment(enable)` | Enable/disable development |
| `enableCuriosity(enable)` | Enable/disable curiosity |

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails
**Solution**: Check that the configuration is valid and contains required parameters:

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 1)

brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Failed to initialize brain")
    # Check configuration
    print("Configuration keys:", config.getKeys())
```

#### Issue: Memory systems not working
**Solution**: Ensure memory systems are properly integrated:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Check if memory systems are available
if brain.getWorkingMemory() is None:
    print("Warning: Working memory not available")
    
if brain.getEpisodicMemory() is None:
    print("Warning: Episodic memory not available")
```

#### Issue: Checkpoint save/load fails
**Solution**: Check file permissions and disk space:

```python
import os
import tempfile

# Use temporary directory
temp_dir = tempfile.gettempdir()
checkpoint_path = os.path.join(temp_dir, "nlm_checkpoint.bin")

if brain.save(checkpoint_path):
    print(f"Checkpoint saved to {checkpoint_path}")
else:
    print(f"Failed to save checkpoint to {checkpoint_path}")
    print(f"Available disk space: {os.statvfs(temp_dir).f_bavail} bytes")
```

## Best Practices

### Configuration Management

1. Use JSON configuration files for complex configurations
2. Keep simple configurations as key=value files for backward compatibility
3. Document configuration parameters in comments
4. Validate configuration before using it

### Error Handling

1. Always check return values from NLM functions
2. Use try-catch blocks for file operations
3. Log errors with sufficient context
4. Provide fallback values for optional parameters

### Performance Optimization

1. Use larger timesteps for faster simulations
2. Monitor memory usage during long runs
3. Use checkpointing for long-running simulations
4. Profile code to identify performance bottlenecks

### Development

1. Start with small networks for testing
2. Gradually increase complexity
3. Monitor all three aspects: neural activity, memory, and behavior
4. Use consistent random seeds for reproducible results

## Examples Directory

The `examples/` directory contains additional example scripts:

1. `simple_brain.py` - Basic brain simulation
2. `agent_world.py` - Complete agent-world simulation
3. `parameter_tuning.py` - Parameter optimization
4. `memory_analysis.py` - Memory system analysis
5. `performance_monitoring.py` - Performance tracking

These examples demonstrate advanced NLM usage and can be used as starting points for custom applications.

## Conclusion

This document provides comprehensive guidance for advanced NLM usage. The system offers:

- **Flexible Configuration**: JSON and command-line configuration management
- **Integrated Memory Systems**: Working and episodic memory
- **Neuromodulation**: Reward, curiosity, and novelty detection
- **Development**: Multi-stage development with age-based plasticity
- **Performance Monitoring**: Comprehensive statistics and checkpointing
- **Robust Error Handling**: Comprehensive error handling and logging

NLM is a powerful platform for building artificial developmental systems with brain-like properties. The advanced features enable complex simulations, detailed analysis, and reproducible experimentation.

---

*For more information, refer to the NLM documentation, including ARCHITECTURE.md and SCIENCE.md for technical details, and the examples directory for practical applications.*