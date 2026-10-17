# NLM - API Reference Documentation

## Overview

This document provides comprehensive API reference for NLM, including detailed information about classes, methods, functions, parameters, return values, and usage examples. The API is organized by functional areas and follows consistent documentation standards.

## API Structure

### Core API Modules

1. **Brain Module** - Core neural processing
2. **Agent Module** - Brain-environment interaction
3. **World Module** - Environment simulation
4. **Config Module** - Configuration management
5. **Memory Module** - Memory systems
6. **Neuromodulation Module** - Learning mechanisms
7. **Development Module** - Brain development
8. **Prediction Module** - Forward models
9. **Plasticity Module** - Synaptic changes
10. **Utility Module** - Helper functions

### API Documentation Standards

#### Method Signature Format

```cpp
typename ClassName::methodName(parameters) const [optional modifiers]

// Example
std::vector<NeuronId> Brain::getConnectedNeurons(NeuronId source) const noexcept
```

#### Parameter Format

```cpp
parameter_name : type [optional modifiers] - Description of parameter
```

#### Return Value Format

```cpp
Returns: type - Description of return value
```

#### Exception Format

```cpp
Throws: exception_type - Description of when exception is thrown
```

#### Example Format

```cpp
void Brain::exampleMethod(int parameter1, std::string parameter2) {
    // Implementation
}

// Example usage:
Brain brain(config);
brain.initialize();
brain.exampleMethod(42, "example");
```

## Brain Module

### Class: Brain

#### Constructor/Destructor

```cpp
Brain(const Config& config) noexcept(false)
Brain(const Brain&) = delete
Brain& operator=(const Brain&) = delete
~Brain() noexcept
```

**Constructor Parameters:**
- `config`: Configuration object - Brain configuration

**Constructor Exceptions:**
- `std::invalid_argument`: If config is invalid
- `std::runtime_error`: If brain initialization fails

**Destructor:**
- No throw guarantee - cleans up resources safely

#### Core Methods

##### Initialization and Configuration

```cpp
void Brain::initialize() noexcept(false)

void Brain::initialize(const std::string& checkpoint_path) noexcept(false)

void Brain::reset() noexcept

void Brain::setConfig(const Config& config) noexcept

Config Brain::getConfig() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize()` | None | void | Initialize brain with current configuration |
| `initialize(checkpoint_path)` | `checkpoint_path`: string | void | Initialize brain from checkpoint |
| `reset()` | None | void | Reset brain to initial state |
| `setConfig(config)` | `config`: Config | void | Update brain configuration |
| `getConfig()` | None | Config | Get current brain configuration |

**Method Exceptions:**
- `initialize()`: Throws if initialization fails
- `initialize(checkpoint_path)`: Throws if checkpoint loading fails

##### Simulation Control

```cpp
void Brain::step(int step_number) noexcept

void Brain::step(int step_number, double time) noexcept

void Brain::step(const std::vector<SpikeEvent>& external_spikes) noexcept

void Brain::step(int step_number, double time, 
                const std::vector<SpikeEvent>& external_spikes) noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `step(step_number)` | `step_number`: int | void | Run simulation step with given number |
| `step(step_number, time)` | `step_number`: int, `time`: double | void | Run simulation step with timestamp |
| `step(external_spikes)` | `external_spikes`: vector<SpikeEvent> | void | Run simulation with external spikes |
| `step(step_number, time, external_spikes)` | All parameters | void | Complete step with all inputs |

##### Input/Output Processing

```cpp
void Brain::receiveSensoryInput(const std::vector<double>& sensory_data) noexcept

double Brain::produceAction() noexcept

std::vector<double> Brain::getSensoryOutput() const noexcept

const std::vector<double>& Brain::getMotorOutput() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `receiveSensoryInput(sensory_data)` | `sensory_data`: vector<double> | void | Process sensory input data |
| `produceAction()` | None | double | Generate motor command output |
| `getSensoryOutput()` | None | vector<double> | Get processed sensory output |
| `getMotorOutput()` | None | const vector<double>& | Get motor output vector |

##### Memory Management

```cpp
void Brain::save(const std::string& filename) const noexcept

void Brain::load(const std::string& filename) noexcept

void Brain::saveCheckpoint(const std::string& prefix, int step) const noexcept

bool Brain::loadCheckpoint(const std::string& prefix, int step) noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `save(filename)` | `filename`: string | void | Save brain state to file |
| `load(filename)` | `filename`: string | void | Load brain state from file |
| `saveCheckpoint(prefix, step)` | `prefix`: string, `step`: int | void | Save checkpoint with step number |
| `loadCheckpoint(prefix, step)` | `prefix`: string, `step`: int | bool | Load checkpoint, returns success |

##### Neuron and Synapse Management

```cpp
NeuronId Brain::addNeuron(const NeuronConfig& neuron_config) noexcept

bool Brain::removeNeuron(NeuronId neuron_id) noexcept

NeuronId Brain::addSynapse(NeuronId source, NeuronId target, 
                          const SynapseConfig& synapse_config) noexcept

bool Brain::removeSynapse(NeuronId source, NeuronId target) noexcept

std::vector<NeuronId> Brain::getConnectedNeurons(NeuronId neuron_id) const noexcept

std::vector<std::pair<NeuronId, double>> Brain::getSynapseWeights(NeuronId neuron_id) const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `addNeuron(neuron_config)` | `neuron_config`: NeuronConfig | NeuronId | Add new neuron |
| `removeNeuron(neuron_id)` | `neuron_id`: NeuronId | bool | Remove neuron, returns success |
| `addSynapse(source, target, synapse_config)` | `source`: NeuronId, `target`: NeuronId, `synapse_config`: SynapseConfig | NeuronId | Add new synapse |
| `removeSynapse(source, target)` | `source`: NeuronId, `target`: NeuronId | bool | Remove synapse, returns success |
| `getConnectedNeurons(neuron_id)` | `neuron_id`: NeuronId | vector<NeuronId> | Get neurons connected to source |
| `getSynapseWeights(neuron_id)` | `neuron_id`: NeuronId | vector<pair<NeuronId, double>> | Get connected neurons and weights |

##### Statistics and Metrics

```cpp
double Brain::getTotalNeuronCount() const noexcept

double Brain::getTotalSynapseCount() const noexcept

double Brain::getFiringNeuronCount() const noexcept

double Brain::getAverageFiringRate() const noexcept

double Brain::getExcitationInhibitionRatio() const noexcept

double Brain::getMembranePotentialMean() const noexcept

double Brain::getMembranePotentialStd() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getTotalNeuronCount()` | None | double | Total number of neurons |
| `getTotalSynapseCount()` | None | double | Total number of synapses |
| `getFiringNeuronCount()` | None | double | Number of currently firing neurons |
| `getAverageFiringRate()` | None | double | Average firing rate (Hz) |
| `getExcitationInhibitionRatio()` | None | double | Excitation-inhibition balance |
| `getMembranePotentialMean()` | None | double | Mean membrane potential |
| `getMembranePotentialStd()` | None | double | Standard deviation of membrane potential |

##### State and Development

```cpp
void Brain::updateDevelopment(double dt) noexcept

int Brain::getDevelopmentalStage() const noexcept

double Brain::getPlasticityRate() const noexcept

double Brain::getExcitability() const noexcept

double Brain::getMaturationProgress() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `updateDevelopment(dt)` | `dt`: double | void | Update developmental state |
| `getDevelopmentalStage()` | None | int | Current developmental stage |
| `getPlasticityRate()` | None | double | Current plasticity rate |
| `getExcitability()` | None | double | Neural excitability |
| `getMaturationProgress()` | None | double | Maturation progress (0-1) |

##### Memory System Integration

```cpp
void Brain::storeWorkingMemory(const std::vector<double>& pattern) noexcept

std::vector<double> Brain::retrieveWorkingMemory(const std::vector<double>& pattern, 
                                               double similarity_threshold) const noexcept

void Brain::recordEpisodicMemory(const std::vector<double>& pattern, 
                                int episode_id, double strength) noexcept

bool Brain::replayEpisodicMemory(int episode_id, double replay_strength) noexcept

int Brain::getWorkingMemoryUsage() const noexcept

int Brain::getWorkingMemoryCapacity() const noexcept

int Brain::getEpisodicMemoryCount() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `storeWorkingMemory(pattern)` | `pattern`: vector<double> | void | Store pattern in working memory |
| `retrieveWorkingMemory(pattern, similarity_threshold)` | `pattern`: vector<double>, `similarity_threshold`: double | vector<double> | Retrieve similar pattern |
| `recordEpisodicMemory(pattern, episode_id, strength)` | `pattern`: vector<double>, `episode_id`: int, `strength`: double | void | Record episodic memory |
| `replayEpisodicMemory(episode_id, replay_strength)` | `episode_id`: int, `replay_strength`: double | bool | Replay episodic memory |
| `getWorkingMemoryUsage()` | None | int | Current working memory usage |
| `getWorkingMemoryCapacity()` | None | int | Working memory capacity |
| `getEpisodicMemoryCount()` | None | int | Number of episodic memories |

##### Neuromodulation Integration

```cpp
void Brain::applyReward(double reward, double predicted_reward) noexcept

void Brain::applyCuriosity(double novelty_level) noexcept

void Brain::applyDopamine(double dopamine_level) noexcept

void Brain::updateNeuromodulators(double dt) noexcept

double Brain::getDopamineLevel() const noexcept

double Brain::getCuriosityLevel() const noexcept

double Brain::getNoveltyLevel() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `applyReward(reward, predicted_reward)` | `reward`: double, `predicted_reward`: double | void | Apply reward modulation |
| `applyCuriosity(novelty_level)` | `novelty_level`: double | void | Apply curiosity modulation |
| `applyDopamine(dopamine_level)` | `dopamine_level`: double | void | Apply dopamine modulation |
| `updateNeuromodulators(dt)` | `dt`: double | void | Update neuromodulator levels |
| `getDopamineLevel()` | None | double | Current dopamine level |
| `getCuriosityLevel()` | None | double | Current curiosity level |
| `getNoveltyLevel()` | None | double | Current novelty level |

##### Prediction and Planning

```cpp
void Brain::updatePrediction(const std::vector<double>& sensory_input) noexcept

double Brain::getPredictionConfidence() const noexcept

double Brain::getPredictionError() const noexcept

std::vector<double> Brain::getPredictedState() const noexcept

std::vector<double> Brain::getPlannedAction() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `updatePrediction(sensory_input)` | `sensory_input`: vector<double> | void | Update prediction model |
| `getPredictionConfidence()` | None | double | Prediction confidence |
| `getPredictionError()` | None | double | Prediction error |
| `getPredictedState()` | None | vector<double> | Predicted system state |
| `getPlannedAction()` | None | vector<double> | Planned action sequence |

### Class: NeuronConfig

```cpp
struct NeuronConfig {
    NeuronType type;
    double v_thresh;
    double v_rest;
    double v_reset;
    double tau_mem;
    double tau_ref;
    double weight_init_mean;
    double weight_init_std;
    double excitability;
    // ... other fields
};
```

**Fields:**

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `type` | NeuronType | `NeuronType::Excitatory` | Neuron type |
| `v_thresh` | double | -50.0 | Threshold potential (mV) |
| `v_rest` | double | -70.0 | Resting potential (mV) |
| `v_reset` | double | -75.0 | Reset potential (mV) |
| `tau_mem` | double | 20.0 | Membrane time constant (ms) |
| `tau_ref` | double | 2.0 | Refractory period (ms) |
| `weight_init_mean` | double | 0.5 | Initial weight mean |
| `weight_init_std` | double | 0.1 | Initial weight standard deviation |
| `excitability` | double | 1.0 | Neural excitability |

### Class: SynapseConfig

```cpp
struct SynapseConfig {
    double weight;
    double delay;
    PlasticityType plasticity_type;
    double plasticity_rate;
    // ... other fields
};
```

**Fields:**

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `weight` | double | 0.0 | Initial synaptic weight |
| `delay` | double | 5.0 | Synaptic delay (ms) |
| `plasticity_type` | PlasticityType | `PlasticityType::STDP` | Plasticity mechanism |
| `plasticity_rate` | double | 0.001 | Plasticity learning rate |

## Agent Module

### Class: AgentBrain

#### Core Methods

```cpp
AgentBrain::AgentBrain(Brain& brain) noexcept

void AgentBrain::initialize(World& world) noexcept

void AgentBrain::processSensoryInput(const std::vector<double>& percept) noexcept

std::vector<double> AgentBrain::decodeMotorCommand() noexcept

void AgentBrain::applyRewardModulation(double reward, double predicted_reward) noexcept

void AgentBrain::updateDevelopment(double dt) noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `initialize(world)` | `world`: World | void | Initialize agent with world |
| `processSensoryInput(percept)` | `percept`: vector<double> | void | Process sensory input |
| `decodeMotorCommand()` | None | vector<double> | Decode motor command from brain |
| `applyRewardModulation(reward, predicted_reward)` | `reward`: double, `predicted_reward`: double | void | Apply reward modulation |
| `updateDevelopment(dt)` | `dt`: double | void | Update agent development |

##### Subsystem Control

```cpp
void AgentBrain::enableRewardModulation(bool enable) noexcept

void AgentBrain::enableCuriosity(bool enable) noexcept

void AgentBrain::enableDevelopment(bool enable) noexcept

void AgentBrain::enableStructuralPlasticity(bool enable) noexcept

void AgentBrain::enablePrediction(bool enable) noexcept

bool AgentBrain::isRewardModulationEnabled() const noexcept

bool AgentBrain::isCuriosityEnabled() const noexcept

bool AgentBrain::isDevelopmentEnabled() const noexcept

bool AgentBrain::isStructuralPlasticityEnabled() const noexcept

bool AgentBrain::isPredictionEnabled() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `enableRewardModulation(enable)` | `enable`: bool | void | Enable/disable reward modulation |
| `enableCuriosity(enable)` | `enable`: bool | void | Enable/disable curiosity system |
| `enableDevelopment(enable)` | `enable`: bool | void | Enable/disable development |
| `enableStructuralPlasticity(enable)` | `enable`: bool | void | Enable/disable structural plasticity |
| `enablePrediction(enable)` | `enable`: bool | void | Enable/disable prediction |
| `isRewardModulationEnabled()` | None | bool | Check if reward modulation enabled |
| `isCuriosityEnabled()` | None | bool | Check if curiosity enabled |
| `isDevelopmentEnabled()` | None | bool | Check if development enabled |
| `isStructuralPlasticityEnabled()` | None | bool | Check if structural plasticity enabled |
| `isPredictionEnabled()` | None | bool | Check if prediction enabled |

##### Statistics

```cpp
double AgentBrain::getCuriosityLevel() const noexcept

double AgentBrain::getNoveltyLevel() const noexcept

double AgentBrain::getPredictionError() const noexcept

double AgentBrain::getNeuromodulationLevel() const noexcept

double AgentBrain::getDevelopmentProgress() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getCuriosityLevel()` | None | double | Current curiosity level |
| `getNoveltyLevel()` | None | double | Current novelty level |
| `getPredictionError()` | None | double | Current prediction error |
| `getNeuromodulationLevel()` | None | double | Current neuromodulation level |
| `getDevelopmentProgress()` | None | double | Development progress (0-1) |

## World Module

### Class: World

#### Core Methods

```cpp
World::World() noexcept

void World::configure(int width, int height, 
                      int vision_width, int vision_height) noexcept

void World::reset() noexcept

void World::update(double dt) noexcept

void World::setAgentStart(double x, double y) noexcept

void World::applyMotorCommand(const std::vector<double>& command, double time) noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `configure(width, height, vision_width, vision_height)` | `width`: int, `height`: int, `vision_width`: int, `vision_height`: int | void | Configure world dimensions |
| `reset()` | None | void | Reset world to initial state |
| `update(dt)` | `dt`: double | void | Update world state |
| `setAgentStart(x, y)` | `x`: double, `y`: double | void | Set agent starting position |
| `applyMotorCommand(command, time)` | `command`: vector<double>, `time`: double | void | Apply motor command to world |

##### State Access

```cpp
std::vector<double> World::getSensoryPercept() const noexcept

std::vector<double> World::getAgentBody() const noexcept

double World::getSimulationTime() const noexcept

double World::getMaxEnergy() const noexcept

void World::setMaxEnergy(double energy) noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `getSensoryPercept()` | None | vector<double> | Get sensory perception |
| `getAgentBody()` | None | vector<double> | Get agent body state |
| `getSimulationTime()` | None | double | Get current simulation time |
| `getMaxEnergy()` | None | double | Get maximum energy |
| `setMaxEnergy(energy)` | `energy`: double | void | Set maximum energy |

##### Object Management

```cpp
void World::addObject(const WorldObject& object) noexcept

bool World::removeObject(double x, double y) noexcept

bool World::isValidPosition(double x, double y) const noexcept

std::vector<WorldObject> World::getObjectsInRadius(double x, double y, double radius) const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `addObject(object)` | `object`: WorldObject | void | Add object to world |
| `removeObject(x, y)` | `x`: double, `y`: double | bool | Remove object at position |
| `isValidPosition(x, y)` | `x`: double, `y`: double | bool | Check if position is valid |
| `getObjectsInRadius(x, y, radius)` | `x`: double, `y`: double, `radius`: double | vector<WorldObject> | Get objects in radius |

## Config Module

### Class: Config

#### Core Methods

```cpp
Config::Config() noexcept

void Config::set(const std::string& key, const std::string& value) noexcept

void Config::set(const std::string& key, int value) noexcept

void Config::set(const std::string& key, double value) noexcept

void Config::set(const std::string& key, bool value) noexcept

template<typename T> T Config::get(const std::string& key) const noexcept

bool Config::has(const std::string& key) const noexcept

std::vector<std::string> Config::getKeys() const noexcept

std::string Config::getString(const std::string& key) const noexcept

int Config::getInt(const std::string& key) const noexcept

double Config::getDouble(const std::string& key) const noexcept

bool Config::getBool(const std::string& key) const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `set(key, value)` | `key`: string, `value`: string | void | Set string value |
| `set(key, value)` | `key`: string, `value`: int | void | Set int value |
| `set(key, value)` | `key`: string, `value`: double | void | Set double value |
| `set(key, value)` | `key`: string, `value`: bool | void | Set bool value |
| `get<T>(key)` | `key`: string | T | Get value with type conversion |
| `has(key)` | `key`: string | bool | Check if key exists |
| `getKeys()` | None | vector<string> | Get all configuration keys |
| `getString(key)` | `key`: string | string | Get string value |
| `getInt(key)` | `key`: string | int | Get int value |
| `getDouble(key)` | `key`: string | double | Get double value |
| `getBool(key)` | `key`: string | bool | Get bool value |

##### Configuration File Operations

```cpp
void Config::loadFromFile(const std::string& filename) noexcept

void Config::saveToFile(const std::string& filename) const noexcept

void Config::merge(const Config& other) noexcept

Config Config::clone() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `loadFromFile(filename)` | `filename`: string | void | Load configuration from file |
| `saveToFile(filename)` | `filename`: string | void | Save configuration to file |
| `merge(other)` | `other`: Config | void | Merge with other configuration |
| `clone()` | None | Config | Create copy of configuration |

##### Configuration Validation

```cpp
std::vector<std::string> Config::validate() const noexcept

bool Config::validateKey(const std::string& key) const noexcept

std::string Config::validationError() const noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `validate()` | None | vector<string> | Validate configuration, return errors |
| `validateKey(key)` | `key`: string | bool | Validate specific key |
| `validationError()` | None | string | Get validation error message |

##### Configuration Templates

```cpp
static Config Config::createDefaultConfig() noexcept

static Config Config::createResearchConfig() noexcept

static Config Config::createProductionConfig() noexcept

static Config Config::createDevelopmentConfig() noexcept
```

**Method Details:**

| Method | Parameters | Returns | Description |
|--------|------------|---------|-------------|
| `createDefaultConfig()` | None | Config | Create default configuration |
| `createResearchConfig()` | None | Config | Create research configuration |
| `createProductionConfig()` | None | Config | Create production configuration |
| `createDevelopmentConfig()` | None | Config | Create development configuration |

## Python API

### Module: pynlm

#### Core Functions

```python
import pynlm

# Creation functions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Initialization
brain.initialize()
agent.initialize(world)

# Simulation
for step in range(1000):
    brain.step(step)
    
# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### Python Classes

##### Brain Class

```python
class Brain:
    def __init__(self, config):
        """Create brain with configuration."""
    
    def initialize(self):
        """Initialize brain."""
    
    def step(self, step_number, time=None):
        """Run simulation step."""
    
    def reset(self):
        """Reset brain state."""
    
    def save(self, filename):
        """Save brain state."""
    
    def load(self, filename):
        """Load brain state."""
    
    def getTotalNeuronCount(self):
        """Get total neuron count."""
    
    def getTotalSynapseCount(self):
        """Get total synapse count."""
    
    def getFiringNeuronCount(self):
        """Get firing neuron count."""
    
    def getAverageFiringRate(self):
        """Get average firing rate."""
    
    def getExcitationInhibitionRatio(self):
        """Get E/I ratio."""
    
    def receiveSensoryInput(self, sensory_data):
        """Process sensory input."""
    
    def produceAction(self):
        """Generate motor action."""
```

##### AgentBrain Class

```python
class AgentBrain:
    def __init__(self, brain):
        """Create agent with brain."""
    
    def initialize(self, world):
        """Initialize agent with world."""
    
    def processSensoryInput(self, percept):
        """Process sensory input."""
    
    def decodeMotorCommand(self):
        """Decode motor command."""
    
    def applyRewardModulation(self, reward, predicted_reward):
        """Apply reward modulation."""
    
    def updateDevelopment(self, dt):
        """Update development."""
    
    def enableRewardModulation(self, enable):
        """Enable/disable reward modulation."""
    
    def enableCuriosity(self, enable):
        """Enable/disable curiosity."""
    
    def enableDevelopment(self, enable):
        """Enable/disable development."""
    
    def enableStructuralPlasticity(self, enable):
        """Enable/disable structural plasticity."""
    
    def getCuriosityLevel(self):
        """Get curiosity level."""
    
    def getNoveltyLevel(self):
        """Get novelty level."""
    
    def getPredictionError(self):
        """Get prediction error."""
    
    def getNeuromodulationLevel(self):
        """Get neuromodulation level."""
```

##### SimpleWorld Class

```python
class SimpleWorld:
    def __init__(self):
        """Create simple world."""
    
    def configure(self, width, height, vision_width, vision_height):
        """Configure world dimensions."""
    
    def reset(self):
        """Reset world to initial state."""
    
    def update(self, dt):
        """Update world state."""
    
    def setAgentStart(self, x, y):
        """Set agent starting position."""
    
    def applyMotorCommand(self, command, time):
        """Apply motor command."""
    
    def getSensoryPercept(self):
        """Get sensory perception."""
    
    def getAgentBody(self):
        """Get agent body state."""
```

##### Config Class

```python
class Config:
    def __init__(self):
        """Create empty configuration."""
    
    def set(self, key, value):
        """Set configuration value."""
    
    def get(self, key):
        """Get configuration value."""
    
    def has(self, key):
        """Check if key exists."""
    
    def getKeys(self):
        """Get all keys."""
    
    def loadFromFile(self, filename):
        """Load configuration from file."""
    
    def saveToFile(self, filename):
        """Save configuration to file."""
```

#### Enumerations

##### NeuronType

```python
class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Sensory = 2
    Motor = 3
    Modulatory = 4
    Internal = 5
```

##### DevelopmentalStage

```python
class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4
```

##### ActionType

```python
class ActionType:
    MoveForward = 0
    TurnLeft = 1
    TurnRight = 2
    Interact = 3
    Eat = 4
    Drink = 5
    Sleep = 6
    Explore = 7
```

##### WorldObjectType

```python
class WorldObjectType:
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4
```

## Function References

### Utility Functions

#### Brain Creation Functions

```cpp
Brain createBrain(const Config& config) noexcept

Config createDefaultConfig() noexcept

Config createResearchConfig() noexcept

Config createProductionConfig() noexcept

Config createDevelopmentConfig() noexcept
```

#### Agent Creation Functions

```cpp
AgentBrain createAgentBrain(Brain& brain) noexcept

World createSimpleWorld() noexcept
```

#### Configuration Functions

```cpp
void saveConfig(const Config& config, const std::string& filename) noexcept

Config loadConfig(const std::string& filename) noexcept

bool validateConfig(const Config& config) noexcept

std::string configToString(const Config& config) noexcept

Config configFromString(const std::string& config_string) noexcept
```

#### Simulation Functions

```cpp
double runSimulation(const Config& config, 
                     int num_steps,
                     std::function<void(Brain&, int)> callback = nullptr) noexcept

bool runAgentSimulation(const Config& config,
                        const std::string& world_config,
                        int num_steps) noexcept

std::vector<double> runCompleteExperiment(const Config& config,
                                          const std::string& experiment_name) noexcept
```

## Performance Considerations

### 1. Memory Management

#### Memory Allocation
```cpp
// Avoid dynamic allocation in hot paths
void Brain::step(int step_number) noexcept {
    // Pre-allocate memory pools
    static thread_local std::vector<Neuron> neuron_pool;
    neuron_pool.reserve(getTotalNeuronCount());
    
    // Use pool instead of new/delete
    for (auto& neuron : neurons_) {
        neuron_pool.push_back(neuron);
    }
}
```

### 2. CPU Optimization

#### Cache-friendly access
```cpp
// Process neurons in cache-friendly order
void Brain::updateAllNeurons(double dt) noexcept {
    // Process neurons sequentially for cache efficiency
    for (auto& neuron : neurons_) {
        updateNeuron(neuron, dt);
    }
}
```

### 3. I/O Optimization

#### Binary I/O
```cpp
void Brain::save(const std::string& filename) const noexcept {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    // Write binary data directly
    file.write(reinterpret_cast<const char*>(neurons_.data()),
               neurons_.size() * sizeof(Neuron));
}
```

## Error Handling

### C++ API Error Handling

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"

try {
    // Code that may throw
    Brain brain(config);
    brain.initialize();
    brain.step(0);
} catch (const nlm::ValidationException& e) {
    std::cerr << "Validation error: " << e.what() << std::endl;
} catch (const nlm::NeuralException& e) {
    std::cerr << "Neural error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
}
```

### Python API Error Handling

```python
try:
    # Code that may raise
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.NeuralError as e:
    print(f"Neural error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Thread Safety

### C++ API Thread Safety

```cpp
// Thread-safe operations
void Brain::step(int step_number) noexcept {
    // Use mutex for thread safety
    std::lock_guard<std::mutex> lock(mutex_);
    // Update neurons
}

// Read-only operations can be thread-safe without mutex
const std::vector<Neuron>& Brain::getNeurons() const noexcept {
    // Return const reference
    return neurons_;
}
```

### Python API Thread Safety

```python
import threading
import pynlm

# Use thread locks for safety
brain_lock = threading.Lock()

def safe_brain_operation(brain, step):
    with brain_lock:
        brain.step(step)
        return brain.getFiringNeuronCount()
```

## Migration Guide

### C++ to Python Migration

```cpp
// C++ code
Brain brain(config);
brain.initialize();
brain.step(0);
double firing_rate = brain.getAverageFiringRate();

// Python equivalent
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
firing_rate = brain.getAverageFiringRate()
```

### Configuration Migration

```cpp
// C++ configuration
Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

// Python configuration
import pynlm
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
```

## API Changelog

### Version 1.0.0
- Initial API release
- All core modules implemented
- Comprehensive error handling
- Thread safety support

### Version 1.1.0
- Enhanced performance
- Improved memory management
- Additional configuration options
- Better error messages

### Version 1.2.0
- Python API improvements
- Enhanced documentation
- Additional neural models
- Better integration support

## References

1. **NLM Documentation**: https://github.com/yourusername/nlm/wiki
2. **API Reference**: https://yourusername.github.io/nlm/api/
3. **Python Bindings**: https://pypi.org/project/pynlm/
4. **Examples**: https://github.com/yourusername/nlm/tree/main/examples

## Contact

For API questions, issues, or contributions:
- **GitHub Issues**: https://github.com/yourusername/nlm/issues
- **Email**: api.support@yourcompany.com
- **Documentation**: https://docs.yourcompany.com/nlm

---

*This API reference provides comprehensive documentation for the NLM API.*
*For configuration details, see CONFIGURATION_GUIDE.md.*
*For development practices, see DEVELOPMENT_GUIDE.md.*
*For performance information, see PERFORMANCE_GUIDE.md.*
*For error handling, see ERROR_HANDLING_REFERENCE.md.*

**Last updated**: 2024-01-17
**Version**: 1.0.0

*This documentation is generated from source code comments and maintained by the NLM development team.*

