# NLM - Neural Learning Machine Documentation

## Overview

**NLM (熙然, meaning "serene flow")** is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### What NLM IS NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

### What NLM IS INTENDED TO BECOME:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase: **Phase 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators (integrated)
│   ├── memory/           # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
├── tests/                 # Test framework
├── configs/               # Configuration files
└── python/               # Python bindings (if any)
```

## Building and Installation

### Prerequisites

- CMake 3.19 or higher
- C++20 compatible compiler (GCC 9+, Clang 8+, MSVC 2019+)
- Python 3.8+ (for Python bindings)

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installation

```bash
cd build
sudo make install
```

### Build Options

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DENABLE_TESTS=ON \
    -DENABLE_PYTHON_BINDINGS=ON
```

## Running NLM

### Basic Execution

After building, you can run the main NLM executable:

```bash
./nlm
```

This will run Phase 6 integration tests with comprehensive neural network simulation.

### Demo Executables

```bash
# Phase 3 demo
./nlm_phase3_demo

# Phase 4 demo
./nlm_phase4_demo
```

### Command-Line Options

```bash
./nlm --help

Options:
  --help, -h              Show this help message
  --version               Show version information
  --config <file>         Load configuration from file
  --validate-config       Validate configuration and exit
  --performance-test      Enable performance testing mode
  --debug                 Enable debug mode
  --simulation-steps <N>  Set maximum simulation steps
  --neuron-count <N>      Set number of neurons
  --timestep <ms>         Set simulation timestep
  --random-seed <N>       Set random seed for reproducibility
```

### Configuration Files

#### Default Configuration

File: `configs/default.cfg`

```ini
# NLM Configuration File
# This is the default configuration for Phase 6: Final Integration

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

#### JSON Configuration

File: `configs/default.json`

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

## Configuration System

### Supported Configuration Formats

1. **Simple Format** (default): Key-value pairs
   ```
   key = value
   ```

2. **JSON Format**: Full JSON configuration
   ```json
   {
     "key": "value",
     "number": 42,
     "boolean": true,
     "array": [1, 2, 3]
   }
   ```

3. **YAML Format**: YAML configuration (future implementation)

### Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int64 | 1000 | Number of neurons in the brain |
| `region_count` | int64 | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Connection probability for random connectivity |
| `plasticity_learning_rate` | float | 0.01 | Learning rate for plasticity |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant |
| `development_synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate |
| `development_pruning_rate` | float | 0.0001 | Pruning rate |
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `reward_discount_factor` | float | 0.99 | Reward discount factor |
| `environment_name` | string | "GridWorld" | Environment name |
| `environment_width` | int64 | 10 | Environment width |
| `environment_height` | int64 | 10 | Environment height |
| `log_level` | string | "INFO" | Logging level |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | "nlm.log" | Log file name |
| `max_simulation_steps` | int64 | 10000 | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | Simulation time limit |
| `visualization_enabled` | bool | false | Whether visualization is enabled |
| `visualization_update_rate` | float | 30.0 | Visualization update rate |

## Running Tests

### Test Framework

NLM includes a comprehensive test framework for verifying functionality:

```bash
# Run all tests
make test

# Or run the test executable directly
./nlm_test
```

### Available Tests

- `test_types.cpp` - Type system tests
- `test_config.cpp` - Configuration system tests
- `test_random.cpp` - Random number generator tests
- `test_clock.cpp` - Simulation clock tests
- `test_neuron.cpp` - Neuron dynamics tests
- `test_synapse.cpp` - Synapse transmission tests
- `test_stdp.cpp` - STDP plasticity tests
- `test_brain.cpp` - Brain integration tests

### Test Results

All tests should pass successfully. If any tests fail, check the test output for detailed error messages.

## API Reference

### Core Classes

#### Brain
The central coordinator of the neural system implementing:
- Real spiking neural computation with LIF neuron dynamics
- Event-driven dynamics with synaptic delays and spike propagation
- Integrated memory systems (working, episodic, associative)
- Neuromodulation system (dopamine, curiosity, novelty, prediction error)
- Prediction and cognitive systems (planning, concept formation, attention)
- Development system for structural plasticity and maturation
- Plasticity rules (STDP, Hebbian, structural)
- Modular architecture with independent subsystems
- Closed-loop interaction with environment
- Checkpoint save/load capabilities
- Performance optimizations (memory pools, event queues, SIMD)

#### Neuron
Leaky Integrate-and-Fire neuron implementation with:
- Membrane potential dynamics
- Refractory period handling
- Spike-frequency adaptation
- Synaptic input integration
- State management

#### Synapse
Synaptic transmission with:
- Event-driven spike propagation
- Short-term plasticity (facilitation and depression)
- Eligibility traces for reward-modulated learning
- STDP learning rules
- Weight management and clamping

### Configuration System

#### Config
Configuration management supporting:
- Multiple input formats (simple, JSON, YAML)
- Type-safe value access
- Validation and error handling
- Command-line argument parsing
- File loading and saving

## Performance Considerations

### Memory Management

NLM implements several performance optimizations:

1. **Memory Pools**: For efficient allocation/deallocation
2. **Event Queues**: For event-driven processing
3. **SIMD Vectorization**: For parallel processing
4. **Parallel Processing**: Multi-core support

### Build Configuration

```bash
# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Minimal size build
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j4
```

## Scientific Background

### Architecture Philosophy

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

### Scientific Limitations

NLM is a research project investigating computational brain-like systems. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Getting Help

For questions, issues, or contributions:

1. Check the documentation in the `docs/` directory
2. Review the API reference in this file
3. Look at test files for usage examples
4. Check the project repository for additional resources

## Future Improvements

### Planned Features

1. **Full YAML Support**: Implement proper YAML parser
2. **Advanced Python Bindings**: Enhanced Python API with intuitive interface
3. **More Sophisticated Learning**: Advanced learning algorithms and mechanisms
4. **Improved Simulation Flexibility**: More environment options and scenarios
5. **Enhanced Visualization**: Better visualization tools and interfaces
6. **Distributed Computing**: Support for large-scale parallel simulation
7. **Hardware Acceleration**: GPU support for neural computation
8. **Real-time Interaction**: Live visualization and control interfaces

## Troubleshooting

### Common Issues

#### Build Failures

If you encounter build failures:

1. **Compiler Compatibility**: Ensure you're using a C++20 compatible compiler
2. **CMake Version**: Update CMake to 3.19 or higher
3. **Dependencies**: Install required packages
4. **Memory**: Check available memory for large builds

#### Runtime Errors

If you encounter runtime errors:

1. **Configuration Files**: Check configuration file syntax
2. **Memory Usage**: Monitor memory usage for large networks
3. **Log Files**: Check `nlm.log` for detailed error information
4. **Debug Mode**: Use `--debug` flag for more detailed output

#### Test Failures

If tests fail:

1. **Environment**: Check system environment variables
2. **Dependencies**: Ensure all dependencies are properly installed
3. **Random Seed**: Try running with a different random seed
4. **Configuration**: Validate configuration files

### Getting Help

For specific issues:

1. **Issue Tracker**: Report bugs in the project issue tracker
2. **Community**: Join the NLM community for discussions
3. **Documentation**: Check the latest documentation
4. **Examples**: Look at example configurations and code

## Conclusion

NLM represents a comprehensive approach to creating artificial neural systems inspired by biological brains. The project implements a full stack of neural computation, from low-level neuron dynamics to high-level cognitive functions, all integrated into a coherent architecture capable of learning and adaptation.

The codebase is designed to be both scientifically rigorous and computationally efficient, with modern C++20 features, comprehensive error handling, and extensive testing to ensure reliability and correctness.

---

*Last updated: September 25, 2026*
*NLM Phase 6: FINAL INTEGRATION*