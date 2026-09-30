// Phase 1: Neuron ID and Basic Types
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for core types
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    
    // Enums
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
    
    // ID types
    struct NeuronId;
    struct SynapseId;
    struct RegionId;
    struct PopulationId;
    
    // Constants
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    extern const RegionId INVALID_REGION_ID;
    extern const PopulationId INVALID_POPULATION_ID;
}

namespace py = pybind11;
namespace nlm {

// Helper function for type conversion and validation
class PyBind11Utils {
public:
    static std::vector<float> validateAndConvertData(const py::object& data) {
        if (py::isinstance<py::list>(data)) {
            return data.cast<std::vector<float>>();
        } else if (py::isinstance<py::tuple>(data)) {
            // Handle tuple conversion
            auto tuple = data.cast<py::tuple>();
            std::vector<float> result;
            for (size_t i = 0; i < tuple.size(); ++i) {
                result.push_back(tuple[i].cast<float>());
            }
            return result;
        } else if (py::isinstance<py::array>(data)) {
            // Handle numpy array
            auto arr = data.cast<py::array>();
            if (arr.ndim() == 1) {
                return std::vector<float>(arr.size(), 0.0f);
            }
            return std::vector<float>(arr.size(), 0.0f);
        }
        return std::vector<float>();
    }
    
    static bool validateNeuronCount(size_t count) {
        return count > 0 && count < 1000000; // Reasonable limits
    }
    
    static bool validateConnectionProbability(float prob) {
        return prob >= 0.0f && prob <= 1.0f;
    }
};

// Configuration helper class for Python
class PyConfig {
public:
    static std::shared_ptr<Config> createSafeConfig() {
        auto config = std::make_shared<Config>();
        
        // Set reasonable defaults for Python users
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        return config;
    }
    
    static bool safeConfigure(std::shared_ptr<Config> config, 
                              const std::string& key, 
                              const py::object& value) {
        try {
            if (key == "neuron_count" && py::isinstance<py::int_>(value)) {
                size_t count = value.cast<size_t>();
                if (!PyBind11Utils::validateNeuronCount(count)) {
                    throw std::runtime_error("neuron_count must be between 1 and 1000000");
                }
                config->set(key, static_cast<int64_t>(count), ConfigSource::User);
            } else if (key == "connection_probability" && py::isinstance<py::float_>(value)) {
                float prob = value.cast<float>();
                if (!PyBind11Utils::validateConnectionProbability(prob)) {
                    throw std::runtime_error("connection_probability must be between 0.0 and 1.0");
                }
                config->set(key, prob, ConfigSource::User);
            } else if (key == "random_seed" && py::isinstance<py::int_>(value)) {
                config->set(key, value.cast<int64_t>(), ConfigSource::User);
            } else {
                // Try to convert based on existing type
                if (config->has(key)) {
                    auto existingValue = config->getAsString(key);
                    if (!existingValue.empty()) {
                        // Simple type-based conversion
                        config->set(key, value.cast<std::string>(), ConfigSource::User);
                    }
                } else {
                    config->set(key, value.cast<std::string>(), ConfigSource::User);
                }
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Configuration error: " << e.what() << std::endl;
            return false;
        }
    }
};

void initPythonBindings(py::module_& m) {
    // Phase 1: Basic Type Definitions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Neuron ID with better Python interface
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
    
    // Synapse ID
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
    
    // Region ID
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
    
    // Population ID
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
    
    // Enums with better Python representations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that reduce firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that produce actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that affect plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses with direct current flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapses with electrical coupling")
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for sensory learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage with moderate plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable plasticity")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining plasticity")
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at resting potential")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited and not firing")
        .export_values();
    
    // Action type enums with more Python-friendly names
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left without moving")
        .value("TurnRight", ActionType::TurnRight, "Turn right without moving")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat food or resource")
        .value("Drink", ActionType::Drink, "Drink water or resource")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without taking any action")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Edible resource or food")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object or obstacle")
        .value("Wall", WorldObjectType::Wall, "Wall or boundary")
        .value("Marker", WorldObjectType::Marker, "Marker or goal")
        .export_values();
}

void initConfigBindings(py::module_& m) {
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
        // Python-friendly convenience methods
        .def("setNeuronCount", [](Config& self, size_t count) {
            if (count > 0 && count < 1000000) {
                self.set("neuron_count", static_cast<int64_t>(count), ConfigSource::User);
                return true;
            }
            return false;
        }, py::arg("count"), "Set neuron count with validation")
        .def("setConnectionProbability", [](Config& self, float prob) {
            if (prob >= 0.0f && prob <= 1.0f) {
                self.set("connection_probability", prob, ConfigSource::User);
                return true;
            }
            return false;
        }, py::arg("probability"), "Set connection probability with validation")
        .def("getNeuronCount", [](Config& self) {
            auto opt = self.get<size_t>("neuron_count");
            return opt ? *opt : 1000ULL;
        }, "Get neuron count")
        .def("getConnectionProbability", [](Config& self) {
            auto opt = self.get<float>("connection_probability");
            return opt ? *opt : 0.1f;
        }, "Get connection probability")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });
}

void initSensoryBindings(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Vision class with enhanced Python interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with dimensions")
        .def("setData", [](Vision& self, const py::object& data) {
            auto converted = PyBind11Utils::validateAndConvertData(data);
            self.setData(converted);
        }, py::arg("data"), "Set vision data from Python object (list, tuple, or array)")
        .def("setDataFromArray", [](Vision& self, const py::array_t<float>& arr) {
            if (arr.ndim() == 2) {
                std::vector<float> data;
                auto buffer = arr.request();
                float* ptr = (float*)buffer.ptr;
                for (ssize_t i = 0; i < arr.shape(0); ++i) {
                    for (ssize_t j = 0; j < arr.shape(1); ++j) {
                        data.push_back(ptr[i * arr.shape(1) + j]);
                    }
                }
                self.setData(data);
            }
        }, py::arg("array"), "Set vision data from 2D numpy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getShape", [](Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get vision dimensions as a list");
    
    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"), "Create audio input")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // Internal signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const py::object& values) {
            auto converted = PyBind11Utils::validateAndConvertData(values);
            for (float val : converted) {
                self.addSignal(val);
            }
        }, py::arg("values"), "Add multiple signals from Python list/tuple")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"), "Get signal by index");
}

void initActionBindings(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action from action type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"), "Create action with parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](Action& self, const std::string& name, float defaultValue = 0.0f) {
            const auto& params = self.getParameters();
            if (name == "x" && params.size() > 0) return params[0];
            if (name == "y" && params.size() > 1) return params[1];
            if (name == "speed" && params.size() > 2) return params[2];
            if (name == "angle" && params.size() > 3) return params[3];
            return defaultValue;
        }, py::arg("name"), py::arg("defaultValue") = 0.0f, "Get parameter by name")
        .def("setParameter", [](Action& self, const std::string& name, float value) {
            auto params = self.getParameters();
            if (name == "x") {
                if (params.size() < 1) params.resize(1, 0.0f);
                params[0] = value;
            } else if (name == "y") {
                if (params.size() < 2) params.resize(2, 0.0f);
                params[1] = value;
            } else if (name == "speed") {
                if (params.size() < 3) params.resize(3, 0.0f);
                params[2] = value;
            } else if (name == "angle") {
                if (params.size() < 4) params.resize(4, 0.0f);
                params[3] = value;
            } else {
                params.push_back(value);
            }
            self.setParameters(params);
        }, py::arg("name"), py::arg("value"), "Set parameter by name")
        .def("clone", &Action::clone)
        .def("toDict", [](Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            Action action;
            if (d.contains("type")) {
                action.setType(py::cast<ActionType>(d["type"]));
            }
            if (d.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(d["parameters"]));
            }
            return action;
        }, py::arg("d"), "Create action from Python dictionary");
}

void initWorldBindings(py::module_& m) {
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and type")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isVisible", [](const WorldObject& self, float radius) {
            return self.active && self.radius > 0.0f;
        }, py::arg("radius"), "Check if object is visible within radius");
    
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is healthy")
        .def("isActive", [](const AgentBody& self) {
            return self.energy > 0.0f && self.health > 0.0f;
        }, "Check if agent is active");
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward and success status")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert result to Python dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            ActionResult result;
            if (d.contains("reward")) {
                result.reward = py::cast<float>(d["reward"]);
            }
            if (d.contains("success")) {
                result.success = py::cast<bool>(d["success"]);
            }
            if (d.contains("message")) {
                result.message = py::cast<std::string>(d["message"]);
            }
            return result;
        }, py::arg("d"), "Create result from Python dictionary")
        .def("__repr__", [](const ActionResult& self) {
            return "<ActionResult: reward=" + std::to_string(self.reward) +
                   ", success=" + std::to_string(self.success) + ", " +
                   "message=\"" + self.message + "\">";
        });
}

void initPerceptBindings(py::module_& m) {
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        // Python-friendly methods
        .def("getVisionShape", [](SensoryPercept& self) {
            return std::vector<size_t>{self.getVisionWidth(), self.getVisionHeight()};
        }, "Get vision shape as list")
        .def("getVisionData", [](SensoryPercept& self) {
            return self.getVision().getData();
        }, "Get vision data as list")
        .def("setVisionFromArray", [](SensoryPercept& self, const py::array_t<float>& arr) {
            if (arr.ndim() == 2) {
                Vision vision;
                std::vector<float> data;
                auto buffer = arr.request();
                float* ptr = (float*)buffer.ptr;
                for (ssize_t i = 0; i < arr.shape(0); ++i) {
                    for (ssize_t j = 0; j < arr.shape(1); ++j) {
                        data.push_back(ptr[i * arr.shape(1) + j]);
                    }
                }
                vision.setData(data);
                self.setVision(vision);
            }
        }, py::arg("array"), "Set vision percept from 2D numpy array")
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
        // Convenience methods
        .def("addInternalSignal", &SensoryPercept::addInternalSignal, py::arg("value"), "Add internal signal")
        .def("addProprioceptionSignal", &SensoryPercept::addProprioceptionSignal, py::arg("value"), "Add proprioception signal")
        .def("getInternalSignalCount", [](SensoryPercept& self) {
            return self.getInternal().getSignalCount();
        }, "Get number of internal signals")
        .def("getProprioceptionSignalCount", [](SensoryPercept& self) {
            return self.getProprioception().getSignalCount();
        }, "Get number of proprioception signals");
}

void initWorldBindings(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        // Python-friendly methods
        .def("createObject", [](SimpleWorld& self, float x, float y, 
                               WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"),
           py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
           "Create and add object to world")
        .def("getObjects", [](SimpleWorld& self) {
            // This would require adding a getObjects method to SimpleWorld class
            return std::vector<WorldObject>();
        }, "Get all objects in world (requires implementation)")
        .def("getObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            // This would require adding a getObjectsByType method to SimpleWorld class
            return std::vector<WorldObject>();
        }, py::arg("type"), "Get objects by type (requires implementation)")
        .def("findClosestObject", [](SimpleWorld& self, float x, float y) {
            // This would require adding a findClosestObject method to SimpleWorld class
            WorldObject closest;
            closest.active = false;
            return closest;
        }, py::arg("x"), py::arg("y"), "Find closest object to point (requires implementation)")
        .def("updateAgent", [](SimpleWorld& self, float x, float y, float orientation) {
            AgentBody body = self.getAgentBody();
            body.x = x;
            body.y = y;
            body.orientation = orientation;
            // Note: This doesn't actually update the agent body in the world
            return body;
        }, py::arg("x"), py::arg("y"), py::arg("orientation"),
           "Update agent position and orientation (requires implementation)");
}

void initBrainBindings(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        // Python-friendly convenience methods
        .def("createRegion", &Brain::addRegion, py::arg("name") = "",
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
        // Convenience methods for Python
        .def("runSteps", [](Brain& self, size_t numSteps, bool withStats = false) {
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(numSteps); ++step) {
                self.step(step);
            }
            if (withStats) {
                self.logStatus();
            }
            return self.getTotalSpikeCount();
        }, py::arg("numSteps"), py::arg("withStats") = false,
           "Run multiple steps and return spike count")
        .def("getNeuronStats", [](Brain& self) {
            py::dict stats;
            stats["totalNeurons"] = self.getTotalNeuronCount();
            stats["totalSynapses"] = self.getTotalSynapseCount();
            stats["activeNeurons"] = self.getActiveNeuronCount();
            stats["firingNeurons"] = self.getFiringNeuronCount();
            stats["totalSpikes"] = self.getTotalSpikeCount();
            stats["avgFiringRate"] = self.getAverageFiringRate();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("getConnectivityStats", [](Brain& self) {
            py::dict stats;
            stats["totalSynapses"] = self.getTotalSynapseCount();
            stats["excitationInhibitionRatio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get connectivity statistics")
        .def("injectSensoryPattern", [](Brain& self, const py::object& pattern) {
            // Create a simple sensory input from pattern
            Vision vision;
            auto converted = PyBind11Utils::validateAndConvertData(pattern);
            vision.setData(converted);
            self.receiveSensoryInput(vision);
        }, py::arg("pattern"), "Inject sensory pattern from Python object");
}

void initAgentBindings(py::module_& m) {
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
        // Enable/disable methods with Python-friendly names
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, 
                            size_t numSteps, bool enableLearning = true) {
            for (size_t step = 0; step < numSteps; ++step) {
                world.update(0.1f);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                // Note: Brain step would be done by external agent loop
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                if (enableLearning) {
                    // Simple reward based on brain activity
                    float reward = self.getNeuromodulationLevel();
                    self.applyRewardModulation(reward, 0.0f);
                }
            }
            return self.getNeuromodulationLevel();
        }, py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
           "Run an episode in the world")
        .def("getAgentStats", [](AgentBrain& self) {
            py::dict stats;
            stats["neuromodulationLevel"] = self.getNeuromodulationLevel();
            stats["curiosityLevel"] = self.getCuriosityLevel();
            stats["noveltyLevel"] = self.getNoveltyLevel();
            stats["predictionError"] = self.getPredictionError();
            stats["developmentalStage"] = self.getDevelopmentalStage();
            return stats;
        }, "Get agent statistics as dictionary");
}

void initModuleFunctions(py::module_& m) {
    // Factory functions with safe defaults for Python users
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return PyConfig::createSafeConfig();
    }, "Create a default configuration with safe Python-friendly settings");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Advanced factory functions
    m.def("createBrainWithConfig", [](size_t neuronCount, float connectionProb) -> std::shared_ptr<Brain> {
        auto config = PyConfig::createSafeConfig();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
        config->set("connection_probability", connectionProb, ConfigSource::User);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), py::arg("connectionProb"), 
       "Create brain with custom configuration");
    
    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), 
       "Create agent brain interface initialized with world");
    
    // Utility functions
    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, 
                                 size_t numSteps, 
                                 bool logProgress = false) {
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(numSteps); ++step) {
            brain->step(step);
            if (logProgress && step % 100 == 0) {
                std::cout << "Step " << step << ", "
                         << "Firing neurons: " << brain->getFiringNeuronCount() << ", "
                         << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"), py::arg("logProgress") = false,
       "Run brain simulation for specified number of steps");
    
    m.def("runAgentEpisode", [](std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world,
                              size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            // Note: Brain step would typically be called by external loop
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return agent->getNeuromodulationLevel();
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"),
       "Run an agent episode in the world");
    
    // Version and utility functions
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__license__") = "MIT";
    
    // Error types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Configuration validation and schema
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        py::dict errors;
        bool valid = true;
        
        auto neuronCountOpt = config->get<size_t>("neuron_count");
        if (neuronCountOpt && (*neuronCountOpt == 0 || *neuronCountOpt > 1000000)) {
            errors["neuron_count"] = "Must be between 1 and 1000000";
            valid = false;
        }
        
        auto connectionProbOpt = config->get<float>("connection_probability");
        if (connectionProbOpt && (*connectionProbOpt < 0.0f || *connectionProbOpt > 1.0f)) {
            errors["connection_probability"] = "Must be between 0.0 and 1.0";
            valid = false;
        }
        
        return std::make_pair(valid, errors);
    }, py::arg("config"), "Validate configuration and return errors if any");
    
    // Batch operations
    m.def("batchStepBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                               const std::vector<size_t>& steps) {
        std::vector<size_t> spikeCounts;
        for (size_t i = 0; i < brains.size(); ++i) {
            for (size_t j = 0; j < steps[i]; ++j) {
                brains[i]->step(static_cast<SimulationStep>(j));
            }
            spikeCounts.push_back(brains[i]->getTotalSpikeCount());
        }
        return spikeCounts;
    }, py::arg("brains"), py::arg("steps"), "Batch step multiple brains");
    
    // Performance and debugging utilities
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["python_bindings"] = "pybind11";
        info["cpp_standard"] = "C++20";
        info["build_type"] = "Release";
        return info;
    }, "Get system information");
    
    m.def("exportBrainStats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Export brain statistics as dictionary");
    
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(numSteps); ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict result;
        result["steps_performed"] = numSteps;
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["execution_time_ms"] = static_cast<double>(duration.count());
        result["spikes_per_second"] = (double)brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        
        return result;
    }, py::arg("brain"), py::arg("numSteps"), "Benchmark brain performance");
}

PYBIND11_MODULE(pynlm, m) {
    initPythonBindings(m);
    initConfigBindings(m);
    initSensoryBindings(m);
    initActionBindings(m);
    initWorldBindings(m);
    initPerceptBindings(m);
    initBrainBindings(m);
    initAgentBindings(m);
    initModuleFunctions(m);
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the NLM (熙然) brain simulation framework,
        enabling you to create, train, and interact with artificial brains that
        learn from experience through neural dynamics, plasticity, and memory.
        
        Key Features:
        - Create virtual brains with configurable neurons and connections
        - Simulate brain activity over time with LIF neuron dynamics
        - Use memory systems (working, episodic, associative)
        - Apply learning through STDP and Hebbian plasticity
        - Use neuromodulation (dopamine, curiosity, novelty)
        - Develop and evolve brains over time
        - Save and load brain states
        - Connect brains to simulated environments
        - Run complete agent-based learning episodes
        
        Basic Usage:
        ```python
        import pynlm
        
        # Create brain with default configuration
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for i in range(1000):
            brain.step(i)
        
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run episode
        for step in range(500):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
        
        For complete documentation, see the NLM project documentation.
    )pbdoc";
}

} // namespace nlm