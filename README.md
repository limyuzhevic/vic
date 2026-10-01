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

**PHASE 2/4 HYBRID: Real Neural Computation + Advanced Cognitive Systems**

This project represents a hybrid approach:

- **Phase 2 Core**: Real LIF neuron dynamics, event-driven spike propagation, STDP/Hebbian plasticity
- **Phase 4 Integration**: Working memory, episodic memory, prediction systems, cognition, attention, planning
- **Phase 6 Integration Framework**: Architecture designed for full integration but currently requires completion

**Key Status:** The neural hardware (Phase 2) and cognitive systems (Phase 4) are implemented but not fully integrated into a coherent artificial brain. The project needs Phase 6 integration to connect all systems into a unified artificial brain.

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

### Phase 6 (INCOMPLETE - Hybrid Implementation)
- The current implementation is a hybrid of Phase 2 (neural core) and Phase 4 (cognitive systems)
- All systems are **implemented but NOT integrated** into a coherent brain loop
- The Brain::step() method only partially integrates the systems
- This project is investigating whether memory systems can integrate with neural dynamics
- This project is investigating whether neuromodulation can affect plasticity in a coordinated way
- This project is investigating whether prediction can become a central organizing principle
- This project is investigating whether developmental stages can modulate learning
- This project is investigating whether replay can reinforce memory consolidation
- **NOT completed**: Whether the complete brain loop functions coherently

**Phase 6 Goal:** Create a unified artificial brain where all systems (memory, prediction, cognition, neuromodulation, development) work together as a coherent whole, not just as disconnected components.

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

The NLM brain aims to operate as a closed-loop system, but currently implements:

**Working Components (Phase 2 + Phase 4):**
- ✅ **WORLD → SENSORY INPUT**: Simple world interface and sensory processing
- ✅ **SENSORY INPUT → NEURAL PROCESSING**: LIF neuron dynamics, spike propagation
- ✅ **NEURAL PROCESSING → INTERNAL STATE**: Basic neuromodulation and plasticity
- ✅ **INTERNAL STATE → MEMORY / PREDICTION**: Implemented but not well-integrated
- ✅ **MEMORY / PREDICTION → MOTIVATION / NEUROMODULATION**: Partially connected
- ✅ **MOTIVATION / NEUROMODULATION → ACTION SELECTION**: Implemented
- ✅ **ACTION SELECTION → MOTOR OUTPUT**: Working motor system
- ✅ **MOTOR OUTPUT → WORLD CONSEQUENCE**: Environment interaction
- ✅ **WORLD CONSEQUENCE → REWARD / SURPRISE / ERROR**: Basic reward signals
- ✅ **REWARD / SURPRISE / ERROR → PLASTICITY**: STDP, Hebbian, structural plasticity
- ✅ **PLASTICITY → MEMORY / DEVELOPMENT**: Implemented but not coordinated
- ✅ **MEMORY / DEVELOPMENT → CHANGED BRAIN**: Neural changes occur
- ✅ **CHANGED BRAIN → CHANGED FUTURE BEHAVIOR**: Behavior changes result

**Missing/Incomplete Connections:**
- ❌ Working memory <-> Episodic memory integration
- ❌ Prediction system <-> Neural processing integration  
- ❌ Cognition systems <-> Attention/mood connections
- ❌ Neuromodulation <-> Memory consolidation
- ❌ Sleep/rest cycle with memory replay
- ❌ Proper developmental progression
- ❌ Continual learning without catastrophic forgetting

**Current Focus:** Phase 6 integration - making these connections work together as a coherent artificial brain rather than just implementing more features.

## License

MIT

## Authors

Research project — See docs for scientific background.
