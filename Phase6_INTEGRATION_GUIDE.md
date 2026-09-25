# NLM (熙然) Neural Learning Machine - Phase 6 Integration

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements an artificial developmental brain. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Phase 6: Final Integration

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- **Working Memory**: Maintains persistent activity with competition dynamics
- **Episodic Memory**: Stores experiences with encoding and replay capabilities
- **Associative Memory**: Forms Hebbian pattern associations
- **All memory systems**: Connected to neural processing through integrated architecture

#### Neuromodulation Integration
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration behavior
- **Novelty**: Detects and responds to unexpected stimuli
- **Prediction Error**: Compares predicted vs actual outcomes
- **All neuromodulators**: Connected to plasticity rules

#### Prediction System
- **Prediction system**: Integrated into brain loop
- **Prediction error signals**: Affect learning
- **Confidence tracking**: Implemented for uncertainty estimation

#### Cognition Systems
- **Neural planner**: Evaluates action sequences and makes decisions
- **Concept formation**: Extracts patterns from experience
- **Attention**: Implements competitive selection dynamics
- **All cognition systems**: Connected to perception and action

#### Development Integration
- **Developmental stages**: Affect plasticity rates
- **Structural plasticity**: Modulated by age
- **Neural excitability**: Changes with development

#### Persistence
- **Checkpoint save/load**: Implemented
- **Brain state serialization**: Working
- **Resume capability**: Can recover from saved states

#### Replay and Consolidation
- **Episodic memory replay**: During simulation cycles
- **Memory consolidation**: For important episodes
- **Integration with rest**: Sleep/rest cycle functionality

## Build System

### Requirements
- CMake 3.16 or higher
- C++20 compatible compiler
- Python 3.8+ (for bindings)

### Build Instructions

#### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Build with Python Bindings
```bash
# Using scikit-build (preferred)
pip install -e .

# Or traditional CMake build
mkdir build_python
cd build_python
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4
```

#### Running Tests
```bash
cd build
ctest --output-on-failure
```

#### Demo Executables
```bash
# Phase 2: Basic neural computation demo
./nlm

# Phase 3: Advanced learning experiments
./nlm_phase3_demo

# Phase 4: Cognitive capabilities demo
./nlm_phase4_demo

# Phase 6: Complete integration demo (NEW)
./nlm_phase6_demo
```

## Command Line Interface

### Basic Usage
```bash
./nlm [OPTIONS]
```

### Available Options

#### General Options
- `--help, -h`: Show help message
- `--config <file>`: Load configuration from file

#### Configuration Options
- `--neuron-count <num>`: Number of neurons (default: 500)
- `--region-count <num>`: Number of regions (default: 1)
- `--timestep <ms>`: Simulation timestep (default: 0.001)
- `--random-seed <seed>`: Random seed for reproducibility

#### Plasticity Parameters
- `--stdp-ltp-weight <weight>`: STDP LTP weight (default: 0.02)
- `--stdp-ltd-weight <weight>`: STDP LTD weight (default: 0.015)
- `--stdp-tau <ms>`: STDP time constant (default: 20.0)

#### Structural Plasticity
- `--synaptogenesis-rate <rate>`: Synaptogenesis rate (default: 0.0001)
- `--pruning-rate <rate>`: Pruning rate (default: 0.00001)

#### Logging
- `--log-level <level>`: Log level (Trace, Debug, Info, Warning, Error, Critical)

### Configuration File Format

The NLM system supports configuration files in INI-style format:

```ini
# configs/default.cfg

# Core brain parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation parameters
log_level = Info
```

## Performance Characteristics

### Computational Complexity
- **Neurons**: O(N) processing per timestep
- **Synapses**: O(S) transmission events
- **Memory access**: O(log M) for episodic memory retrieval

### Memory Usage
- **Neuron states**: ~64 bytes per neuron
- **Synapse weights**: ~32 bytes per synapse
- **Memory traces**: ~128 bytes per active memory item
- **Configuration storage**: ~1 KB

### Performance Monitoring

The NLM system includes built-in performance monitoring:

```bash
# Run with performance monitoring
./nlm --log-level Info

# Example output:
2026-09-25 18:01:17.123456 [INFO] === NLM Brain Status (Phase 6 - Integrated) ===
2026-09-25 18:01:17.125678 [INFO]   Regions: 1
2026-09-25 18:01:17.127890 [INFO]   Total neurons: 1000
2026-09-25 18:01:17.130123 [INFO]   Total synapses: 15000
2026-09-25 18:01:17.132345 [INFO]   Active neurons: 5
2026-09-25 18:01:17.134567 [INFO]   Firing neurons (this step): 3
2026-09-25 18:01:17.136789 [INFO]   Total spikes: 42
2026-09-25 18:01:17.139012 [INFO]   Pending spike events: 0
2026-09-25 18:01:17.141234 [INFO]   Average firing rate: 0.042
2026-09-25 18:01:17.143456 [INFO]   E/I ratio: 1.5
2026-09-25 18:01:17.145678 [INFO]   Working memory traces: 8
2026-09-25 18:01:17.147890 [INFO]   Episodic memory episodes: 15
2026-09-25 18:01:17.150123 [INFO]   Dopamine level: 0.3
2026-09-25 18:01:17.152345 [INFO]   Developmental stage: 3
```

## Scientific Background

### Neural Model
NLM uses a simplified Leaky Integrate-and-Fire (LIF) neuron model:

```math
dV/dt = (V_rest - V)/τ + I/C
```

Where:
- V is membrane potential
- V_rest is resting potential (-70 mV)
- τ is time constant (20 ms)
- I is synaptic current
- C is membrane capacitance (1 nF)

### Plasticity Rules
- **STDP**: Spike-timing dependent plasticity
- **Hebbian**: Co-activation strengthening
- **Structural**: Synaptogenesis and pruning

### Neuromodulation
- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Salience detection

## Applications

### Research
- Computational neuroscience
- Artificial developmental systems
- Neuromodulation studies
- Memory system integration

### Education
- Neural network education
- Brain function demonstration
- AI/Neural interface teaching

### Development
- Reference implementation for brain-inspired systems
- Testing ground for neural architectures
- Benchmarking platform

## License

MIT License

## Authors

NLM Authors

See docs/ for detailed scientific background and architectural information.

## Key Improvements in This Version

1. **Complete Phase 6 Demo**: Added `nlm_phase6_demo` executable for full integration testing
2. **Enhanced CLI**: Comprehensive command-line interface with help and validation
3. **Improved Error Handling**: Robust error checking and validation throughout
4. **Better Documentation**: Comprehensive comments and API documentation
5. **Extensive Testing**: Added Phase 6 integration tests covering all subsystems
6. **Modern Build System**: Updated CMake configuration with proper dependencies

## Future Directions

- Real-time hardware integration
- Sensor/actuator interfaces for embodiment
- Multi-agent coordination
- Advanced learning algorithms
- Biological validation studies