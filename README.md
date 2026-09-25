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

## Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo --phase6 [OPTIONS]
```

This runs a comprehensive Phase 6 integration test verifying all brain systems are connected.

**New Phase 6 Options:**
- `--phase6`: Run complete integrated brain demo
- `--agent`: Run Phase 6 with agent integration
- `--steps N`: Number of simulation steps (default: 1000)
- `--neurons N`: Number of neurons (default: 1000)
- `--regions N`: Number of brain regions (default: 1)

**Examples:**
```bash
# Run basic Phase 6 integration demo
./nlm_phase6_demo --phase6

# Run Phase 6 with agent integration
./nlm_phase6_demo --phase6 --agent

# Run with custom parameters
./nlm_phase6_demo --phase6 --steps 5000 --neurons 2000

# Run original Phase 2 test (backward compatibility)
./nlm_phase6_demo --phase2
```

## New Phase 6 Integration Demo

The updated `main.cpp` now includes comprehensive Phase 6 integration tests that demonstrate:

### Phase 6: Complete Integrated Brain

The Phase 6 integration demo showcases the fully integrated artificial brain with all systems working together:

1. **Memory Systems**: 
   - NeuralWorkingMemory with persistent activity
   - NeuralEpisodicMemory with experience storage and replay
   - Associative memory with pattern associations

2. **Neuromodulation**: 
   - Dopamine affecting neural excitability and plasticity
   - Curiosity driving exploration behavior
   - Novelty detection integrated with sensory processing

3. **Prediction System**: 
   - Prediction system integrated into brain loop
   - Prediction error signals affect learning
   - Confidence tracking implemented

4. **Development System**: 
   - Developmental stages affecting plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

5. **Performance**: 
   - Performance optimizations with memory pools and event queues
   - SIMD vectorization and parallel processing
   - Checkpoint system for persistence

### Agent Integration (NEW!)

New to Phase 6 is the agent integration demo that shows:

- **AgentBrain**: Connects NLM brain to world environment
- **SimpleWorld**: Provides sensory experience without semantic labels
- **Full Life Cycle**: Sense → Process → Act → Learn → Repeat

### Command-Line Interface (NEW!)

The main executable now supports multiple modes:

| Mode | Command | Description |
|------|---------|-------------|
| Phase 2 | `--phase2` | Basic neural computation test (original) |
| Phase 6 | `--phase6` | Complete integrated brain demo |
| Agent | `--agent` | Phase 6 with agent integration |

**Help:**
```bash
./nlm_phase6_demo --help
```

### Usage Examples

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo --phase6

# Run Phase 6 with agent integration
./nlm_phase6_demo --phase6 --agent

# Run Phase 6 with custom parameters
./nlm_phase6_demo --phase6 --steps 2000 --neurons 1500

# Run original Phase 2 demo (backward compatibility)
./nlm_phase6_demo --phase2
```

### Performance Benchmarking (NEW!)

Phase 6 demo includes performance benchmarking:
```bash
# Phase 6 demo automatically includes benchmark results
# Shows steps/sec, time per step, and system statistics
```

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
./nlm_phase6_demo --phase6
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 6 with Agent Integration (NEW!)

```bash
./nlm_phase6_demo --phase6 --agent
```

This runs the complete agent demo showing:
- Sensory processing through neural encoding
- Action selection via motor neuron activity  
- Reward-based learning through neuromodulation
- Development and changing plasticity
- Curiosity-driven exploration

### Original Phase 2 Demo (Backward Compatibility)

```bash
./nlm_phase6_demo --phase2
```

This runs the original Phase 2 tests for backward compatibility.

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
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── agent/         # Agent system
│   ├── world/         # World simulation
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
├── tests/
├── configs/
└── python/           # Python bindings
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
- Neuromodulation affects plasticity and neural dynamics
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

## New Phase 6 Integration Features

Phase 6 demonstrates a sophisticated brain architecture with:

### 1. Comprehensive Memory Integration
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience storage with replay capability
- **Associative Memory**: Pattern-based relationships between experiences
- **All memory systems**: Directly connected to neural processing loops

### 2. Advanced Neuromodulation
- **Dopamine**: Modulates neural excitability and synaptic plasticity
- **Curiosity**: Drives exploration and reward prediction error computation
- **Novelty**: Detects changes in sensory input for learning
- **Predictive Coding**: Integration with prediction error signals

### 3. Development System
- **Plasticity modulation**: Developmental stages affect learning rates
- **Structural changes**: Synaptogenesis and pruning with age
- **Experience-dependent**: Neural connectivity changes with use
- **Maturation process**: Progressive stabilization of neural circuits

### 4. Prediction and Cognition
- **Forward models**: Internal predictions of sensory consequences
- **Planning**: Action sequence evaluation based on predictions
- **Concept formation**: Abstraction from experience patterns
- **Attention**: Competitive selection of neural populations

### 5. Persistence and Replay
- **Checkpointing**: Save/load brain state for long-term simulations
- **Memory replay**: Consolidate important experiences during rest
- **Sleep cycles**: Offline processing and memory strengthening
- **Experience replay**: Reactivation of neural patterns for consolidation

## Performance Optimizations

Phase 6 includes advanced optimizations:

### 1. Memory Management
- **Memory pools**: Efficient allocation/deallocation of neurons and synapses
- **Event queues**: Optimized spike event processing
- **Sparse connectivity**: Reduced computational overhead for large networks

### 2. Parallel Processing
- **SIMD vectorization**: Parallel processing of neural computations
- **Multi-threading**: Scalable performance on multi-core systems
- **Asynchronous operations**: Non-blocking I/O and event handling

### 3. Event-Driven Dynamics
- **Real-time simulation**: Precise timing of neural events
- **Spike propagation**: Fast synaptic transmission with delays
- **Plasticity triggers**: Activity-dependent synaptic modification

## Scientific Contributions

Phase 6 advances computational neuroscience by demonstrating:

1. **Integrated Memory-Prediction Loop**: Shows how memory systems can work with prediction to guide behavior
2. **Neuromodulatory Coordination**: Demonstrates how multiple neuromodulators can coordinate learning
3. **Developmental Trajectories**: Shows how neural development affects learning capabilities
4. **Experience-Dependent Plasticity**: Demonstrates how experience shapes brain structure and function

The Phase 6 architecture provides a foundation for investigating whether complex cognitive abilities can emerge from the interaction of biologically-inspired neural components working together in a unified system.

## License

MIT

## Authors

Research project — See docs for scientific background.
