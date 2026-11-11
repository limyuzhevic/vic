#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <numpy/ndarray.h>

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

        This module provides access to an experimental artificial developmental brain
        with neurons, synapses, memory systems, neuromodulation, and learning.
    )pbdoc";

    // Exception definitions
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "InvalidArgument", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "OutOfRange", PyExc_IndexError);

    // NeuronId class with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with enhanced Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", &SynapseId::operator>)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class with enhanced Python interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", &RegionId::operator>)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with enhanced Python interface
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("to_string", [](NeuronType type) {
            switch(type) {
                case NeuronType::Excitatory: return std::string("Excitatory");
                case NeuronType::Inhibitory: return std::string("Inhibitory");
                case NeuronType::Modulatory: return std::string("Modulatory");
                case NeuronType::Sensory: return std::string("Sensory");
                case NeuronType::Motor: return std::string("Motor");
                case NeuronType::Internal: return std::string("Internal");
                default: return std::string("Unknown");
            }
        });

    // SynapseType enum with enhanced Python interface
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("to_string", [](SynapseType type) {
            switch(type) {
                case SynapseType::Excitatory: return std::string("Excitatory");
                case SynapseType::Inhibitory: return std::string("Inhibitory");
                case SynapseType::Modulatory: return std::string("Modulatory");
                case SynapseType::Electrical: return std::string("Electrical");
                case SynapseType::GapJunction: return std::string("GapJunction");
                default: return std::string("Unknown");
            }
        });

    // DevelopmentalStage enum with enhanced Python interface
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("to_string", [](DevelopmentalStage stage) {
            switch(stage) {
                case DevelopmentalStage::Initial: return std::string("Initial");
                case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
                case DevelopmentalStage::Maturation: return std::string("Maturation");
                case DevelopmentalStage::Adult: return std::string("Adult");
                case DevelopmentalStage::Aging: return std::string("Aging");
                default: return std::string("Unknown");
            }
        });

    // FiringState enum with enhanced Python interface
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("to_string", [](FiringState state) {
            switch(state) {
                case FiringState::Resting: return std::string("Resting");
                case FiringState::Active: return std::string("Active");
                case FiringState::Refractory: return std::string("Refractory");
                case FiringState::Inhibited: return std::string("Inhibited");
                default: return std::string("Unknown");
            }
        });

    // ActionType enum with enhanced Python interface
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
        .export_values()
        .def("to_string", [](ActionType action) {
            switch(action) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::MoveLeft: return std::string("MoveLeft");
                case ActionType::MoveRight: return std::string("MoveRight");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        });

    // MotorCommand enum with enhanced Python interface
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("to_string", [](MotorCommand cmd) {
            switch(cmd) {
                case MotorCommand::MoveForward: return std::string("MoveForward");
                case MotorCommand::MoveBackward: return std::string("MoveBackward");
                case MotorCommand::TurnLeft: return std::string("TurnLeft");
                case MotorCommand::TurnRight: return std::string("TurnRight");
                case MotorCommand::LookLeft: return std::string("LookLeft");
                case MotorCommand::LookRight: return std::string("LookRight");
                case MotorCommand::Interact: return std::string("Interact");
                case MotorCommand::Wait: return std::string("Wait");
                default: return std::string("Unknown");
            }
        });

    // WorldObjectType enum with enhanced Python interface
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("to_string", [](WorldObjectType type) {
            switch(type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        });

    // Enhanced Config class with more Pythonic API
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Enhanced get methods for better Python integration
        .def("get_int", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get an integer configuration value")
        .def("get_int64", static_cast<std::optional<int64_t> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get a 64-bit integer configuration value")
        .def("get_double", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get a floating-point configuration value")
        .def("get_bool", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get a boolean configuration value")
        .def("get_string", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get a string configuration value")
        .def("get_vector", static_cast<std::optional<std::vector<double>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get a vector of doubles configuration value")
        // Enhanced get_or methods
        .def("get_or_int", &Config::getOr<int>, py::arg("key"), py::arg("default_value"),
             "Get an integer configuration value with default")
        .def("get_or_int64", &Config::getOr<int64_t>, py::arg("key"), py::arg("default_value"),
             "Get a 64-bit integer configuration value with default")
        .def("get_or_double", &Config::getOr<double>, py::arg("key"), py::arg("default_value"),
             "Get a floating-point configuration value with default")
        .def("get_or_bool", &Config::getOr<bool>, py::arg("key"), py::arg("default_value"),
             "Get a boolean configuration value with default")
        .def("get_or_string", &Config::getOr<std::string>, py::arg("key"), py::arg("default_value"),
             "Get a string configuration value with default")
        .def("get_or_vector", &Config::getOr<std::vector<double>>, py::arg("key"), py::arg("default_value"),
             "Get a vector of doubles configuration value with default")
        // Set methods with better Python API
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value with optional source")
        .def("set_int", &Config::set<int>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set an integer configuration value")
        .def("set_int64", &Config::set<int64_t>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a 64-bit integer configuration value")
        .def("set_double", &Config::set<double>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a floating-point configuration value")
        .def("set_bool", &Config::set<bool>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a boolean configuration value")
        .def("set_string", &Config::set<std::string>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a string configuration value")
        .def("set_vector", &Config::set<std::vector<double>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a vector configuration value")
        // Utility methods
        .def("remove", &Config::remove, py::arg("key"), "Remove a configuration key")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", &Config::has, py::arg("key"), "Check if key exists in config")
        .def("__getitem__", [](Config& cfg, const std::string& key) {
            auto value = cfg.getOr<std::string>(key, "");
            return value;
        }, py::arg("key"), "Get configuration value by key (Python-style access)")
        .def("__setitem__", [](Config& cfg, const std::string& key, const ConfigValue& value) {
            cfg.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value by key (Python-style assignment)");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with NumPy support
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_data_numpy", [](Vision& self, py::array_t<float> data) {
            // Convert numpy array to vector
            auto buf = data.request();
            std::vector<float> vec(buf.size);
            std::memcpy(vec.data(), buf.ptr, buf.size * sizeof(float));
            self.setData(vec);
        }, py::arg("data"), "Set data from NumPy array")
        .def("get_data_numpy", [](Vision& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Get data as NumPy array")
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);

    // Enhanced Audio class with NumPy support
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_data_numpy", [](Audio& self, py::array_t<float> data) {
            // Convert numpy array to vector
            auto buf = data.request();
            std::vector<float> vec(buf.size);
            std::memcpy(vec.data(), buf.ptr, buf.size * sizeof(float));
            self.setData(vec);
        }, py::arg("data"), "Set data from NumPy array")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .def("get_signal_count", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("get_signal", [](InternalSignals& self, size_t index) {
            auto data = self.getData();
            if (index < data.size()) {
                return data[index];
            }
            return 0.0f;
        }, py::arg("index"), "Get internal signal at index");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def("from_dict", [](Action& action, const py::dict& data) {
            std::string type_str = py::cast<std::string>(data["type"]);
            ActionType action_type;
            // Convert string to ActionType (simplified for now)
            action.setType(action_type);
            std::vector<float> parameters = py::cast<std::vector<float>>(data["parameters"]);
            action.setParameters(parameters);
        }, py::arg("data"), "Create action from Python dictionary");

    // Enhanced WorldObject class
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_near", [](const WorldObject& obj, float x, float y, float threshold) {
            return obj.distance_to(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if object is near position");

    // Enhanced AgentBody class
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
        .def("get_position", [](AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as (x, y)")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("get_velocity", [](AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get agent velocity as (vx, vy)")
        .def("get_speed", [](AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get agent speed")
        .def("get_direction", [](AgentBody& body) {
            return std::atan2(body.velocityY, body.velocityX);
        }, "Get agent direction in radians");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   ", success=" + std::to_string(result.success) +
                   ", message='" + result.message + "'>";
        })
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert action result to Python dictionary");

    // Enhanced SensoryPercept class
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
        .def("has_vision", [](SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Check if vision data is available")
        .def("has_touch", [](SensoryPercept& self) {
            return self.getTouch() != nullptr;
        }, "Check if touch data is available")
        .def("has_internal", [](SensoryPercept& self) {
            return self.getInternal() != nullptr;
        }, "Check if internal signals are available")
        .def("has_audio", [](SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Check if audio data is available");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        // Enhanced world methods for Python
        .def("get_objects", [](SimpleWorld& self) {
            // Return all active world objects
            std::vector<WorldObject> objects;
            // Note: SimpleWorld doesn't have a public method to get all objects
            // This would need to be added to the C++ API
            return objects;
        }, "Get all world objects")
        .def("get_agent_position", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body->x, body->y);
        }, "Get agent position")
        .def("get_agent_direction", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body->orientation;
        }, "Get agent orientation");

    // Enhanced Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Enhanced brain methods for Python
        .def("create_region", [](Brain& self, const std::string& name) {
            return self.addRegion(name);
        }, py::arg("name"), "Create a region and return its ID")
        .def("get_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count")
        .def("get_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count")
        .def("get_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_firing_count", &Brain::getFiringNeuronCount,
             "Get firing neuron count")
        .def("get_avg_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate")
        .def("get_ei_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("get_stage", &Brain::getDevelopmentalStage,
             "Get developmental stage")
        .def("set_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Enhanced agent brain methods for Python
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        .def("get_brain_ref", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get brain reference (alias for get_brain)");

    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions and constants
    m.def("get_invalid_ids", []() {
        py::dict result;
        result["neuron"] = INVALID_NEURON_ID;
        result["synapse"] = INVALID_SYNAPSE_ID;
        result["region"] = INVALID_REGION_ID;
        result["population"] = INVALID_POPULATION_ID;
        return result;
    }, "Get invalid ID values for error handling");

    // Enhanced type conversion utilities
    m.def("neuron_type_to_string", [](NeuronType type) {
        switch(type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    });

    m.def("synapse_type_to_string", [](SynapseType type) {
        switch(type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    });

    // Performance and utility functions
    m.def("benchmark_neural_computation", [](const std::shared_ptr<Brain>& brain, int steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return py::make_tuple(brain->getTotalSpikeCount(), duration.count());
    }, py::arg("brain"), py::arg("steps"), "Benchmark neural computation performance");

    // Add module metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework with NumPy support and advanced API features";
    m.attr("__license__") = "MIT";
    m.attr("__documentation__") = "https://nlm.readthedocs.io";

    // Add convenience constants
    m.attr("EPSILON") = 1e-6f;
    m.attr("MAX_WEIGHT") = 1.0f;
    m.attr("MIN_WEIGHT") = -1.0f;
    m.attr("DEFAULT_LEARNING_RATE") = 0.01f;
    m.attr("DEFAULT_STDP_LTP") = 0.02f;
    m.attr("DEFAULT_STDP_LTD") = 0.015f;

    // Add example and tutorial functions
    m.def("example_mnist_classification", []() {
        // Example showing how to use NLM for digit classification
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // This would be a simplified example
        return "Example: Load MNIST data, create neural network, train on digits, test accuracy";
    }, "Example: Digit classification using NLM");

    m.def("example_robot_learning", []() {
        // Example showing robot learning in a simulated environment
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 5, 5);
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return "Example: Robot navigation learning with neural control";
    }, "Example: Robot learning in simulated environment");

    // Add experimental features
    m.def("experimental_advanced_plasticity", [](const std::shared_ptr<Brain>& brain) {
        // Enable advanced plasticity rules
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // Enable all plasticity
            }
        }
        return "Advanced plasticity rules enabled";
    }, py::arg("brain"), "Enable advanced plasticity rules");

    m.def("experimental_memory_profiling", [](const std::shared_ptr<Brain>& brain) {
        // Basic memory profiling (in a real implementation, this would be more sophisticated)
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        return py::make_tuple(neuronCount, synapseCount, neuronCount * sizeof(Neuron) + synapseCount * sizeof(Synapse));
    }, py::arg("brain"), "Get memory usage statistics");
}

} // namespace nlm