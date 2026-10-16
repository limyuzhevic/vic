#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

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

// Helper template for registering enums to reduce code duplication
template<typename EnumType>
void register_enum(py::module_& m, const std::string& name, const std::string& doc) {
    py::enum_<EnumType> enum_class(m, name.c_str(), py::doc(doc.c_str()));
    
    // Get all enum values dynamically
    auto values = get_enum_values<EnumType>();
    for (const auto& pair : values) {
        enum_class.value(pair.first.c_str(), pair.second);
    }
    enum_class.export_values();
}

// Helper to get enum values
template<typename EnumType>
std::unordered_map<std::string, EnumType> get_enum_values() {
    std::unordered_map<std::string, EnumType> values;
    
    // This is a compile-time helper - we need to specialize for each enum
    // For now, we'll implement this per enum manually in the implementation
    // but the template provides a consistent interface
    return values;
}

// Python error types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError(msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError(msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Unique identifier classes with better Python integration
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enum registrations using improved helper functions
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, " excitatory neuron that promotes firing in other neurons")
        .value("Inhibitory", NeuronType::Inhibitory, " inhibitory neuron that reduces firing in other neurons")
        .value("Modulatory", NeuronType::Modulatory, " neuron that modulates synaptic plasticity")
        .value("Sensory", NeuronType::Sensory, " neuron that receives sensory input")
        .value("Motor", NeuronType::Motor, " neuron that controls motor output")
        .value("Internal", NeuronType::Internal, " internal processing neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, " excitatory synapse that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, " inhibitory synapse that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, " synapse that modulates plasticity")
        .value("Electrical", SynapseType::Electrical, " electrical synapse with direct current flow")
        .value("GapJunction", SynapseType::GapJunction, " gap junction synapse for electrical coupling")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, " initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, " sensitive period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, " maturation phase with growth")
        .value("Adult", DevelopmentalStage::Adult, " fully mature adult stage")
        .value("Aging", DevelopmentalStage::Aging, " aging phase with decline")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, " neuron not currently firing")
        .value("Active", FiringState::Active, " neuron currently firing action potential")
        .value("Refractory", FiringState::Refractory, " neuron recovering after firing")
        .value("Inhibited", FiringState::Inhibited, " neuron actively suppressed")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, " move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, " move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, " move left in world")
        .value("MoveRight", ActionType::MoveRight, " move right in world")
        .value("TurnLeft", ActionType::TurnLeft, " turn left in world")
        .value("TurnRight", ActionType::TurnRight, " turn right in world")
        .value("Look", ActionType::Look, " look in current direction")
        .value("LookUp", ActionType::LookUp, " look up")
        .value("LookDown", ActionType::LookDown, " look down")
        .value("Interact", ActionType::Interact, " interact with world object")
        .value("Eat", ActionType::Eat, " eat food/resource")
        .value("Drink", ActionType::Drink, " drink water")
        .value("Rest", ActionType::Rest, " rest and recover energy")
        .value("Wait", ActionType::Wait, " wait without action")
        .value("Custom", ActionType::Custom, " custom action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, " move forward one unit")
        .value("MoveBackward", MotorCommand::MoveBackward, " move backward one unit")
        .value("TurnLeft", MotorCommand::TurnLeft, " turn left 90 degrees")
        .value("TurnRight", MotorCommand::TurnRight, " turn right 90 degrees")
        .value("LookLeft", MotorCommand::LookLeft, " look left")
        .value("LookRight", MotorCommand::LookRight, " look right")
        .value("Interact", MotorCommand::Interact, " interact with object")
        .value("Wait", MotorCommand::Wait, " do nothing")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, " empty space with no object")
        .value("Resource", WorldObjectType::Resource, " consumable resource")
        .value("Hazard", WorldObjectType::Hazard, " dangerous object")
        .value("Wall", WorldObjectType::Wall, " impassable wall")
        .value("Marker", WorldObjectType::Marker, " special marker/target")
        .export_values();

    // Config class with improved Python API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("get", &Config::get, py::arg("key"),
             "Get configuration value by key. Returns None if key doesn't exist.")
        .def("get_or", &Config::getOr, py::arg("key"), py::arg("default_value"),
             "Get configuration value by key with default fallback")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                throw ConfigurationError("Value cannot be None");
            }
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<double>>();
                self.set(key, list, ConfigSource::Runtime);
            } else {
                throw ConfigurationError("Unsupported value type: " + value.attr("__class__").attr("__name__").cast<std::string>());
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value. Accepts int, float, bool, str, or list.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"))
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("__contains__", &Config::has, py::arg("key"))
        .def("__len__", &Config::getKeys)
        .def("__iter__", [](Config& self) {
            py::list keys;
            auto keyList = self.getKeys();
            for (const auto& key : keyList) {
                keys.append(key);
            }
            return keys;
        });

    // SensoryInput classes with improved Python API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with Pythonic properties
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def_readwrite("width", &Vision::getWidth) // This will need a property API
        .def_readwrite("height", &Vision::getHeight) // This will need a property API
        .def_readwrite("channels", &Vision::getChannels); // This will need a property API

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
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData); // Get all signals

    // Action class with Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def_readwrite("type", &Action::getType)
        .def_readwrite("parameters", &Action::getParameters)
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            return result;
        }, "Convert action to dictionary representation")
        .def_static("from_dict", [](const py::dict& d) {
            Action action;
            if (d.contains("type")) {
                action.setType(py::cast<ActionType>(d["type"]));
            }
            if (d.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(d["parameters"]));
            }
            return action;
        }, py::arg("dict"), "Create action from dictionary representation");

    // WorldObject class with Pythonic API
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
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to position")
        .def("is_at", [](const WorldObject& self, float x, float y, float tolerance = 0.1f) {
            return self.distance_to(x, y) <= tolerance;
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f, "Check if object is at position within tolerance")
        .def("to_dict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["type"] = obj.type;
            result["value"] = obj.value;
            result["radius"] = obj.radius;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to dictionary representation")
        .def_static("from_dict", [](const py::dict& d) {
            WorldObject obj;
            if (d.contains("x")) obj.x = py::cast<float>(d["x"]);
            if (d.contains("y")) obj.y = py::cast<float>(d["y"]);
            if (d.contains("type")) obj.type = py::cast<WorldObjectType>(d["type"]);
            if (d.contains("value")) obj.value = py::cast<float>(d["value"]);
            if (d.contains("radius")) obj.radius = py::cast<float>(d["radius"]);
            if (d.contains("active")) obj.active = py::cast<bool>(d["active"]);
            return obj;
        }, py::arg("dict"), "Create world object from dictionary representation");

    // AgentBody class with Pythonic API
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
        .def("reset", &AgentBody::reset)
        .def("get_position", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("get_velocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("set_velocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity");

    // ActionResult class with Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert action result to dictionary")
        .def_static("from_dict", [](const py::dict& d) {
            ActionResult result;
            if (d.contains("reward")) result.reward = py::cast<float>(d["reward"]);
            if (d.contains("success")) result.success = py::cast<bool>(d["success"]);
            if (d.contains("message")) result.message = py::cast<std::string>(d["message"]);
            return result;
        }, py::arg("dict"), "Create action result from dictionary");

    // SensoryPercept class with Pythonic API
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert sensory percept to dictionary representation")
        .def_static("from_dict", [](const py::dict& d) {
            SensoryPercept percept;
            if (d.contains("vision")) percept.setVision(py::cast<Vision>(d["vision"]));
            if (d.contains("touch")) percept.setTouch(py::cast<Touch>(d["touch"]));
            if (d.contains("internal")) percept.setInternal(py::cast<InternalSignals>(d["internal"]));
            if (d.contains("proprioception")) percept.setProprioception(py::cast<Proprioception>(d["proprioception"]));
            if (d.contains("audio")) percept.setAudio(py::cast<Audio>(d["audio"]));
            if (d.contains("timestamp")) percept.setTimestamp(py::cast<Timestamp>(d["timestamp"]));
            return percept;
        }, py::arg("dict"), "Create sensory percept from dictionary representation")
        .def("copy", &SensoryPercept::clone, "Create a copy of this percept");

    // SimpleWorld class with Pythonic API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object to simulation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy for agents")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("to_dict", [](const SimpleWorld& world) {
            py::dict result;
            result["width"] = world.getWidth();
            result["height"] = world.getHeight();
            result["maxEnergy"] = world.getMaxEnergy();
            result["energyDecayRate"] = world.getEnergyDecayRate();
            result["simulationTime"] = world.getSimulationTime();
            return result;
        }, "Get world state as dictionary")
        .def("step", [](SimpleWorld& world, float timestep) {
            world.update(timestep);
        }, py::arg("timestep"), "Alias for update method")
        .def("get_agent_position", [](SimpleWorld& world) {
            auto& body = world.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as tuple")
        .def("set_agent_position", [](SimpleWorld& world, float x, float y) {
            world.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position");

    // Brain class with improved Python API
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
             "Log brain status")
        .def("getStatistics", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["developmental_stage"] = brain.getDevelopmentalStage();
            return stats;
        }, "Get comprehensive brain statistics")
        .def("getStatus", [](Brain& brain) {
            py::dict status;
            status["total_neurons"] = brain.getTotalNeuronCount();
            status["total_synapses"] = brain.getTotalSynapseCount();
            status["firing_rate"] = brain.getAverageFiringRate();
            status["developmental_stage"] = brain.getDevelopmentalStage();
            return status;
        }, "Get brief brain status")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        });

    // AgentBrain class with improved Python API
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getFeatures", [](AgentBrain& agent) {
            py::dict features;
            features["neuromodulation_level"] = agent.getNeuromodulationLevel();
            features["curiosity_level"] = agent.getCuriosityLevel();
            features["novelty_level"] = agent.getNoveltyLevel();
            features["prediction_error"] = agent.getPredictionError();
            features["developmental_stage"] = agent.getDevelopmentalStage();
            features["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            features["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            features["development_enabled"] = agent.isDevelopmentEnabled();
            features["curiosity_enabled"] = agent.isCuriosityEnabled();
            return features;
        }, "Get all agent features as dictionary");

    // Module-level functions
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

    // Pythonic convenience functions
    m.def("run_brain_simulation", [](std::shared_ptr<Brain> brain, 
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    size_t num_steps,
                                    float timestep = 0.1f) {
        for (size_t step = 0; step < num_steps; ++step) {
            if (world) {
                world->update(timestep);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
            }
            brain->step(static_cast<SimulationStep>(step));
            if (agent) {
                auto action = agent->decodeMotorCommand();
                if (world) {
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("timestep") = 0.1f,
       "Run a complete brain simulation with world and agent");

    m.def("create_brain_from_dict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            auto value = item.second;
            if (value.is_none()) {
                throw ConfigurationError("Configuration value cannot be None for key: " + key);
            }
            if (py::isinstance<py::int_>(value)) {
                config->set(key, value.cast<int64_t>(), ConfigSource::File);
            } else if (py::isinstance<py::float_>(value)) {
                config->set(key, value.cast<double>(), ConfigSource::File);
            } else if (py::isinstance<py::bool_>(value)) {
                config->set(key, value.cast<bool>(), ConfigSource::File);
            } else if (py::isinstance<py::str>(value)) {
                config->set(key, value.cast<std::string>(), ConfigSource::File);
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<double>>();
                config->set(key, list, ConfigSource::File);
            }
        }
        return config;
    }, py::arg("config_dict"), "Create a brain configuration from dictionary");

    // Error types
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Utility functions
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (!value) continue;
            // Basic validation logic
            if (key == "neuron_count") {
                try {
                    int64_t count = std::stoll(*value);
                    if (count < 0) {
                        throw ConfigurationError("neuron_count must be non-negative");
                    }
                } catch (...) {
                    throw ConfigurationError("Invalid neuron_count value: " + *value);
                }
            }
        }
        return true;
    }, py::arg("config"), "Validate configuration and return True if valid");

    m.def("generate_report", [](const std::shared_ptr<Brain>& brain) {
        std::stringstream ss;
        ss << "=== NLM Brain Report ===\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        ss << "==========================\n";
        return ss.str();
    }, py::arg("brain"), "Generate a human-readable report of brain state");
}

} // namespace nlm
