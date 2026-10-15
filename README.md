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

**PHASE 6: FINAL INTEGRATION (IN PROGRESS)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements (Partial Integration)

The NLM system contains sophisticated components that need full integration:

#### Memory Systems (Needing Integration)
- Working memory with persistent activity and competition - ✅ IMPLEMENTED
- Episodic memory with experience encoding and replay - ✅ IMPLEMENTED  
- Associative memory with Hebbian pattern associations - ✅ IMPLEMENTED
- **ISSUE**: All memory systems are disconnected from main brain loop

#### Neuromodulation Integration (Partially Working)
- Dopamine affects neural excitability and plasticity - ⚠️ PARTIAL (Phase 2 TODOs)
- Curiosity drives exploration behavior - ✅ WORKING
- Novelty detection integrated with sensory processing - ✅ WORKING
- All neuromodulators connected to plasticity rules - ⚠️ PARTIAL

#### Prediction System (Disconnected)
- Prediction system integrated into brain loop - ✅ IMPLEMENTED BUT NOT USED
- **ISSUE**: Returns nullptr in Brain class

#### Cognition Systems (Partially Connected)
- Neural planner with action sequence evaluation - ✅ IMPLEMENTED BUT NOT USED
- Concept formation from experience patterns - ✅ IMPLEMENTED BUT NOT USED
- Attention with competitive selection dynamics - ✅ IMPLEMENTED BUT NOT FULLY USED

#### Development Integration (Basic)
- Developmental stages affect plasticity rates - ✅ IMPLEMENTED
- Structural plasticity modulated by age - ⚠️ PARTIAL
- Neural excitability changes with development - ✅ IMPLEMENTED

#### Persistence (Implemented)
- Checkpoint save/load implemented - ✅ WORKING
- Brain state serialization working - ✅ WORKING
- Can resume from saved checkpoints - ✅ WORKING

#### Replay and Consolidation (Implemented)
- Episodic memory replay during simulation - ✅ WORKING
- Memory consolidation for important episodes - ✅ WORKING
- Integration with sleep/rest cycle - ✅ IMPLEMENTED

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test - **NEEDS IMPROVEMENT**)
```bash
./nlm_phase6_demo
```

**CURRENT STATUS**: The Phase 6 demo exists but integration is incomplete. Many cognitive systems are disconnected from the main brain loop.

### Improved Phase 6 Demo (New Implementation)
The main.cpp file has been updated to show a more comprehensive Phase 6 integration demo with test suites for:
1. Memory systems integration
2. Neuromodulation integration
3. Development integration
4. Checkpoint and persistence
5. Replay and consolidation

```bash
./nlm_phase6_demo_improved
```

*(Note: This demo is available as main.cpp but renamed for clarity)*

### Phase 2 Demo (Basic Neural Computation)
```bash
./nlm_phase2_demo
```

This runs the basic neural computation demo with working memory, episodic memory, and integrated neuromodulation.

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
