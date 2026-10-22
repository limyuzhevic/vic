# NLM (熙然) - Neural Learning Machine

## Quick Start Guide

This is a quick guide for getting started with the NLM (熙然) neural learning machine.

### Installation

First, build the NLM system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running the Basic Demo

Run the Phase 2 demo (real neural computation with LIF neurons):

```bash
./nlm
```

This will run a comprehensive demonstration of:
- Basic neural connectivity
- Plasticity learning experiment
- STDP (Spike-Timing-Dependent Plasticity) verification

### Using Configuration Files

NLM supports both simple key=value and JSON configuration files:

#### Simple Key=Value Format (`config.cfg`)

```
# Neural system configuration
random_seed = 123
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### JSON Format (`config.json`)

```json
{
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### Command Line Options

```bash
nlm [OPTIONS]

Options:
  --help, -h                Show this help message
  --config <file>           Load configuration from file (JSON or key=value format)
```

### Python API Usage

The NLM system comes with Python bindings for easy integration. Here's how to use it:

#### Basic Usage

```python
import pynlm

# Create a default brain
brain = pynlm.createBrain()

# Configure the brain
config = brain.getConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("random_seed", 42)

# Initialize and run
brain.initialize()

# Run a simulation step
for step in range(100):
    brain.step(step, step * 0.001)
    print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
```

#### Complete Agent System

```python
import pynlm

# Create a complete agent system (world + agent brain)
world, agent_brain = pynlm.createAgentSystem(640, 480)

# Configure the agent system
agent_brain.getBrain().getConfig().set("neuron_count", 800)
agent_brain.getBrain().getConfig().set("random_seed", 123)

# Main loop
import time
while True:
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in agent brain
    agent_brain.processSensoryInput(percept)
    
    # Produce action
    action = agent_brain.decodeMotorCommand()
    
    # Execute action in world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for termination
    if result.success:
        print("Action completed successfully")
    
    world.update(0.001)
    time.sleep(0.001)
```

#### Configuration Management

```python
import pynlm
import json

# Create configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Modify configuration
config.set("neuron_count", 1500)
config.set("region_count", 3)

# Save configuration
config.saveToFile("my_config.json")

# Validate configuration
config.validate()

# Get configuration values
neuron_count = config.getInt("neuron_count")
random_seed = config.getInt("random_seed", 42)
```

### Working with Environments

#### SimpleWorld

The SimpleWorld provides a 2D environment for testing NLM agents:

```python
import pynlm

# Create a world
world = pynlm.createSimpleWorld()
world.configure(800, 600, 128, 128)  # width, height, vision_width, vision_height

# Set agent start position
world.setAgentStart(400, 300)

# Add objects to the world
obj = pynlm.WorldObject(100, 100, pynlm.WorldObjectType.Resource, 10.0)
world.addObject(obj)

# Get agent state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent health: {body.health}")
print(f"Agent energy: {body.energy}")
```

### Advanced Python Features

#### Using the AgentBrain Interface

The AgentBrain provides a convenient interface between the NLM brain and the world:

```python
import pynlm

# Create brain and agent brain
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
world.configure(640, 480)

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Get neural network statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio()}")
```

### Performance Optimization Tips

1. **Increase parallelism**: Use `make -j$(nproc)` for faster compilation
2. **Use smaller networks**: For testing, reduce `neuron_count` in configuration
3. **Adjust timestep**: Larger timesteps run faster but may affect stability
4. **Limit simulation steps**: For demos, use fewer steps than the full 10,000

### Troubleshooting

#### Common Issues

**Problem**: NLM fails to build
**Solution**: Ensure CMake version >= 3.16, install all required dependencies, and run `make -j4`

**Problem**: Configuration file not found
**Solution**: Use absolute paths or place config files in the working directory. Check `--help` for usage.

**Problem**: Python bindings not found
**Solution**: Install the package with `pip install .` or build with Python bindings enabled.

**Problem**: Simulation runs too slowly
**Solution**: Reduce `neuron_count`, `region_count`, or number of simulation steps.

### Files Included

- `configs/` - Example configuration files
- `src/` - Source code for the NLM system
- `src/agent/` - Agent system (body, sensory percept, agent-brain interface)
- `src/world/` - World system (environment, objects, simple world)
- `src/experiments/` - Experiment framework (Phase 3, 4, 5, 6)
- `python/` - Python bindings
- `tests/` - Unit tests
- `docs/` - Technical documentation

### Further Reading

For more detailed information:

1. **Phase 6 Integration Demo**: Run `./nlm_phase6_demo` for a comprehensive integration test
2. **Phase 3 Demo**: Run `./nlm_phase3_demo` for basic neural computation tests
3. **Phase 4 Demo**: Run `./nlm_phase4_demo` for social learning experiments
4. **Technical Documentation**: See `docs/ARCHITECTURE.md`, `docs/SCIENCE.md`, and `docs/ROADMAP.md`

### License

MIT License - See LICENSE file for details

---

**Note**: NLM is an experimental research project. Use for educational and research purposes only.
