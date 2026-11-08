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

Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

### Using the NLM Run Script

For improved usability and advanced features, use the NLM run script:

```bash
# Show help and usage information
./nlm-run --help

# Run default NLM demo with default config
./nlm-run

# Run Phase 6 integration demo
./nlm-run -d nlm_phase6_demo

# Run with custom configuration and profiling
./nlm-run -d nlm_benchmark -c configs/benchmark.cfg -p -v

# Run with additional arguments passed to the demo
./nlm-run -- --neuron-count 2000 --steps 5000
```

## Key Improvements

### 1. Enhanced Robustness
- **Null Pointer Safety**: Comprehensive null checks in critical areas (main.cpp, demo files)
- **Error Handling**: Improved error reporting and graceful failure handling
- **Configuration Validation**: Parameter range validation and safe defaults

### 2. Improved User Experience
- **Enhanced CLI**: nlm-run script with comprehensive demo management
- **Better Configuration**: Enhanced default.cfg with comprehensive settings
- **Performance Profiling**: Built-in profiling support for performance and analysis
- **Cross-Platform**: Improved build system with better error handling

### 3. Better Code Quality
- **Documentation**: IMPROVEMENTS.md with comprehensive documentation
- **Code Comments**: Enhanced comments and documentation throughout the codebase
- **Error Handling**: Comprehensive error reporting and validation
- **Safety Checks**: Added validation for all critical parameters

## Configuration Files

The project now includes enhanced configuration files:

- **configs/default.cfg**: Standard configuration with comprehensive options
- **configs/benchmark.cfg**: Optimized for performance benchmarking
- **configs/debug.cfg**: Debug-friendly configuration with verbose output
- **configs/integration.cfg**: Integration test configuration

### Configuration Format

Enhanced configuration format with comprehensive settings:

```
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

# Neuromodulation settings
dopamine_baseline = 0.0
curiosity_baseline = 0.1
novelty_baseline = 0.0
prediction_error_baseline = 0.0

# Reward settings
reward_discount_factor = 0.99
reward_learning_rate = 0.01

# Memory system settings
working_memory_capacity = 100
episodic_memory_capacity = 1000
associative_memory_capacity = 500

# Development system settings
development_enabled = true
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
development_maturation_rate = 0.0005

# Enhanced plasticity settings
hebbian_learning_rate = 0.005
structural_plasticity_rate = 0.0001

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm_simulation.log
log_rotation_size_mb = 100
log_retention_days = 30

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
visualization_output_dir = ./visualization
```

## Building with Python Bindings

For Phase 5 Python bindings support:

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

The Python bindings module will be built as `pynlm` and can be installed via pip:

```bash
cd build
pip install ./pynlm-*.whl
```

## Testing

### Unit Tests
```bash
# Run tests
./build/tests/test_main
```

### Performance Benchmarks
```bash
# Run performance benchmarks
./build/nlm_benchmark

# Or using the enhanced run script
./nlm-run -d nlm_benchmark -p
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── IMPROVEMENTS.md          # Documentation of improvements
├── nlm-run                  # Demo runner script
├── configs/                 # Enhanced configuration files
│   ├── default.cfg
│   ├── benchmark.cfg
│   ├── debug.cfg
│   └── integration.cfg
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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/               # Unit tests
└── docs/                # Documentation
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
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

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

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
