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

**PHASE 6: DEVELOPMENTAL INTEGRATION - PARTIALLY WORKING**

Phase 6 focuses on integrating existing components into a coherent brain system. Previous phases built individual components; Phase 6 is establishing functional connections between them. Integration is **70% complete** but critical cognitive and memory systems remain disconnected.

### Phase 6 Integration Status

#### ✅ WORKING SYSTEMS:

**Core Neural Dynamics**
- Real LIF neuron dynamics with event-driven spike propagation
- STDP and Hebbian plasticity mechanisms
- Structural plasticity (synaptogenesis and pruning)
- Neural population management

**Basic Agent System**
- World simulation with sensory input/output
- Agent brain interface with basic processing
- Neuromodulation (dopamine, curiosity, novelty)
- Reward prediction error computation

**Basic Development**
- Developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
- Plasticity modifiers that change with development
- Structural plasticity rate modulation

#### ⚠️ PARTIALLY CONNECTED SYSTEMS:

**Memory Systems**
- NeuralWorkingMemory, NeuralEpisodicMemory, SemanticMemory, ProceduralMemory, NeuralAssociativeMemory
- **STATUS**: Implemented but not integrated into brain loop - their accessors return nullptr
- Episodic memory storage, consolidation, and replay never actually occur

**Prediction System**
- PredictionSystem with forward model and prediction error
- **STATUS**: Implemented but not trained - no real prediction error computation

**Cognitive Systems**
- NeuralPlanner (action sequences, planning depth)
- ConceptFormation (pattern abstraction)
- AttentionalSelection (competitive selection)
- SelfModel (body schema, prediction)
- SocialLearning (imitation, theory of mind)
- **STATUS**: Implemented but never integrated into action selection or brain loop

#### ❌ BROKEN SYSTEMS:

**Checkpoint Persistence**
- Brain::save() and Brain::load() exist but checkpoints are incomplete
- Synapse loading only logs count, doesn't actually restore states
- Memory systems not included in checkpoint/restore

## What's Actually Working vs What's Broken

| Feature | Status | Description |
|---------|--------|-------------|
| **Neurons & Synapses** | ✅ WORKING | LIF dynamics, spike propagation, basic plasticity |
| **World Simulation** | ✅ WORKING | Environment with vision, touch, internal signals |
| **Agent Interface** | ✅ WORKING | Sensory processing, motor decoding, reward modulation |
| **Development** | ✅ WORKING | Stages, plasticity modifiers, synaptogenesis/pruning |
| **Memory Storage** | ❌ BROKEN | Classes exist but accessors return nullptr, never used |
| **Prediction** | ❌ BROKEN | Classes exist but not trained, no prediction error |
| **Cognition** | ❌ BROKEN | Classes exist but disconnected, never update brain behavior |
| **Persistence** | ⚠️ PARTIAL | Checkpoint infrastructure exists but incomplete |
| **Python API** | ✅ WORKING | Basic API functional, but missing advanced bindings |

## What's Actually Working vs What's Broken

| Feature | Status | Description |
|---------|--------|-------------|
| **Neurons & Synapses** | ✅ WORKING | LIF dynamics, spike propagation, basic plasticity |
| **World Simulation** | ✅ WORKING | Environment with vision, touch, internal signals |
| **Agent Interface** | ✅ WORKING | Sensory processing, motor decoding, reward modulation |
| **Development** | ✅ WORKING | Stages, plasticity modifiers, synaptogenesis/pruning |
| **Memory Storage** | ❌ BROKEN | Classes exist but accessors return nullptr, never used |
| **Prediction** | ❌ BROKEN | Classes exist but not trained, no prediction error |
| **Cognition** | ❌ BROKEN | Classes exist but disconnected, never update brain behavior |
| **Persistence** | ⚠️ PARTIAL | Checkpoint infrastructure exists but incomplete |
| **Python API** | ✅ WORKING | Basic API functional, but missing advanced bindings |

## Python Bindings

### Available (Working)
```python
import pynlm

# Basic neural components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Agent system
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Basic operations
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Missing (Not Available in Python)
```python
# These DO NOT exist in current Python bindings:
brain.getWorkingMemory()  # Returns None
brain.getEpisodicMemory()  # Returns None
brain.getPredictionSystem()  # Returns None
brain.getPlanner()  # Returns None
brain.getAttention()  # Returns None
brain.getConceptFormation()  # Returns None
brain.getSelfModel()  # Returns None
brain.getSocialLearning()  # Returns None
```

**IMPORTANT**: For advanced cognitive capabilities, use the C++ API directly. Python bindings only expose basic neural components.

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
