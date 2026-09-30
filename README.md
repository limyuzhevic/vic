# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Project Clarification

This document clarifies the current project status and provides accurate information about the NLM (Neural Learning Machine) development phases.

**Current Status: PHASE 6 - Final Integration**

Phase 6 represents the final integration of all previous phases into a coherent artificial brain system. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

**Phase Clarification:**
- **Phase 1 (Complete):** Project skeleton, core types, neural interfaces
- **Phase 2 (Complete):** Real LIF neuron dynamics, event-driven processing, STDP, structural plasticity
- **Phase 3 (Complete):** World interaction, sensory input/motor output, reward prediction error, development, novelty, curiosity
- **Phase 4 (Complete):** Neural prediction system, working memory, episodic memory, concept formation, attention, planning, self-model, social learning, continual learning
- **Phase 5 (Complete):** Performance optimizations, memory pools, SIMD, parallel processing, checkpoint system
- **Phase 6 (IN PROGRESS - Final Integration):** All systems integrated, memory-neural connections, neuromodulation effects, development integration, checkpoint/replay functional

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements in Phase 6

Phase 6 focuses on integrating all existing systems into a coherent artificial brain:

### Core Integration Achievements

#### Memory Systems Integration
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration  
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System Integration
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognitive Systems Integration
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Advanced Features
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Technical Implementation Notes

Phase 6 implements a closed-loop brain architecture:

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

## Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running Phase 6 Integration Test

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
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                    # Source code
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators (integrated)
│   ├── memory/           # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework
│   └── visualization/     # Visualization
├── tests/                 # Tests
└── configs/               # Configuration
```

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

## Integration Status

Phase 6 achieves:
- ✓ Memory systems connected to neural processing
- ✓ Neuromodulation affects plasticity and dynamics
- ✓ Prediction integrated with learning
- ✓ Development affects plasticity rates
- ✓ Checkpoint save/load working
- ✓ Replay and consolidation functional
- ✓ Phase 6 integration experiment created

## Authors

Research project — See docs for scientific background.

