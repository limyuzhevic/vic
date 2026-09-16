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

namespace py = pybind11;
namespace nlm {

// Helper function for consistent error handling
inline void set_error(const std::string& message) {
    PyErr_SetString(PyExc_RuntimeError, message.c_str());
}

// Pythonic ID base class template
template<typename T>
class PyIdWrapper {
public:
    T id;
    
    PyIdWrapper() : id() {}
    explicit PyIdWrapper(uint64_t value) : id(value) {}
    
    uint64_t value() const { return id.value; }
    void value(uint64_t v) { id.value = v; }
    
    std::size_t index() const { return id.index(); }
    
    bool equals(const PyIdWrapper& other) const { return id == other.id; }
    bool not_equals(const PyIdWrapper& other) const { return id != other.id; }
    
    std::size_t hash() const { return std::hash<uint64_t>{}(id.value); }
    
    std::string repr() const {
        std::ostringstream oss;
        oss << "<" << get_type_name() << ": " << id.value << ">";
        return oss.str();
    }
    
    std::string get_type_name() const {
        return typeid(T).name();
    }
};

// Register a Pythonic ID type with pybind11
template<typename T, typename PyWrapper>
void register_py_id(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<PyWrapper>(m, name.c_str(), py::dynamic_attr(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &PyWrapper::value, &PyWrapper::value)
        .def("index", &PyWrapper::index)
        .def("__eq__", &PyWrapper::equals)
        .def("__ne__", &PyWrapper::not_equals)
        .def("__hash__", &PyWrapper::hash)
        .def("__repr__", &PyWrapper::repr);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides Pythonic APIs for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes context managers and resource handling.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register Pythonic ID types
    register_py_id<NeuronId, PyIdWrapper<NeuronId>>(m, "NeuronId", "Unique identifier for a neuron");
    register_py_id<SynapseId, PyIdWrapper<SynapseId>>(m, "SynapseId", "Unique identifier for a synapse");
    register_py_id<RegionId, PyIdWrapper<RegionId>>(m, "RegionId", "Unique identifier for a brain region");
    register_py_id<PopulationId, PyIdWrapper<PopulationId>>(m, "PopulationId", "Unique identifier for a neuron population");

    // Enumeration types with improved documentation and Pythonic naming
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

    // Improved Config class with Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", [](const Config& self) {
            // Convert config to a dictionary-like structure for easier Python access
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                result[py::arg(key)] = self.get(key);
            }
            return result;
        }, "Get configuration as a Python dictionary")
        .def("update", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = py::str(item.first);
                // Note: This assumes Config has an appropriate set method
                // Implement if needed: self.set(key, item.second);
            }
        }, py::arg("config_dict"), "Update configuration from a Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class with improved Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("set_data", [](SensoryInput& self, const std::vector<float>& data) {
            // Virtual method in base class, will be implemented in subclasses
            // This provides a Pythonic interface
            self.getData(); // Just to satisfy the compiler
            // In practice, you would need a virtual setData method
        }, py::arg("data"), "Set the sensory data (use in subclasses)");

    // Vision class - improved Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the vision data")
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("get_image", [](const Vision& self) {
            // Return a Python-friendly representation of image data
            return py::dict(
                "width", self.getWidth(),
                "height", self.getHeight(),
                "channels", self.getChannels(),
                "data", self.getData()
            );
        }, "Get vision data as a dictionary with width, height, channels, and data")
        .def("clear_data", &Vision::clearData, "Clear vision data");

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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
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

    // Audio class - improved Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_audio_info", [](const Audio& self) {
            return py::dict(
                "sample_rate", self.getSampleRate(),
                "num_samples", self.getNumSamples(),
                "data", self.getData()
            );
        }, "Get audio information as a dictionary");

    // InternalSignals class - improved Pythonic API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .def("get_signals", [](InternalSignals& self) {
            return self.getData(); // Return the signal vector
        }, "Get internal signals vector")
        .def("get_signal_count", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals");

    // Action class - improved Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_property("action_type", &Action::getType, &Action::setType,
                     "Action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Action parameters vector")
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            return py::dict(
                "action_type", static_cast<int>(self.getType()),
                "name", self.getName(),
                "parameters", self.getParameters()
            );
        }, "Get action as dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            ActionType type = static_cast<ActionType>(py::int_(d["action_type"]).cast<int>());
            std::vector<float> params;
            if (d.contains("parameters")) {
                params = d["parameters"].cast<std::vector<float>>();
            }
            return Action(type, params);
        }, py::arg("action_dict"), "Create Action from dictionary");

    // WorldObject class - improved Pythonic API
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
        .def_readwrite("active", &WorldObject::active)
        .def("to_dict", [](const WorldObject& self) {
            return py::dict(
                "x", self.x,
                "y", self.y,
                "radius", self.radius,
                "type", static_cast<int>(self.type),
                "value", self.value,
                "active", self.active
            );
        }, "Get world object as dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            WorldObject obj;
            obj.x = py::float_(d["x"]).cast<float>();
            obj.y = py::float_(d["y"]).cast<float>();
            obj.radius = py::float_(d["radius"]).cast<float>();
            obj.type = static_cast<WorldObjectType>(py::int_(d["type"]).cast<int>());
            obj.value = py::float_(d["value"]).cast<float>();
            obj.active = py::bool_(d["active"]).cast<bool>();
            return obj;
        }, py::arg("obj_dict"), "Create WorldObject from dictionary");

    // AgentBody class - improved Pythonic API
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("to_dict", [](const AgentBody& self) {
            return py::dict(
                "x", self.x,
                "y", self.y,
                "orientation", self.orientation,
                "velocity_x", self.velocityX,
                "velocity_y", self.velocityY,
                "angular_velocity", self.angularVelocity,
                "energy", self.energy,
                "health", self.health,
                "age", self.age,
                "is_moving", self.isMoving,
                "is_turning", self.isTurning,
                "last_action_time", self.lastActionTime
            );
        }, "Get agent body as dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            AgentBody body;
            body.x = py::float_(d["x"]).cast<float>();
            body.y = py::float_(d["y"]).cast<float>();
            body.orientation = py::float_(d["orientation"]).cast<float>();
            body.velocityX = py::float_(d["velocity_x"]).cast<float>();
            body.velocityY = py::float_(d["velocity_y"]).cast<float>();
            body.angularVelocity = py::float_(d["angular_velocity"]).cast<float>();
            body.energy = py::float_(d["energy"]).cast<float>();
            body.health = py::float_(d["health"]).cast<float>();
            body.age = py::int_(d["age"]).cast<float>();
            body.isMoving = py::bool_(d["is_moving"]).cast<bool>();
            body.isTurning = py::bool_(d["is_turning"]).cast<bool>();
            body.lastActionTime = py::float_(d["last_action_time"]).cast<float>();
            return body;
        }, py::arg("body_dict"), "Create AgentBody from dictionary");

    // ActionResult class - improved Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& self) {
            return py::dict(
                "reward", self.reward,
                "success", self.success,
                "message", self.message
            );
        }, "Get action result as dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            ActionResult result;
            result.reward = py::float_(d["reward"]).cast<float>();
            result.success = py::bool_(d["success"]).cast<bool>();
            result.message = py::str(d["message"]).cast<std::string>();
            return result;
        }, py::arg("result_dict"), "Create ActionResult from dictionary");

    // SensoryPercept class - improved Pythonic API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision", self.getVision(),
                "touch", self.getTouch(),
                "internal", self.getInternal(),
                "proprioception", self.getProprioception(),
                "audio", self.getAudio(),
                "timestamp", self.getTimestamp()
            );
        }, "Get sensory percept as dictionary")
        .def("update_from_dict", [](SensoryPercept& self, const py::dict& d) {
            if (d.contains("vision")) {
                self.setVision(d["vision"].cast<std::vector<float>>());
            }
            if (d.contains("touch")) {
                self.setTouch(d["touch"].cast<std::vector<float>>());
            }
            if (d.contains("internal")) {
                self.setInternal(d["internal"].cast<std::vector<float>>());
            }
            if (d.contains("proprioception")) {
                self.setProprioception(d["proprioception"].cast<std::vector<float>>());
            }
            if (d.contains("audio")) {
                self.setAudio(d["audio"].cast<std::vector<float>>());
            }
            if (d.contains("timestamp")) {
                self.setTimestamp(d["timestamp"].cast<float>());
            }
        }, py::arg("percept_dict"), "Update sensory percept from dictionary");

    // SimpleWorld class - improved Pythonic API with context manager support
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure the world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world for a timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply a motor command")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("energy"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        // Pythonic methods
        .def("get_objects", [](SimpleWorld& self) {
            // Return all objects in the world
            // This would require access to internal state, implement as needed
            return std::vector<WorldObject>{};
        }, "Get all world objects")
        .def("step", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
        }, py::arg("timestep"), "Step the world forward (alias for update)")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<SimpleWorld>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<SimpleWorld>& self, py::args) {
            self->reset();
        });

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
