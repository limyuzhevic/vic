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

**PHASE 6: FINAL INTEGRATION (PARTIALLY COMPLETE)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### ⚠️ Status: Significant Integration Gaps Remain

Based on the comprehensive Phase 6 audit (docs/PHASE6_FINAL_AUDIT.md), the NLM brain has sophisticated **anatomy** but lacks the **integration** that makes it function as a coherent artificial brain.

**Current Integration Score: 35.8% (43/120)**

### ✅ What Has Been Implemented (Core Neural Machinery)

#### 1. Neural Core Systems - FULLY WORKING
- **LIF Neuron Dynamics**: Real Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Synaptic Transmission**: Excitatory/inhibitory connections with delays and plasticity
- **Spike Processing**: Event-driven spike system with precise timing
- **Plasticity Rules**: STDP and Hebbian learning applied each simulation step
- **Structural Plasticity**: Synaptogenesis and pruning every 100 steps
- **Neuromodulation**: Dopamine affects plasticity rates

#### 2. Agent-World Interface - PARTIALLY INTEGRATED
- **Sensory Processing**: Vision, touch, internal, and proprioception input injection
- **Motor Decoding**: Neural activity to motor commands conversion
- **Reward Modulation**: Prediction error signals applied to synapses
- **Curiosity/Exploration**: Novelty-driven exploration behavior
- **Development**: Age-based plasticity changes

### ❌ What's Missing: Critical Integrations

#### 3. Memory Systems - DISCONNECTED PLACEHOLDERS
- **Working Memory**: NeuralWorkingMemory class exists but returns nullptr
- **Episodic Memory**: NeuralEpisodicMemory class exists but never updated
- **Semantic/Procedural Memory**: Empty stub implementations
- **Associative Memory**: Stub implementation

#### 4. Prediction System - DISCONNECTED
- **Temporal Prediction**: NeuralPrediction class exists but never used
- **Action-Consequence**: ActionConsequencePredictor stub
- **Prediction Error**: Computed but not integrated with memory

#### 5. Cognition Systems - DISCONNECTED
- **Neural Planner**: NeuralPlanner class exists but never instantiated
- **Concept Formation**: ConceptFormation class exists but never used
- **Attention**: AttentionalSelection class exists but disconnected
- **Self-Model**: Forward model for body schema (never used)
- **Social Learning**: Observation capabilities (never used)

#### 6. Development Beyond Plasticity - MINIMAL
- Synaptogenesis/pruning rates change with age
- But NO effects on:
  - Neural excitability
  - Attention modulation
  - Memory consolidation
  - Neuromodulation levels

#### 7. Checkpoint System - STUBS
- CheckpointSystem class fully implemented
- But Brain::save() and Brain::load() are empty stubs

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Note**: The build system compiles all components, but many cognition and memory systems will not function due to integration gaps.

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

## License

MIT

## Authors

Research project — See docs for scientific background.
