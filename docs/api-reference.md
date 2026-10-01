# NLM API Reference

This document provides complete API documentation for all NLM classes, functions, and methods. It includes:

- **Class Hierarchies**: Complete inheritance diagrams
- **Method Signatures**: Detailed parameter and return value documentation
- **Usage Examples**: Practical code examples for each major component
- **Type Definitions**: All custom types and enumerations
- **Configuration Options**: Complete parameter lists for all configuration classes

## 🏗️ Module Structure

NLM is organized into the following main modules:

### Core Module (`core/`)
- **Config** - Configuration management
- **Random** - Deterministic random number generation
- **Logger** - Structured logging system
- **SimulationClock** - Time management
- **Types** - Strongly-typed identifiers

### Brain Module (`brain/`)
- **Brain** - Main neural processing system
- **Neuron** - Individual neuron model
- **Synapse** - Synaptic connections
- **NeuralPopulation** - Neuron groups
- **NeuralRegion** - Brain regions

### Dynamics Module (`dynamics/`)
- **NeuralDynamics** - Base class for neuron dynamics
- **LIFNeuron** - Leaky Integrate-and-Fire implementation
- **SpikeSystem** - Event-driven spike processing

### Plasticity Module (`plasticity/`)
- **PlasticityRule** - Base class for plasticity rules
- **STDP** - Spike-Timing-Dependent Plasticity
- **Hebbian** - Hebbian learning rule
- **StructuralPlasticity** - Synapse formation/removal

### Memory Module (`memory/`)
- **WorkingMemory** - Persistent activity for temporary storage
- **EpisodicMemory** - Experience encoding and replay
- **SemanticMemory** - Knowledge representation
- **AssociativeMemory** - Pattern associations

### Neuromodulation Module (`neuromodulation/`)
- **Neuromodulator** - Abstract base class
- **Dopamine** - Reward and reinforcement
- **Curiosity** - Exploration motivation
- **Novelty** - Novelty detection
- **PredictionError** - Error signals

### Development Module (`development/`)
- **DevelopmentSystem** - Orchestrates development stages
- **Synaptogenesis** - New synapse formation
- **Pruning** - Weak synapse removal
- **Maturation** - Adult property development

### Prediction Module (`prediction/`)
- **PredictionSystem** - Temporal sequence learning
- **NeuralPrediction** - Next state prediction
- **ActionConsequencePredictor** - Action effect prediction

### Cognitive Module (`cognition/`)
- **Attention** - Competitive selection (NOT Transformer)
- **NeuralPlanner** - Predictive action selection
- **ConceptFormation** - Pattern discovery
- **SelfModel** - Sensorimotor self-awareness

### Sensory/Motor Module
- **SensoryInput** - Base class for sensory data
- **Vision** - Visual input processing
- **MotorSystem** - Neural-to-action conversion

### Environment Module (`world/`)
- **Environment** - Abstract environment interface
- **SimpleWorld** - 2D grid-based environment

### Experiment Module (`experiments/`)
- **Experiment** - Single experiment recording
- **ExperimentRunner** - Experiment execution
- **Metrics** - Data analysis

## 📋 Configuration API

### Config Class (`core/Config.hpp`)

#### Creation and Basic Operations

```cpp
// Create default configuration
std::shared_ptr<Config> config = std::make_shared<Config>();

// Load from file
config->loadFromFile("config.json");

// Save to file
config->saveToFile("config.json");
```

#### Query Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `bool has(const std::string& key) const` | bool | Check if key exists |
| `std::vector<std::string> getKeys() const` | std::vector<std::string> | Get all top-level keys |
| `std::vector<std::string> getKeys(const std::string& prefix) const` | std::vector<std::string> | Get keys with given prefix |

#### Set/Get Methods

| Method | Type | Default | Description |
|--------|------|---------|-------------|
| `void set(const std::string& key, int value)` | void | - | Set integer value |
| `void set(const std::string& key, float value)` | void | - | Set float value |
| `void set(const std::string& key, bool value)` | void | - | Set boolean value |
| `void set(const std::string& key, const std::string& value)` | void | - | Set string value |
| `int getInt(const std::string& key) const` | int | - | Get integer value |
| `float getFloat(const std::string& key) const` | float | - | Get float value |
| `bool getBool(const std::string& key) const` | bool | - | Get boolean value |
| `std::string getString(const std::string& key) const` | std::string | - | Get string value |

#### Nested Access

```cpp
// Set nested values
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.learning_rate", 0.001);
config->set("neuromod.dopamine.scale", 1.0);

// All keys in a section
auto brain_keys = config->getKeys("brain");
```

### createDefaultConfig() (Python API)

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Check configuration
print(f"Neuron count: {config.getInt('brain.neuron_count')}")
print(f"Synapse density: {config.getFloat('brain.synapse_density')}")
```

## 🧠 Brain API

### Brain Class (`brain/Brain.hpp`)

#### Creation and Initialization

```cpp
// Create brain with configuration
std::shared_ptr<Brain> brain = std::make_shared<Brain>(config);

// Initialize
bool success = brain->initialize();
if (!success) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

#### Core Simulation

| Method | Signature | Description |
|--------|-----------|-------------|
| `void step(SimulationStep step)` | `void step(SimulationStep step, Timestamp time = 0)` | Run one simulation step |
| `void reset()` | void | Reset brain to initial state |
| `void receiveSensoryInput(const SensoryInput& input)` | void | Inject sensory data |
| `Action produceAction() const` | Action | Get motor action from neural activity |

#### Query Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `size_t getTotalNeuronCount() const` | size_t | Total neurons |
| `size_t getTotalSynapseCount() const` | size_t | Total synapses |
| `size_t getFiringNeuronCount() const` | size_t | Currently firing neurons |
| `float getAverageFiringRate() const` | float | Average firing rate (Hz) |
| `float getExcitationInhibitionRatio() const` | float | E/I balance ratio |
| `float getAveragePathLength() const` | float | Average connection path length |

#### State Management

```cpp
// Save checkpoint
bool save(const std::string& filepath) const;

// Load checkpoint  
bool load(const std::string& filepath);

// Development
DevelopmentalStage getDevelopmentalStage() const;
float getBrainAge() const;
```

#### Memory Access

```cpp
// Memory systems (Phase 4+)
WorkingMemory* getWorkingMemory() const;
EpisodicMemory* getEpisodicMemory() const;
SemanticMemory* getSemanticMemory() const;
ProceduralMemory* getProceduralMemory() const;
PredictionSystem* getPredictionSystem() const;
```

#### Regions

```cpp
// Neural regions
RegionId addRegion(const std::string& name);
NeuralRegion* getRegion(RegionId id);
std::vector<NeuralRegion*> getAllRegions() const;
```

### Python Brain API

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Core operations
brain.step(step)                    # Run one step
brain.reset()                       # Reset state

# Input/output
brain.receiveSensoryInput(input)   # Inject sensory data
action = brain.produceAction()    # Get motor action

# Statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

## 🧫 Neuron API

### Neuron Class (`brain/Neuron.hpp`)

#### Neuron Types

```cpp
// Neuron type enumerations
enum class NeuronType {
    Excitatory,
    Inhibitory, 
    Sensory,
    Motor,
    Modulatory,
    Internal
};
```

#### Core Properties

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void stepLIF(Timestamp time, float dt)` | void | Update LIF dynamics |
| `bool isFiring() const` | bool | Check if neuron is firing |
| `float getCurrentPotential() const` | float | Get membrane potential |
| `float getThreshold() const` | float | Get firing threshold |
| `void reset() const` | void | Reset neuron state |
| `NeuronType getType() const` | NeuronType | Get neuron type |
| `void setType(NeuronType type)` | void | Set neuron type |

#### Synaptic Properties

```cpp
// Synaptic connections
void addSynapse(std::shared_ptr<Synapse> synapse);
void removeSynapse(SynapseId id);
std::vector<SynapseId> getOutgoingSynapses() const;
std::vector<SynapseId> getIncomingSynapses() const;
```

## ⚡ Synapse API

### Synapse Class (`brain/Synapse.hpp`)

#### Core Properties

| Method | Return Type | Description |
|--------|-------------|-------------|
| `float getWeight() const` | float | Get synaptic weight |
| `void setWeight(float weight)` | void | Set synaptic weight |
| `Timestamp getDelay() const` | Timestamp | Get transmission delay |
| `bool isExcitatory() const` | bool | Check if excitatory |
| `bool isInhibitory() const` | bool | Check if inhibitory |
| `float getConductance() const` | float | Get synaptic conductance |

#### Plasticity

```cpp
// STDP plasticity
void applySTDP(Timestamp preSpikeTime, Timestamp postSpikeTime);

// Hebbian plasticity  
void applyHebbian(float preActivity, float postActivity);

// Structural plasticity
void applyStructuralPlasticity();  // Formation/pruning
```

## 💾 Memory API

### WorkingMemory Class (`memory/WorkingMemory.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void update(const std::vector<float>& input)` | void | Update memory with input |
| `std::vector<float> getContents() const` | std::vector<float> | Get memory contents |
| `void clear()` | void | Clear memory |
| `bool isActive() const` | bool | Check if memory is active |
| `void activate()` | void | Activate memory |
| `void deactivate()` | void | Deactivate memory |

#### Storage Properties

```cpp
// Persistent activity
float getAverageActivity() const;
float getPeakActivity() const;
Timestamp getLastUpdateTime() const;
```

### EpisodicMemory Class (`memory/NeuralEpisodicMemory.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void encode(const Experience& experience)` | void | Store experience |
| `std::vector<Experience> recall(TemporalRange timeRange) const` | std::vector<Experience> | Retrieve experiences |
| `void replay(TemporalRange timeRange)` | void | Replay experiences |
| `void consolidate()` | void | Consolidate important memories |
| `size_t getMemoryCount() const` | size_t | Get number of memories |

#### Experience Structure

```cpp
struct Experience {
    Timestamp timestamp;
    std::vector<float> sensoryInput;
    Action action;
    float reward;
    std::string context;
    std::vector<float> outcome;
};
```

## 🧠 Agent API

### AgentBrain Class (`agent/AgentBrain.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void initialize(std::shared_ptr<Environment> environment)` | void | Initialize with world |
| `void processSensoryInput(const SensoryPercept& percept)` | void | Process sensory input |
| `Action decodeMotorCommand() const` | Action | Decode motor command |
| `void applyRewardModulation(float reward, float predictedReward)` | void | Apply reward learning |
| `void updateDevelopment(float dt)` | void | Update developmental stage |

#### Subsystems

```cpp
// Enable/disable subsystems
void enableRewardModulation(bool enable);
void enableStructuralPlasticity(bool enable);
void enableDevelopment(bool enable);
void enableCuriosity(bool enable);
void enablePrediction(bool enable);
void enableAttention(bool enable);

// Get statistics
float getCuriosityLevel() const;
float getNoveltyLevel() const;
float getPredictionError() const;
float getNeuromodulationLevel() const;
DevelopmentalStage getDevelopmentalStage() const;
```

## 🌍 Environment API

### SimpleWorld Class (`world/SimpleWorld.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void configure(int width, int height, int visionWidth, int visionHeight)` | void | Configure world dimensions |
| `void reset()` | void | Reset to initial state |
| `void update(float dt)` | void | Update world dynamics |
| `SensoryPercept getSensoryPercept() const` | SensoryPercept | Get agent's perception |
| `AgentBody getAgentBody() const` | AgentBody | Get agent physical state |
| `void applyMotorCommand(const Action& action, Timestamp time)` | void | Apply action to world |

#### World Management

```cpp
// Object management
void addObject(const WorldObject& object);
void removeObject(float x, float y);
WorldObject* getObject(float x, float y);
std::vector<WorldObject> getAllObjects() const;

// Position validation
bool isValidPosition(float x, float y) const;

// Agent control
void setAgentStart(float x, float y);
float getAgentX() const;
float getAgentY() const;
```

## 🔮 Prediction API

### PredictionSystem Class (`prediction/PredictionSystem.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void train(const std::vector<float>& sequence)` | void | Train on sequence |
| `float predictNext(const std::vector<float>& context) const` | float | Predict next state |
| `float getPredictionError() const` | float | Get prediction error |
| `void update(float reward)` | void | Update with reward |
| `bool isReady() const` | bool | Check if system is trained |

#### State

```cpp
// History tracking
std::vector<std::vector<float>> getHistory() const;
float getConfidence() const;  // Prediction confidence
Timestamp getLastUpdateTime() const;
```

## 🧠 Cognitive API

### NeuralPlanner Class (`cognition/NeuralPlanner.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `Action selectAction(const std::vector<float>& state) const` | Action | Select best action |
| `void evaluateAction(const Action& action, float reward) const` | void | Evaluate action outcome |
| `std::vector<Action> getPossibleActions() const` | std::vector<Action> | Get all possible actions |
| `float getActionValue(const Action& action) const` | float | Get action value |

### ConceptFormation Class (`cognition/ConceptFormation.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void learnFromPattern(const std::vector<float>& pattern)` | void | Learn from pattern |
| `std::vector<float> getPrototype(const std::string& concept) const` | std::vector<float> | Get concept prototype |
| `bool recognizePattern(const std::vector<float>& pattern, float threshold) const` | bool | Recognize pattern |
| `std::vector<std::string> getAllConcepts() const` | std::vector<std::string> | Get all learned concepts |

## 🎨 Visualization API

### BrainVisualizer Class (`visualization/BrainVisualizer.hpp`)

#### Core Operations

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void generate(const std::string& filename, VisualizationFormat format)` | void | Generate visualization |
| `void update(float dt)` | void | Update visualization |
| `void render()` | void | Render visualization |
| `void enable(bool enable)` | void | Enable/disable visualization |
| `bool isEnabled() const` | bool | Check if enabled |

#### Export Formats

```cpp
enum class VisualizationFormat {
    PNG,
    JSON,
    CSV,
    HDF5,
    Parquet
};
```

## 🔧 Utility APIs

### PerformanceProfiler (`profiling/PerformanceProfiler.hpp`)

| Method | Return Type | Description |
|--------|-------------|-------------|
| `void startProfile(const std::string& name)` | void | Start profiling |
| `void stopProfile()` | void | Stop profiling |
| `float getAverageTime(const std::string& name) const` | float | Get average time |
| `float getMinTime(const std::string& name) const` | float | Get minimum time |
| `float getMaxTime(const std::string& name) const` | float | Get maximum time |

### DataAnalyzer (`analysis/DataAnalyzer.hpp`)

| Method | Return Type | Description |
|--------|-------------|-------------|
| `std::vector<float> computeStatistics(const std::vector<float>& data) const` | std::vector<float> | Compute statistics |
| `std::vector<float> detectAnomalies(const std::vector<float>& data, float threshold) const` | std::vector<float> | Detect anomalies |
| `std::vector<float> extractFeatures(const std::vector<float>& data) const` | std::vector<float> | Extract features |

### PluginSystem (`plugins/PluginSystem.hpp`)

| Method | Return Type | Description |
|--------|-------------|-------------|
| `bool loadPlugin(const std::string& path)` | bool | Load plugin from file |
| `bool unloadPlugin(const std::string& name)` | bool | Unload plugin |
| `std::vector<std::string> getLoadedPlugins() const` | std::vector<std::string> | Get loaded plugins |
| `void registerPlugin(std::shared_ptr<Plugin> plugin)` | void | Register plugin |
| `std::shared_ptr<Plugin> getPlugin(const std::string& name) const` | std::shared_ptr<Plugin> | Get plugin |

## 🎯 Enumerations

### NeuronType
```cpp
enum class NeuronType {
    Excitatory,    // Positive feedback
    Inhibitory,    // Negative feedback
    Sensory,       // Receive external input
    Motor,         // Send output signals
    Modulatory,    // Regulate other neurons
    Internal       // Internal processing
};
```

### DevelopmentalStage
```cpp
enum class DevelopmentalStage {
    Initial,       // Newborn, high plasticity
    CriticalPeriod, // Sensitive learning window
    Maturation,    // Developing skills
    Adult,         // Full capabilities
    Aging          // Declining performance
};
```

### ActionType
```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    RotateCW,
    RotateCCW,
    Interact,
    Eat,
    Sleep,
    Explore,
    Reproduce
};
```

### WorldObjectType
```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker,
    Food,
    Shelter,
    Companion
};
```

### VisualizationFormat
```cpp
enum class VisualizationFormat {
    PNG,
    JSON,
    CSV,
    HDF5,
    Parquet
};
```

## 📊 Type Definitions

### Core Types

| Type | Description |
|------|-------------|
| `NeuronId` | Unique neuron identifier |
| `SynapseId` | Unique synapse identifier |
| `RegionId` | Unique region identifier |
| `PopulationId` | Unique population identifier |
| `SimulationStep` | Simulation step counter |
| `Timestamp` | Time in milliseconds |
| `Potential` | Membrane potential (mV) |
| `Weight` | Synaptic weight |

### Smart Pointers

```cpp
using NeuronPtr = std::shared_ptr<Neuron>;
using SynapsePtr = std::shared_ptr<Synapse>;
using NeuralPopulationPtr = std::shared_ptr<NeuralPopulation>;
using NeuralRegionPtr = std::shared_ptr<NeuralRegion>;
using ConfigPtr = std::shared_ptr<Config>;
```

## 🚀 Python-Specific Functions

### Core Python Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain instance |
| `createAgentBrain(brain)` | Create agent interface |
| `createSimpleWorld()` | Create simple 2D environment |
| `createVisualizationAPI()` | Create visualization interface |
| `__version__` | NLM version string |

### Brain Methods (Python)

| Method | Description |
|----------|-------------|
| `brain.step(step)` | Run simulation step |
| `brain.initialize()` | Initialize brain |
| `brain.reset()` | Reset brain state |
| `brain.receiveSensoryInput(input)` | Inject sensory data |
| `brain.produceAction()` | Get motor action |
| `brain.getTotalNeuronCount()` | Get total neurons |
| `brain.getFiringNeuronCount()` | Get firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.save(filename)` | Save brain state |
| `brain.load(filename)` | Load brain state |

### Agent Methods (Python)

| Method | Description |
|----------|-------------|
| `agent.initialize(world)` | Initialize with world |
| `agent.processSensoryInput(percept)` | Process sensory input |
| `agent.decodeMotorCommand()` | Decode motor command |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward learning |
| `agent.updateDevelopment(dt)` | Update development |
| `agent.enableRewardModulation(bool)` | Enable reward modulation |
| `agent.enableCuriosity(bool)` | Enable curiosity |
| `agent.getCuriosityLevel()` | Get curiosity level |
| `agent.getNoveltyLevel()` | Get novelty level |

## 🔍 Advanced API Usage

### Batch Processing

```cpp
// Process multiple steps efficiently
for (SimulationStep step = 0; step < total_steps; ++step) {
    brain->step(step);
    
    // Periodic statistics
    if (step % 100 == 0) {
        auto stats = brain->getStatistics();
        logStatistics(stats);
    }
    
    // Periodic saving
    if (step % 1000 == 0) {
        brain->save(f"checkpoint_{step}.bin");
    }
}
```

### Memory-Efficient Processing

```cpp
// Process with memory management
std::vector<float> processInChunks(
    const std::vector<float>& data, 
    size_t chunk_size
) {
    std::vector<float> results;
    results.reserve(data.size());
    
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, data.size());
        std::vector<float> chunk(data.begin() + i, data.begin() + end);
        
        // Process chunk
        auto processed = brain->processChunk(chunk);
        results.insert(results.end(), processed.begin(), processed.end());
        
        // Clear intermediate results to free memory
        processed.clear();
    }
    
    return results;
}
```

### Error Handling

```cpp
// Robust error handling
bool safeBrainOperation(
    std::shared_ptr<Brain> brain,
    std::function<void()> operation
) {
    try {
        // Check brain state
        if (!brain || !brain->isInitialized()) {
            std::cerr << "Brain not properly initialized!" << std::endl;
            return false;
        }
        
        // Perform operation
        operation();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during brain operation: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error during brain operation!" << std::endl;
        return false;
    }
}
```

## 📚 API Reference Summary

This API reference covers all NLM classes, methods, and functions. For specific details:

1. **Use the search function** to find specific classes or methods
2. **Check the examples** for practical usage patterns
3. **Review the tutorials** for step-by-step guidance
4. **Consult the configuration guide** for parameter documentation
5. **Check best practices** for recommended usage patterns

## 🔗 Cross-References

- **Configuration API**: See `docs/configuration.md`
- **Best Practices**: See `docs/best-practices.md`
- **Tutorial Usage**: See `docs/tutorials/`
- **Examples**: See `examples/`
- **Design Decisions**: See `DESIGN_DECISIONS.md`

## 📝 API Evolution

The NLM API evolves through Phase 6 integration. Key changes:

- **Phase 4+**: Cognitive systems become fully integrated
- **Phase 5+**: Performance infrastructure is integrated
- **Phase 6+**: All systems work together as coherent brain

## 🛡️ API Stability

This API is designed with:

- **Backward Compatibility**: Existing code continues to work
- **Forward Compatibility**: New features added with versioning
- **Extensibility**: Clear interfaces for adding new components
- **Modularity**: Components can be used independently or together

## 📊 Performance Notes

API usage considerations:

1. **Synchronous Operations**: Most operations are synchronous
2. **Memory Management**: Use smart pointers for automatic cleanup
3. **Threading**: Not thread-safe - use external synchronization
4. **Real-time**: Use batch operations for better performance
5. **Profiling**: Use PerformanceProfiler for optimization

This completes the NLM API Reference. All classes, methods, and functions are documented with examples and usage patterns for both C++ and Python APIs.