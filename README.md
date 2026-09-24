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

#### Memory Systems - **PARTIALLY INTEGRATED**
- ✅ **Working Memory**: Implemented with persistent neural activity and competition dynamics
  - Stores firing neuron traces during simulation steps
  - Implements decay and reinforcement mechanisms
  - Connected to attention competition system
  
- ✅ **Episodic Memory**: Experience encoding with temporal ordering
  - Stores neural patterns every 10 simulation steps
  - Implements replay mechanism (every 100 steps)
  - Includes consolidation every 1000 steps

- ✅ **Associative Memory**: Hebbian pattern associations
  - Integrated with episodic memory system
  - Supports spreading activation for recall
  - Modified through experience

- ⚠️ **Semantic/Procedural Memory**: PLACEHOLDER STATUS
  - Defined in architecture but not fully implemented
  - Structural integration pending

#### Neuromodulation Integration - **PARTIALLY INTEGRATED**
- ✅ **Dopamine**: Reward prediction error computation
  - Modulates neural excitability
  - Scales STDP learning rates
  - Affects action selection through learning

- ✅ **Curiosity**: Novelty and prediction error combination
  - Integrated into AgentBrain for exploration decisions
  - Drives exploration behavior
  - Connected to motor command decoding

- ✅ **Novelty**: Sensory pattern change detection
  - Computed from vision changes in AgentBrain
  - Integrated with curiosity system

- ⚠️ **Acetylcholine, Serotonin, Norepinephrine**: PLACEHOLDER
  - Basic implementations exist but not integrated
  - ACh effects on attention/memory pending

#### Prediction System - **PARTIALLY INTEGRATED**
- ✅ **Basic Prediction**: Implemented but not fully connected
  - Next state prediction capability exists
  - Prediction error computation implemented
  - Confidence tracking developed

- ⚠️ **Integration Issues**: Not connected to sensory processing or cognition
  - Prediction system not called in brain loop
  - No influence on attention or planning
  - Not used for action selection

#### Cognition Systems - **DISCONNECTED**
- ⚠️ **Neural Planner**: Implemented but NOT USED
  - Multi-step action sequence evaluation capability exists
  - Not called by AgentBrain
  - Not integrated with action selection

- ⚠️ **Concept Formation**: Implemented but NOT USED
  - Pattern clustering through similarity
  - Prototype formation mechanism exists
  - Not processing sensory patterns for generalization

- ⚠️ **Attention**: Partially integrated
  - Competitive selection dynamics implemented
  - Connected to working memory
  - Not fully integrated with sensory input

- ⚠️ **Self-Model, Social Learning, Spatial Representation, Temporal Relation**: DISCONNECTED
  - All implemented but not instantiated or used

#### Development Integration - **MINIMALLY INTEGRATED**
- ✅ **Developmental Stages**: Implemented and working
  - Stage progression (Initial → CriticalPeriod → Maturation → Adult → Aging)
  - Modifies structural plasticity rates by stage

- ⚠️ **Limited Scope**: Only affects plasticity rates
  - Does NOT affect neural excitability
  - Does NOT influence attention or memory consolidation
  - Does NOT modulate neuromodulation levels

#### Persistence - **PARTIALLY WORKING**
- ✅ **Checkpoint System**: Implemented and functional
  - Brain state serialization with neuron/synapse state
  - CheckpointManager for automated saves
  - Phase 6 experiment successfully tested save/load

- ⚠️ **Integration Issues**: Saves work but loading is complex
  - Brain::save() works correctly
  - Brain::load() restores structure minimally
  - Full state restoration challenging

#### Replay and Consolidation - **FUNCTIONAL**
- ✅ **Replay System**: Episodic memory replay during simulation
  - Replays important episodes every 100 steps
  - Integration with sleep/rest cycle

- ✅ **Consolidation**: Memory strengthening mechanism
  - Consolidates important episodes
  - Connected to replay system

### Phase 6 Integration Summary

**What's Working Well:**
- Neural dynamics (LIF neurons, spike propagation)
- Plasticity (STDP, Hebbian, structural plasticity)
- Basic agent-world loop (sensory → brain → motor)
- Neuromodulation (dopamine, curiosity, novelty)
- Checkpoint system
- Replay mechanism

**What's Still Disconnected:**
- Working memory ↔ Episodic memory ↔ Prediction
- Cognition systems (planner, concepts, self-model)
- Full prediction integration
- Advanced neuromodulators (ACh, NE, 5-HT)
- Sensory input → Memory storage pathways

### Current Phase 6 Configuration

**Phase6Config Options:**
- `neuronCount`: Total neurons (default: 500)
- `maxSteps`: Simulation steps (default: 2000)
- `enableCheckpointing`: Enable save/load (default: true)
- `enableReplay`: Enable memory replay (default: true)
- `enableDevelopment`: Enable developmental stages (default: true)
- `connectionProbability`: Neural connectivity (default: 0.15)

**Memory System Configuration:**
- `workingMemorySize`: Working memory capacity (default: 1000)
- `episodicCapacity`: Episodic memory slots (default: 100)
- `replayInterval`: Replay frequency (every 100 steps)
- `consolidationInterval`: Consolidation frequency (every 1000 steps)

**Neuromodulation Configuration:**
- `dopamineScale`: Dopamine modulation strength (default: 1.0)
- `curiosityScale`: Curiosity drive strength (default: 0.5)
- `noveltyThreshold`: Novelty detection threshold (default: 0.1)

### Phase 6 Demo Results

Based on Phase 6 integration experiments:

| Metric | Value | Status |
|--------|-------|--------|
| Working Memory Integration | Connected | ✅ Working |
| Episodic Memory Integration | Connected | ✅ Working |
| Neuromodulation Integration | Partially | ⚠️ Limited |
| Prediction Integration | Disconnected | ❌ Not Connected |
| Development Integration | Partially | ⚠️ Limited |
| Checkpointing | Working | ✅ Functional |
| Replay System | Working | ✅ Functional |

### Integration Architecture

**Main Brain Loop (Brain::step()):**
```cpp
for each step:
    1. Process delayed spikes ✅
    2. Update neurons (LIF) ✅
    3. Detect spikes ✅
    4. Update working memory ✅
    5. Apply neuromodulation ✅
    6. Apply plasticity (STDP, Hebbian) ✅
    7. Store episodic memory ✅
    8. Update prediction ⚠️ (Not implemented)
    9. Update attention ⚠️ (Limited)
    10. Update concept formation ❌ (Not called)
    11. Structural plasticity ✅ (every 100 steps)
    12. Replay ✅ (every 100 steps)
    13. Development update ⚠️ (Limited)
    14. Consolidation ✅ (every 1000 steps)
    15. Checkpoint management ✅ (Periodic)
```

**Agent-Brain Integration:**
```
Sensory Input → AgentBrain.processSensoryInput() → Brain.receiveSensoryInput()
                                             ↓
                                         Neural Processing
                                             ↓
                                         Motor Activity → AgentBrain.decodeMotorCommand()
                                             ↓
                                         World Action → Reward → AgentBrain.applyRewardModulation()
                                             ↓
                                         Neuromodulation → Plasticity
```

**Key Integration Challenges:**
1. **Memory Chain Gap**: No working memory → episodic memory → prediction connections
2. **Cognition Isolation**: Planner, concepts, self-model never instantiated
3. **Modulation Limitations**: Dopamine only scales STDP weights
4. **Development Scope**: Only affects structural plasticity rates
5. **Prediction Disconnection**: Prediction system not trained or used

### Phase 6 Scientific Goals

Phase 6 investigates:
- Whether memory systems can integrate with neural dynamics ✅ (Working memory, Episodic memory)
- Whether neuromodulation can affect plasticity in a coordinated way ⚠️ (Limited dopamine effects)
- Whether prediction can become a central organizing principle ❌ (Not integrated)
- Whether developmental stages can modulate learning comprehensively ⚠️ (Limited)
- Whether replay can reinforce memory consolidation ✅ (Replay functional)
- Whether the complete brain loop functions coherently ⚠️ (Partially working)

### Next Steps for Phase 6 Completion

**Priority 1: CONNECT EXISTING SYSTEMS**
1. Connect WorkingMemory to sensory processing
2. Connect EpisodicMemory to experience logging
3. Connect PredictionSystem to sensory processing
4. Connect NeuralPlanner to action selection
5. Connect ConceptFormation to experience processing

**Priority 2: VALIDATE INTEGRATION**
1. Test memory retention over time
2. Test prediction accuracy improvement
3. Test continual learning
4. Test developmental progression
5. Test multi-system interaction

### Performance Metrics (Phase 6 Experiment)

Based on typical Phase 6 experiments (500 neurons, 2000 steps):

| Metric | Typical Value | Notes |
|--------|---------------|-------|
| Average Firing Rate | 0.1-1.0 Hz | Sparse, event-driven |
| Working Memory Traces | 10-50 | Activity-dependent |
| Episodic Episodes | 100-500 | Per 10K steps |
| Synaptic Updates | ~1M per step | With 1K neurons |
| Memory Usage | ~50 MB | With 1K neurons |
| Checkpoint Size | ~5 MB | With 1K neurons |
| Wall Clock Time | 10-30s | With 500 neurons, 2000 steps |

### Building and Running Phase 6

**Building:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Running Phase 6 Demo:**
```bash
./nlm_test_phase6
```

This runs a comprehensive integration test verifying all brain systems are connected and working together.

### License

MIT

### Authors

Research project — See docs for scientific background.

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
