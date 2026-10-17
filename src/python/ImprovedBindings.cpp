#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings with better API design

// Type conversion helpers
inline NeuronId toNeuronId(py::object obj) {
    if (py::isinstance<NeuronId>(obj)) {
        return obj.cast<NeuronId>();
    } else if (py::isinstance<int>(obj)) {
        return NeuronId(obj.cast<uint64_t>());
    } else {
        throw py::type_error("NeuronId must be NeuronId or int");
    }
}

inline SynapseId toSynapseId(py::object obj) {
    if (py::isinstance<SynapseId>(obj)) {
        return obj.cast<SynapseId>();
    } else if (py::isinstance<int>(obj)) {
        return SynapseId(obj.cast<uint64_t>());
    } else {
        throw py::type_error("SynapseId must be SynapseId or int");
    }
}

inline RegionId toRegionId(py::object obj) {
    if (py::isinstance<RegionId>(obj)) {
        return obj.cast<RegionId>();
    } else if (py::isinstance<int>(obj)) {
        return RegionId(obj.cast<uint64_t>());
    } else {
        throw py::type_error("RegionId must be RegionId or int");
    }
}

// Enhanced Config class with Pythonic API
class PythonConfig {
private:
    std::shared_ptr<Config> config_;

public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    PythonConfig(const std::shared_ptr<Config>& config) : config_(config) {}

    // Load configuration
    bool load_file(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }

    bool load_args(const std::vector<std::string>& args) {
        int argc = args.size();
        std::vector<char*> argv(argc + 1);
        argv[0] = const_cast<char*>("nlm");
        for (int i = 0; i < argc; ++i) {
            argv[i + 1] = const_cast<char*>(args[i].c_str());
        }
        return config_->loadFromArgs(argc, argv.data());
    }

    // Get configuration values with type conversion
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        auto value = config_->get<T>(key);
        return value ? *value : default_value;
    }

    // Set configuration values
    void set(const std::string& key, const py::object& value) {
        if (py::isinstance<py::bool_(value)>) {
            config_->set(key, value.cast<bool>());
        } else if (py::isinstance<py::int_(value)>) {
            config_->set(key, value.cast<int64_t>());
        } else if (py::isinstance<py::float_(value)>) {
            config_->set(key, value.cast<double>());
        } else if (py::isinstance<py::str>(value)) {
            config_->set(key, value.cast<std::string>());
        } else if (py::isinstance<py::list>(value) && value.cast<std::vector<float>>().size() > 0) {
            config_->set(key, value.cast<std::vector<float>>());
        } else if (py::isinstance<py::list>(value) && value.cast<std::vector<int>>().size() > 0) {
            config_->set(key, value.cast<std::vector<int>>());
        } else {
            throw py::type_error("Unsupported value type for config key: " + key);
        }
    }

    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }

    // Get all keys
    std::vector<std::string> keys() const {
        return config_->getKeys();
    }

    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }

    // Clear configuration
    void clear() {
        config_->clear();
    }

    // Create default config
    static std::shared_ptr<PythonConfig> create_default() {
        auto config = std::make_shared<PythonConfig>();
        // Set default values
        config->set("random_seed", uint64_t(42));
        config->set("max_energy", 100.0);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("development_enabled", true);
        config->set("reward_modulation_enabled", true);
        config->set("structural_plasticity_enabled", true);
        config->set("curiosity_enabled", true);
        config->set("novelty_enabled", true);
        config->set("prediction_enabled", true);
        config->set("planning_enabled", true);
        config->set("attention_enabled", true);
        return config;
    }

    // Convert to underlying Config pointer
    std::shared_ptr<Config> get_raw() const {
        return config_;
    }
};

// Enhanced Brain class with Pythonic API
class PythonBrain {
private:
    std::shared_ptr<Brain> brain_;

public:
    PythonBrain(const std::shared_ptr<Config>& config) : brain_(std::make_shared<Brain>(config)) {}
    PythonBrain(const std::shared_ptr<PythonConfig>& config) : brain_(std::make_shared<Brain>(config->get_raw())) {}

    // Initialize brain
    bool initialize() {
        return brain_->initialize();
    }

    // Simulation step
    void step(size_t step, double time = 0.0) {
        if (time > 0.0) {
            brain_->step(step, time);
        } else {
            brain_->step(step);
        }
    }

    // Get statistics
    size_t total_neurons() const { return brain_->getTotalNeuronCount(); }
    size_t total_synapses() const { return brain_->getTotalSynapseCount(); }
    size_t active_neurons() const { return brain_->getActiveNeuronCount(); }
    size_t firing_neurons() const { return brain_->getFiringNeuronCount(); }
    float firing_rate() const { return brain_->getAverageFiringRate(); }
    float excitation_inhibition_ratio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t total_spikes() const { return brain_->getTotalSpikeCount(); }

    // Region management
    NeuronId add_region(const std::string& name = "") {
        return brain_->addRegion(name);
    }

    // Brain control
    void reset() { brain_->reset(); }
    void save(const std::string& filepath) { brain_->save(filepath); }
    bool load(const std::string& filepath) { return brain_->load(filepath); }

    // Get development stage
    std::string developmental_stage() const {
        static const std::string stage_names[] = {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"};
        auto stage = brain_->getDevelopmentalStage();
        return stage_names[static_cast<size_t>(stage)];
    }

    // Set development stage
    void set_developmental_stage(const std::string& stage_str) {
        static const std::map<std::string, DevelopmentalStage> stage_map = {
            {"Initial", DevelopmentalStage::Initial},
            {"CriticalPeriod", DevelopmentalStage::CriticalPeriod},
            {"Maturation", DevelopmentalStage::Maturation},
            {"Adult", DevelopmentalStage::Adult},
            {"Aging", DevelopmentalStage::Aging}
        };

        auto it = stage_map.find(stage_str);
        if (it != stage_map.end()) {
            brain_->setDevelopmentalStage(it->second);
        } else {
            throw std::invalid_argument("Invalid developmental stage: " + stage_str);
        }
    }

    // Get brain pointer (for advanced usage)
    std::shared_ptr<Brain> get_raw() const { return brain_; }

    // Create a new brain with this configuration
    static std::shared_ptr<PythonBrain> create(const std::shared_ptr<PythonConfig>& config) {
        return std::make_shared<PythonBrain>(config);
    }
};

// Enhanced World class with Pythonic API
class PythonWorld {
private:
    std::shared_ptr<SimpleWorld> world_;

public:
    PythonWorld() : world_(std::make_shared<SimpleWorld>()) {}

    // Configure world
    void configure(float width, float height, size_t vision_width, size_t vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }

    // Get world dimensions
    float width() const { return world_->getWidth(); }
    float height() const { return world_->getHeight(); }

    // Set agent start position
    void set_agent_start(float x, float y) {
        world_->setAgentStart(x, y);
    }

    // Reset world
    void reset() {
        world_->reset();
    }

    // Update world
    void update(double timestep = 0.01) {
        world_->update(timestep);
    }

    // Apply action
    py::dict apply_action(int action_type, double time = 0.0) {
        auto action_result = world_->applyMotorCommand(static_cast<MotorCommand>(action_type), time);
        py::dict result;
        result["reward"] = action_result.reward;
        result["success"] = action_result.success;
        result["message"] = action_result.message;
        return result;
    }

    // Add object to world
    void add_object(float x, float y, int object_type, float value = 0.0f, float radius = 0.5f) {
        world_->addObject(WorldObject(x, y, static_cast<WorldObjectType>(object_type), value, radius));
    }

    // Get agent body
    py::dict get_agent_body() const {
        auto body = world_->getAgentBody();
        py::dict body_dict;
        body_dict["x"] = body.x;
        body_dict["y"] = body.y;
        body_dict["orientation"] = body.orientation;
        body_dict["energy"] = body.energy;
        body_dict["health"] = body.health;
        body_dict["age"] = body.age;
        body_dict["is_moving"] = body.isMoving;
        body_dict["is_turning"] = body.isTurning;
        return body_dict;
    }

    // Get sensory percept
    py::dict get_sensory_percept() const {
        auto percept = world_->getSensoryPercept();
        py::dict percept_dict;
        
        // Vision
        auto vision = percept.getVision();
        if (!vision.empty()) {
            percept_dict["vision"] = vision;
            auto& vision_data = percept_dict["vision"].cast<std::vector<float>&>();
        }
        
        // Touch
        auto touch = percept.getTouch();
        if (!touch.empty()) {
            percept_dict["touch"] = touch;
        }
        
        // Internal
        auto internal = percept.getInternal();
        if (!internal.empty()) {
            percept_dict["internal"] = internal;
        }
        
        // Proprioception
        auto proprioception = percept.getProprioception();
        if (!proprioception.empty()) {
            percept_dict["proprioception"] = proprioception;
        }
        
        // Audio
        auto audio = percept.getAudio();
        if (!audio.empty()) {
            percept_dict["audio"] = audio;
        }
        
        // Timestamp
        percept_dict["timestamp"] = percept.getTimestamp();
        
        return percept_dict;
    }

    // Get simulation time
    double simulation_time() const {
        return world_->getSimulationTime();
    }

    // Set random seed
    void set_random_seed(uint64_t seed) {
        world_->setRandomSeed(seed);
    }

    // Get random seed
    uint64_t random_seed() const {
        return world_->getRandomSeed();
    }

    // Create a new world
    static std::shared_ptr<PythonWorld> create() {
        return std::make_shared<PythonWorld>();
    }
};

// Enhanced AgentBrain class with Pythonic API
class PythonAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;

public:
    PythonAgentBrain(const std::shared_ptr<Brain>& brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}

    // Initialize
    void initialize(const SimpleWorld& world) {
        agent_brain_->initialize(world);
    }

    // Process sensory input
    void process_sensory_input(const py::dict& percept_dict) {
        // Convert dict to SensoryPercept (simplified - would need full implementation)
        agent_brain_->processSensoryInput(SensoryPercept());
    }

    // Get motor command
    int decode_motor_command() {
        return static_cast<int>(agent_brain_->decodeMotorCommand());
    }

    // Apply reward
    void apply_reward(float reward, float predicted_reward = 0.0f) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }

    // Update development
    void update_development(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }

    // Configuration methods
    void enable_reward_modulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }

    void enable_structural_plasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }

    void enable_development(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }

    void enable_curiosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }

    // Get levels
    float neuromodulation_level() const {
        return agent_brain_->getNeuromodulationLevel();
    }

    float curiosity_level() const {
        return agent_brain_->getCuriosityLevel();
    }

    float novelty_level() const {
        return agent_brain_->getNoveltyLevel();
    }

    float prediction_error() const {
        return agent_brain_->getPredictionError();
    }

    std::string developmental_stage() const {
        static const std::string stage_names[] = {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"};
        auto stage = agent_brain_->getDevelopmentalStage();
        return stage_names[static_cast<size_t>(stage)];
    }

    // Reset
    void reset() {
        agent_brain_->reset();
    }

    // Get brain
    std::shared_ptr<Brain> get_brain() const {
        return agent_brain_->getBrain();
    }

    // Create agent brain
    static std::shared_ptr<PythonAgentBrain> create(const std::shared_ptr<Brain>& brain) {
        return std::make_shared<PythonAgentBrain>(brain);
    }
};

// Factory functions
PYBIND11_MODULE(enhanced_pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        A powerful Python API for the NLM (Neural Learning Machine) neural simulation framework.
        
        This module provides enhanced, Pythonic interfaces to all NLM components with:
        - Type-safe configuration management
        - High-level abstractions for brain simulation
        - World interaction utilities
        - Experiment management tools
        - Comprehensive error handling
        - Extensive documentation
    )pbdoc";

    // Configure exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown exception in NLM");
        }
    });

    // Configure iostream redirection
    py::add_ostream_redirect(m, "cout_redirect");

    // Type definitions
    m.def("to_neuron_id", &toNeuronId, "Convert to NeuronId", py::arg("obj"));
    m.def("to_synapse_id", &toSynapseId, "Convert to SynapseId", py::arg("obj"));
    m.def("to_region_id", &toRegionId, "Convert to RegionId", py::arg("obj"));

    // Enums
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Core types
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enhanced Config
    py::class_<PythonConfig>(m, "Config")
        .def(py::init<>())
        .def("load_file", &PythonConfig::load_file, "Load configuration from file", py::arg("filepath"))
        .def("load_args", &PythonConfig::load_args, "Load configuration from arguments", py::arg("args"))
        .def("get", py::overload_cast<const std::string&, const PythonConfig::value_type&>(&PythonConfig::get), 
             "Get configuration value with default", py::arg("key"), py::arg("default_value") = py::none())
        .def("set", &PythonConfig::set, "Set configuration value", py::arg("key"), py::arg("value"))
        .def("has", &PythonConfig::has, "Check if key exists", py::arg("key"))
        .def("keys", &PythonConfig::keys, "Get all configuration keys")
        .def("summary", &PythonConfig::summary, "Get configuration summary")
        .def("clear", &PythonConfig::clear, "Clear all configuration entries")
        .def("create_default", &PythonConfig::create_default, "Create default configuration");

    // Enhanced Brain
    py::class_<PythonBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &PythonBrain::initialize, "Initialize brain with configuration")
        .def("step", py::overload_cast<size_t, double>(&PythonBrain::step),
             "Perform simulation step", py::arg("step"), py::arg("time") = 0.0)
        .def("total_neurons", &PythonBrain::total_neurons, "Get total neuron count")
        .def("total_synapses", &PythonBrain::total_synapses, "Get total synapse count")
        .def("active_neurons", &PythonBrain::active_neurons, "Get active neuron count")
        .def("firing_neurons", &PythonBrain::firing_neurons, "Get firing neuron count")
        .def("firing_rate", &PythonBrain::firing_rate, "Get average firing rate")
        .def("excitation_inhibition_ratio", &PythonBrain::excitation_inhibition_ratio,
             "Get excitation/inhibition ratio")
        .def("total_spikes", &PythonBrain::total_spikes, "Get total spike count")
        .def("add_region", &PythonBrain::add_region, "Add neural region", py::arg("name") = "")
        .def("reset", &PythonBrain::reset, "Reset brain state")
        .def("save", &PythonBrain::save, "Save brain state to file", py::arg("filepath"))
        .def("load", &PythonBrain::load, "Load brain state from file", py::arg("filepath"))
        .def("developmental_stage", &PythonBrain::developmental_stage, "Get developmental stage")
        .def("set_developmental_stage", &PythonBrain::set_developmental_stage,
             "Set developmental stage", py::arg("stage"))
        .def("create", py::overload_cast<std::shared_ptr<PythonConfig>>(&PythonBrain::create),
             "Create brain with configuration", py::arg("config"));

    // Enhanced World
    py::class_<PythonWorld>(m, "World")
        .def(py::init<>())
        .def("configure", &PythonWorld::configure,
             "Configure world", py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"))
        .def("width", &PythonWorld::width, "Get world width")
        .def("height", &PythonWorld::height, "Get world height")
        .def("set_agent_start", &PythonWorld::set_agent_start,
             "Set agent starting position", py::arg("x"), py::arg("y"))
        .def("reset", &PythonWorld::reset, "Reset world to initial state")
        .def("update", &PythonWorld::update, "Update world state", py::arg("timestep") = 0.01)
        .def("apply_action", &PythonWorld::apply_action,
             "Apply motor command", py::arg("action_type"), py::arg("time") = 0.0)
        .def("add_object", &PythonWorld::add_object,
             "Add object to world", py::arg("x"), py::arg("y"), py::arg("object_type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def("get_agent_body", &PythonWorld::get_agent_body, "Get agent body state")
        .def("get_sensory_percept", &PythonWorld::get_sensory_percept, "Get current sensory percept")
        .def("simulation_time", &PythonWorld::simulation_time, "Get simulation time")
        .def("set_random_seed", &PythonWorld::set_random_seed, "Set random seed", py::arg("seed"))
        .def("random_seed", &PythonWorld::random_seed, "Get random seed")
        .def("create", &PythonWorld::create, "Create new world");

    // Enhanced AgentBrain
    py::class_<PythonAgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &PythonAgentBrain::initialize,
             "Initialize with world", py::arg("world"))
        .def("process_sensory_input", &PythonAgentBrain::process_sensory_input,
             "Process sensory percept and inject into brain", py::arg("percept"))
        .def("decode_motor_command", &PythonAgentBrain::decode_motor_command,
             "Decode brain motor activity into motor command")
        .def("apply_reward", &PythonAgentBrain::apply_reward,
             "Apply reward-based neuromodulation", py::arg("reward"), py::arg("predicted_reward") = 0.0f)
        .def("update_development", &PythonAgentBrain::update_development,
             "Update development system", py::arg("timestep"))
        .def("neuromodulation_level", &PythonAgentBrain::neuromodulation_level,
             "Get neuromodulation level")
        .def("curiosity_level", &PythonAgentBrain::curiosity_level,
             "Get curiosity level")
        .def("novelty_level", &PythonAgentBrain::novelty_level,
             "Get novelty level")
        .def("prediction_error", &PythonAgentBrain::prediction_error,
             "Get prediction error")
        .def("developmental_stage", &PythonAgentBrain::developmental_stage,
             "Get developmental stage")
        .def("enable_reward_modulation", &PythonAgentBrain::enable_reward_modulation,
             "Enable/disable reward modulation", py::arg("enable"))
        .def("enable_structural_plasticity", &PythonAgentBrain::enable_structural_plasticity,
             "Enable/disable structural plasticity", py::arg("enable"))
        .def("enable_development", &PythonAgentBrain::enable_development,
             "Enable/disable development", py::arg("enable"))
        .def("enable_curiosity", &PythonAgentBrain::enable_curiosity,
             "Enable/disable curiosity", py::arg("enable"))
        .def("reset", &PythonAgentBrain::reset, "Reset agent for new episode")
        .def("create", py::overload_cast<std::shared_ptr<Brain>>(&PythonAgentBrain::create),
             "Create agent brain with brain", py::arg("brain"));

    // Factory functions
    m.def("create_default_config", &PythonConfig::create_default, "Create default configuration");

    // Utility functions
    m.def("create_brain", &PythonBrain::create, "Create brain with configuration", py::arg("config"));
    m.def("create_world", &PythonWorld::create, "Create new world");
    m.def("create_agent_brain", &PythonAgentBrain::create, "Create agent brain with brain", py::arg("brain"));

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
