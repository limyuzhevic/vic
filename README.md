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
│   └── visualization/ # Visualization
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
- **Real-world experience integration**: The Phase 3 experiment in `main.cpp` demonstrates complete world interaction with:
  - Sensorimotor loops (sensory → neural processing → action → world)
  - Neuromodulation (reward, novelty, curiosity)
  - Development over time
  - Structural plasticity (growing and pruning connections)
  - Learning through prediction errors and rewards
- **Implementation**: This is the real-world learning experiment integrated into the main executable, showing how neural systems interact with an environment

### Phase 4 (Complete)
- **Advanced Cognitive Capabilities**: Phase 4 experiments demonstrate sophisticated brain functions:
  - **Neural Prediction Systems**: The brain can predict future sensory input, detect patterns, and learn temporal sequences
  - **Working Memory**: The brain maintains and manipulates information over short time periods, tracking retention and capacity
  - **Episodic Memory**: The brain stores and retrieves specific experiences with detailed context (what, when, how)
  - **Concept Formation**: The brain abstracts and generalizes from experiences, recognizing patterns across variations
  - **Attentional Selection**: The brain prioritizes relevant information amid competition, showing selective attention
  - **Neural Planning**: The brain plans multi-step actions to achieve goals, evaluating different action sequences
  - **Self-Model Development**: The brain develops a model of its own body and actions, predicting consequences
  - **Social Learning**: The brain learns from observing others' actions and outcomes
  - **Continual Learning**: The brain can learn multiple tasks sequentially without catastrophic forgetting
  - **Generalization**: The brain applies learned concepts to novel situations
- **Integration**: These cognitive capabilities work together in the Phase 4 integrated experiment, showing how a complete brain system can process complex information and adapt behavior

### Phase 5 (Complete)
- **Performance Optimizations**: Phase 5 focuses on making the brain system more efficient and capable:
  - **Memory Pooling**: Optimized memory management for better performance
  - **Event Queues**: Efficient processing of neural events
  - **SIMD Vectorization**: Parallel processing for faster computation
  - **Parallel Processing**: Multiple operations can run simultaneously
  - **Checkpoint System**: Can save and resume brain states efficiently
  - **Scalability**: The system can handle larger networks and more complex simulations
  - **Lifetime Learning**: Extended learning over many phases (6 developmental phases)
  - **Damage Recovery**: Brain can recover from simulated damage and maintain function
  - **Multi-Agent Coordination**: Multiple brains can interact and learn from each other
  - **Ablation Studies**: Systematic removal of brain components to understand their function
- **Research Framework**: Phase 5 provides tools for scientific research into artificial brains, including:
  - Lifetime experiment tracking
  - Multi-seed experiments for statistical analysis
  - Comprehensive performance measurement
  - Visualization and reporting tools

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

## Current Execution

The system can be run using:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Phase 3 Integration Note

Phase 3 is now **integrated directly into the main executable** rather than being a separate demo. This provides the real learning experiments with proper synaptic changes. The main executable runs three tests:
1. Basic connectivity test
2. Plasticity learning experiment (demonstrates measurable synaptic changes)
3. STDP verification test

For the Phase 3 demo executable, see `nlm_phase3_demo` (built from `src/experiments/Phase3Demo.cpp`).

## License

MIT

## Authors

Research project — See docs for scientific background.
