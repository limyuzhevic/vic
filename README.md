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
./nlm
```

This runs a comprehensive integration test verifying all brain systems are connected.

Alternative demo executables are also available:
- `./nlm_phase3_demo` - Phase 3 world interaction demo
- `./nlm_phase4_demo` - Phase 4 emerging cognition demo
- `./nlm_phase6_demo` - Alternative Phase 6 demo (same as nlm)

## Command Line Arguments

### For the main `nlm` executable (Phase 6 demo):
```bash
./nlm [num_episodes] [steps_per_episode]

Arguments:
- num_episodes: Number of episodes to run (default: 5)
- steps_per_episode: Steps per episode (default: 2000)
```

### For Phase 3 demo:
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]

Arguments:
- num_episodes: Number of episodes to run (default: 5)
- steps_per_episode: Steps per episode (default: 200)
```

### For Phase 4 demo:
```bash
./nlm_phase4_demo [num_trials]

Arguments:
- num_trials: Number of trials to run (default: 50)
```

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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
│   └── easy_usage.md    # Easy usage guide
│   └── HOW_TO_USE.md     # Detailed usage guide
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

## Quick Start

1. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the Phase 6 integration demo (recommended):**
   ```bash
   ./nlm
   ```
   This will test all integrated brain systems and show whether they work together.

3. **Or try older demos:**
   ```bash
   ./nlm_phase3_demo 10 500    # Phase 3 world interaction
   ./nlm_phase4_demo 100      # Phase 4 emerging cognition
   ```

## Integration Testing

The Phase 6 demo (`./nlm`) specifically tests:
- **Memory integration:** Working and episodic memory systems
- **Neuromodulation integration:** Dopamine, curiosity, novelty
- **Prediction integration:** Prediction system functionality
- **Development integration:** Age-dependent plasticity
- **Checkpointing:** Save/load functionality
- **Replay:** Memory replay and consolidation

If all systems show "CONNECTED" or "WORKING" in the final output, then the integration was successful!

## Improving NLM

If you want to improve NLM:

1. **Add new brain regions:** Look at `src/brain/NeuralRegion.hpp/cpp`
2. **New plasticity rules:** Add to `src/plasticity/` directory
3. **New neuromodulators:** Add to `src/neuromodulation/` directory
4. **New memory types:** Add to `src/memory/` directory
5. **Better prediction models:** Look at `src/prediction/` directory
6. **Enhanced cognition:** Add to `src/cognition/` directory
7. **Python bindings:** Add to `python/` directory

The project is designed to be extensible - you can add new components without breaking existing functionality.

---

**Note:** The main `nlm` executable now runs the Phase 6 integration demo. For simpler usage, try the Phase 3 (`./nlm_phase3_demo`) or Phase 4 (`./nlm_phase4_demo`) demos first to understand the basic operation before trying the full Phase 6 integration.