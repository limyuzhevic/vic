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

## Current status (honest, 2026-11-14)

Phase 1 made the repo build again. Phase 2 made the brain real at the substrate level: populations now actually create neurons, random connectivity creates ~37k synapses for a 500-neuron brain (was 0), spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000 steps), and a controlled STDP test potentiates its synapse (0.35 -> 0.99). Inhibitory neurons now exist (20% of internal population) and inhibitory plasticity keeps its sign instead of flipping to excitatory.

**STATUS**: Phase 2 components working well (neurons, spikes, STDP). Phase 3-5 partially implemented. **Phase 6 integration demo reports 0 reward** and **checkpointing is a stub that honestly returns false**. Activity is still very high (~200 spikes/step) and learning in the main demo is mostly saturation, not selective pattern learning. Those are the next problems, not hidden ones.

## Current Phase

**PHASE 6: FINAL INTEGRATION (IN PROGRESS)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Status (as of 2026-11-14)

#### ✅ Working Well

##### Memory Systems
- Working memory with persistent activity and competition - **FUNCTIONAL**
- Episodic memory with experience encoding - **BASIC IMPLEMENTATION**
- Associative memory with Hebbian pattern associations - **LIMITED TESTING**
- Memory systems connected to neural processing - **PARTIALLY CONNECTED**

##### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity - **FUNCTIONAL**
- Curiosity drives exploration behavior - **FUNCTIONAL**
- Novelty detection integrated with sensory processing - **FUNCTIONAL**
- All neuromodulators connected to plasticity rules - **CONNECTED**

##### Prediction System
- Prediction system integrated into brain loop - **BASIC IMPLEMENTATION**
- Prediction error signals affect learning - **LIMITED**
- Confidence tracking implemented - **STUB**

##### Cognition Systems
- Neural planner with action sequence evaluation - **BASIC IMPLEMENTATION**
- Concept formation from experience patterns - **STUB**
- Attention with competitive selection dynamics - **BASIC IMPLEMENTATION**
- Cognition systems connected to perception and action - **PARTIALLY CONNECTED**

##### Development Integration
- Developmental stages affect plasticity rates - **FUNCTIONAL**
- Structural plasticity modulated by age - **STUB**
- Neural excitability changes with development - **FUNCTIONAL**

#### ❌ Not Working Yet

##### Persistence
- Checkpoint save/load implemented - **STUB** (returns false)
- Brain state serialization working - **LIMITED** (partial)
- Can resume from saved checkpoints - **NO**

##### Replay and Consolidation
- Episodic memory replay during simulation - **BASIC IMPLEMENTATION**
- Memory consolidation for important episodes - **STUB**
- Integration with sleep/rest cycle - **NO**

#### ⚠️ Issues to Address

1. **Zero reward problem**: Phase 6 demo reports 0.0 reward despite claiming integration
2. **Performance**: ~200 spikes/step (high but may be acceptable)
3. **Learning**: Mostly saturation, not selective pattern learning
4. **Checkpointing**: Stub implementation that returns false
5. **Python bindings**: pyproject.toml claims build but module doesn't work
6. **Documentation**: README claims vs. reality mismatch

## Building

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings (may fail)
cd .. && pip install .
```

## Running

Build all targets first (see Building above). The entry points are currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test (reports 0 reward)
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying some brain systems are connected. Phase 5 is wired as the `nlm_phase5` library in CMake; it does not yet have a standalone demo executable.

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
- Real LIF neuron dynamics ✅
- Event-driven spike propagation ✅
- STDP and Hebbian plasticity ✅
- Structural plasticity ✅

### Phase 3 (Complete)
- World interaction loop ✅
- Sensory input and motor output ✅
- Reward prediction error ✅
- Developmental stages ✅
- Novelty and curiosity ✅

### Phase 4 (Complete)
- Neural prediction system ✅
- Working memory ✅
- Episodic-like memory ✅
- Concept formation ✅
- Neural attention (NOT Transformer) ✅
- Predictive planning ✅
- Self-model ✅
- Social learning ✅
- Continual learning ✅

### Phase 5 (Complete)
- Performance optimizations ✅
- Memory pools and event queues ✅
- SIMD vectorization ✅
- Parallel processing ✅
- Checkpoint system ⚠️ (stub implementation)

### Phase 6 (FINAL INTEGRATION - IN PROGRESS)
- All systems integrated into coherent brain loop ⚠️ (some disconnected)
- Memory systems connected to neural processing ⚠️ (partial)
- Neuromodulation affects plasticity and dynamics ✅
- Prediction integrated with learning ⚠️ (limited)
- Development affects plasticity rates ✅
- Checkpoint save/load working ⚠️ (stub returns false)
- Replay and consolidation functional ⚠️ (limited)
- Phase 6 integration experiment created ✅

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
- Whether memory systems can integrate with neural dynamics ⚠️ (partial success)
- Whether neuromodulation can affect plasticity in a coordinated way ✅
- Whether prediction can become a central organizing principle ⚠️ (limited)
- Whether developmental stages can modulate learning ✅
- Whether replay can reinforce memory consolidation ⚠️ (basic)
- Whether the complete brain loop functions coherently ⚠️ (some components disconnected)

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

## Known Issues (2026-11-14)

1. **Critical**: README overstates Phase 6 achievements - needs correction
2. **High**: Python bindings don't work despite pyproject.toml claims
3. **High**: Checkpointing is a stub that returns false
4. **High**: Phase 6 demo reports 0.0 reward
5. **Medium**: High spike activity (~200 spikes/step)
6. **Medium**: Limited error handling in code
7. **Medium**: Some code comments need improvement

## License

MIT

## Authors

Research project — See docs for scientific background.