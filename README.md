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

# Advanced Command Line Interface

NLM now features an enhanced command-line interface for advanced users:

### Phase Selection

**Default Phase (Phase 2):** Real neural computation
```bash
./nlm
```

**Integration Phase (Phase 6):** Full system integration
```bash
./nlm_phase6_demo
```

**Integration Test:** Verify all systems work together
```bash
./nlm --test-integration
```

### Advanced Commands

**Enable advanced features:**
```bash
./nlm --advanced
./nlm_phase6_demo --advanced
```

**Quick functionality test:**
```bash
./nlm --quick
./nlm_phase6_demo --quick
```

**Run integration verification:**
```bash
./nlm --test-integration
./nlm_phase6_demo --test-integration
```

**Display memory statistics:**
```bash
./nlm --memory-stats
./nlm_phase6_demo --memory-stats
```

**Performance benchmark:**
```bash
./nlm --benchmark
./nlm_phase6_demo --benchmark
```

**Save brain state:**
```bash
./nlm --output=brain_state.bin
./nlm_phase6_demo --output=phase6_state.bin
```

**Experience levels:**
```bash
# Basic mode (simplified configuration)
./nlm --novice

# Advanced mode (full control)
./nlm --expert

# Research mode (maximum observability)
./nlm --research
```

**Performance modes:**
```bash
# Optimized for speed
./nlm --performance

# Optimized for learning accuracy
./nlm --accuracy

# Balanced performance
./nlm --balanced
```

### Advanced Configuration Options

**Memory systems:**
```bash
./nlm --working_memory_capacity=1000
./nlm --episodic_memory_capacity=5000
./nlm --replay_enabled
./nlm --consolidation_enabled
```

**Cognition and prediction:**
```bash
./nlm --attention_enabled
./nlm --prediction_enabled
./nlm --cognitive_planning_enabled
```

**Performance optimizations:**
```bash
./nlm --memory_pool_size=2000
./nlm --event_queue_size=1000
./nlm --simd_enabled
./nlm --parallel_enabled
```

**Development features:**
```bash
./nlm --enable_curiosity
./nlm --enable_structural_plasticity
./nlm --enable_development
./nlm --checkpoint_path=./checkpoints/
```

**Neuromodulation control:**
```bash
./nlm --enable_dopamine
./nlm --enable_curiosity
./nlm --enable_novelty
./nlm --enable_prediction_error
```

### Complex Examples

**Advanced integration demo with full monitoring:**
```bash
./nlm_phase6_demo \
  --advanced \
  --test-integration \
  --memory-stats \
  --output=integrated_brain.bin \
  --benchmark \
  --enable_curiosity \
  --enable_structural_plasticity \
  --enable_development \
  --checkpoint_path=./checkpoints/phase6
```

**Research mode with maximum observability:**
```bash
./nlm_phase6_demo \
  --research \
  --memory-stats \
  --log_file=research.log \
  --log_level=DEBUG \
  --replay_enabled \
  --consolidation_enabled \
  --attention_enabled \
  --prediction_enabled \
  --cognitive_planning_enabled
```

**Performance tuning for large-scale simulations:**
```bash
./nlm_phase6_demo \
  --performance \
  --memory_pool_size=5000 \
  --event_queue_size=3000 \
  --simd_enabled \
  --parallel_enabled \
  --working_memory_capacity=2000 \
  --episodic_memory_capacity=10000 \
  --neuron_count=5000 \
  --region_count=8
```

**Quick development test:**
```bash
./nlm_phase6_demo \
  --quick \
  --test-integration \
  --enable_curiosity \
  --enable_structural_plasticity \
  --enable_development
```

### Help and Documentation

**Show advanced help:**
```bash
./nlm --help
./nlm_phase6_demo --help
```

**Show version information:**
```bash
./nlm --version
```

**See all command-line options:**
```bash
./nlm --help
```

## License

MIT

## Authors

Research project — See docs for scientific background.
