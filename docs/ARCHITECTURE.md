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

#### Types System
- **Strongly-typed identifiers**: NeuronId, SynapseId, RegionId, PopulationId
  - **Purpose**: Type-safe brain component identification
  - **Benefits**: Prevents invalid component references, improves debugging
  - **Implementation**: Combines integer IDs with type tags for compile-time safety

#### Configuration Management
- **Config Class**: Centralized parameter management
  - **Purpose**: Manages all simulation parameters with validation
  - **Key Features**:
    - Type-safe parameter storage (int, double, string, bool, arrays)
    - Multiple configuration sources (default, file, command line, runtime)
    - Parameter validation with custom validators
    - Section-based organization
  - **Configuration Sources**:
    - Default: Built-in default values
    - File: Loaded from configuration files
    - CommandLine: Parsed from program arguments
    - Runtime: Set during program execution

#### Random Number Generation
- **Random Class**: Deterministic random number generator
  - **Purpose**: Provides reproducible stochastic behavior
  - **Features**: Thread-safe, supports multiple distributions
  - **Use Cases**: Neuron initialization, synaptic weight randomization

#### Logging System
- **Logger Class**: Structured logging infrastructure
  - **Purpose**: Comprehensive logging for debugging and monitoring
  - **Levels**: Trace, Debug, Info, Warning, Error, Fatal
  - **Outputs**: Console, file, or custom handlers
  - **Example Usage**:
    ```cpp
    NLM_LOG_INFO("Brain initialized with {} neurons", brain->getTotalNeuronCount());
    NLM_LOG_ERROR("Failed to load configuration: {}", errorMessage);
    ```

#### Simulation Clock
- **SimulationClock Class**: Time management for discrete simulations
  - **Purpose**: Provides consistent timestep management
  - **Features**:
    - Monotonic time tracking
    - Fixed or variable timestep support
    - Synchronization across components
  - **Methods**:
    - `getCurrentTime()`: Current simulation time
    - `advanceTime(double dt)`: Advance simulation time
    - `getTimestep()`: Get current timestep duration

### 2. Brain Layer

#### Neuron Class
- **Individual neuron representation**: Implements Leaky Integrate-and-Fire (LIF) dynamics
- **Purpose**: Biological-inspired computational unit
- **Key Features**:
  - **LIF Dynamics**: Membrane potential integration with exponential decay
  - **Refractory Period**: Prevents excessive firing rates
  - **Spike-Frequency Adaptation**: Slows firing over time
  - **Multiple States**: Resting, Active, Refractory, etc.
- **Core Methods**:
  ```cpp
  Neuron::stepLIF(Timestamp time, TimestepDuration dt)
  Neuron::recordSpike(Timestamp time)
  Neuron::initializeRandom(RandomGenerator& rng)
  Neuron::receiveExcitatoryInput(MembranePotential amplitude)
  Neuron::receiveInhibitoryInput(MembranePotential amplitude)
  ```

#### Synapse Class
- **Synaptic connection**: Implements spike-based transmission with plasticity
- **Purpose**: Models biological synaptic communication
- **Key Features**:
  - **Synaptic Delays**: 1-100+ step delays
  - **Weight Dynamics**: Plasticity with bounds (-1.0 to +1.0)
  - **Short-Term Plasticity**: Facilitation and depression
  - **Eligibility Traces**: For reward-modulated learning
- **Core Methods**:
  ```cpp
  Synapse::step(Timestamp currentTime)
  Synapse::recordPreSpike(Timestamp time)
  Synapse::recordPostSpike(Timestamp time)
  Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)
  ```

#### NeuralPopulation Class
- **Group of neurons with shared properties**: Enables efficient processing
- **Purpose**: Represents neuron groups with common characteristics
- **Key Features**:
  - **Property Sharing**: Neurons share type, region, and population properties
  - **Efficient Operations**: Batch operations on neuron groups
  - **Type Specialization**: Different populations for different functions
- **Use Cases**:
  - Sensory processing populations
  - Motor output populations
  - Interconnecting populations

#### NeuralRegion Class
- **Brain region**: Logical grouping of neural populations
- **Purpose**: Represents brain regions with specialized functions
- **Key Features**:
  - **Hierarchical Organization**: Regions contain populations
  - **Regional Connectivity**: Intra-regional and inter-regional connections
  - **Function Specialization**: Different regions handle different tasks
- **Examples**:
  - Cortex regions for sensory processing
  - Motor cortex regions for action planning
  - Memory regions for storage and retrieval

#### Brain Class
- **Central coordinator**: Manages all brain components and simulation loop
- **Purpose**: Main interface for brain operations
- **Key Features**:
  - **Component Management**: Creates and manages neurons, synapses, regions
  - **Simulation Loop**: Implements timestep-based processing
  - **Integration Point**: Connects all subsystems
  - **State Management**: Saves/loads brain state
- **Core Methods**:
  ```cpp
  Brain::initialize(): Initialize all components
  Brain::step(SimulationStep step): Run one simulation timestep
  Brain::reset(): Reset brain to initial state
  Brain::getTotalNeuronCount(): Get total neuron count
  Brain::getTotalSynapseCount(): Get total synapse count
  ```

### 3. Dynamics Layer

#### NeuralDynamics Class (Abstract)
- **Abstract base**: Defines interface for neuron dynamics implementations
- **Purpose**: Enables different neuron models
- **Key Features**:
  - **Template Pattern**: Concrete implementations provide dynamics
  - **Extensibility**: Easy to add new neuron models
  - **Consistency**: Standardized interface across models
- **Example Implementations**:
  - LIF Dynamics: Leaky Integrate-and-Fire
  - HH Dynamics: Hodgkin-Huxley (full ion channel dynamics)

#### SpikeSystem Class
- **Event-driven spike processing**: Efficient spike propagation
- **Purpose**: Manages spike timing and delivery
- **Key Features**:
  - **Event Queue**: Processes spikes in temporal order
  - **Delay Handling**: Implements synaptic transmission delays
  - **Efficiency**: Sparse representation for sparse networks
  - **Concurrency**: Parallel processing capabilities
- **Core Methods**:
  ```cpp
  SpikeSystem::processSpikes(Timestamp currentTime)
  SpikeSystem::addSpikes(NeuronId neuronId, const std::vector<Timestamp>& times)
  SpikeSystem::getMostActiveNeurons(int count) const
  ```

### 4. Plasticity Layer

#### PlasticityRule Class (Abstract)
- **Abstract base**: Defines interface for learning rules
- **Purpose**: Standardizes plasticity mechanisms
- **Key Features**:
  - **Modular Design**: Each rule can be enabled/disabled independently
  - **Interface Consistency**: All rules follow same pattern
  - **Extensibility**: Easy to add new learning rules

#### STDP (Spike-Timing-Dependent Plasticity)
- **Learning rule**: Weight changes depend on precise spike timing
- **Purpose**: Biologically realistic learning
- **Learning Rule**: Δw = {A+ if pre before post, A- if post before pre}
- **Time Constants**: Typically 20-50ms for both potentiation and depression
- **Key Features**:
  - **Pre-Post Ordering**: Pre before post → LTP (potentiate)
  - **Post-Pre Ordering**: Post before pre → LTD (depress)
  - **Time Window**: ~100ms window for effective pairing

#### Hebbian Learning
- **Learning rule**: Simple correlation-based learning
- **Purpose**: Implements "fire together, wire together"
- **Mathematical Formulation**: Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
- **Key Features**:
  - **Correlation-based**: Learns statistical regularities
  - **Bounded**: Prevents runaway potentiation
  - **Simpler than STDP**: Computationally efficient

#### Structural Plasticity
- **Dynamic synapse formation/removal**: Activity-dependent connectivity
- **Purpose**: Enables network self-organization
- **Key Features**:
  - **Synaptogenesis**: Forms new connections based on activity
  - **Pruning**: Eliminates weak or unused connections
  - **Homeostatic Balance**: Maintains optimal connectivity density

### 5. Development Layer

#### DevelopmentSystem Class
- **Development orchestration**: Manages brain development stages
- **Purpose**: Implements developmental learning trajectory
- **Key Stages**:
  1. **Initial**: Primitive networks, high plasticity
  2. **CriticalPeriod**: Enhanced learning, sensitive periods
  3. **Maturation**: Specialization, refined circuits
  4. **Adult**: Stable, efficient processing
  5. **Aging**: Gradual decline in plasticity

#### Synaptogenesis Class
- **New synapse formation**: Activity-dependent connectivity growth
- **Purpose**: Builds initial network structure
- **Algorithm**: Probabilistic synapse creation based on pre-synaptic activity
- **Rate Modulation**: Higher during critical periods

#### Pruning Class
- **Weak synapse removal**: Eliminates unused connections
- **Purpose**: Refines network for efficiency
- **Mechanism**: Based on synaptic efficacy and activity correlation
- **Benefits**: Reduces noise, improves signal-to-noise ratio

#### Maturation Class
- **Adult property development**: Stabilization of neural properties
- **Purpose**: Transitions to stable adult function
- **Processes**:
  - Synaptic weight stabilization
  - Myelin development (in biological models)
  - Reduction of developmental plasticity

### 6. Neuromodulation Layer

#### Neuromodulator Class (Abstract)
- **Abstract base**: Defines neuromodulator interface
- **Purpose**: Modulates neural activity and plasticity
- **Key Features**:
  - **Global Signals**: Affect multiple brain regions
  - **State-Dependent**: Responses vary with brain state
  - **Learning Integration**: Modulate plasticity rules

#### Dopamine Class
- **Reward signaling**: Implements reward prediction error
- **Purpose**: Reinforcement learning modulation
- **Functions**:
  - **Reward Response**: Burst firing on unexpected rewards
  - **Prediction Error**: Differences between expected and actual rewards
  - **Plasticity Modulation**: Enhances or suppresses learning

#### Novelty Class
- **Novelty detection**: Identifies unexpected stimuli
- **Purpose**: Drives exploration and curiosity
- **Mechanism**: Compares current input to internal predictions
- **Output**: Novelty signal modulates attention and learning

#### Curiosity Class
- **Exploration motivation**: Drives information-seeking behavior
- **Purpose**: Balances exploration vs exploitation
- **Algorithm**: Maximizes information gain while maintaining performance

#### PredictionError Class
- **Error computation**: Computes prediction mismatches
- **Purpose**: Enables predictive coding
- **Use Cases**: Error-based learning, confidence estimation

### 7. Memory Systems (Phase 4)

#### NeuralWorkingMemory Class
- **Persistent activity**: Maintains information temporarily
- **Purpose**: Short-term storage during ongoing tasks
- **Mechanism**: Recurrent connections with competitive dynamics
- **Applications**: Working memory, task maintenance

#### NeuralEpisodicMemory Class
- **Experience encoding**: Stores complete episodes
- **Purpose**: Long-term memory of specific events
- **Features**:
  - **Pattern Completion**: Retrieves full episodes from partial cues
  - **Replay**: Reactivates patterns during consolidation
  - **Temporal Indexing**: Organizes experiences chronologically

#### NeuralAssociativeMemory Class
- **Experience-based associations**: Links related patterns
- **Purpose**: Connects concepts and experiences
- **Mechanism**: Hebbian associations between related patterns
- **Use Cases**: Memory recall, inference

#### ConceptFormation Class
- **Pattern discovery**: Discovers patterns without labels
- **Purpose**: Categorization and abstraction
- **Algorithm**: Clustering similar patterns into prototypes
- **Stability Measurement**: Assesses pattern consistency

### 8. Prediction Systems (Phase 4)

#### NeuralPrediction Class
- **Temporal sequence learning**: Learns time-dependent relationships
- **Purpose**: Enables forward modeling
- **Architecture**:
  - **Input Layer**: Current state
  - **Hidden Layer**: Temporal features
  - **Output Layer**: Predicted next state
- **Training**: Self-supervised learning from experience

#### ActionConsequencePredictor Class
- **Action-effect prediction**: Associates actions with outcomes
- **Purpose**: Enables planning and decision making
- **Learning**: Maps actions to their consequences
- **Use Cases**: Policy evaluation, reinforcement learning

### 9. Cognitive Systems (Phase 4)

#### AttentionalSelection Class
- **Competitive selection**: Chooses between competing representations
- **Purpose**: Focuses processing resources
- **Mechanism**: Lateral inhibition with top-down bias
- **Note**: NOT Transformer QKV attention - implements biologically plausible attention

#### NeuralPlanner Class
- **Predictive action selection**: Evaluates future outcomes
- **Purpose**: Multi-step planning
- **Algorithm**: Simulates possible futures using learned predictions
- **Output**: Selected action based on predicted value

#### SelfModel Class
- **Sensorimotor self-awareness**: Represents body's capabilities
- **Purpose**: Distinguishes self-generated from external effects
- **Applications**: Agency detection, error correction

#### SocialLearning Class
- **Observation and imitation**: Learning from other agents
- **Purpose**: Cultural transmission and social cognition
- **Mechanism**: Imitates observed actions and their outcomes

### 10. Sensory Layer

#### SensoryInput Class (Abstract)
- **Abstract base**: Defines sensory interface
- **Purpose**: Standardizes sensory input processing
- **Implementations**:
  - **Vision**: Visual field processing
  - **Audio**: Sound processing
  - **InternalSignals**: Proprioception, interoception

#### Vision Class
- **Visual processing**: Extracts features from visual input
- **Purpose**: Provides visual information to brain
- **Parameters**:
  - Field of view (FOV)
  - Resolution (pixels per cell)
  - Contrast sensitivity

#### Audio Class
- **Sound processing**: Processes auditory information
- **Purpose**: Enables sound-based behaviors
- **Features**:
  - Frequency analysis
  - Temporal pattern recognition

### 11. Motor Layer

#### Action Class
- **Action representation**: Encodes motor commands
- **Purpose**: Translates neural activity to behavior
- **Types**:
  - **Movement**: Physical displacement
  - **Grasping**: Object manipulation
  - **Communication**: Social signals

#### MotorSystem Class
- **Neural-to-action conversion**: Maps brain activity to behavior
- **Purpose**: Controls physical interaction with environment
- **Algorithm**: Decodes neural patterns into motor commands
- **Feedback**: Receives sensory feedback for error correction

### 12. Environment Layer

#### Environment Class (Abstract)
- **Abstract environment interface**: Defines environment standards
- **Purpose**: Provides interaction platform
- **Key Methods**:
  - `reset()`: Initialize environment state
  - `update(dt)`: Advance environment by timestep
  - `getSensoryPercept()`: Get current sensory input
  - `applyMotorCommand(action, time)`: Execute action

#### SimpleWorld Class
- **2D environment with objects**: Simple test environment
- **Purpose**: Prototypical environment for testing
- **Features**:
  - **Static Objects**: Walls, resources, hazards
  - **Dynamic Agent**: Movable agent with sensing capabilities
  - **World State**: Grid-based representation

### 13. Experiments Layer

#### Experiment Class (Abstract)
- **Abstract experiment base**: Defines experiment interface
- **Purpose**: Standardizes experiment implementation
- **Key Methods**:
  - `setup()`: Initialize experiment
  - `run(step)`: Execute one timestep
  - `analyze()`: Analyze results

#### ExperimentRunner Class
- **Experiment execution**: Manages experiment lifecycle
- **Purpose**: Orchestrates experiment runs
- **Features**:
  - **Multiple Experiments**: Run series of experiments
  - **Metrics Collection**: Track performance metrics
  - **Result Persistence**: Save/load experimental data

## Component Interaction Example

### Brain Initialization and Usage

```cpp
// Create and configure brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000, ConfigSource::Default);
config->set("plasticity.stdp.enable", true, ConfigSource::Default);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Create environment and agent
auto world = std::make_shared<SimpleWorld>();
world->configure(20, 20, 8, 8);

// Run simulation loop
for (SimulationStep step = 0; step < 1000; ++step) {
    // 1. Update environment
    world->update(0.1);
    
    // 2. Get sensory input
    auto percept = world->getSensoryPercept();
    
    // 3. Process in brain
    brain->receiveSensoryInput(percept);
    brain->step(step);
    
    // 4. Generate action
    auto action = brain->produceAction();
    
    // 5. Apply action to world
    world->applyMotorCommand(action, step);
}
```

### Key Integration Points

1. **Configuration Bridge**: Config provides parameters to all components
2. **Data Flow**: Sensory → Brain → Motor → Environment → World
3. **Learning Integration**: Plasticity rules operate within brain processing loop
4. **State Management**: Brain state saved/loaded independently
5. **Development Timeline**: Brain undergoes staged development throughout simulation

## Design Principles

### 1. Biological Inspiration
- Spiking neuron dynamics (LIF)
- Event-driven processing
- Competitive attention
- Neuromodulatory control

### 2. Computational Efficiency
- Event-driven spike propagation
- Sparse representations
- Modular architecture
- Deterministic random number generation

### 3. Extensibility
- Clear interface abstractions
- Plugin-style plasticity rules
- Configurable network topology
- Modular environment support

### 4. Reproducibility
- Deterministic simulations via random seeds
- Checkpoint/save functionality
- Configuration-based parameter management
- Comprehensive logging

## Performance Considerations

### Memory Efficiency
- Use appropriate neuron counts for available memory
- Configure region sizes for cache locality
- Disable unused plasticity mechanisms

### Speed Optimization
- Use event-driven spike propagation
- Configure appropriate simulation timestep
- Parallelize independent components

### Scalability
- Design for different network sizes
- Support for varying connectivity patterns
- Memory management for large networks

## Further Reading

- [API_REFERENCE.md](docs/API_REFERENCE.md) - Complete API documentation
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and biological inspiration
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization techniques
- [HOW_TO_USE.md](HOW_TO_USE.md) - Python API usage examples

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