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

## Phase 6: Final Integration (Phase 6 Demo)

**Integration Test: The complete artificial brain integration**

### What This Tests

Phase 6 verifies that all previously developed brain systems work together as a unified artificial brain:

```
WORLD (16x16 grid) ← SENSORIAL PERCEPT (vision, touch, internal)
                         ↓
AGENT BRAIN (SensoryTransduction) → BRAIN (Neural Processing)
                         ↓
                   WORKING MEMORY ← EPISODIC MEMORY
                   NEUROMODULATION
                   PREDICTION SYSTEM
                   DEVELOPMENT SYSTEM
                         ↓
                     MOTOR OUTPUT ← ACTION SELECTION
                         ↓
                     WORLD (Apply Action)
                         ↓
                     REWARD / ERROR → PLASTICITY
```

### Phase 6 Integration Achievements

**Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.**

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ **Working Memory**: Persistent activity with competitive selection
- ✅ **Episodic Memory**: Experience encoding with pattern completion
- ✅ **Associative Memory**: Hebbian pattern associations
- ✅ **Memory Integration**: All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ **Dopamine**: Affects neural excitability and plasticity
- ✅ **Curiosity**: Drives exploration behavior
- ✅ **Novelty Detection**: Integrated with sensory processing
- ✅ **All Neuromodulators**: Connected to plasticity rules

#### Prediction System
- ✅ **Prediction System**: Integrated into brain loop
- ✅ **Prediction Error**: Signals affect learning
- ✅ **Confidence Tracking**: Implemented

#### Cognition Systems
- ✅ **Neural Planner**: Action sequence evaluation
- ✅ **Concept Formation**: Pattern discovery from experience
- ✅ **Attention**: Competitive selection dynamics
- ✅ **All Cognition**: Connected to perception and action

#### Development Integration
- ✅ **Developmental Stages**: Affect plasticity rates
- ✅ **Structural Plasticity**: Modulated by age
- ✅ **Neural Excitability**: Changes with development

#### Persistence
- ✅ **Checkpoint System**: Save/load implemented
- ✅ **Brain State Serialization**: Working
- ✅ **Resume Capability**: Can restart from saved checkpoints

#### Replay and Consolidation
- ✅ **Episodic Memory Replay**: During simulation
- ✅ **Memory Consolidation**: For important episodes
- ✅ **Integration**: With sleep/rest cycle

### Running the Phase 6 Demo

```bash
# After building with CMake
./nlm_phase6_demo
```

### What You See

The demo runs comprehensive integration tests:

1. **Integration Verification**: Confirms all brain systems are present
2. **Memory Integration Test**: Verifies working and episodic memory
3. **Neuromodulation Integration Test**: Checks dopamine, curiosity, novelty
4. **Checkpoint Test**: Tests save/load functionality
5. **Replay Test**: Verifies episodic memory replay
6. **Full Integration Experiment**: Complete 2000-step simulation with agent

### Expected Output

You should see output like:

```
=== NLM Phase 6 Integration Demo ===
Testing the integrated artificial brain...

--- Integration Verification ---
[PASS] Working memory is integrated
[PASS] Episodic memory is integrated
[PASS] Dopamine system is integrated
[PASS] Curiosity system is integrated
[PASS] Prediction system is integrated
[PASS] Planner is integrated
[PASS] Concept formation is integrated
[PASS] Attention is integrated
[PASS] Development system is integrated

Integration verification passed!

--- Memory Integration Test ---
[PASS] Working memory has active traces: X
[PASS] Episodic memory has episodes: Y

--- Full Integration Experiment ---
Step 0 | Reward: 0.5 | Firing: 12.3 | WorkingMem: 2
...

=== FINAL RESULTS ===
Total reward (avg per step): 0.523
Average firing rate: 12.7
Synaptic weight mean: 0.512 ± 0.087
Episodic episodes stored: 42
Working memory usage: 3
Memory retention: 0.832
Novelty level: 0.156
Curiosity level: 0.234
Dopamine level: 0.012
Learning efficiency: 0.456

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
```

### Integration Score Explained

The **System Integration Score (0.0-1.0)** shows how well all systems work together:

- **1.0** = Perfect integration
- **0.8-0.99** = Excellent integration
- **0.6-0.79** = Good integration
- **0.4-0.59** = Moderate integration
- **0.2-0.39** = Poor integration
- **0.0-0.19** = Failed integration

### Phase 6 Key Achievements

1. **All Systems Working**: Every brain system is functional and connected
2. **Learning Observed**: Synaptic weights change through experience
3. **Memory Formed**: Working memory maintains information, episodic memory stores experiences
4. **Neuromodulation Active**: Dopamine affects plasticity, curiosity drives exploration
5. **Prediction Working**: Brain predicts action consequences
6. **Development Progress**: Brain matures over time
7. **Checkpointing Functional**: State can be saved and restored
8. **Replay Available**: Past experiences can be replayed for consolidation

### What This Means

Phase 6 represents the culmination of the NLM project:

✅ **Real Neural Computation**: LIF neuron dynamics with event-driven spikes
✅ **World Interaction**: Complete brain-world loop with agent
✅ **Learning Mechanisms**: STDP, Hebbian, and reward-modulated plasticity
✅ **Cognitive Capabilities**: Memory, prediction, attention, planning
✅ **Development**: Maturation and structural plasticity
✅ **Integration**: All systems work together as one brain

The NLM brain is now a **functioning artificial neural substrate** capable of:
- Changing its synaptic connections through experience
- Learning from reward prediction errors
- Maintaining information in working memory
- Storing and retrieving episodic memories
- Predicting action consequences
- Adapting behavior based on developmental stage

This represents a **complete artificial brain** that can learn, remember, and adapt through interaction with its environment.

## Building

### Build the Phase 6 Demo

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Run All Demos

```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: World interaction
./nlm_phase3_demo 5 200

# Phase 4: Cognitive emergence
./nlm_phase4_demo

# Phase 6: Final integration (NEW!)
./nlm_phase6_demo
```

### Build Options

- `-DCMAKE_BUILD_TYPE=Debug` for debugging
- `-DCMAKE_BUILD_TYPE=Release` for performance
- `-DBUILD_TESTS=ON` to build test suite
