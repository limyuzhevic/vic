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

## Command Line Interface

NLM now features an enhanced command-line interface for flexible configuration and operation:

### Basic Usage

```bash
# Run with help
./nlm --help

# Run with version info
./nlm --version

# Run with quiet mode (minimal output)
./nlm --quiet

# Run with verbose output (debug info)
./nlm --verbose
```

### Configuration Options

```bash
# Load custom configuration file
./nlm --config my_config.cfg

# Configure parameters directly
./nlm --neuron-count 2000 --region-count 4 --connection-prob 0.2

# Configure STDP parameters
./nlm --stdp-ltp 0.03 --stdp-ltd 0.035 --stdp-tau 25.0

# Configure plasticity parameters
./nlm --synaptogenesis 0.002 --pruning 0.00002

# Set random seed for reproducibility
./nlm --seed 12345

# Set demo mode (controls what gets tested)
./nlm --demo-mode basic      # Just basic connectivity
./nlm --demo-mode plasticity  # Plasticity learning experiment
./nlm --demo-mode stdp       # STDP verification only
./nlm --demo-mode all        # All tests (default)
```

### Checkpoint Options

```bash
# Enable checkpointing
./nlm --checkpoint

# Load brain state from checkpoint
./nlm --load saved_brain.nbl

# Save brain state to checkpoint
./nlm --save checkpoint.nbl
```

### Simulation Control

```bash
# Set number of simulation steps
./nlm --steps 2000

# Combine multiple options
./nlm --demo-mode stdp --steps 500 --quiet --checkpoint
```

### Examples

```bash
# Minimal demo with custom neurons
./nlm --neuron-count 500 --region-count 1 --steps 1000

# Full plasticity experiment
./nlm --demo-mode plasticity --stdp-ltp 0.02 --stdp-ltd 0.015 --synaptogenesis 0.0001 --quiet

# Debug mode with checkpointing
./nlm --verbose --demo-mode all --checkpoint --save debug_brain.nbl
```

### Configuration File Format

The default configuration file `configs/default.cfg` contains all available parameters. You can create a custom configuration file and load it with `--config`.

```cfg
# Example custom configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1500
region_count = 3
connection_probability = 0.2
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.00001
checkpoint_dir = ./my_checkpoints
replay_interval = 200
```

### Configuration Categories

The configuration system is organized into logical categories:

#### **Core Simulation**
- `random_seed`: Random number generator seed
- `simulation_timestep`: Simulation time step (s)
- `neuron_count`: Total number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Neural connectivity density

#### **Plasticity**
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_time_constant`: STDP time constant (ms)
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

#### **Neuromodulation**
- `dopamine_baseline`: Baseline dopamine level
- `curiosity_threshold`: Curiosity activation threshold
- `novelty_sensitivity`: Novelty detection sensitivity
- `prediction_error_decay`: Prediction error decay rate

#### **Development**
- `development_critical_period_start`: Critical period start
- `development_critical_period_end`: Critical period end
- `development_plasticity_decay_rate`: Plasticity decay during development

#### **Memory**
- `working_memory_capacity`: Working memory capacity
- `episodic_memory_max_episodes`: Maximum episodic memories
- `associative_memory_capacity`: Associative memory capacity

#### **Performance**
- `enable_vectorization`: Enable vectorized operations
- `enable_parallel_processing`: Enable parallel execution
- `enable_simd`: Enable SIMD optimizations

#### **Checkpointing**
- `checkpoint_dir`: Directory for checkpoint files
- `checkpoint_interval`: Checkpoint save interval
- `checkpoint_compress`: Enable checkpoint compression
- `checkpoint_max_files`: Maximum number of checkpoints to keep

#### **Replay**
- `replay_interval`: Memory replay interval
- `replay_episodes`: Number of episodes to replay
- `replay_decay_factor`: Memory decay during replay

### Environment Interaction

NLM can interact with external environments for behavioral experiments:

```bash
# Run in specific environment (if supported)
./nlm --environment GridWorld --environment-width 10 --environment-height 10

# Configure sensory parameters
./nlm --sensory-input-range 15.0 --motor-output-scale 2.0

# Set action selection strategy
./nlm --action-selection-method attention
```

### Development and Debugging

```bash
# Enable debug mode
./nlm --debug-mode

# Set debug output file
./nlm --debug-output debug.log

# Enable performance profiling
./nlm --profile-performance

# Enable statistics collection
./nlm --enable-stats-collection
```

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
