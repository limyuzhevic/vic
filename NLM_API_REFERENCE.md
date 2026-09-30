# NLM API Reference

## Overview

This document provides a comprehensive reference for the NLM (Neural Learning Machine) API, covering both the C++ core API and the Python interface API.

## Table of Contents

### Part 1: Core NLM C++ API
1. [NLM Core API](#nlm-core-api)
   - [Brain Class](#brain-class)
   - [World Class](#world-class)
   - [Neuron Classes](#neuron-classes)
   - [Synaptic Connection Classes](#synaptic-connection-classes)
   - [Memory System Classes](#memory-system-classes)
   - [Plasticity Rule Classes](#plasticity-rule-classes)
   - [Neuromodulation Classes](#neuromodulation-classes)
   - [Prediction Classes](#prediction-classes)
   - [Cognition Classes](#cognition-classes)
   - [Development Classes](#development-classes)
   - [Event System](#event-system)
   - [Plugin System](#plugin-system)

### Part 2: NLM Python Interface API
2. [Python Interface API](#python-interface-api)
   - [Main Interface Class](#main-interface-class)
   - [Brain Controller Class](#brain-controller-class)
   - [Simulation Controller Class](#simulation-controller-class)
   - [Event System Classes](#event-system-classes)
   - [Plugin System Classes](#plugin-system-classes)
   - [Safety Classes](#safety-classes)
   - [Visualization Classes](#visualization-classes)
   - [Integration Classes](#integration-classes)

## Part 1: Core NLM C++ API

### NLM Core API

#### Brain Class

```cpp
class Brain {
public:
    // Construction and initialization
    Brain(const BrainConfig& config);
    Brain() = default;
    ~Brain();
    
    void initialize(const BrainConfig& config);
    void reset();
    
    // Neural operations
    void inject_stimulus(const NeuronStimulus& stimulus);
    void update_neural_dynamics(float dt);
    void process_plasticity(float dt);
    void update_neuromodulation(float dt);
    void update_prediction(float dt);
    void update_cognition(float dt);
    void update_development(float dt);
    
    // Memory operations
    void update_working_memory();
    void update_episodic_memory(const Experience& experience);
    void update_associative_memory(const Pattern& pattern);
    
    // Accessors
    const std::vector<Neuron>& get_neurons() const;
    const std::vector<Synapse>& get_synapses() const;
    const WorkingMemory& get_working_memory() const;
    const EpisodicMemory& get_episodic_memory() const;
    const AssociativeMemory& get_associative_memory() const;
    
    // State management
    json serialize_state() const;
    void deserialize_state(const json& state);
    
    // Statistics and monitoring
    BrainStatistics get_statistics() const;
    float get_total_activity() const;
    float get_firing_rate() const;
};
```

**Key Methods:**

- `initialize(const BrainConfig& config)`: Initialize brain with configuration
- `reset()`: Reset brain to initial state
- `inject_stimulus(const NeuronStimulus& stimulus)`: Inject external stimulus
- `update_neural_dynamics(float dt)`: Update neural dynamics with time step
- `process_plasticity(float dt)`: Process synaptic plasticity
- `get_statistics()`: Get comprehensive brain statistics

#### World Class

```cpp
class World {
public:
    // Construction and initialization
    World(int width, int height);
    World(const WorldConfig& config);
    
    // World state
    void update(float dt);
    void reset();
    
    // Sensory and motor interfaces
    SensoryInput get_sensory_input() const;
    void set_motor_output(const MotorCommand& command);
    
    // Environment properties
    const EnvironmentProperties& get_properties() const;
    
    // Environment interaction
    void apply_force(const Vec3& position, const Vec3& force);
    void add_obstacle(const Obstacle& obstacle);
    Vec3 get_position_at(const Vec3& location) const;
    
    // World simulation
    void simulate(float dt);
    void handle_collisions();
    void apply_physics(float dt);
    
    // State management
    json serialize_state() const;
    void deserialize_state(const json& state);
    
    // Statistics
    WorldStatistics get_statistics() const;
};
```

**Key Methods:**

- `get_sensory_input()`: Get current sensory input
- `set_motor_output()`: Set motor command output
- `update(float dt)`: Update world state
- `simulate(float dt)`: Run world simulation

#### Neuron Classes

##### Base Neuron Class

```cpp
class Neuron {
public:
    // Neuron properties
    enum class Type {
        EXCITATORY,
        INHIBITORY,
        MODULATORY,
        CUSTOM
    };
    
    // Core neuron properties
    float voltage;
    float current;
    float membrane_potential;
    float threshold;
    float reset_potential;
    float refractory_period;
    float refractory_timer;
    
    // Dynamics parameters
    float capacitance;
    float leak_conductance;
    float reversal_potential;
    
    // State
    NeuronState state;
    std::vector<Synapse*> inputs;
    std::vector<Synapse*> outputs;
    
    // Construction and initialization
    Neuron();
    Neuron(const NeuronConfig& config);
    
    // Core dynamics
    virtual void update(float dt);
    virtual void inject_current(float current);
    virtual bool should_spike() const;
    virtual void reset_spike();
    
    // Synapse management
    void add_input(Synapse* synapse);
    void add_output(Synapse* synapse);
    void remove_input(Synapse* synapse);
    void remove_output(Synapse* synapse);
    
    // Plasticity
    virtual void apply_plasticity(float dt, const PlasticityRule* rule);
    
    // Serialization
    json serialize() const;
    void deserialize(const json& data);
    
    // Statistics
    float get_firing_rate() const;
    float get_average_voltage() const;
};
```

##### Custom Neuron Types

```cpp
class IzhikevichNeuron : public Neuron {
public:
    IzhikevichNeuron();
    IzhikevichNeuron(const IzhikevichConfig& config);
    
    void update(float dt) override;
    void apply_plasticity(float dt, const PlasticityRule* rule) override;
    
    // Izhikevich specific parameters
    float a;
    float b;
    float c;
    float d;
    float vpeak;
};

class HodgkinHuxleyNeuron : public Neuron {
public:
    HodgkinHuxleyNeuron();
    HodgkinHuxleyNeuron(const HHConfig& config);
    
    void update(float dt) override;
    void apply_plasticity(float dt, const PlasticityRule* rule) override;
    
    // HH specific dynamics
    float m_gate;      // Sodium activation
    float h_gate;      // Sodium inactivation
    float n_gate;      // Potassium activation
    float dm_dt;       // dm/dt
    float dh_dt;       // dh/dt
    float dn_dt;       // dn/dt
};
```

#### Synaptic Connection Classes

```cpp
class Synapse {
public:
    // Synapse properties
    Neuron* pre_neuron;
    Neuron* post_neuron;
    float weight;
    float delay;
    float conductance;
    float reversal_potential;
    float tau_rise;
    float tau_fall;
    
    // State
    float current_weight;
    float current_conductance;
    float activation;
    float depression;
    float facilitation;
    
    // Plasticity
    std::vector<PlasticityRule*> plasticity_rules;
    
    // Construction and initialization
    Synapse(Neuron* pre, Neuron* post);
    Synapse(const SynapseConfig& config);
    
    // Transmission
    void transmit(float dt);
    void update_conductance(float dt);
    
    // Plasticity
    void apply_plasticity(float dt, const PlasticityRule* rule);
    
    // Serialization
    json serialize() const;
    void deserialize(const json& data);
    
    // Statistics
    float get_efficiency() const;
    float get_transmission_rate() const;
};
```

#### Memory System Classes

##### Working Memory

```cpp
class WorkingMemory {
public:
    struct Item {
        uint64_t id;
        float activation;
        float importance;
        float decay_rate;
        float time_to_live;
        std::vector<float> representation;
        std::chrono::steady_clock::time_point created_time;
        std::chrono::steady_clock::time_point last_accessed;
        bool is_active;
    };
    
    // Core functionality
    uint64_t store(const std::vector<float>& representation, 
                  float importance = 1.0f);
    Item* retrieve(uint64_t id);
    void update(float dt);
    void clear_inactive_items();
    
    // Competitive dynamics
    void competitive_encoding(const std::vector<float>& pattern);
    void winner_take_all();
    
    // Statistics and monitoring
    size_t get_item_count() const;
    float get_total_activation() const;
    json serialize() const;
    void deserialize(const json& data);
}; 
```

##### Episodic Memory

```cpp
class EpisodicMemory {
public:
    struct Episode {
        uint64_t id;
        std::chrono::steady_clock::time_point timestamp;
        Experience experience;
        std::vector<float> episode_vector;
        float strength;
        std::vector<uint64_t> related_episodes;
        std::vector<float> prediction_error;
    };
    
    // Core functionality
    uint64_t store(const Experience& experience);
    std::vector<Episode*> retrieve(const ExperienceQuery& query);
    void update(float dt);
    
    // Replay and consolidation
    void initiate_replay(const Episode& episode);
    void consolidate_important_episodes();
    
    // Statistics
    size_t get_episode_count() const;
    float get_total_memory_strength() const;
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Plasticity Rule Classes

```cpp
class PlasticityRule {
public:
    enum class Type {
        STDP,
        HEBBIAN,
        ANTI_HEBBIAN,
        OJA,
        BATCH_NORM,
        ERLANGIAN,
        CUSTOM
    };
    
    // Core properties
    float learning_rate;
    float trace_decay_rate;
    float max_weight;
    float min_weight;
    bool is_homeostatic;
    
    // Construction and initialization
    PlasticityRule(const PlasticityRuleConfig& config);
    
    // Core plasticity
    virtual float calculate_weight_change(Neuron* pre, Neuron* post,
                                         float pre_activity, float post_activity,
                                         float dt);
    
    // Trace management
    virtual void update_traces(float dt);
    virtual void reset_traces();
    
    // Homeostatic mechanisms
    virtual float calculate_homeostatic_correction(Neuron* neuron);
    
    // Serialization
    virtual json serialize() const;
    virtual void deserialize(const json& data);
    
    // Statistics
    virtual float get_total_weight_change() const;
};

class STDP : public PlasticityRule {
public:
    STDP();
    STDP(const STDPConfig& config);
    
    float calculate_weight_change(Neuron* pre, Neuron* post,
                                 float pre_activity, float post_activity,
                                 float dt) override;
    
    // STDP specific parameters
    float tau_plus;      // LTP time constant
    float tau_minus;     // LTD time constant
    float a_plus;        // LTP magnitude
    float a_minus;       // LTD magnitude
    float w_max;         // Maximum weight
};

class Oja : public PlasticityRule {
public:
    Oja();
    Oja(const OjaConfig& config);
    
    float calculate_weight_change(Neuron* pre, Neuron* post,
                                 float pre_activity, float post_activity,
                                 float dt) override;
    
    // Oja specific parameters
    float lambda;        // Oja normalization factor
};
```

#### Neuromodulation Classes

```cpp
class Neuromodulation {
public:
    enum class Type {
        DOPAMINE,
        ACETYLCHOLINE,
        NORADRENALINE,
        SEROTONIN,
        ANGIOTENSIN,
        CURIOUSITY,
        NOVELTY,
        CUSTOM
    };
    
    // Core properties
    float level;
    float baseline_level;
    float effect_on_excitability;
    float effect_on_plasticity;
    float time_constant;
    float decay_rate;
    
    // State
    float history_buffer[16];
    int buffer_index;
    
    // Construction and initialization
    Neuromodulation(Type type, const NeuromodulationConfig& config);
    
    // Core functionality
    void update(float dt);
    void set_level(float level);
    float get_effective_level() const;
    
    // Effects
    void apply_excitability_effect(Neuron* neuron, float dt);
    void apply_plasticity_effect(Synapse* synapse, float dt);
    void apply_behavior_effect(ActionSelection* action, float dt);
    
    // Serialization
    json serialize() const;
    void deserialize(const json& data);
};

class NeuromodulationSystem {
public:
    // Core functionality
    void initialize(const NeuromodulationConfig& config);
    void update(float dt);
    void add_neuromodulator(Neuromodulation::Type type, float level);
    void set_neuromodulator_level(Neuromodulation::Type type, float level);
    
    // Accessors
    float get_neuromodulator_level(Neuromodulation::Type type) const;
    const Neuromodulation& get_neuromodulator(Neuromodulation::Type type) const;
    
    // Effects
    void apply_neuromodulation_to_neuron(Neuron* neuron, float dt);
    void apply_neuromodulation_to_synapse(Synapse* synapse, float dt);
    
    // Statistics
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Prediction Classes

```cpp
class PredictionError {
public:
    // Error properties
    float value;
    float precision;
    float confidence;
    float surprise_level;
    float arousal_level;
    
    // Temporal dynamics
    float prediction_error_history[16];
    int history_index;
    
    // Construction and initialization
    PredictionError();
    
    void update(float error, float precision);
    float get_weighted_error() const;
    float get_recent_error_average(int window_size) const;
};

class PredictiveCoding {
public:
    // Core functionality
    void initialize(const PredictionConfig& config);
    void update(float dt);
    
    // Prediction operations
    float generate_prediction(const std::vector<float>& state);
    PredictionError calculate_prediction_error(const std::vector<float>& state);
    
    // Learning
    void update_weights(const PredictionError& error, float dt);
    
    // Accessors
    const std::vector<float>& get_prediction() const;
    const PredictionError& get_last_error() const;
    
    // Statistics
    float get_prediction_accuracy() const;
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Cognition Classes

```cpp
class Attention {
public:
    struct Focus {
        uint64_t target_id;
        float strength;
        float priority;
        float scope;
        std::vector<float> weighting;
    };
    
    // Core functionality
    void initialize(const AttentionConfig& config);
    void update(float dt);
    
    // Selection
    void evaluate_candidates(const std::vector<Focus>& candidates);
    Focus select_focus();
    
    // Modulation
    void modulate_neural_activity(const std::vector<Neuron*>& neurons);
    void modulate_plasticity(const std::vector<Synapse*>& synapses);
    
    // Statistics
    const Focus* get_current_focus() const;
    size_t get_candidate_count() const;
    json serialize() const;
    void deserialize(const json& data);
};

class NeuralPlanner {
public:
    struct ActionSequence {
        std::vector<Action> actions;
        float expected_utility;
        float probability;
        float time_required;
    };
    
    // Core functionality
    void initialize(const PlannerConfig& config);
    void update(float dt);
    
    // Planning
    std::vector<ActionSequence> plan_actions(const State& current_state);
    void evaluate_action_sequence(ActionSequence& sequence);
    
    // Learning
    void learn_from_outcome(const ActionSequence& sequence, float outcome);
    
    // Statistics
    const ActionSequence* get_best_action_sequence() const;
    float get_planning_confidence() const;
    json serialize() const;
    void deserialize(const json& data);
};

class ConceptFormation {
public:
    struct Concept {
        uint64_t id;
        std::vector<float> prototype;
        std::vector<float> variance;
        float strength;
        std::unordered_set<uint64_t> related_concepts;
    };
    
    // Core functionality
    void initialize(const ConceptConfig& config);
    void update(float dt);
    
    // Formation
    uint64_t form_concept(const std::vector<float>& pattern);
    void refine_concept(uint64_t concept_id, const std::vector<float>& pattern);
    
    // Retrieval
    std::vector<uint64_t> retrieve_related_concepts(uint64_t concept_id);
    
    // Statistics
    const Concept* get_concept(uint64_t id) const;
    size_t get_concept_count() const;
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Development Classes

```cpp
class DevelopmentStage {
public:
    enum class StageType {
        EMBRYONIC,
        INFANT,
        ADOLESCENT,
        ADULT
    };
    
    // Stage properties
    StageType type;
    float start_time;
    float end_time;
    float duration;
    DevelopmentParameters parameters;
    
    // Effects
    virtual void apply_effects(Brain& brain, float dt);
    virtual DevelopmentParameters get_modified_parameters() const;
    
    // Stage-specific implementations
    static std::unique_ptr<DevelopmentStage> create_embryonic_stage();
    static std::unique_ptr<DevelopmentStage> create_infant_stage();
    static std::unique_ptr<DevelopmentStage> create_adolescent_stage();
    static std::unique_ptr<DevelopmentStage> create_adult_stage();
};

class DevelopmentSystem {
public:
    // Core functionality
    void initialize(const DevelopmentConfig& config);
    void update(float dt);
    
    // Stage management
    void add_development_stage(std::unique_ptr<DevelopmentStage> stage);
    void advance_to_next_stage();
    void go_to_stage(DevelopmentStage::StageType stage);
    
    // Effects
    void apply_development_effects(Brain& brain, float dt);
    
    // Accessors
    DevelopmentStage::StageType get_current_stage() const;
    float get_developmental_age() const;
    float get_plasticity_rate() const;
    
    // Statistics
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Event System

```cpp
class Event {
public:
    enum class Type {
        NEURON_SPIKE,
        SYNAPSE_TRANSMISSION,
        PLASTICITY_CHANGE,
        MEMORY_ENCODING,
        MEMORY_RETRIEVAL,
        NEUROMODULATION_CHANGE,
        PREDICTION_ERROR,
        ATTENTION_SELECTION,
        ACTION_TAKEN,
        ENVIRONMENT_CHANGE,
        PLUGIN_EVENT,
        CUSTOM
    };
    
    // Event properties
    Type type;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::any> data;
    uint64_t source_id;
    uint64_t target_id;
    float importance;
    
    // Construction and initialization
    Event(Type type);
    Event(Type type, const std::unordered_map<std::string, std::any>& data);
};

class EventEmitter {
public:
    // Core functionality
    void emit(const Event& event);
    void emit(Event::Type type, const std::unordered_map<std::string, std::any>& data);
    void emit_to_target(const Event& event, uint64_t target_id);
    
    // Filtering
    void add_filter(std::function<bool(const Event&)> filter);
    void remove_filter(std::function<bool(const Event&)> filter);
    
    // Subscription
    void subscribe(uint64_t subscriber_id, 
                   Event::Type event_type,
                   std::function<void(const Event&)> handler);
    void unsubscribe(uint64_t subscriber_id, Event::Type event_type);
};

class EventSystem {
public:
    // Core functionality
    void initialize(const EventSystemConfig& config);
    void update(float dt);
    
    // Event management
    void emit(const Event& event);
    void emit(Event::Type type, const std::unordered_map<std::string, std::any>& data);
    
    // Subscription
    void subscribe(uint64_t subscriber_id,
                   Event::Type event_type,
                   std::function<void(const Event&)> handler);
    void unsubscribe(uint64_t subscriber_id, Event::Type event_type);
    
    // Statistics
    size_t get_event_count() const;
    json serialize() const;
    void deserialize(const json& data);
};
```

#### Plugin System

```cpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Lifecycle
    virtual void initialize(IPluginContext* context) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void unload() = 0;
    
    // Event handling
    virtual void handle_event(const Event& event) = 0;
    
    // Capabilities
    virtual PluginCapabilities get_capabilities() const = 0;
    
    // Health
    virtual bool health_check() const = 0;
};

class PluginInfo {
public:
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    PluginType type;
    std::unordered_map<std::string, std::any> metadata;
    SecurityLevel security_level;
    std::vector<std::string> dependencies;
};

class PluginManager {
public:
    // Core functionality
    void initialize(const PluginManagerConfig& config);
    
    // Plugin loading
    std::shared_ptr<IPlugin> load_plugin(const std::string& plugin_path);
    std::shared_ptr<IPlugin> load_plugin(const PluginInfo& plugin_info);
    void unload_plugin(const std::string& plugin_name);
    
    // Plugin management
    void enable_plugin(const std::string& plugin_name);
    void disable_plugin(const std::string& plugin_name);
    void reload_plugin(const std::string& plugin_name);
    
    // Query
    std::shared_ptr<IPlugin> get_plugin(const std::string& plugin_name);
    std::vector<PluginInfo> list_plugins() const;
    std::vector<PluginInfo> search_plugins(const std::string& query) const;
    
    // Event handling
    void subscribe_to_plugin_events(const std::string& plugin_name,
                                   Event::Type event_type,
                                   std::function<void(const Event&)> handler);
    
    // Statistics
    json serialize() const;
    void deserialize(const json& data);
};
```

## Part 2: NLM Python Interface API

### Python Interface API

#### Main Interface Class

```python
class ScriptingInterface:
    """Main interface for NLM brain control and monitoring"""
    
    def __init__(self, config_file=None, world_params=None):
        """Initialize NLM Python interface"""
    
    def initialize_system(self, config_file=None, world_params=None):
        """Initialize NLM system"""
    
    def create_brain(self, config_file=None):
        """Create brain instance"""
    
    def create_world(self, width=100, height=100, 
                    vision_width=32, vision_height=32):
        """Create world instance"""
    
    def run_step(self, dt=1.0, auto=True):
        """Run a single simulation step"""
    
    def run_loop(self, duration, dt=1.0, auto=True):
        """Run simulation loop"""
    
    def inject_stimulus(self, stimulus_type, parameters):
        """Inject stimulus into brain"""
    
    def get_state(self):
        """Get current brain state"""
    
    def save_brain_state(self, filepath, include_plasticities=True):
        """Save brain state to file"""
    
    def load_brain_state(self, filepath):
        """Load brain state from file"""
    
    def set_mode(self, mode):
        """Set scripting mode"""
    
    def reset_system(self):
        """Reset system to initial state"""
```

**Key Methods:**

- `initialize_system()`: Initialize the complete NLM system
- `create_brain()`: Create and configure brain instance
- `create_world()`: Create simulation world
- `run_step()`: Execute single simulation step
- `run_loop()`: Run continuous simulation loop
- `inject_stimulus()`: Inject external stimuli
- `save_brain_state()`: Persist brain state
- `load_brain_state()`: Restore brain state

#### Brain Controller Class

```python
class BrainController:
    """Advanced brain controller with real-time monitoring"""
    
    def __init__(self, brain):
        self.brain = brain
    
    def start_monitoring(self, interval=1.0, callbacks=None):
        """Start brain monitoring"""
    
    def stop_monitoring(self):
        """Stop brain monitoring"""
    
    def subscribe_to_neuron(self, neuron_id, callback, event_types=None):
        """Subscribe to neuron events"""
    
    def subscribe_to_region(self, region_id, callback, event_types=None):
        """Subscribe to region events"""
    
    def inject_stimulus_advanced(self, stimulus):
        """Advanced stimulus injection"""
    
    def get_brain_statistics(self):
        """Get detailed brain statistics"""
    
    def get_synaptic_activity(self):
        """Get synaptic activity statistics"""
```

**Key Methods:**

- `start_monitoring()`: Begin continuous brain monitoring
- `subscribe_to_neuron()`: Monitor specific neuron events
- `subscribe_to_region()`: Monitor region-level events
- `inject_stimulus_advanced()`: Advanced stimulus control
- `get_brain_statistics()`: Comprehensive brain metrics

#### Simulation Controller Class

```python
class SimulationController:
    """Comprehensive simulation controller"""
    
    def __init__(self, interface):
        self.interface = interface
    
    def start_simulation(self, duration=None, dt=1.0, auto=True):
        """Start simulation"""
    
    def pause_simulation(self, reason=None):
        """Pause simulation"""
    
    def resume_simulation(self):
        """Resume simulation"""
    
    def step_simulation(self, dt=1.0):
        """Take a single step"""
    
    def reset_simulation(self, keep_config=True):
        """Reset simulation"""
    
    def save_checkpoint(self, name, metadata=None):
        """Save simulation checkpoint"""
    
    def load_checkpoint(self, name):
        """Load simulation checkpoint"""
    
    def start_recording(self):
        """Start simulation recording"""
    
    def stop_recording(self):
        """Stop simulation recording"""
```

**Key Methods:**

- `start_simulation()`: Begin simulation run
- `pause_simulation()`: Temporarily stop simulation
- `step_simulation()`: Execute single time step
- `save_checkpoint()`: Save simulation state
- `load_checkpoint()`: Restore simulation state

#### Event System Classes

```python
class EventFilter:
    """Filter events based on criteria"""
    
    def __init__(self, filter_func):
        self.filter_func = filter_func
    
    def matches(self, event):
        """Check if event matches filter"""
        return self.filter_func(event)
class EventEmitter:
    """Emit events to handlers"""
    
    def emit(self, event_type, data=None):
        """Emit event"""
    
    def emit_to_target(self, event_type, data, target_id):
        """Emit event to specific target"""
class EventBus:
    """Manage event emitters and routing"""
    
    def add_emitter(self, emitter):
        """Add event emitter"""
    
    def remove_emitter(self, emitter):
        """Remove event emitter"""
    
    def route_event(self, event):
        """Route event to appropriate handlers"""
class EventDrivenInterface:
    """Event-driven interface for NLM brain control"""
    
    def __init__(self, interface):
        self.interface = interface
    
    def add_event_handler(self, event_type, handler):
        """Add event handler"""
    
    def remove_event_handler(self, event_type, handler):
        """Remove event handler"""
    
    def process_events(self):
        """Process pending events"""
```

#### Plugin System Classes

```python
class PluginInfo:
    """Plugin metadata and capabilities"""
    
    def __init__(self, name, version, description, author,
                 plugin_type, categories, capabilities):
        self.name = name
        self.version = version
        self.description = description
        self.author = author
        self.plugin_type = plugin_type
        self.categories = categories
        self.capabilities = capabilities

class Plugin:
    """Base class for all NLM plugins"""
    
    def __init__(self, info):
        self.info = info
        self._enabled = False
    
    def initialize(self, context, config=None):
        """Initialize the plugin with system context"""
        self.config = config or {}
        self._initialize_plugin()
    
    def enable(self):
        """Enable the plugin"""
        self._enabled = True
        self._enable_plugin()
    
    def disable(self):
        """Disable the plugin"""
        self._enabled = False
        self._disable_plugin()
    
    def unload(self):
        """Unload the plugin"""
        self._unload_plugin()
        self._enabled = False
    
    def handle_event(self, event):
        """Handle system events"""
        self._handle_event(event)
    
    def get_capabilities(self):
        """Get plugin capabilities"""
        return self.info.capabilities
    
    def health_check(self):
        """Perform health check"""
        return self._health_check()
    
    # Protected methods for subclass implementation
    def _initialize_plugin(self):
        """Subclass-specific initialization"""
        pass
    
    def _enable_plugin(self):
        """Subclass-specific enable logic"""
        pass
    
    def _disable_plugin(self):
        """Subclass-specific disable logic"""
        pass
    
    def _unload_plugin(self):
        """Subclass-specific unload logic"""
        pass
    
    def _handle_event(self, event):
        """Subclass-specific event handling"""
        pass
    
    def _health_check(self):
        """Subclass-specific health check"""
        return True
class PluginManager:
    """Plugin manager for loading and managing plugins"""
    
    def __init__(self, interface, config=None):
        self.interface = interface
        self.config = config or {}
    
    def load_plugin(self, plugin_spec, config=None):
        """Load a plugin from specification"""
        pass
    
    def unload_plugin(self, plugin_name):
        """Unload a plugin"""
        pass
    
    def enable_plugin(self, plugin_name):
        """Enable a plugin"""
        pass
    
    def disable_plugin(self, plugin_name):
        """Disable a plugin"""
        pass
    
    def get_plugin(self, plugin_name):
        """Get loaded plugin by name"""
        pass
    
    def list_plugins(self):
        """List all loaded plugins"""
        pass
```

#### Safety Classes

```python
class AccessLevel:
    """Access level enumeration"""
    
    READ = "read"
    WRITE = "write"
    EXECUTE = "execute"
    ADMIN = "admin"
class SecurityLevel:
    """Security level enumeration"""
    
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"
class SecurityPolicy:
    """Security policy for NLM operations"""
    
    def __init__(self, config):
        self.config = config
        self.access_control = AccessControl(config)
        self.input_validator = InputValidator(config)
        self.resource_monitor = ResourceMonitor(config)
    
    def validate_access(self, user, operation, resource):
        """Validate access request"""
        return self.access_control.validate(user, operation, resource)
    
    def validate_input(self, input_data, context):
        """Validate input data"""
        return self.input_validator.validate(input_data, context)
    
    def check_resources(self, operation, resource_usage):
        """Check resource availability"""
        return self.resource_monitor.check(operation, resource_usage)
class Sandbox:
    """Sandbox environment for safe code execution"""
    
    def __init__(self, config):
        self.config = config
        self.executor = SandboxExecutor(config)
        self.isolation = ProcessIsolation(config)
    
    def execute(self, code, context=None):
        """Execute code in sandbox"""
        pass
    
    def restrict_filesystem(self, allowed_paths):
        """Restrict filesystem access"""
        pass
    
    def restrict_network(self, allowed_network):
        """Restrict network access"""
        pass
class AccessControl:
    """Access control system"""
    
    def __init__(self, config):
        self.config = config
        self.policy_engine = PolicyEngine(config)
        self.authentication = Authentication(config)
    
    def authenticate(self, credentials):
        """Authenticate user"""
        pass
    
    def authorize(self, user, operation, resource):
        """Authorize operation"""
        pass
class InputValidator:
    """Input validation for NLM operations"""
    
    def __init__(self, config):
        self.config = config
        self.schemas = ValidationSchema(config)
    
    def validate(self, input_data, schema_name):
        """Validate input data against schema"""
        pass
    
    def sanitize(self, input_data):
        """Sanitize input data"""
        pass
class ResourceMonitor:
    """Resource monitor for NLM system"""
    
    def __init__(self, config):
        self.config = config
        self.limits = ResourceLimits(config)
        self.monitoring = ResourceMonitoring(config)
    
    def check(self, operation, resource_usage):
        """Check if operation can proceed"""
        pass
    
    def get_usage(self):
        """Get current resource usage"""
        pass
```

#### Visualization Classes

```python
class PlotManager:
    """Manage plotting operations"""
    
    def __init__(self, backend='matplotlib'):
        self.backend = backend
    
    def create_plot(self, data, plot_type='line', **kwargs):
        """Create plot"""
        pass
    
    def update_plot(self, plot, data):
        """Update existing plot"""
        pass
    
    def save_plot(self, plot, filepath):
        """Save plot to file"""
        pass
class InteractivePlot:
    """Interactive plotting with events"""
    
    def __init__(self, figure, axes):
        self.figure = figure
        self.axes = axes
    
    def on_click(self, callback):
        """Register click handler"""
        pass
    
    def on_hover(self, callback):
        """Register hover handler"""
        pass
    
    def on_key(self, callback):
        """Register key handler"""
        pass
class VisualizationDashboard:
    """Comprehensive visualization dashboard"""
    
    def __init__(self, layout='grid'):
        self.layout = layout
        self.widgets = []
    
    def add_widget(self, widget):
        """Add widget to dashboard"""
        pass
    
    def remove_widget(self, widget_id):
        """Remove widget from dashboard"""
        pass
    
    def update_dashboard(self):
        """Update all widgets"""
        pass
class ConsoleDashboard:
    """Console-based dashboard"""
    
    def __init__(self, width=80, height=24):
        self.width = width
        self.height = height
    
    def update(self, data):
        """Update dashboard display"""
        pass
    
    def clear(self):
        """Clear dashboard"""
        pass
class WebDashboard:
    """Web-based dashboard"""
    
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
    
    def start(self):
        """Start web dashboard server"""
        pass
    
    def stop(self):
        """Stop web dashboard server"""
        pass
    
    def update_data(self, data):
        """Update dashboard data"""
        pass
```

#### Integration Classes

```python
class NumPyIntegration:
    """NumPy integration"""
    
    def __init__(self, interface):
        self.interface = interface
    
    def export_brain_state(self):
        """Export brain state as NumPy arrays"""
        pass
    
    def import_brain_state(self, state_data):
        """Import brain state from NumPy arrays"""
        pass
    
    def export_neural_activity(self):
        """Export neural activity as NumPy arrays"""
        pass
class PandasIntegration:
    """Pandas integration"""
    
    def __init__(self, interface):
        self.interface = interface
    
    def analyze_brain_state(self):
        """Analyze brain state with Pandas"""
        pass
    
    def track_simulation_history(self):
        """Track simulation history"""
        pass
class MatplotlibIntegration:
    """Matplotlib integration"""
    
    def __init__(self, interface):
        self.interface = interface
    
    def create_neural_plots(self):
        """Create neural network visualizations"""
        pass
    
    def create_activity_plots(self):
        """Create neural activity visualizations"""
        pass
    
    def create_memory_plots(self):
        """Create memory system visualizations"""
        pass
```

## API Reference Summary

### Core C++ API Classes

| Class | Header | Description |
|-------|--------|-------------|
| Brain | `nlm/brain.h` | Main neural system controller |
| World | `nlm/world.h` | Simulation environment |
| Neuron | `nlm/neuron.h` | Base neuron class |
| Synapse | `nlm/synapse.h` | Neural connections |
| WorkingMemory | `nlm/memory/working_memory.h` | Working memory system |
| EpisodicMemory | `nlm/memory/episodic_memory.h` | Episodic memory system |
| PlasticityRule | `nlm/plasticity/plasticity_rule.h` | Learning rules |
| Neuromodulation | `nlm/neuromodulation.h` | Neuromodulatory systems |
| PredictionError | `nlm/prediction/error.h` | Prediction error signals |
| Attention | `nlm/cognition/attention.h` | Attention system |
| NeuralPlanner | `nlm/cognition/planner.h` | Action planning |
| ConceptFormation | `nlm/cognition/concepts.h` | Concept learning |

### Python Interface Classes

| Class | Module | Description |
|-------|--------|-------------|
| ScriptingInterface | `pynlm/interface.py` | Main interface for brain control |
| BrainController | `pynlm/brain_control.py` | Advanced brain monitoring |
| SimulationController | `pynlm/simulation.py` | Simulation control |
| EventSystem | `pynlm/event_system.py` | Event-driven programming |
| Plugin | `pynlm/plugin_system.py` | Base plugin class |
| PluginManager | `pynlm/plugin_system.py` | Plugin management |
| SecurityPolicy | `pynlm/safety.py` | Security controls |
| Sandbox | `pynlm/safety.py` | Safe execution environment |

### Key APIs

#### Construction and Initialization

**C++:**
```cpp
Brain brain(config);
brain.initialize(config);
```

**Python:**
```python
brain = interface.create_brain()
interface.initialize_system()
```

#### Core Operations

**C++:**
```cpp
brain.inject_stimulus(stimulus);
brain.update_neural_dynamics(dt);
brain.process_plasticity(dt);
```

**Python:**
```python
interface.inject_stimulus("current", {"neuron_id": 0, "current": 1.0})
interface.run_step(dt=0.1)
```

#### State Management

**C++:**
```cpp
json state = brain.serialize_state();
brain.deserialize_state(state);
```

**Python:**
```python
interface.save_brain_state("state.json")
interface.load_brain_state("state.json")
```

#### Plugin Management

**C++:**
```cpp
auto plugin = plugin_manager.load_plugin("plugin_name");
plugin_manager.enable_plugin("plugin_name");
```

**Python:**
```python
plugin = plugin_manager.load_plugin('custom_plugin')
plugin_manager.enable_plugin('custom_plugin')
```

## API Usage Examples

### C++ Example

```cpp
#include "nlm/brain.h"
#include "nlm/world.h"
#include <iostream>

int main() {
    // Create brain and world
    Brain brain;
    World world(100, 100);
    
    // Initialize with default configuration
    brain.initialize();
    world.initialize();
    
    // Main simulation loop
    while (!should_exit) {
        // Update world
        world.update(0.1f);
        
        // Inject stimulus
        NeuronStimulus stimulus;
        stimulus.neuron_id = 0;
        stimulus.current = 1.0f;
        brain.inject_stimulus(stimulus);
        
        // Update brain
        brain.update_neural_dynamics(0.1f);
        brain.process_plasticity(0.1f);
        brain.update_working_memory();
        
        // Check for events
        Event event = event_system.poll_event();
        if (event.type != Event::Type::NONE) {
            // Handle event
        }
    }
    
    return 0;
}
```

### Python Example

```python
import pynlm
import time

# Initialize NLM interface
interface = pynlm.initialize_nlm()

# Create brain and world
brain = interface.create_brain()
world = interface.create_world()

# Run simulation
interface.run_loop(duration=10.0, dt=0.1)

# Monitor brain state
stats = brain.get_statistics()
print(f"Activity: {stats.activity}")
print(f"Firing rate: {stats.firing_rate}")

# Save and restore state
interface.save_brain_state("checkpoint.json")
# ... do something ...
interface.load_brain_state("checkpoint.json")
```

### Plugin Example

```python
import pynlm

# Create custom plugin
class CustomPlugin(pynlm.Plugin):
    def _initialize_plugin(self):
        self.register_event_handler(
            pynlm.EventType.STATE_CHANGED,
            self._on_state_change
        )
    
    def _enable_plugin(self):
        print(f"Plugin enabled: {self.info.name}")
    
    def _on_state_change(self, event):
        print(f"State changed: {event.event_type}")

# Create and load plugin
plugin_info = pynlm.PluginInfo(
    name='custom_plugin',
    version='1.0.0',
    description='Custom plugin for NLM',
    author='Your Name',
    plugin_type=pynlm.PluginType.UTILITY
)

plugin = CustomPlugin(plugin_info)
loaded_plugin = plugin_manager.load_plugin(plugin)
plugin_manager.enable_plugin('custom_plugin')
```

## API Error Handling

### C++ Error Types

```cpp
enum class NLMError {
    SUCCESS = 0,
    INVALID_CONFIG,
    BRAIN_NOT_INITIALIZED,
    WORLD_NOT_CREATED,
    NEURON_NOT_FOUND,
    SYNAPSE_NOT_FOUND,
    PLUGIN_NOT_LOADED,
    SECURITY_VIOLATION,
    RESOURCE_EXHAUSTED,
    INVALID_INPUT,
    SERIALIZATION_ERROR,
    UNKNOWN_ERROR
};

NLMError initialize_brain(const BrainConfig& config);
NLMError save_brain_state(const std::string& filepath);
NLMError load_brain_state(const std::string& filepath);
```

### Python Error Handling

```python
import pynlm

# Error handling
interface = pynlm.initialize_nlm()

try:
    brain = interface.create_brain()
    interface.run_loop(duration=10.0)
except pynlm.NLMPythonError as e:
    print(f"NLM Python error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Check system health
if not plugin_manager.health_check():
    print("Plugin system health check failed")
```

## API Migration Guide

### C++ API Changes (Version 6.0+)

1. **Brain Initialization**: Use `initialize(const BrainConfig& config)` instead of constructor parameters
2. **Event System**: Use `EventSystem` class instead of global event functions
3. **Memory Access**: Use `get_working_memory()`, `get_episodic_memory()` instead of direct access
4. **Plugin System**: Use `PluginManager` instead of global plugin functions

### Python API Changes (Version 2.0+)

1. **Interface Initialization**: Use `initialize_system()` instead of constructor parameters
2. **Brain Creation**: Use `create_brain()` instead of direct constructor
3. **Plugin Management**: Use `PluginManager` instead of global plugin functions
4. **Event Handling**: Use `EventDrivenInterface` instead of global event handlers

## API Support and Limitations

### Supported Platforms

- **Operating Systems**: Linux, macOS, Windows
- **Compilers**: GCC 7+, Clang 6+, MSVC 2019+
- **Python**: 3.8, 3.9, 3.10, 3.11

### Performance Considerations

- **Memory Usage**: Monitor plugin memory usage with `ResourceMonitor`
- **CPU Usage**: Profile neural operations with `PerformanceMonitor`
- **I/O Operations**: Use async I/O for file operations
- **Event Processing**: Batch events for better performance

### Known Limitations

1. **Plugin Loading**: Plugin loading is blocking and may cause system delays
2. **Event Processing**: Event processing has some overhead
3. **Memory Management**: Manual memory management required for some components
4. **State Serialization**: Large brain states may require significant I/O time

## API Documentation Generation

API documentation can be generated using:

```bash
# Generate C++ documentation
 doxygen Doxyfile.cpp

# Generate Python documentation
 sphinx-build docs/python /path/to/output

# Generate combined API reference
python generate_api_docs.py --cpp src --python python
```

## API Testing

The NLM API includes comprehensive tests:

### Unit Tests

- `tests/test_brain.cpp` - Brain functionality tests
- `tests/test_neuron.cpp` - Neuron dynamics tests
- `tests/test_plasticity.cpp` - Plasticity rule tests
- `tests/test_memory.cpp` - Memory system tests
- `tests/test_prediction.cpp` - Prediction system tests

### Python Tests

- `tests/test_interface.py` - Interface tests
- `tests/test_brain_controller.py` - Brain controller tests
- `tests/test_simulation.py` - Simulation tests
- `tests/test_plugin_system.py` - Plugin system tests
- `tests/test_event_system.py` - Event system tests

### Integration Tests

- `tests/test_integration.py` - End-to-end integration tests
- `tests/test_performance.py` - Performance tests
- `tests/test_safety.py` - Security tests

Run tests with:

```bash
# C++ tests
cd build
ctest --output-on-failure

# Python tests
python -m pytest tests/ -v
```

## API Reference Summary

The NLM API provides comprehensive access to all neural system components:

### Core APIs

1. **Brain Control**: Create, initialize, and control neural systems
2. **World Simulation**: Create and interact with simulation environments
3. **Memory Systems**: Access and manage working and episodic memory
4. **Plasticity**: Configure and control learning rules
5. **Neuromodulation**: Control neuromodulatory effects
6. **Prediction**: Access prediction systems and error signals
7. **Cognition**: Access attention, planning, and concept formation
8. **Development**: Manage developmental stages and effects

### Interface APIs

1. **Main Interface**: Complete brain control and monitoring
2. **Brain Controller**: Advanced brain monitoring and control
3. **Simulation Controller**: Comprehensive simulation control
4. **Event System**: Event-driven programming support
5. **Plugin System**: Plugin management and loading
6. **Safety System**: Security and access control
7. **Visualization**: Real-time monitoring and plotting
8. **Integration**: External system integration

### Key Features

- **Comprehensive**: Access to all neural system components
- **Flexible**: Multiple access patterns (imperative and event-driven)
- **Secure**: Built-in security and sandboxing
- **Extensible**: Plugin system for custom functionality
- **Monitorable**: Real-time monitoring and debugging
- **Testable**: Comprehensive test coverage

This API reference provides complete documentation for developing custom NLM applications, plugins, and extensions.