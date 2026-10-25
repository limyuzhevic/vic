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

**PHASE 2: REAL NEURAL COMPUTATION (In Progress)**

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

**NOTE:** This project is still under development. The documentation previously claimed "Phase 6: FINAL INTEGRATION" but the codebase contains many TODOs and incomplete implementations. We are currently focused on implementing Phase 2 and its core neural computation systems.

### Current Phase 2 Achievements

#### Core Neural Computation
- ✓ Real LIF neuron dynamics implemented
- ✓ Event-driven spike propagation with delays
- ✓ STDP and Hebbian plasticity rules
- ✓ Structural plasticity (synaptogenesis/pruning)
- ✓ Learning experiment demonstrates measurable changes
- ✓ Network shows activity-dependent synaptic modification

#### Configuration System Improvements
- ✓ Enhanced configuration system with JSON/YAML support
- ✓ Improved command-line interface with advanced options
- ✓ Configuration validation and error handling
- ✓ Better logging integration

#### Software Quality Improvements
- ✓ Comprehensive unit and integration tests
- ✓ Improved logging with error handling and rotation
- ✓ Better code organization and structure
- ✓ Enhanced documentation and help systems

### Key Improvements Made

#### Configuration System
- Added JSON/YAML configuration file support alongside simple key=value format
- Enhanced command-line interface with more options
- Configuration validation and error handling
- Better user experience with help and validation commands

#### Logging System
- Thread-safe logging with mutexes
- Log file rotation to prevent disk space issues
- Enhanced log formatting with thread IDs and source locations
- Better error handling for log operations
- Color support and different log levels

#### Command-Line Interface
- Comprehensive help system with detailed options
- Configuration validation commands
- Advanced options for running specific tests
- Better error messages and user guidance

#### Testing
- Comprehensive unit tests for individual components
- Integration tests for complete system functionality
- Test coverage for configuration, logging, and brain systems
- Automated test framework with proper error handling

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 2 Demo (Neural Computation Test)
```bash
./nlm
```

This runs a comprehensive test of the neural computation systems, including:
1. Basic neural connectivity test
2. Plasticity learning experiment
3. STDP verification test

### Advanced Options
```bash
./nlm --help
```

This shows all available command-line options, including:
- Configuration file loading (--config)
- Specific test selection (--run_test)
- Simulation parameters (--num_steps, --step_duration)
- Logging configuration (--log_level, --log_colors)
- Advanced plasticity options

### Configuration Files

The system supports multiple configuration file formats:

1. **Simple key=value format** (default: configs/default.cfg):
```
neuron_count = 500
region_count = 1
simulation_timestep = 0.001
```

2. **JSON format**:
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1
}
```

3. **YAML format**:
```yaml
neuron_count: 500
region_count: 1
simulation_timestep: 0.001
connection_probability: 0.1
```

### Testing

Run the test suite to verify all systems:
```bash
cd build
./test_nlm
```

This runs all unit and integration tests, including:
- Configuration system tests
- Neuron dynamics tests
- Synapse and plasticity tests
- Brain integration tests
- Complete system integration tests

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
│   └── PHASE2_PHASE.md      # Updated Phase 2 documentation
├── src/                    # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/         # Prediction systems
│   ├── cognition/          # Cognitive mechanisms
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework
│   └── visualization/      # Visualization
├── tests/                  # Test suite
│   ├── test_*.cpp         # Individual component tests
│   └── test_main.cpp      # Test runner
└── configs/                # Configuration files
    ├── default.cfg        # Simple format (default)
    ├── default.json       # JSON format
    └── default.yaml       # YAML format
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Current - In Progress)
- ✓ Real LIF neuron dynamics
- ✓ Event-driven spike propagation
- ✓ STDP and Hebbian plasticity
- ✓ Structural plasticity
- ✓ Enhanced configuration system
- ✓ Improved logging and error handling
- ✓ Comprehensive testing framework
- ✓ Advanced command-line interface

### Phase 3 (Planned)
- World interaction loop
- Sensory input and motor output
- Reward prediction error

### Phase 4 (Planned)
- Neural prediction system
- Working memory
- Episodic-like memory

### Phase 5 (Planned)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing

### Phase 6 (Planned)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Advanced debugging and monitoring tools

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT

## Authors

Research project — See docs for scientific background.
