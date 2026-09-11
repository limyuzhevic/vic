# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
───────────────────────────────────────────────────────────────────┐
│                       NLM Brain                                 │
───────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │   Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │ │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└───────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations

### 8. Prediction Layer
- **PredictionSystem**: State prediction and error computation
- **NeuralPrediction**: Temporal sequence learning
- **PredictionErrorSignal**: Error signal computation

### 9. Cognitive Layer
- **NeuralPlanner**: Predictive action selection
- **ConceptFormation**: Pattern discovery and category formation

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Agent-Brain Interface Layer
- **AgentBrain**: Connects NLM brain to world (sensory transduction, motor decoding)
- **SensoryPercept**: Sensory data packet from agent
- **AgentBody**: Physical agent properties

### 14. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase3Experiment/Phase4Experiment**: Phase-specific experiments

## Integration Layer (Phase 6)

### Key Integration Components:

#### Agent-Brain Integration (Phase 3/4)
- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

#### AgentBrain Class (src/agent/AgentBrain.cpp/hpp)
- **SensoryPercept processing** → **Brain receiveSensoryInput**
- **MotorCommand decoding** → **Action selection**
- **Reward modulation** → **Neuromodulation** (dopamine levels)
- **Curiosity-driven exploration** → **Motor command selection**
- **Novelty detection** → **Curiosity computation**
- **Development stages** → **Plasticity modifiers**

#### AgentBody Class (src/agent/AgentBody.hpp)
- **Physical properties**: Position, orientation, velocity
- **Internal state**: Energy, health, age
- **Movement state**: IsMoving, isTurning
- **Action execution**: Movement, turning, interaction

#### SensoryPercept Class (src/agent/SensoryPercept.cpp/hpp)
- **Vision**: 16x16 grayscale intensity grid (no object labels)
- **Touch**: Binary collision signals
- **Internal**: Homeostatic signals
- **Proprioception**: Body position/velocity signals
- **Audio**: Audio-like signals (if enabled)

## Data Flow

### Phase 2: Neural Computation
```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Structural Plasticity (synaptogenesis/pruning)
```

### Phase 3: World Interaction
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Development System (stage progression)
```

### Phase 4: Emerging Cognition
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (competitive selection)
      ↓
SENSORY INPUT
      ↓
PREDICTION (temporal sequence learning)
      ↓
WORKING MEMORY (persistent activity)
      ↓
EPISODIC MEMORY (experience storage)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step prediction)
      ↓
SELF-MODEL (sensorimotor learning)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION
      ↓
PLASTICITY
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

### Phase 6: Integration
```
WORLD → SENSORY PERCEPT → AgentBrain.processSensoryInput() →
Brain.receiveSensoryInput() → Neural Processing → Memory Systems →
Prediction System → Neuromodulation → AgentBrain.decodeMotorCommand() →
WORLD.applyMotorCommand() → Reward → AgentBrain.applyRewardModulation()
```

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Current Implementation Status

### Fully Implemented & Integrated:
- ✅ LIF neuron dynamics with event-driven spikes
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Sensory processing (vision, touch, internal, proprioception)
- ✅ Motor decoding and action execution
- ✅ Reward prediction error computation
- ✅ Novelty and curiosity mechanisms
- ✅ Developmental stages and plasticity modulation
- ✅ Basic neuromodulation (dopamine-like signals)
- ✅ Eligibility traces for reward-modulated learning
- ✅ Agent/Body system with energy and state management
- ✅ Phase 3 experiment demo working

### Partially Implemented/Connected:
- ✅ WorkingMemory: NeuralWorkingMemory implemented but not integrated into brain loop
- ✅ EpisodicMemory: NeuralEpisodicMemory exists but not used
- ✅ PredictionSystem: Basic prediction system implemented but not integrated
- ⚠️ NeuralPlanner: Implemented but not used anywhere
- ⚠️ ConceptFormation: Partially implemented
- ⚠️ SocialLearning: Implemented but not integrated

### Missing/Placeholder Implementations:
- ❌ Brain::save() and Brain::load() are stubs
- ❌ NeuralWorkingMemory not integrated into brain loop
- ❌ EpisodicMemory never updated with experiences
- ❌ Prediction system never trained with real data
- ❌ NeuralPlanner never used for action selection
- ❌ Concept formation never processes experiences
- ❌ Self-model never updates from experience

## Component Locations

| Component | Header | Implementation |
|-----------|--------|----------------|
| Core (Types, Config, Random, Logger) | `src/core/` | `src/core/*.cpp` |
| Brain (Neuron, Synapse, NeuralPopulation) | `src/brain/` | `src/brain/*.cpp` |
| Dynamics (NeuralDynamics, SpikeSystem) | `src/dynamics/` | `src/dynamics/*.cpp` |
| Plasticity (Hebbian, STDP) | `src/plasticity/` | `src/plasticity/*.cpp` |
| Development (DevelopmentSystem) | `src/development/` | `src/development/*.cpp` |
| Neuromodulation (Neuromodulator, Dopamine) | `src/neuromodulation/` | `src/neuromodulation/*.cpp` |
| Memory (NeuralWorkingMemory, NeuralEpisodicMemory) | `src/memory/` | `src/memory/*.cpp` |
| Prediction (PredictionSystem) | `src/prediction/` | `src/prediction/*.cpp` |
| Cognition (NeuralPlanner) | `src/cognition/` | `src/cognition/*.cpp` |
| Sensory (Vision, Audio) | `src/sensory/` | `src/sensory/*.cpp` |
| Motor (MotorSystem) | `src/motor/` | `src/motor/*.cpp` |
| Environment (SimpleWorld) | `src/world/` | `src/world/*.cpp` |
| Agent (AgentBrain, SensoryPercept) | `src/agent/` | `src/agent/*.cpp` |
| Experiments (Phase3, Phase4 demos) | `src/experiments/` | `src/experiments/*.cpp` |