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

│   ├── agent/           # Agent systems (brain-body interface)
│   │   ├── AgentBrain/     # Sensory processing, motor decoding
│   │   ├── SensoryPercept/ # Sensory data packets
│   │   └── AgentBody/      # Physical agent properties
│   ├── brain/          # Neural components (working)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems (partially disconnected)
│   ├── prediction/     # Prediction systems (partially disconnected)
│   ├── cognition/      # Cognitive mechanisms (implemented but disconnected)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── world/         # Environment interface
│   ├── environment/   # Abstract environment
│   ├── experiments/   # Experiment framework
│   └── visualization/  # Visualization

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real membrane potential dynamics
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
- Episodic memory
- Concept formation
- Neural attention
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

### Phase 6 (PLANNING - NOT COMPLETE)
- **Status**: Integration planned, but not yet achieved
- **Focus**: Connecting existing disconnected systems
- **Key Challenges**: 
  - NeuralWorkingMemory not integrated into brain loop
  - EpisodicMemory never updated with experiences
  - Prediction system not integrated
  - Cognitive systems disconnected from action selection
  - Reward modulation doesn't integrate with memory systems

## Implementation Status

### ✅ WORKING & INTEGRATED:
- LIF neuron dynamics with event-driven spikes
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Sensory processing (vision, touch, internal, proprioception)
- Motor decoding and action execution
- Reward prediction error computation
- Novelty and curiosity mechanisms
- Developmental stages and plasticity modulation
- Basic neuromodulation (dopamine-like signals)
- Eligibility traces for reward-modulated learning
- Agent/Body system with energy and state management
- Phase 3 experiment demo working

### ⚠️ PARTIALLY IMPLEMENTED/CONNECTED:
- NeuralWorkingMemory: Implemented but not integrated into brain loop
- NeuralEpisodicMemory: Exists but not used
- PredictionSystem: Basic prediction implemented but not integrated
- NeuralPlanner: Implemented but not used
- ConceptFormation: Partially implemented
- SocialLearning: Implemented but not integrated

### ❌ MISSING/PLACEHOLDER:
- Brain::save() and Brain::load() are stubs
- NeuralWorkingMemory not integrated into brain loop
- EpisodicMemory never updated with experiences
- Prediction system never trained with real data
- NeuralPlanner never used for action selection
- Concept formation never processes experiences

## Integration Test Availability

### Phase 6 Integration Components
Phase 6 will focus on connecting:

1. **Memory Systems**: Working memory for active maintenance
2. **Prediction Systems**: Temporal sequence learning
3. **Cognition Systems**: Planning, attention, concept formation
4. **Neuromodulation**: Reward, curiosity, novelty signals
5. **Development**: Stage-based plasticity modulation

### Current Status
- **Phase 3**: Working and functional
- **Phase 4**: Implemented but disconnected
- **Phase 5**: Performance infrastructure in progress
- **Phase 6**: Integration planned (connect existing systems)

## Anti-Transformer Principle

Throughout all phases, NLM must NEVER become:
- A transformer architecture
- An LLM
- A pretrained model
- A conventional deep learning system
- "LLM + memory"
- "LLM + tools"

The architecture must always be based on:
- Individual neurons
- Synapses
- Spike-based communication
- Neural plasticity
- Developmental processes

If at any point the architecture begins to resemble a transformer, it should be redirected or the project renamed to clarify it is no longer NLM.

## Running Examples

### Phase 3 Demo (Working)
```bash
# Basic world interaction demo
./nlm_phase3_demo [num_trials] [max_steps]
```

### Phase 4 Demo (Disconnected Components)
```bash
# Cognitive experiments - note: systems are implemented but disconnected
./nlm_phase4_demo [num_trials]
```

## Scientific Background

For detailed scientific background, refer to:
- `docs/SCIENCE.md` - Biological motivation and computational implementation
- `docs/LIMITATIONS.md` - What NLM is not and current limitations
- `docs/PHASE6_FINAL_AUDIT.md` - Detailed system integration analysis
- `docs/EXPERIMENTS.md` - Experimental framework and test procedures

## License

MIT

## Authors

Research project — See docs for scientific background.

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
