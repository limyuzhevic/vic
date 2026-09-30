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

    // Custom exception for NLM errors
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        
        // Factory methods
        .def_static("createDefault", []() {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        
        // File operations
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Configuration access
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        
        // Pythonic properties
        .def("get_or", &Config::getOr, py::arg("key"), py::arg("default_value"),
             "Get configuration value with fallback")
        
        // Context manager support
        .def("__enter__", [](Config& self) {
            return &self;
        })
        .def("__exit__", [](Config& self, py::args) {
            // Cleanup can be added here if needed
            return false;
        })
        
        // Pythonic property methods
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return std::string();
        }, py::arg("key"), "Get string value from key (legacy method)")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            return value ? *value : 0;
        }, py::arg("key"), "Get int value from key")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            return value ? *value : 0.0;
        }, py::arg("key"), "Get float value from key")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            return value ? *value : false;
        }, py::arg("key"), "Get bool value from key")
        .def("get_vector", [](const Config& self, const std::string& key) {
            auto value = self.get<std::vector<float>>(key);
            return value ? *value : std::vector<float>();
        }, py::arg("key"), "Get vector value from key")
        
        // Set methods with Pythonic naming
        .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set int value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set bool value")
        .def("set_vector", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set vector value")
        
        // Python iterators
        .def("__iter__", [](Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        })
        
        // String representations
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Custom sensory input exception
    py::register_exception<std::runtime_error>(m, "SensoryError");
    
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        
        // Pythonic properties
        .def_property("timestamp", &SensoryInput::getTimestamp, &SensoryInput::setTimestamp,
                     "Get or set timestamp")
        
        // Context manager for sensory processing
        .def("__enter__", [](SensoryInput& self) {
            return &self;
        })
        .def("__exit__", [](SensoryInput& self, py::args) {
            return false;
        })
        
        // String representations
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        })
        .def("__str__", [](const SensoryInput& input) {
            return "SensoryInput(type=" + std::to_string(static_cast<int>(input.getType())) + ")";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def_property_readonly("width", &Vision::getWidth, "Get vision width")
        .def_property_readonly("height", &Vision::getHeight, "Get vision height")
        .def_property_readonly("channels", &Vision::getChannels, "Get vision channels")
        
        // Pythonic properties
        .def_property("data", &Vision::getData, [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, "Get or set vision data")
        
        // Context manager
        .def("__enter__", [](Vision& self) {
            return &self;
        })
        .def("__exit__", [](Vision& self, py::args) {
            return false;
        })
        
        // String representations
        .def("__repr__", [](const Vision& vision) {
            return "<Vision " + std::to_string(vision.getWidth()) + "x" + std::to_string(vision.getHeight()) + ">";
        })
        .def("__str__", [](const Vision& vision) {
            return "Vision(width=" + std::to_string(vision.getWidth()) +
                   ", height=" + std::to_string(vision.getHeight()) + ")";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples")
        
        // Pythonic properties
        .def_property("sample_rate", &Audio::getSampleRate, &Audio::setSampleRate,
                     "Get or set sample rate")
        .def_property("num_samples", &Audio::getNumSamples, [](Audio& self, size_t) {
            // Read-only property
        }, "Get number of samples")
        .def_property("data", &Audio::getData, [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, "Get or set audio data")
        
        // Context manager
        .def("__enter__", [](Audio& self) {
            return &self;
        })
        .def("__exit__", [](Audio& self, py::args) {
            return false;
        })
        
        // String representations
        .def("__repr__", [](const Audio& audio) {
            return "<Audio sample_rate=" + std::to_string(audio.getSampleRate()) +
                   ", samples=" + std::to_string(audio.getNumSamples()) + ">";
        })
        .def("__str__", [](const Audio& audio) {
            return "Audio(sample_rate=" + std::to_string(audio.getSampleRate()) +
                   ", samples=" + std::to_string(audio.getNumSamples()) + ")";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear all signals")
        
        // Context manager
        .def("__enter__", [](InternalSignals& self) {
            return &self;
        })
        .def("__exit__", [](InternalSignals& self, py::args) {
            self.clearSignals();
            return false;
        })
        
        // String representations
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals signals=" + std::to_string(signals.getData().size()) + ">";
        })
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals(size=" + std::to_string(signals.getData().size()) + ")";
        });

    // Custom action type for Python actions
    py::class_<Action, std::shared_ptr<Action>>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        
        // Factory methods
        .def_static("create", [](ActionType type) {
            return std::make_shared<Action>(type);
        }, py::arg("type"), "Create action from type")
        .def_static("create_with_params", [](ActionType type, const std::vector<float>& params) {
            return std::make_shared<Action>(type, params);
        }, py::arg("type"), py::arg("parameters"), "Create action with parameters")
        .def_static("wait", []() {
            return std::make_shared<Action>(ActionType::Wait);
        }, "Create a wait action")
        .def_static("move_forward", []() {
            return std::make_shared<Action>(ActionType::MoveForward);
        }, "Create a move forward action")
        
        // Pythonic property access
        .def_property("type", &Action::getType, &Action::setType,
                     "Get or set action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Get or set action parameters")
        
        // Convenience properties
        .def("get_name", &Action::getName, "Get action name as string")
        .def("get_type_name", [](const Action& action) {
            switch (action.getType()) {
                case ActionType::MoveForward: return "MoveForward";
                case ActionType::MoveBackward: return "MoveBackward";
                case ActionType::MoveLeft: return "MoveLeft";
                case ActionType::MoveRight: return "MoveRight";
                case ActionType::TurnLeft: return "TurnLeft";
                case ActionType::TurnRight: return "TurnRight";
                case ActionType::Look: return "Look";
                case ActionType::LookUp: return "LookUp";
                case ActionType::LookDown: return "LookDown";
                case ActionType::Interact: return "Interact";
                case ActionType::Eat: return "Eat";
                case ActionType::Drink: return "Drink";
                case ActionType::Rest: return "Rest";
                case ActionType::Wait: return "Wait";
                case ActionType::Custom: return "Custom";
                default: return "Unknown";
            }
        }, "Get action type as readable string")
        
        // Context manager for action execution
        .def("__enter__", [](Action& self) {
            return &self;
        })
        .def("__exit__", [](Action& self, py::args) {
            return false;
        })
        
        // Cloning
        .def("clone", &Action::clone, "Create a copy of this action")
        
        // String representations
        .def("__repr__", [](const Action& action) {
            std::string typeName = "Unknown";
            switch (action.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
            }
            return "<Action: " + typeName + " at " + std::to_string((uintptr_t)&action) + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, value and radius")
        
        // Pythonic properties
        .def_property("x", &WorldObject::x, nullptr, "Get or set x position")
        .def_property("y", &WorldObject::y, nullptr, "Get or set y position")
        .def_property("radius", &WorldObject::radius, nullptr, "Get or set radius")
        .def_property("type", &WorldObject::type, nullptr, "Get or set object type")
        .def_property("value", &WorldObject::value, nullptr, "Get or set object value")
        .def_property("active", &WorldObject::active, nullptr, "Get or set active state")
        
        // Convenience methods
        .def("get_position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get position as (x, y) tuple")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        
        .def("get_type_name", [](const WorldObject& obj) {
            switch (obj.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get object type as string")
        
        // Context manager
        .def("__enter__", [](WorldObject& self) {
            return &self;
        })
        .def("__exit__", [](WorldObject& self, py::args) {
            return false;
        })
        
        // String representations
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName;
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "<WorldObject " + std::to_string(obj.x) + "," + std::to_string(obj.y) +
                   " type=" + typeName + ">";
        })
        .def("__str__", [](const WorldObject& obj) {
            std::string typeName;
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "WorldObject(x=" + std::to_string(obj.x) + ", y=" + std::to_string(obj.y) +
                   ", type=" + typeName + ")";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        
        // Pythonic properties
        .def_property("x", &AgentBody::x, nullptr, "Get or set x position")
        .def_property("y", &AgentBody::y, nullptr, "Get or set y position")
        .def_property("orientation", &AgentBody::orientation, nullptr, "Get or set orientation")
        .def_property("velocity_x", &AgentBody::velocityX, nullptr, "Get or set velocity X")
        .def_property("velocity_y", &AgentBody::velocityY, nullptr, "Get or set velocity Y")
        .def_property("angular_velocity", &AgentBody::angularVelocity, nullptr, "Get or set angular velocity")
        .def_property("energy", &AgentBody::energy, nullptr, "Get or set energy")
        .def_property("health", &AgentBody::health, nullptr, "Get or set health")
        .def_property("age", &AgentBody::age, nullptr, "Get or set age")
        .def_property("is_moving", &AgentBody::isMoving, nullptr, "Get or set moving state")
        .def_property("is_turning", &AgentBody::isTurning, nullptr, "Get or set turning state")
        .def_property("last_action_time", &AgentBody::lastActionTime, nullptr, "Get or set last action time")
        
        // Convenience properties
        .def("get_position", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        
        .def("get_velocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        
        .def("get_orientation_degrees", [](const AgentBody& body) {
            return body.orientation * 180.0f / 3.14159f;
        }, "Get orientation in degrees")
        .def("set_orientation_degrees", [](AgentBody& body, float degrees) {
            body.orientation = degrees * 3.14159f / 180.0f;
        }, py::arg("degrees"), "Set orientation in degrees")
        
        // Context manager
        .def("__enter__", [](AgentBody& self) {
            return &self;
        })
        .def("__exit__", [](AgentBody& self, py::args) {
            return false;
        })
        
        // Reset method
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        
        // String representations
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) + ">";
        })
        .def("__str__", [](const AgentBody& body) {
            return "AgentBody(x=" + std::to_string(body.x) + ", y=" + std::to_string(body.y) + ")";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message")
        
        // Pythonic properties
        .def_property("reward", &ActionResult::reward, nullptr, "Get or set reward value")
        .def_property("success", &ActionResult::success, nullptr, "Get or set success status")
        .def_property("message", &ActionResult::message, nullptr, "Get or set result message")
        
        // Convenience methods
        .def("get_success_string", [](const ActionResult& result) {
            return result.success ? "Success" : "Failure";
        }, "Get success as string")
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if action result is good")
        .def("is_bad", [](const ActionResult& result) {
            return !result.success || result.reward < 0.0f;
        }, "Check if action result is bad")
        
        // Context manager
        .def("__enter__", [](ActionResult& self) {
            return &self;
        })
        .def("__exit__", [](ActionResult& self, py::args) {
            return false;
        })
        
        // String representations
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + ">";
        })
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) +
                   ", success=" + std::to_string(result.success) + ", " + result.message + ")";
        });

    // Custom sensory percept exception
    py::register_exception<std::runtime_error>(m, "SensoryError");
    
    py::class_<SensoryPercept, std::shared_ptr<SensoryPercept>>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        
        // Context manager for percept processing
        .def("__enter__", [](SensoryPercept& self) {
            return &self;
        })
        .def("__exit__", [](SensoryPercept& self, py::args) {
            return false;
        })
        
        // Pythonic property access for vision
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision,
                     "Get or set vision data")
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch,
                     "Get or set touch data")
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal,
                     "Get or set internal data")
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception,
                     "Get or set proprioception data")
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio,
                     "Get or set audio data")
        
        // Convenience methods
        .def("get_vision_width", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated")
        
        .def_property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp,
                     "Get or set timestamp")
        
        // Iterators for all signal types
        .def("__iter__", [](SensoryPercept& self) {
            // Create a tuple iterator for all signals
            return py::make_iterator(
                std::vector<std::pair<std::string, const std::vector<float>>&>{
                    {"vision", self.getVision()},
                    {"touch", self.getTouch()},
                    {"internal", self.getInternal()},
                    {"proprioception", self.getProprioception()},
                    {"audio", self.getAudio()}
                }.begin(),
                std::vector<std::pair<std::string, const std::vector<float>>&>{
                    {"vision", self.getVision()},
                    {"touch", self.getTouch()},
                    {"internal", self.getInternal()},
                    {"proprioception", self.getProprioception()},
                    {"audio", self.getAudio()}
                }.end()
            );
        })
        
        // String representations
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept at " + std::to_string(percept.getTimestamp()) + ">";
        })
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(vision_size=" + std::to_string(percept.getVision().size()) +
                   ", touch_size=" + std::to_string(percept.getTouch().size()) +
                   ", internal_size=" + std::to_string(percept.getInternal().size()) +
                   ", proprioception_size=" + std::to_string(percept.getProprioception().size()) +
                   ", audio_size=" + std::to_string(percept.getAudio().size()) + ")";
        });

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        
        // Factory method
        .def_static("create", []() {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world")
        
        // Configuration
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"), "Configure world dimensions")
        
        // World operations
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("command"), py::arg("current_time"), "Apply motor command")
        
        // Context manager for world simulation
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
            return false;
        })
        
        // Sensory and agent access
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        
        // Object management
        .def("add_object", &SimpleWorld::addObject, py::arg("object"), "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        
        // Pythonic property access
        .def_property_readonly("width", &SimpleWorld::getWidth, "Get world width")
        .def_property_readonly("height", &SimpleWorld::getHeight, "Get world height")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Get or set maximum energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Get or set energy decay rate")
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def_property("random_seed", &SimpleWorld::getRandomSeed, &SimpleWorld::setRandomSeed,
                     "Get or set random seed")
        
        // Iterators for world objects
        .def("__iter__", [](SimpleWorld& self) {
            return py::make_iterator(self.objects_.begin(), self.objects_.end());
        })
        
        // String representations
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ">";
        })
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) +
                   ", height=" + std::to_string(world.getHeight()) +
                   ", agents=" + std::to_string(1) + ")";
        });

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Factory method
        .def_static("create", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        
        // Initialization and lifecycle
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        
        // Context manager for brain simulation
        .def("__enter__", [](Brain& self) {
            self.initialize();
            return &self;
        })
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
            return false;
        })
        
        // Simulation steps
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        
        // Sensory input and injection
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        // Action production
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        
        // Statistics
        .def_property_readonly("total_neuron_count", &Brain::getTotalNeuronCount,
                             "Get total neuron count across all regions")
        .def_property_readonly("total_synapse_count", &Brain::getTotalSynapseCount,
                             "Get total synapse count across all regions")
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount,
                             "Get count of active neurons")
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount,
                             "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                             "Get average firing rate across all neurons")
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
                             "Get excitation/inhibition balance ratio")
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount,
                             "Get total spike count")
        
        // Development
        .def_property("developmental_stage", &Brain::getDevelopmentalStage, &Brain::setDevelopmentalStage,
                     "Get or set developmental stage")
        
        // Configuration
        .def_property_readonly("config", &Brain::getConfig, "Get the configuration")
        
        // Access to subsystems
        .def("get_spike_system", &Brain::getSpikeSystem, "Get spike system")
        .def("get_stdp", &Brain::getSTDP, "Get STDP plasticity")
        .def("get_hebbian", &Brain::getHebbian, "Get Hebbian plasticity")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity")
        
        .def("get_working_memory", &Brain::getWorkingMemory, "Get working memory")
        .def("get_episodic_memory", &Brain::getEpisodicMemory, "Get episodic memory")
        .def("get_associative_memory", &Brain::getAssociativeMemory, "Get associative memory")
        
        .def("get_prediction_system", &Brain::getPredictionSystem, "Get prediction system")
        .def("get_planner", &Brain::getPlanner, "Get neural planner")
        .def("get_concept_formation", &Brain::getConceptFormation, "Get concept formation")
        .def("get_attention", &Brain::getAttention, "Get attentional selection")
        .def("get_development_system", &Brain::getDevelopmentSystem, "Get development system")
        
        .def("get_dopamine", &Brain::getDopamine, "Get dopamine system")
        .def("get_curiosity", &Brain::getCuriosity, "Get curiosity system")
        .def("get_novelty", &Brain::getNovelty, "Get novelty system")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        
        // Logging
        .def("log_status", &Brain::logStatus, "Log brain status")
        
        // String representations
        .def("__repr__", [](const Brain& brain) {
            return "<Brain with " + std::to_string(brain.getTotalNeuronCount()) + " neurons>";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain(neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   ", regions=" + std::to_string(brain.getRegionCount()) +
                   ", spikes=" + std::to_string(brain.getTotalSpikeCount()) + ")";
        });

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        
        // Factory method
        .def_static("create", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain")
        
        // Initialization
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        
        // Sensory and motor processing
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation and development
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
        // State access
        .def_property_readonly("brain", &AgentBrain::getBrain, "Get the underlying brain")
        .def_property_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
                             "Get expected sensory input size")
        .def_property_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
                             "Get expected motor output size")
        
        // Developmental stage
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage,
                             "Get current developmental stage")
        
        // Neuromodulation levels
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                             "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                             "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                             "Get novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                             "Get prediction error")
        
        // Configuration flags
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // Context manager for agent brain session
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        })
        .def("__exit__", [](AgentBrain& self, py::args) {
            self.reset();
            return false;
        })
        
        // Reset and lifecycle
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        
        // String representations
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain connected to brain at " + std::to_string((uintptr_t)agent.getBrain()) + ">";
        })
        .def("__str__", [](const AgentBrain& agent) {
            return "AgentBrain(neuromodulation=" + std::to_string(agent.getNeuromodulationLevel()) +
                   ", curiosity=" + std::to_string(agent.getCuriosityLevel()) + ")";
        });

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
