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

## Current status (honest, 2026-11-10)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Phase 3 and Phase 4 demonstrated sensory-motor interaction and
emergent cognition (planning, concept formation, attention, memory).
Phase 5 added performance infrastructure and proper reward-based
learning.

Phase 6 (Current - FINAL INTEGRATION): All major systems are integrated
and the brain functions as a coherent artificial brain capable of:
- Processing sensory information from SimpleWorld
- Forming working and episodic memories
- Applying neuromodulation (dopamine, novelty, curiosity)
- Predicting action consequences
- Selective attention and action selection
- Development with stage progression
- Memory replay and consolidation
- Proper persistent storage via checkpoint system

The current integration allows basic intelligent behavior but still requires:
- Better memory retention mechanisms
- More sophisticated prediction
- Enhanced planning capabilities
- Multi-step concept formation
- Improved exploration/exploitation balance

All core neural mechanisms are now integrated and working together as an artificial brain.

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

### Phase 1 (Initial: Complete)
- **Core**: Project skeleton, core types and configuration
- **Status**: ✅ Complete - foundational architecture

### Phase 2 (Real Neural Computation: Complete)
- **Core**: LIF neuron dynamics, event-driven spike processing
- **Plasticity**: STDP and Hebbian rules, structural plasticity
- **Status**: ✅ Complete - functioning neural substrate

### Phase 3 (World Integration: Complete)
- **Core**: Neural dynamics with sensory input and motor output
- **Cognition**: Reward prediction error, developmental stages
- **Status**: ✅ Complete - working with environment

### Phase 4 (Emergent Cognition: Complete)
- **Core**: Working memory, episodic memory, associative memory
- **Cognitive**: Neural planner, concept formation, self-model, attention, social learning
- **Prediction**: Sequence prediction and prediction error
- **Status**: ✅ Complete - all systems defined

### Phase 5 (Performance: Complete)
- **Core**: Performance optimizations, memory pools, queues
- **Plasticity**: Neuromodulation (dopamine, novelty, curiosity)
- **Status**: ✅ Complete - efficient implementation

### Phase 6 (Final Integration: IN PROGRESS)
- **Core**: Full integration of all Phase 4 and 5 systems
- **Memory**: Working, episodic, associative memory systems connected
- **Integration**: Neuromodulation affects plasticity and neural dynamics
- **Prediction**: Integrated with cognitive systems
- **Development**: Affects plasticity rates, stage transitions
- **Persistence**: Checkpoint save/load functional
- **Replay**: Memory consolidation and replay operational
- **Current Focus**: Refining integration, improving connections
- **Status**: ⚠️ Significant progress - ~70% effective integration

## Current Integration Status

The NLM brain now implements a **coherent artificial brain loop**:

```
SENSORY INPUT → NEURAL PROCESSING → WORKING MEMORY → PREDICTION → ACTION SELECTION → MOTOR OUTPUT → WORLD → REWARD → NEUROMODULATION → PLASTICITY → MEMORY/DEVELOPMENT
```

### What's Working:
- ✅ LIF neuron dynamics with event-driven spikes
- ✅ Synaptic transmission with delays
- ✅ STDP and Hebbian plasticity
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Working memory with persistent activity
- ✅ Episodic memory with experience encoding
- ✅ Neuromodulation (dopamine, curiosity, novelty)
- ✅ Prediction system with error signals
- ✅ Attention with competitive selection
- ✅ Planning with action sequence evaluation
- ✅ Self-model and social learning
- ✅ Checkpoint save/load implementation
- ✅ Memory replay and consolidation
- ✅ Developmental stage progression
- ✅ Proper integration between systems

### What's Being Refined:
- **Memory retention**: More robust memory consolidation
- **Prediction accuracy**: Better prediction error signals
- **Planning**: Multi-step goal-directed actions
- **Concept formation**: Pattern discovery from experience
- **Exploration**: Better curiosity-driven behavior
- **Integration**: Stronger connections between systems
- **Efficiency**: Optimized neural resource usage

## Architecture Philosophy

The NLM brain operates as an **integrated system**:

1. **Sensory Processing**: World input drives neural activity
2. **Memory Integration**: Working and episodic memory store experiences
3. **Prediction & Attention**: Anticiates sensory consequences and focuses resources
4. **Action Selection**: Plans and selects appropriate motor commands
5. **Learning Loop**: Rewards drive neuromodulation and synaptic changes
6. **Development**: Adaptive changes over time
7. **Persistence**: Saves learned behaviors across sessions

The brain is designed as a **complete system** that learns from experience,
avoids catastrophic forgetting, and develops increasingly sophisticated behaviors through developmental stages.

## License

MIT

## Authors

Research project — See docs for scientific background.

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
