# NLM API Reference

## Core Classes and Interfaces

### `nlm::Brain`
**File**: `brain/Brain.hpp`

Represents the central brain system containing neurons, regions, and processing capabilities.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Brain(const std::shared_ptr<Config>& config)` | Constructor | `explicit Brain(const std::shared_ptr<Config>& config)` |
| `initialize()` | Initialize brain with neurons and regions | `bool initialize()` |
| `step(SimulationStep step, double time)` | Process one simulation step | `void step(SimulationStep step, double time)` |
| `reset()` | Reset brain to initial state | `void reset()` |
| `getTotalNeuronCount()` | Get total number of neurons | `size_t getTotalNeuronCount() const` |
| `getTotalSpikeCount()` | Get total spike count | `size_t getTotalSpikeCount() const` |
| `getFiringNeuronCount()` | Get number of firing neurons | `size_t getFiringNeuronCount() const` |
| `getAverageFiringRate()` | Get average firing rate | `float getAverageFiringRate() const` |
| `getTotalSynapseCount()` | Get total synapse count | `size_t getTotalSynapseCount() const` |
| `getRegion(RegionId id)` | Get brain region | `NeuralRegion* getRegion(RegionId id)` |
| `getSpikeSystem()` | Get spike system | `SpikeSystem* getSpikeSystem()` |
| `injectCurrentToNeurons(NeuronType type, float current)` | Inject current to neuron type | `void injectCurrentToNeurons(NeuronType type, float current)` |
| `logStatus()` | Log brain status information | `void logStatus() const` |

#### Constants

```cpp
namespace nlm {
    using NeuronId = uint64_t;
    using RegionId = uint64_t;
    using SimulationStep = uint64_t;
}
```

### `nlm::Neuron`
**File**: `brain/Neuron.hpp`

LIF (Leaky Integrate-and-Fire) neuron model.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Neuron(const std::shared_ptr<Config>& config)` | Constructor | `Neuron(const std::shared_ptr<Config>& config)` |
| `~Neuron()` | Destructor | `~Neuron()` |
| `initialize()` | Initialize neuron state | `void initialize()` |
| `update(double time)` | Update membrane potential | `void update(double time)` |
| `injectCurrent(float current)` | Inject external current | `void injectCurrent(float current)` |
| `getMembranePotential()` | Get membrane potential | `float getMembranePotential() const` |
| `getThreshold()` | Get firing threshold | `float getThreshold() const` |
| `getRefractoryPeriod()` | Get refractory period | `float getRefractoryPeriod() const` |
| `getNeuronType()` | Get neuron type | `NeuronType getNeuronType() const` |
| `isFiring()` | Check if neuron is firing | `bool isFiring() const` |
| `reset()` | Reset neuron state | `void reset()` |
| `getSpikeCount()` | Get spike count | `size_t getSpikeCount() const` |
| `getLastSpikeTime()` | Get last spike time | `double getLastSpikeTime() const` |

#### Neuron Types

```cpp
enum class NeuronType {
    EXCITATORY,
    INHIBITORY,
    SENSORY,
    MOTOR,
    ASSOCIATION
};
```

### `nlm::Synapse`
**File**: `brain/Synapse.hpp`

Synaptic connection between neurons.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Synapse(NeuronId pre, NeuronId post)` | Constructor | `Synapse(NeuronId pre, NeuronId post)` |
| `update(double time, float preSpikeTime)` | Update synapse state | `void update(double time, float preSpikeTime)` |
| `getWeight()` | Get current weight | `float getWeight() const` |
| `setWeight(float weight)` | Set weight | `void setWeight(float weight)` |
| `enablePlasticity(bool hebbian, bool stdp, bool structural)` | Enable plasticity rules | `void enablePlasticity(bool hebbian, bool stdp, bool structural)` |
| `getDelay()` | Get transmission delay | `float getDelay() const` |
| `getLastPostSpikeTime()` | Get last post-synaptic spike time | `double getLastPostSpikeTime() const` |
| `reset()` | Reset synapse state | `void reset()` |

### `nlm::Memory`
**File**: `memory/Memory.hpp`

Abstract base class for all memory systems.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Memory(const std::shared_ptr<Brain>& brain)` | Constructor | `Memory(const std::shared_ptr<Brain>& brain)` |
| `~Memory()` | Destructor | `virtual ~Memory() = default` |
| `initialize()` | Initialize memory system | `virtual void initialize() = 0` |
| `update(double time)` | Update memory state | `virtual void update(double time) = 0` |
| `store(Experience experience)` | Store new experience | `virtual void store(const Experience& experience) = 0` |
| `retrieve(Query query)` | Retrieve information | `virtual std::vector<Experience> retrieve(const Query& query) = 0` |
| `consolidate()` | Consolidate memories | `virtual void consolidate() = 0` |

### `nlm::Reward`
**File**: `neuromodulation/Reward.hpp`

Reward prediction and processing system.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Reward(const std::shared_ptr<Brain>& brain)` | Constructor | `Reward(const std::shared_ptr<Brain>& brain)` |
| `update(double time, float reward)` | Update with reward signal | `void update(double time, float reward)` |
| `getCurrentPrediction()` | Get current reward prediction | `float getCurrentPrediction() const` |
| `getPredictionError()` | Get prediction error | `float getPredictionError() const` |
| `reset()` | Reset prediction | `void reset()` |

### `nlm::PredictionError`
**File**: `neuromodulation/PredictionError.hpp`

Prediction error signaling system.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `PredictionError(const std::shared_ptr<Brain>& brain)` | Constructor | `PredictionError(const std::shared_ptr<Brain>& brain)` |
| `update(double time, float prediction, float actual)` | Update prediction error | `void update(double time, float prediction, float actual)` |
| `getError()` | Get prediction error | `float getError() const` |
| `reset()` | Reset error | `void reset()` |

### `nlm::Curiosity`
**File**: `neuromodulation/Curiosity.hpp`

Curiosity-driven exploration system.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Curiosity(const std::shared_ptr<Brain>& brain)` | Constructor | `Curiosity(const std::shared_ptr<Brain>& brain)` |
| `update(double time, float novelty)` | Update curiosity level | `void update(double time, float novelty)` |
| `getCuriosityLevel()` | Get current curiosity level | `float getCuriosityLevel() const` |
| `shouldExplore()` | Check if should explore | `bool shouldExplore() const` |
| `reset()` | Reset curiosity | `void reset()` |

## Configuration System

### `nlm::Config`
**File**: `core/Config/Config.hpp`

Central configuration management system.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Config()` | Default constructor | `Config()` |
| `loadFromFile(const std::string& filename)` | Load from file | `bool loadFromFile(const std::string& filename)` |
| `loadFromArgs(int argc, char** argv)` | Load from command line | `void loadFromArgs(int argc, char** argv)` |
| `getOr<T>(const std::string& key, T defaultValue)` | Get value with default | `template<typename T> T getOr(const std::string& key, T defaultValue)` |
| `set(const std::string& key, const T& value, ConfigSource source)` | Set value | `template<typename T> void set(const std::string& key, const T& value, ConfigSource source)` |
| `get<int64_t>(const std::string& key)` | Get int64_t | `int64_t get<int64_t>(const std::string& key) const` |
| `get<double>(const std::string& key)` | Get double | `double get<double>(const std::string& key) const` |
| `get<float>(const std::string& key)` | Get float | `float get<float>(const std::string& key) const` |
| `get<std::string>(const std::string& key)` | Get string | `std::string get<std::string>(const std::string& key) const` |

#### Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducibility |
| `simulation_timestep` | double | 0.001 | Simulation time step in seconds |
| `neuron_count` | int64_t | 500 | Number of neurons |
| `region_count` | int64_t | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of neuron connections |
| `stdp_ltp_weight` | float | 0.02 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.015 | STDP LTD weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Structural plasticity rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |

## Environment System

### `nlm::Environment`
**File**: `environment/Environment.hpp`

Abstract environment interface.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Environment()` | Constructor | `Environment()` |
| `~Environment()` | Destructor | `~Environment()` |
| `initialize()` | Initialize environment | `virtual void initialize() = 0` |
| `update(double time)` | Update environment | `virtual void update(double time) = 0` |
| `getSensoryPercept()` | Get sensory perception | `virtual Percept getSensoryPercept() const = 0` |
| `applyMotorCommand(Action action, double time)` | Apply motor command | `virtual void applyMotorCommand(const Action& action, double time) = 0` |
| `reset()` | Reset environment | `virtual void reset() = 0` |
| `getSimulationTime()` | Get simulation time | `double getSimulationTime() const` |

### `nlm::Percept`
**File**: `environment/Observation.hpp`

Sensory observation from environment.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Percept()` | Default constructor | `Percept()` |
| `Percept(const std::vector<float>& data)` | Data constructor | `explicit Percept(const std::vector<float>& data)` |
| `getData()` | Get sensory data | `const std::vector<float>& getData() const` |
| `getFeature(int index)` | Get feature value | `float getFeature(int index) const` |
| `size()` | Get number of features | `size_t size() const` |

### `nlm::Action`
**File**: `environment/Action.hpp`

Action to be performed in environment.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Action()` | Default constructor | `Action()` |
| `Action(ActionType type, const std::vector<float>& parameters)` | Constructor | `Action(ActionType type, const std::vector<float>& parameters)` |
| `getType()` | Get action type | `ActionType getType() const` |
| `getParameters()` | Get action parameters | `const std::vector<float>& getParameters() const` |
| `getParameter(int index)` | Get parameter value | `float getParameter(int index) const` |

#### Action Types

```cpp
enum class ActionType {
    NONE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    GRAB,
    RELEASE,
    CUSTOM
};
```

## Agent System

### `nlm::AgentBrain`
**File**: `agent/AgentBrain.hpp`

Interface between brain and environment.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `AgentBrain(const std::shared_ptr<Brain>& brain)` | Constructor | `AgentBrain(const std::shared_ptr<Brain>& brain)` |
| `initialize(const std::shared_ptr<Environment>& env)` | Initialize agent | `void initialize(const std::shared_ptr<Environment>& env)` |
| `processSensoryInput(const Percept& percept)` | Process sensory input | `void processSensoryInput(const Percept& percept)` |
| `decodeMotorCommand()` | Decode motor command | `Action decodeMotorCommand()` |
| `applyRewardModulation(float reward, float predictedReward)` | Apply reward modulation | `void applyRewardModulation(float reward, float predictedReward)` |
| `enableRewardModulation(bool enabled)` | Enable reward modulation | `void enableRewardModulation(bool enabled)` |
| `enableStructuralPlasticity(bool enabled)` | Enable structural plasticity | `void enableStructuralPlasticity(bool enabled)` |
| `enableDevelopment(bool enabled)` | Enable development | `void enableDevelopment(bool enabled)` |
| `enableCuriosity(bool enabled)` | Enable curiosity | `void enableCuriosity(bool enabled)` |

### `nlm::SensoryPercept`
**File**: `agent/SensoryPercept.hpp`

Processed sensory perception for agent.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `SensoryPercept()` | Default constructor | `SensoryPercept()` |
| `addFeature(float value)` | Add sensory feature | `void addFeature(float value)` |
| `getFeatures()` | Get all features | `const std::vector<float>& getFeatures() const` |
| `getFeature(int index)` | Get feature value | `float getFeature(int index) const` |
| `size()` | Get feature count | `size_t size() const` |
| `reset()` | Reset perception | `void reset()` |

## Python Bindings

The core API is exposed through Python bindings in the `pynlm` module.

### Python Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `createBrain(config)` | Create a brain | `config` - Configuration object |
| `createDefaultConfig()` | Create default config | None |
| `createSimpleWorld()` | Create simple world | None |
| `createAgentBrain(brain)` | Create agent interface | `brain` - Brain object |

### Python Class Methods

| Class | Method | Description | Parameters |
|-------|--------|-------------|------------|
| `Brain` | `initialize()` | Initialize brain | None |
| `Brain` | `step(step_number)` | Process one step | `step_number` - Step index |
| `Brain` | `getTotalNeuronCount()` | Get neuron count | None |
| `Brain` | `getTotalSpikeCount()` | Get spike count | None |
| `Brain` | `getAverageFiringRate()` | Get firing rate | None |
| `Brain` | `getDevelopmentalStage()` | Get development stage | None |
| `World` | `configure(width, height, visionWidth, visionHeight)` | Configure world | Size parameters |
| `World` | `reset()` | Reset world | None |
| `World` | `update(time)` | Update world | `time` - Time step |
| `World` | `getSensoryPercept()` | Get perception | None |
| `World` | `applyMotorCommand(action, time)` | Apply action | `action` - Action, `time` - Time |
| `Agent` | `initialize(world)` | Initialize agent | `world` - World object |
| `Agent` | `processSensoryInput(percept)` | Process input | `percept` - Perception |
| `Agent` | `decodeMotorCommand()` | Get action | None |
| `Agent` | `enableRewardModulation(enabled)` | Enable learning | `enabled` - Boolean |
| `Agent` | `enableCuriosity(enabled)` | Enable exploration | `enabled` - Boolean |

## Utility Classes

### `nlm::SimulationClock`
**File**: `core/SimulationClock/SimulationClock.hpp`

Manages simulation timing.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `SimulationClock(double timestep)` | Constructor | `explicit SimulationClock(double timestep)` |
| `update()` | Advance time | `void update()` |
| `getTime()` | Get current time | `double getTime() const` |
| `getTimestep()` | Get time step | `double getTimestep() const` |
| `reset()` | Reset to zero | `void reset()` |

### `nlm::Logger`
**File**: `core/Logger/Logger.hpp`

Logging system.

#### Public Methods

| Method | Description | Signature |
|--------|-------------|-----------|
| `Logger()` | Constructor | `Logger()` |
| `addLogger(std::shared_ptr<LoggerInterface> logger)` | Add logger | `void addLogger(std::shared_ptr<LoggerInterface> logger)` |
| `logInfo(const std::string& message)` | Log info | `void logInfo(const std::string& message)` |
| `logWarning(const std::string& message)` | Log warning | `void logWarning(const std::string& message)` |
| `logError(const std::string& message)` | Log error | `void logError(const std::string& message)` |

#### Log Levels

```cpp
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};
```

## Constants and Types

### Core Types

```cpp
typedef uint64_t NeuronId;
typedef uint64_t RegionId;
typedef uint64_t SimulationStep;
typedef std::vector<float> PerceptData;
typedef std::vector<float> ActionParameters;
```

### Mathematical Constants

```cpp
constexpr float PI = 3.141592653589793f;
constexpr float E = 2.718281828459045f;
constexpr float GOLDEN_RATIO = 1.618033988749895f;
```

## Error Codes

```cpp
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_CONFIG = 1,
    INITIALIZATION_FAILED = 2,
    INVALID_PARAMETERS = 3,
    MEMORY_ALLOCATION_FAILED = 4,
    FILE_NOT_FOUND = 5,
    UNKNOWN_ERROR = 6
};
```

## Version Information

```cpp
namespace nlm {
    constexpr const char* VERSION = "0.1.0";
    constexpr const char* BUILD_DATE = "2024-01-01";
    constexpr const char* COMMIT_HASH = "abcd1234";
}
```
