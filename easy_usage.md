# NLM — 熙然 (Neural Learning Machine)

## Quick Start Guide

This document provides a comprehensive overview of how to get started with NLM (熙然), the experimental artificial developmental brain project.

### Key Features

- **Artificial Developmental Brain**: Neural system that develops and adapts over time
- **Spiking Neural Network**: Leaky Integrate-and-Fire (LIF) neurons with event-driven computation
- **Full Integration**: Memory, neuromodulation, prediction, cognition, and development systems
- **Python Bindings**: Easy-to-use Python API for simulation and experimentation
- **C++ Core**: High-performance neural computation engine

## Part 1: Building from Source

### Prerequisites

```bash
# C++ Compiler:
- GCC 10+, Clang 12+, or MSVC 2019+

# CMake:
- Version 3.16 or higher

# Python (for Python bindings):
- Python 3.8+
- pip

# Optional build dependencies:
- pybind11 >= 2.11.0
- scikit-build-core
```

### Build Steps

```bash
# Navigate to the project directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (use all available CPU cores)
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

## Part 2: Using the Python Library

### Quick Installation (Recommended)

```bash
# Install using pip (requires scikit-build-core and pybind11)
pip install scikit-build-core pybind11 numpy pytest
pip install .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Installation in Development Mode

```bash
pip install -e .
```

### Verification

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print("✓ NLM Python bindings work correctly!")
```

## Part 3: Quick Usage Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Example 3: Learning Experiment

```python
import pynlm

# Create configuration with plasticity enabled
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a learning experiment
print("Starting learning experiment...")
for step in range(2000):
    # Inject input pattern
    for i in range(20):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    
    # Step the brain
    brain.step(step)
    
    # Print statistics every 200 steps
    if step % 200 == 0:
        print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"E/I ratio={brain.getExcitationInhibitionRatio():.2f}")

print("Learning experiment complete!")
```

### Example 4: Checkpoint System

```python
import pynlm
import os

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(500):
    brain.step(step)

# Save checkpoint
checkpoint_file = "nlm_checkpoint.bin"
success = brain.save(checkpoint_file)
if success:
    print(f"✓ Brain state saved to {checkpoint_file}")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"✓ Cleaned up checkpoint file")
else:
    print("✗ Failed to save checkpoint")
```

## Part 4: Running NLM Applications

### Phase 2: Basic Neural Computation

```bash
# Navigate to build directory
cd build

# Run Phase 2 demo (basic neural computation)
./nlm_phase2_demo
```

### Phase 3: World Interaction

```bash
# Run Phase 3 demo (world interaction)
./nlm_phase3_demo
```

### Phase 4: Cognitive Capabilities

```bash
# Run Phase 4 demo (cognitive abilities)
./nlm_phase4_demo
```

### Phase 6: Final Integration

```bash
# Run Phase 6 demo (integration test)
./nlm_phase6_demo
```

## Part 5: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |

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

## Part 6: Performance Tips

### For Large-Scale Simulations

1. **Use Release Build**: Always build with `-DCMAKE_BUILD_TYPE=Release`
2. **Parallel Processing**: Use `make -j$(nproc)` for multi-core compilation
3. **Memory Management**: Use checkpoint system for long simulations
4. **Optimization Flags**: Add `-DNLM_OPTIMIZED` for additional optimizations

### For Real-Time Applications

1. **Event-Driven Processing**: NLM uses efficient event-driven spike propagation
2. **Lazy Evaluation**: Only compute when necessary
3. **Async Processing**: Consider running in separate threads

## Part 7: Troubleshooting

### Common Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Part 8: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Final integration audit

## Support

For issues and support, please refer to the project documentation or GitHub repository.
