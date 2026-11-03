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

## Current status (honest, 2026-11-03)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

**PHASE 2: REAL NEURAL COMPUTATION** - Core implementation complete:
- ✅ Leaky Integrate-and-Fire (LIF) neuron dynamics
- ✅ Event-driven spike propagation with delays
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)

**PHASE 3: WORLD INTERACTION** - Demonstrated in separate demos:
- ✅ Experience → neural activity → action → consequence → learning
- ✅ Sensory input processing and motor output generation
- ✅ Reward-based learning (demonstrated in Phase3Demo)

**PHASE 4: TEMPORAL PROCESSING** - Frameworks exist but not integrated:
- ⚠️ Working memory with persistent activity (framework incomplete)
- ⚠️ Episodic memory with experience encoding (framework incomplete)
- ⚠️ Concept formation and neural planning (framework incomplete)

**PHASE 5: PERFORMANCE AND PERSISTENCE** - Frameworks exist but not integrated:
- ⚠️ Memory pools and event queues (framework)
- ⚠️ Checkpoint system (stub, not functional)
- ⚠️ Performance optimizations (framework)

**PHASE 6: FINAL INTEGRATION** - Integration verification only:
- ⚠️ Integration demonstration exists but only verifies system existence
- ⚠️ Neuromodulation frameworks incomplete (dopamine, curiosity, novelty)
- ⚠️ Prediction system framework incomplete
- ⚠️ Development system framework incomplete

## Realistic Current State

The NLM project currently implements **Phase 2: Real Neural Computation** as a functioning artificial neural substrate. Phase 3 capabilities (world interaction) are demonstrated in separate experiment builds, but most higher-phase systems exist as incomplete frameworks rather than functional components.

**What's Actually Working:**
- Real spiking neural network with ~500 neurons
- ~37,000 random synapses with weights between -1.0 and 1.0
- Event-driven spike propagation with 1-5ms delays
- STDP and Hebbian plasticity rules
- Structural synaptogenesis and pruning
- Basic reward-based learning in world interaction demos

**What's Missing:**
- Complete working memory with persistent activity
- Episodic memory with experience replay
- Functional neuromodulation (dopamine, curiosity, reward prediction error)
- Integrated prediction system
- Full cognitive planning and concept formation
- Functional checkpoint persistence and replay
- Developmental plasticity effects

**Phase 6 integration verification exists but only checks system existence, not functional integration.**

## Current Phase

**PHASE 2: REAL NEURAL COMPUTATION** - Core neural computation system implemented:

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

**Phase 2 demonstrates:**
- Real neuron dynamics with membrane potentials and spiking
- Network connectivity with ~37k random synapses
- Spike propagation through the network
- Activity-dependent synaptic modification through STDP
- Structural plasticity that creates and prunes synapses
- Working memory through neural firing persistence

**Note:** While the README originally claimed "Phase 6: FINAL INTEGRATION," the current implementation in src/main.cpp demonstrates Phase 2 capabilities. Higher phases (3-6) have experimental frameworks and separate demo executables, but most systems exist as incomplete frameworks rather than functional integrated components.

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
./nlm                 # Phase 2 demo in src/main.cpp (current working brain)
./nlm_phase3_demo     # Phase 3 world interaction (experience → neural activity → action → consequence)
./nlm_phase4_demo     # Phase 4 cognition demonstration (prediction, memory, planning)
./nlm_phase6_demo     # Phase 6 integration test (verifies system existence)
./nlm_benchmark       # Performance benchmarking
```

`./nlm_phase6_demo` runs a comprehensive integration test verifying that all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Current Demonstration Hierarchy

**Phase 2 (`./nlm`) - CORE IMPLEMENTATION:**
- Demonstrates real spiking neural computation
- Shows synaptic plasticity (STDP, Hebbian, structural)
- This is the working, functional brain that actually changes through experience

**Phase 3 (`./nlm_phase3_demo`) - WORLD INTERACTION:**
- Experience → neural activity → action → consequence → learning
- Basic world interaction and reward-based learning

**Phase 4 (`./nlm_phase4_demo`) - TEMPORAL PROCESSING:**
- Prediction, working memory, episodic recall
- Neural attention and planning mechanisms

**Phase 5 + Phase 6 Demos:**
- Phase 5: Performance optimizations and checkpointing
- Phase 6: Integration verification (only checks system existence)

**Current State:** The NLM brain is a functioning artificial neural substrate (Phase 2) but most higher-level capabilities (Phase 3-6) exist as frameworks and separate demonstrations rather than integrated components.

## Current Demonstration Hierarchy

**Phase 2 (`./nlm`) - CORE IMPLEMENTATION:**
- Demonstrates real spiking neural computation
- Shows synaptic plasticity (STDP, Hebbian, structural)
- This is the working, functional brain that actually changes through experience

**Phase 3 (`./nlm_phase3_demo`) - WORLD INTERACTION:**
- Experience → neural activity → action → consequence → learning
- Basic world interaction and reward-based learning

**Phase 4 (`./nlm_phase4_demo`) - TEMPORAL PROCESSING:**
- Prediction, working memory, episodic recall
- Neural attention and planning mechanisms

**Phase 5 + Phase 6 Demos:**
- Phase 5: Performance optimizations and checkpointing
- Phase 6: Integration verification (only checks system existence)

**Current State:** The NLM brain is a functioning artificial neural substrate (Phase 2) but most higher-level capabilities (Phase 3-6) exist as frameworks and separate demonstrations rather than integrated components.

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
│   ├── brain/          # Neural components (Phase 2 implemented)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system (framework)
│   ├── neuromodulation/ # Neuromodulators (frameworks)
│   ├── memory/        # Memory systems (frameworks)
│   ├── prediction/     # Prediction systems (frameworks)
│   ├── cognition/     # Cognitive mechanisms (frameworks)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment frameworks (Phase 3-6)
│   └── visualization/ # Visualization
├── tests/             # Unit tests (Phase 2)
└── configs/           # Configuration files
```

**Note:** While the structure suggests a complete Phase 6 system, the actual implementation reflects the current development reality: Phase 2 neural computation is operational, with Phase 3-6 capabilities existing as experimental frameworks and separate demonstration executables.

### Phase 2 (Complete)

- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 3 (Implemented but not in main build)

- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Frameworks exist but not integrated)

- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Frameworks exist but not functional)

- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system (stub that returns false)

### Phase 6 (Integration verification only)

- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working (but stub returns false)
- Replay and consolidation functional (but demos report 0 reward)

**Important Note:** While the README claims "Phase 6: FINAL INTEGRATION," the main demonstration (src/main.cpp) only implements Phase 2 capabilities. Phase 3-6 capabilities exist as separate demo executables and experimental frameworks but are not integrated into the core brain simulation loop.

The NLM project is currently at **Phase 2: Real Neural Computation** - a functioning artificial neural substrate capable of changing its own synaptic connections through experience, but missing most higher-level cognitive and integration capabilities.

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
