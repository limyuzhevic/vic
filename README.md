# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "spark" or "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

### Phase 2 Demo (Real Neural Computation)
```bash
./nlm --mode phase2
```

### Phase 6 Demo (Final Integration)
```bash
./nlm --mode phase6
```

### Performance Benchmark
```bash
./nlm --mode benchmark --neurons 1000 --iterations 3
```

### Using Custom Configuration
```bash
./nlm --mode phase2 --config configs/development.cfg
```

### Help and Version
```bash
./nlm --help
./nlm --version
```

### Available Configuration Files

The project includes several example configuration files:

- `configs/default.cfg` - Standard configuration for Phase 6 integration
- `configs/development.cfg` - Optimized for debugging and development
- `configs/performance.cfg` - Optimized for speed and benchmarking
- `configs/research.cfg` - Comprehensive configuration for scientific research
- `configs/minimal.cfg` - Ultra-lightweight configuration for quick testing

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── configs/
│   ├── default.cfg                    # Standard configuration
│   ├── development.cfg                # Development/debugging
│   ├── performance.cfg                # Benchmark/optimized
│   ├── research.cfg                   # Scientific research
│   └── minimal.cfg                     # Quick testing
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   ├── utils/         # Common utilities
│   ├── phase2_demo.cpp # Phase 2: Real Neural Computation demo
│   ├── phase6_demo.cpp # Phase 6: Final Integration demo
│   └── main.cpp        # Main entry point with CLI interface
├── tests/
└── docs/
    ├── ARCHITECTURE.md
    ├── SCIENCE.md
    ├── ROADMAP.md
    ├── EXPERIMENTS.md
    ├── LIMITATIONS.md
    └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
```

## Configuration Options

### Core Settings
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 500 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `simulation_timestep` | double | 0.001 | Simulation time step in seconds |
| `random_seed` | int64 | 42 | Random seed for reproducibility |

### Neural Dynamics
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_threshold` | float | -50.0 | Neuron threshold potential (mV) |
| `neuron_rest_potential` | float | -70.0 | Resting potential (mV) |
| `neuron_reset_potential` | float | -75.0 | Reset potential after spike (mV) |
| `membrane_time_constant` | float | 20.0 | Membrane time constant (ms) |
| `refractory_period` | float | 2.0 | Refractory period (ms) |

### Plasticity
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate |
| `synaptogenesis_rate` | float | 0.0001 | Structural synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |

### Neuromodulation
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Dopamine baseline level |
| `curiosity_scaling` | float | 1.0 | Curiosity motivation scaling |
| `novelty_threshold` | float | 0.5 | Novelty detection threshold |
| `prediction_error_decay` | float | 0.1 | Prediction error decay rate |

### Memory Systems
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int | 100 | Working memory capacity |
| `episodic_memory_capacity` | int | 1000 | Episodic memory capacity |
| `associative_memory_strength` | float | 0.5 | Associative memory connection strength |

### Prediction System
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction_horizon` | int | 10 | Prediction time horizon |
| `prediction_confidence_threshold` | float | 0.7 | Confidence threshold for predictions |

### Development
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_start_time` | float | 0.0 | Start of development period |
| `development_end_time` | float | 100.0 | End of development period |
| `development_stage_duration` | float | 20.0 | Duration of each developmental stage |

### Logging
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | INFO | Log level: DEBUG, INFO, WARN, ERROR |
| `log_to_file` | bool | false | Log to file in addition to console |
| `log_filename` | string | nlm.log | Log file name |
| `log_detailed` | bool | true | Enable detailed logging |

### Simulation
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_simulation_steps` | int | 10000 | Maximum simulation steps |
| `simulation_time_limit` | float | 0.0 | Simulation time limit (0 = unlimited) |
| `enable_checkpoints` | bool | true | Enable checkpoint saving |
| `checkpoint_interval` | int | 1000 | Checkpoint save interval |

## Command Line Interface

### Usage
```bash
./nlm [OPTIONS]
```

### Options

| Option | Description |
|--------|-------------|
| `--mode MODE` | Demo mode: phase2, phase6, benchmark (default: phase2) |
| `--config FILE` | Configuration file (default: configs/default.cfg) |
| `--steps N` | Number of simulation steps for benchmark mode |
| `--neurons N` | Number of neurons for benchmark mode |
| `--iterations N` | Number of iterations for benchmark mode |
| `--help` | Show this help message |
| `--version` | Show version information |

### Demo Modes

#### Phase 2: Real Neural Computation
- Demonstrates Leaky Integrate-and-Fire (LIF) neuron dynamics
- Shows event-driven spike propagation with synaptic delays
- Implements STDP and Hebbian plasticity rules
- Demonstrates structural plasticity (synaptogenesis/pruning)
- Validates learning through measurable synaptic changes

#### Phase 6: Final Integration
- Comprehensive integration test of all brain systems
- Verifies memory systems are working together
- Tests neuromodulation integration with neural processing
- Validates prediction system integration with learning
- Confirms cognition systems are connected to perception and action
- Tests developmental stages affecting plasticity rates
- Validates checkpoint save/load functionality
- Tests replay and consolidation mechanisms

#### Benchmark
- Performance testing with configurable parameters
- Measures steps per second (performance)
- Tests scalability with different neuron counts
- Validates stability over multiple iterations

### Example Commands

```bash
# Run Phase 2 demo with standard configuration
./nlm --mode phase2

# Run Phase 6 integration test with development configuration
./nlm --mode phase6 --config configs/development.cfg

# Run performance benchmark with custom parameters
./nlm --mode benchmark --neurons 2000 --iterations 3 --steps 5000

# Run quick test with minimal configuration
./nlm --mode phase2 --config configs/minimal.cfg

# Show help information
./nlm --help
```

## What is NLM?

NLM (熙然, meaning "spark" or "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

## Build Requirements

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

## License

MIT

## Authors

Research project — See docs for scientific background.