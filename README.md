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

## CLI Usage

### Basic Commands

NLM now includes a comprehensive command-line interface for running experiments and managing simulations:

```bash
# List all available commands
nlm --list-commands

# Get help for a specific command
nlm basic --help

# Show advanced help with all options
nlm --show-advanced-help

# View version information
nlm --version
```

### Available Commands

1. **Auto Mode** - Run all experiments in sequence
   ```bash
   nlm auto [--config file] [--steps N] [--neurons N]
   ```
   Runs: basic → plasticity → stdp → benchmark

2. **Basic Mode** - Simple connectivity and activity tests
   ```bash
   nlm basic [--config file] [--steps N] [--neurons N]
   ```
   Tests neural connectivity and spike propagation

3. **Plasticity Mode** - Learning experiments
   ```bash
   nlm plasticity [--config file] [--steps N] [--neurons N]
   ```
   Demonstrates synaptic learning through Hebbian and STDP mechanisms

4. **STDP Mode** - Spike-timing dependent plasticity verification
   ```bash
   nlm stdp [--config file] [--steps N] [--neurons N]
   ```
   Verifies pre-before-post potentiation (LTP)

5. **Benchmark Mode** - Performance measurement
   ```bash
   nlm benchmark [--steps N] [--neurons N] [--iterations M]
   ```
   Measures simulation performance and efficiency

6. **Integration Mode** - Phase 6 integration test
   ```bash
   nlm integration [--config file] [--steps N] [--neurons N]
   ```
   Runs comprehensive integration test with all brain systems

7. **Script Mode** - Execute automation scripts
   ```bash
   nlm script <script_file> [args...]
   ```
   Execute custom scripts (Phase 7 feature)

### Common Options

- `--config FILE` - Configuration file (default: configs/default.cfg)
- `--steps N` - Number of simulation steps
- `--neurons N` - Number of neurons (default: 500)
- `--timestep F` - Simulation timestep (default: 0.001)
- `--seed N` - Random seed (default: 42)
- `--checkpoint FILE` - Enable checkpointing
- `--enable-replay` - Enable replay system
- `--enable-development` - Enable developmental processes
- `--verbose, -v` - Enable verbose output
- `--output FILE` - Output file for results

### Example Usage

```bash
# Run basic connectivity test with 1000 neurons for 5000 steps
nlm basic --neurons 1000 --steps 5000

# Run plasticity learning experiment with custom config
nlm plasticity --config my_config.cfg

# Run comprehensive integration test
nlm integration --enable-replay --enable-development --neurons 1000

# Run benchmark to measure performance
nlm benchmark --neurons 500 --steps 10000 --iterations 5

# Run all experiments in auto mode
nlm auto --steps 2000 --neurons 800
```

## Building and Running

### Prerequisites

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running

#### Original Main Executable
```bash
./nlm
./nlm --help
```

#### CLI Interface
```bash
./nlm_cli  # Advanced CLI with all features
./nlm_cli basic  # Run basic tests
./nlm_cli integration  # Run Phase 6 integration
```

#### Phase 6 Demo
```bash
./nlm_phase6_demo  # Integration demo
```

#### Phase 3/4 Demos
```bash
./nlm_phase3_demo
./nlm_phase4_demo
```

## Project Structure

```
NLM/
├── src/                    # Source code
│   ├── cli/               # CLI interface (Phase 6 feature)
│   │   ├── CLI.hpp
│   │   └── CLI.cpp
│   ├── main_cli.cpp        # CLI main entry point
│   └── main.cpp            # Original main executable
├── src/agent/             # Agent system
├── src/brain/              # Neural components
├── src/cognition/          # Cognitive mechanisms
├── src/development/        # Developmental system
├── src/dynamics/           # Neural dynamics
├── src/environment/        # Environment interface
├── src/experiments/        # Experiment framework
├── src/memory/             # Memory systems
├── src/motor/              # Motor system
├── src/neuromodulation/     # Neuromodulators
├── src/plasticity/         # Plasticity rules
├── src/prediction/         # Prediction systems
├── src/sensory/            # Sensory processing
├── src/visualization/       # Visualization
├── src/world/              # World system
├── configs/                # Configuration files
├── docs/                   # Documentation
└── tests/                  # Test suite
```

## Key Improvements (Phase 6)

### New Features

1. **Comprehensive CLI Interface**
   - 7 specialized modes for different experiments
   - Advanced configuration options
   - Help system and auto-completion
   - Scripting support

2. **Performance Benchmarking**
   - Built-in performance measurement
   - Multi-iteration benchmarks
   - Efficiency metrics

3. **Enhanced Experiment Control**
   - Individual system testing
   - Integration verification
   - Configurable experimental parameters

4. **Development Tools**
   - Checkpoint/save-load system
   - Replay capabilities
   - Development processes

### Code Quality Improvements

1. **Better Abstraction**
   - CLI class with command pattern
   - Separation of concerns
   - Modular design

2. **Enhanced Configuration**
   - Flexible command-line options
   - Config file support
   - Runtime parameter overrides

3. **Improved Documentation**
   - Comprehensive help system
   - Usage examples
   - API documentation

## Configuration

### Configuration File Format

```
# Configuration file for NLM
# Comments start with #

# Basic parameters
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Simulation parameters
simulation_timestep = 0.001
random_seed = 42

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_capacity = 100
episodic_memory_capacity = 50
```

### Command Line Overrides

Command-line arguments override configuration file values:

```bash
nlm basic --config configs/production.cfg --neurons 1000 --seed 123
```

## Testing

Run the test suite:

```bash
cd build
./test/nlm_tests
```

Or run individual tests:

```bash
./nlm_tests test_config
./nlm_tests test_brain
./nlm_tests test_neuron
```

## Future Enhancements

Phase 7 (Planned)

1. **Advanced Scripting**
   - Custom NLM scripting language
   - Macro system for experiment control
   - Event-driven scripting

2. **Visualization Tools**
   - Real-time neural activity visualization
   - Network topology graphs
   - Memory replay visualization

3. **Extended Benchmarking**
   - Comparative benchmarks
   - Resource usage tracking
   - Scalability testing

4. **Distributed Computing**
   - Multi-processor simulation
   - Parallel experiment execution
   - Cloud integration

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
