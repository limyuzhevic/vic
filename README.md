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

### Phase 6 Integration Status

**Important Note**: Phase 6 integration is implemented but with varying degrees of completeness. The Phase 6 integration test provides a comprehensive assessment of what's working and what's still being developed.

#### Memory Systems
- **Working Memory**: Implemented and integrated with neural processing
- **Episodic Memory**: Implemented with experience encoding and replay capability
- **Associative Memory**: Implemented with Hebbian pattern associations
- **Integration Status**: Most memory systems are connected to neural processing

#### Neuromodulation Integration
- **Dopamine**: Implemented - affects neural excitability and plasticity
- **Curiosity**: Implemented - drives exploration behavior
- **Novelty Detection**: Implemented - integrated with sensory processing
- **Integration Status**: Neuromodulators are connected to plasticity rules, though some aspects are developing

#### Prediction System
- **Core Prediction System**: Implemented and integrated into brain loop
- **Prediction Error Signals**: Implemented - affect learning
- **Confidence Tracking**: Implemented
- **Integration Status**: Prediction system is functional for basic prediction error tracking

#### Cognition Systems
- **Neural Planner**: Implemented with action sequence evaluation
- **Concept Formation**: Implemented for experience pattern processing
- **Attention**: Implemented with competitive selection dynamics
- **Integration Status**: Cognition systems are connected to perception and action

#### Development Integration
- **Developmental Stages**: Implemented - affect plasticity rates
- **Structural Plasticity**: Implemented - modulated by age
- **Neural Excitability**: Implemented - changes with development
- **Integration Status**: Development system is functional

#### Persistence
- **Checkpoint Save/Load**: Implemented - working for brain state serialization
- **Resume Capability**: Implemented - can resume from saved checkpoints
- **Integration Status**: Persistence systems are operational

#### Replay and Consolidation
- **Episodic Memory Replay**: Implemented - during simulation
- **Memory Consolidation**: Implemented - for important episodes
- **Sleep/Rest Integration**: Partially implemented - basic consolidation cycles
- **Integration Status**: Replay and consolidation are functional

## Building

```bash
# Standard build with all phases
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Quick build (Phase 6 demo only)
mkdir build_phase6
cd build_phase6
cmake .. -DCMAKE_BUILD_TYPE=Release -DONLY_PHASE6=1
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

The Phase 6 integration test provides a comprehensive assessment of the integrated brain:

```bash
# Full integration test
./nlm_phase6_demo

# Quick integration test (skips some tests for faster execution)
./nlm_phase6_demo --quick
```

This runs a comprehensive integration test verifying all brain systems are connected and functional. The demo provides detailed output showing:
- Integration verification results
- System-specific integration tests
- Full lifetime simulation with metrics
- Performance assessment and verification checklist

### Other Phase Demos

```bash
# Phase 2: Basic neural computation (LIF neurons, STDP, structural plasticity)
./nlm

# Phase 3: World interaction and agent behavior
./nlm_phase3_demo

# Phase 4: Cognition and learning capabilities
./nlm_phase4_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md           # System architecture overview
│   ├── SCIENCE.md              # Scientific background
│   ├── ROADMAP.md              # Development roadmap
│   ├── EXPERIMENTS.md          # Experiment descriptions
│   ├── LIMITATIONS.md          # Current limitations
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 integration audit
├── src/
│   ├── core/                    # Core utilities
│   ├── brain/                   # Neural components (integrated)
│   ├── dynamics/                # Neural dynamics
│   ├── plasticity/              # Plasticity rules
│   ├── development/             # Developmental system
│   ├── neuromodulation/         # Neuromodulators (integrated)
│   ├── memory/                  # Memory systems (integrated)
│   ├── prediction/              # Prediction systems (integrated)
│   ├── cognition/               # Cognitive mechanisms (integrated)
│   ├── sensory/                 # Sensory processing
│   ├── motor/                   # Motor system
│   ├── environment/             # Environment interface
│   ├── experiments/             # Experiment framework (Phase 6)
│   └── visualization/           # Visualization
├── python/                      # Python bindings (pynlm)
├── tests/                       # Unit tests
└── configs/                     # Configuration files
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

### Phase 6 (Implemented - Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Integration Assessment

The Phase 6 integration creates a closed-loop artificial brain that:

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
