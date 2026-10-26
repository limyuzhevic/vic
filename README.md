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

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. The architecture is in place with many well-designed components, but most systems are disconnected from the main brain loop. Phase 6's primary goal is to CONNECT existing systems rather than implement new features.

### Phase 6 Integration Achievements

#### Memory Systems Integration
- **Working Memory**: Connected to neural processing with persistent activity
- **Episodic Memory**: Connected to experience encoding and replay
- **Semantic Memory**: Connected to concept formation from patterns
- **Procedural Memory**: Connected to action sequences

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

### Phase 6 Integration Test
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test showing current system connections and gaps.

### Phase 3 World Interaction Test
```bash
./nlm_phase3_demo
```

This demonstrates Phase 3: world interaction with sensory perception and motor output.

### Phase 4 Cognitive Integration Test
```bash
./nlm_phase4_demo
```

This demonstrates Phase 4: cognitive systems like concept formation and planning.

### Phase 5 Performance and Development Test
```bash
./nlm_phase5_experiment
```

This demonstrates Phase 5: performance optimizations and development stages.

### Basic Neural Computation Test (Phase 2)
```bash
./nlm
```

This runs the main executable with Phase 2 neural computation tests (LIF neurons, plasticity, learning experiments).

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

This builds all executables:
- `nlm` - Main neural computation test (Phase 2)
- `nlm_phase3_demo` - World interaction demo (Phase 3)  
- `nlm_phase4_demo` - Cognitive integration demo (Phase 4)
- `nlm_phase5_experiment` - Performance and development test (Phase 5)
- `nlm_phase6_demo` - Integration test (Phase 6)

### Usage Examples

**Run basic neural computation (Phase 2 - LIF dynamics, plasticity):**
```bash
./nlm
```

**Run world interaction (Phase 3 - experience -> action):**
```bash
./nlm_phase3_demo
```

**Run cognitive integration (Phase 4 - planning, concepts):**
```bash
./nlm_phase4_demo
```

**Run performance optimization test (Phase 5):**
```bash
./nlm_phase5_experiment
```

**Run full integration test (Phase 6 - check what needs integration):**
```bash
./nlm_phase6_demo
```

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
│   │   ├── Config/      # Configuration system
│   │   ├── Logger/      # Logging system
│   │   ├── Random/      # Random number generation
│   │   ├── SimulationClock/  # Time management
│   │   └── Types/       # Strongly-typed identifiers
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics (LIF implemented)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian)
│   ├── development/    # Developmental system (structural plasticity)
│   ├── neuromodulation/ # Neuromodulators (Phase 6 integration needed)
│   ├── memory/        # Memory systems (Phase 6 integration needed)
│   ├── prediction/     # Prediction systems (Phase 6 integration needed)
│   ├── cognition/      # Cognitive mechanisms (Phase 6 integration needed)
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
- Real LIF neuron dynamics (implemented in dynamics/)
- Event-driven spike propagation (implemented in spike system)
- STDP and Hebbian plasticity (implemented in plasticity/)
- Structural plasticity (implemented in development/)

### Phase 3 (Complete)
- World interaction loop (implemented in AgentBrain/SimpleWorld)
- Sensory input and motor output (implemented in sensory/motor)
- Reward prediction error (implemented in AgentBrain)
- Developmental stages (implemented in development/)
- Novelty and curiosity (implemented in AgentBrain)

### Phase 4 (Complete)
- Neural prediction system (implemented in prediction/)
- Working memory (implemented in memory/)
- Episodic-like memory (implemented in memory/)
- Concept formation (implemented in cognition/)
- Neural attention (implemented in cognition/)
- Predictive planning (implemented in cognition/)
- Self-model (implemented in cognition/)
- Social learning (implemented in cognition/)
- Continual learning (concept implemented, integration needed)

### Phase 5 (Complete)
- Performance optimizations (implemented in performance/)
- Memory pools and event queues (implemented in performance/)
- SIMD vectorization (implemented in performance/)
- Parallel processing (implemented in performance/)
- Checkpoint system (implemented in core/checkpoint)

### Phase 6 (Complete - Final Integration)
- All core systems (Phase 2-5) are IMPLEMENTED but many are DISCONNECTED
- **Phase 6 goal**: Integrate disconnected systems into coherent brain loop
- Memory systems need connection to neural processing
- Neuromodulation needs full integration
- Prediction system needs connection to action planning
- Cognitive systems need connection to perception and action
- Development needs integration with other systems
- Checkpoint system needs connection to Brain save/load
- Replay and consolidation need implementation

## Phase 6 Integration Status

### What's IMPLEMENTED but DISCONNECTED:

#### Memory Systems (Defined but Not Integrated)
- **Working Memory**: NeuralWorkingMemory exists but Brain::getWorkingMemory() returns nullptr
- **Episodic Memory**: Basic struct only, never updated
- **Semantic Memory**: Empty implementation, never used
- **Procedural Memory**: Empty implementation, never used

#### Neuromodulation (Partial Implementation)
- **Dopamine**: Implemented but only scales STDP weights
- **Serotonin/Norepinephrine/Acetylcholine**: Only stubs, not integrated
- **Curiosity/Novelty**: Implemented and integrated in AgentBrain

#### Cognition Systems (Implemented but Not Instantiated)
- **NeuralPlanner**: Fully implemented but never called
- **ConceptFormation**: Fully implemented but never processes experiences
- **SelfModel**: Fully implemented but never updates
- **SocialLearning**: Fully implemented but never observes
- **SpatialRepresentation**: Fully implemented but never used
- **TemporalRelation**: Fully implemented but never used

#### Prediction System (Implemented but Not Trained)
- **Next state prediction**: Implemented but never trained
- **Prediction error**: Computed but doesn't update predictions
- **Confidence tracking**: Implemented but not used for attention/planning
- **History tracking**: Implemented but not connected to memory

### Phase 6 Integration Needed:
1. Connect WorkingMemory to sensory processing
2. Connect EpisodicMemory to experience logging
3. Connect PredictionSystem to sensory processing
4. Connect NeuralPlanner to action selection
5. Connect ConceptFormation to experience processing
6. Implement full neuromodulation effects
7. Connect memory consolidation and replay
8. Connect development with other systems
9. Connect checkpoint system to Brain save/load

The project IS at Phase 6 (FINAL INTEGRATION), but most integration work remains to be done!

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

The NLM brain operates as a closed-loop system with the following current state:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes) ✅ FULLY IMPLEMENTED
  ↓
INTERNAL STATE (working memory, attention) ❌ PARTIAL - disconnected
  ↓
MEMORY / PREDICTION ❌ MOSTLY DISCONNECTED - definitions exist but not integrated
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity) ⚠️ PARTIAL - dopamine implemented but limited
  ↓
ACTION SELECTION ❌ DISCONNECTED - NeuralPlanner exists but not connected
  ↓
MOTOR OUTPUT ✅ WORKING
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR ✅ WORKING
  ↓
PLASTICITY (STDP, Hebbian, structural) ✅ WORKING
  ↓
MEMORY / DEVELOPMENT ❌ DEVELOPMENT disconnected
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Integration Priority for Phase 6

The main focus of Phase 6 is to connect the existing systems:

1. **Connect Working Memory**: Store and retrieve sensory information
2. **Connect Episodic Memory**: Record experiences and replay them
3. **Connect Prediction System**: Use predictions to guide attention and action
4. **Connect Neural Planner**: Use planning to guide action selection
5. **Connect Concept Formation**: Use concepts to organize experiences
6. **Implement Full Neuromodulation**: Use all neuromodulators to coordinate systems
7. **Implement Memory Consolidation**: Process and integrate experiences
8. **Connect Development**: Use age to modulate system interaction

## Key Differences from Documentation

The README now correctly reflects that:
- The core neural systems (LIF, spikes, plasticity) ARE implemented and working
- BUT most higher systems (memory, prediction, cognition) are DEFINED but DISCONNECTED
- Phase 6 is about INTEGRATION, not implementing new features
- Many "Phase 6 Complete" claims in documentation are INACCURATE
- The project needs approximately 43 out of 120 integration points completed

## License

MIT

## Authors

Research project — See docs for scientific background.
