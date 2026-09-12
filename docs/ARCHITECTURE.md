# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
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
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
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

### 7. Memory Layer (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 4)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

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

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Integration Diagram

### Phase 6 Brain Integration Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                      Phase 6: Full Integration                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Memory   │───▶│   Cognitive  │          │
│  │   Input    │    │  Systems    │    │  Systems    │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Working    │    │  Neural     │          │
│  │  Processing │    │  Memory     │    │  Planner    │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Vision     │    │  Episodic   │    │  Concept    │          │
│  │  Neurons    │    │  Memory     │    │  Formation  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                            │
│                            ▼                            │
│                    ┌─────────────┐                      │
│                    │  Prediction │                      │
│                    │   System    │                      │
│                    └─────────────┘                      │
│                            │                            │
│                            ▼                            │
│                    ┌─────────────┐                      │
│                    │Neuromodul. │                      │
│                    │  Signals    │                      │
│                    └─────────────┘                      │
│                            │                            │
│                            ▼                            │
│                    ┌─────────────┐                      │
│                    │ Development │                      │
│                    │   System    │                      │
│                    └─────────────┘                      │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                  AGENT-BRAIN INTERFACE                       ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │ Sensory │  │ Motor   │  │ Reward  │  │ Dev/Att │       ││
│  │  │ Trans-  │  │ Decod-  │  │ Mod-    │  │/Plan   │       ││
│  │  │ duction │  │ ing    │  │ ulation │  │         │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### Detailed Data Flow

**From Sensory to Action (Phase 3/4 Complete):**
1. **World Perception**: SimpleWorld → SensoryPercept
2. **Sensory Transduction**: AgentBrain.processSensoryInput() → Brain.receiveSensoryInput()
3. **Neural Processing**: Brain.step() with LIF dynamics and plasticity
4. **Motor Decoding**: AgentBrain.decodeMotorCommand() → SimpleWorld.applyAction()

**Phase 6 Integration Points (Target):**

#### Memory Integration Pipeline
```
Sensory Input (Vision, Touch, Internal, Proprioception)
      ↓
  ┌─────────────────────────────────────┐
  │  Working Memory Storage               │
  │  - Persistent activity patterns       │
  │  - Competition for selective retention │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Attention System                     │
  │  - Lateral inhibition competition     │
  │  - Top-down and bottom-up bias        │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Concept Formation                    │
  │  - Pattern discovery without labels   │
  │  - Prototype formation                │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Planning System                      │
  │  - Multi-step prediction              │
  │  - Goal-directed action selection     │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Episodic Memory Storage              │
  │  - Experience encoding                │
  │  - Replay for consolidation            │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Self-Model Update                    │
  │  - Sensorimotor learning              │
  │  - Agency representation              │
  └─────────────────────────────────────┘
      ↓
  ┌─────────────────────────────────────┐
  │  Social Learning                      │
  │  - Observation and imitation          │
  │  - Simple communication signals       │
  └─────────────────────────────────────┘
```

#### Neuromodulation Integration
```
Reward Prediction Error
      ↓
Dopamine Modulates:
  - STDP weight scaling
  - Plasticity rates throughout brain
  - Memory consolidation
      ↓
ACh Modulates:
  - Attention gain
  - Memory encoding
  - Novelty signals
      ↓
NE Modulates:
  - Arousal and vigilance
  - Sensory processing
  - Fight-or-flight responses
      ↓
5-HT Modulates:
  - Mood
  - Decision making
  - Social behavior
      ↓
Curiosity:
  - Novelty + Prediction error
  - Exploration drive
  - Integration with planning
```

## Integration Phases

### Phase 2: Neural Computation

**Current Focus:** Basic spiking neural computation and plasticity

**Components Integrated:**
- LIF Neuron dynamics
- Synaptic transmission
- Spike processing (event-driven)
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: World Interaction

**Current Integration:** Embodied cognition with sensory-motor loop

**Missing:** Higher cognitive systems (memory, prediction, planning)

### Phase 4: Emerging Cognition

**Components Implemented:**
- Attention (competitive selection)
- Working Memory (persistent activity)
- Episodic Memory (experience storage)
- Concept Formation (pattern discovery)
- Planning (multi-step prediction)
- Self-Model (sensorimotor learning)
- Social Learning (observation/imitation)

**Missing Integration:** These systems are fully defined but NOT used in the brain loop.

### Phase 5: Extended Cognition

**Extensions:**
- Neuromodulation systems (dopamine, novelty, curiosity)
- Development system (age-based plasticity)
- Integration with world through AgentBrain

### Phase 6: Full Integration (Target)

**Goal:** Create a unified artificial brain where all cognitive systems work together

**Integration Requirements:**
1. **Memory Integration**
   - Sensory input → Working memory storage
   - Working memory → Attention
   - Attention → Concept formation
   - Concept formation → Planning
   - Planning → Action selection
   - Action → Self-model update
   - Experience → Episodic memory
   - Episodic memory → Consolidation
   - Sleep/rest cycle for consolidation
   - Replay mechanism for memory strengthening

2. **Cognitive Integration**
   - Neural planner used in action selection
   - Concept formation processes all experiences
   - Self-model updates based on actions
   - Social learning observes other agents
   - Spatial representation guides navigation

3. **Neuromodulation Integration**
   - Dopamine affects plasticity throughout brain
   - ACh connects attention to memory
   - NE modulates arousal and vigilance
   - Serotonin regulates mood and decision making
   - Curiosity integrates exploration with learning
   - Novelty signals focus attention on new experiences

4. **Prediction Integration**
   - Prediction system trained on sensory input
   - Predictions used for attention selection
   - Prediction errors train all systems
   - Predictive coding improves efficiency

5. **Persistence**
   - Brain state saved/loaded via CheckpointSystem
   - Life history tracking for development
   - Experience replay for consolidation

### Data Flow Integration

**Current Brain::step() Flow:**
1. Process delayed spikes
2. Update neurons (LIF dynamics)
3. Detect spikes and schedule events
4. Apply STDP and Hebbian plasticity
5. Structural plasticity (every 100 steps)

**Phase 6 Brain::step() Flow (Target):**
1. Process delayed spikes ✅
2. Update neurons (LIF dynamics) ✅
3. Detect spikes and schedule events ✅
4. Apply STDP and Hebbian plasticity ✅
5. Structural plasticity (every 100 steps) ✅
6. **Update working memory from sensory input** ❌
7. **Store experience in episodic memory** ❌
8. **Update prediction system** ❌
9. **Update attention system** ❌
10. **Update concept formation** ❌
11. **Apply neuromodulation effects** ❌
12. **Consolidate memory (every 1000 steps)** ❌
13. **Replay important memories** ❌
14. **Apply development effects (every 1000 steps)** ✅

## Phase 4 Key Innovations

### Prediction System
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability
- Error signals modulate learning

### Working Memory
- Persistent neural activity
- Recurrent connections for maintenance
- Competition for selective retention
- Neural-grounded (not a vector store)

### Episodic Memory
- Experience encoded as neural patterns
- Pattern completion retrieval
- Temporal and spatial indexing
- Replay during consolidation

### Concept Formation
- Discovers patterns without labels
- Prototype formation through averaging
- Stability measurement
- Category hints from properties

### Attention
- Lateral inhibition competition
- Winner-take-more dynamics
- Top-down and bottom-up bias
- NOT Transformer QKV attention

### Planning
- Uses learned predictions
- Evaluates potential futures
- Selects actions leading to goals
- Adapts based on outcomes

### Self-Model
- Learns body schema
- Predicts action consequences
- Self vs external distinction
- Capability representation

### Social Learning
- Observes other agents
- Learns action effects
- Imitation capability
- Simple communication signals