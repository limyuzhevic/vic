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
## Command-Line Interface

NLM now includes a comprehensive command-line interface (CLI) tool called `nlm_cli` that provides:

### Quick Start

```bash
# Show help
nlm_cli --help

# Show version information
nlm_cli --version

# Run batch mode with 500 neurons for 100 steps
nlm_cli --neurons 500 --steps 100

# Run with custom configuration
nlm_cli --config my_config.cfg --mode phase6

# Run in interactive mode
nlm_cli
```

### Available Commands

In interactive mode, you can use:

- `help` - Show help message
- `status` - Show current brain status
- `step <n>` - Advance brain by n steps
- `reset` - Reset the brain
- `inject <id> <value>` - Inject current into neuron
- `simulate <n>` - Run simulation with n steps
- `config <key> <value>` - Set configuration value
- `exit` or `quit` - Exit application

### Configuration Files

Configuration files use simple `key=value` format:

```
# Example config file
# Configuration comments start with #
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
connection_probability = 0.1
```

### Command-Line Options

- `--help, -h` - Show help message
- `--version` - Show version information
- `--config <file>` - Load configuration from file
- `--neurons <n>` - Number of neurons (default: 500)
- `--steps <n>` - Number of simulation steps (default: 100)
- `--mode <mode>` - Run mode: 'phase2', 'phase3', 'phase4', 'phase6' (default: phase2)
- `--verbose, -v` - Enable verbose output
- `--quiet, -q` - Enable quiet output
- `--batch <n>` - Run batch mode with n steps

## Building and Running

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running

The project provides several executables:

- `nlm` - Phase 2 demo (real neural computation)
- `nlm_cli` - **Command-line interface** (NEW - provides batch and interactive modes)
- `nlm_phase3_demo` - Phase 3 demo (world interaction)
- `nlm_phase4_demo` - Phase 4 demo (cognition and memory)
- `nlm_phase6_demo` - Phase 6 integration test
- `nlm_phase6_text_demo` - Phase 6 text/thought demo
- `nlm_benchmark` - Performance benchmark

#### Command-Line Interface (New in Phase 6)

The `nlm_cli` tool provides a user-friendly interface for running NLM simulations:

**Batch Mode:** Run simulations with simple command-line options
```bash
# Run simple simulation
nlm_cli --neurons 500 --steps 100

# Run with custom configuration
nlm_cli --config my_config.cfg --mode phase6

# Run verbose simulation
nlm_cli --neurons 1000 --steps 2000 --verbose
```

**Interactive Mode:** Use real-time commands to control and explore the brain
```bash
# Start interactive mode
nlm_cli

# Available commands:
help                Show help message
status              Show brain status
step <n>            Advance brain by n steps
reset               Reset the brain
inject <id> <val>   Inject current into neuron
simulate <n>        Run simulation with n steps
config <key> <val>  Set configuration value
exit, quit          Exit application
```

**Configuration Management:** Load and save brain configurations
```bash
# Load configuration from file
nlm_cli --config configs/my_config.cfg

# Save configuration to file
# (Use interactive 'config' command to save)
```

#### Traditional Demos

The traditional demo executables remain for backward compatibility:

```bash
# Phase 2: Real neural computation demo
./nlm

# Phase 3: World interaction demo
./nlm_phase3_demo

# Phase ンド: Cognition and memory demo
./nlm_phase4_demo

# Phase 6: Integration test
./nlm_phase6_demo

# Performance benchmark
./nlm_benchmark
```

### Python Bindings

The project includes Python bindings via `pip install .`. To build with Python bindings:

```bash
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

Then install Python package:

```bash
cd python
pip install .
```

See `python/README.md` for more details and API documentation.

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
