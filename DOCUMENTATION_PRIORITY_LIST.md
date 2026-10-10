# NLM Public API Documentation Priority List

## IMMEDIATE DOCUMENTATION PRIORITY (Complete by Phase 7)

### 1. CORE MODULE - Foundation APIs

#### src/core/Types/Types.hpp ⭐⭐⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 28 type definitions, 16 classes/structs, 12 enums

**Must-Document Classes**:
- **NeuronId** - Neuron identifier with indexing and comparison
- **SynapseId** - Synapse identifier with indexing and comparison  
- **RegionId** - Brain region identifier with indexing and comparison
- **PopulationId** - Neural population identifier with indexing and comparison

**Must-Document Enums**:
- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType`: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited

**Must-Document Structs**:
- `PlasticityFlags` - Bitfield for plasticity rule enables
- `SpikeEvent` - Neural spike event with metadata
- `DelayedSpikeEvent` - Spike with transmission delay

**Key Documentation Points**:
- Type safety rationale and design decisions
- ID management and lifecycle considerations
- Strong typing benefits over raw integers
- Sentinel value usage (`INVALID_*_ID`)

#### src/core/Config/Config.hpp ⭐⭐⭐⭐
**Status**: COMPLETE (Phase 1+)
**API Surface Area**: 16 methods, template-based value access

**Must-Document Classes**:
- **Config** - Configuration management with JSON support

**Must-Document Methods**:
- Configuration lifecycle: `loadFromFile()`, `saveToFile()`, `loadFromArgs()`
- Type-safe access: `get<T>()`, `getOr<T>()` template methods
- Configuration management: `set()`, `has()`, `remove()`, `getKeys()`
- Advanced features: `ConfigValue` variant system

**Key Documentation Points**:
- Type-safe configuration design
- JSON file format specification
- Runtime vs. file vs. command-line configuration hierarchy
- Template parameter usage and limitations

### 2. BRAIN MODULE - Neural System Core

#### src/brain/Brain.hpp ⭐⭐⭐⭐⭐
**Status**: COMPLETE (Phase 6+)
**API Surface Area**: 58 methods across 7 functional areas

**Must-Document Classes**:
- **Brain** - Central neural system coordinator

**Must-Document Method Categories**:

**A. Core Lifecycle** (8 methods):
- `Brain(std::shared_ptr<Config> config)` - Constructor with configuration
- `initialize()` - System initialization
- `step(SimulationStep, Timestamp)` - Main simulation step
- `reset()` - State reset
- `develop()` - Developmental stage management
- `updatePlasticity()` - Plasticity rule application

**B. I/O Integration** (5 methods):
- `receiveSensoryInput()` - Sensory input processing
- `injectCurrent()` - Direct neuron current injection
- `produceAction()` - Motor action generation
- `applyNeuromodulation()` - Neuromodulator application
- `logStatus()` - System status reporting

**C. System Access** (15 methods):
- All subsystem getters (SpikeSystem, STDP, Memory, Prediction, Cognition, Neuromodulation)
- Region management (add/get/remove regions, inter-region connections)
- Statistics gathering (neuron/synapse counts, firing rates, etc.)

**D. Persistence** (2 methods):
- `save()`, `load()` - Checkpointing support

**Key Documentation Points**:
- Brain's role as system coordinator
- Subsystem integration architecture
- PImpl pattern implementation details
- Move semantics and resource management

#### src/brain/Neuron.hpp ⭐⭐⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 43 methods covering LIF dynamics

**Must-Document Classes**:
- **Neuron** - Leaky Integrate-and-Fire neuron implementation

**Must-Document Method Categories**:

**A. Core Dynamics** (10 methods):
- `stepLIF()` - Actual LIF computation (Phase 2)
- `step()` - General step (placeholder for Phase 3)
- `checkThreshold()` - Spike detection
- `getLastSpikeTime()` - Spike history access

**B. State Management** (12 methods):
- Complete state access/modification API
- Firing state control (resting, active, refractory, inhibited)
- Membrane potential management
- Refractory period control

**C. Synaptic Operations** (10 methods):
- Input reception (excitatory/inhibitory/modulatory)
- Synapse relationship management
- Plasticity flag management
- Regional/population membership

**D. Initialization** (5 methods):
- `initializeRandom()` - Random parameter setup
- `reset()` - State reset
- Various setter methods for neuron parameters

**Key Documentation Points**:
- LIF dynamics explanation
- Biological inspiration and abstractions
- Input integration mechanisms
- Plasticity interaction points

#### src/brain/NeuralRegion.hpp ⭐⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 25 methods for regional coordination

**Must-Document Classes**:
- **NeuralRegion** - Brain region with population and connectivity

**Must-Document Method Categories**:

**A. Population Management** (8 methods):
- Create/retrieve populations
- Population statistics and access
- Population-level operations (stepping, resetting)

**B. Synapse Management** (7 methods):
- Local connection creation and management
- Synapse retrieval and statistics
- Connectivity queries (from/to neuron)

**C. Regional Operations** (7 methods):
- Step all regional components
- Reset operations
- Random connectivity initialization
- Neuron access across populations

**D. Statistics** (3 methods):
- Regional metrics and activity measurements
- Synaptic density calculations
- Average weight computations

**Key Documentation Points**:
- Regional modularity benefits
- Local vs. global connectivity distinction
- Population-level optimization opportunities

#### src/brain/Synapse.hpp ⭐⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 24 methods for synaptic dynamics

**Must-Document Classes**:
- **Synapse** - Neural connection with delay and plasticity

**Must-Document Method Categories**:

**A. Connection Properties** (8 methods):
- Weight, delay, type management
- Excitatory/inhibitory classification
- Basic connection state

**B. Activity Tracking** (7 methods):
- Pre/post spike history recording
- Eligibility trace management
- Efficacy modulation
- Plasticity flag management

**C. Operations** (6 methods):
- Step function (placeholder)
- Reset operations
- Random initialization
- Plasticity rule enabling

**D. Advanced Features** (3 methods):
- Reward-modulated learning support
- Synaptic efficacy management
- Plasticity rule configuration

**Key Documentation Points**:
- Synaptic transmission model
- Delay-based transmission mechanism
- Plasticity rule integration
- Short-term vs. long-term changes

### 3. MEMORY MODULE - Learning Systems

#### src/memory/Memory.hpp ⭐⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 40 methods across 5 memory systems

**Must-Document Classes**:
- **WorkingMemory** - Transient active information storage
- **EpisodicMemory** - Event and experience storage
- **SemanticMemory** - Fact-based knowledge storage
- **ProceduralMemory** - Skill and habit storage
- **AssociativeMemory** - Pattern association storage

**Must-Document Method Categories** (per class):

**A. Core Operations** (3-4 methods each):
- Store/retrieve operations
- Capacity and size management
- Clear/reset functionality

**B. Advanced Features** (2-3 methods each):
- WorkingMemory: decay operations
- EpisodicMemory: consolidation
- ProceduralMemory: proficiency updates
- AssociativeMemory: strength management

**Structs to Document**:
- **EpisodicMemoryItem** - Event structure with metadata
- **Skill** - Procedural memory skill definition

**Key Documentation Points**:
- Memory system distinctions and purposes
- Integration with neural patterns
- Consolidation and decay mechanisms
- Temporary vs. permanent storage strategies

### 4. DYNAMICS MODULE - Neural Computation

#### src/dynamics/SpikeSystem.hpp ⭐⭐⭐⭐
**Status**: COMPLETE (Phase 3+)
**API Surface Area**: 20 methods for event-driven computation

**Must-Document Classes**:
- **SpikeSystem** - Event-driven neural computation

**Must-Document Method Categories**:

**A. Spike Management** (6 methods):
- Queue spike events (with and without delays)
- Process spikes and delayed spikes
- Register handler callbacks
- History management

**B. Statistics** (6 methods):
- Spike count and rate calculations
- Most active neuron identification
- Pending event tracking

**C. Operations** (4 methods):
- Reset functionality
- Update rate management
- Visualization data access

**Structs to Document**:
- **DetailedSpikeEvent** - Enhanced spike event with metadata

**Key Documentation Points**:
- Event-driven vs. time-stepped computation
- Delay-based synaptic transmission
- Performance optimization strategies
- Handler callback mechanisms

### 5. PLASTICITY MODULE - Learning Rules

#### src/plasticity/PlasticityRule.hpp ⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 12 methods in abstract base class

**Must-Document Classes**:
- **PlasticityRule** - Abstract base for all plasticity rules

**Must-Document Methods**:
- Abstract interface: `update()`, `applyWeightChange()`, `getName()`
- Concrete methods: `isEnabled()`, `setEnabled()`
- Configuration patterns for derived rules

**Key Documentation Points**:
- Abstract base class design rationale
- Plasticity rule interface standards
- Implementation patterns for derived classes

#### src/plasticity/STDP.hpp ⭐⭐⭐
**Status**: COMPLETE (Phase 2+)
**API Surface Area**: 20 methods across 3 STDP variants

**Must-Document Classes**:
- **STDP** - Spike-Timing-Dependent Plasticity
- **RewardModulatedSTDP** - Dopamine-modulated variant
- **TripletSTDP** - Triplet interaction STDP

**Must-Document Methods**:
- Core plasticity: `update()`, `applyWeightChange()`, `getName()`
- Parameter management: `setLTPWeight()`, `getLTDWeight()`, etc.
- Configuration: `configure()` method

**Key Documentation Points**:
- STDP biological inspiration
- LTP/LTD mechanism explanation
- Reward modulation integration
- Parameter tuning guidance

### 6. COGNITION MODULE - Thought Processes

#### src/cognition/NeuralPlanner.hpp ⭐⭐⭐
**Status**: COMPLETE (Phase 4+)
**API Surface Area**: 20+ methods for planning and evaluation

**Must-Document Classes**:
- **NeuralPlanner** - Predictive action planning
- **SelfModel** - Internal self-representation
- **SocialLearning** - Social interaction learning

**Must-Document Methods**:

**NeuralPlanner**:
- `planAction()` - Primary planning interface
- `evaluateSequence()` - Action sequence evaluation
- `updatePlanQuality()` - Outcome-based learning

**SelfModel**:
- `recordSelfAction()` - Experience recording
- `predictActionConsequence()` - Forward model
- `computeSelfGeneratedLikeness()` - Self vs. external discrimination

**SocialLearning**:
- `observeAgentAction()` - Social observation
- `canImitate()` - Imitation capability
- `detectSignal()` - Communication detection

**Key Documentation Points**:
- Predictive coding architecture
- Self-model formation mechanisms
- Social learning algorithms
- Planning vs. search distinction

### 7. PREDICTION MODULE - Forward Models

#### src/prediction/PredictionSystem.hpp ⭐⭐⭐
**Status**: COMPLETE (Phase 4+)
**API Surface Area**: 15+ methods for predictive processing

**Must-Document Classes**:
- **PredictionSystem** - Central prediction engine
- **NeuralPrediction** - Individual prediction mechanisms

**Key Documentation Points**:
- Predictive coding theory implementation
- Prediction error computation
- Confidence and uncertainty handling
- Integration with learning systems

## DOCUMENTATION GAPS AND TODO ITEMS

### Phase 6 Completion Status
- **✅ Core modules**: Complete with documentation needs
- **✅ Brain module**: Complete with comprehensive APIs
- **✅ Memory systems**: Complete with multiple subsystems
- **✅ Neuromodulation**: Complete with integration
- **✅ Prediction systems**: Complete with forward models
- **✅ Cognition**: Complete with planning mechanisms
- **✅ Performance**: Complete with optimization systems
- **✅ Experimentation**: Complete with measurement systems

### Missing Documentation Areas
1. **Module Integration**: How components interact
2. **Performance Characteristics**: Time/memory complexity
3. **Configuration Usage**: Practical examples
4. **Biological Inspiration**: Neural vs. computational mappings
5. **Development Trajectory**: Phase-by-phase evolution

## NEXT STEPS FOR DOCUMENTATION

### Phase 7 Documentation Sprint
1. **Week 1**: Core module documentation (Types, Config)
2. **Week 2**: Brain module documentation (Brain, Neuron, Synapse)
3. **Week 3**: Memory and Dynamics module documentation
4. **Week 4**: Plasticity and Neuromodulation documentation

### Documentation Standards Implementation
1. **Doxygen Configuration**: Set up for consistent documentation
2. **API Reference**: Create searchable reference manual
3. **Tutorial Series**: Step-by-step usage examples
4. **Integration Guide**: Component interaction documentation

This analysis provides a comprehensive roadmap for prioritizing Doxygen documentation efforts, focusing on the most critical public APIs that form the foundation of the NLM system while ensuring all Phase 6 completed components are properly documented.