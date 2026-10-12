# NLM (Neural Learning Machine) - Easy Usage Guide

## Quick Start

NLM (熙然) is an experimental computational brain project that simulates neural systems using biologically inspired mechanisms. This guide provides a quick introduction to the most important features and use cases.

## Installation

### Quick Installation (Python bindings)

```bash
# Install dependencies
pip install pynlm[all]

# For development
pip install -e .[all]
```

### Quick Installation (C++)

```bash
# Clone repository
https://github.com/nlm-project/nlm.git

# Build and install
cd nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
make install
```

## Basic Usage Examples

### 1. Minimal Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

### 2. Complete Agent Simulation

```python
import pynlm

# Create agent with world interaction
agent = pynlm.createAgentBrain(
    brain=pynlm.createBrain(pynlm.createDefaultConfig()),
    maxEnergy=100.0,
    decayRate=0.01
)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=16,
    visionHeight=16,
    energyType="gradient",
    foodSpawnInterval=50
)
world.reset()
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Energy={world.getAgentEnergy():.1f}, "
              f"Curiosity={agent.getCuriosityLevel():.2f}")
```

## Key Features

### 🧠 Neural Components

- **LIF Neurons**: Leaky Integrate-and-Fire neurons with biological parameters
- **Synaptic Plasticity**: STDP, Hebbian, and structural plasticity mechanisms
- **Neuromodulation**: Dopamine, curiosity, and reward processing
- **Memory Systems**: Working memory, episodic memory, and associative memory

### 🌍 World Simulation

- **2D Environment**: Continuous 2D world with dynamic objects
- **Sensory Processing**: Multi-modal sensory input (vision, touch, internal)
- **Action Execution**: Motor control with action selection
- **Resource Management**: Energy and resource dynamics

### 🧬 Agent Architecture

- **Brain-Agent Interface**: Seamless integration between brain and world
- **Sensory Processing**: Low-level sensory transduction and integration
- **Motor Decoding**: Neural activity to action mapping
- **Development**: Multi-stage developmental trajectory

## Configuration

### Default Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Customize specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.threshold", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### Configuration Files

```bash
# Save configuration
cp config.json config_default.json

# Load configuration
python -c "import pynlm; c = pynlm.Config(); c.loadFromFile('config_default.json')"
```

## Testing

### Run Built-in Tests

```bash
# Run unit tests
python -m pytest tests/ -v

# Run demonstration scripts
python examples/phase3_demo.py
python examples/phase4_demo.py
python examples/phase6_demo.py
```

### Test Modes

```python
import pynlm

# Create brain with different test configurations
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Run basic connectivity test
brain.step(0)
spikes = brain.getTotalSpikeCount()

# Enable plasticity
brain.setPlasticityEnabled(True)
brain.setHebbianEnabled(True)
brain.setSTDPenabled(True)

# Run plasticity experiment
for i in range(100):
    brain.step(i)
```

## Performance Tips

### Optimize Memory Usage

```python
# Use memory pools for better performance
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.use_memory_pool", True)
config.set("brain.pool_size", 2048)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Optimize CPU Usage

```python
# Enable SIMD optimizations
import os
os.environ['CPU_FEATURES'] = 'avx2'

# Configure for performance
config = pynlm.createDefaultConfig()
config.set("performance.simulation_timestep", 0.001)
config.set("performance.use_parallel", True)
```

## Advanced Usage

### Custom Plugin Development

```python
# Create custom brain types
class CustomBrain:
    def __init__(self, config):
        self.config = config
        self.neurons = []
        self.synapses = []
    
    def initialize(self):
        # Custom initialization logic
        pass
    
    def step(self, time):
        # Custom simulation logic
        pass
```

### Multi-Agent Systems

```python
import pynlm

# Create multiple agents
agents = []
for i in range(3):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
world.reset()

# Run simulation with multiple agents
for step in range(1000):
    for agent in agents:
        agent.initialize(world)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), step)
```

## Troubleshooting

### Common Issues

**Issue: Simulation is too slow**
- Increase timestep: `config.set("simulation.timestep", 0.01)`
- Enable SIMD: Set environment variable `CPU_FEATURES=avx2`
- Reduce neurons: Decrease `brain.neuron_count`

**Issue: Memory usage is too high**
- Use memory pools: `config.set("brain.use_memory_pool", True)`
- Reduce simulation steps
- Use sparse connectivity: `config.set("brain.sparse_connections", True)`

**Issue: Python bindings not found**
```bash
pip install pynlm[all]
python -c "import pynlm; print(pynlm.__version__)"
```

### Error Messages

- `ImportError: No module named 'pynlm'`: Install with `pip install pynlm`
- `ModuleNotFoundError`: Check Python path and installation
- `AttributeError`: Update to latest version or check API usage

## Best Practices

### For Beginners

1. **Start Simple**: Begin with minimal configuration and build up
2. **Use Examples**: Study existing example scripts
3. **Check Documentation**: Refer to this guide for API usage
4. **Debug Gradually**: Test one component at a time

### For Advanced Users

1. **Optimize Early**: Profile and optimize from the start
2. **Use Configuration**: Leverage configuration files for testing
3. **Monitor Performance**: Use built-in performance monitoring
4. **Extend System**: Use plugin system for custom components

### For Research

1. **Document Changes**: Keep track of experimental modifications
2. **Reproduce Results**: Use configuration files for reproducibility
3. **Profile Performance**: Analyze bottlenecks and optimize
4. **Validate Models**: Compare with experimental data

## Getting Help

### Documentation

- **API Reference**: See `docs/API.md` for detailed API documentation
- **Architecture**: See `docs/ARCHITECTURE.md` for system architecture
- **Science**: See `docs/SCIENCE.md` for scientific background
- **Experiments**: See `docs/EXPERIMENTS.md` for experiment descriptions

### Community

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Join community discussions
- **Examples**: Contribute example scripts and tutorials

### Support

```python
import pynlm

# Get help with specific API usage
try:
    brain = pynlm.createBrain(config)
except Exception as e:
    print(f"Error: {e}")
    print("For help: https://github.com/nlm-project/nlm/issues")
```

## Conclusion

NLM provides a powerful platform for neural network research with:

- **Biological Plausibility**: LIF neurons and plasticity mechanisms
- **Performance**: Optimized with SIMD and memory pools
- **Extensibility**: Plugin system for custom components
- **Usability**: Simple Python API and comprehensive documentation
- **Flexibility**: Extensive configuration options

Get started with the examples above, and don't hesitate to reach out for help with your neural network research needs.

**Happy simulating!** 🌟

---

## Quick Reference

### Core Functions

```python
import pynlm

# Brain creation and control
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step_number)
brain.reset()

# Configuration
config = pynlm.createDefaultConfig()
config.set("parameter", value)

# Agents and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

# Simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), step)
```

### Key Classes

| Class | Purpose |
|-------|---------|
| `Brain` | Neural simulation engine |
| `AgentBrain` | Brain-world interface |
| `SimpleWorld` | 2D simulation environment |
| `Config` | Configuration management |

### Common Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Create Brain | `pynlm.createBrain(config)` | Initialize neural system |
| Step Simulation | `brain.step(step)` | Advance simulation |
| Get Statistics | `brain.getTotalSpikeCount()` | Query system state |
| Save State | `brain.save("file")` | Persist simulation state |
| Load State | `brain.load("file")` | Restore simulation state |

---

**Ready to start your neural network research? Welcome to NLM!** 🚀

For more detailed information, visit the project documentation and examples.

---

*NLM Research Team* | *Version 2.0.0* | *Last Updated: $(date)*