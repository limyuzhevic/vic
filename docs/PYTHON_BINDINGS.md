# NLM Python Bindings - Documentation

This document describes the improved Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The NLM Python bindings (`pynlm`) provide a high-level interface to the C++ NLM neural simulation framework. This improved version includes enhanced configuration support, better error handling, and additional features for advanced usage.

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- pybind11 (automatically installed via scikit-build)

### Installation from Source

```bash
# Clone the repository
cd /path/to/nlm

# Build and install Python bindings
pip install -e .

# Or build with pip install
python -m pip install scikit-build-core pybind11
python -m pip install -e .
```

### Quick Test

After installation, you can verify the bindings work:

```python
import pynlm
print("NLM Python bindings version:", pynlm.__version__ if hasattr(pynlm, '__version__') else "Unknown")
config = pynlm.createDefaultConfig()
print("Default config created successfully")
```

## API Reference

### Configuration System

The improved configuration system now supports multiple file formats and better type handling.

#### createDefaultConfig()

```python
config = pynlm.createDefaultConfig()
```

Create a new configuration object with default settings.

#### Config.loadFromFile(filepath)

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")  # Now supports JSON and YAML
config.loadFromFile("config.yaml")
config.loadFromFile("config.cfg")   # Falls back to simple key=value format
```

Load configuration from a file. Automatically detects file format based on extension.

#### Config.loadFromArgs(args)

```python
config = pynlm.createDefaultConfig()
config.loadFromArgs(["--neuron_count", "1000", "--connection_probability", "0.2"])
```

Load configuration from command-line arguments.

#### Config.set(key, value)

```python
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)
config.set("stdp_ltp_weight", 0.02)
```

Set configuration values. Supports all standard Python types (int, float, str, bool).

#### Config.get(key)

```python
value = config.get("neuron_count")
if value:
    neuron_count = value
    print(f"Neuron count: {neuron_count}")
```

Get a configuration value. Returns `None` if the key doesn't exist.

#### Config.getOr(key, default)

```python
neuron_count = config.getOr("neuron_count", 500)
```

Get a configuration value with a default fallback.

#### Config.summary()

```python
print(config.summary())
```

Get a human-readable summary of all configuration entries.

#### Config.saveToFile(filepath)

```python
config.saveToFile("my_config.json")
```

Save configuration to a file (JSON format recommended).

### Brain Class

The main `Brain` class represents the neural simulation system.

#### createBrain(config)

```python
from pynlm import createBrain, createDefaultConfig

config = createDefaultConfig()
brain = createBrain(config)
```

Create a new brain instance with the given configuration.

#### brain.initialize()

```python
if brain.initialize():
    print("✓ Brain initialized successfully")
else:
    print("✗ Brain initialization failed")
```

Initialize the brain with the configured settings. Returns `True` on success, `False` on failure.

#### brain.step(step, time)

```python
for i in range(100):
    brain.step(i, i * 0.001)
```

Perform a simulation step. The `time` parameter is optional (defaults to `step * timestep`).

#### brain.getTotalNeuronCount()

```python
neuron_count = brain.getTotalNeuronCount()
print(f"Total neurons: {neuron_count}")
```

Get the total number of neurons in the brain.

#### brain.getTotalSynapseCount()

```python
synapse_count = brain.getTotalSynapseCount()
print(f"Total synapses: {synapse_count}")
```

Get the total number of synapses in the brain.

#### brain.getAverageFiringRate()

```python
firing_rate = brain.getAverageFiringRate()
print(f"Average firing rate: {firing_rate:.2f} Hz")
```

Get the average firing rate across all neurons (in Hz).

#### brain.getTotalSpikeCount()

```python
spike_count = brain.getTotalSpikeCount()
print(f"Total spikes: {spike_count}")
```

Get the total number of spikes generated during simulation.

#### brain.receiveSensoryInput(input)

```python
from pynlm import SensoryInput

# Create sensory input
vision_input = SensoryInput.getVision(16, 16, 3)
# ... set data ...
brain.receiveSensoryInput(vision_input)
```

Inject sensory input into the brain for processing.

#### brain.produceAction()

```python
action = brain.produceAction()
print(f"Action type: {action.getType()}")
```

Produce a motor action based on current neural activity.

#### brain.save(filepath)

```python
if brain.save("/tmp/my_brain.bin"):
    print("✓ Brain saved successfully")
else:
    print("✗ Failed to save brain")
```

Save the current brain state to a checkpoint file.

#### brain.load(filepath)

```python
if brain.load("/tmp/my_brain.bin"):
    print("✓ Brain loaded successfully")
else:
    print("✗ Failed to load brain")
```

Load a brain state from a checkpoint file.

#### brain.reset()

```python
brain.reset()
print("✓ Brain reset")
```

Reset the brain to its initial state.

#### brain.logStatus()

```python
brain.logStatus()
```

Log detailed information about the current brain state.

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction and motor decoding.

#### createAgentBrain(brain)

```python
agent = createAgentBrain(brain)
```

Create an agent brain interface for the given brain.

#### agent.initialize(world)

```python
from pynlm import createSimpleWorld

world = createSimpleWorld()
world.configure(16, 16, 16, 16)
agent.initialize(world)
```

Initialize the agent with the given world.

#### agent.processSensoryInput(percept)

```python
from pynlm import SensoryPercept

percept = SensoryPercept()
# ... set perception data ...
agent.processSensoryInput(percept)
```

Process sensory input and inject it into the brain.

#### agent.decodeMotorCommand()

```python
motor_cmd = agent.decodeMotorCommand()
print(f"Decoded motor command: {motor_cmd}")
```

Decode neural activity into a motor command.

#### agent.getNeuromodulationLevel()

```python
neuromod_level = agent.getNeuromodulationLevel()
print(f"Neuromodulation level: {neuromod_level:.3f}")
```

Get the current neuromodulation level.

#### agent.getCuriosityLevel()

```python
curiosity = agent.getCuriosityLevel()
print(f"Curiosity level: {curiosity:.3f}")
```

Get the current curiosity level.

#### agent.getNoveltyLevel()

```python
novelty = agent.getNoveltyLevel()
print(f"Novelty level: {novelty:.3f}")
```

Get the current novelty level.

#### agent.applyRewardModulation(reward, predicted_reward)

```python
agent.applyRewardModulation(reward, predicted_reward)
```

Apply reward-based neuromodulation.

#### agent.updateDevelopment(timestep)

```python
agent.updateDevelopment(0.001)
```

Update the development system.

#### agent.getThoughtTrace()

```python
thought_trace = agent.getThoughtTrace()
print(f"Thought trace: {thought_trace}")
```

Get the agent's thought trace (Phase 5 feature).

#### agent.getLearnedPreference()

```python
preference = agent.getLearnedPreference()
print(f"Learned preference: {preference:.3f}")
```

Get the agent's learned preference trait (survives checkpoint reload).

### World Class

The `SimpleWorld` class provides a basic 2D world for NLM simulation.

#### createSimpleWorld()

```python
world = createSimpleWorld()
```

Create a new simple world instance.

#### world.configure(width, height, vision_width, vision_height)

```python
world.configure(16, 16, 16, 16)
```

Configure world dimensions and vision parameters.

#### world.update(timestep)

```python
world.update(0.001)
```

Update world state (e.g., move agents, change environment).

#### world.applyMotorCommand(cmd, time)

```python
result = world.applyMotorCommand(motor_cmd, time)
print(f"Action result: reward={result.reward}, success={result.success}")
```

Apply a motor command to the world and get the result.

#### world.getSensoryPercept()

```python
percept = world.getSensoryPercept()
```

Get the current sensory perception from the world.

### Advanced Features

#### Text Encoding (Phase 6)

```python
from pynlm import AgentBrain

# Encode text into neural patterns
pattern = AgentBrain.encodeText("hello")
print(f"'hello' -> {pattern}")

# Verify encoding consistency
pattern2 = AgentBrain.encodeText("hello")
assert pattern == pattern2, "Encoding should be consistent"
```

The text-as-a-sense feature allows words to be associated with neural patterns, enabling association learning without string matching in the decision logic.

#### Memory Integration

```python
brain = createBrain(config)
working_mem = brain.getWorkingMemory()
if working_mem:
    print(f"Working memory active traces: {working_mem.getActiveTraces()}")

episodic_mem = brain.getEpisodicMemory()
if episodic_mem:
    print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
```

Access integrated memory systems for working memory and episodic memory.

#### Neuromodulation

```python
brain = createBrain(config)

# Access all major neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Check their levels and use them for control
if dopamine and curiosity and novelty:
    total_modulation = (dopamine.getLevel() + 
                       curiosity.getLevel() + 
                       novelty.getLevel()) / 3.0
    print(f"Average neuromodulation level: {total_modulation:.3f}")
```

Access the comprehensive neuromodulation system with real implementations.

#### Cognitive Systems

```python
brain = createBrain(config)

# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Check their availability and use
if planner and concept_formation and attention:
    print("All cognitive systems integrated and functional")
```

Access the integrated cognitive systems (neural planner, concept formation, attention).

#### Development System

```python
brain = createBrain(config)
dev_system = brain.getDevelopmentSystem()

if dev_system:
    print(f"Current developmental stage: {dev_system.getDevelopmentalStage()}")
    
    # Simulate development over time
    for age in [10.0, 50.0, 100.0, 500.0]:
        dev_system.update(brain, age * 0.001)
        print(f"Age {age}: stage={dev_system.getDevelopmentalStage()}, "
              f"plasticity={dev_system.getPlasticityModifier():.2f}")
```

Access and interact with the developmental system.

## Example Usage

For a complete working example, see `examples/nlm_python_example.py` in the repository.

## Configuration File Examples

### JSON Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "checkpoint_dir": "./checkpoints",
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true
}
```

### YAML Configuration

```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.15
simulation_timestep: 0.001
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
checkpoint_dir: ./checkpoints
replay_interval: 100
consolidation_interval: 1000
enable_development: true
enable_curiosity: true
enable_reward_modulation: true
```

### Simple Key=Value Configuration

```
# config.cfg
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
checkpoint_dir = ./checkpoints
replay_interval = 100
consolidation_interval = 1000
```

## Error Handling

The improved bindings include comprehensive error handling:

```python
try:
    config = pynlm.createDefaultConfig()
    config.loadFromFile("config.json")
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        raise RuntimeError("Brain initialization failed")
        
    # Run simulation
    for i in range(1000):
        brain.step(i, i * 0.001)
        
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
```

## Performance Considerations

### Large-Scale Simulation

For simulations with many neurons, consider these optimizations:

1. **Use efficient configuration**: Load configuration from files rather than setting individual values
2. **Batch operations**: Process sensory inputs and actions in batches when possible
3. **Memory management**: Monitor memory usage in long-running simulations
4. **Checkpointing**: Use periodic checkpointing for long simulations

### Multiprocessing

While the NLM bindings themselves are single-threaded, you can use Python's multiprocessing capabilities:

```python
import multiprocessing
import pynlm

def simulate_config(config_dict):
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    if brain.initialize():
        for i in range(100):
            brain.step(i, i * 0.001)
        return brain.getTotalSpikeCount()
    return 0

# Run simulations in parallel
configs = [
    {"neuron_count": 100, "region_count": 1},
    {"neuron_count": 200, "region_count": 2},
    {"neuron_count": 300, "region_count": 3},
]

with multiprocessing.Pool() as pool:
    results = pool.map(simulate_config, configs)
    print(f"Results: {results}")
```

## Compatibility

The NLM Python bindings are compatible with:

- **Python**: 3.8, 3.9, 3.10, 3.11, 3.12
- **Operating Systems**: Linux, macOS, Windows
- **Build Systems**: CMake (scikit-build-core)

## Future Improvements

The following improvements are planned for future releases:

1. **Enhanced Configuration**: More sophisticated JSON/YAML schema validation
2. **Parallel Processing**: Built-in support for multi-core execution
3. **Distributed Computing**: Support for distributed NLM simulations
4. **Advanced Visualization**: Integration with matplotlib and other plotting libraries
5. **Real-time Interface**: WebSocket support for real-time interaction
6. **Machine Learning Integration**: Enhanced integration with ML frameworks

## Support and Contributions

For support, bug reports, and contributions:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: File issues at https://github.com/nlm-project/nlm/issues
- **Discussions**: Join the discussion at https://github.com/nlm-project/nlm/discussions

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

The NLM Python bindings improvements build upon the foundational work of:

- The NLM research team
- The pybind11 library for C++/Python bindings
- The scikit-build-core build system
- All contributors to the NLM project

---

*Version: 0.1.0*
*Last Updated: 2026-11-02*
*Phase 6: Final Integration*