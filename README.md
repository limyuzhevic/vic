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

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working (Phase 6 integrated experiment)
- Replay and consolidation functional (Phase 6 integrated experiment)
- Phase 6 integration experiment created (demonstrates all systems connected)

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
- **ALL SYSTEMS INTEGRATED into coherent brain loop**
- Memory systems (working memory, episodic memory, associative memory) connected to neural processing
- Neuromodulation (dopamine, curiosity, novelty, prediction error) integrated with plasticity and dynamics
- Prediction system integrated with sensory processing
- Development system affecting plasticity rates throughout brain
- Checkpoint save/load working (demonstrated in Phase 6 integration experiment)
- Replay and consolidation functional (demonstrated in Phase 6 integration experiment)
- **Phase 6 integration experiment created that verifies all systems work together**

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

This runs the Phase 6 integration test that verifies:
- All brain systems are connected and functional
- Memory systems work together
- Neuromodulation affects learning and behavior
- Prediction and attention are integrated
- Development modulates plasticity over time
- Checkpointing allows state preservation
- Replay consolidates memories

### Unified NLM Command Line
The main `nlm` executable now supports multiple phases:

```bash
# Run Phase 6 integration demo (default)
nlm --phase6

# Run Phase 2 neural computation demo
nlm --phase2

# Show help
nlm --help
```

### Python Bindings
```bash
# Install Python bindings (optional)
pip install scikit-build-core pybind11
pip install .
```

Then in Python:
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes generated.")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Scientific documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Updated integration audit
├── src/
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── memory/           # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/        # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   ├── experiments/      # Experiment framework (Phase 6)
│   └── visualization/    # Visualization
├── tests/                # Unit tests and integration tests
└── configs/              # Configuration files
```

## Phase 6 Integration Status

The Phase 6 integration has successfully connected all previously disconnected systems:

| System | Integration Status | Notes |
|--------|-------------------|-------|
| Working Memory | ✅ INTEGRATED | Connected to sensory input, attention, and neural activity |
| Episodic Memory | ✅ INTEGRATED | Stores experiences, supports replay and consolidation |
| Semantic Memory | ✅ DEFINED | Available for future enhancement |
| Procedural Memory | ✅ DEFINED | Available for future enhancement |
| Prediction System | ✅ INTEGRATED | Connected to sensory processing and attention |
| Neural Planner | ✅ INTEGRATED | Connected to action selection |
| Concept Formation | ✅ INTEGRATED | Connected to working memory and attention |
| Attention System | ✅ INTEGRATED | Connected to working memory competition |
| Development System | ✅ INTEGRATED | Affects plasticity throughout brain |
| Neuromodulation | ✅ INTEGRATED | Dopamine, curiosity, novelty, prediction error |
| Checkpoint System | ✅ INTEGRATED | Save/load brain state with CheckpointManager |

## Current Phase 6 Demo Results

Running `./nlm_phase6_demo` will show:

- ✅ Integration verification (all systems present)
- ✅ Memory integration tests (working memory, episodic memory)
- ✅ Neuromodulation integration tests (dopamine, curiosity, novelty)
- ✅ Checkpoint save/load tests
- ✅ Replay system tests
- ✅ Full integration experiment (500 neurons, 2000 steps)

Expected results:
- Total reward accumulated throughout simulation
- Average firing rate showing network activity
- Episodes stored in episodic memory
- Dopamine level showing reward processing
- All integration status indicators showing "CONNECTED"

## Scientific Significance

NLM demonstrates:

1. **Experience-driven learning**: Synaptic changes through interaction with environment
2. **Development-driven maturation**: Plasticity rates change over "age"
3. **Neural plasticity**: STDP, Hebbian, and structural plasticity work together
4. **Memory integration**: Working memory, episodic memory, and prediction systems interact
5. **Neuromodulation**: Dopamine, curiosity, and novelty affect learning and behavior
6. **Embodied intelligence**: Sensorimotor loop enables real interaction
7. **Continual learning**: Concepts form and generalize from experience
8. **Predictive processing**: Brain predicts sensory input and updates based on errors

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
