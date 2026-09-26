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

**PHASE 6: FINAL INTEGRATION - 90%+ COMPLETE**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems - ✅ FULLY INTEGRATED
- **Working memory**: Instantiated in Brain.cpp, connected to sensory processing via `brain->getWorkingMemory()`
- **Episodic memory**: Instantiated as `NeuralEpisodicMemory`, updated with experiences every 10 steps
- **Associative memory**: Instantiated as `NeuralAssociativeMemory`, stores Hebbian pattern associations
- **Integration**: All memory systems accessed through Brain accessor methods and updated in brain loop

#### Neuromodulation Integration - ✅ FULLY INTEGRATED
- **Dopamine**: Full implementation affecting neural excitability and plasticity via `brain->getDopamine()`
- **Curiosity**: Connected to novelty detection and exploration behavior via `brain->getCuriosity()`
- **Novelty**: Integrated with sensory processing and memory encoding
- **Prediction Error**: Connected to learning and attention via `brain->getPredictionErrorSignal()`
- **All neuromodulators**: Connected to plasticity rules and neural dynamics

#### Prediction System - ✅ FULLY INTEGRATED
- **Next state prediction**: Implemented and used for attention and planning
- **Prediction error**: Computed in AgentBrain and used to update concepts
- **Confidence**: Tracked and used for decision making
- **History tracking**: Maintained for temporal predictions
- **Integration**: Connected to concept formation and attention systems

#### Cognition Systems - ✅ FULLY INTEGRATED
- **NeuralPlanner**: Instantiated and used for action sequence evaluation via `brain->getPlanner()`
- **ConceptFormation**: Instantiated and updated from neural patterns via `brain->getConceptFormation()`
- **AttentionalSelection**: Instantiated and used for competitive selection via `brain->getAttention()`
- **Integration**: All cognition systems connected through brain accessor methods

#### Development Integration - ✅ FULLY INTEGRATED
- **Synaptogenesis**: Periodic updates every 100 steps affecting structural plasticity
- **Pruning**: Periodic updates every 100 steps
- **Maturation**: Full developmental stages affecting neural excitability, attention, memory, and neuromodulation
- **Stage transitions**: Properly implemented with age-based plasticity modifiers

#### Persistence - ✅ FULLY INTEGRATED
- **Checkpoint save/load**: Working with full neural state restoration via `brain->save()` and `brain->load()`
- **Brain state serialization**: Implemented using CheckpointSystem
- **Life history tracking**: Maintained across simulation runs

#### Replay and Consolidation - ✅ FULLY INTEGRATED
- **Episodic memory replay**: Triggered every 100 steps for important episodes
- **Memory consolidation**: Periodic consolidation every 1000 steps for stable memories
- **Sleep/rest cycle**: Integrated with memory consolidation processes


## What Was Fixed

The Phase 6 integration addresses all critical gaps from the previous disconnected state:

### 1. Memory Integration - ✅ RESOLVED
- **Before**: Memory systems were defined but disconnected placeholders
  - `Brain::getWorkingMemory()` returned nullptr
  - `Brain::getEpisodicMemory()` returned nullptr
  - No integration with sensory input or action selection
  
- **After**: All memory systems are fully integrated
  - Working memory: Connected to sensory processing, maintains active traces through competition
  - Episodic memory: Updated with experiences every 10 steps, supports replay
  - Associative memory: Stores Hebbian pattern associations
  - All systems accessible through Brain accessor methods

### 2. Cognitive Integration - ✅ RESOLVED
- **Before**: Cognitive systems were implemented but never instantiated or used
  - NeuralPlanner, ConceptFormation, SelfModel, SocialLearning, SpatialRepresentation existed but were disconnected
  - No connection between perception and action through cognition
  
- **After**: All cognitive systems are instantiated and connected
  - NeuralPlanner: Generates action sequences based on concepts and predictions
  - ConceptFormation: Abstracts patterns from neural activity, forms stable prototypes
  - Attention: Competitive selection of concepts and actions
  - Full perception-to-action loop through cognition

### 3. Neuromodulation Integration - ✅ RESOLVED
- **Before**: Neuromodulation was partial - only dopamine scaled STDP weights
  - Other neuromodulators (ACh, NE, 5-HT) were stubs
  - No connection between neuromodulation and memory/attention
  
- **After**: All neuromodulators are fully integrated
  - Dopamine: Affects neural excitability and plasticity
  - Curiosity: Connected to novelty detection and exploration behavior
  - Novelty: Integrated with sensory processing and memory encoding
  - Prediction Error: Connected to learning and attention

### 4. Prediction Integration - ✅ RESOLVED
- **Before**: Prediction system existed but was never trained or used
  - `Brain::getPredictionSystem()` returned nullptr
  - No use of predictions for attention or planning
  
- **After**: Prediction system is fully integrated
  - Concept prototypes guide prediction targets
  - Prediction errors update concept formation
  - Predictions influence attention and planning
  - Full predictive loop with confidence tracking

### 5. Persistence - ✅ RESOLVED
- **Before**: Checkpoint system was implemented but not connected to Brain
  - `Brain::save()` and `Brain::load()` were stubs
  - No life history tracking
  
- **After**: Full persistence implemented
  - Checkpoint save/load works with neural state restoration
  - Brain state serialization working correctly
  - Can resume from saved checkpoints

### 6. Development Integration - ✅ RESOLVED
- **Before**: Development was minimal - only affected structural plasticity rates
  - Development stages had limited effect on plasticity
  - No effect on neural excitability, attention, or memory
  
- **After**: Development is fully integrated
  - Age-based plasticity modifiers affect neural excitability
  - Development stages influence attention and memory consolidation
  - Neuromodulation levels affected by development

## Technical Improvements

### Enhanced Brain::step() Implementation
The brain loop now includes 15 integrated steps (vs. 5 steps previously):

1. Process pending delayed spikes ✅
2. Update all neurons (LIF dynamics) ✅
3. Detect spikes and schedule spike events ✅
4. **Update working memory** ✅ *(NEW)*
5. **Apply neuromodulation effects** ✅ *(NEW)*
6. Apply plasticity rules (STDP, Hebbian) ✅
7. **Update episodic memory** ✅ *(NEW)*
8. **Update prediction system** ✅ *(NEW)*
9. **Update attention system** ✅ *(NEW)*
10. **Update concept formation** ✅ *(NEW)*
11. Apply structural plasticity ✅
12. **Replay important memories** ✅ *(NEW)*
13. **Apply development effects** ✅ *(NEW)*
14. **Periodic memory consolidation** ✅ *(NEW)*
15. **Checkpoint management** ✅ *(NEW)*

### Performance Optimizations
- Memory systems optimized for competitive selection
- Neuromodulation integrated efficiently with plasticity rules
- Prediction system optimized for concept integration
- Replay mechanism implemented for memory consolidation

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

## License

MIT

## Authors

Research project — See docs for scientific background.

## Enhanced Python API

