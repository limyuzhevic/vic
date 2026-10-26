# NLM (Neural Learning Machine) - Enhanced README

## Quick Start

NLM is an experimental artificial brain project that simulates neural networks with biologically realistic properties. The enhanced version now includes a comprehensive Python interface for easier simulation control.

### Basic Usage

```bash
# Build and install
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .

# Quick test
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); brain.step(0); print('Brain created successfully!')"
```

## What's New in Enhanced Version

### 1. Enhanced Python Interface

The enhanced Python interface provides two main ways to use NLM:

- **Simple/Classic API** (backward compatible)
- **Enhanced/High-level API** (new features and utilities)

### 2. New Features

- **ConfigBuilder**: Fluent API for creating NLM configurations with validation
- **NLM class**: High-level simulation control interface
- **PerformanceMonitor**: Performance tracking and optimization tools
- **Context managers**: Automatic resource management (`brain_context`, `simulation_episode`)
- **Enhanced exceptions**: Better error messages and recovery suggestions
- **Configuration presets**: Ready-to-use configurations for common scenarios

### 3. Backward Compatibility

All existing code continues to work unchanged. The enhanced interface adds new features without breaking existing functionality.

## Installation

### Build Dependencies

```bash
pip install pybind11 scikit-build-core pytest numpy
```

### Build and Install

```bash
# Build C++ components
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Quick Examples

### Example 1: Simple Brain (Classic API)

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print("Simulation complete!")
```

### Example 2: Learning Agent (Enhanced Interface)

```python
import pynlm
from pynlm import NLM, create_educational_simulation

# Method 1: Using high-level NLM class
nlm = NLM()
nlm.setup(world_width=30.0, world_height=30.0, enable_learning=True)
stats = nlm.run(1000, verbose=True)

print(f"Simulation completed: {stats['steps_completed']} steps")
print(f"Efficiency score: {stats['efficiency_score']:.4f}")
```

### Example 3: Configuration Builder (New Feature)

```python
import pynlm
from pynlm import ConfigBuilder

# Create custom configuration
config = (ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(2)
         .set_connection_probability(0.15)
         .enable_curiosity(True)
         .enable_structural_plasticity(True)
         .build())

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 4: Performance Monitoring (New Feature)

```python
import pynlm
from pynlm import create_performance_monitor

# Create simulation
brain, world, agent = pynlm.create_educational_simulation()

# Setup performance monitoring
monitor = create_performance_monitor()

# Run simulation with monitoring
for step in range(500):
    monitor.start_step()
    
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Performance: {perf_stats['avg_step_time_ms']:.1f} ms/step")
print(f"Efficiency: {perf_stats['efficiency_score']:.4f}")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build configuration
├── README.md                         # This file
├── pyproject.toml                    # Python package configuration
├── docs/                             # Documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   ├── EXPERIMENTS.md                # Experiment descriptions
│   ├── LIMITATIONS.md                # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md         # Phase 6 audit
├── src/                              # C++ source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulation systems
│   ├── memory/                       # Memory systems
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                # Visualization
├── tests/                            # Unit tests
├── python/                           # Python bindings source
│   └── bindings.cpp                  # Pybind11 bindings
├── easy_usage.md                     # Easy usage guide
├── HOW_TO_USE.md                     # Detailed usage guide (enhanced)
└── nlm/                             # Enhanced Python interface
    ├── __init__.py                  # Main interface module
    └── core.py                      # Core NLM classes
```

## Key Features

### Neural Computation

- **Real LIF neuron dynamics**: Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Event-driven processing**: Efficient spike-based computation with synaptic delays
- **Multiple plasticity mechanisms**: STDP, Hebbian learning, structural plasticity

### Integrated Systems (Phase 6)

- **Memory systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction system**: Internal forward models and prediction error
- **Cognition systems**: Planning, concept formation, attention
- **Development system**: Age-dependent plasticity and maturation

### Simulation Features

- **World interaction**: 2D environment with perception and action
- **Learning agents**: Agent brain interface for brain-world interaction
- **Checkpointing**: Save/load simulation state
- **Memory replay**: Consolidation and replay of important experiences

### Performance Optimizations

- **Memory pools and event queues**: Efficient memory management
- **SIMD vectorization**: Parallel processing
- **Parallel processing**: Multi-core support
- **Checkpoint system**: Efficient state persistence

## Scientific Background

NLM (熙然, "serene flow") is an experimental computational brain project investigating whether computational systems can develop brain-like properties through interaction with an environment.

### Current Phase

**Phase 6: Final Integration**

All previously developed components are now integrated into a coherent artificial brain system capable of:

- Learning from experience through multiple memory systems
- Exploring environments through curiosity-driven behavior
- Adapting through neuromodulation and plasticity
- Developing over time through age-dependent processes

### Scientific Goals

Phase 6 investigates:

1. **Memory integration**: Whether memory systems can integrate with neural dynamics
2. **Neuromodulation coordination**: Whether neuromodulation can affect plasticity in a coordinated way
3. **Prediction as organizing principle**: Whether prediction can become a central organizing principle
4. **Developmental modulation**: Whether developmental stages can modulate learning
5. **Memory consolidation**: Whether replay can reinforce memory consolidation
6. **Brain loop functionality**: Whether the complete brain loop functions coherently

### Scientific Limitations

NLM is a research project and makes no claims of:

- Accurately reproducing biological brains
- Human intelligence or consciousness
- Human-like reasoning
- Genuine subjective experience

## Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Development Phases

### Phase 1 (Complete)
- Project skeleton and core utilities
- Basic neural interfaces and configuration

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Python Interface API

### Enhanced Interface (Recommended)

#### NLM Class

```python
# High-level simulation control
nlm = NLM()
nlm.setup(world_width=30.0, world_height=30.0, enable_learning=True)
stats = nlm.run(1000, verbose=True)
```

#### ConfigBuilder Class

```python
# Fluent API for configuration with validation
config = (ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(2)
         .set_connection_probability(0.15)
         .enable_curiosity(True)
         .enable_structural_plasticity(True)
         .build())
```

#### PerformanceMonitor Class

```python
# Performance tracking and optimization
monitor = create_performance_monitor()

for step in range(500):
    monitor.start_step()
    # Run simulation step
    monitor.end_step(brain, world)

perf_stats = monitor.get_summary()
```

### Classic Interface (Backward Compatible)

```python
import pynlm

# Legacy API still works
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

### Factory Functions

```python
# Ready-to-use simulation setups
brain, world, agent = create_educational_simulation()

# Configuration presets
research_config = create_research_config()
educational_config = create_educational_config()
simple_config = create_simple_config()
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Synaptic connection probability |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `random_seed` | int | 42 | Random seed for reproducible simulations |

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
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Examples Directory

The repository includes an `examples/` directory with comprehensive demonstration scripts:

- `simple_brain.py` - Basic brain simulation
- `learning_agent.py` - Complete learning simulation
- `performance_monitor.py` - Performance optimization examples

```bash
cd examples
python simple_brain.py
python learning_agent.py
python performance_monitor.py
```

## Building and Running

### Build Instructions

```bash
# Create build directory
cd /path/to/nlm
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests (if available)
ctest --output-on-failure
```

### Run Demos

```bash
# Phase 6 integration demo
cd build
./nlm_phase6_demo

# Python examples
cd ..
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Brain created successfully!')"
```

## Performance Considerations

### Optimization Tips

1. **Use simple configurations for testing**: `create_simple_config()`
2. **Disable learning for speed**: `enable_reward_modulation(False)`
3. **Monitor performance**: Use `PerformanceMonitor()`
4. **Use context managers**: `with simulation_episode(...)`
5. **Leverage presets**: Use predefined configurations

### Performance Monitoring

```python
from nlm import create_performance_monitor

monitor = create_performance_monitor()

for step in range(1000):
    monitor.start_step()
    # Run simulation
    monitor.end_step(brain, world)

stats = monitor.get_summary()
print(f"Average step time: {stats['avg_step_time_ms']:.1f} ms")
print(f"Efficiency score: {stats['efficiency_score']:.4f}")
```

## Testing

Run unit tests to ensure the NLM system works correctly:

```bash
cd build
ctest --output-on-failure
```

## Contributing

### Development Guidelines

1. **Follow the project structure** and architectural philosophy
2. **Write comprehensive tests** for new features
3. **Use the enhanced Python interface** for new code
4. **Maintain backward compatibility** with existing APIs
5. **Document new features** thoroughly

### New Contributors

1. Fork the repository
2. Create a new branch for your feature
3. Implement your changes following the guidelines
4. Write tests for new functionality
5. Submit a pull request

## Support and Documentation

### Documentation

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/LIMITATIONS.md` - Scientific limitations
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 audit report

### Python Interface

For comprehensive Python documentation:
- `easy_usage.md` - Quick start and examples
- `HOW_TO_USE.md` - Detailed API documentation (enhanced)

### Online Resources

- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues

## Acknowledgments

This project is supported by the research team at the NLM Project. Special thanks to all contributors who have helped develop and test the NLM system.

## License

MIT License - See `LICENSE` file for details.

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io

Enjoy exploring the experimental artificial brain! 🚀

---

*Enhanced Python interface version 0.1.0*
*Backward compatible with existing code*
*New features for easier simulation control*