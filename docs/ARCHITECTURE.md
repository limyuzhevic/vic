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

## API Reference Integration

This section provides detailed API documentation for the key NLM classes, extending the architectural overview with implementation details.

### Core Brain Components

#### Brain Class
- **Location**: `src/brain/Brain.hpp`
- **Description**: Central coordinator implementing real spiking neural computation
- **Key Methods**:
  - `initialize()` - Complete brain initialization with neural structure
  - `step(SimulationStep step, Timestamp time)` - Main simulation step
  - `receiveSensoryInput(SensoryInput& input)` - Process sensory data from environment
  - `produceAction()` - Generate motor output based on neural activity
  - `getConfig()` - Access configuration object

#### Neuron Class  
- **Location**: `src/brain/Neuron.hpp`
- **Description**: Individual neuron using Leaky Integrate-and-Fire (LIF) dynamics
- **Key Methods**:
  - `Neuron(NeuronId id)` - Constructor with unique identifier
  - `stepLIF(Timestamp time, TimestepDuration dt)` - LIF dynamics computation
  - `recordSpike(Timestamp time)` - Maintain spike history for plasticity
  - `getState()` - Access neuron state (membrane potential, refractory period, etc.)

#### Synapse Class
- **Location**: `src/brain/Synapse.hpp`
- **Description**: Synaptic connection with delays and short-term plasticity
- **Key Methods**:
  - `Synapse(SynapseId id, NeuronId source, NeuronId destination)` - Connection constructor
  - `step(Timestamp time)` - Update synaptic dynamics
  - `recordPreSpike(Timestamp time)` / `recordPostSpike(Timestamp time)` - STDP spike timing
  - `getWeight()` / `setWeight(SynapticWeight w)` - Synaptic strength control

#### Config Class
- **Location**: `src/core/Config/Config.hpp`
- **Description**: Centralized configuration management with type safety
- **Key Methods**:
  - `loadFromFile(const std::string& filepath)` - Load configuration from file
  - `loadFromArgs(int argc, char** argv)` - Parse command line arguments
  - `get<T>(const std::string& key)` - Type-safe value retrieval
  - `set(const std::string& key, T value, ConfigSource source)` - Set configuration values

### Dynamics Models

#### NeuralDynamics (Abstract)
- **Location**: `src/dynamics/NeuralDynamics.hpp`
- **Description**: Abstract base for neuron and synapse dynamics
- **Implementations**:
  - `IntegrateAndFireDynamics` - Simple and efficient LIF model
  - `HodgkinHuxleyDynamics` - Detailed biophysical model (placeholder)
  - `FitzHughNagumoDynamics` - Simplified excitable system (placeholder)

#### IntegrateAndFireDynamics
- **Description**: Implements Leaky Integrate-and-Fire neuron dynamics
- **Key Features**:
  - Membrane time constant parameter control
  - Threshold-based spike generation
  - Exponential decay toward resting potential
  - Biological plausibility with configurable parameters

### Plasticity Rules

#### PlasticityRule (Abstract)
- **Location**: `src/plasticity/PlasticityRule.hpp`
- **Description**: Abstract base for synaptic weight changes
- **Implementations**:
  - `HebbianRule` - "Neurons that fire together, wire together"
  - `AntiHebbianRule` - "Fire together, wire apart" (competition)
  - `BCMRule` - Bienenstock-Cooper-Munro sliding threshold learning

#### HebbianRule
- **Description**: Standard Hebbian learning with configurable learning rate
- **Parameters**:
  - `setLearningRate(float rate)` - Control learning strength
  - `getLearningRate()` - Query current learning rate

### Development System

#### DevelopmentSystem
- **Description**: Orchestrates age-dependent changes in brain connectivity
- **Developmental Stages**:
  - `DevelopmentalStage::Initial` - Newborn stage
  - `DevelopmentalStage::Juvenile` - Critical period
  - `DevelopmentalStage::Adult` - Mature brain
- **Key Processes**:
  - `Synaptogenesis` - New synapse formation
  - `Pruning` - Elimination of weak connections
  - `Maturation` - Development of adult properties

### Neuromodulation Systems

#### Neuromodulators
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation based on novelty and prediction error
- **Novelty**: Detects unexpected sensory input
- **PredictionError**: Computes mismatch between expected and actual outcomes

### Memory Systems

#### Working Memory
- **Description**: Persistent neural activity for temporary information storage
- **Key Features**:
  - Recurrent connections for maintenance
  - Competition for selective retention
  - Neural-grounded representation (not vector storage)
  - Capacity limitations and decay dynamics

#### Episodic Memory
- **Description**: Experience encoding in neural patterns
- **Key Features**:
  - Temporal and spatial indexing of experiences
  - Pattern completion for retrieval
  - Replay during consolidation phases
  - Integration with concept formation

## Current Implementation Status

### Neural Core (Brain) - **PARTIALLY INTEGRATED**

**Status**: ✅ **WORKING** - Core neural computation is functional

**Status**: ❌ **DISCONNECTED** - Cognitive systems not integrated with main brain loop

**Critical Issues**:
- `Brain::getWorkingMemory()` returns nullptr
- `Brain::getEpisodicMemory()` returns nullptr  
- `Brain::getPredictionSystem()` returns nullptr
- `Brain::save()` and `Brain::load()` are not implemented
- `Brain::produceAction()` not connected to motor system

**Working Components**:
- ✅ LIF Neuron dynamics with refractory periods
- ✅ Synaptic transmission with delays and excitatory/inhibitory types
- ✅ Event-driven spike processing and propagation
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning every 100 steps)
- ✅ Basic neuromodulation (dopamine-like signals)
- ✅ Novelty detection and curiosity mechanisms
- ✅ Eligibility traces for reward-modulated learning

### Cognitive Integration Needs

**Target**: Create integrated brain that functions as a coherent artificial cognitive system

**Key Integration Challenges**:
1. **Memory System Integration**:
   - Connect working memory to planning systems
   - Integrate episodic memory with prediction systems
   - Implement associative memory for concept formation

2. **Prediction System Integration**:
   - Connect prediction errors to neuromodulation
   - Integrate temporal and action consequence prediction
   - Link prediction systems to action selection

3. **Cognition System Integration**:
   - Connect attention systems to perception
   - Integrate planning with working memory
   - Implement self-model for body awareness
   - Add social learning capabilities

4. **Action Selection Integration**:
   - Connect motor output to neural activity
   - Implement action consequence learning
   - Add reward-based learning mechanisms

### Development and Research Directions

#### Current Capabilities (Phase 3)
- ✅ Working environment interface (SimpleWorld)
- ✅ Basic sensory processing (Vision, Touch, Internal, Proprioception)
- ✅ Basic motor output (MotorCommand decoding)
- ✅ Developmental system implementation (stages, plasticity modifiers)
- ✅ Synaptogenesis implementation
- ✅ Pruning implementation
- ✅ Reward computation (prediction error)
- ✅ Basic neuromodulation (dopamine-like signal)
- ✅ Novelty detection
- ✅ Curiosity mechanism
- ✅ Eligibility traces for reward-modulated learning
- ✅ Agent/Body system with energy
- ✅ Phase 3 experiment demo

#### Next Steps (Phase 4-5)
- ✅ **Emergent Cognition**: Prediction systems, working memory, episodic memory
- ✅ **Advanced Learning**: Concept formation, spatial/temporal representation
- ✅ **Cognitive Architecture**: Attention, planning, self-model, social learning
- ✅ **Integration**: Complete brain-cognition-behavior loop
- ✅ **Advanced Experiments**: Working memory, episodic recall, concept formation, attention, planning

## Technical Specifications

### Simulation Parameters

#### Core Configuration
```yaml
# Neural Network
neuron_count: 1000                    # Total neurons
region_count: 1                        # Brain regions  
connection_probability: 0.1            # Synapse creation probability
random_seed: 42                        # Deterministic initialization

# Dynamics
neuron_model: "LIF"                    # Leaky Integrate-and-Fire
membrane_time_constant: 20.0          # Milliseconds (tau)
firing_threshold: -55.0               # Millivolts
reset_potential: -70.0                # Millivolts

# Plasticity
plasticity_enabled: true              # Enable all plasticity rules
stdp_ltp_weight: 0.01                  # STDP long-term potentiation
stdp_ltd_weight: 0.012                 # STDP long-term depression
stdp_tau: 20.0                         # STDP time constant
learning_rate: 0.01                    # Hebbian learning rate

# Development
enable_development: true              # Age-dependent plasticity
initial_development_stage: "Adult"    # Starting development stage

# Simulation
integration_interval: 1.0             # Seconds per timestep
simulation_timestep: 0.001            # Simulation timestep
replay_interval: 100                  # Memory replay frequency
consolidation_interval: 1000           # Memory consolidation frequency
```

### Performance Characteristics

#### Memory Requirements
- **Neurons**: ~100 neurons × 64 bytes = 6.4 KB
- **Synapses**: ~10,000 synapses × 48 bytes = 480 KB  
- **Systems**: ~1 MB total memory footprint

#### Computational Complexity
- **Time per timestep**: O(N²) for synaptic connections
- **Spike processing**: O(S) for S spikes
- **Plasticity updates**: O(P) for P plastic synapses

#### Scalability
- **Single thread**: Efficient for up to 10,000 neurons
- **Memory**: Linear scaling with neuron count
- **Time**: Quadratic scaling with connectivity

## Research and Biological Basis

### Biological Inspiration
1. **Neuron Dynamics**: Leaky Integrate-and-Fire model approximates real neuron behavior
2. **Synaptic Transmission**: Short-term plasticity based on Tsodyks-Markram model
3. **Plasticity Rules**: Inspired by Hebbian learning and STDP from experimental data
4. **Neuromodulation**: Dopamine, novelty, and curiosity based on biological evidence
5. **Development**: Synaptogenesis, pruning, and critical periods from developmental biology

### Scientific References
- Maass et al. (2002) "Computational models of spiking neurons"
- Song et al. (2000) "Competitive learning in recurrent networks"
- Dayan & Abbott (2001) "Theoretical Neuroscience"
- Sporns (2011) "Network Biology"
- Bienenstock et al. (1982) "Theory for the development of neuron selectivity"

### Limitations and Future Work

#### Current Limitations
1. **Simplified Dynamics**: LIF model lacks detailed ion channel dynamics
2. **Placeholder Components**: Some systems (HH dynamics, FHN dynamics) are empty implementations
3. **Disconnected Systems**: Major cognitive systems not integrated with main brain loop
4. **No Hierarchical Processing**: Single-layer architecture

#### Research Opportunities
1. **Advanced Dynamics**: Implement Hodgkin-Huxley for biological accuracy
2. **Hierarchical Processing**: Multi-scale brain architecture
3. **Full Integration**: Complete cognitive system integration
4. **Advanced Learning**: Unsupervised and self-supervised learning mechanisms
5. **Biological Fidelity**: More realistic neural and synaptic models

## Future Development Roadmap

### Phase 5: Advanced Neural Computation
- Implement Hodgkin-Huxley dynamics
- Add detailed synaptic modeling
- Enhance structural plasticity
- Develop advanced neuromodulation

### Phase 6: Complete Integration
- Full cognitive system integration
- Advanced learning algorithms
- Enhanced memory systems
- Comprehensive behavioral capabilities

### Phase 7: Advanced Research
- Hierarchical processing
- Neuromodular learning
- Complex decision making
- Social cognition

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
Structural Plasticity (synaptogenesis/pruning)
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
NEURAL PROCESSING
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
MOTOR ACTION
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
SOCIAL LEARNING (observation/imitation)
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

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