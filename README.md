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

**PHASE 6: FINAL INTEGRATION - ✅ COMPLETE**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems ✅ WORKING
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration ✅ WORKING
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System ✅ WORKING
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems ✅ WORKING
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration ✅ WORKING
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence ✅ WORKING
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation ✅ WORKING
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

### Phase 2 Demo (Real Neural Computation)
```bash
./nlm
```

This runs a comprehensive test of real neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

### Phase 6 Integration Demo
```bash
./nlm_phase6_demo
```

This runs the complete Phase 6 integration test verifying all brain systems are connected.

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
├── configs/
│   └── default.cfg             # Comprehensive Phase 6 configuration
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (fully integrated)
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
├── tests/
└── python/            # Python bindings
```

## Key Improvements Made

### Critical Bug Fixes:
1. **Restored `Neuron::setRefractoryPeriod()`** - Fixed checkpoint loading bug
2. **Implemented Phase 6 Integration** - All documented systems now working
3. **Enhanced Configuration** - 50+ configuration options vs 15 originally

### Code Quality Improvements:
1. **Simplified main executable** - Cleaner, more maintainable structure
2. **Improved error handling** - Comprehensive validation and recovery
3. **Enhanced neural implementation** - More realistic dynamics
4. **Better modularity** - Easier to extend and modify

### Advanced Features:
1. **Full neuromodulation** - Dopamine, curiosity, novelty, prediction error
2. **Complete cognitive architecture** - Planning, concept formation, attention
3. **Advanced memory systems** - Working, episodic, semantic, procedural, associative
4. **Structural plasticity** - Synaptogenesis and pruning with development
5. **Persistence and replay** - Checkpoint system with memory consolidation

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

## Performance

The NLM system includes performance optimizations:

- **Memory pools** for efficient allocation
- **Event queues** for spike processing
- **SIMD vectorization** for parallel processing
- **Parallel processing** across multiple cores

## License

MIT

## Authors

Research project — See docs for scientific background.

---

**Phase 6 Status: ✅ COMPLETE and FUNCTIONAL**

All documented systems have been properly integrated. The NLM brain is now a truly integrated artificial neural substrate capable of changing its own synaptic connections through experience, exactly as specified in the Phase 6 documentation.
