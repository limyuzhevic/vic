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

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Current Phase

**PHASE 6: FINAL INTEGRATION (IN PROGRESS)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Status (from audit): ~36% complete

#### ✅ Working Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay  
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### ⚠️ Partially Working
- Neuromodulation affects neural excitability and plasticity
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented
- All cognition systems connected to perception and action
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- Checkpoint save/load partially implemented
- Brain state serialization working
- Can resume from saved checkpoints
- Episodic memory replay during simulation

#### ❌ Not Working
- **Dopamine system disconnected** (brain->getDopamine() returns nullptr)
- **Curiosity system disconnected** (brain->getCuriosity() returns nullptr)
- **Novelty system disconnected** (brain->getNovelty() returns nullptr)
- **Prediction system disconnected** (brain->getPredictionSystem() returns nullptr)
- **Working Memory disconnected** (brain->getWorkingMemory() returns nullptr)
- **Episodic Memory disconnected** (brain->getEpisodicMemory() returns nullptr)
- **Semantic Memory disconnected** (brain->getSemanticMemory() returns nullptr)
- **Procedural Memory disconnected** (brain->getProceduralMemory() returns nullptr)
- **Associative Memory disconnected** (brain->getAssociativeMemory() returns nullptr)
- **Planner disconnected** (brain->getPlanner() returns nullptr)
- **Concept Formation disconnected** (brain->getConceptFormation() returns nullptr)
- **Attention disconnected** (brain->getAttention() returns nullptr)
- **Development System disconnected** (brain->getDevelopmentSystem() returns nullptr)

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

This demonstrates basic LIF neuron dynamics, spike propagation, and plasticity rules.

### Phase 3 Demo
```bash
./nlm_phase3_demo
```

This demonstrates world interaction, sensory processing, and reward systems.

### Phase 4 Demo  
```bash
./nlm_phase4_demo
```

This demonstrates integrated prediction, memory, cognition, and social learning.

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying which systems are connected and functional.

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

### Phase 6 (IN PROGRESS - ~36% Complete)
- Core neural computation and memory systems working
- Many cognition and neuromodulation systems disconnected
- Integration verification shows which systems need work

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

Phase 6 investigates:
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

## Project Improvements

The NLM project actively seeks community contributions to improve the integration of brain systems:

### Current Focus Areas

1. **Neuromodulation Integration**: Implement disconnected dopamine, curiosity, and novelty systems
2. **Prediction System**: Connect prediction system to neural processing
3. **Cognition Systems**: Implement disconnected planner, concept formation, and attention
4. **Memory Integration**: Ensure all memory systems are properly connected
5. **Development Integration**: Complete development system implementation

### For Users

The Python bindings provide access to all NLM brain systems:

```python
import pynlm

# Create brain with configuration
brain = pynlm.Brain(pynlm.Config())
brain.initialize()

# Access integrated systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
prediction_system = brain.getPredictionSystem()

# Run simulation
brain.step(0, 0.0)
```

For full documentation, see the `/docs/` directory.

