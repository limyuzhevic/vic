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

### Other Available Executables

| Executable | Description |
|------------|-------------|
| `nlm` | Phase 2 real neural computation demo |
| `nlm_phase3_demo` | Phase 3 experience loop demonstration |
| `nlm_phase4_demo` | Phase 4 cognitive functions demonstration |
| `nlm_phase5_demo` | Phase 5 performance optimization demonstration |
| `nlm_phase6_demo` | Phase 6 final integration test |
| `nlm_profile` | Performance profiling tool |
| `nlm_visualize` | Network visualization tool |
| `nlm` | Advanced command-line interface (with subcommands) |

### Command Line Interface (CLI) Usage

The new `nlm` executable provides an advanced command-line interface with the following subcommands:

#### Config Management
- `nlm config create <file>` - Create a new configuration file
- `nlm config load <file>` - Load configuration from file
- `nlm config save <file>` - Save configuration to file
- `nlm config show` - Display current configuration
- `nlm config get <key>` - Get configuration value
- `nlm config set <key> <value>` - Set configuration value

#### Performance Benchmarking
- `nlm benchmark run [neurons] [steps]` - Run performance benchmark
- `nlm benchmark compare <file1> <file2>` - Compare benchmark results
- `nlm benchmark export <file> <output>` - Export benchmark data

#### System Information
- `nlm info system` - Display system information
- `nlm info brain` - Display brain configuration
- `nlm info memory` - Display memory statistics

#### Experiment Management
- `nlm experiment run <config> [steps]` - Run NLM experiment
- `nlm experiment profile <config>` - Profile experiment performance
- `nlm experiment analyze <file>` - Analyze experiment results

#### Utility Commands
- `nlm help` - Show help message
- `nlm version` - Display version information

## Phase 5 Performance Optimization

Phase 5 focuses on performance optimizations for large-scale neural simulations.

### Phase 5 Achievements

#### Performance Optimizations
- **Memory pools**: Efficient memory allocation/deallocation
- **Event queues**: Batch processing of neural events
- **SIMD vectorization**: Parallel processing of neuron computations
- **Parallel processing**: Multi-core CPU utilization
- **Checkpoint system**: Save/load brain states

### Phase 5 Demo

Run the Phase 5 demo to see performance optimizations in action:
```bash
./nlm_phase5_demo
```

### Profiling and Analysis

The new profiling tools provide detailed performance analysis:

#### Performance Profiling
```bash
./nlm_profile
```

Generates comprehensive performance reports including:
- CPU and wall-clock time measurements
- Memory usage analysis
- Performance bottleneck identification
- Recommendations for optimization

#### Network Visualization
```bash
./nlm_visualize
```

Provides network visualization capabilities:
- Network topology display
- Time series visualization
- Data export (CSV, JSON formats)

## Configuration System

The configuration system has been significantly improved with:

### Format Support
- **JSON**: Modern configuration format with full data type support
- **YAML**: Human-readable configuration format
- **Key=value**: Legacy format for backward compatibility

### Advanced Features
- Type-safe configuration values (int, double, string, bool, arrays)
- Multiple configuration sources (file, command-line, runtime)
- Configuration validation and conversion utilities
- Rich documentation and examples

### Example Configuration

JSON format:
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000
  }
}
```

Key=value format:
```
brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
simulation.timestep = 0.001
simulation.max_steps = 10000
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/
└── configs/
```

## License

MIT

## Authors

Research project — See docs for scientific background.
