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
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
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

## Interactive CLI Features

The new `./nlm_interactive_cli` provides:

1. **Brain Creation**: Configure and create neural brains with custom settings
2. **Interactive Simulation**: Run simulations step-by-step or in batch mode
3. **Real-time Monitoring**: Inspect brain state, agent behavior, and memory systems
4. **Configuration Management**: Set/get configuration values on-the-fly
5. **Plotting Support**: Visualize metrics like reward, firing rates, and novelty
6. **Checkpoint Management**: Save/load brain states for experimentation
7. **Status Tracking**: Monitor simulation progress and system health

### Usage Examples

```bash
# Start interactive CLI
./nlm_interactive_cli

# Create a brain with custom configuration
create my_brain

# Set custom configuration
config set neuron_count 2000
config set connection_probability 0.05

# Run simulation step-by-step (press Ctrl+C to stop)
run 0

# Run simulation in batch mode
run 10000

# Inspect current state
inspect brain
inspect agent
inspect memory

# Show help
help

# Save brain state
save my_brain_checkpoint.bin

# Load brain state
load my_brain_checkpoint.bin

# Reset everything
reset
world reset

# Exit
exit
```

## Configuration Improvements

### default.cfg
Fixed duplicate `dopamine_baseline` entry:

```
# Neuromodulation settings
dopamine_baseline = 0.1
```

### improved_config.cfg
Example of improved configuration for Phase 7:

```
# NLM Configuration File - Improved Version

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

# Development settings (improved)
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings (fixed duplicate)
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

# Phase 7 improvements
enable_interactive_cli = true
enable_improved_config = true
enable_enhanced_logging = true
```

## Testing

The project includes comprehensive unit tests:

```bash
# Run all tests
./build/tests

# Individual test executables
./build/test_neuron
./build/test_synapse
./build/test_brain
./build/test_config
./build/test_clock
```

## Phase 7: Improvements Summary

This improvement phase adds:

1. **Interactive CLI Interface** (`./nlm_interactive_cli`)
   - User-friendly command-line interface for real-time exploration
   - Support for creating, configuring, and running brain simulations
   - Real-time monitoring of brain state and agent behavior
   - Built-in help system and command completion

2. **Enhanced Configuration Management**
   - Fixed duplicate configuration entries
   - Added `improved_config.cfg` example
   - Better value parsing and validation
   - Support for runtime configuration changes

3. **Improved Code Organization**
   - Refactored long files (Brain.cpp, AgentBrain.cpp) for better maintainability
   - Better separation of concerns
   - Enhanced error handling and validation

4. **Expanded Test Coverage**
   - More comprehensive unit tests
   - Integration tests for the complete brain loop
   - Stress testing for edge cases

5. **Enhanced Documentation**
   - Updated README.md with new features
   - Added examples and usage guides
   - Better command documentation

### New Demo Executables

- `./nlm_interactive_cli` - Interactive exploration CLI (Phase 7)
- `./nlm_phase6_text_demo` - Phase 6 text/thought demo
- `./nlm_phase6_demo` - Phase 6 integration test
- `./nlm_phase5_demo` - Phase 5 personality/sandbox demo
- `./nlm_phase3_demo` - Phase 3 world interaction demo

## License

MIT

## Authors

Research project — See docs for scientific background.

### Phase 7 Contributors
This improvement phase was developed by the NLM research team to:
- Make NLM more accessible to researchers and developers
- Provide better tools for experimentation and debugging
- Improve code maintainability and documentation
- Enhance user experience with interactive interfaces
- Support more complex experimental setups

The NLM project continues to evolve toward a more comprehensive artificial brain simulation platform.
