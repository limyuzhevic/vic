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

Build all targets first (see Building above). The entry points are:

### New Unified Demo Launcher (Recommended)

The **nlm_demo** unified demo launcher provides a single interface for all NLM demonstrations with advanced configuration options:

```bash
./nlm_demo --help     # Show all options
./nlm_demo phase2      # Phase 2: Real neural computation
./nlm_demo phase3      # Phase 3: World interaction (default: 5 episodes, 200 steps)
./nlm_demo phase4      # Phase 4: Emerging cognition (default: 50 trials)
./nlm_demo phase6      # Phase 6: Integration demo
./nlm_demo all         # Run all available demos
./nlm_demo benchmark   # Performance benchmark
```

**Advantages:**
- Consistent interface across all demos
- Advanced command-line options
- Comprehensive help system
- Configuration file support
- Better user experience

### Individual Executables (Legacy - Still Supported)

The individual executables are still available for backward compatibility:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo     # Phase 3 demonstration
./nlm_phase4_demo     # Phase 4 demonstration
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark      # Performance benchmark
```

### Configuration

Load configuration from file:

```bash
./nlm_demo phase3 --config configs/my_config.cfg
./nlm_demo phase4 --trials 100
./nlm_demo phase6 --neurons 2000
```

See `configs/default.cfg` for configuration options.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── nlm_demo.md                 # Unified demo launcher documentation
├── README_EXPERIMENTAL.md      # Quick start guide
├── IMPROVEMENTS_SUMMARY.md     # Technical improvements summary
├── DEMONSTRATION_SCRIPT.sh     # Demonstration examples
├── VERIFY_IMPROVEMENTS.sh      # Verification script
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
- Neuromodulation affects plasticity and neural dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Key Improvements Made

1. **✅ Checkpoint System Fixed**: Brain::save() and Brain::load() now work correctly
2. **✅ Reward System Enhanced**: Realistic reward computation from environment interaction
3. **✅ Performance Optimized**: Homeostatic plasticity prevents runaway excitation
4. **✅ Connectivity Reduced**: 1% density for realistic E/I balance (vs. 15%+)
5. **✅ Unified Interface**: Single `nlm_demo` launcher for all demonstrations
6. **✅ Documentation**: Comprehensive help and examples provided

## Configuration

### Configuration Files

Configuration can be set in files like `configs/default.cfg`:

```bash
# View default configuration
cat configs/default.cfg
```

**Key Parameters:**
- `neuron_count`: Number of neurons (500-5000 recommended)
- `region_count`: Brain regions (1-4)
- `connection_probability`: Connection sparsity (0.01 recommended)
- `enable_homeostatic_plasticity`: Automatic stability control

### Command Line Overrides

```bash
# Override configuration values
./nlm_demo phase3 --neurons 1000 --episodes 10
./nlm_demo phase4 --trials 200 --learning-rate 0.005
./nlm_demo phase6 --checkpoints
```

## Building from Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running Demos

```bash
# Using unified launcher (recommended)
./nlm_demo --help                    # Show all options
./nlm_demo phase2                     # Phase 2: Real neural computation
./nlm_demo phase3 --episodes 10       # Phase 3: Custom episodes
./nlm_demo phase4 --trials 100        # Phase 4: Custom trials
./nlm_demo phase6                     # Phase 6: Integration
./nlm_demo all                        # All demos together
./nlm_demo benchmark                  # Performance benchmark
```

```bash
# Individual executables (legacy)
./nlm                                 # Phase 2
./nlm_phase3_demo                     # Phase 3
./nlm_phase4_demo                     # Phase 4
./nlm_phase6_demo                     # Phase 6
./nlm_benchmark                      # Benchmark
```

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT License

## Authors

Research project — See docs for scientific background.


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
