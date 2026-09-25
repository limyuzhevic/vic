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

### Phase 6 (Complete - Final Integration with Enhanced Cognitive Systems)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **Phase 4 cognitive systems now integrated with AgentBrain**
- Neural planner, concept formation, attention, self-model, social learning connected
- **Comprehensive Phase 4 integration tests created**
- Phase 6 integration experiment updated with cognitive system metrics

## Enhanced Cognitive Systems Integration

### AgentBrain Cognitive System Integration

Phase 4 cognitive systems have been successfully integrated into AgentBrain:

#### NeuralPlanner Integration
- **Connected to AgentBrain**: NeuralPlanner used by AgentBrain::decodeMotorCommand() for action planning
- **Goal-directed planning**: Plans multi-step action sequences based on current sensory state
- **Experience-based quality**: Action quality learned through experience and updated with outcomes

#### ConceptFormation Integration
- **Connected to AgentBrain**: ConceptFormation used by AgentBrain::processSensoryInput() to form concepts
- **Pattern discovery**: Discovers recurring patterns from sensory experience
- **Category emergence**: Categories emerge through repeated experience, not predefined

#### AttentionalSelection Integration
- **Connected to AgentBrain**: AttentionalSelection used for sensory filtering and prioritization
- **Competitive selection**: Competes between neural pathways for focus
- **Bottom-up and top-down**: Both novelty-driven and goal-driven attention mechanisms

#### Self-Model Integration
- **Connected to Agency**: SelfModel provides self-awareness and action consequence prediction
- **Forward model**: Predicts sensory consequences of actions
- **Self-other distinction**: Learns to distinguish self-generated vs external events

#### Social Learning Integration
- **Connected to Agency**: SocialLearning enables learning from observing other agents
- **Imitation capability**: Can imitate observed actions of other agents
- **Communication signals**: Learns simple communication from other agents

### Integration Methods Added

AgentBrain now includes:

- **Cognitive system pointers**: Access to NeuralPlanner, ConceptFormation, AttentionalSelection
- **Integration configuration**: Flags to enable/disable specific cognitive systems
- **Integration methods**: `integrateCognitiveSystems()` and `areCognitiveSystemsIntegrated()`
- **Enhanced processing**: Cognitive systems involved in sensory processing and action selection
- **Metrics collection**: Cognitive system performance tracked in experiments

### Phase 4 Integration Tests

Created comprehensive test suite:

- `test_phase4_integration.cpp` - Tests all Phase 4 systems integration
- `test_phase4_planner.cpp` - Tests NeuralPlanner functionality  
- `test_phase4_concept_formation.cpp` - Tests concept formation
- `test_phase4_attention.cpp` - Tests attentional selection
- `test_phase4_self_model.cpp` - Tests self-model functionality
- `test_phase4_social_learning.cpp` - Tests social learning

### Enhanced Phase 6 Integration Experiment

Phase 6 integration experiment now includes:

- **Cognitive system metrics**: Tracks planner confidence, concept count, attention selectivity
- **Integration verification**: Tests for self-model and social learning integration
- **Performance monitoring**: Cognitive system performance measured and reported

## Architecture Philosophy (Updated)

The NLM brain with Phase 4 integration operates as:

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
COGNITIVE SYSTEMS (planner, concepts, attention, self, social)
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION (neural decoding + cognitive guidance)
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT / COGNITIVE SYSTEMS
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
