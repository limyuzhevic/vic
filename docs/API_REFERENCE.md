# NLM API Reference Documentation

## Overview

This document provides comprehensive API reference for the enhanced NLM neural simulation framework, covering all new features from Phases 1-5.

## Python Bindings (Phase 1.1-1.4)

### Core Classes

#### NeuronId
```python
class NeuronId:
    def __init__(self, value: Optional[int] = None)
    @property
    def value(self) -> int
    def index(self) -> int
    def __eq__(self, other: object) -> bool
    def __ne__(self, other: object) -> bool
    def __hash__(self) -> int
    def __repr__(self) -> str
    def __str__(self) -> str
```

#### SynapseId
```python
class SynapseId:
    def __init__(self, value: Optional[int] = None)
    @property
    def value(self) -> int
    def index(self) -> int
```

#### RegionId
```python
class RegionId:
    def __init__(self, value: Optional[int] = None)
    @property
    def value(self) -> int
    def index(self) -> int
```

#### PopulationId
```python
class PopulationId:
    def __init__(self, value: Optional[int] = None)
    @property
    def value(self) -> int
    def index(self) -> int
```

#### NeuronType (Enum)
```python
class NeuronType(Enum):
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5
```

#### SynapseType (Enum)
```python
class SynapseType(Enum):
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4
```

#### DevelopmentalStage (Enum)
```python
class DevelopmentalStage(Enum):
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4
```

#### FiringState (Enum)
```python
class FiringState(Enum):
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3
```

#### ActionType (Enum)
```python
class ActionType(Enum):
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14
```

#### MotorCommand (Enum)
```python
class MotorCommand(Enum):
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7
```

#### WorldObjectType (Enum)
```python
class WorldObjectType(Enum):
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4
```

### Configuration

#### Config
```python
class Config:
    def __init__(self)
    @staticmethod
    def from_dict(config_dict: dict) -> 'Config'
    def to_dict(self) -> dict
    def get(self, key: str, default_value: Any = None) -> Any
    def set(self, key: str, value: Any) -> None
    def load_from_file(self, filepath: str) -> None
    def load_from_args(self, argc: int, argv: list) -> bool
    def save_to_file(self, filepath: str) -> None
    def has(self, key: str) -> bool
    def get_keys(self) -> list
    def clear(self) -> None
    def summary(self) -> str
    def validate(self) -> dict
    def __repr__(self) -> str
```

### Sensory Inputs

#### Vision
```python
class Vision(SensoryInput):
    def __init__(self, width: Optional[int] = None, height: Optional[int] = None, channels: int = 3)
    def set_data(self, data: Union[list, np.ndarray]) -> None
    def get_data(self) -> np.ndarray
    def get_data_vector(self) -> list
    def get_width(self) -> int
    def get_height(self) -> int
    def get_channels(self) -> int
    def get_dimensions(self) -> tuple
    def is_valid_numpy_shape(self, array: np.ndarray) -> bool
    def validate_and_convert_numpy(self, obj: Any) -> tuple
```

#### Audio
```python
class Audio(SensoryInput):
    def __init__(self, sampleRate: Optional[int] = None, numSamples: Optional[int] = None)
    def set_data(self, data: Union[list, np.ndarray]) -> None
    def get_data(self) -> np.ndarray
    def get_data_vector(self) -> list
    def set_sample_rate(self, sampleRate: int) -> None
    def get_sample_rate(self) -> int
    def get_num_samples(self) -> int
    def get_dimensions(self) -> tuple
```

#### InternalSignals
```python
class InternalSignals(SensoryInput):
    def __init__(self)
    def add_signal(self, value: float) -> None
    def clear_signals(self) -> None
```

#### Proprioception
```python
class Proprioception(SensoryInput):
    def __init__(self, position: Optional[tuple] = None, orientation: Optional[float] = None)
    def set_position(self, x: float, y: float) -> None
    def get_position(self) -> tuple
    def set_orientation(self, angle: float) -> None
    def get_orientation(self) -> float
```

### Motor Outputs

#### Action
```python
class Action:
    def __init__(self, actionType: Optional[ActionType] = None, parameters: Optional[list] = None)
    def __init__(self, actionType: ActionType, parameters: list)
    def get_type(self) -> ActionType
    def set_type(self, actionType: ActionType) -> None
    def get_parameters(self) -> list
    def set_parameters(self, params: list) -> None
    def get_name(self) -> str
    def clone(self) -> 'Action'
```

### Environment Objects

#### WorldObject
```python
class WorldObject:
    def __init__(self, x: float, y: float, objType: WorldObjectType, value: float = 0.0, radius: float = 0.5)
    @property
    def x(self) -> float
    @x.setter
    def x(self, value: float) -> None
    @property
    def y(self) -> float
    @y.setter
    def y(self, value: float) -> None
    @property
    def radius(self) -> float
    @radius.setter
    def radius(self, value: float) -> None
    @property
    def type(self) -> WorldObjectType
    @type.setter
    def type(self, value: WorldObjectType) -> None
    @property
    def value(self) -> float
    @value.setter
    def value(self, value: float) -> None
    @property
    def active(self) -> bool
    @active.setter
    def active(self, value: bool) -> None
```

#### AgentBody
```python
class AgentBody:
    def __init__(self)
    @property
    def x(self) -> float
    @x.setter
    def x(self, value: float) -> None
    @property
    def y(self) -> float
    @y.setter
    def y(self, value: float) -> None
    @property
    def orientation(self) -> float
    @orientation.setter
    def orientation(self, value: float) -> None
    @property
    def velocityX(self) -> float
    @velocityX.setter
    def velocityX(self, value: float) -> None
    @property
    def velocityY(self) -> float
    @velocityY.setter
    def velocityY(self, value: float) -> None
    @property
    def angularVelocity(self) -> float
    @angularVelocity.setter
    def angularVelocity(self, value: float) -> None
    @property
    def energy(self) -> float
    @energy.setter
    def energy(self, value: float) -> None
    @property
    def health(self) -> float
    @health.setter
    def health(self, value: float) -> None
    @property
    def age(self) -> float
    @age.setter
    def age(self, value: float) -> None
    @property
    def isMoving(self) -> bool
    @isMoving.setter
    def isMoving(self, value: bool) -> None
    @property
    def isTurning(self) -> bool
    @isTurning.setter
    def isTurning(self, value: bool) -> None
    @property
    def lastActionTime(self) -> float
    @lastActionTime.setter
    def lastActionTime(self, value: float) -> None
    def reset(self) -> None
```

### Main Components

#### Brain
```python
class Brain:
    def __init__(self, config: Optional[Config] = None, thread_safe: bool = False, precision_mode: str = "single")
    def initialize(self) -> None
    def step(self, currentStep: int, currentTime: Optional[float] = None) -> None
    def receive_sensory_input(self, input: SensoryInput) -> None
    def inject_current(self, neuron: NeuronId, current: float) -> None
    def inject_current_to_neurons(self, neuronType: NeuronType, current: float) -> None
    def produce_action(self) -> Action
    def reset(self) -> None
    def save(self, filepath: str) -> None
    def load(self, filepath: str) -> None
    def add_region(self, name: str = "") -> RegionId
    def get_region(self, id: RegionId) -> Any
    def get_region_count(self) -> int
    def get_region_ids(self) -> list
    def get_regions(self) -> list
    def get_total_neuron_count(self) -> int
    def get_total_synapse_count(self) -> int
    def get_active_neuron_count(self) -> int
    def get_firing_neuron_count(self) -> int
    def get_average_firing_rate(self) -> float
    def get_excitation_inhibition_ratio(self) -> float
    def get_total_spike_count(self) -> int
    def get_developmental_stage(self) -> DevelopmentalStage
    def set_developmental_stage(self, stage: DevelopmentalStage) -> None
    def get_config(self) -> Config
    def log_status(self) -> None
    def enable_memory_pool(self, enable: bool) -> None
    def __enter__(self) -> 'Brain'
    def __exit__(self, *args) -> None
```

#### AgentBrain
```python
class AgentBrain:
    def __init__(self, brain: Brain, world: Optional[Any] = None)
    def initialize(self, world: Any) -> None
    def get_sensory_input_size(self) -> tuple
    def get_motor_output_size(self) -> int
    def process_sensory_input(self, percept: Any) -> None
    def decode_motor_command(self) -> MotorCommand
    def apply_reward_modulation(self, reward: float, predictedReward: float) -> None
    def update_development(self, timestep: int) -> None
    def get_developmental_stage(self) -> DevelopmentalStage
    def get_neuromodulation_level(self) -> float
    def get_curiosity_level(self) -> float
    def get_novelty_level(self) -> float
    def get_prediction_error(self) -> float
    def reset(self) -> None
    def get_brain(self) -> Brain
    def enable_reward_modulation(self, enable: bool) -> None
    def enable_structural_plasticity(self, enable: bool) -> None
    def enable_development(self, enable: bool) -> None
    def enable_curiosity(self, enable: bool) -> None
    def is_reward_modulation_enabled(self) -> bool
    def is_structural_plasticity_enabled(self) -> bool
    def is_development_enabled(self) -> bool
    def is_curiosity_enabled(self) -> bool
    def get_configuration(self) -> Config
    @staticmethod
    def create_with_defaults(brainName: str) -> 'AgentBrain'
```

#### SimpleWorld
```python
class SimpleWorld:
    def __init__(self)
    def configure(self, width: int, height: int, visionWidth: int, visionHeight: int) -> None
    def reset(self) -> None
    def set_agent_start(self, x: float, y: float) -> None
    def update(self, timestep: int) -> None
    def apply_motor_command(self, cmd: MotorCommand, currentTime: float) -> Any
    def get_sensory_percept(self) -> Any
    def get_agent_body(self) -> AgentBody
    def add_object(self, obj: WorldObject) -> None
    def remove_object(self, x: float, y: float) -> None
    def is_valid_position(self, x: float, y: float) -> bool
    def get_width(self) -> int
    def get_height(self) -> int
    def get_max_energy(self) -> float
    def set_max_energy(self, e: float) -> None
    def get_energy_decay_rate(self) -> float
    def set_energy_decay_rate(self, r: float) -> None
    def get_simulation_time(self) -> float
    def set_random_seed(self, seed: int) -> None
    def get_random_seed(self) -> int
```

### Advanced Python Classes (Phase 1.1.3)

#### NLMAgent
```python
class NLMAgent:
    def __init__(self, brain: Brain, world: SimpleWorld, config: Optional[Config] = None)
    def step(self) -> Action
    def process_observation(self, percept: Any) -> None
    def update(self, timestep: int) -> None
    def reset(self) -> None
    def get_state(self) -> dict
    def set_state(self, state: dict) -> None
    def learn(self, reward: float, done: bool) -> None
    def is_alive(self) -> bool
    def get_energy(self) -> float
    def set_energy(self, energy: float) -> None
```

#### SimulationManager
```python
class SimulationManager:
    def __init__(self, brain: Brain, world: SimpleWorld, config: Optional[Config] = None)
    def create_episode(self, episodeId: Optional[str] = None) -> int
    def run_episode(self, episodeId: str, maxSteps: int) -> dict
    def restore_episode(self, episodeId: str) -> None
    def save_checkpoint(self, episodeId: str, step: int) -> None
    def load_checkpoint(self, episodeId: str, step: int) -> None
    def get_episode_stats(self, episodeId: str) -> dict
    def list_episodes(self) -> list
    def delete_episode(self, episodeId: str) -> None
    def cleanup_old_episodes(self, keepLatest: int) -> None
```

#### Visualizer
```python
class Visualizer:
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None)
    def visualize_neuron_activity(self, neurons: list, activity: list) -> np.ndarray
    def visualize_synaptic_connections(self, brain: Brain) -> np.ndarray
    def visualize_brain_regions(self, brain: Brain) -> np.ndarray
    def generate_spike_raster(self, spikes: list, binWidth: float = 1.0) -> dict
    def visualize_network_topology(self, brain: Brain) -> dict
    def export_for_external_tool(self, data: dict, format: str = "json") -> str
    def plot_to_file(self, data: dict, filepath: str) -> None
    def show_live_plot(self, duration: Optional[float] = None) -> None
```

### Python Utilities (Phase 1.1.2)

#### Session Management
```python
def start_session_logger() -> Any
```

#### Performance Timing
```python
class PythonTimer:
    def __init__(self, name: str)
    def elapsed(self) -> float
```

#### Numpy Utilities
```python
class NumpyUtils:
    @staticmethod
    def is_numpy_array(obj: Any) -> bool
    
    @staticmethod
    def get_numpy_array_dimensions(obj: Any) -> list
    
    @staticmethod
    def convert_vector_to_numpy(vec: list) -> np.ndarray
    
    @staticmethod
    def convert_numpy_to_vector(arr: np.ndarray) -> list
    
    @staticmethod
    def safe_numpy_access(obj: Any, index: int) -> np.ndarray
```

#### Data Export/Import
```python
def export_brain_state(brain: Brain) -> dict
def import_brain_state(brain: Brain, data: dict) -> bool
def export_world_state(world: SimpleWorld) -> dict
def visualize_neuron_activity(brain: Brain) -> np.ndarray
def visualize_synaptic_connections(brain: Brain) -> np.ndarray
def visualize_brain_regions(brain: Brain) -> np.ndarray
def generate_spike_raster(brain: Brain) -> dict
def visualize_network_topology(brain: Brain) -> dict
```

### Factory Functions

```python
# Configuration Factory
def create_default_config() -> Config

def create_brain_config(brain_name: str, simulation_steps: int = 1000) -> Config

def create_agent_config(energy: float = 100.0, max_steps: int = 1000) -> Config

# Component Factory
def create_brain(config: Optional[Config] = None, thread_safe: bool = False, precision_mode: str = "single") -> Brain

def create_agent_brain(brain_name: str = "default") -> AgentBrain

def create_simple_world(width: int = 100, height: int = 100) -> SimpleWorld

def create_vision(width: int, height: int, channels: int = 3) -> Vision

def create_audio(sampleRate: int, numSamples: int) -> Audio

def create_internal_signals() -> InternalSignals

def create_proprioception(position: Optional[tuple] = None, orientation: Optional[float] = None) -> Proprioception

def create_action(actionType: ActionType, parameters: Optional[list] = None) -> Action

def create_world_object(x: float, y: float, objType: WorldObjectType, value: float = 0.0, radius: float = 0.5) -> WorldObject

def create_agent_body(x: float = 0.0, y: float = 0.0) -> AgentBody

# Agent Factory
def create_nlm_agent(brain_name: str = "default", world: Optional[SimpleWorld] = None, config: Optional[Config] = None) -> NLMAgent

# Simulation Factory
def create_simulation_manager(brain: Brain, world: SimpleWorld, config: Optional[Config] = None) -> SimulationManager

def create_visualizer(brain: Brain, world: Optional[SimpleWorld] = None) -> Visualizer

# Utility Factory
def create_timer(name: str) -> PythonTimer

def get_numpy_array(obj: Any) -> np.ndarray

def get_vector_from_numpy(obj: Any) -> list
```

### Context Manager Support

```python
# Brain context manager support (built-in)
with pynlm.create_brain(config) as brain:
    brain.initialize()
    brain.step(0, 0.0)
    # Automatic cleanup on exit
```

## C++ Core Classes

### Core Layer Classes

#### Config
```cpp
class Config {
public:
    Config();
    ~Config();
    
    // Type-safe getters with defaults
    bool get(const std::string& key, bool defaultValue = false) const;
    int get(const std::string& key, int defaultValue = 0) const;
    double get(const std::string& key, double defaultValue = 0.0) const;
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    
    // Type-safe setters
    void set(const std::string& key, bool value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, const std::string& value);
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Query operations
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Validation
    std::map<std::string, std::string> validate() const;
};
```

#### Types (Strongly-typed IDs)
```cpp
class NeuronId {
public:
    NeuronId();
    NeuronId(uint64_t value);
    uint64_t value() const;
    size_t index() const;
    bool operator==(const NeuronId& other) const;
    bool operator!=(const NeuronId& other) const;
    bool operator<(const NeuronId& other) const;
};

class SynapseId {
public:
    SynapseId();
    SynapseId(uint64_t value);
    uint64_t value() const;
    size_t index() const;
    bool operator==(const SynapseId& other) const;
    bool operator!=(const SynapseId& other) const;
    bool operator<(const SynapseId& other) const;
};

class RegionId {
public:
    RegionId();
    RegionId(uint64_t value);
    uint64_t value() const;
    size_t index() const;
    bool operator==(const RegionId& other) const;
    bool operator!=(const RegionId& other) const;
    bool operator<(const RegionId& other) const;
};

class PopulationId {
public:
    PopulationId();
    PopulationId(uint64_t value);
    uint64_t value() const;
    size_t index() const;
    bool operator==(const PopulationId& other) const;
    bool operator!=(const PopulationId& other) const;
    bool operator<(const PopulationId& other) const;
};
```

#### RandomGenerator
```cpp
class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(uint64_t seed);
    void setSeed(uint64_t seed);
    uint64_t getSeed() const;
    float uniform();
    float uniform(float min, float max);
    int integer(int min = 0, int max = INT_MAX);
    double gaussian(double mean = 0.0, double stddev = 1.0);
    bool bernoulli(double p = 0.5);
};
```

#### SimulationClock
```cpp
class SimulationClock {
public:
    SimulationClock();
    SimulationClock(Timestamp startTime);
    void setStartTime(Timestamp startTime);
    Timestamp getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    void advanceStep();
    void advanceTime(Timestamp dt);
    Timestamp getElapsedTime() const;
    SimulationStep getElapsedSteps() const;
};
```

### Brain Layer Classes

#### Neuron
```cpp
class Neuron {
public:
    Neuron();
    Neuron(NeuronType type, float threshold = 1.0f);
    
    // State management
    void setId(NeuronId id);
    NeuronId getId() const;
    void setRegionId(RegionId regionId);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId popId);
    PopulationId getPopulationId() const;
    void setType(NeuronType type);
    NeuronType getType() const;
    
    // State variables
    float getMembranePotential() const;
    void setMembranePotential(float potential);
    float getRefractoryCounter() const;
    void setRefractoryCounter(float counter);
    float getAdaptationVariable() const;
    void setAdaptationVariable(float variable);
    FiringState getFiringState() const;
    void setFiringState(FiringState state);
    
    // Dynamics
    void stepLIF(Timestamp time, float dt);
    bool hasSpiked() const;
    void clearSpike();
    
    // Input/output
    void addInput(float current);
    void reset();
    std::string toString() const;
};
```

#### Synapse
```cpp
class Synapse {
public:
    Synapse();
    Synapse(NeuronId preNeuron, NeuronId postNeuron, float weight);
    
    // Connectivity
    NeuronId getPreNeuronId() const;
    NeuronId getPostNeuronId() const;
    RegionId getPreRegionId() const;
    RegionId getPostRegionId() const;
    PopulationId getPrePopulationId() const;
    PopulationId getPostPopulationId() const;
    
    // Properties
    float getWeight() const;
    void setWeight(float weight);
    float getDelay() const;
    void setDelay(float delay);
    SynapseType getType() const;
    void setType(SynapseType type);
    PlasticityFlags getPlasticityFlags() const;
    void setPlasticityFlags(PlasticityFlags flags);
    
    // Transmission
    float getDelayTime() const;
    void addDelayedSpike(Timestamp time);
    bool hasSpikeReady(Timestamp currentTime) const;
    float getTransmissionStrength() const;
    
    // Plasticity
    void applySTDP(const NeuronId& postNeuron, bool postSpiked);
    void applyHebbian(float preActivity, float postActivity);
    
    // State
    void reset();
    std::string toString() const;
};
```

#### NeuralPopulation
```cpp
class NeuralPopulation {
public:
    NeuralPopulation();
    NeuralPopulation(PopulationId id, RegionId regionId, NeuronType type);
    
    // Identity
    PopulationId getId() const;
    RegionId getRegionId() const;
    NeuronType getType() const;
    
    // Neurons
    NeuronId addNeuron(const Neuron& neuron);
    Neuron& getNeuron(NeuronId id);
    const Neuron& getNeuron(NeuronId id) const;
    bool hasNeuron(NeuronId id) const;
    void removeNeuron(NeuronId id);
    size_t getNeuronCount() const;
    std::vector<NeuronId> getNeuronIds() const;
    
    // Statistics
    float getAverageMembranePotential() const;
    float getFiringRate() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    
    // Input/output
    void injectCurrent(float current);
    void reset();
    std::string toString() const;
};
```

#### NeuralRegion
```cpp
class NeuralRegion {
public:
    NeuralRegion();
    NeuralRegion(RegionId id, const std::string& name = "");
    
    // Identity
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Populations
    PopulationId addPopulation(const NeuralPopulation& population);
    NeuralPopulation& getPopulation(PopulationId id);
    const NeuralPopulation& getPopulation(PopulationId id) const;
    bool hasPopulation(PopulationId id) const;
    void removePopulation(PopulationId id);
    size_t getPopulationCount() const;
    std::vector<PopulationId> getPopulationIds() const;
    
    // Statistics
    size_t getNeuronCount() const;
    size_t getSynapseCount() const;
    float getAverageFiringRate() const;
    float getExcitatoryInhibitionRatio() const;
    
    // Input/output
    void injectCurrent(NeuronType type, float current);
    void reset();
    std::string toString() const;
};
```

### Dynamics Layer Classes

#### NeuralDynamics (Abstract)
```cpp
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    virtual void update(Neuron& neuron, float dt, Timestamp time) = 0;
    virtual std::string getName() const = 0;
};
```

#### LIFDynamics
```cpp
class LIFDynamics : public NeuralDynamics {
public:
    LIFDynamics();
    LIFDynamics(float threshold, float reset, float tau, float decayRate);
    
    void update(Neuron& neuron, float dt, Timestamp time) override;
    std::string getName() const override;
    
    // Parameters
    float getThreshold() const;
    void setThreshold(float threshold);
    float getReset() const;
    void setReset(float reset);
    float getTimeConstant() const;
    void setTimeConstant(float tau);
    float getDecayRate() const;
    void setDecayRate(float decayRate);
};
```

#### SpikeSystem
```cpp
class SpikeSystem {
public:
    SpikeSystem();
    
    // Event-driven spike processing
    void addImmediateSpike(NeuronId neuronId, Timestamp time);
    void addDelayedSpike(NeuronId neuronId, Timestamp time, float delay);
    std::vector<NeuronId> processSpikes(Timestamp currentTime);
    std::vector<std::pair<NeuronId, Timestamp>> getPendingSpikes() const;
    
    // Statistics
    size_t getPendingSpikeCount() const;
    size_t getTotalSpikeCount() const;
    float getFiringRate() const;
    
    // Reset
    void reset();
};
```

### Plasticity Layer Classes

#### PlasticityRule (Abstract)
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    virtual void applyPlasticity(Synapse& synapse, const Neuron& preNeuron, const Neuron& postNeuron) = 0;
    virtual std::string getName() const = 0;
};
```

#### STDP
```cpp
class STDP : public PlasticityRule {
public:
    STDP();
    STDP(float learningRate, float tauPre, float tauPost);
    
    void applyPlasticity(Synapse& synapse, const Neuron& preNeuron, const Neuron& postNeuron) override;
    std::string getName() const override;
    
    // Parameters
    float getLearningRate() const;
    void setLearningRate(float rate);
    float getTauPre() const;
    void setTauPre(float tau);
    float getTauPost() const;
    void setTauPost(float tau);
};
```

#### Hebbian
```cpp
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    Hebbian(float learningRate);
    
    void applyPlasticity(Synapse& synapse, const Neuron& preNeuron, const Neuron& postNeuron) override;
    std::string getName() const override;
    
    // Parameters
    float getLearningRate() const;
    void setLearningRate(float rate);
```

#### StructuralPlasticity
```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity();
    
    // Synaptogenesis
    bool tryCreateSynapse(const NeuronId& preNeuron, const NeuronId& postNeuron, float probability);
    
    // Pruning
    bool tryRemoveSynapse(const NeuronId& postNeuron, float threshold);
    
    // Statistics
    size_t getCreatedSynapses() const;
    size_t getRemovedSynapses() const;
    
    // Reset
    void reset();
};
```

### Development Layer Classes

#### DevelopmentSystem
```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    
    // Stage management
    void setCurrentStage(DevelopmentalStage stage);
    DevelopmentalStage getCurrentStage() const;
    bool advanceStage();
    bool isInStage(DevelopmentalStage stage) const;
    
    // Plasticity modifiers
    float getPlasticityModulator(PlasticityType type) const;
    void setPlasticityModulator(PlasticityType type, float value);
    
    // Critical periods
    bool isInCriticalPeriod() const;
    float getCriticalPeriodFactor() const;
    
    // Synaptogenesis probability
    float getSynaptogenesisProbability() const;
    void setSynaptogenesisProbability(float prob);
    
    // Pruning threshold
    float getPruningThreshold() const;
    void setPruningThreshold(float threshold);
    
    // Update
    void update(Timestamp time, float dt);
    
    // Reset
    void reset();
};
```

### Neuromodulation Layer Classes

#### Neuromodulator (Abstract)
```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    virtual float computeSignal(const Brain& brain, const std::map<std::string, float>& context) = 0;
    virtual std::string getName() const = 0;
};
```

#### Dopamine
```cpp
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    Dopamine(float learningRate, float decayRate);
    
    float computeSignal(const Brain& brain, const std::map<std::string, float>& context) override;
    std::string getName() const override;
    
    // Parameters
    float getLearningRate() const;
    void setLearningRate(float rate);
    float getDecayRate() const;
    void setDecayRate(float rate);
};
```

#### Novelty
```cpp
class Novelty : public Neuromodulator {
public:
    Novelty();
    Novelty(float sensitivity);
    
    float computeSignal(const Brain& brain, const std::map<std::string, float>& context) override;
    std::string getName() const override;
    
    // Parameters
    float getSensitivity() const;
    void setSensitivity(float sensitivity);
```

#### Curiosity
```cpp
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    Curiosity(float explorationRate, float decayRate);
    
    float computeSignal(const Brain& brain, const std::map<std::string, float>& context) override;
    std::string getName() const override;
    
    // Parameters
    float getExplorationRate() const;
    void setExplorationRate(float rate);
    float getDecayRate() const;
    void setDecayRate(float rate);
```

#### PredictionError
```cpp
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    PredictionError(float sensitivity, float decayRate);
    
    float computeSignal(const Brain& brain, const std::map<std::string, float>& context) override;
    std::string getName() const override;
    
    // Parameters
    float getSensitivity() const;
    void setSensitivity(float sensitivity);
    float getDecayRate() const;
    void setDecayRate(float rate);
```

### Memory Layer Classes

#### NeuralWorkingMemory
```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    
    // Persistent activity
    void storePattern(const std::vector<NeuronId>& neurons, float strength = 1.0f);
    std::vector<NeuronId> retrievePattern(float threshold = 0.5f) const;
    void updatePattern(const std::vector<NeuronId>& neurons, float dt);
    void decayAll(float rate);
    
    // Competition
    void applyWinnerTakeAll(float threshold);
    std::vector<float> getPatternStrengths() const;
    
    // Statistics
    size_t getActivePatternCount() const;
    float getAveragePatternStrength() const;
    
    // Reset
    void reset();
```

#### NeuralEpisodicMemory
```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    
    // Recording
    void recordExperience(const Brain& brain, Timestamp time, const Action& action, const ActionResult& result);
    std::vector<Episode> replayEpisode(size_t episodeId, float replayStrength = 1.0f) const;
    
    // Retrieval
    std::vector<Episode> retrieveSimilarEpisodes(const std::vector<NeuronId>& pattern, size_t limit = 10) const;
    void consolidateMemories(float consolidationRate);
    
    // Statistics
    size_t getEpisodeCount() const;
    float getMemoryStrength(EpisodeId id) const;
    
    // Reset
    void reset();
```

#### ConceptFormation
```cpp
class ConceptFormation {
public:
    ConceptFormation();
    
    // Pattern discovery
    ConceptId learnConcept(const std::vector<NeuronId>& pattern, float stabilityThreshold = 0.8f);
    std::vector<NeuronId> recallConcept(ConceptId conceptId) const;
    
    // Categorization
    std::vector<ConceptId> categorizePattern(const std::vector<NeuronId>& pattern) const;
    void updateConceptStability(ConceptId conceptId, float newStability);
    
    // Statistics
    float getConceptStability(ConceptId conceptId) const;
    std::vector<ConceptId> getActiveConcepts() const;
    
    // Reset
    void reset();
```

### Prediction Layer Classes

#### NeuralPrediction
```cpp
class NeuralPrediction {
public:
    NeuralPrediction();
    
    // Temporal sequence learning
    void learnSequence(const std::vector<Timestamp>& eventTimes, const std::vector<Action>& actions);
    std::vector<Action> predictFutureActions(Timestamp horizon, size_t steps = 10) const;
    
    // Multi-step prediction
    std::vector<Timestamp> predictEvents(Timestamp startTime, size_t horizon, size_t steps) const;
    float predictActionConsequences(Action action, Timestamp startTime, size_t steps) const;
    
    // Error computation
    float computePredictionError(Timestamp predictedTime, Timestamp actualTime, Action actualAction) const;
    void updateWithError(float error);
    
    // Statistics
    float getPredictionAccuracy() const;
    std::vector<float> getSequenceStrengths() const;
    
    // Reset
    void reset();
```

#### PredictionErrorSignal
```cpp
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    
    // Error computation
    float computeError(const std::vector<Action>& predictedActions, const std::vector<Action>& actualActions) const;
    std::vector<float> computeGradient(const std::vector<Action>& predictedActions, const std::vector<Action>& actualActions) const;
    
    // Learning signal
    float getLearningSignal(float error, float learningRate) const;
    void applyToSynapses(STDP& stdp, float weight = 1.0f);
    
    // Statistics
    float getAverageError() const;
    size_t getErrorCount() const;
    
    // Reset
    void reset();
```

### Cognitive Layer Classes

#### AttentionalSelection
```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    
    // Competition-based selection
    NeuronId selectTarget(const std::vector<NeuronId>& candidates, float topPercent = 0.1f) const;
    std::vector<float> computeSelectionWeights(const std::vector<NeuronId>& candidates) const;
    
    // Bias modulation
    void setTopDownBias(float bias);
    void setBottomUpBias(float bias);
    
    // Dynamic modulation
    void updateWithNeuromodulation(float neuromodulationLevel);
    void updateWithPrediction(float predictionConfidence);
    
    // Statistics
    float getSelectionBias() const;
    float getSelectionConfidence() const;
    
    // Reset
    void reset();
```

#### NeuralPlanner
```cpp
class NeuralPlanner {
public:
    NeuralPlanner();
    
    // Multi-step planning
    std::vector<Action> planActions(const Brain& brain, size_t horizon, float timeStep = 1.0f) const;
    float evaluateAction(const Action& action, const Brain& brain, size_t steps) const;
    
    // Goal-based selection
    void setGoal(const std::string& goal);
    std::string getGoal() const;
    Action selectBestAction(const std::vector<Action>& actions, const Brain& brain) const;
    
    // Prediction integration
    void updateWithPredictions(const std::vector<std::vector<Action>>& predictions);
    void setPredictionConfidence(float confidence);
    
    // Statistics
    float getPlanQuality() const;
    size_t getPlanSteps() const;
    
    // Reset
    void reset();
```

#### SelfModel
```cpp
class SelfModel {
public:
    SelfModel();
    
    // Body schema learning
    void updateBodySchema(const AgentBody& body, const std::vector<Action>& actions, const std::vector<ActionResult>& results);
    std::vector<float> predictActionEffects(Action action) const;
    
    // Capability representation
    void recordCapability(const std::string& capability, float successRate);
    float queryCapability(const std::string& capability) const;
    
    // Self vs external distinction
    float computeActionOwnership(Action action) const;
    void updateSelfOtherDistinction(float distinction);
    
    // Statistics
    std::map<std::string, float> getCapabilities() const;
    float getSelfOtherDistinction() const;
    
    // Reset
    void reset();
```

#### SocialLearning
```cpp
class SocialLearning {
public:
    SocialLearning();
    
    // Observation learning
    void observeOtherAgent(const Brain& otherBrain, const std::vector<Action>& observedActions);
    std::vector<Action> learnFromObservation(const std::vector<Action>& observedActions) const;
    
    // Imitation
    bool canImitate(const std::vector<Action>& actions) const;
    Action imitateAction(const std::vector<Action>& actions) const;
    
    // Communication
    void sendSimpleSignal(float signal);
    float receiveSimpleSignal() const;
    
    // Statistics
    size_t getObservationCount() const;
    float getImitationSuccessRate() const;
    
    // Reset
    void reset();
```

### Sensory Layer Classes

#### Vision
```cpp
class Vision : public SensoryInput {
public:
    Vision();
    Vision(size_t width, size_t height, size_t channels = 3);
    
    // Data management
    void setData(const std::vector<float>& data);
    const std::vector<float>& getData() const;
    void setData(const std::vector<std::vector<float>>& imageData);
    std::vector<std::vector<float>> getImageData() const;
    
    // Dimensions
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    size_t getDimensions() const;
    
    // Processing
    void applyFilter(const std::vector<float>& kernel);
    void enhanceContrast(float factor);
    void reduceNoise(float sigma);
    
    // Statistics
    float getMeanIntensity() const;
    float getVariance() const;
    
    // Validation
    bool isValidDataSize(size_t size) const;
```

#### Audio
```cpp
class Audio : public SensoryInput {
public:
    Audio();
    Audio(size_t sampleRate, size_t numSamples);
    
    // Data management
    void setData(const std::vector<float>& data);
    const std::vector<float>& getData() const;
    void setSamples(const std::vector<float>& samples);
    std::vector<float> getSamples() const;
    
    // Properties
    size_t getSampleRate() const;
    size_t getNumSamples() const;
    size_t getDuration() const;
    float getFrequency(float index) const;
    
    // Processing
    void applyFilter(const std::vector<float>& filterCoefficients);
    void applyWindow(const std::vector<float>& window);
    void computeFFT(std::vector<float>& magnitude, std::vector<float>& phase) const;
    
    // Statistics
    float getRMS() const;
    float getPeak() const;
    float getZeroCrossingRate() const;
    
    // Validation
    bool isValidDataSize(size_t size) const;
```

### Motor Layer Classes

#### Action
```cpp
class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& parameters);
    
    // Type
    ActionType getType() const;
    void setType(ActionType type);
    const std::string& getTypeName() const;
    
    // Parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& parameters);
    void addParameter(float value);
    size_t getParameterCount() const;
    float getParameter(size_t index) const;
    
    // Properties
    const std::string& getName() const;
    float getValue() const;
    
    // Utility
    std::string toString() const;
    Action clone() const;
    
    // Validation
    bool isValid() const;
```

#### MotorSystem
```cpp
class MotorSystem {
public:
    MotorSystem();
    
    // Command generation
    Action generateAction(const Brain& brain, float timeStep = 1.0f) const;
    std::vector<Action> generateActions(const Brain& brain, size_t count, float timeStep = 1.0f) const;
    
    // Decoding
    void setDecodingParameters(float threshold, float gain, float noiseLevel);
    float decodeNeuralActivity(const std::vector<float>& neuralActivity, ActionType type) const;
    
    // Learning
    void learnFromFeedback(const Action& action, const ActionResult& result, float learningRate);
    void updatePolicy(const std::vector<Action>& actions, const std::vector<float>& values);
    
    // Statistics
    float getActionSelectionEntropy() const;
    float getPolicyStability() const;
    
    // Reset
    void reset();
```

### Environment Layer Classes

#### WorldObject
```cpp
class WorldObject {
public:
    WorldObject();
    WorldObject(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f);
    
    // Position
    float getX() const;
    void setX(float x);
    float getY() const;
    void setY(float y);
    std::pair<float, float> getPosition() const;
    void setPosition(float x, float y);
    
    // Geometry
    float getRadius() const;
    void setRadius(float radius);
    float getArea() const;
    
    // Properties
    WorldObjectType getType() const;
    void setType(WorldObjectType type);
    const std::string& getTypeName() const;
    float getValue() const;
    void setValue(float value);
    bool isActive() const;
    void setActive(bool active);
    
    // Utility
    float distanceTo(float x, float y) const;
    float distanceTo(const WorldObject& other) const;
    bool intersects(const WorldObject& other) const;
    std::string toString() const;
    
    // Validation
    bool isValid() const;
```

#### AgentBody
```cpp
class AgentBody {
public:
    AgentBody();
    AgentBody(float x, float y);
    
    // Position
    float getX() const;
    void setX(float x);
    float getY() const;
    void setY(float y);
    
    // Orientation
    float getOrientation() const;
    void setOrientation(float angle);
    void rotate(float angle);
    
    // Velocity
    float getVelocityX() const;
    void setVelocityX(float vx);
    float getVelocityY() const;
    void setVelocityY(float vy);
    float getSpeed() const;
    float getDirection() const;
    void setVelocity(float vx, float vy);
    
    // Dynamics
    float getAngularVelocity() const;
    void setAngularVelocity(float omega);
    void applyTorque(float torque);
    
    // State
    float getEnergy() const;
    void setEnergy(float energy);
    void addEnergy(float delta);
    float getHealth() const;
    void setHealth(float health);
    void damage(float amount);
    void heal(float amount);
    float getAge() const;
    void setAge(float age);
    
    // State flags
    bool isMoving() const;
    void setIsMoving(bool moving);
    bool isTurning() const;
    void setIsTurning(bool turning);
    float getLastActionTime() const;
    void setLastActionTime(float time);
    
    // Actions
    void move(float dx, float dy);
    void turn(float angle);
    void reset();
    std::string toString() const;
    
    // Validation
    bool isValid() const;
```

#### SimpleWorld
```cpp
class SimpleWorld {
public:
    SimpleWorld();
    
    // Configuration
    void configure(int width, int height, int visionWidth = 10, int visionHeight = 10);
    int getWidth() const;
    int getHeight() const;
    
    // Agent
    void setAgentStart(float x, float y);
    void setAgent(const AgentBody& agent);
    AgentBody& getAgent();
    const AgentBody& getAgent() const;
    
    // Objects
    void addObject(const WorldObject& object);
    void removeObject(float x, float y);
    std::vector<WorldObject> getObjectsInRange(float x, float y, float range) const;
    std::vector<WorldObject> getVisibleObjects(const AgentBody& agent, float range) const;
    
    // Physics
    void update(float dt);
    void applyPhysics(float dt);
    void resolveCollisions();
    
    // Simulation
    void reset();
    void setTime(float time);
    float getTime() const;
    
    // Utilities
    bool isValidPosition(float x, float y) const;
    float distance(float x1, float y1, float x2, float y2) const;
    float angle(float x1, float y1, float x2, float y2) const;
    
    // Statistics
    int getObjectCount() const;
    float getTotalEnergy() const;
    
    // External interfaces
    SensoryPercept getSensoryPercept(const AgentBody& agent) const;
    ActionResult applyMotorCommand(const Action& action, float currentTime);
    
    // Reset
    void resetSimulation();
```

### Agent Layer Classes

#### SensoryPercept
```cpp
class SensoryPercept {
public:
    SensoryPercept();
    
    // Sensory inputs
    void setVision(const Vision& vision);
    const Vision& getVision() const;
    void setTouch(const std::vector<float>& touchData);
    const std::vector<float>& getTouch() const;
    void setInternal(const std::vector<float>& internalData);
    const std::vector<float>& getInternal() const;
    void setProprioception(const Proprioception& proprioception);
    const Proprioception& getProprioception() const;
    void setAudio(const Audio& audio);
    const Audio& getAudio() const;
    
    // Combined perception
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    void setTimestamp(Timestamp timestamp);
    Timestamp getTimestamp() const;
    
    // Reset
    void reset();
```

#### ActionResult
```cpp
class ActionResult {
public:
    ActionResult();
    ActionResult(float reward, bool success, const std::string& message = "");
    
    // Reward
    float getReward() const;
    void setReward(float reward);
    void addReward(float delta);
    
    // Success
    bool isSuccess() const;
    void setSuccess(bool success);
    
    // Message
    const std::string& getMessage() const;
    void setMessage(const std::string& message);
    
    // Utility
    std::string toString() const;
    
    // Reset
    void reset();
```

#### AgentBrain
```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Sensory processing
    bool processSensoryInput(const SensoryPercept& percept);
    
    // Motor output
    Action decodeMotorCommand();
    ActionResult applyAction(const Action& action, float currentTime);
    
    // Learning
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(int timestep);
    
    // State
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Control
    void reset();
    bool isAlive() const;
    void setAlive(bool alive);
    
    // Access
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Features
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Configuration
    std::shared_ptr<Config> getConfiguration() const;
```

#### AgentBody (World Object Extension)
```cpp
class AgentBody : public WorldObject {
public:
    AgentBody();
    AgentBody(float x, float y);
    
    // Position (inherited)
    using WorldObject::getX;
    using WorldObject::setX;
    using WorldObject::getY;
    using WorldObject::setY;
    
    // Agent-specific properties
    float getEnergy() const;
    void setEnergy(float energy);
    float getHealth() const;
    void setHealth(float health);
    float getAge() const;
    void setAge(float age);
    bool isMoving() const;
    void setIsMoving(bool moving);
    bool isTurning() const;
    void setIsTurning(bool turning);
    float getLastActionTime() const;
    void setLastActionTime(float time);
    
    // Actions
    void move(float dx, float dy);
    void turn(float angle);
    void update(float dt);
    void reset();
    
    // Validation
    bool isAlive() const;
    void die();
    void resurrect();
```

### Experiment Layer Classes

#### Experiment
```cpp
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    
    // Identification
    const std::string& getName() const;
    void setName(const std::string& name);
    ExperimentId getId() const;
    
    // Data collection
    void recordStep(int step, const Brain& brain, const SimpleWorld& world);
    void recordMetrics(const std::map<std::string, float>& metrics);
    std::map<std::string, float> getMetrics() const;
    std::vector<std::pair<int, std::map<std::string, float>>> getHistory() const;
    
    // Analysis
    float getMetric(const std::string& name) const;
    float getAverage(const std::string& name) const;
    float getStandardDeviation(const std::string& name) const;
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Reset
    void reset();
```

#### ExperimentRunner
```cpp
class ExperimentRunner {
public:
    ExperimentRunner();
    
    // Execution
    void runExperiment(const Experiment& experiment, const BrainConfig& config, int steps);
    void runMultipleExperiments(const std::vector<Experiment>& experiments, const BrainConfig& config, int steps);
    
    // Results
    std::map<std::string, Experiment> getResults() const;
    std::vector<std::string> getExperimentNames() const;
    
    // Analysis
    void analyzeResults();
    std::map<std::string, float> compareExperiments(const std::string& experiment1, const std::string& experiment2) const;
    
    // Reset
    void reset();
```

#### Metrics
```cpp
class Metrics {
public:
    Metrics();
    
    // Recording
    void record(const std::string& name, float value);
    void record(const std::string& name, float value, int step);
    
    // Aggregation
    float getCurrent(const std::string& name) const;
    float getAverage(const std::string& name) const;
    float getMin(const std::string& name) const;
    float getMax(const std::string& name) const;
    float getSum(const std::string& name) const;
    size_t getCount(const std::string& name) const;
    
    // Statistics
    std::map<std::string, float> getStatistics() const;
    std::map<std::string, std::vector<float>> getTimeSeries() const;
    
    // Reset
    void reset();
    void reset(const std::string& name);
```

## C++ Build System Configuration

### CMake Configuration

#### Core CMakeLists.txt
```cmake
# NLM Core CMake Configuration
cmake_minimum_required(VERSION 3.22)
project(NLM VERSION 0.5.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# Platform-specific settings
if(WIN32)
    add_definitions(-DNOMINMAX -D_WIN32_WINNT=0x0601)
else()
    add_definitions(-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700)
endif()

# Installation paths
include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Build configurations
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
```

#### Python Bindings CMakeLists.txt
```cmake
# NLM Python Bindings CMake Configuration
cmake_minimum_required(VERSION 3.22)

project(pynlm_bindings
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Python bindings for NLM neural simulation framework"
)

# Python and pybind11 detection
set(PYTHON_BINDING_REQUIRED ON CACHE BOOL "Build Python bindings")

if(NOT PYTHON_BINDING_REQUIRED)
    return()
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED CONFIG)

# Compiler configuration
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type -fPIC)
endif()

# Python include directories
set(PYTHON_INCLUDE_DIRS
    ${Python_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIR}
)

# Core library sources
set(NLM_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types/Types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Random/Random.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Logger/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/SimulationClock/SimulationClock.cpp
    # ... other core sources
)

# Build core as static library
add_library(nlm_core_py STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core_py PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_core_py PUBLIC cxx_std_20)

# Python module using pybind11
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core_py)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Installation
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITEARCH}
)

# Python module metadata
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../pyproject.toml
    DESTINATION ${Python_SITEARCH}
    RENAME "nlm-${PROJECT_VERSION}.dist-info/pyproject.toml"
)
```

### Performance Configuration

#### Performance.hpp
```cpp
#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>

namespace nlm {
namespace performance {

struct PerformanceConfig {
    // Core performance settings
    size_t maxNeurons;
    size_t maxSynapses;
    size_t eventQueueSize;
    size_t timeBuckets;
    
    // Parallelism
    size_t numThreads;
    size_t chunkSize;
    bool enableSIMD;
    bool enableOpenMP;
    
    // Memory management
    size_t neuronPoolSize;
    size_t synapsePoolSize;
    size_t cacheLineSize;
    bool enableMemoryPool;
    
    // I/O and logging
    bool enableProfiling;
    bool enableLogging;
    std::string logFile;
    bool enableCheckpointing;
    std::string checkpointDirectory;
    
    // Scientific accuracy
    bool enableBiologicalFidelity;
    float adaptationFactor;
    float plasticityRate;
};

class PerformanceMonitor {
public:
    PerformanceMonitor(const PerformanceConfig& config);
    ~PerformanceMonitor();
    
    void startMeasurement(const std::string& operation);
    void endMeasurement(const std::string& operation);
    double getAverageTime(const std::string& operation) const;
    double getMinTime(const std::string& operation) const;
    double getMaxTime(const std::string& operation) const;
    double getTotalTime(const std::string& operation) const;
    size_t getOperationCount(const std::string& operation) const;
    
    void logMetrics(const std::string& filename) const;
    void reset();
    
    static PerformanceMonitor& getInstance(const PerformanceConfig& config = PerformanceConfig{});
    
private:
    struct Measurement {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::string operation;
    };
    
    std::vector<Measurement> measurements_;
    std::map<std::string, std::vector<double>> times_;
    PerformanceConfig config_;
    std::mutex mutex_;
};

class MemoryPool {
public:
    MemoryPool(size_t neuronCapacity, size_t synapseCapacity);
    ~MemoryPool();
    
    // Neuron allocation
    NeuronId allocateNeuron();
    void deallocateNeuron(NeuronId id);
    Neuron& getNeuron(NeuronId id);
    const Neuron& getNeuron(NeuronId id) const;
    bool hasNeuron(NeuronId id) const;
    void reset();
    
    // Synapse allocation
    SynapseId allocateSynapse();
    void deallocateSynapse(SynapseId id);
    Synapse& getSynapse(SynapseId id);
    const Synapse& getSynapse(SynapseId id) const;
    bool hasSynapse(SynapseId id) const;
    void reset();
    
    // Statistics
    size_t getNeuronCapacity() const;
    size_t getSynapseCapacity() const;
    size_t getNeuronUsage() const;
    size_t getSynapseUsage() const;
    float getMemoryUsage() const;
    
    // Configuration
    void setNeuronCapacity(size_t capacity);
    void setSynapseCapacity(size_t capacity);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class EventQueue {
public:
    EventQueue(size_t capacity = 16384);
    
    // Event management
    void pushImmediate(NeuronId neuronId, Timestamp time);
    void pushDelayed(NeuronId neuronId, Timestamp time, float delay);
    std::vector<NeuronId> processEvents(Timestamp currentTime);
    
    // Statistics
    size_t getQueueSize() const;
    size_t getImmediateEvents() const;
    size_t getDelayedEvents() const;
    
    // Reset
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class TimeBucketer {
public:
    TimeBucketer(size_t numBuckets = 1024, float maxDelay = 100.0f);
    
    // Event scheduling
    void scheduleEvent(NeuronId neuronId, Timestamp time, float delay);
    std::vector<NeuronId> deliverEvents(Timestamp currentTime);
    
    // Configuration
    void setNumBuckets(size_t numBuckets);
    void setMaxDelay(float maxDelay);
    
    // Statistics
    size_t getNumBuckets() const;
    float getMaxDelay() const;
    size_t getTotalEvents() const;
    
    // Reset
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class SIMDVectorizer {
public:
    SIMDVectorizer();
    
    // Vectorized LIF update
    void updateNeurons(std::vector<Neuron>& neurons, const std::vector<float>& currents,
                      Timestamp time, float dt, size_t batchSize = 16);
    
    // Vectorized spike processing
    void processSpikes(std::vector<Neuron>& neurons, std::vector<Synapse>& synapses,
                      const EventQueue& spikeEvents, Timestamp time, float dt);
    
    // SIMD capabilities
    bool hasAVX512() const;
    bool hasAVX2() const;
    bool hasSSE() const;
    size_t getSIMDWidth() const;
    
    // Statistics
    uint64_t getOperationsPerformed() const;
    double getSIMDOfficiency() const;
    
    // Reset
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class ParallelProcessor {
public:
    ParallelProcessor(size_t numThreads = std::thread::hardware_concurrency());
    
    // Parallel for
    template<typename Iterator, typename Function>
    void parallelFor(Iterator begin, Iterator end, Function func);
    
    template<typename Iterator, typename Function, typename Reduction>
    Reduction parallelForReduce(Iterator begin, Iterator end, Function func, Reduction init);
    
    // Neural processing
    void processNeurons(std::vector<Neuron>& neurons, std::vector<Synapse>& synapses,
                       const EventQueue& spikeEvents, Timestamp time, float dt);
    void processSynapticTransmission(std::vector<Synapse>& synapses, const std::vector<Neuron>& neurons,
                                    const EventQueue& spikeEvents, Timestamp time);
    
    // Thread pool
    void setNumThreads(size_t numThreads);
    size_t getNumThreads() const;
    
    // Statistics
    double getParallelEfficiency() const;
    uint64_t getTotalTasks() const;
    uint64_t getCompletedTasks() const;
    
    // Reset
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace performance
} // namespace nlm
```

## Testing Framework

### Test Structure

#### Test Base Classes
```cpp
class NLMBrainTest {
public:
    virtual ~NLMBrainTest() = default;
    virtual void setup() = 0;
    virtual void teardown() = 0;
    virtual void runTest() = 0;
    std::string getName() const { return name_; }
    
protected:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    std::string name_;
};

class PerformanceTest : public NLMBrainTest {
public:
    virtual ~PerformanceTest() = default;
    virtual double getExpectedPerformance() const = 0;
    virtual double getMeasuredPerformance() = 0;
    virtual double getTolerance() const { return 0.1; }
    
    bool passes() {
        double expected = getExpectedPerformance();
        double measured = getMeasuredPerformance();
        double tolerance = getTolerance();
        return std::abs(measured - expected) <= tolerance * expected;
    }
};
```

#### Test Fixtures
```cpp
class BrainFixture {
public:
    void SetUp();
    void TearDown();
    
    // Brain configuration
    std::shared_ptr<Config> createDefaultConfig() const;
    std::shared_ptr<Brain> createBrain() const;
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) const;
    
    // Common test data
    std::vector<float> createRandomInput(size_t size) const;
    std::vector<float> createRandomSynapses(size_t size) const;
    
protected:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
};

class PerformanceFixture : public BrainFixture {
public:
    void SetUp() override;
    void TearDown() override;
    
    // Performance measurements
    double measureStepTime(int steps = 100) const;
    double measureMemoryUsage() const;
    double measureFiringRate() const;
    
    // Scaling tests
    void testScalability(size_t neuronCount, size_t synapseCount) const;
    
protected:
    std::shared_ptr<performance::PerformanceMonitor> monitor_;
    std::shared_ptr<performance::MemoryPool> memoryPool_;
};
```

### Test Categories

#### Unit Tests
```cpp
// Test suites
SUITE(TestCoreTypes)
{
    TEST(NeuronIdCreation)
    {
        NeuronId id1;
        NeuronId id2(123);
        ASSERT_NE(id1, id2);
        ASSERT_EQ(id2, NeuronId(123));
    }
    
    TEST(ConfigBasic)
    {
        Config config;
        config.set("test_bool", true);
        config.set("test_int", 42);
        config.set("test_double", 3.14);
        config.set("test_string", "hello");
        
        ASSERT_TRUE(config.get("test_bool"));
        ASSERT_EQ(config.get("test_int"), 42);
        ASSERT_DOUBLE_EQ(config.get("test_double"), 3.14);
        ASSERT_EQ(config.get("test_string"), "hello");
    }
}

SUITE(TestNeuronDynamics)
{
    TEST(LIFNeuronBasic)
    {
        LIFDynamics lif;
        Neuron neuron(NeuronType::Excitatory, 1.0f);
        neuron.stepLIF(0.0f, 0.01f);
        // Test basic behavior
    }
    
    TEST(RefractoryPeriod)
    {
        Neuron neuron(NeuronType::Excitatory);
        neuron.setFiringState(FiringState::Active);
        neuron.stepLIF(0.0f, 0.01f);
        ASSERT_EQ(neuron.getFiringState(), FiringState::Refractory);
    }
}
```

#### Integration Tests
```cpp
// Test brain integration
TEST_F(BrainFixture, BasicBrainOperation)
{
    brain_->initialize();
    EXPECT_TRUE(brain_->getTotalNeuronCount() > 0);
    
    // Test sensory input
    Vision vision(10, 10, 3);
    std::vector<float> data(90, 0.5f);
    vision.setData(data);
    brain_->receiveSensoryInput(vision);
    
    // Test stepping
    brain_->step(0, 0.0f);
    EXPECT_GT(brain_->getTotalSpikeCount(), 0);
}

// Test plasticity
TEST_F(BrainFixture, STDPPlasticity)
{
    brain_->initialize();
    
    // Create neurons with known IDs
    NeuronId preId, postId;
    
    // Apply STDP
    STDP stdp;
    std::vector<Synapse> synapses = brain_->getAllSynapses();
    
    for (auto& synapse : synapses) {
        Neuron preNeuron = synapse.getPreNeuron();
        Neuron postNeuron = synapse.getPostNeuron();
        
        // Pre-synaptic spike
        synapse.applySTDP(preNeuron, postNeuron, false);
        
        // Post-synaptic spike
        synapse.applySTDP(preNeuron, postNeuron, true);
    }
}
```

#### Performance Tests
```cpp
// Test performance benchmarks
TEST_F(PerformanceFixture, LFIBenchmark)
{
    double expectedTime = 0.001; // seconds per neuron
    double measuredTime = measureStepTime(1000);
    
    ASSERT_LT(measuredTime, expectedTime * 1000); // Allow 1000x tolerance
}

TEST_F(PerformanceFixture, MemoryUsage)
{
    double memoryUsage = measureMemoryUsage();
    double maxMemory = 100.0 * 1024 * 1024; // 100 MB
    
    ASSERT_LT(memoryUsage, maxMemory);
}

TEST_F(PerformanceFixture, Scaling)
{
    testScalability(1000, 10000);
    testScalability(10000, 100000);
    testScalability(100000, 1000000);
}
```

### Testing Configuration

#### CMake Testing Integration
```cmake
# Enable testing
enable_testing()

# Add test executables
add_executable(nlm_tests test/main.cpp)
target_link_libraries(nlm_tests nlm_core_py)
add_test(NAME nlm_tests COMMAND nlm_tests)

# Add Python tests if available
if(PYTHON_BINDING_REQUIRED)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    pybind11_add_module(pynlm_tests test/python_tests.cpp)
    target_link_libraries(pynlm_tests nlm_core_py pybind11::module)
    add_test(NAME pynlm_tests COMMAND ${Python_EXECUTABLE} -m pytest)
endif()

# Performance benchmarks
add_executable(nlm_benchmarks benchmark/main.cpp)
target_link_libraries(nlm_benchmarks nlm_core_py performance)
add_test(NAME nlm_benchmarks COMMAND nlm_benchmarks)
```

## Examples and Demos

### Basic Examples

#### Example 1: Creating a Simple Brain
```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create default configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 1000);
    config->set("neuron_count", 100);
    config->set("synapse_density", 0.1f);
    
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.01f);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ", "
                     << "Active neurons: " << brain->getActiveNeuronCount() << ", "
                     << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Save results
    brain->save("results/brain_state.bin");
    
    return 0;
}
```

#### Example 2: Python Basic Usage
```python
import pynlm
import numpy as np

# Create default configuration
config = pynlm.create_default_config()
config.set("simulation_steps", 1000)
config.set("neuron_count", 100)
config.set("synapse_density", 0.1f)

# Create brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.01)
    
    if step % 100 == 0:
        print(f"Step {step}, Active neurons: {brain.get_active_neuron_count()}, "
              f"Firing neurons: {brain.get_firing_neuron_count()}")

# Save results
brain.save("results/brain_state.bin")
print("Simulation completed and saved!")
```

### Intermediate Examples

#### Example 3: Sensory Input Processing
```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 500);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create visual input
    Vision vision(20, 20, 3);
    std::vector<float> imageData(1200, 0.0f);
    
    // Fill with a simple pattern (white square in center)
    for (int y = 8; y < 12; ++y) {
        for (int x = 8; x < 12; ++x) {
            size_t index = y * 20 * 3 + x * 3;
            imageData[index] = 1.0f;     // Red
            imageData[index + 1] = 1.0f; // Green
            imageData[index + 2] = 1.0f; // Blue
        }
    }
    
    vision.setData(imageData);
    
    // Send input to brain
    brain->receiveSensoryInput(vision);
    
    // Run a few steps to process
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.01f);
    }
    
    // Visualize results
    auto activity = brain->visualizeNeuronActivity();
    brain->saveVisualization("results/vision_response.png");
    
    return 0;
}
```

#### Example 4: Python Advanced Usage
```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create enhanced configuration
config = pynlm.create_brain_config("vision_test", 2000)

# Create agent brain with world
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(50, 50)

agent_brain = pynlm.create_agent_brain("agent", brain, world)
agent_brain.initialize(world)

# Create agent
agent = pynlm.create_nlm_agent("vision_agent", brain, world, config)

# Run episode
episode_stats = agent.run_episode("vision_demo", 100)

# Analyze results
print(f"Episode statistics: {episode_stats}")
print(f"Final energy: {agent.get_energy()}")
print(f"Total actions: {len(episode_stats['actions'])}")

# Visualize neural activity
activity = brain.visualize_neuron_activity()
plt.imshow(activity)
plt.title("Neural Activity Heatmap")
plt.savefig("results/neural_activity.png")
plt.show()
```

### Advanced Examples

#### Example 5: Learning with Neuromodulation
```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create configuration with neuromodulation
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 1000);
    config->set("enable_reward_modulation", true);
    config->set("enable_curiosity", true);
    config->set("enable_structural_plasticity", true);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(30, 30, 10, 10);
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain, world);
    agent->initialize(*world);
    
    // Create environment
    Vision vision(10, 10, 3);
    
    // Run learning episode
    float totalReward = 0.0f;
    for (int step = 0; step < 200; ++step) {
        // Generate action
        Action action = agent->decodeMotorCommand();
        
        // Apply to world
        ActionResult result = world->applyMotorCommand(action, step * 0.01f);
        
        // Process sensory input
        SensoryPercept percept = world->getSensoryPercept(agent->getAgentBody());
        agent->processSensoryInput(percept);
        
        // Apply reward modulation
        float reward = result.getReward();
        float predictedReward = 0.1f; // Simple prediction
        agent->applyRewardModulation(reward, predictedReward);
        
        // Update development
        agent->updateDevelopment(step);
        
        totalReward += reward;
        
        // Step brain
        brain->step(step, step * 0.01f);
        
        if (step % 50 == 0) {
            std::cout << "Step " << step << ", Reward: " << reward 
                     << ", Total: " << totalReward << std::endl;
        }
    }
    
    std::cout << "Learning completed. Total reward: " << totalReward << std::endl;
    
    // Save learned weights
    brain->save("results/learned_brain.bin");
    
    return 0;
}
```

#### Example 6: Python Performance Monitoring
```python
import pynlm
import numpy as np
import time

# Create performance-optimized configuration
config = pynlm.create_brain_config("performance_test", 5000)

# Create brain with memory pool
brain = pynlm.create_brain(config, thread_safe=True, precision_mode="double")
brain.initialize()

# Create timer for performance monitoring
timer = pynlm.create_timer("simulation_step")

# Run performance test
start_time = time.time()
num_steps = 1000

for step in range(num_steps):
    step_start = time.time()
    
    # Generate random sensory input
    vision = pynlm.create_vision(20, 20, 3)
    random_data = np.random.rand(1200).astype(np.float32)
    vision.set_data(random_data)
    
    # Process input
    brain.receive_sensory_input(vision)
    
    # Step simulation
    brain.step(step, step * 0.01)
    
    step_time = time.time() - step_start
    timer_elapsed = timer.elapsed()
    
    if step % 100 == 0:
        print(f"Step {step}: {step_time*1000:.2f}ms, "
              f"Timer: {timer_elapsed:.4f}s")

end_time = time.time()
total_time = end_time - start_time
average_step_time = total_time / num_steps

print(f"Performance summary:")
print(f"  Total time: {total_time:.2f}s")
print(f"  Average step time: {average_step_time*1000:.2f}ms")
print(f"  Steps per second: {num_steps/total_time:.1f}")

# Export results for analysis
state = pynlm.export_brain_state(brain)
np.save("results/brain_state.npy", state)
print("Results saved to brain_state.npy")
```

### Example: Simulation Manager
```cpp
#include <nlm.h>
#include <iostream>
#include <fstream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 5000);
    
    // Create brain and world
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(40, 40, 8, 8);
    
    // Create simulation manager
    auto simManager = std::make_shared<SimulationManager>(brain, world, config);
    
    // Run multiple episodes
    std::vector<std::string> episodeIds;
    
    for (int episode = 0; episode < 10; ++episode) {
        std::string episodeId = "episode_" + std::to_string(episode);
        episodeIds.push_back(episodeId);
        
        std::cout << "Running episode " << episode << std::endl;
        
        // Run episode
        auto stats = simManager->runEpisode(episodeId, 200);
        
        // Save checkpoint
        simManager->saveCheckpoint(episodeId, 199);
        
        // Print statistics
        std::cout << "  Reward: " << stats["reward"] << std::endl;
        std::cout << "  Steps: " << stats["steps"] << std::endl;
        std::cout << "  Success rate: " << stats["success_rate"] * 100 << "%" << std::endl;
    }
    
    // Analyze all episodes
    std::cout << std::endl << "Episode Analysis:" << std::endl;
    for (const auto& episodeId : episodeIds) {
        auto stats = simManager->getEpisodeStats(episodeId);
        std::cout << episodeId << ": "
                 << "Reward=" << stats["reward"] << ", "
                 << "Success=" << stats["success_rate"] << std::endl;
    }
    
    // Restore from checkpoint
    std::cout << std::endl << "Restoring from checkpoint..." << std::endl;
    simManager->restoreEpisode("episode_5", 100);
    
    // Continue simulation
    auto stats = simManager->runEpisode("episode_5", 100, 150);
    
    // Cleanup old episodes
    simManager->cleanupOldEpisodes(5);
    
    return 0;
}
```

### Example: Visualization Tools
```cpp
#include <nlm.h>
#include <iostream>
#include <fstream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 1000);
    
    // Create brain and world
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(50, 50, 10, 10);
    
    // Create visualizer
    auto visualizer = std::make_shared<Visualizer>(brain, world);
    
    // Run simulation and collect data
    std::vector<std::vector<float>> activityHistory;
    std::vector<std::vector<float>> connectionHistory;
    std::vector<std::vector<float>> regionHistory;
    
    brain->initialize();
    
    for (int step = 0; step < 500; ++step) {
        brain->step(step, step * 0.01f);
        
        // Collect visualization data
        if (step % 10 == 0) {
            activityHistory.push_back(
                visualizer->visualizeNeuronActivity(*brain)
            );
            
            connectionHistory.push_back(
                visualizer->visualizeSynapticConnections(*brain)
            );
            
            regionHistory.push_back(
                visualizer->visualizeBrainRegions(*brain)
            );
        }
    }
    
    // Generate spike raster
    auto spikeData = visualizer->generateSpikeRaster(*brain);
    
    // Visualize network topology
    auto topologyData = visualizer->visualizeNetworkTopology(*brain);
    
    // Export for external tools
    visualizer->exportForExternalTool("spike_data.json", "spike_raster", spikeData);
    visualizer->exportForExternalTool("network_data.json", "network_topology", topologyData);
    
    // Generate static plots
    visualizer->plotToFile("activity_heatmap.png", "activity", activityHistory);
    visualizer->plotToFile("connection_matrix.png", "connections", connectionHistory);
    visualizer->plotToFile("region_activity.png", "regions", regionHistory);
    
    // Generate interactive HTML
    visualizer->generateInteractiveHTML("results/interactive_visualization.html");
    
    std::cout << "Visualization completed and saved to results/" << std::endl;
    
    return 0;
}
```

## Contribution Guidelines

### Setting Up Development Environment

#### Prerequisites
```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential cmake git python3 python3-pip python3-dev

# Install Python dependencies
pip3 install pybind11 numpy matplotlib scipy

# Install C++ dependencies
cmake --version
python3 -c "import pybind11; print(pybind11.__version__)"
```

#### Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build all targets
make -j$(nproc)

# Build Python bindings specifically
make pynlm

# Build tests
make nlm_tests

# Build benchmarks
make nlm_benchmarks
```

#### Running Tests
```bash
# Run C++ tests
./bin/nlm_tests

# Run Python tests (if available)
python3 -m pytest

# Run benchmarks
./bin/nlm_benchmarks
```

### Code Style and Conventions

#### C++ Style
```cpp
// Naming conventions
class MyClass {           // CamelCase for classes
public:
    void myMethod();    // camelCase for methods
    int myVariable;      // camelCase for variables
    
    static const int MAX_SIZE = 100;  // UPPER_SNAKE_CASE for constants
};

// Indentation (4 spaces)
if (condition) {
    doSomething();
    if (anotherCondition) {
        doSomethingElse();
    }
}

// Line length (maximum 120 characters)
// Keep lines shorter than 120 characters for readability

// Comments (Doxygen style)
/**
 * @brief Brief description of what this function does
 * @param[in] param1 Description of input parameter 1
 * @param[out] param2 Description of output parameter 2
 * @return Return value description
 * @throws std::runtime_error Description if exception is thrown
 */
void myFunction(int param1, std::string& param2);
```

#### Python Style
```python
# Naming conventions
def my_function(param1, param2):  # snake_case for functions/variables
    """Brief description of function
    
    Args:
        param1: Description of parameter 1
        param2: Description of parameter 2
    
    Returns:
        Description of return value
    
    Raises:
        ExceptionType: Description if exception is raised
    """
    pass

class MyClass:  # PascalCase for classes
    """Brief description of class."""
    
    def __init__(self, param1, param2):
        """Initialize with given parameters.
        
        Args:
            param1: Description of parameter 1
            param2: Description of parameter 2
        """
        self.param1 = param1
        self.param2 = param2
    
    def my_method(self, arg1, arg2=None):
        """Method description.
        
        Args:
            arg1: Description of arg1
            arg2: Optional parameter, defaults to None
        
        Returns:
            Description of return value
        """
        pass
```

### Testing Best Practices

#### Unit Testing
```cpp
// Use testing framework
#include <gtest/gtest.h>

TEST(NeuronTest, BasicProperties) {
    Neuron neuron(NeuronType::Excitatory);
    EXPECT_EQ(neuron.getType(), NeuronType::Excitatory);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), 0.0f);
}

TEST(ConfigTest, BasicOperations) {
    Config config;
    config.set("test", 42);
    EXPECT_TRUE(config.has("test"));
    EXPECT_EQ(config.get<int>("test"), 42);
}
```

#### Performance Testing
```cpp
// Performance assertions
TEST_P(PerformanceTest, LFIBenchmark) {
    double expectedTime = 0.001;  // seconds
    double measuredTime = measureStepTime(1000);
    
    ASSERT_LT(measuredTime, expectedTime * 1000 * 10);  // 10x tolerance
}

TEST_P(PerformanceTest, MemoryUsage) {
    double memoryUsage = measureMemoryUsage();
    double maxMemory = 100.0 * 1024 * 1024;  // 100 MB
    
    ASSERT_LT(memoryUsage, maxMemory);
}
```

### Build and Deployment Procedures

#### Building for Production
```bash
# Release build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Install locally
cmake --install . --prefix ./install

# Create package
make package
```

#### Building for Distribution
```bash
# Create DEB package (Debian/Ubuntu)
cmake -DBUILD_DEB=ON ..
make package

# Create RPM package (Fedora/RHEL/CentOS)
cmake -DBUILD_RPM=ON ..
make package

# Create Python package
python3 setup.py bdist_wheel
alpine wheel build
```

### Performance Optimization Guidelines

#### General Principles
1. **Memory Efficiency**
   - Use pre-allocated pools for neurons and synapses
   - Prefer value semantics over reference semantics
   - Cache frequently accessed data

2. **CPU Optimization**
   - Vectorize hot loops with SIMD instructions
   - Use cache-friendly data layouts (SoA)
   - Minimize function call overhead

3. **I/O Optimization**
   - Use efficient serialization formats
   - Batch write operations
   - Compress checkpoint data

#### Profiling and Optimization
```cpp
// Example profiling code
#include <nlm/performance/PerformanceMonitor.hpp>

void criticalFunction() {
    auto& monitor = nlm::performance::PerformanceMonitor::getInstance();
    
    monitor.startMeasurement("critical_function");
    
    // Critical code
    for (int i = 0; i < N; ++i) {
        computeSomething(i);
    }
    
    monitor.endMeasurement("critical_function");
    
    double avgTime = monitor.getAverageTime("critical_function");
    std::cout << "Average time: " << avgTime * 1000 << " ms" << std::endl;
}
```

#### Memory Pool Configuration
```cpp
// Configure memory pools for large-scale simulations
nlm::performance::PerformanceConfig config;
config.maxNeurons = 1000000;  // 1 million neurons
config.maxSynapses = 10000000;  // 10 million synapses
config.neuronPoolSize = 2 * config.maxNeurons;
config.synapsePoolSize = 2 * config.maxSynapses;
config.enableMemoryPool = true;
config.enableSIMD = true;
config.numThreads = std::thread::hardware_concurrency();

auto& monitor = nlm::performance::PerformanceMonitor::getInstance(config);
auto memoryPool = std::make_shared<nlm::performance::MemoryPool>(
    config.neuronPoolSize, config.synapsePoolSize
);
```

## Development Documentation

### Project Structure

#### Source Code Layout
```
src/
├── core/
│   ├── Types/                    # Core type definitions
│   │   ├── Types.hpp
│   │   └── Types.cpp
│   ├── Config/                   # Configuration system
│   │   ├── Config.hpp
│   │   └── Config.cpp
│   ├── Random/                   # Random number generation
│   │   ├── Random.hpp
│   │   └── Random.cpp
│   └── Logger/                   # Logging system
│       ├── Logger.hpp
│       └── Logger.cpp
├── brain/                        # Main brain implementation
│   ├── Brain.hpp
│   ├── Brain.cpp
│   ├── Neuron.hpp
│   ├── Neuron.cpp
│   ├── Synapse.hpp
│   ├── Synapse.cpp
│   └── ... (other brain components)
├── ... (other layers)
```

#### Python Bindings
```
python/
├── bindings.cpp          # Main pybind11 module
├── CMakeLists.txt         # Python bindings build system
└── ... (other Python files)
```

### Build System Configuration

#### CMake Configuration Files
```
build/
├── CMakeCache.txt           # CMake cache
├── CMakeFiles/               # CMake internal files
├── Makefile                   # Top-level makefile
├── lib/                       # Built libraries
├── bin/                       # Built executables
├── python/                    # Python module
│   ├── pynlm.cpython-*.so    # Compiled Python module
│   └── nlm-*.dist-info/       # Python metadata
└── ... (other build artifacts)
```

### Continuous Integration

#### GitHub Actions Workflow
```yaml
name: NLM CI/CD
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Set up CMake
        uses: abhay-ashwin/setup-cmake@v1.0.0
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.9'
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake python3-dev python3-pip
          pip3 install pybind11 numpy matplotlib scipy
      - name: Configure
        run: cmake -B build
      - name: Build
        run: cmake --build build --parallel
      - name: Run tests
        run: ./build/bin/nlm_tests
      - name: Run benchmarks
        run: ./build/bin/nlm_benchmarks
      - name: Build Python bindings
        run: |
          cd build
          make pynlm
          python3 -c "import sys; sys.path.insert(0, '.'); import pynlm; print('Python bindings work!')"
```

#### Testing Matrix
```yaml
strategy:
  matrix:
    build_type: [Debug, Release, RelWithDebInfo]
    compiler: [gcc, clang]
    python_version: ['3.8', '3.9', '3.10', '3.11']
    platform: [ubuntu-latest, macos-latest]
    exclude:
      - compiler: clang
        platform: ubuntu-latest
      - build_type: Debug
        platform: macos-latest
```

### Research Methods

#### Experimental Protocol
```cpp
// Standard experimental protocol
class NLMExperiment {
public:
    virtual ~NLMExperiment() = default;
    
    virtual void initialize() = 0;
    virtual void run() = 0;
    virtual void analyze() = 0;
    virtual void saveResults(const std::string& filepath) const = 0;
    
    // Common experimental utilities
    void setRandomSeed(uint64_t seed);
    void configureBrain(std::shared_ptr<Brain> brain, const std::string& condition);
    void recordTimeSeries(const std::string& name, const std::vector<float>& data);
    void recordStatistics(const std::string& name, double value);
    
protected:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::map<std::string, std::vector<float>> timeSeries_;
    std::map<std::string, std::vector<double>> statistics_;
};
```

#### Data Collection
```cpp
// Standard data collection methods
void NLMExperiment::recordTimeSeries(const std::string& name, const std::vector<float>& data) {
    timeSeries_[name] = data;
    
    // Also write to CSV for easy analysis
    std::string filename = "data/" + name + "_" + std::to_string(experimentId_) + ".csv";
    std::ofstream file(filename);
    if (file.is_open()) {
        for (size_t i = 0; i < data.size(); ++i) {
            file << i << "," << data[i] << std::endl;
        }
        file.close();
    }
}

void NLMExperiment::recordStatistics(const std::string& name, double value) {
    statistics_[name].push_back(value);
}
```

### Anti-Transformer Guidelines

NLM must NEVER become:
- A transformer architecture
- An LLM
- A pretrained model
- A conventional deep learning system
- "LLM + memory"
- "LLM + tools"

The architecture must always be based on:
- Individual neurons
- Synapses
- Spike-based communication
- Neural plasticity
- Developmental processes

If at any point the architecture begins to resemble a transformer, it should be redirected or the project renamed to clarify it is no longer NLM.

### Future Directions

#### Phase 6 Roadmap (NOT STARTED)
1. GPU acceleration for large-scale simulations
2. Distributed simulation across machines
3. More detailed neuron models (Hodgkin-Huxley)
4. Calcium-based STDP
5. Glial cell interactions
6. Anatomically constrained connectivity
7. Reinforcement learning integration
8. Hierarchical planning structures

#### Research Opportunities
- Multi-scale modeling (from ion channels to behavior)
- Neuromorphic hardware implementation
- Human brain connectome integration
- Cross-species comparative studies
- Cognitive development modeling
- Social brain networks

### License and Attribution

#### Software License
```
MIT License

Copyright (c) 2025 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

#### Acknowledgments

The NLM project builds on decades of research in computational neuroscience:
- Maass et al. (2002) on spiking neural networks
- Vogl et al. (1995) on dynamic brain models
- above frameworks and many other contributions to neural computation

Special thanks to the open-source community for pybind11, Eigen, and other essential libraries.

### Contact and Support

#### Bug Reports and Feature Requests
- Report bugs at: https://github.com/NLM/nlm/issues
- Request features at: https://github.com/NLM/nlm/issues
- Join discussions at: https://github.com/NLM/nlm/discussions

#### Documentation Issues
- Report documentation issues at: https://github.com/NLM/nlm/issues
- Propose improvements at: https://github.com/NLM/nlm/discussions

#### Academic Collaboration
- Email: research@nlm.org
- Academic partnerships: partnerships@nlm.org
- Industrial collaborations: industry@nlm.org

#### Community Support
- GitHub Discussions: For general questions and community support
- Stack Overflow: For coding questions with [nlm] tag
- Discord: For real-time discussions (invite link on website)

### Version History

#### Release Versions
- v0.1.0 (Phase 1): Initial skeleton release
- v0.2.0 (Phase 2): Working neural core with real spiking dynamics
- v0.3.0 (Phase 3): World interaction and developmental mechanisms
- v0.4.0 (Phase 4): Emerging cognition - prediction, memory, attention, planning, self-model, social learning
- v0.5.0 (Phase 5): Scaling + Research Platform - performance optimizations, biological fidelity, lifelong learning
- v0.6.0 (Phase 6): Future directions - GPU acceleration, distributed computing, advanced neuron models

#### Development Roadmap
1. **Phase 5 Implementation** (In Progress): Complete scaling infrastructure and research platform
2. **Phase 5 Testing** (Ongoing): Comprehensive test suite for all new features
3. **Phase 5 Documentation** (Current): Complete API reference and user guides
4. **Phase 6 Planning** (Upcoming): GPU and distributed computing strategies

### Acknowledgments

This documentation is generated automatically from source code and maintained by the NLM development team.

Special thanks to all contributors who have helped build the NLM framework.

### Updates and Changes

#### Documentation Updates
This documentation is continuously updated. Changes are tracked in:
- CHANGELOG.md
- docs/ROADMAP.md
- docs/PERFORMANCE.md

#### Code Updates
Source code changes are documented in:
- git history
- docs/SCIENCE.md (scientific changes)
- docs/PHASE6_FINAL_AUDIT.md (audit trails)

This documentation will be updated as the project evolves.
