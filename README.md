# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to simulate artificial developmental brains. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features
- **Phase-based Development**: Evolution from skeleton to complete functional brain
- **Research-Grade Neural Simulation**: LIF neurons with event-driven spikes, multiple plasticity mechanisms
- **Dual APIs**: Complete C++ access and Python bindings for ease of use
- **Integrated Brain Systems**: Memory, neuromodulation, prediction, cognition, development
- **World Interaction**: Complete agent-environment loop with sensory input and motor output
- **Checkpointing & Replay**: Save/load brain states and replay experiences

## Quick Start

### For Python Users (Recommended)

```bash
# Install from source (one-time setup)
pip install pybind11 scikit-build-core pytest numpy
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### For C++ Users

```bash
# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build system
├── pyproject.toml                    # Python package definition
├── README.md                         # This documentation
├── easy_usage.md                     # Python beginner guide
├── HOW_TO_USE.md                     # Comprehensive usage guide
├── configs/                          # Configuration files
│   └── default.cfg                   # Default configuration
├── docs/                             # Technical documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   ├── EXPERIMENTS.md               # Experiment descriptions
│   ├── LIMITATIONS.md               # Current limitations
│   └── PHASE6_FINAL_AUDIT.md       # Final integration audit
├── src/                             # C++ source code
│   ├── core/                        # Core utilities
│   ├── brain/                       # Neural components
│   ├── memory/                      # Memory systems
│   ├── neuromodulation/              # Neuromodulators
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   ├── prediction/                   # Prediction systems
│   ├── development/                  # Developmental system
│   ├── plasticity/                   # Plasticity rules
│   ├── environment/                  # Environment interface
│   ├── agent/                        # Agent system
│   ├── experiments/                  # Experiment framework
│   └── visualization/                 # Visualization interface
├── python/                          # Python bindings source
│   └── bindings.cpp                 # Complete pybind11 bindings
└── tests/                          # Test suite
```

## Current Status

**Phase 6: FINAL INTEGRATION** - All major systems are implemented and integrated into a coherent artificial brain.

### Achieved in Phase 6:
- ✓ Memory systems (working, episodic, semantic, procedural, associative)
- ✓ Neuromodulation (dopamine, curiosity, novelty detection, prediction error)
- ✓ Prediction system with error signals and confidence tracking
- ✓ Cognition systems (neural planner, concept formation, attention, planning)
- ✓ Development system with age-dependent plasticity
- ✓ Persistence (checkpoint save/load, brain state serialization)
- ✓ Replay and consolidation (episodic memory replay, sleep/rest cycle)

## Building

### C++ Build Instructions

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake (Release build recommended)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Installation

```bash
# Build and install Python package
pip install .  # Install from wheel
# or
pip install -e .  # Install in development mode

# Build only (without installing)
python -m build

# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy
```

### Build Outputs

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running

### C++ Executables

```bash
# Phase 2: Real Neural Computation (main executable)
./nlm

# Phase 3: Basic world interaction demo
./nlm_phase3_demo

# Phase 4: Advanced agent demo
./nlm_phase4_demo

# Phase 6: Full integration experiment
./nlm_phase6_demo
```

### Python Examples

#### Minimal Example

```python
import pynlm

# Create and initialize a virtual brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

#### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(num_steps):
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    return {
        'total_reward': totalReward,
        'avg_firing_rate': brain.getAverageFiringRate(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage()
    }

# Run simulation
results = run_agent_simulation(1000)
print("Simulation results:", results)
```

## Python API Reference

### Core Classes

#### Brain Class
- `createBrain(config)` - Create a virtual brain
- `initialize()` - Start up the brain (call before step)
- `step(step_number, time=None)` - Process one simulation step
- `reset()` - Reset brain state
- `save(filename)` - Save state to file
- `load(filename)` - Load state from file
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getDevelopmentalStage()` - Get current developmental stage

#### Config Class
- `createDefaultConfig()` - Create default configuration
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value
- `loadFromFile(filename)` - Load from file
- `saveToFile(filename)` - Save to file

#### AgentBrain Class
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward learning
- `updateDevelopment(time)` - Update development
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level

#### SimpleWorld Class
- `createSimpleWorld()` - Create world instance
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world state
- `update(time)` - Update world by time
- `getSensoryPercept()` - Get sensory input
- `applyMotorCommand(action, time)` - Apply action
- `getAgentBody()` - Get agent body
- `addObject(object)` - Add world object
- `removeObject(x, y)` - Remove object

### Enumerations

#### Neuron Types
- `NeuronType.Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

#### Developmental Stages
- `DevelopmentalStage.Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

#### Action Types
- `ActionType.MoveForward`, `TurnLeft`, `TurnRight`, `Interact`, `Eat`

#### World Object Types
- `WorldObjectType.Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

## Configuration Reference

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Features

### For Advanced Users

The following advanced features are available:

1. **Checkpointing and Persistence**
   ```python
   brain.save("checkpoint.bin")
   brain.load("checkpoint.bin")
   ```

2. **Memory Management**
   ```python
   # Access memory systems
   brain.getWorkingMemory()
   brain.getEpisodicMemory()
   brain.getSemanticMemory()
   brain.getProceduralMemory()
   brain.getAssociativeMemory()
   ```

3. **Neuromodulation Control**
   ```python
   brain.getDopamine()
   brain.getCuriosity()
   brain.getNovelty()
   brain.getPredictionError()
   ```

4. **Development Control**
   ```python
   brain.getDevelopmentSystem()
   brain.getSynaptogenesis()
   brain.getPruning()
   brain.getMaturation()
   ```

5. **Prediction and Planning**
   ```python
   brain.getPredictionSystem()
   brain.getNeuralPlanner()
   brain.getConceptFormation()
   brain.getAttention()
   ```

### Configuration Customization

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -45.0)

# Enable specific subsystems
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## Development Guidelines

### Adding New Features

1. **For Python Extensions**: Extend the `bindings.cpp` file
2. **For C++ Extensions**: Add new source files in appropriate `src/` subdirectories
3. **For Configuration**: Add new keys to `configs/default.cfg`
4. **For Testing**: Add new test files in `tests/`

### Building Your Own Experiments

```bash
# Create new experiment directory
mkdir -p src/experiments/MyExperiment
cp -r src/experiments/Phase6IntegratedExperiment.cpp src/experiments/MyExperiment/

# Modify experiment for your use case
# Add to CMakeLists.txt to build your experiment
```

### Documentation

- Read `docs/ARCHITECTURE.md` for system architecture
- Read `docs/SCIENCE.md` for scientific background
- Read `docs/EXPERIMENTS.md` for experiment descriptions
- Read `docs/LIMITATIONS.md` for current limitations
- Read `docs/PHASE6_FINAL_AUDIT.md` for final integration audit

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration values are valid

### Performance Issues

**Memory usage high:**
- Reduce `brain.neuron_count` in configuration
- Increase `simulation_timestep` for less granular time steps

**Simulation slow:**
- Use Release build with `-j$(nproc)` for parallel compilation
- Consider reducing `max_simulation_steps`

## Contributing

### Project Philosophy

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No claim of consciousness or human-like cognition

### Code Quality Guidelines

1. Follow existing naming conventions and coding style
2. Add comprehensive comments for new modules
3. Include unit tests for new functionality
4. Update documentation when adding new features
5. Maintain backward compatibility where possible

### Research Collaboration

For research collaboration, please reference:
- This README for project overview
- `docs/SCIENCE.md` for theoretical background
- `docs/EXPERIMENTS.md` for experiment protocols
- `docs/LIMITATIONS.md` for known limitations

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Version Information

- **Version**: 0.1.0 (Alpha)
- **Build Date**: $(date)
- **Documentation**: This README
- **Python Package**: `pynlm` 0.1.0
- **C++ Version**: C++20

## Support

For support, questions, or bug reports:
1. Check existing documentation
2. Review code for common issues
3. Test with minimal reproduction cases
4. Report issues with complete build environment information