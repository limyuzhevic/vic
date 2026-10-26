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

## Code Quality Improvements

### 1. Documentation Standards

All public classes and methods follow consistent documentation:

```cpp
class NeuralRegion {
    /**
     * Add a new neural region with specified name
     * 
     * @param name Name of the region (empty for auto-generated)
     * @return RegionId of the newly created region
     * @throws std::invalid_argument if name is invalid
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Get region by ID
     * 
     * @param id Region identifier to look up
     * @return Pointer to region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    
    // ... other methods
};
```

### 2. Error Handling Patterns

Consistent error handling throughout the codebase:

```cpp
bool Brain::initialize() {
    // Validate configuration
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Invalid configuration for brain initialization");
        return false;
    }
    
    // Safe initialization with proper error handling
    try {
        // Initialization code
        if (!region->initializeRandomConnectivity(...)) {
            NLM_LOG_ERROR("Failed to initialize connectivity in region " + std::to_string(i));
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    }
    
    return true;
}
```

### 3. Memory Management Best Practices

Proper resource management throughout the codebase:

```cpp
class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    
    // Use unique_ptr for ownership transfer
    virtual std::unique_ptr<SensoryInput> clone() const = 0;
    
    // Delete copy operations for value types
    SensoryInput(const SensoryInput&) = delete;
    SensoryInput& operator=(const SensoryInput&) = delete;
    
    // Allow move operations
    SensoryInput(SensoryInput&&) noexcept = default;
    SensoryInput& operator=(SensoryInput&&) noexcept = default;
};
```

### 4. Configuration Management

Robust configuration system with validation:

```cpp
class Config {
public:
    /**
     * Load configuration from file with validation
     * 
     * @param filepath Path to configuration file
     * @return true if successful, false on error
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Set configuration value with type safety
     * 
     * @tparam T Type of value (int, float, std::string, bool)
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of configuration
     */
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source);
    
    /**
     * Get configuration value with validation
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return Optional value (nullopt if not found or type mismatch)
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
};
```

### 5. Testing Integration

Comprehensive testing framework with clear organization:

```bash
tests/
├── test_brain.cpp          # Core brain functionality
├── test_neuron.cpp         # Neuron dynamics
├── test_synapse.cpp        # Synaptic connections
├── test_stdp.cpp          # STDP learning
├── test_config.cpp         # Configuration management
├── test_clock.cpp          # Time management
├── test_random.cpp         # Random number generation
└── test_main.cpp           # Main functionality tests
```

### 6. Build System Improvements

Modern CMake configuration with clear targets:

```cmake
# Core libraries (shared by all targets)
add_library(nlm_core STATIC src/brain/Neuron.cpp ...)

# Separate libraries for different components
add_library(nlm_agent STATIC src/agent/AgentBrain.cpp ...)
add_library(nlm_world STATIC src/world/SimpleWorld.cpp ...)

# Demo applications
add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world)

# Python bindings
pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
```

## Performance Considerations

### 1. Memory Efficiency

Use of smart pointers and efficient data structures:

```cpp
// Use move semantics where appropriate
std::vector<std::unique_ptr<Neuron>> neurons;
neurons.emplace_back(std::make_unique<Neuron>(id));

// Avoid unnecessary copies
Synapse connection = std::move(other_connection);
```

### 2. Cache Locality

Data-oriented design for better cache performance:

```cpp
// Process neurons in batches for cache efficiency
for (size_t i = 0; i < batch_size; ++i) {
    neurons[i]->stepLIF(time, dt);
}

// Use contiguous containers where possible
std::vector<float> membranePotentials(neuronCount);
```

### 3. Parallel Processing Opportunities

Identify areas where parallel processing can be applied:

```cpp
// Event processing can be parallelized
void SpikeSystem::processSpikes(SimulationStep step) {
    // Process spike events for different neurons
    // Can use SIMD or parallel processing
    for (auto& event : pendingSpikes_) {
        processSpikeEvent(event);
    }
}
```

## Future Extensibility

### 1. Adding New Components

Clear patterns for extending the system:

```cpp
// New plasticity rule
class NewPlasticityRule : public PlasticityRule {
public:
    void update(Synapse* syn, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom learning rule implementation
    }
};

// Add to Brain::Impl
plasticityRules.push_back(std::make_unique<NewPlasticityRule>());
```

### 2. New Sensory Modalities

Easy addition of new sensory systems:

```cpp
class TemperatureInput : public SensoryInput {
public:
    TemperatureInput() : SensoryInput() {
        setType("temperature");
    }
    
    void setTemperature(float temp) {
        data_.push_back(temp);
    }
    
    float getTemperature() const {
        return data_.empty() ? 0.0f : data_[0];
    }
};
```

## Documentation Quality Metrics

| Metric | Current Status | Target |
|--------|----------------|--------|
| Inline Comments | Good | Excellent |
| Header Documentation | Good | Complete |
| Code Examples | Limited | Comprehensive |
| API Documentation | Basic | Detailed |
| Tutorial Materials | None | Complete |

## Maintenance Recommendations

### 1. Code Review Process

- Review pull requests for code quality
- Ensure consistent documentation standards
- Validate error handling patterns
- Check for performance bottlenecks

### 2. Testing Strategy

- Unit tests for all critical components
- Integration tests for system functionality
- Performance benchmarks for critical paths
- Memory leak detection and analysis

### 3. Documentation Updates

- Keep API documentation in sync with code changes
- Update tutorials and examples regularly
- Maintain changelog for significant changes
- Provide migration guides for breaking changes

## Conclusion

The NLM codebase has been improved with:

1. **Better error handling and validation** throughout all components
2. **Comprehensive documentation** following consistent standards
3. **Modern CMake configuration** with clear separation of concerns
4. **Robust Python bindings** with extensive functionality
5. **Advanced features for power users** including memory management and simulation control
6. **Enhanced code organization** with clear patterns for extensibility

These improvements make NLM more robust, maintainable, and easier to use while preserving its core functionality as a biologically-inspired neural network simulator.

---

## License

MIT

## Authors

Research project — See docs for scientific background.