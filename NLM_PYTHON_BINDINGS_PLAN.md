# NLM Python Bindings Enhancement - Implementation Plan

## Overview

This plan details the implementation of missing Python bindings for the NLM (Neural Learning Machine) framework. The current Python API is missing many critical neuroscience classes and systems that are essential for advanced neural simulation and research.

## Implementation Priority Matrix

| Priority | Classes/Systems | Implementation Complexity | User Impact |
|----------|----------------|-------------------------|-------------|
| **HIGH** | Neuron, Synapse, NeuralPopulation, NeuralRegion | Medium | High |
| **HIGH** | WorkingMemory, EpisodicMemory, PredictionSystem | High | High |
| **HIGH** | Dopamine, Curiosity, NoveltyDetector | Medium | High |
| **MEDIUM** | NeuralPlanner, ConceptFormation, AttentionalSelection | High | Medium |
| **MEDIUM** | DevelopmentSystem, StructuralPlasticity | High | Medium |
| **LOW** | RandomGenerator, CheckpointManager, ExperimentRunner | Medium | Low |

## Implementation Strategy

### Phase 1: Core Neural Components (Weeks 1-3)

#### 1.1 Neuron Class Binding

**Goal**: Expose LIF neuron dynamics with full control over synaptic integration, plasticity, and state management.

**Key Methods to Expose**:
```cpp
// Constructor and Identity
Neuron(NeuronId id)
NeuronId getId() const

// Neuron Type and Properties
NeuronType getType() const
void setType(NeuronType type)

// Membrane Dynamics
MembranePotential getMembranePotential() const
void setMembranePotential(MembranePotential potential)
void injectCurrent(MembranePotential current)
MembranePotential getTotalCurrent() const
void clearTotalCurrent()

// LIF Computation
bool stepLIF(Timestamp currentTime, TimestepDuration dt)
bool checkThreshold() const
float getLastSpikeTime() const

// State Management
const NeuronState& getState() const
NeuronState& getState()
void reset()

// Spike History and Plasticity
void recordSpike(Timestamp timestamp)
const std::vector<Timestamp>& getSpikeHistory() const
void clearSpikeHistory()

// Synaptic Connections
void addIncomingSynapse(SynapseHandle handle)
void addOutgoingSynapse(SynapseHandle handle)
const std::vector<SynapseHandle>& getIncomingSynapses() const

// Plasticity Flags
const PlasticityFlags& getPlasticityFlags() const
PlasticityFlags& getPlasticityFlags()
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)

// Development and Parameters
void setLeakConductance(MembranePotential conductance)
MembranePotential getLeakConductance() const
void setRestingPotential(MembranePotential potential)
MembranePotential getRestingPotential() const
void setResetPotential(MembranePotential potential)
```

**Python API Design**:
```python
class Neuron:
    def __init__(self, neuron_id: int):
        """Create a new LIF neuron with given ID."""
    
    def get_id(self) -> int:
        """Get neuron ID."""
    
    def get_type(self) -> NeuronType:
        """Get neuron type (excitatory, inhibitory, sensory, motor, internal)."""
    
    def set_type(self, neuron_type: NeuronType):
        """Set neuron type."""
    
    def get_membrane_potential(self) -> float:
        """Get current membrane potential in mV."""
    
    def set_membrane_potential(self, potential: float):
        """Set membrane potential (advanced use)."""
    
    def inject_current(self, current: float):
        """Inject external current into neuron."""
    
    def step_lif(self, current_time: float, dt: float) -> bool:
        """
        Perform one LIF integration step.
        
        Returns:
            bool: True if neuron fired this step.
        """
    
    def get_last_spike_time(self) -> float:
        """Get timestamp of last spike (-1 if none)."""
    
    def record_spike(self, timestamp: float):
        """Manually record a spike (for testing)."""
    
    def reset(self):
        """Reset neuron to initial state."""
    
    def enable_plasticity(self, hebbian: bool = True, stdp: bool = True, 
                         reward_modulated: bool = False):
        """Enable plasticity rules."""
    
    def get_firing_rate(self) -> float:
        """Get current firing rate in Hz."""
    
    def get_activity(self) -> float:
        """Get activity level (|V - V_rest| / 20)."""
```

#### 1.2 Synapse Class Binding

**Goal**: Expose synaptic transmission dynamics with plasticity, delays, and short-term effects.

**Key Methods to Expose**:
```cpp
// Constructor and Identity
Synapse(SynapseId id, NeuronId source, NeuronId destination)
SynapseId getId() const

// Connection Identity
NeuronId getSourceNeuron() const
NeuronId getDestinationNeuron() const

// Synaptic Properties
SynapticWeight getWeight() const
void setWeight(SynapticWeight weight)
void addToWeight(SynapticWeight delta)

// Delay Dynamics
Delay getDelay() const
void setDelay(Delay delay)

// Synapse Type
SynapseType getType() const
void setType(SynapseType type)

// Activity History
void recordPreSpike(Timestamp timestamp)
void recordPostSpike(Timestamp timestamp)
const std::vector<Timestamp>& getPreSpikeHistory() const
const std::vector<Timestamp>& getPostSpikeHistory() const
void clearHistory()

// Plasticity
const PlasticityFlags& getPlasticityFlags() const
PlasticityFlags& getPlasticityFlags()
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)

// Eligibility Trace (Reward-Modulated Learning)
float getEligibilityTrace() const
void setEligibilityTrace(float trace)
void decayEligibilityTrace(float decayRate)

// Synaptic Efficacy (Use-Dependent Modulation)
float getEfficacy() const
void setEfficacy(float efficacy)

// State Management
void reset()
```

**Python API Design**:
```python
class Synapse:
    def __init__(self, synapse_id: int, source_id: int, dest_id: int):
        """Create a new synapse connecting source to destination neuron."""
    
    def get_id(self) -> int:
        """Get synapse ID."""
    
    def get_source(self) -> int:
        """Get source neuron ID."""
    
    def get_destination(self) -> int:
        """Get destination neuron ID."""
    
    def get_weight(self) -> float:
        """Get current synaptic weight."""
    
    def set_weight(self, weight: float):
        """Set synaptic weight."""
    
    def get_delay(self) -> int:
        """Get synaptic delay in simulation steps."""
    
    def set_delay(self, delay: int):
        """Set synaptic delay."""
    
    def get_type(self) -> SynapseType:
        """Get synapse type (excitatory, inhibitory, modulatory, etc.)."""
    
    def set_type(self, synapse_type: SynapseType):
        """Set synapse type."""
    
    def is_excitatory(self) -> bool:
        """Check if synapse is excitatory."""
    
    def is_inhibitory(self) -> bool:
        """Check if synapse is inhibitory."""
    
    def record_pre_spike(self, timestamp: float):
        """Record pre-synaptic spike for STDP."""
    
    def record_post_spike(self, timestamp: float):
        """Record post-synaptic spike for STDP."""
    
    def get_pre_spike_history(self) -> List[float]:
        """Get history of pre-synaptic spikes."""
    
    def get_post_spike_history(self) -> List[float]:
        """Get history of post-synaptic spikes."""
    
    def set_eligibility_trace(self, trace: float):
        """Set eligibility trace for reward-modulated learning."""
    
    def decay_eligibility_trace(self, decay_rate: float):
        """Decay eligibility trace."""
    
    def reset(self):
        """Reset synapse to initial state."""
```

#### 1.3 Population and Region Classes

**Goal**: Provide management interfaces for neuron groups and brain organization.

**Key Classes**:
```cpp
class NeuralPopulation {
public:
    NeuralPopulation(PopulationId id, size_t size);
    PopulationId getId() const;
    size_t getSize() const;
    Neuron* getNeuron(NeuronIndex index);
    const Neuron* getNeuron(NeuronIndex index) const;
    void createNeurons(NeuronId startId);
    const std::vector<Neuron*>& getNeurons() const;
    NeuronType getNeuronType() const;
    void setNeuronType(NeuronType type);
    size_t getActiveCount() const;
    size_t getFiringCount() const;
    float getAverageFiringRate() const;
    void initializeRandom(RandomGenerator& rng);
    void step(Timestamp currentTime);
    void reset();
};

class NeuralRegion {
public:
    NeuralRegion(RegionId id, const std::string& name);
    RegionId getId() const;
    const std::string& getName() const;
    void addPopulation(std::unique_ptr<NeuralPopulation> population);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    void addNeuron(std::unique_ptr<Neuron> neuron);
    const std::vector<Neuron*>& getAllNeurons() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    size_t getTotalNeuronCount() const;
    size_t getSynapseCount() const;
    float getAverageSynapticWeight() const;
};
```

### Phase 2: Memory Systems Integration (Weeks 4-6)

#### 2.1 Working Memory System

**Goal**: Expose working memory with trace maintenance, competition, and activation management.

**Key Methods**:
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    void initialize(Brain* brain);
    void setCapacity(size_t capacity);
    void storeToNeuron(NeuronId neuronId, float activation);
    void update(TimestepDuration dt);
    bool hasTrace(NeuronId neuronId) const;
    float getActivation(NeuronId neuronId) const;
    const std::vector<NeuronId>& getMemoryNeurons() const;
    const std::vector<float>& getMemoryActivations() const;
    const std::vector<uint64_t>& getMemoryTimestamps() const;
    size_t getActiveTraces() const;
    void clear();
    void restoreSnapshot(const std::vector<NeuronId>& neurons, 
                        const std::vector<float>& activations,
                        const std::vector<uint64_t>& timestamps);
};
```

**Python API**:
```python
class WorkingMemory:
    def __init__(self):
        """Create a working memory system."""
    
    def set_capacity(self, capacity: int):
        """Set working memory capacity (number of traces)."""
    
    def store_to_neuron(self, neuron_id: int, activation: float):
        """
        Store activation trace for neuron.
        
        Args:
            neuron_id: ID of neuron to store trace for
            activation: Activation level (0.0 to 1.0)
        """
    
    def update(self, dt: float):
        """Update working memory (decay and competition)."""
    
    def has_trace(self, neuron_id: int) -> bool:
        """Check if neuron has active trace."""
    
    def get_activation(self, neuron_id: int) -> float:
        """Get current activation for neuron."""
    
    def get_memory_neurons(self) -> List[int]:
        """Get IDs of neurons with active traces."""
    
    def get_memory_activations(self) -> List[float]:
        """Get activation levels of all traces."""
    
    def get_active_traces(self) -> int:
        """Get number of active traces."""
    
    def clear(self):
        """Clear all memory traces."""
```

#### 2.2 Episodic Memory System

**Goal**: Expose episodic memory with experience storage, replay, and consolidation.

**Key Classes**:
```cpp
struct EpisodicMemoryItem {
    uint64_t timestamp;
    float positionX;
    float positionY;
    float orientation;
    ActionType action;
    float reward;
    float energy;
    float novelty;
    float resultingReward;
    uint64_t age;
    std::vector<float> sensoryState;
    std::vector<float> resultingSensoryState;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
};

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    void initialize(Brain* brain);
    void setMaxEpisodes(size_t maxEpisodes);
    void storeEpisode(const EpisodicMemoryItem& episode);
    const std::vector<EpisodicMemoryItem*>& getEpisodes() const;
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    size_t getEpisodeCount() const;
    void consolidate(float threshold);
    void clear();
    void restoreEpisodes(const std::vector<EpisodicMemoryItem>& episodes);
};
```

**Python API**:
```python
class EpisodicMemoryItem:
    def __init__(self):
        # Initialize empty episode
    
    # Accessors
    def get_timestamp(self) -> int:
        pass
    
    def get_position(self) -> Tuple[float, float]:
        pass
    
    def get_action(self) -> ActionType:
        pass
    
    def get_reward(self) -> float:
        pass

class EpisodicMemory:
    def __init__(self):
        """Create an episodic memory system."""
    
    def set_max_episodes(self, max_episodes: int):
        """Set maximum number of episodes to store."""
    
    def store_episode(self, episode: EpisodicMemoryItem):
        """Store an episodic memory item."""
    
    def get_episodes(self) -> List[EpisodicMemoryItem]:
        """Get all stored episodes."""
    
    def get_episodes_for_replay(self, count: int) -> List[EpisodicMemoryItem]:
        """Get episodes suitable for replay (most recent/importants)."""
    
    def replay_episode(self, episode: EpisodicMemoryItem):
        """Replay an episode (reactivate associated neural patterns)."""
    
    def get_episode_count(self) -> int:
        """Get number of stored episodes."""
    
    def consolidate(self, threshold: float):
        """Remove weak episodes below consolidation threshold."""
```

#### 2.3 Associative Memory System

**Goal**: Expose pattern association and content-addressable memory.

### Phase 3: Neuromodulation Systems (Weeks 7-9)

#### 3.1 Dopamine System

**Goal**: Expose reward prediction error and reward-based modulation.

#### 3.2 Curiosity System

**Goal**: Expose novelty detection and curiosity-driven exploration.

#### 3.3 Novelty Detection

**Goal**: Expose change detection and surprise signals.

### Phase 4: Advanced Systems (Weeks 10-12)

#### 4.1 Prediction System

**Goal**: Expose forward models and prediction error signaling.

#### 4.2 Cognition Systems

**Goal**: Expose planning, concept formation, and attentional selection.

#### 4.3 Development System

**Goal**: Expose developmental stages and plasticity modulation.

## Implementation Schedule

### Week 1-3: Core Neural Components
- Implement Neuron class with full binding
- Implement Synapse class with plasticity support
- Implement NeuralPopulation and NeuralRegion
- Create Python test suite for core components

### Week 4-6: Memory Systems
- Implement WorkingMemory system
- Implement EpisodicMemory with storage/replay
- Implement AssociativeMemory
- Create memory system test suite

### Week 7-9: Neuromodulation Systems
- Implement Dopamine system
- Implement Curiosity and Novelty systems
- Add neuromodulation effects on plasticity
- Create neuromodulation test suite

### Week 10-12: Advanced Systems
- Implement PredictionSystem
- Implement NeuralPlanner
- Implement DevelopmentSystem
- Create advanced system test suite

## Testing and Validation

### Unit Tests
- **Neuron**: LIF dynamics, spike detection, plasticity
- **Synapse**: STDP learning, weight dynamics, delays
- **WorkingMemory**: Trace maintenance, competition, decay
- **EpisodicMemory**: Storage, retrieval, replay

### Integration Tests
- **Full Pipeline**: Brain initialization, simulation, plasticity
- **Memory Integration**: Working + Episodic memory interactions
- **Neuromodulation Integration**: Reward effects on learning
- **Development Integration**: Age-dependent plasticity changes

### Performance Tests
- **Scalability**: Test with 100 to 10,000 neurons
- **Real-time Simulation**: Measure simulation speed
- **Memory Usage**: Monitor memory consumption patterns

## Files to Modify

### Python Bindings (`python/bindings.cpp`)
- Add missing class definitions
- Update existing class bindings
- Add new function bindings
- Update module initialization

### CMake Configuration
- Update Python bindings CMakeLists.txt
- Ensure proper linking and include paths

### Documentation
- Update Python documentation
- Add type hints for better IDE support
- Create usage examples for new bindings

## Success Criteria

### Technical Requirements
- [ ] All core neuroscience classes exposed to Python
- [ ] Memory systems fully functional
- [ ] Neuromodulation systems integrated
- [ ] Advanced cognitive systems available
- [ ] Performance comparable to native C++

### User Experience Requirements
- [ ] Intuitive Python API design
- [ ] Comprehensive documentation
- [ ] Working examples and tutorials
- [ ] Robust error handling
- [ ] Performance benchmarking tools

### Testing Requirements
- [ ] 95%+ unit test coverage
- [ ] Comprehensive integration tests
- [ ] Performance benchmarks
- [ ] Cross-platform compatibility

## Risk Mitigation

### Technical Risks
- **Binding Complexity**: Use modern C++11/14/17 features
- **Memory Safety**: Use smart pointers and proper ownership
- **Performance**: Profile and optimize critical paths

### Timeline Risks
- **Scope Creep**: Prioritize high-impact features first
- **Testing Delays**: Parallel implementation and testing
- **Documentation Gaps**: Use automated documentation generation

## Conclusion

This comprehensive binding enhancement will provide NLM users with access to the full power of the neural simulation framework through an intuitive Python interface. The implementation will bridge the gap between the powerful C++ core and end users, researchers, and developers, enabling:

- **Research**: Advanced neuroscience modeling and experimentation
- **Education**: Neural computation learning and demonstration
- **Development**: Custom neural applications and tools
- **Discovery**: Novel neural architectures and learning algorithms

The enhanced Python API will make NLM accessible to a much broader audience while maintaining the computational power and flexibility of the original C++ implementation.