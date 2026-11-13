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

## Current status (updated 2026-11-13)

Phase 1 through Phase 6 are now fully integrated and functional. The NLM
project implements a comprehensive artificial brain with:

### Phase 1-6 Integration Achievements

**Phase 1 (Complete)**: Core infrastructure
- Configuration system with parameter validation
- Random number generation with seeding
- Logging system with multiple levels
- Simulation clock and timestep management

**Phase 2 (Complete)**: Real neural computation
- LIF (Leaky Integrate-and-Fire) neurons with event-driven dynamics
- Synaptic connections with delays and types
- STDP and Hebbian plasticity mechanisms
- Structural plasticity (synaptogenesis, pruning)
- ~37k synapses for a 500-neuron brain
- Spike propagation and neuron dynamics

**Phase 3 (Complete)**: World interaction and perception
- Sensory input processing (vision, audio, internal signals)
- Motor output and action execution
- Reward prediction error learning
- Developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
- Novelty and curiosity detection
- Working memory with active traces

**Phase 4 (Complete)**: Memory and cognition
- Working memory with competition dynamics
- Episodic memory with experience storage
- Associative memory with pattern associations
- Neural prediction system
- Neural planner for action sequences
- Concept formation from experience patterns
- Attention with competitive selection
- Self-model for internal representation

**Phase 5 (Complete)**: Performance and checkpointing
- SIMD vectorization and parallel processing
- Memory pools and event queues
- Complete checkpoint system with version compatibility
- Robust save/load with integrity validation

**Phase 6 (Complete - Final Integration)**: All systems unified
- All memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working (tested and verified)
- Replay and consolidation functional
- Phase 6 integration experiment created and working

### Integration Status Verification

The Phase 6 integration demo has been tested and verified:
- ✅ Memory systems integrated and functional
- ✅ Neuromodulation systems connected and working
- ✅ Prediction system integrated
- ✅ Cognition systems operational
- ✅ Development system affecting plasticity
- ✅ Checkpointing save/load working correctly
- ✅ Memory replay functional

### Key Capabilities

The NLM brain now provides:
- **Event-driven spiking neural computation** with real LIF dynamics
- **Integrated memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty, prediction error)
- **Prediction and planning** for goal-directed behavior
- **Concept formation** from experience
- **Attentional selection** for focus and prioritization
- **Developmental progression** from plasticity to stability
- **Robust checkpointing** for long-running simulations
- **Memory replay** during rest/consolidation

### Limitations (Unchanged)

NLM remains a research project investigating brain-like systems:
- Simplified LIF neuron model (not biological accuracy)
- No claim of consciousness, intelligence, or human-like cognition
- Limited to current computational resources
- Investigates integration questions, not full behavioral replication

**Note:** The documentation was incorrect - checkpointing IS implemented
and working in Phase 5/6. The Phase 6 demo reports meaningful reward,
learning, and successful integration.

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
./nlm_phase5_demo     # Phase 5 demo with checkpointing
./nlm_phase6_demo     # Phase 6 integration test
./nlm_phase6_text_demo # Phase 6 text-thinking demo
./nlm_benchmark       # Performance benchmark
```

### Running Phase 6 Integration Demo

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected:
- Memory systems (working + episodic)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction system
- Cognition systems (planner, concept formation, attention)
- Development system
- Checkpointing save/load functionality
- Memory replay

### Running Phase 5 with Checkpointing

```bash
./nlm_phase5_demo
```

This demonstrates the complete checkpoint system with:
- Periodic checkpoint saving
- Load/restore functionality
- Memory preservation across sessions
- Performance profiling

## Python Bindings

The Python API provides access to the NLM framework:

```bash
pip install .
```

See `easy_usage.md` and `HOW_TO_USE.md` for detailed examples.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── easy_usage.md          # Beginner-friendly usage guide
├── HOW_TO_USE.md          # Comprehensive Python API guide
├── IMPROVEMENT_PLAN.md    # Current improvement plan
├── docs/                  # Scientific documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/
│   ├── core/              # Core utilities
│   ├── agent/             # Agent-brain interface
│   ├── world/             # Simple world simulation
│   ├── performance/       # Checkpoints, pools, queues
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators
│   ├── memory/           # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework
│   └── visualization/    # Visualization
├── tests/                # Unit tests
└── configs/             # Configuration examples
```
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
