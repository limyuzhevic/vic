# System Overview

## Introduction

NLM (熙然) is a comprehensive computational brain simulation framework designed to model complex neural processes including learning, memory, development, and cognition. The system integrates multiple biological-inspired components to create a unified neural simulation platform.

## System Architecture

### Core Architecture Components

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

### System Layers

#### Layer 1: Core Infrastructure
The foundation layer provides essential utilities and infrastructure:

- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system with JSON support
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system with configurable levels
- **SimulationClock**: High-precision time management

#### Layer 2: Neural Core
The core neural computation layer implements fundamental brain processes:

- **Brain**: Central coordinator managing all neural components
- **Neuron**: Individual neuron with Leaky Integrate-and-Fire dynamics
- **Synapse**: Synaptic connections with plasticity support
- **NeuralPopulation**: Groups of neurons with shared properties
- **NeuralRegion**: Brain regions with regional interactions

#### Layer 3: Learning Systems
The learning layer implements various plasticity mechanisms:

- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Correlated activity-based learning
- **STDP**: Spike-timing-dependent plasticity
- **StructuralPlasticity**: Synapse formation and pruning

#### Layer 4: Developmental Systems
The development layer models brain development and maturation:

- **DevelopmentSystem**: Orchestrates developmental processes
- **Synaptogenesis**: New synapse formation
- **Pruning**: Elimination of weak connections
- **Maturation**: Development of adult properties

#### Layer 5: Neuromodulation
The neuromodulation layer provides biological modulation signals:

- **Neuromodulator**: Abstract neuromodulator base
- **Dopamine**: Reward and reinforcement signals
- **Novelty**: Novelty detection and exploration
- **Curiosity**: Motivation for exploration
- **PredictionError**: Error signals for learning

#### Layer 6: Memory Systems
The memory layer provides working and episodic memory:

- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Long-term memory encoding and retrieval

#### Layer 7: Predictive Processing
The prediction layer implements forward models:

- **NeuralPrediction**: Temporal sequence prediction
- **PredictionSystem**: Action effect prediction

#### Layer 8: Cognitive Systems
The cognitive layer implements higher-level processes:

- **ConceptFormation**: Pattern discovery and abstraction
- **NeuralPlanner**: Predictive action selection

#### Layer 9: Sensory Processing
The sensory layer processes external and internal inputs:

- **SensoryInput**: Base class for all sensory inputs
- **Vision**: Visual processing and feature extraction
- **Audio**: Audio processing and analysis
- **InternalSignals**: Internal state signals

#### Layer 10: Motor Control
The motor layer generates actions and movements:

- **Action**: Action representation and planning
- **MotorSystem**: Neural-to-action conversion

#### Layer 11: Environment Interface
The environment layer provides interaction with external world:

- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper

#### Layer 12: Experiment Framework
The experiment layer provides testing and validation:

- **Experiment**: Individual experiment recording
- **ExperimentRunner**: Experiment execution and analysis

## System Components

### 1. Core Component

The Core component provides the fundamental infrastructure for the NLM system:

**Key Responsibilities**:
- Type safety through strongly-typed identifiers
- Configuration management with JSON support
- Deterministic random number generation
- Structured logging and debugging
- High-precision time management

**Interfaces**:
- `core/Types/Types.hpp` - Type definitions and identifiers
- `core/Config/Config.hpp` - Configuration management
- `core/Random/Random.hpp` - Random number generation
- `core/Logger/Logger.hpp` - Logging system
- `core/SimulationClock/SimulationClock.hpp` - Time management

### 2. Brain Component

The Brain component represents the central neural processing unit:

**Key Responsibilities**:
- Manage neuron populations and their interactions
- Coordinate synaptic plasticity and learning
- Integrate sensory inputs and generate motor outputs
- Implement developmental processes
- Coordinate neuromodulatory signals

**Interfaces**:
- `brain/Brain.hpp` - Main brain interface
- `brain/Neuron.hpp` - Individual neuron representation
- `brain/Synapse.hpp` - Synaptic connections
- `brain/NeuralPopulation.hpp` - Neuron groups
- `brain/NeuralRegion.hpp` - Brain regions

### 3. Plasticity Component

The Plasticity component implements learning rules and synaptic changes:

**Key Responsibilities**:
- Implement spike-timing-dependent plasticity (STDP)
- Implement Hebbian learning rules
- Manage structural plasticity (synaptogenesis/pruning)
- Coordinate reward-modulated learning
- Provide plasticity rule interfaces

**Interfaces**:
- `plasticity/PlasticityRule.hpp` - Base plasticity interface
- `plasticity/Hebbian.hpp` - Hebbian learning
- `plasticity/STDP.hpp` - STDP learning
- `plasticity/StructuralPlasticity.hpp` - Structural changes

### 4. Development Component

The Development component models brain development and maturation:

**Key Responsibilities**:
- Implement synaptogenesis (new synapse formation)
- Implement pruning (elimination of weak connections)
- Model maturation (development of adult properties)
- Coordinate developmental critical periods
- Adjust plasticity rates based on developmental stage

**Interfaces**:
- `development/DevelopmentSystem.hpp` - Development orchestration
- `development/Synaptogenesis.hpp` - Synapse formation
- `development/Pruning.hpp` - Synapse elimination
- `development/Maturation.hpp` - Adult property development

### 5. Neuromodulation Component

The Neuromodulation component provides biological modulation signals:

**Key Responsibilities**:
- Implement dopamine reward signals
- Implement novelty detection and exploration
- Implement curiosity-driven exploration
- Implement prediction error signals
- Coordinate neuromodulatory effects on neural activity

**Interfaces**:
- `neuromodulation/Neuromodulator.hpp` - Base neuromodulator
- `neuromodulation/Dopamine.hpp` - Reward signals
- `neuromodulation/Novelty.hpp` - Novelty detection
- `neuromodulation/Curiosity.hpp` - Exploration motivation
- `neuromodulation/PredictionError.hpp` - Error signals

### 6. Memory Component

The Memory component provides working and episodic memory:

**Key Responsibilities**:
- Implement working memory for temporary information storage
- Implement episodic memory for long-term experience encoding
- Support memory replay and consolidation
- Provide memory retrieval and search capabilities
- Coordinate memory with learning processes

**Interfaces**:
- `memory/Memory.hpp` - High-level memory interface
- `memory/NeuralWorkingMemory.hpp` - Working memory
- `memory/NeuralEpisodicMemory.hpp` - Episodic memory

### 7. Prediction Component

The Prediction component implements forward models and prediction:

**Key Responsibilities**:
- Learn temporal sequences and predict future states
- Implement action consequence prediction
- Coordinate prediction with learning processes
- Provide prediction error signals
- Support multi-step prediction

**Interfaces**:
- `prediction/PredictionSystem.hpp` - Prediction interface
- `prediction/NeuralPrediction.hpp` - Sequence prediction

### 8. Cognition Component

The Cognition component implements higher-level cognitive processes:

**Key Responsibilities**:
- Implement attentional selection
- Implement forward planning and action selection
- Form self-models based on sensorimotor experience
- Implement social learning mechanisms
- Support concept formation and abstraction

**Interfaces**:
- `cognition/NeuralPlanner.hpp` - Action planning
- `cognition/ConceptFormation.hpp` - Concept formation

### 9. Sensory Component

The Sensory component processes external and internal inputs:

**Key Responsibilities**:
- Process visual inputs and extract features
- Process audio inputs and analyze sounds
- Handle internal state signals
- Provide sensory preprocessing
- Coordinate sensory inputs with brain systems

**Interfaces**:
- `sensory/SensoryInput.hpp` - Base sensory interface
- `sensory/Vision.hpp` - Visual processing
- `sensory/Audio.hpp` - Audio processing
- `sensory/InternalSignals.hpp` - Internal signals

### 10. Motor Component

The Motor component generates actions and movements:

**Key Responsibilities**:
- Convert neural activity to motor actions
- Plan and sequence actions
- Implement action selection mechanisms
- Coordinate motor outputs with environment
- Provide motor control interfaces

**Interfaces**:
- `motor/MotorSystem.hpp` - Motor control interface
- `motor/Action.hpp` - Action representation

### 11. Environment Component

The Environment component provides simulation environments:

**Key Responsibilities**:
- Implement simulation environments
- Provide sensory observations
- Handle action execution
- Support episode management
- Coordinate environment interaction

**Interfaces**:
- `environment/Environment.hpp` - Base environment interface
- `environment/Observation.hpp` - Observation processing
- `environment/SimpleWorld.hpp` - Simple environment implementation

### 12. Experiment Component

The Experiment component provides testing and validation:

**Key Responsibilities**:
- Record experimental data
- Execute and manage experiments
- Analyze experimental results
- Support experiment comparisons
- Provide experiment management interfaces

**Interfaces**:
- `experiments/Experiment.hpp` - Experiment interface
- `experiments/ExperimentRunner.hpp` - Experiment execution
- `experiments/Metrics.hpp` - Data analysis

## Data Flow Architecture

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
Environment.update()
      ↓
Get Observations
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation
      ↓
Motor Output
      ↓
Environment.executeAction()
```

### Phase 4: Emerging Cognition
```
Sensory Input
      ↓
Prediction System
      ↓
Prediction Error
      ↓
Learning Signals
      ↓
Cognition System
      ↓
Action Selection
      ↓
Motor Output
      ↓
Environment Feedback
      ↓
Memory Update
      ↓
Concept Formation
```

## Configuration

### Core Configuration

The NLM system is configured through JSON configuration files with the following structure:

```json
{
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000,
        "enable_progress_bar": true,
        "enable_logging": true,
        "log_level": "INFO"
    },
    "brain": {
        "neuron_count": 500,
        "region_count": 1,
        "enable_development": false
    },
    "memory": {
        "enable_working_memory": true,
        "enable_episodic_memory": true,
        "wm_capacity": 20,
        "wm_decay_rate": 0.001
    },
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_novelty": true,
        "enable_curiosity": true,
        "enable_prediction_error": true
    },
    "prediction": {
        "enable_prediction": true,
        "prediction_horizon": 20
    },
    "cognition": {
        "enable_attention": true,
        "enable_planning": true,
        "enable_self_model": true,
        "enable_social_learning": true
    },
    "development": {
        "enable_development": true,
        "developmental_stages": 10
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_reward_modulated": true
    }
}
```

### Configuration Loading

Configuration can be loaded from:
1. JSON files using `Config::loadFromFile()`
2. JSON strings using `Config::loadFromJsonString()`
3. Command-line arguments using `Config::loadFromArgs()`
4. Runtime settings using `Config::set()`

## Performance Considerations

### Computational Complexity

- **Neuron Update**: O(N) where N is number of neurons
- **Synapse Processing**: O(E) where E is number of synapses
- **Plasticity**: O(E) per time step
- **Memory Operations**: O(1) for working memory, O(log N) for episodic memory

### Memory Usage

- **Neuron State**: ~100 bytes per neuron
- **Synapse State**: ~80 bytes per synapse
- **Memory Storage**: Variable based on content
- **Prediction Models**: O(T) where T is prediction horizon

### Optimization Strategies

1. **Sparse Connectivity**: Use sparse synaptic connections
2. **Vectorization**: Use SIMD instructions for parallel processing
3. **Event-Driven Updates**: Use spike-based updates instead of time-stepped
4. **Memory Pools**: Use memory pools for frequent allocations
5. **Caching**: Cache frequently accessed data
6. **Asynchronous Processing**: Use multi-threading for parallel computation

## Extensibility

### Adding New Components

1. **New Brain Regions**: Add new `NeuralRegion` subclasses
2. **New Plasticity Rules**: Implement new `PlasticityRule` subclasses
3. **New Neuromodulators**: Implement new `Neuromodulator` subclasses
4. **New Memory Systems**: Implement new memory system interfaces
5. **New Sensor Types**: Implement new `SensoryInput` subclasses
6. **New Motor Controllers**: Implement new action generation strategies

### Component Composition

Components can be composed through:
- **Brain Composition**: Multiple brain regions with different functions
- **Plasticity Mixing**: Multiple plasticity rules with different weights
- **Development Stages**: Multiple developmental stages with different properties
- **Neuromodulation Integration**: Multiple neuromodulators with different effects

## System States

### Simulation States

1. **Initialized**: System constructed, default configuration loaded
2. **Configured**: System configured with user parameters
3. **Running**: System actively simulating neural processes
4. **Paused**: System simulation temporarily halted
5. **Completed**: System finished execution
6. **Reset**: System state reset to initial conditions

### Component States

- **Neuron**: Resting, Firing, Refractory
- **Synapse**: Active, Inactive, Plastic
- **Memory**: Active, Inactive, Consolidating
- **Prediction**: Learning, Predicting, Evaluating
- **Cognition**: Processing, Planning, Executing

## System Integration

### Inter-Component Communication

Components communicate through:
- **Direct Method Calls**: Synchronous function calls
- **Event Systems**: Asynchronous event notification
- **Message Queues**: Thread-safe communication
- **Shared Memory**: Fast inter-process communication
- **Signal Slots**: Qt-style signal/slot connections

### Data Flow

Data flows through the system in the following pattern:
1. **Sensory Input** → **Sensory Processing** → **Brain Core**
2. **Brain Core** → **Plasticity System** → **Development System**
3. **Development System** → **Neuromodulation System**
4. **Neuromodulation System** → **Memory System**
5. **Memory System** → **Prediction System**
6. **Prediction System** → **Cognition System**
7. **Cognition System** → **Motor System**
8. **Motor System** → **Environment**
9. **Environment** → **Sensory Input** (feedback loop)

## System Monitoring

### Metrics and Monitoring

The system provides monitoring through:
- **Performance Metrics**: Computation time, memory usage, accuracy
- **Health Indicators**: Component status, error rates, convergence
- **Debug Information**: Step-by-step execution trace
- **Logging**: Structured logging with configurable levels
- **Profiling**: Performance profiling and analysis

### Monitoring Interfaces

- **CLI Monitoring**: Command-line monitoring tools
- **GUI Monitoring**: Graphical monitoring interfaces
- **API Monitoring**: RESTful API for monitoring
- **Remote Monitoring**: Network-based monitoring

## Future Enhancements

### Phase 5: Advanced Cognitive Functions
- **Theory of Mind**: Understanding others' mental states
- **Emotional Processing**: Emotion generation and regulation
- **Language Processing**: Natural language understanding and generation
- **Long-term Memory**: Episodic memory consolidation

### Phase 6: Integrated Intelligence
- **Meta-learning**: Learning how to learn
- **Transfer Learning**: Generalization across tasks
- **Self-improvement**: Autonomous system improvement
- **Creative Generation**: Novelty generation and creativity

### Phase 7: Human-level Intelligence
- **Common sense reasoning**: Everyday knowledge and reasoning
- **Abstract thinking**: Conceptual and theoretical reasoning
- **Moral reasoning**: Ethical decision making
- **Consciousness simulation**: Conscious experience modeling

## Conclusion

The NLM system provides a comprehensive framework for neural simulation that integrates multiple biologically-inspired components. The modular architecture allows for flexible component composition, while the comprehensive API enables easy extension and customization. The system supports a wide range of simulation scenarios from basic neural dynamics to advanced cognitive functions.