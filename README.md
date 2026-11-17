# NLM - Documentation and Usage Guide

## Overview

NLM (熙然) is an experimental computational brain project focused on creating a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Status**: Phase 6: Final Integration (2026)

## Major Improvements in This Version

### 1. Critical Bug Fixes

#### Configuration File Bug Fix (CONFIG-001)
- **Issue**: Duplicate `dopamine_baseline` key in `configs/default.cfg` (values: 0.0 and 0.1)
- **Fix**: Removed duplicate key, keeping the intended value (0.1)
- **Impact**: Ensures proper dopamine baseline configuration

### 2. Checkpoint System Improvements

#### CheckpointWriter Placeholder Methods (CHKP-002)
- **Issue**: Placeholder methods in `CheckpointWriter` class returned hardcoded values
- **Fix**: Implemented proper functionality:
  - `writeHeader()`: Now writes actual checkpoint header with metadata
  - `writeSectionHeader()`: Writes section headers with proper checksums
  - `padToAlignment()`: Implements file alignment for I/O optimization
- **Impact**: Robust checkpoint serialization/deserialization with proper error handling

#### Code Organization Improvements
- **Issue**: Complex checkpoint logic in single method
- **Fix**: Refactored `finalize()` method to use proper write methods
- **Impact**: Better maintainability and debugging capabilities

### 3. Command-Line Interface Enhancements

#### Enhanced Argument Parsing (CLI-003)
- **Issue**: Basic `--key=value` parsing only
- **Fix**: Added comprehensive CLI features:
  - Help/Version flags (`--help`, `-h`, `--version`)
  - Configuration file option (`--config`, `-c`)
  - Log file option (`--logfile`, `-l`)
  - Seed and steps configuration (`--seed`, `-s`, `--steps`, `-t`)
  - Type-aware value parsing
  - Unknown argument warning
- **Impact**: User-friendly configuration management with extensive options

### 4. Code Quality Improvements

#### Code Organization Refactoring
- **Issue**: `main.cpp` was 435 lines with complex test logic mixed together
- **Fix**: Extracted test logic into organized, modular structure
- **Impact**: Better code organization and maintainability

#### Enhanced Logging System
- **Issue**: Basic logging with limited progress reporting
- **Fix**: Enhanced logging system with better progress tracking
- **Impact**: More informative and actionable log output

### 5. Documentation and Help Systems

#### Comprehensive Documentation (DOCS-004)
- **Issue**: Outdated and incomplete documentation
- **Fix**: Enhanced documentation with:
  - Detailed build instructions
  - Configuration options
  - Usage examples
  - Troubleshooting guide
  - Architecture overview
- **Impact**: Improved user experience and onboarding

## Building Instructions

### Prerequisites

```bash
# Ubuntu/Debian
cmake curl git
pip3 install pybind11 scikit-build-core pytest numpy

# macOS
brew install cmake git python3
pip3 install pybind11 scikit-build-core pytest numpy

# Windows (using scoop)
scoop install cmake git python
pip install pybind11 scikit-build-core pytest numpy
```

### Standard Build

```bash
# Navigate to project root
cd /path/to/nlm

# Create and enter build directory
mkdir build
cd build

# Configure CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets with parallel execution
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Bindings Build

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .
```

### Alternative Python Build

```bash
# Install build tools
pip install build

# Create source distribution
python -m build

# Install from distribution
pip install dist/*.whl
```

## Build Outputs

### Executable Targets

| Target | Description |
|--------|-------------|
| `nlm` | Phase 2 demo - Basic neural computation validation |
| `nlm_phase3_demo` | Phase 3 demo - World interaction and learning |
| `nlm_phase4_demo` | Phase 4 demo - Cognitive capabilities demonstration |
| `nlm_phase5_demo` | Phase 5 demo - Performance optimizations and sandboxing |
| `nlm_phase6_demo` | Phase 6 integration test - Full brain system verification |
| `nlm_phase6_text_demo` | Phase 6 text demo - Advanced cognitive capabilities |
| `nlm_benchmark` | Performance benchmark suite |

### Library Targets

| Library | Purpose |
|---------|---------|
| `nlm_core` | Core brain components (neurons, synapses, etc.) |
| `nlm_agent` | Agent brain interface (sensory, motor, etc.) |
| `nlm_world` | World simulation and environment |
| `nlm_phase3` | Phase 3 experiment library |
| `nlm_phase4` | Phase 4 experiment library |
| `nlm_phase5` | Phase 5 experiment library |
| `nlm_phase6` | Phase 6 integration library |

## Command-Line Usage

### Basic Usage

```bash
# Run the main Phase 2 demo
./nlm

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run with custom configuration
./nlm --config=my_config.cfg
```

### Advanced Options

```bash
# Show help message
./nlm --help
./nlm -h

# Show version information
./nlm --version

# Set random seed for reproducible experiments
./nlm --seed=12345

# Set maximum simulation steps
./nlm --steps=5000

# Specify log file
./nlm --logfile=simulation.log

# Set configuration file
./nlm --config=production.cfg

# Override configuration parameters
./nlm --neuron_count=2000 --connection_probability=0.05

# Short option format
./nlm -s 999 -c custom.cfg
```

### Configuration File Format

NLM uses a simple key=value format:

```ini
# Example configuration file
brain.neuron_count = 1000
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromod.dopamine.scale = 1.0
log_level = INFO
log_to_file = true
log_filename = nlm.log
max_simulation_steps = 10000
```

### Command-Line Overrides

Command-line arguments override configuration file values:

```bash
# Configuration file sets defaults
cat > my_config.cfg
echo "brain.neuron_count = 1000" > my_config.cfg

# Command line overrides
./nlm --config=my_config.cfg --brain.neuron_count=2000
```

Result: `brain.neuron_count` will be 2000 (from command line)

## Python API Usage

### Installation Verification

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Basic Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
print("Running simulation for 1000 steps...")
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation

```python
import pynlm

def run_complete_simulation(num_steps=1000):
    """Run a complete agent simulation with all subsystems."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
simulation_result = run_complete_simulation(1000)
```

## Key Classes and Methods

### Brain Class

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain instance |
| `initialize()` | Initialize the brain with default state |
| `step(step_number, time)` | Run one simulation step |
| `reset()` | Reset brain to initial state |
| `save(filepath)` | Save brain state to checkpoint file |
| `load(filepath)` | Load brain state from checkpoint file |
| `addRegion(name)` | Add a new neural region |
| `getRegion(id)` | Get a region by ID |

### Config Class

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create a new default configuration |
| `set(key, value)` | Set configuration value |
| `get(key)` | Get configuration value |
| `getOr(key, default)` | Get value or default if not set |
| `has(key)` | Check if configuration key exists |
| `loadFromFile(filepath)` | Load configuration from file |
| `saveToFile(filepath)` | Save configuration to file |

### AgentBrain Class

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent brain interface |
| `initialize(world)` | Initialize agent with world interface |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Get motor command from brain |
| `applyRewardModulation(reward, predicted)` | Apply reward modulation |
| `updateDevelopment(time)` | Update developmental state |

### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create a new simple world |
| `configure(width, height, ...)` | Configure world dimensions |
| `update(timestep)` | Update world state |
| `reset()` | Reset world to initial state |
| `getSensoryPercept()` | Get sensory percept from world |
| `applyMotorCommand(action, time)` | Apply motor command to world |

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Initial synaptic weight mean |
| `brain.initial_weight_std` | float | 0.1 | Initial synaptic weight standard deviation |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (potentiation) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (depression) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Logging Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | INFO | Minimum log level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `log_to_file` | bool | false | Enable file logging |
| `log_filename` | string | nlm.log | Log file name |

## Performance Considerations

### Memory Usage

- **Brain Memory**: Scales with `neuron_count × (neuron_data_size)`
- **Synaptic Memory**: Scales with `neuron_count² × connection_probability`
- **Memory Optimization**: Phase 5 includes memory pool optimizations

### CPU Performance

- **Simulation Speed**: Optimized with event-driven spike propagation
- **Plasticity**: STDP and Hebbian rules implemented efficiently
- **Multithreading**: Phase 5 includes SIMD vectorization and parallel processing

### Checkpoint Performance

- **I/O Efficiency**: Binary format with compression support
- **Memory Efficiency**: Stream-based writing for large checkpoints
- **Validation**: Built-in checksum verification

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

**Missing dependencies:**
```bash
# Install common build tools
# Ubuntu: sudo apt-get install build-essential cmake git
# macOS: brew install cmake git
# Windows: Use Chocolatey or Scoop
```

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration values are within valid ranges

**Slow performance:**
- Reduce `neuron_count` for testing
- Use smaller `max_simulation_steps`
- Disable unnecessary plasticity mechanisms

### Common Errors and Solutions

| Error | Solution |
|-------|----------|
| "Unknown command line argument" | Use `--help` for valid options |
| "Failed to create checkpoint file" | Check file permissions and disk space |
| "Configuration key not found" | Use `has()` method to check existence |
| "Neuron count mismatch in checkpoint" | Ensure same configuration when loading |

## Advanced Usage

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("log_level", "DEBUG")
config.set("log_to_file", True)
config.set("log_filename", "my_simulation.log")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multiple Simulations

```python
import pynlm
import os

def run_multiple_simulations(conditions, num_runs=10):
    """Run multiple simulations with different conditions."""
    
    results = []
    
    for i, condition in enumerate(conditions):
        print(f"Running simulation {i+1}/{len(conditions)}")
        
        for run in range(num_runs):
            # Create configuration for this run
            config = pynlm.createDefaultConfig()
            
            # Apply condition-specific settings
            for key, value in condition.items():
                config.set(key, value)
            
            # Add run-specific seed for reproducibility
            seed = condition.get("seed", 42) + run
            config.set("random_seed", seed)
            
            # Create and run brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(condition.get("steps", 1000)):
                brain.step(step)
            
            # Save result
            filename = f"results_run_{i}_{run}.bin"
            brain.save(filename)
            
            # Collect metrics
            result = {
                "condition": condition,
                "run": run,
                "seed": seed,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "filename": filename
            }
            
            results.append(result)
            print(f"  Run {run}: {result['total_spikes']} spikes")
    
    return results

# Define different experimental conditions
experimental_conditions = [
    {"brain.neuron_count": 1000, "steps": 500},
    {"brain.neuron_count": 2000, "steps": 1000},
    {"brain.neuron_count": 5000, "steps": 2000},
]

# Run multiple simulations
results = run_multiple_simulations(experimental_conditions, num_runs=5)
```

### Real-Time Monitoring

```python
import pynlm
import time
import threading

def monitor_simulation(brain, interval=100):
    """Monitor simulation progress in real-time."""
    
    def monitor():
        start_time = time.time()
        last_spikes = brain.getTotalSpikeCount()
        
        while not brain.isTerminated():  # Assuming such method exists
            time.sleep(interval * 0.001)  # Convert to seconds
            
            current_spikes = brain.getTotalSpikeCount()
            spikes_per_second = (current_spikes - last_spikes) / interval
            
            print(f"[MONITOR] Spikes/s: {spikes_per_second:.1f}, "
                  f"Total: {current_spikes}, "
                  f"Firing: {brain.getFiringNeuronCount()}")
            
            last_spikes = current_spikes
    
    monitor_thread = threading.Thread(target=monitor, daemon=True)
    monitor_thread.start()

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start monitoring
monitor_simulation(brain, interval=50)

# Run simulation
for step in range(1000):
    brain.step(step)
```

## Future Development Roadmap

### Phase 8: Advanced Cognitive Capabilities

1. **Enhanced Memory Systems**
   - Long-term memory consolidation
   - Semantic memory representation
   - Working memory optimization

2. **Complex Decision Making**
   - Multi-step planning
   - Risk assessment
   - Value-based decision making

3. **Social Learning**
   - Imitation learning
   - Cultural transmission
   - Social cooperation

### Phase 9: Biological Plausibility

1. **Biologically Realistic Neurons**
   - Hodgkin-Huxley dynamics
   - Ion channel modeling
   - Dendritic computation

2. **Neuroanatomical Structure**
   - Layered cortical structures
   - Feedback connections
   - Modulatory systems

3. **Developmental Processes**
   - Gene-environment interaction
   - Experience-dependent development
   - Neurodegeneration and repair

### Phase 10: Real-World Applications

1. **Embodied Intelligence**
   - Physical robot control
   - Sensorimotor integration
   - Adaptive behavior

2. **Healthcare Applications**
   - Neural prosthesis control
   - Disease modeling
   - Treatment optimization

3. **Education and Training**
   - Personalized learning
   - Skill acquisition
   - Performance monitoring

## Contact and Support

For questions, issues, or support:

- **Documentation Issues**: Please report any documentation errors to the NLM team
- **Bug Reports**: Submit issues through the NLM GitHub repository
- **Feature Requests**: Submit enhancement requests through the NLM GitHub repository
- **Technical Support**: Contact the NLM development team at support@nlm-project.org

## License

NLM is licensed under the MIT License. See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project. All rights reserved.

---

*Document last updated: November 17, 2026*
*Phase 6: Final Integration*
*Version 0.1.0 development branch*

## Acknowledgments

This project is made possible by contributions from:

- Phase 1: Core framework development
- Phase 2: Neural computation implementation
- Phase 3: World interaction and learning
- Phase 4: Cognitive capabilities
- Phase 5: Performance optimizations
- Phase 6: Final integration and comprehensive testing

Special thanks to all contributors, testers, and researchers who have advanced the NLM project forward.

---

*This documentation is part of the NLM Phase 6 final integration effort. The project continues to evolve with new features, improvements, and research discoveries.*
