#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function to convert any to string
template<typename T>
std::string to_string_with_precision(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Improved ID classes with Pythonic APIs
class NeuronIdPy {
    NeuronId original;
public:
    NeuronIdPy() : original() {}
    NeuronIdPy(uint64_t value) : original(value) {}
    
    uint64_t get_value() const { return original.value; }
    void set_value(uint64_t value) { original.value = value; }
    size_t get_index() const { return original.index(); }
    bool is_valid() const { return original.value != INVALID_NEURON_ID.value; }
    
    std::string str() const { return "<NeuronId: " + std::to_string(original.value) + ">"; }
    std::string __repr__() const { return str(); }
    bool __eq__(const NeuronIdPy& other) const { return original == other.original; }
    bool __ne__(const NeuronIdPy& other) const { return original != other.original; }
    size_t __hash__() const { return std::hash<uint64_t>{}(original.value); }
};

class SynapseIdPy {
    SynapseId original;
public:
    SynapseIdPy() : original() {}
    SynapseIdPy(uint64_t value) : original(value) {}
    
    uint64_t get_value() const { return original.value; }
    void set_value(uint64_t value) { original.value = value; }
    size_t get_index() const { return original.index(); }
    bool is_valid() const { return original.value != INVALID_SYNAPSE_ID.value; }
    
    std::string str() const { return "<SynapseId: " + std::to_string(original.value) + ">"; }
    std::string __repr__() const { return str(); }
    bool __eq__(const SynapseIdPy& other) const { return original == other.original; }
    bool __ne__(const SynapseIdPy& other) const { return original != other.original; }
    size_t __hash__() const { return std::hash<uint64_t>{}(original.value); }
};

class RegionIdPy {
    RegionId original;
public:
    RegionIdPy() : original() {}
    RegionIdPy(uint64_t value) : original(value) {}
    
    uint64_t get_value() const { return original.value; }
    void set_value(uint64_t value) { original.value = value; }
    size_t get_index() const { return original.index(); }
    bool is_valid() const { return original.value != INVALID_REGION_ID.value; }
    
    std::string str() const { return "<RegionId: " + std::to_string(original.value) + ">"; }
    std::string __repr__() const { return str(); }
    bool __eq__(const RegionIdPy& other) const { return original == other.original; }
    bool __ne__(const RegionIdPy& other) const { return original != other.original; }
    size_t __hash__() const { return std::hash<uint64_t>{}(original.value); }
};

class PopulationIdPy {
    PopulationId original;
public:
    PopulationIdPy() : original() {}
    PopulationIdPy(uint64_t value) : original(value) {}
    
    uint64_t get_value() const { return original.value; }
    void set_value(uint64_t value) { original.value = value; }
    size_t get_index() const { return original.index(); }
    bool is_valid() const { return original.value != INVALID_POPULATION_ID.value; }
    
    std::string str() const { return "<PopulationId: " + std::to_string(original.value) + ">"; }
    std::string __repr__() const { return str(); }
    bool __eq__(const PopulationIdPy& other) const { return original == other.original; }
    bool __ne__(const PopulationIdPy& other) const { return original != other.original; }
    size_t __hash__() const { return std::hash<uint64_t>{}(original.value); }
};

// Context manager for Brain simulation
class BrainSimulator {
private:
    std::shared_ptr<Brain> brain;
    bool should_auto_reset;
    
public:
    BrainSimulator(std::shared_ptr<Brain> b, bool auto_reset = true) 
        : brain(b), should_auto_reset(auto_reset) {}
    
    ~BrainSimulator() {
        if (should_auto_reset && brain) {
            brain->reset();
        }
    }
    
    // Context manager interface
    BrainSimulator& enter() { return *this; }
    void exit() {
        if (should_auto_reset) {
            brain->reset();
        }
    }
    
    // Python API
    std::shared_ptr<Brain> get_brain() { return brain; }
    float step(size_t step, double time = 0.0) {
        brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
        return brain->getAverageFiringRate();
    }
    
    // Run simulation for multiple steps
    std::vector<float> run_steps(size_t num_steps, double time_step = 0.001) {
        std::vector<float> firing_rates;
        firing_rates.reserve(num_steps);
        
        for (size_t step = 0; step < num_steps; ++step) {
            float rate = this->step(static_cast<SimulationStep>(step), time_step * step);
            firing_rates.push_back(rate);
        }
        
        return firing_rates;
    }
    
    // Run until condition is met
    size_t run_until(std::function<bool(const Brain&)> condition, size_t max_steps = 10000, double time_step = 0.001) {
        for (size_t step = 0; step < max_steps; ++step) {
            brain->step(static_cast<SimulationStep>(step), time_step * step);
            if (condition(*brain)) {
                return step + 1;
            }
        }
        return max_steps;
    }
    
    // Get current brain state
    std::string get_stats() const {
        std::stringstream ss;
        ss << "Brain Statistics:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  Spike count: " << brain->getTotalSpikeCount() << std::endl;
        return ss.str();
    }
};

// Enhanced Config class with Pythonic API
class ConfigPy {
private:
    std::shared_ptr<Config> config;
public:
    ConfigPy() : config(std::make_shared<Config>()) {}
    explicit ConfigPy(const std::string& filepath) {
        config = std::make_shared<Config>();
        config->loadFromFile(filepath);
    }
    
    // Set configuration values with type checking
    ConfigPy& set_int(const std::string& key, int64_t value) {
        config->set(key, value, ConfigSource::Default);
        return *this;
    }
    
    ConfigPy& set_float(const std::string& key, float value) {
        config->set(key, value, ConfigSource::Default);
        return *this;
    }
    
    ConfigPy& set_double(const std::string& key, double value) {
        config->set(key, value, ConfigSource::Default);
        return *this;
    }
    
    ConfigPy& set_string(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::Default);
        return *this;
    }
    
    // Get configuration values
    int64_t get_int(const std::string& key, int64_t default_val = 0) const {
        return config->getOr<int64_t>(key, default_val);
    }
    
    float get_float(const std::string& key, float default_val = 0.0f) const {
        return config->getOr<float>(key, default_val);
    }
    
    double get_double(const std::string& key, double default_val = 0.0) const {
        return config->getOr<double>(key, default_val);
    }
    
    std::string get_string(const std::string& key, const std::string& default_val = "") const {
        return config->getOr<std::string>(key, default_val);
    }
    
    // Check if key exists
    bool has(const std::string& key) const { return config->has(key); }
    
    // Get all keys
    std::vector<std::string> get_keys() const { return config->getKeys(); }
    
    // Save/load
    bool save(const std::string& filepath) const { return config->saveToFile(filepath); }
    bool load(const std::string& filepath) {
        bool result = config->loadFromFile(filepath);
        if (!result) {
            throw std::runtime_error("Failed to load config from file: " + filepath);
        }
        return result;
    }
    
    // Summary and string representation
    std::string summary() const { return config->summary(); }
    std::string __repr__() const { return "<Config: " + summary() + ">"; }
    
    // Access to underlying config
    std::shared_ptr<Config> get_config() { return config; }
};

// Enhanced Action class
class ActionPy {
private:
    std::unique_ptr<Action> action;
public:
    ActionPy() : action(std::make_unique<Action>()) {}
    ActionPy(ActionType type) : action(std::make_unique<Action>(type)) {}
    ActionPy(ActionType type, const std::vector<float>& params) 
        : action(std::make_unique<Action>(type, params)) {}
    
    ActionType get_type() const { return action->getType(); }
    void set_type(ActionType type) { action->setType(type); }
    
    const std::vector<float>& get_parameters() const { return action->getParameters(); }
    void set_parameters(const std::vector<float>& params) { action->setParameters(params); }
    
    std::string get_name() const { return action->getName(); }
    
    // Clone action
    std::unique_ptr<ActionPy> clone() const {
        auto cloned = std::make_unique<ActionPy>();
        cloned->action = action->clone();
        return cloned;
    }
    
    // String representation
    std::string str() const { return "<Action: " + get_name() + " params=" + std::to_string(get_parameters().size()) + ">"; }
    std::string __repr__() const { return str(); }
    
    // Comparison
    bool __eq__(const ActionPy& other) const {
        return action->getType() == other.get_type() && 
               action->getParameters() == other.get_parameters();
    }
    
    // Access to underlying action
    Action* get_action() { return action.get(); }
    const Action* get_action() const { return action.get(); }
};

// Enhanced World class
class WorldPy {
private:
    std::shared_ptr<SimpleWorld> world;
public:
    WorldPy() : world(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(size_t width, size_t height, size_t vision_width = 32, size_t vision_height = 32) {
        world->configure(width, height, vision_width, vision_height);
    }
    
    // Reset world
    void reset() { world->reset(); }
    
    // Set agent start position
    void set_agent_start(float x, float y) { world->setAgentStart(x, y); }
    
    // Apply action
    float apply_action(const ActionPy& action, double current_time = 0.0) {
        return world->applyMotorCommand(action.get_action()->getType(), current_time);
    }
    
    // Get percept
    SensoryPercept get_percept() const {
        return world->getSensoryPercept(nullptr); // Note: needs brain context
    }
    
    // Get agent body
    AgentBody get_agent_body() const {
        return world->getAgentBody();
    }
    
    // Add/remove objects
    void add_object(const WorldObject& obj) { world->addObject(obj); }
    bool remove_object(float x, float y) { return world->removeObject(x, y); }
    bool is_valid_position(float x, float y) const { return world->isValidPosition(x, y); }
    
    // Get world properties
    size_t get_width() const { return world->getWidth(); }
    size_t get_height() const { return world->getHeight(); }
    float get_max_energy() const { return world->getMaxEnergy(); }
    void set_max_energy(float e) { world->setMaxEnergy(e); }
    float get_energy_decay_rate() const { return world->getEnergyDecayRate(); }
    void set_energy_decay_rate(float r) { world->setEnergyDecayRate(r); }
    double get_simulation_time() const { return world->getSimulationTime(); }
    void set_random_seed(uint64_t seed) { world->setRandomSeed(seed); }
    uint64_t get_random_seed() const { return world->getRandomSeed(); }
    
    // Update world
    void update(double timestep = 0.001) { world->update(timestep); }
    
    // Get world state
    std::string get_stats() const {
        std::stringstream ss;
        ss << "World Statistics:" << std::endl;
        ss << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
        ss << "  Agent position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")" << std::endl;
        ss << "  Simulation time: " << world->getSimulationTime() << std::endl;
        ss << "  Max energy: " << world->getMaxEnergy() << std::endl;
        return ss.str();
    }
    
    // Access to underlying world
    std::shared_ptr<SimpleWorld> get_world() { return world; }
};

// Phase 6 integration test results Pythonic wrapper
class Phase6IntegrationResultPy {
private:
    Phase6IntegrationResult original;
public:
    Phase6IntegrationResultPy() = default;
    explicit Phase6IntegrationResultPy(const Phase6IntegrationResult& result) : original(result) {}
    
    // Access to results
    float get_total_reward() const { return original.totalReward; }
    float get_avg_firing_rate() const { return original.avgFiringRate; }
    float get_memory_episodes_stored() const { return original.memoryEpisodesStored; }
    float get_novelty_level() const { return original.noveltyLevel; }
    float get_curiosity_level() const { return original.curiosityLevel; }
    float get_dopamine_level() const { return original.dopamineLevel; }
    double get_total_wall_clock_time() const { return original.totalWallClockTime; }
    
    // Integration status
    bool is_working_memory_integrated() const { return original.memoryWorkingMemoryIntegrated; }
    bool is_episodic_memory_integrated() const { return original.memoryEpisodicMemoryIntegrated; }
    bool is_neuromodulation_integrated() const { return original.neuromodulationIntegrated; }
    bool is_prediction_integrated() const { return original.predictionIntegrated; }
    bool is_development_integrated() const { return original.developmentIntegrated; }
    bool is_checkpointing_works() const { return original.checkpointingWorks; }
    bool is_replay_works() const { return original.replayWorks; }
    
    // Get all results as dictionary (for easy Python use)
    py::dict to_dict() const {
        py::dict result;
        result["total_reward"] = original.totalReward;
        result["avg_firing_rate"] = original.avgFiringRate;
        result["memory_episodes_stored"] = original.memoryEpisodesStored;
        result["novelty_level"] = original.noveltyLevel;
        result["curiosity_level"] = original.curiosityLevel;
        result["dopamine_level"] = original.dopamineLevel;
        result["total_wall_clock_time"] = original.totalWallClockTime;
        result["working_memory_integrated"] = original.memoryWorkingMemoryIntegrated;
        result["episodic_memory_integrated"] = original.memoryEpisodicMemoryIntegrated;
        result["neuromodulation_integrated"] = original.neuromodulationIntegrated;
        result["prediction_integrated"] = original.predictionIntegrated;
        result["development_integrated"] = original.developmentIntegrated;
        result["checkpointing_works"] = original.checkpointingWorks;
        result["replay_works"] = original.replayWorks;
        return result;
    }
    
    std::string __repr__() const {
        std::stringstream ss;
        ss << "Phase6IntegrationResultPy(";
        ss << "reward=" << original.totalReward << ", ";
        ss << "firing_rate=" << original.avgFiringRate << ", ";
        ss << "episodes=" << original.memoryEpisodesStored << ", ";
        ss << "dopamine=" << original.dopamineLevel << ")";
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        -----------------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Pythonic APIs with property access and method chaining
        - Context managers for resource management
        - High-level convenience functions
        - Better error handling and type checking
        - Enhanced debugging and inspection tools
        
        Key features:
        - Improved ID classes with validation and Pythonic properties
        - Brain simulation context manager for safe simulation control
        - Enhanced configuration system with type-safe APIs
        - Action and world classes with intuitive interfaces
        - Integration test results in Python-friendly formats
        - Comprehensive error handling with meaningful exceptions
        
        Usage examples:
        ```python
        import pynlm as nlm
        
        # Create and configure brain
        config = nlm.Config().set_int("neuron_count", 1000).set_float("connection_probability", 0.1)
        brain = nlm.create_brain(config)
        
        # Use context manager for safe simulation
        with nlm.BrainSimulator(brain) as simulator:
            # Run simulation
            for step in range(1000):
                simulator.step(step)
            
            # Get statistics
            print(simulator.get_stats())
        
        # Create enhanced world
        world = nlm.World()
        world.configure(100, 100)
        
        # Create and use action
        action = nlm.Action(nlm.ActionType.MoveForward)
        world.set_agent_start(10, 10)
        world.apply_action(action)
        ```
        
        See also: Phase 6 integration test utilities for comprehensive testing
        "pbdoc";

    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::invalid_argument>(m, "NLMInvalidArgument");

    // Register exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        }
    });

    // Improved ID classes
    py::class_<NeuronIdPy>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &NeuronIdPy::get_value, &NeuronIdPy::set_value)
        .def("index", &NeuronIdPy::get_index)
        .def("is_valid", &NeuronIdPy::is_valid)
        .def("str", &NeuronIdPy::str)
        .def("__repr__", &NeuronIdPy::__repr__)
        .def("__eq__", &NeuronIdPy::__eq__)
        .def("__ne__", &NeuronIdPy::__ne__)
        .def("__hash__", &NeuronIdPy::__hash__);

    py::class_<SynapseIdPy>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &SynapseIdPy::get_value, &SynapseIdPy::set_value)
        .def("index", &SynapseIdPy::get_index)
        .def("is_valid", &SynapseIdPy::is_valid)
        .def("str", &SynapseIdPy::str)
        .def("__repr__", &SynapseIdPy::__repr__)
        .def("__eq__", &SynapseIdPy::__eq__)
        .def("__ne__", &SynapseIdPy::__ne__)
        .def("__hash__", &SynapseIdPy::__hash__);

    py::class_<RegionIdPy>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &RegionIdPy::get_value, &RegionIdPy::set_value)
        .def("index", &RegionIdPy::get_index)
        .def("is_valid", &RegionIdPy::is_valid)
        .def("str", &RegionIdPy::str)
        .def("__repr__", &RegionIdPy::__repr__)
        .def("__eq__", &RegionIdPy::__eq__)
        .def("__ne__", &RegionIdPy::__ne__)
        .def("__hash__", &RegionIdPy::__hash__);

    py::class_<PopulationIdPy>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &PopulationIdPy::get_value, &PopulationIdPy::set_value)
        .def("index", &PopulationIdPy::get_index)
        .def("is_valid", &PopulationIdPy::is_valid)
        .def("str", &PopulationIdPy::str)
        .def("__repr__", &PopulationIdPy::__repr__)
        .def("__eq__", &PopulationIdPy::__eq__)
        .def("__ne__", &PopulationIdPy::__ne__)
        .def("__hash__", &PopulationIdPy::__hash__);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class
    py::class_<ConfigPy>(m, "Config", R"pbdoc(Enhanced configuration class with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("filepath"))
        .def("set_int", &ConfigPy::set_int, py::arg("key"), py::arg("value"),
             "Set an integer configuration value (method chaining)")
        .def("set_float", &ConfigPy::set_float, py::arg("key"), py::arg("value"),
             "Set a float configuration value (method chaining)")
        .def("set_double", &ConfigPy::set_double, py::arg("key"), py::arg("value"),
             "Set a double configuration value (method chaining)")
        .def("set_string", &ConfigPy::set_string, py::arg("key"), py::arg("value"),
             "Set a string configuration value (method chaining)")
        .def("get_int", &ConfigPy::get_int, py::arg("key"), py::arg("default_val") = 0,
             "Get an integer configuration value")
        .def("get_float", &ConfigPy::get_float, py::arg("key"), py::arg("default_val") = 0.0f,
             "Get a float configuration value")
        .def("get_double", &ConfigPy::get_double, py::arg("key"), py::arg("default_val") = 0.0,
             "Get a double configuration value")
        .def("get_string", &ConfigPy::get_string, py::arg("key"), py::arg("default_val") = "",
             "Get a string configuration value")
        .def("has", &ConfigPy::has, py::arg("key"), "Check if key exists")
        .def("get_keys", &ConfigPy::get_keys, "Get all configuration keys")
        .def("save", &ConfigPy::save, py::arg("filepath"), "Save to file")
        .def("load", &ConfigPy::load, py::arg("filepath"), "Load from file")
        .def("summary", &ConfigPy::summary, "Get configuration summary")
        .def("__repr__", &ConfigPy::__repr__);

    // Enhanced Action class
    py::class_<ActionPy>(m, "Action", R"pbdoc(Enhanced action class with Pythonic API)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_property("type", &ActionPy::get_type, &ActionPy::set_type)
        .def_property("parameters", &ActionPy::get_parameters, &ActionPy::set_parameters)
        .def("get_name", &ActionPy::get_name)
        .def("clone", &ActionPy::clone)
        .def("str", &ActionPy::str)
        .def("__repr__", &ActionPy::__repr__)
        .def("__eq__", &ActionPy::__eq__);

    // Enhanced World class
    py::class_<WorldPy>(m, "World", R"pbdoc(Enhanced world simulation class with Pythonic API)pbdoc")
        .def(py::init<>())
        .def("configure", &WorldPy::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width") = 32, py::arg("vision_height") = 32,
             "Configure world dimensions")
        .def("reset", &WorldPy::reset, "Reset world to initial state")
        .def("set_agent_start", &WorldPy::set_agent_start, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("apply_action", &WorldPy::apply_action, py::arg("action"), py::arg("current_time") = 0.0,
             "Apply action to world")
        .def("get_percept", &WorldPy::get_percept, "Get current percept")
        .def("get_agent_body", &WorldPy::get_agent_body, "Get agent body state")
        .def("add_object", &WorldPy::add_object, py::arg("obj"), "Add world object")
        .def("remove_object", &WorldPy::remove_object, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &WorldPy::is_valid_position, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &WorldPy::get_width, "Get world width")
        .def("get_height", &WorldPy::get_height, "Get world height")
        .def("get_max_energy", &WorldPy::get_max_energy, "Get maximum energy")
        .def("set_max_energy", &WorldPy::set_max_energy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &WorldPy::get_energy_decay_rate, "Get energy decay rate")
        .def("set_energy_decay_rate", &WorldPy::set_energy_decay_rate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &WorldPy::get_simulation_time, "Get simulation time")
        .def("set_random_seed", &WorldPy::set_random_seed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &WorldPy::get_random_seed, "Get random seed")
        .def("update", &WorldPy::update, py::arg("timestep") = 0.001, "Update world")
        .def("get_stats", &WorldPy::get_stats, "Get world statistics");

    // Enhanced Brain simulator with context manager
    py::class_<BrainSimulator>(m, "BrainSimulator", R"pbdoc(Context manager for safe brain simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, bool>(), py::arg("brain"), py::arg("auto_reset") = true,
             "Create brain simulator with optional auto-reset")
        .def("enter", &BrainSimulator::enter, "Enter context manager")
        .def("exit", &BrainSimulator::exit, "Exit context manager")
        .def("get_brain", &BrainSimulator::get_brain, "Get underlying brain")
        .def("step", &BrainSimulator::step, py::arg("step"), py::arg("time") = 0.0,
             "Perform a simulation step, returns average firing rate")
        .def("run_steps", &BrainSimulator::run_steps, py::arg("num_steps"), py::arg("time_step") = 0.001,
             "Run multiple simulation steps")
        .def("run_until", &BrainSimulator::run_until, py::arg("condition"), py::arg("max_steps") = 10000,
             py::arg("time_step") = 0.001, "Run until condition is met")
        .def("get_stats", &BrainSimulator::get_stats, "Get brain statistics");

    // Phase 6 integration results
    py::class_<Phase6IntegrationResultPy>(m, "Phase6IntegrationResult", R"pbdoc(Phase 6 integration test results)pbdoc")
        .def(py::init<>())
        .def("get_total_reward", &Phase6IntegrationResultPy::get_total_reward)
        .def("get_avg_firing_rate", &Phase6IntegrationResultPy::get_avg_firing_rate)
        .def("get_memory_episodes_stored", &Phase6IntegrationResultPy::get_memory_episodes_stored)
        .def("get_novelty_level", &Phase6IntegrationResultPy::get_novelty_level)
        .def("get_curiosity_level", &Phase6IntegrationResultPy::get_curiosity_level)
        .def("get_dopamine_level", &Phase6IntegrationResultPy::get_dopamine_level)
        .def("get_total_wall_clock_time", &Phase6IntegrationResultPy::get_total_wall_clock_time)
        .def("is_working_memory_integrated", &Phase6IntegrationResultPy::is_working_memory_integrated)
        .def("is_episodic_memory_integrated", &Phase6IntegrationResultPy::is_episodic_memory_integrated)
        .def("is_neuromodulation_integrated", &Phase6IntegrationResultPy::is_neuromodulation_integrated)
        .def("is_prediction_integrated", &Phase6IntegrationResultPy::is_prediction_integrated)
        .def("is_development_integrated", &Phase6IntegrationResultPy::is_development_integrated)
        .def("is_checkpointing_works", &Phase6IntegrationResultPy::is_checkpointing_works)
        .def("is_replay_works", &Phase6IntegrationResultPy::is_replay_works)
        .def("to_dict", &Phase6IntegrationResultPy::to_dict, "Convert results to Python dictionary")
        .def("__repr__", &Phase6IntegrationResultPy::__repr__);

    // Original classes (for backward compatibility)
    py::class_<Config>(m, "Config", R"pbdoc(Original config class)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Original action class)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience functions for Python users
    m.def("create_brain_simulator", [](std::shared_ptr<Brain> brain, bool auto_reset = true) {
        return BrainSimulator(brain, auto_reset);
    }, py::arg("brain"), py::arg("auto_reset") = true,
       "Create a brain simulator with context manager support");

    m.def("run_phase6_integration", [](const Phase6Config& config) -> Phase6IntegrationResultPy {
        Phase6IntegratedExperiment experiment;
        return Phase6IntegrationResultPy(experiment.run(config));
    }, py::arg("config"), "Run Phase 6 integration experiment");

    m.def("verify_phase6_integration", []() -> bool {
        Phase6IntegratedExperiment experiment;
        return experiment.verifyIntegration();
    }, "Verify Phase 6 integration");

    // Helper functions for Python users
    m.def("get_version", []() { return "NLM Enhanced Python Bindings v2.0"; });
    
    m.def("create_test_config", [](size_t neuron_count = 100, size_t region_count = 1, 
                                 float connection_prob = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(region_count), ConfigSource::Default);
        config->set("connection_probability", connection_prob, ConfigSource::Default);
        return config;
    }, py::arg("neuron_count") = 100, py::arg("region_count") = 1, 
       py::arg("connection_prob") = 0.1, "Create a test configuration");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

} // namespace nlm
