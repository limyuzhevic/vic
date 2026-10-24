# NLM (Neural Learning Machine) - Documentation

**Version:** 0.1.0 | **Phase 6: Final Integration**

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project designed to investigate computational brain-like systems. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Phase-based Development**: Complete implementation through 6 phases
- **Integrated Brain Systems**: Memory, neuromodulation, prediction, cognition, development
- **PyPI Package**: Python bindings available via `pip install pynlm`
- **Advanced Demo**: `nlm_phase6_demo` with comprehensive testing and validation
- **Configuration Validation**: Strict validation with detailed error reporting
- **Performance Monitoring**: Real-time metrics and optimization tracking

## Quick Start Guide

### 1. Installation

```bash
# Clone the repository
cd /path/to/nlm

# Build the project (recommended)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings (if using pybind11)
cd python
pip install -e .
```

### 2. Running the Demo

```bash
# Basic demo (quick integration test)
./nlm_phase6_demo --quick

# Full comprehensive test
./nlm_phase6_demo --full --verbose

# Custom configuration
./nlm_phase6_demo --neurons 2000 --steps 5000 --verbose

# With checkpointing disabled
./nlm_phase6_demo --quick --no-checkpointing

# Run diagnostics
./nlm_phase6_demo --diagnostics
```

### 3. Python Usage

```python
import pynlm

# Create a brain with custom configuration
brain = pynlm.Brain(neuron_count=1000, region_count=2)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Access brain systems
working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()
dopamine = brain.get_dopamine()
curiosity = brain.get_curiosity()

# Save and load checkpoints
brain.save("checkpoint.bin")
brain2 = pynlm.Brain("checkpoint.bin")
brain2.load("checkpoint.bin")

# Run integration test
experiment = pynlm.Phase6IntegratedExperiment()
result = experiment.run()
print(f"Integration Status: {result.get_system_integration_status()}")
```

## Architecture

### Brain Components

The NLM brain consists of several integrated systems:

1. **Memory Systems**
   - Working Memory: Short-term storage and manipulation
   - Episodic Memory: Experience recording and replay
   - Semantic Memory: Knowledge representation
   - Procedural Memory: Skills and habits
   - Associative Memory: Pattern associations

2. **Neuromodulation**
   - Dopamine: Reward prediction and learning
   - Curiosity: Exploration motivation
   - Novelty: Change detection
   - Prediction Error: Learning signals
   - Reward: Environmental feedback

3. **Cognition**
   - Neural Planner: Action sequence evaluation
   - Concept Formation: Pattern abstraction
   - Attention: Competitive selection
   - Prediction System: Forward models

4. **Development**
   - Synaptogenesis: New connection formation
   - Pruning: Connection elimination
   - Maturation: Parameter optimization
   - Developmental System: Age-dependent plasticity

5. **Environment**
   - SimpleWorld: Grid-based environment
   - Sensory Processing: Vision, audio, internal signals
   - Motor System: Action execution
   - Visualization: Output and monitoring

### Integration Flow

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                          ↓                    ↓
MEMORY / PREDICTION ← NEUROMODULATION → ACTION SELECTION
    ↓                          ↓                    ↓
ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
    ↓                          ↓                    ↓
REWARD / SURPRISE / ERROR ← PLASTICITY → MEMORY / DEVELOPMENT
    ↓                          ↓                    ↓
CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## Configuration System

### Configuration File Format

NLM uses a simple key=value configuration file format:

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration Validation

The configuration system includes comprehensive validation:

1. **Value Range Validation**: Numeric values must be within valid ranges
2. **Key Format Validation**: Keys must follow naming conventions
3. **Type Validation**: Values must match expected types
4. **Relationship Validation**: Cross-parameter validation (e.g., regions < neurons)
5. **Known Keys**: Only predefined configuration keys are allowed

### Available Configuration Keys

| Key | Type | Range | Description |
|-----|------|-------|-------------|
| neuron_count | int64_t | 1-1000000 | Number of neurons |
| region_count | int64_t | 1-100 | Number of brain regions |
| connection_probability | float | 0.0-1.0 | Connection probability |
| simulation_timestep | double | 0.0001-0.1 | Simulation timestep |
| plasticity_learning_rate | double | 0.0-10.0 | Learning rate |
| stdp_ltp_weight | double | 0.0-1.0 | STDP LTP weight |
| stdp_ltd_weight | double | 0.0-1.0 | STDP LTD weight |
| log_level | string | DEBUG, INFO, WARNING, ERROR, FATAL | Logging level |
| environment_width | int64_t | 1-1000 | Environment width |
| environment_height | int64_t | 1-1000 | Environment height |
| max_simulation_steps | int64_t | 1-1000000000 | Max simulation steps |
| visualization_update_rate | double | 0.0-1000.0 | Visualization update rate |
| reward_discount_factor | double | 0.0-1.0 | Reward discount factor |

### Command Line Interface

The demo supports comprehensive command-line options:

```bash
nlm_phase6_demo [OPTIONS]

Basic Options:
  -v, --verbose           Enable verbose output
  -q, --quick             Run quick integration verification only
  -f, --full              Run full comprehensive test
  --diagnostics           Run detailed system diagnostics
  -h, --help              Show this help message

Configuration Options:
  --neurons N             Number of neurons (default: 500)
  --steps S               Number of simulation steps (default: 1000)
  --no-checkpointing      Disable checkpoint testing
  --no-replay             Disable replay testing
  --no-development        Disable development system
  --checkpoint FILE        Checkpoint file path (default: ./checkpoint_test.bin)

Output Options:
  --export DIR            Export metrics to directory (default: ./demo_output)
```

## Python API Reference

### Core Classes

#### Brain
Main interface for neural simulation.

**Methods:**
- `__init__([neuron_count, region_count, connection_prob])` - Constructor
- `initialize()` - Initialize brain with current configuration
- `reset()` - Reset and reinitialize brain
- `step(step, time)` - Execute one simulation step
- `inject_current_to_neurons(neuron_type, current)` - Inject current into neurons
- `get_total_neuron_count()` - Get total neuron count
- `get_total_synapse_count()` - Get total synapse count
- `get_firing_neuron_count()` - Get firing neuron count
- `get_average_firing_rate()` - Get average firing rate
- `get_total_spike_count()` - Get total spike count
- `get_working_memory()` - Get working memory
- `get_episodic_memory()` - Get episodic memory
- `get_dopamine()` - Get dopamine system
- `get_curiosity()` - Get curiosity system
- `get_prediction_system()` - Get prediction system
- `get_development_system()` - Get development system
- `save(filename)` - Save brain state to file
- `load(filename)` - Load brain state from file
- `log_status()` - Log current brain status

#### Agent
Interface for agent-environment interaction.

**Methods:**
- `__init__(brain)` - Constructor
- `initialize(world)` - Initialize agent with world
- `enable_reward_modulation(enable)` - Enable reward modulation
- `enable_structural_plasticity(enable)` - Enable structural plasticity
- `enable_development(enable)` - Enable development
- `enable_curiosity(enable)` - Enable curiosity
- `observe()` - Get sensory perception
- `process_sensory_input(percept)` - Process sensory input
- `decode_motor_command()` - Get motor command
- `apply_reward_modulation(reward, error)` - Apply reward modulation
- `update_development(dt)` - Update development

#### World
Environment simulation interface.

**Methods:**
- `initialize(width, height)` - Initialize world
- `observe(region)` - Get observation
- `apply_action(region, command)` - Apply action
- `compute_reward(region)` - Compute reward
- `get_width()` - Get world width
- `get_height()` - Get world height

#### Phase6IntegratedExperiment
Integration testing framework.

**Methods:**
- `run(config)` - Run complete integration test
- `verify_integration()` - Verify basic integration
- `test_memory_integration()` - Test memory systems
- `test_neuromodulation_integration()` - Test neuromodulation
- `test_checkpointing()` - Test checkpoint save/load
- `test_replay()` - Test replay system

#### Config
Configuration management.

**Methods:**
- `__init__([filename])` - Constructor
- `set(key, value)` - Set configuration value
- `get_string(key, default)` - Get string value
- `get_int(key, default)` - Get integer value
- `get_double(key, default)` - Get double value
- `get_float(key, default)` - Get float value
- `load_from_file(filename)` - Load from file
- `load_from_args(argc, argv)` - Load from command line

### Enums and Types

#### NeuronType
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons  
- `Internal` - Internal neurons

#### MotorCommand
- `action` - Action type
- `intensity` - Action intensity
- `duration` - Action duration

#### SensoryPercept
- `vision` - Visual perception
- `audio` - Audio perception
- `internal_signals` - Internal signals

## Advanced Usage

### Performance Optimization

1. **Configure for Performance:**
   ```ini
   # Use larger neuron counts for better neural dynamics
   neuron_count = 5000
   region_count = 4
   
   # Optimize simulation parameters
   simulation_timestep = 0.0005
   max_simulation_steps = 5000
   
   # Enable performance features
   stdp_ltp_weight = 0.02
   stdp_ltd_weight = 0.018
   ```

2. **Memory Management:**
   ```python
   # Use checkpointing for long simulations
   brain.save("checkpoint_1000.bin")  # Save every 1000 steps
   
   # Monitor memory usage
   memory = brain.get_working_memory()
   print(f"Active traces: {memory.getActiveTraces()}")
   ```

### Custom Experiments

1. **Create Custom Experiment:**
   ```python
   class CustomExperiment:
       def __init__(self, brain):
           self.brain = brain
           self.results = {}
       
       def run_experiment(self, steps):
           for step in range(steps):
               self.brain.step(step, step * 0.001)
               self.collect_metrics(step)
           return self.results
       
       def collect_metrics(self, step):
           self.results[f"step_{step}"] = {
               'firing_rate': self.brain.get_average_firing_rate(),
               'spike_count': self.brain.get_total_spike_count(),
               'memory_traces': self.brain.get_working_memory().getActiveTraces()
           }
   ```

2. **Export Results:**
   ```python
   # Export results for analysis
   import json
   with open("experiment_results.json", "w") as f:
       json.dump(experiment_results, f, indent=2)
   ```

### Debugging and Troubleshooting

1. **Common Issues:**
   - **No spikes generated**: Check neuron parameters and current injection
   - **Memory overflow**: Reduce neuron count or increase simulation timestep
   - **Integration failures**: Check configuration values
   - **Checkpoint errors**: Verify file permissions and disk space

2. **Debugging Tools:**
   ```bash
   # Run with verbose output for detailed logging
   ./nlm_phase6_demo --verbose --diagnostics
   
   # Check configuration validation
   ./nlm_phase6_demo --quick --neurons 100
   
   # Monitor performance metrics
   ./nlm_phase6_demo --full --export ./metrics
   ```

## Development Guide

### Building from Source

1. **Prerequisites:**
   - CMake 3.16 or higher
   - C++20 compiler (GCC 9+, Clang 10+, or MSVC 19+)
   - Python 3.8+ (optional, for Python bindings)

2. **Build Instructions:**
   ```bash
   # Create build directory
   mkdir build
   cd build
   
   # Configure CMake
   cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DBUILD_PYTHON_BINDINGS=ON \
             -DBUILD_TESTS=ON
   
   # Build all targets
   make -j4
   
   # Build specific targets
   make nlm_phase6_demo
   make pynlm
   
   # Run tests
   make test
   ```

3. **Python Bindings:**
   ```bash
   # Build Python module
   cd python
   pip install -e .
   
   # Test installation
   python -c "import pynlm; print(pynlm.__version__)"
   ```

### Contributing

1. **Code Style:**
   - Use C++20 standard
   - Follow existing code conventions
   - Add comments for complex logic
   - Use meaningful variable names

2. **Testing:**
   - Write unit tests for new functionality
   - Test integration with existing systems
   - Include edge cases and error conditions
   - Add performance benchmarks

3. **Documentation:**
   - Update API documentation
   - Add usage examples
   - Document configuration options
   - Provide troubleshooting guides

### Phase Roadmap

- **Phase 1**: Project skeleton, core types, configuration, neural interfaces
- **Phase 2**: Real LIF neuron dynamics, spike propagation, plasticity
- **Phase 3**: World interaction, sensory-motor loops, reward learning
- **Phase 4**: Neural prediction, working memory, episodic memory
- **Phase 5**: Performance optimizations, memory pools, parallel processing
- **Phase 6**: Final integration - all systems working together

## License

MIT License

Copyright (c) 2024 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.