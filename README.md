# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Bindings

The NLM project includes Python bindings via pybind11, allowing you to control neural simulations from Python scripts.

```bash
# Build Python bindings (typically done as part of the main build)
# For development, you can build in a separate directory:
mkdir build_python
cd build_python
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/path/to/python
make -j4
```

After building, the Python module `pynlm` will be available in the `build_python/` directory or installed location.

## Python Usage Examples

### Basic Setup
```python
import pynlm
import numpy as np

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Create an agent brain interface
agent_brain = pynlm.createAgentBrain(brain)
```

### Simulation Loop
```python
def run_episode(agent_brain, world, max_steps=1000):
    total_reward = 0.0
    steps_completed = 0
    
    for step in range(max_steps):
        # Get sensory input from the world
        percept = world.getSensoryPercept()
        
        # Process sensory input through the agent brain
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command from brain activity
        action = agent_brain.decodeMotorCommand()
        
        # Apply action in the world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(result.reward, 0.0)
        
        total_reward += result.reward
        
        # Update development
        agent_brain.updateDevelopment(1.0 / 60.0)  # 60 Hz timestep
        
        # Update world
        world.update(1.0 / 60.0)
        
        steps_completed += 1
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Reward = {result.reward:.3f}, "
                  f"Total = {total_reward:.3f}")
    
    return total_reward, steps_completed

# Run a simulation episode
agent_brain, world = setup_for_simulation()  # Your setup function
reward, steps = run_episode(agent_brain, world, max_steps=1000)
print(f"Episode finished: {steps} steps, total reward = {reward:.3f}")
```

### Working with Neural Data
```python
# Access brain properties
brain = pynlm.createBrain(config)
brain.initialize()

# Get neural statistics
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_rate = brain.getAverageFiringRate()

print(f"Brain statistics:")
print(f"  Neurons: {neuron_count}")
print(f"  Synapses: {synapse_count}")
print(f"  Active neurons: {active_neurons}")
print(f"  Average firing rate: {firing_rate:.3f} Hz")

# Access regions and their properties
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    # Process region data as needed
    pass

# Inject external current (simulating sensory input)
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)

# Step the simulation
brain.step(0, 0.001)
```

### Action and World Interaction
```python
# Create and use actions
action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action: {action.getName()}")  # "MoveForward"

# Create actions with parameters
custom_action = pynlm.Action(
    pynlm.ActionType.Custom, 
    [0.5, 0.8, 1.2]  # Custom parameters
)

# Apply actions in the world
world = pynlm.createSimpleWorld()
world.configure(50, 50, 10, 10)

# Set agent starting position
world.setAgentStart(25, 25)

# Apply motor commands
for _ in range(100):
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    result = world.applyMotorCommand(action, world.getSimulationTime())
    print(f"Action result: {result}")
    world.update(0.1)
```

### Configuration and Serialization
```python
# Load configuration from file
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/my_config.json")

# Modify configuration
config.set("neuron_count", 1000)
config.set("random_seed", 42)

# Save configuration to file
config.saveToFile("configs/my_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(config)

# Save/load brain state
brain.save("my_brain_state.nnl")
# Later...
brain.load("my_brain_state.nnl")
```

### Advanced Python Features
```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Use the convenience simulation function
agent_brain = pynlm.createAgentBrain(brain)
world.configure(80, 80, 15, 15)
world.setAgentStart(40, 40)

# Run a complete episode
total_reward, steps = pynlm.simulateEpisode(agent_brain, world, maxSteps=500, render=True)
print(f"Episode completed: {steps} steps, total reward: {total_reward}")

# Benchmark brain performance
config = pynlm.createRandomConfig()
steps_completed = pynlm.benchmarkBrain(config, durationMs=2000)
print(f"Brain benchmark: {steps_completed} steps in 2 seconds")
```

### Working with Data Structures
```python
# Vision sensory input
vision = pynlm.Vision(64, 64, 3)  # 64x64 RGB image
vision.setData(list(np.random.random(64*64*3)))  # Random pixel data

# Audio sensory input
audio = pynlm.Audio(44100, 44100 // 10)  # 1 second of audio at 44.1kHz
audio.setData(list(np.random.random(44100 // 10)))

# Internal signals
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.addSignal(0.3)

# Sensory percept (combines all modalities)
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
percept.setInternal(internal)
percept.setTimestamp(world.getSimulationTime())

# Agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation:.3f} rad ({body.getOrientationDegrees():.1f} deg)")
print(f"Agent energy: {body.energy:.2f}")
print(f"Agent health: {body.health:.2f}")

# World objects
world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 10.0))
world.addObject(pynlm.WorldObject(30, 30, pynlm.WorldObjectType.Hazard, -5.0))
```

### Error Handling
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created successfully")
except pynlm.RuntimeError as e:
    print(f"Error creating brain: {e}")
```

## Python Module Information

### Available Classes

| Class | Description |
|-------|-------------|
| `Config` | Configuration management |
| `Brain` | Central neural simulation brain |
| `AgentBrain` | Agent-world interface |
| `SimpleWorld` | 2D simulation world |
| `Vision` | Visual sensory input |
| `Audio` | Audio sensory input |
| `InternalSignals` | Internal neural signals |
| `Action` | Motor action representation |
| `AgentBody` | Agent physical state |
| `ActionResult` | Action outcome |
| `SensoryPercept` | Complete sensory data |

### Available Constants

- `INVALID_NEURON_ID`: Invalid neuron identifier
- `INVALID_SYNAPSE_ID`: Invalid synapse identifier
- `INVALID_REGION_ID`: Invalid region identifier
- `INVALID_POPULATION_ID`: Invalid population identifier

### Enumerations

All NeuronType, ActionType, MotorCommand, WorldObjectType, etc. are available as Python enums for easy use.

## Installation and Setup

### Requirements

- Python 3.8+
- pybind11 (automatically downloaded)
- CMake 3.16+
- C++ compiler with C++20 support

### Building from Source

```bash
# Create build directory and configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets (including Python bindings)
cmake --build build --parallel

# Install (optional)
cmake --install build
```

### Development Setup

For development, you can build Python bindings separately:

```bash
mkdir build_python
cd build_python
cmake -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
make -j4
```

### Using Installed Package

After installation, you can use the module from anywhere:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print("NLM Python module loaded successfully!")
```

## Performance Considerations

1. **Simulation Speed**: The C++ core provides high-performance neural simulation
2. **Memory Management**: Use context managers for long-running simulations
3. **Threading**: The core simulation is single-threaded but highly optimized
4. **Data Formats**: Use native Python lists/arrays for I/O operations

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure Python bindings were built
   - Check that the module is in your Python path
   - Try: `python3 -c "import sys; sys.path.append('build_python'); import pynlm; print(pynlm.version())"`

2. **RuntimeError during initialization**
   - Check configuration parameters
   - Ensure all required dependencies are available
   - Verify system resources (memory, CPU)

3. **Building Python bindings fails**
   - Ensure pybind11 is available
   - Check Python development headers
   - Try specifying Python executable: `cmake -DPYTHON_EXECUTABLE=/path/to/python`

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Main build configuration
├── python/                          # Python bindings
│   ├── CMakeLists.txt              # Python-specific build
│   └── bindings.cpp                # Pybind11 bindings
├── src/                            # C++ source code
│   ├── brain/                      # Neural components
│   ├── memory/                     # Memory systems
│   ├── neuromodulation/             # Neuromodulators
│   ├── cognition/                  # Cognitive functions
│   ├── sensory/                    # Sensory processing
│   ├── motor/                      # Motor system
│   ├── environment/                # Environment interface
│   └── ...
├── tests/                          # Unit tests
├── configs/                        # Configuration files
├── docs/                           # Documentation
└── README.md
```

## Advanced Features

### Custom Configuration Templates

The `createRandomConfig()` function provides a quick way to create configurations with randomized but reasonable settings.

### Brain Benchmarking

The `benchmarkBrain()` function provides a simple way to measure simulation performance.

### Simulation Convenience Functions

The `simulateEpisode()` function encapsulates the complete simulation loop for easy use in scripts.

## Contributing

### Python Development

1. Modify `python/bindings.cpp` for new bindings
2. Update `python/CMakeLists.txt` if needed
3. Run tests to verify changes
4. Update documentation in this file

### Testing

See the `tests/` directory for C++ unit tests. For Python bindings, ensure the module builds correctly and basic functionality works.

## License

MIT

## Authors

Research project — See docs for scientific background.
