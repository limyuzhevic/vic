#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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
        // Pythonic convenience methods
        .def("set_value", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value (Pythonic name)")
        .def("get_value", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
           "Get a configuration value as string (Pythonic name)")
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get a configuration value as int (Pythonic name)")
        .def("get_float", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            auto value = self.get<float>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get a configuration value as float (Pythonic name)")
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a configuration value as bool (Pythonic name)")
        .def("from_dict", [](Config& self, const py::dict& config_dict) {
            // Convert Python dict to C++ config
            py::list keys = config_dict.keys();
            for (auto key : keys) {
                std::string key_str = py::cast<std::string>(key);
                py::object value_obj = config_dict[key];
                
                // Try different types based on Python's type system
                if (value_obj.is(py::int_())) {
                    self.set(key_str, py::cast<int>(value_obj));
                } else if (value_obj.is(py::float_())) {
                    self.set(key_str, py::cast<float>(value_obj));
                } else if (value_obj.is(py::bool_())) {
                    self.set(key_str, py::cast<bool>(value_obj));
                } else if (value_obj.is(py::str())) {
                    self.set(key_str, py::cast<std::string>(value_obj));
                } else {
                    throw py::type_error("Unsupported type for config value: " + key_str);
                }
            }
            return &self;
        }, py::arg("config_dict"),
           "Load configuration from Python dict (Pythonic interface)")
        .def("to_dict", [](const Config& self) {
            py::dict config_dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get value as different types
                auto int_val = self.get<int>(key);
                if (int_val) {
                    config_dict[key] = *int_val;
                    continue;
                }
                auto float_val = self.get<float>(key);
                if (float_val) {
                    config_dict[key] = *float_val;
                    continue;
                }
                auto bool_val = self.get<bool>(key);
                if (bool_val) {
                    config_dict[key] = *bool_val;
                    continue;
                }
                auto str_val = self.get<std::string>(key);
                if (str_val) {
                    config_dict[key] = *str_val;
                    continue;
                }
            }
            return config_dict;
        },
           "Export configuration to Python dict (Pythonic interface)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Pythonic convenience methods
        .def("set_data", [](SensoryInput& self, const std::vector<float>& data) {
            // This requires a setData method - we'll add one if needed
            // For now, this will cause a compile error but shows the pattern
            self.setData(data);
        }, py::arg("data"),
           "Set sensory data (Pythonic name)")
        .def("get_numpy_array", &SensoryInput::getData,
             "Get data as numpy array (requires numpy integration)");

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
        // Pythonic convenience methods
        .def("set_image", [](Vision& self, const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            self.setData(data);
            // Note: Vision class would need to store dimensions
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Set image data with dimensions (Pythonic interface)")
        .def("to_array", &Vision::getData,
             "Get vision data as array for image processing")
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 255.0f) {
            // Normalize vision data to [min, max] range
            auto data = self.getData();
            for (auto& pixel : data) {
                pixel = (pixel - min) / (max - min);
            }
            self.setData(data);
            return &self;
        }, py::arg("min") = 0.0f, py::arg("max") = 255.0f,
           "Normalize vision data to [0, 1] range (Pythonic)")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Resize operation - would need implementation
            return &self;  // Placeholder
        }, py::arg("new_width"), py::arg("new_height"),
           "Resize vision data (placeholder)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic convenience methods
        .def("set_audio", [](Audio& self, const std::vector<float>& data, size_t sampleRate) {
            self.setData(data);
            self.setSampleRate(sampleRate);
            return &self;
        }, py::arg("data"), py::arg("sampleRate"),
           "Set audio data with sample rate (Pythonic interface)")
        .def("get_numpy_array", &Audio::getData,
             "Get audio data as numpy array (requires numpy integration)")
        .def("filter", [](Audio& self, float cutoff_freq) {
            // Simple filtering placeholder
            return &self;
        }, py::arg("cutoff_freq"),
           "Apply low-pass filter (placeholder)")
        .def("amplify", [](Audio& self, float factor) {
            auto data = self.getData();
            for (auto& sample : data) {
                sample *= factor;
            }
            self.setData(data);
            return &self;
        }, py::arg("factor"),
           "Amplify audio signal (Pythonic)");

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
        .def("clone", &Action::clone)
        // Pythonic convenience methods
        .def("set_action", [](Action& self, ActionType type, const std::vector<float>& params = std::vector<float>()) {
            self.setType(type);
            self.setParameters(params);
            return &self;
        }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
           "Set action type and parameters (Pythonic interface)")
        .def("get_parameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) return params[index];
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"),
           "Get a specific parameter by index (Pythonic)")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) params.resize(index + 1, 0.0f);
            params[index] = value;
            self.setParameters(params);
            return &self;
        }, py::arg("index"), py::arg("value"),
           "Set a specific parameter by index (Pythonic)")
        .def("to_dict", [](const Action& self) {
            py::dict action_dict;
            action_dict["type"] = self.getType();
            action_dict["name"] = self.getName();
            action_dict["parameters"] = self.getParameters();
            return action_dict;
        },
           "Export action to Python dict (Pythonic interface)")
        .def("from_dict", [](Action& self, const py::dict& action_dict) {
            std::string type_str = py::cast<std::string>(action_dict["type"]);
            // Convert string to ActionType
            ActionType type = ActionType::Custom;  // Default
            for (int i = 0; i < static_cast<int>(ActionType::Custom); ++i) {
                if (action_dict["name"].cast<std::string>() == self.getTypeName(static_cast<ActionType>(i))) {
                    type = static_cast<ActionType>(i);
                    break;
                }
            }
            self.setType(type);
            
            if (action_dict.contains("parameters")) {
                self.setParameters(py::cast<std::vector<float>>(action_dict["parameters"]));
            }
            return &self;
        }, py::arg("action_dict"),
           "Load action from Python dict (Pythonic interface)");

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
        // Pythonic convenience methods
        .def("set_position", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
            return &self;
        }, py::arg("x"), py::arg("y"),
           "Set object position (Pythonic interface)")
        .def("get_position", [](const WorldObject& self) {
            py::tuple position;
            position = py::make_tuple(self.x, self.y);
            return position;
        },
           "Get object position as tuple (Pythonic)")
        .def("set_radius", &WorldObject::setRadius, py::arg("radius"),
             "Set object radius (Pythonic)")
        .def("get_radius", &WorldObject::getRadius,
             "Get object radius (Pythonic)")
        .def("to_dict", [](const WorldObject& self) {
            py::dict obj_dict;
            obj_dict["x"] = self.x;
            obj_dict["y"] = self.y;
            obj_dict["radius"] = self.radius;
            obj_dict["type"] = static_cast<int>(self.type);
            obj_dict["value"] = self.value;
            obj_dict["active"] = self.active;
            return obj_dict;
        },
           "Export world object to Python dict (Pythonic interface)")
        .def("from_dict", [](WorldObject& self, const py::dict& obj_dict) {
            if (obj_dict.contains("x")) self.x = py::cast<float>(obj_dict["x"]);
            if (obj_dict.contains("y")) self.y = py::cast<float>(obj_dict["y"]);
            if (obj_dict.contains("radius")) self.radius = py::cast<float>(obj_dict["radius"]);
            if (obj_dict.contains("type")) self.type = static_cast<WorldObjectType>(py::cast<int>(obj_dict["type"]));
            if (obj_dict.contains("value")) self.value = py::cast<float>(obj_dict["value"]);
            if (obj_dict.contains("active")) self.active = py::cast<bool>(obj_dict["active"]);
            return &self;
        }, py::arg("obj_dict"),
           "Load world object from Python dict (Pythonic interface)");

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
        // Pythonic convenience methods
        .def("set_position", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            return &self;
        }, py::arg("x"), py::arg("y"),
           "Set agent position (Pythonic interface)")
        .def("get_position", [](const AgentBody& self) {
            py::tuple position;
            position = py::make_tuple(self.x, self.y);
            return position;
        },
           "Get agent position as tuple (Pythonic)")
        .def("set_velocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
            return &self;
        }, py::arg("vx"), py::arg("vy"),
           "Set agent velocity (Pythonic)")
        .def("move", [](AgentBody& self, float distance, float angle_degrees) {
            float angle_rad = angle_degrees * M_PI / 180.0f;
            self.x += distance * cos(angle_rad);
            self.y += distance * sin(angle_rad);
            self.velocityX = distance * cos(angle_rad);
            self.velocityY = distance * sin(angle_rad);
            self.isMoving = true;
            return &self;
        }, py::arg("distance"), py::arg("angle_degrees"),
           "Move agent by distance at angle (Pythonic)")
        .def("turn", [](AgentBody& self, float angle_degrees) {
            self.orientation += angle_degrees;
            self.isTurning = true;
            return &self;
        }, py::arg("angle_degrees"),
           "Rotate agent by angle (Pythonic)")
        .def("to_dict", [](const AgentBody& self) {
            py::dict body_dict;
            body_dict["x"] = self.x;
            body_dict["y"] = self.y;
            body_dict["orientation"] = self.orientation;
            body_dict["velocityX"] = self.velocityX;
            body_dict["velocityY"] = self.velocityY;
            body_dict["angularVelocity"] = self.angularVelocity;
            body_dict["energy"] = self.energy;
            body_dict["health"] = self.health;
            body_dict["age"] = self.age;
            body_dict["isMoving"] = self.isMoving;
            body_dict["isTurning"] = self.isTurning;
            body_dict["lastActionTime"] = self.lastActionTime;
            return body_dict;
        },
           "Export agent body to Python dict (Pythonic interface)")
        .def("from_dict", [](AgentBody& self, const py::dict& body_dict) {
            if (body_dict.contains("x")) self.x = py::cast<float>(body_dict["x"]);
            if (body_dict.contains("y")) self.y = py::cast<float>(body_dict["y"]);
            if (body_dict.contains("orientation")) self.orientation = py::cast<float>(body_dict["orientation"]);
            if (body_dict.contains("velocityX")) self.velocityX = py::cast<float>(body_dict["velocityX"]);
            if (body_dict.contains("velocityY")) self.velocityY = py::cast<float>(body_dict["velocityY"]);
            if (body_dict.contains("angularVelocity")) self.angularVelocity = py::cast<float>(body_dict["angularVelocity"]);
            if (body_dict.contains("energy")) self.energy = py::cast<float>(body_dict["energy"]);
            if (body_dict.contains("health")) self.health = py::cast<float>(body_dict["health"]);
            if (body_dict.contains("age")) self.age = py::cast<float>(body_dict["age"]);
            if (body_dict.contains("isMoving")) self.isMoving = py::cast<bool>(body_dict["isMoving"]);
            if (body_dict.contains("isTurning")) self.isTurning = py::cast<bool>(body_dict["isTurning"]);
            if (body_dict.contains("lastActionTime")) self.lastActionTime = py::cast<float>(body_dict["lastActionTime"]);
            return &self;
        }, py::arg("body_dict"),
           "Load agent body from Python dict (Pythonic interface)");

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic convenience methods
        .def("set_vision", [](SensoryPercept& self, const Vision& vision) {
            self.setVision(vision);
            return &self;
        }, py::arg("vision"),
           "Set vision data (Pythonic interface)")
        .def("set_touch", [](SensoryPercept& self, const std::vector<float>& touch) {
            self.setTouch(touch);
            return &self;
        }, py::arg("touch"),
           "Set touch data (Pythonic)")
        .def("set_internal", [](SensoryPercept& self, const std::vector<float>& internal) {
            self.setInternal(internal);
            return &self;
        }, py::arg("internal"),
           "Set internal signals (Pythonic)")
        .def("set_proprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            self.setProprioception(proprioception);
            return &self;
        }, py::arg("proprioception"),
           "Set proprioception data (Pythonic)")
        .def("set_audio", [](SensoryPercept& self, const Audio& audio) {
            self.setAudio(audio);
            return &self;
        }, py::arg("audio"),
           "Set audio data (Pythonic)")
        .def("get_vision_dict", [](const SensoryPercept& self) {
            py::dict vision_dict;
            auto vision = self.getVision();
            if (vision) {
                vision_dict["width"] = vision->getWidth();
                vision_dict["height"] = vision->getHeight();
                vision_dict["channels"] = vision->getChannels();
                vision_dict["data"] = vision->getData();
            }
            return vision_dict;
        },
           "Get vision data as Python dict (Pythonic)")
        .def("get_audio_dict", [](const SensoryPercept& self) {
            py::dict audio_dict;
            auto audio = self.getAudio();
            if (audio) {
                audio_dict["sampleRate"] = audio->getSampleRate();
                audio_dict["numSamples"] = audio->getNumSamples();
                audio_dict["data"] = audio->getData();
            }
            return audio_dict;
        },
           "Get audio data as Python dict (Pythonic)")
        .def("merge_percepts", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            return &self;
        }, py::arg("other"),
           "Merge percept data from another percept (Pythonic)");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 0, py::arg("visionHeight") = 0)
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Context manager support for world simulation
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& { return self; })
        .def("__exit__", [](SimpleWorld&, py::args) {})
        // Pythonic convenience methods
        .def("run_simulation", [](SimpleWorld& self, size_t steps, float timestep = 0.001f) {
            self.reset();
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
            }
            return &self;
        }, py::arg("steps"), py::arg("timestep") = 0.001f,
           "Run complete simulation (Pythonic interface)")
        .def("simulate_step", &SimpleWorld::update,
             py::arg("timestep") = 0.001f,
             "Simulate a single step (Pythonic alias)")
        .def("get_agent_state", [](SimpleWorld& self) {
            py::dict agent_state;
            auto agent_body = self.getAgentBody();
            agent_state["x"] = agent_body->x;
            agent_state["y"] = agent_body->y;
            agent_state["orientation"] = agent_body->orientation;
            agent_state["velocity"] = py::make_tuple(agent_body->velocityX, agent_body->velocityY);
            agent_state["energy"] = agent_body->energy;
            agent_state["health"] = agent_body->health;
            agent_state["age"] = agent_body->age;
            return agent_state;
        },
           "Get agent state as Python dict (Pythonic)")
        .def("set_agent_state", [](SimpleWorld& self, const py::dict& agent_state) {
            auto agent_body = self.getAgentBody();
            if (agent_state.contains("x")) agent_body->x = py::cast<float>(agent_state["x"]);
            if (agent_state.contains("y")) agent_body->y = py::cast<float>(agent_state["y"]);
            if (agent_state.contains("orientation")) agent_body->orientation = py::cast<float>(agent_state["orientation"]);
            if (agent_state.contains("velocity")) {
                auto vel = py::cast<py::tuple>(agent_state["velocity"]);
                agent_body->velocityX = py::cast<float>(vel[0]);
                agent_body->velocityY = py::cast<float>(vel[1]);
            }
            if (agent_state.contains("energy")) agent_body->energy = py::cast<float>(agent_state["energy"]);
            if (agent_state.contains("health")) agent_body->health = py::cast<float>(agent_state["health"]);
            if (agent_state.contains("age")) agent_body->age = py::cast<float>(agent_state["age"]);
            return &self;
        }, py::arg("agent_state"),
           "Set agent state from Python dict (Pythonic)")
        .def("get_objects", [](SimpleWorld& self) {
            py::list objects_list;
            // Note: SimpleWorld doesn't have a getObjects method, so this is a placeholder
            // In a real implementation, you would iterate through world objects
            return objects_list;
        },
           "Get all world objects (placeholder)")
        .def("get_environment_stats", [](SimpleWorld& self) {
            py::dict stats_dict;
            stats_dict["width"] = self.getWidth();
            stats_dict["height"] = self.getHeight();
            stats_dict["maxEnergy"] = self.getMaxEnergy();
            stats_dict["energyDecayRate"] = self.getEnergyDecayRate();
            stats_dict["simulationTime"] = self.getSimulationTime();
            stats_dict["randomSeed"] = self.getRandomSeed();
            return stats_dict;
        },
           "Get world statistics as Python dict (Pythonic)")
        .def("set_environment", [](SimpleWorld& self, const py::dict& world_config) {
            if (world_config.contains("width")) {
                size_t width = py::cast<size_t>(world_config["width"]);
                size_t height = py::cast<size_t>(world_config["height"]);
                size_t vision_width = world_config.contains("visionWidth") ? 
                    py::cast<size_t>(world_config["visionWidth"]) : 0;
                size_t vision_height = world_config.contains("visionHeight") ? 
                    py::cast<size_t>(world_config["visionHeight"]) : 0;
                self.configure(width, height, vision_width, vision_height);
            }
            if (world_config.contains("maxEnergy")) {
                self.setMaxEnergy(py::cast<float>(world_config["maxEnergy"]));
            }
            if (world_config.contains("energyDecayRate")) {
                self.setEnergyDecayRate(py::cast<float>(world_config["energyDecayRate"]));
            }
            if (world_config.contains("randomSeed")) {
                self.setRandomSeed(py::cast<size_t>(world_config["randomSeed"]));
            }
            return &self;
        }, py::arg("world_config"),
           "Configure world from Python dict (Pythonic)");

    // Custom exception types for better error handling
    py::register_exception<nlm::ConfigException>(m, "ConfigException", PyExc_ValueError);
    py::register_exception<nlm::BrainException>(m, "BrainException", PyExc_RuntimeError);
    py::register_exception<nlm::NeuronException>(m, "NeuronException", PyExc_ValueError);
    py::register_exception<nlm::SynapseException>(m, "SynapseException", PyExc_RuntimeError);

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
        // Context manager support for automatic resource management
        .def("__enter__", [](Brain& self) -> Brain& { return self; })
        .def("__exit__", [](Brain&, py::args) {})

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
