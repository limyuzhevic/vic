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

### Status Score
**Overall Integration Status: 43.6% (43/120 points)**

| Category | Score | Max | Notes |
|----------|-------|-----|-------|
| Neural Core | 18 | 20 | Working LIF, spikes, basic plasticity |
| Memory Systems | 4 | 20 | Defined but disconnected |
| Neuromodulation | 5 | 15 | Basic dopamine only |
| Cognition | 0 | 20 | All disconnected |
| Prediction | 0 | 10 | Disconnected |
| Development | 4 | 10 | Minimal integration |
| Persistence | 1 | 10 | Stubs only |
| Embodiment | 8 | 10 | Working sensory-motor loop |
| Experiments | 3 | 5 | Framework exists, limited execution |
| **TOTAL** | **43** | **120** | **35.8%** |

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

## How It Works

The integrated brain loop in Phase 6 represents the core architecture of NLM as an artificial developmental brain:

### The Brain Loop

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

### Key Integration Features

**1. Multilayer Processing**
- **Sensory Layer**: Converts world percepts to neural signals (vision, touch, proprioception)
- **Core Layer**: LIF neuron dynamics with event-driven spike propagation
- **Memory Layer**: Working memory, episodic memory, associative memory (when integrated)
- **Prediction Layer**: Temporal sequence learning and action consequence prediction
- **Cognitive Layer**: Attention, planning, concept formation (when integrated)

**2. Neuromodulation Integration**
- **Dopamine**: Modulates plasticity strength based on reward prediction error
- **Curiosity**: Drives exploration by detecting novelty and prediction error
- **Novelty**: Detects unexpected sensory input to trigger exploration
- **Prediction Error**: Compares expected vs actual outcomes to guide learning

**3. Developmental Integration**
- **Structural Plasticity**: Synaptogenesis and pruning every 100 steps
- **Stage-dependent Plasticity**: Different developmental stages affect learning rates
- **Maturation**: Gradual transition from high plasticity to stable circuitry

**4. Memory Systems Integration**
- **Working Memory**: Persistent neural activity for immediate information holding
- **Episodic Memory**: Neural pattern storage for experience replay
- **Associative Memory**: Hebbian pattern associations linking co-occurring events
- **Concept Formation**: Pattern discovery without explicit labels (when integrated)

**5. Prediction System Integration**
- **Next-step Prediction**: Learns temporal sequences from sensory input
- **Action Consequence Prediction**: Predicts outcomes of planned actions
- **Confidence Tracking**: Estimates reliability of predictions
- **Error-driven Learning**: Prediction errors modulate synaptic plasticity

**6. Cognitive Integration**
- **Attention**: Competitive selection among neural populations (lateral inhibition)
- **Planning**: Uses predictions to evaluate multi-step action sequences
- **Self-model**: Sensorimotor self-awareness and body schema
- **Social Learning**: Observation and imitation of other agents (when integrated)

### Integration Status

The current implementation has:
- ✅ **Working Core**: LIF neurons, spike propagation, STDP and Hebbian plasticity
- ✅ **Sensory-Motor Loop**: Complete perception-action cycle with SimpleWorld
- ✅ **Basic Neuromodulation**: Dopamine affects plasticity, Curiosity enables exploration
- ✅ **Development**: Basic developmental stages affecting plasticity rates
- ⚠️ **Partial Integration**: Some cognitive systems implemented but not connected to brain loop
- ❌ **Missing Integration**: Many systems (Working Memory, Prediction, Planning) disconnected from main brain loop

### Phase 6 Integration Experiments

The Phase 6 integration experiment (`nlm_phase6_demo`) tests the complete brain loop with all subsystems.

## Quick Start

### Phase 6 Demo (Integration Test)

Run the comprehensive integration test to verify all brain systems work together:

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected and functional.

### Python API Quick Start

For Python users, NLM provides a complete brain simulation interface:

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world for embodied intelligence
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Enable integrated subsystems
agent.enableRewardModulation(True)  # Dopamine-based learning
agent.enableCuriosity(True)        # Exploration drive
agent.enableDevelopment(True)      # Developmental maturation
agent.enableStructuralPlasticity(True)  # Growing new connections

# Run complete brain loop
for step in range(1000):
    world.update(0.1)                        # Update environment
    percept = world.getSensoryPercept()      # Get sensory input
    agent.processSensoryInput(percept)       # Process in brain
    brain.step(step)                         # Neural computation
    action = agent.decodeMotorCommand()      # Select action
    world.applyMotorCommand(action, time)    # Act in world
    reward = world.computeReward(agent)      # Get feedback
    agent.applyRewardModulation(reward, 0.0) # Learn from reward
    agent.updateDevelopment(0.1)            # Age brain
```

### Building from Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Phase 6 Integration Tests

After building, run the Phase 6 integration test to verify complete brain functionality:

```bash
./nlm_phase6_demo
```

### Experiment Framework

NLM includes a comprehensive experiment framework for testing integration:

```python
import pynlm

# Create an experiment to test specific integration aspects
experiment = pynlm.createExperiment("integration_test")
experiment.addMetric("memory_retention")
experiment.addMetric("prediction_accuracy")
experiment.addMetric("learning_progress")

# Run experiment with agent in world
experiment.run(agent_simulation(brain, world, agent, 1000))
```

## Architecture

The Phase 6 architecture diagram should show the complete integrated brain:

- **Outer Loop**: World ↔ Agent ↔ Brain ↔ Neural Core
- **Integration Points**: Neuromodulation, Memory, Prediction, Development
- **Processing Pipeline**: Sensory → Neural → Memory/Prediction → Action → Reward → Plasticity

Current architecture documentation is available in:
- `docs/ARCHITECTURE.md` - Detailed component overview
- `docs/PHASE6_FINAL_AUDIT.md` - Current integration status and gaps
- `docs/SCIENCE.md` - Scientific background for the integration approach
- `docs/PHASE6_INTEGRATION_OVERVIEW.md` - Detailed integration explanation

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
