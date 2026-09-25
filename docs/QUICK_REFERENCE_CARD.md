# NLM Quick Reference Card

## Installation

### Build from Source
```bash
# Create build directory and configure
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON

# Build with optimizations
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Python Usage
```python
import pynlm

# Create default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

## Basic Commands

### Brain Operations
```python
# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
brain.step(step_number)           # Run one step
brain.reset()                      # Reset state

# Get statistics
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()
```

### Memory Operations
```python
# Save/load brain state
brain.save("checkpoint.bin")     # Save to file
brain.load("checkpoint.bin")     # Load from file
```

### World Operations
```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control world
world.reset()
world.update(timestep)
world.setAgentStart(x, y)

# Get state
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

### Agent Operations
```python
# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Control
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
```

## Key Configuration Parameters

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `brain.v_thresh` | float | -50.0 | Threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Quick Usage Examples

### Example 1: Simple Brain
```python
import pynlm

# One-line brain creation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
```

### Example 2: Brain in World
```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
```

### Example 3: Complete Agent with Learning
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")

run_agent_simulation(1000)
```

## Important Functions and Methods

### Brain Class Methods
| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create new brain |
| `initialize()` | Initialize brain |
| `step(step_number)` | Run simulation step |
| `reset()` | Reset brain state |
| `save(filename)` | Save brain state |
| `load(filename)` | Load brain state |
| `getTotalNeuronCount()` | Get neuron count |
| `getTotalSynapseCount()` | Get synapse count |
| `getFiringNeuronCount()` | Get firing neurons |
| `getAverageFiringRate()` | Get average firing rate |
| `getExcitationInhibitionRatio()` | Get E/I balance |

### Config Class Methods
| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default config |
| `has(key)` | Check if config has key |
| `getKeys()` | Get all keys |
| `set(key, value)` | Set configuration value |
| `loadFromFile(filename)` | Load from file |
| `saveToFile(filename)` | Save to file |

### AgentBrain Class Methods
| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent interface |
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Get motor command |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward learning |
| `updateDevelopment(timestep)` | Update development |
| `getDevelopmentalStage()` | Get development stage |

### SimpleWorld Class Methods
| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create world |
| `configure(width, height, visionWidth, visionHeight)` | Configure world size |
| `reset()` | Reset world |
| `update(timestep)` | Update world |
| `setRandomSeed(seed)` | Set random seed |
| `setAgentStart(x, y)` | Set agent start position |
| `applyMotorCommand(action, time)` | Apply motor command |
| `getSensoryPercept()` | Get sensory input |
| `getAgentBody()` | Get agent body |

## Development Commands

### Building
```bash
# Configure with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON

# Build with parallel jobs
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Testing
```bash
# Run unit tests
./nlm_test

# Run integration tests
./nlm_phase3_demo
./nlm_phase4_demo

# Run benchmark
./nlm --benchmark --neurons=10000 --steps=10000
```

### Development
```bash
# Install in development mode
pip install -e .

# Update documentation
./nlm --update-docs

# Generate API documentation
./nlm --generate-docs
```

## Troubleshooting Quick Fixes

### Common Issues
| Problem | Solution |
|---------|----------|
| Cannot import pynlm | `pip install -e .` |
| ImportError: No module named 'pynlm' | Reinstall: `pip install --force-reinstall .` |
| Segmentation fault | Check `brain.initialize()` before `brain.step()` |
| Brain not doing anything | Call `brain.initialize()` |
| Agent not moving | Call `world.applyMotorCommand()` |

### Fast Fixes
```bash
# Reinstall Python module
pip install --force-reinstall .

# Fix common issues
./nlm --fix-issues

# Check system health
./nlm --health-check
```

## Reference Links

- [NLM Architecture](docs/ARCHITECTURE.md)
- [Performance Guide](docs/PERFORMANCE_GUIDE.md)
- [Architecture Extension Guide](docs/ARCHITECTURE_EXTENSION_GUIDE.md)
- [Troubleshooting Guide](docs/TROUBLESHOOTING_GUIDE.md)
- [Phase 7 Roadmap](Phase7_Roadmap.md)

## Updates

- **Phase 7 Roadmap**: Added new plugin system and advanced features
- **Performance Guide**: Comprehensive benchmarks and optimization strategies
- **Architecture Extension Guide**: Plugin system and customization details
- **Troubleshooting Guide**: Systematic problem resolution approaches
- **Quick Reference Card**: Essential commands and APIs

## Next Steps

1. Read the full documentation for detailed information
2. Join the NLM community for support and collaboration
3. Experiment with different configurations
4. Report issues and contribute to the project
5. Explore research extensions and plugins

The NLM Quick Reference Card provides essential information for getting started with NLM, including installation, basic usage, key configuration parameters, quick examples, and troubleshooting tips.