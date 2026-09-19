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

**PHASE 6: PARTIALLY INTEGRATED - SYSTEMS CONNECTED**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system. **CURRENT STATUS: 35.8% INTEGRATION (43/120 achievable)**

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory: ⚠️ DEFINED but NOT CONNECTED to brain loop
- Episodic memory: ⚠️ DEFINED but NOT CONNECTED to brain loop  
- Semantic memory: ❌ EMPTY IMPLEMENTATION
- Procedural memory: ❌ EMPTY IMPLEMENTATION
- Associative memory: ❌ EMPTY IMPLEMENTATION
- **STATUS: 4/20 achievements (20%)**

#### Neuromodulation Integration
- Dopamine: ⚠️ IMPLEMENTED (scales STDP) but not fully integrated
- Novelty detection: ✅ WORKING (integrated with curiosity)
- Curiosity: ✅ WORKING (integrated with agent exploration)
- Serotonin: ❌ STUB (not integrated)
- Norepinephrine: ❌ STUB (not integrated)
- Acetylcholine: ❌ STUB (not integrated)
- **STATUS: 3/15 achievements (20%)**

#### Prediction System
- Next state prediction: ❌ IMPLEMENTED but NOT USED
- Prediction error: ❌ IMPLEMENTED but NOT USED
- Confidence: ❌ IMPLEMENTED but NOT USED
- History tracking: ❌ IMPLEMENTED but NOT USED
- **STATUS: 0/10 achievements (0%)**

#### Cognition Systems
- Neural planner: ❌ IMPLEMENTED but NOT CONNECTED
- Concept formation: ❌ IMPLEMENTED but NOT CONNECTED
- Self-model: ❌ IMPLEMENTED but NOT CONNECTED
- Social learning: ❌ IMPLEMENTED but NOT CONNECTED
- Spatial representation: ❌ IMPLEMENTED but NOT CONNECTED
- Attentional selection: ⚠️ PARTIAL (not connected to perception/action)
- **STATUS: 1/20 achievements (5%)**

#### Development Integration
- Synaptogenesis: ✅ WORKING (periodic)
- Pruning: ✅ WORKING (periodic)
- Maturation: ⚠️ IMPLEMENTED (limited to plasticity rates)
- Stage transitions: ⚠️ PARTIAL (only affects plasticity)
- **STATUS: 3/10 achievements (30%)**

#### Persistence
- Checkpoint save: ❌ NOT IMPLEMENTED (stub only)
- Checkpoint load: ❌ NOT IMPLEMENTED (stub only)
- **STATUS: 0/5 achievements (0%)**

#### Replay and Consolidation
- Episodic memory replay: ❌ NOT IMPLEMENTED
- Memory consolidation: ❌ NOT IMPLEMENTED
- **STATUS: 0/5 achievements (0%)**

### What's Actually Connected

The core neural systems are functional but most cognitive systems remain disconnected from the main brain loop:

- Neural dynamics: ✅ LIF neurons with spikes and synaptic transmission
- Basic plasticity: ✅ STDP, Hebbian, and structural plasticity
- Sensory-motor loop: ✅ Basic agent-world interaction via SimpleWorld
- Reward learning: ✅ Dopamine affects STDP scaling via eligibility traces
- Basic development: ⚠️ Affects structural plasticity rates

**Critical Integration Gaps:**
- Memory systems never update in brain loop
- Prediction system never processes sensory data
- Cognitive systems never instantiate or connect
- Neuromodulation limited to STDP scaling
- No experience storage or replay
- No goal-directed planning or concept formation

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

### Phase 6 (Partially Integrated - Systems Connected)
- Neural dynamics and basic plasticity working
- Sensory-motor loop implemented
- Reward-modulated learning operational
- Basic development stages affecting plasticity
- **CRITICAL GAP: 35.8% Integration - Most cognitive systems disconnected**

### What Needs Integration

**Priority 1: Connect Existing Systems**
1. Working memory - Update each step with firing neurons
2. Episodic memory - Store experiences and replay
3. Prediction system - Process sensory patterns
4. Neural planner - Connect to action selection
5. Concept formation - Process experiences for generalization

**Priority 2: Implement Missing Mechanisms**
1. Full neuromodulation (ACh, NE, 5-HT)
2. Memory consolidation and sleep cycle
3. Replay mechanisms
4. Checkpoint save/load integration
5. Development effects on memory and attention

**Priority 3: Validate Integration**
1. Test memory retention over time
2. Test prediction accuracy improvement
3. Test continual learning
4. Test developmental progression
5. Test multi-system interaction

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
