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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

### Demo Executables

Build all targets first (see Building above). The entry points are currently separate demos per phase:

```bash
./nlm                 # Phase 2 demo with advanced CLI
./nlm_phase3_demo     # Phase 3: World interaction
./nlm_phase4_demo     # Phase 4: Cognitive capabilities  
./nlm_phase5_demo     # Phase 5: Performance and sandbox
./nlm_phase6_demo     # Phase 6: Complete integration test
./nlm_phase6_text_demo # Phase 6 with text interface
./nlm_benchmark       # Performance benchmark
```

### Enhanced Command-Line Options

All demo executables now support enhanced command-line options:

#### General Options (Available in ./nlm, ./nlm_phase6_demo)

| Option | Description | Default |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | `configs/default.cfg` |
| `--neurons <N>` | Number of neurons | `500` |
| `--timestep <T>` | Simulation timestep in seconds | `0.001` |
| `--steps <S>` | Number of simulation steps | `1000` |
| `--seed <N>` | Random seed for reproducible results | `42` |
| `--verbose` / `-v` | Enable verbose output | disabled |
| `--benchmark` / `--bench` | Run benchmark mode | disabled |
| `--help` / `-h` | Show help message | -

#### Usage Examples

```bash
# Standard demo with defaults
./nlm

# Load custom configuration
./nlm --config my_config.cfg

# Larger network with custom parameters
./nlm --neurons 1000 --steps 5000 --timestep 0.005

# Reproducible execution with debug output
./nlm --seed 12345 --verbose

# Benchmark mode (see ./nlm_benchmark for details)
./nlm --benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Project Structure

```
NLM/                          # Root directory
├── CMakeLists.txt             # Main build configuration
├── README.md                   # This documentation
├── easy_usage.md               # Quick start guide (legacy)
├── HOW_TO_USE.md               # Detailed usage instructions (legacy)
├── pyproject.toml             # Python bindings configuration
├── LICENSE                     # Software license
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # System architecture
│   ├── SCIENCE.md              # Scientific background
│   ├── ROADMAP.md              # Development roadmap
│   ├── EXPERIMENTS.md          # Experiment details
│   ├── LIMITATIONS.md          # Known limitations
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit report
├── src/                        # Source code
│   ├── core/                   # Core utilities
│   ├── agent/                  # Agent-brain interface
│   ├── world/                  # Simple world simulation
│   ├── performance/            # Checkpoints, pools, queues
│   ├── brain/                  # Neural components (integrated)
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/        # Neuromodulators (integrated)
│   ├── memory/                 # Memory systems (integrated)
│   ├── prediction/             # Prediction systems (integrated)
│   ├── cognition/              # Cognitive mechanisms (integrated)
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework (Phase 6)
│   └── visualization/          # Visualization
├── tests/                      # Test suite
└── configs/                    # Configuration files
    ├── default.cfg             # Default configuration
    └── example.cfg             # Example configuration
```

## Phase Summary

### Phase 1 (Complete) - Core Infrastructure
- **Status**: ✅ COMPLETE
- **Deliverables**:
  - Project skeleton and build system
  - Core types and configuration management
  - Neural interfaces and basic data structures
  - Random number generation and simulation clock
- **Key Features**:
  - Strongly-typed identifiers (NeuronId, SynapseId, RegionId, etc.)
  - Configuration system with file and command-line support
  - Core logging and debugging infrastructure

### Phase 2 (Complete) - Real Neural Computation
- **Status**: ✅ COMPLETE
- **Key Features**:
  - **LIF Neuron Dynamics**: Leaky Integrate-and-Fire neuron model with membrane potential, threshold, and refractory period
  - **Event-Driven Spike Propagation**: Precise timing with synaptic delays and spike delivery
  - **Plasticity Rules**: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning mechanisms
  - **Structural Plasticity**: Synaptogenesis and pruning for network topology
  - **Enhanced CLI**: Command-line interface for flexible configuration
- **Technical Improvements**:
  - Fixed region ID inconsistency (0-based indexing)
  - Eliminated null pointer dereferences
  - Improved performance benchmarks with detailed metrics

### Phase 3 (Complete) - World Interaction
- **Status**: ✅ COMPLETE
- **Key Features**:
  - **Closed-Loop System**: Brain ↔ World interaction through sensory input and motor output
  - **Reward Prediction Error**: Learning from environmental feedback
  - **Developmental Stages**: Age-dependent plasticity and learning rates
  - **Novelty and Curiosity**: Exploration-driven learning
  - **Action Selection**: Goal-directed behavior with planning
- **Architecture**: Simple world simulation with agent-brain interface

### Phase 4 (Complete) - Cognitive Architecture
- **Status**: ✅ COMPLETE
- **Key Features**:
  - **Working Memory**: Transient active information storage with competitive dynamics
  - **Episodic Memory**: Experience encoding and replay mechanisms
  - **Neural Prediction**: Forward models and prediction error signals
 广场概念形成**: Pattern discovery and abstraction
  - **Neural Attention**: Competitive selection without Transformer architecture
  - **Predictive Planning**: Action sequence evaluation
  - **Self-Model**: Integration of sensory and motor representations
  - **Social Learning**: Learning from others' experiences
  - **Continual Learning**: Adaptation to new tasks without catastrophic forgetting
- **Integration**: All cognitive systems connected to perception and action

### Phase 5 (Complete) - Performance and Sandbox
- **Status**: ✅ COMPLETE
- **Key Features**:
  - **Memory Pools**: Efficient memory allocation for large-scale simulations
  - **Event Queues**: Asynchronous spike delivery with minimal overhead
  - **SIMD Vectorization**: Parallel processing using SIMD instructions
  - **Parallel Processing**: Multi-threaded simulation capabilities
  - **Checkpoint System**: Save/load brain state for persistence
  - **Sandbox Environment**: Safe testing and experimentation area
- **Performance**: Optimized for both accuracy and computational efficiency

### Phase 6 (Complete) - Final Integration
- **Status**: ✅ COMPLETE
- **Key Achievements**:
  - **System Integration**: All previous phases combined into a coherent brain loop
  - **Memory Integration**: Working memory connected to neural processing
  - **Neuromodulation Integration**: Dopamine and curiosity affect plasticity and dynamics
  - **Prediction Integration**: Prediction system integrated with learning
  - **Development Integration**: Developmental stages modulate plasticity rates
  - **Persistence**: Checkpoint save/load working
  - **Replay and Consolidation**: Memory replay during simulation
  - **Comprehensive Testing**: All systems verified and working together
- **Enhanced Features**:
  - Improved command-line interface with advanced options
  - Better benchmark performance metrics
  - Enhanced documentation and examples
  - Advanced configuration capabilities

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Enhanced Build with Python Bindings
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

## Running

### Demo Executables
Build all targets first (see Building above). The entry points are currently separate demos per phase:

```bash
./nlm                 # Phase 2 demo with advanced CLI
./nlm_phase3_demo     # Phase 3: World interaction
./nlm_phase4_demo     # Phase 4: Cognitive capabilities  
./nlm_phase5_demo     # Phase 5: Performance and sandbox
./nlm_phase6_demo     # Phase 6: Complete integration test
./nlm_phase6_text_demo # Phase 6 with text interface
./nlm_benchmark       # Performance benchmark
```

### Advanced Command-Line Options

All demo executables now support enhanced command-line options:

#### General Options (Available in ./nlm, ./nlm_phase6_demo)

| Option | Description | Default |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | `configs/default.cfg` |
| `--neurons <N>` | Number of neurons | `500` |
| `--timestep <T>` | Simulation timestep in seconds | `0.001` |
| `--steps <S>` | Number of simulation steps | `1000` |
| `--seed <N>` | Random seed for reproducible results | `42` |
| `--verbose` / `-v` | Enable verbose output | disabled |
| `--benchmark` / `--bench` | Run benchmark mode | disabled |
| `--help` / `-h` | Show help message | -

#### Usage Examples

```bash
# Standard demo with defaults
./nlm

# Load custom configuration
./nlm --config my_config.cfg

# Larger network with custom parameters
./nlm --neurons 1000 --steps 5000 --timestep 0.005

# Reproducible execution with debug output
./nlm --seed 12345 --verbose

# Benchmark mode (see ./nlm_benchmark for details)
./nlm --benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Scientific Limitations

**NLM is a research project investigating computational brain-like systems.** We make NO claims that NLM accurately reproduces biological brains.

### Current Limitations Include:
- **Simplified Neuron Model**: LIF neurons (not Hodgkin-Huxley)
- **No Ion Channel Dynamics**: Simplified electrical properties
- **Limited Dendritic Modeling**: No detailed dendritic morphology
- **Simplified Architecture**: Not realistic cortical organization
- **No Consciousness Claims**: No claim of consciousness, intelligence, or human-like cognition
- **Computational Limits**: Limited by available computing resources

### What Phase 6 DOES Investforme investigates:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system with the following computational flow:

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

## Advanced Usage Examples

### 1. Creating Custom Brain Configurations

Create a configuration file for specific experimental conditions:

```ini
# configs/research.cfg
neuron_count=2000
region_count=2  # Multiple cortical areas
connection_probability=0.25  # Sparse connectivity
simulation_timestep=0.005  # Slower simulation
random_seed=12345  # Reproducible results
stdp_ltp_weight=0.015
stdp_ltd_weight=0.01
stdp_tau=50.0  # Longer STDP time constant
synaptogenesis_rate=0.0005  # Higher plasticity
pruning_rate=0.00005  # Active pruning
checkpoint_dir=./research_checkpoints
```

Run with: `./nlm --config configs/research.cfg`

### 2. Performance Testing for Different Scales

Test scalability across different network sizes:

```bash
# Small network (quick test)
./nlm --neurons 100 --steps 500

# Medium network (standard test)  
./nlm --neurons 500 --steps 2000

# Large network (production test)
./nlm --neurons 2000 --steps 5000

# Ultra-large network (research scale)
./nlm --neurons 5000 --steps 10000
```

### 3. Reproducible Research

For scientific experiments, use reproducible settings:

```bash
# Set random seed for reproducibility
./nlm --seed 42 --verbose --config configs/experiment.cfg

# Run multiple trials with different seeds
./nlm --seed 100 --steps 3000
./nlm --seed 200 --steps 3000
./nlm --seed 300 --steps 3000
```

### 4. Benchmark and Performance Analysis

Compare performance across different hardware:

```bash
# Standard benchmark
./nlm_benchmark

# Custom benchmark with different connection probabilities
./custom_benchmark --neurons 1000 --steps 2000 --conn_prob 0.05
./custom_benchmark --neurons 1000 --steps 2000 --conn_prob 0.20
```

### 5. Integration Testing

Verify all Phase 6 systems are working:

```bash
# Quick integration verification
./nlm_phase6_demo --neurons 100 --steps 1000

# Full integration with checkpointing
./nlm_phase6_demo --neurons 500 --steps 5000 --enable_checkpointing

# Integration with development
./nlm_phase6_demo --neurons 300 --steps 3000 --enable_development
```

## Troubleshooting

### Common Issues and Solutions

**1. "No neurons found!" error**
- **Cause**: Configuration parameters result in zero neurons
- **Solution**: Check `neuron_count`, `region_count`, and `connection_probability` parameters

**2. Slow performance**
- **Cause**: Large network size without sufficient hardware
- **Solution**: Use smaller network for testing: `--neurons 100 --steps 500`

**3. Checkpoint failures**
- **Cause**: Insufficient disk space or permissions
- **Solution**: Check disk space and create `./checkpoints` directory if needed

**4. Memory errors**
- **Cause**: System resources exhausted
- **Solution**: Close other applications or increase system memory

### Getting Started Tips

- **Use small configurations first**: Start with `--neurons 100 --steps 500` to understand the system
- **Check logs**: Look for `NLM_LOG_INFO` messages for debug information
- **Use verbose mode**: Add `--verbose` for detailed output
- **Configuration files**: Create custom configs in the `configs/` directory

## Performance Recommendations

### Hardware Requirements

| Task | Minimum Hardware | Recommended Hardware |
|------|-------------------|--------------------|
| Demo test | CPU with 2 cores | CPU with 8 cores |
| Large network (2000 neurons) | 4 GB RAM | 8 GB RAM |
| Research scale (5000 neurons) | 8 GB RAM | 16 GB RAM |

### Optimization Flags

For production builds:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)
```

For debugging and development:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Further Reading

- **Documentation**: Read the `docs/` directory for detailed technical information
- **Architecture**: `docs/ARCHITECTURE.md` explains system design
- **Science**: `docs/SCIENCE.md` provides scientific context
- **Experiments**: `docs/EXPERIMENTS.md` details all experimental work
- **Limitations**: `docs/LIMITATIONS.md` discusses current constraints
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` provides final verification report

## Contact

For questions, issues, or collaboration opportunities:

- **Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: File issues on GitHub
- **Discussions**: Join project discussions for questions

---

*This project represents an ongoing research effort. Current limitations and areas for improvement are identified in the documentation. Future work will focus on biological realism, scalability, and integration with real-world applications.*

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

**Key Improvements Made**:
- ✅ Fixed region ID inconsistency (0-based indexing)
- ✅ Eliminated null pointer dereferences in demo files
- ✅ Enhanced benchmark performance metrics
- ✅ Added advanced command-line interface options
- ✅ Improved documentation with comprehensive examples
- ✅ Better error handling and configuration management
- ✅ Enhanced build system with Python bindings support

**Code Quality Improvements**:
- More robust error checking and null pointer safety
- Better separation of concerns in demo executables
- Enhanced configuration system with command-line overrides
- Comprehensive documentation with working examples
- Improved performance measurement and analysis tools
- Better debugging and verbose output capabilities

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

## Architecture Philosophy

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

## License

MIT

## Authors

Research project — See docs for scientific background.
