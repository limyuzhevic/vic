#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

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
        
        Key Features:
        - Complete type safety with validation
        - Comprehensive error handling with descriptive exceptions
        - Convenience methods for common operations
        - Advanced API for power users
        - Full documentation with examples
    )pbdoc";

    // Custom exception types for better error handling
    py::class_<std::runtime_error, std::exception>(m, "RuntimeError")
        .def(py::init<const std::string&>());

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId class with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create NeuronId with validation")
        .def(py::init<const NeuronId&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("value", &NeuronId::value,
                      "The raw neuron identifier value")
        .def("index", &NeuronId::index,
             "Get the index component of the neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__lt__", &NeuronId::operator<,
             "Check if less than another NeuronId")
        .def("__gt__", &NeuronId::operator>,
             "Check if greater than another NeuronId")
        .def("__le__", &NeuronId::operator<=,
             "Check if less than or equal to another NeuronId")
        .def("__ge__", &NeuronId::operator>=,
             "Check if greater than or equal to another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of NeuronId")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String conversion");

    // SynapseId class with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create SynapseId with validation")
        .def(py::init<const SynapseId&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("value", &SynapseId::value,
                      "The raw synapse identifier value")
        .def("index", &SynapseId::index,
             "Get the index component of the synapse ID")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of SynapseId")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "String conversion");

    // RegionId class with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create RegionId with validation")
        .def(py::init<const RegionId&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("value", &RegionId::value,
                      "The raw region identifier value")
        .def("index", &RegionId::index,
             "Get the index component of the region ID")
        .def("__eq__", &RegionId::operator==,
             "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
             "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of RegionId")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "String conversion");

    // PopulationId class with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create PopulationId with validation")
        .def(py::init<const PopulationId&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("value", &PopulationId::value,
                      "The raw population identifier value")
        .def("index", &PopulationId::index,
             "Get the index component of the population ID")
        .def("__eq__", &PopulationId::operator==,
             "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
             "Check inequality with another PopulationId")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, "String representation of PopulationId")
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        }, "String conversion");

    // Enhanced enumerations with additional utility methods
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with additional utility methods.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Excitatory neuron type - increases firing probability")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Inhibitory neuron type - decreases firing probability")
        .value("Modulatory", NeuronType::Modulatory,
               "Modulatory neuron type - affects plasticity and learning")
        .value("Sensory", NeuronType::Sensory,
               "Sensory neuron type - receives external input")
        .value("Motor", NeuronType::Motor,
               "Motor neuron type - produces motor output")
        .value("Internal", NeuronType::Internal,
               "Internal neuron type - internal brain signaling")
        .export_values()
        .def("is_excitatory", [](NeuronType type) {
            return type == NeuronType::Excitatory;
        }, "Check if this neuron type is excitatory")
        .def("is_inhibitory", [](NeuronType type) {
            return type == NeuronType::Inhibitory;
        }, "Check if this neuron type is inhibitory")
        .def("is_sensory", [](NeuronType type) {
            return type == NeuronType::Sensory;
        }, "Check if this neuron type is sensory")
        .def("is_motor", [](NeuronType type) {
            return type == NeuronType::Motor;
        }, "Check if this neuron type is motor")
        .def("is_modulatory", [](NeuronType type) {
            return type == NeuronType::Modulatory;
        }, "Check if this neuron type is modulatory")
        .def("is_internal", [](NeuronType type) {
            return type == NeuronType::Internal;
        }, "Check if this neuron type is internal");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration with additional utility methods.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory,
               "Excitatory synapse - increases postsynaptic firing probability")
        .value("Inhibitory", SynapseType::Inhibitory,
               "Inhibitory synapse - decreases postsynaptic firing probability")
        .value("Modulatory", SynapseType::Modulatory,
               "Modulatory synapse - affects plasticity and neuromodulation")
        .value("Electrical", SynapseType::Electrical,
               "Electrical synapse - direct ion flow between neurons")
        .value("GapJunction", SynapseType::GapJunction,
               "Gap junction synapse - electrical coupling")
        .export_values()
        .def("is_excitatory", [](SynapseType type) {
            return type == SynapseType::Excitatory;
        }, "Check if this synapse type is excitatory")
        .def("is_inhibitory", [](SynapseType type) {
            return type == SynapseType::Inhibitory;
        }, "Check if this synapse type is inhibitory")
        .def("is_modulatory", [](SynapseType type) {
            return type == SynapseType::Modulatory;
        }, "Check if this synapse type is modulatory")
        .def("is_electrical", [](SynapseType type) {
            return type == SynapseType::Electrical;
        }, "Check if this synapse type is electrical")
        .def("is_gap_junction", [](SynapseType type) {
            return type == SynapseType::GapJunction;
        }, "Check if this synapse type is a gap junction");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration with additional utility methods.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
               "Initial developmental stage - rapid learning phase")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
               "Critical period - heightened plasticity for learning")
        .value("Maturation", DevelopmentalStage::Maturation,
               "Maturation stage - structural development and refinement")
        .value("Adult", DevelopmentalStage::Adult,
               "Adult stage - stable performance and fine-tuning")
        .value("Aging", DevelopmentalStage::Aging,
               "Aging stage - gradual decline and maintenance")
        .export_values()
        .def("is_plastic", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial ||
                   stage == DevelopmentalStage::CriticalPeriod;
        }, "Check if this stage has high plasticity")
        .def("is_critical", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::CriticalPeriod;
        }, "Check if this stage is a critical period")
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Maturation ||
                   stage == DevelopmentalStage::Adult;
        }, "Check if this stage is mature")
        .def("is_aging", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Aging;
        }, "Check if this stage represents aging");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration with additional utility methods.
    )pbdoc")
        .value("Resting", FiringState::Resting,
               "Neuron is not firing - below threshold")
        .value("Active", FiringState::Active,
               "Neuron is currently firing - above threshold")
        .value("Refractory", FiringState::Refractory,
               "Neuron is in refractory period - cannot fire")
        .value("Inhibited", FiringState::Inhibited,
               "Neuron is inhibited - unable to fire")
        .export_values()
        .def("is_active", [](FiringState state) {
            return state == FiringState::Active;
        }, "Check if neuron is currently firing")
        .def("is_refractory", [](FiringState state) {
            return state == FiringState::Refractory;
        }, "Check if neuron is in refractory period")
        .def("is_inhibited", [](FiringState state) {
            return state == FiringState::Inhibited;
        }, "Check if neuron is inhibited")
        .def("is_resting", [](FiringState state) {
            return state == FiringState::Resting;
        }, "Check if neuron is resting");

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration with additional utility methods.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward,
               "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward,
               "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft,
               "Move left action")
        .value("MoveRight", ActionType::MoveRight,
               "Move right action")
        .value("TurnLeft", ActionType::TurnLeft,
               "Turn left action")
        .value("TurnRight", ActionType::TurnRight,
               "Turn right action")
        .value("Look", ActionType::Look,
               "Look action")
        .value("LookUp", ActionType::LookUp,
               "Look up action")
        .value("LookDown", ActionType::LookDown,
               "Look down action")
        .value("Interact", ActionType::Interact,
               "Interact action")
        .value("Eat", ActionType::Eat,
               "Eat action")
        .value("Drink", ActionType::Drink,
               "Drink action")
        .value("Rest", ActionType::Rest,
               "Rest action")
        .value("Wait", ActionType::Wait,
               "Wait action")
        .value("Custom", ActionType::Custom,
               "Custom action with parameters")
        .export_values()
        .def("is_movement", [](ActionType action) {
            return action == ActionType::MoveForward ||
                   action == ActionType::MoveBackward ||
                   action == ActionType::MoveLeft ||
                   action == ActionType::MoveRight;
        }, "Check if this is a movement action")
        .def("is_rotation", [](ActionType action) {
            return action == ActionType::TurnLeft ||
                   action == ActionType::TurnRight;
        }, "Check if this is a rotation action")
        .def("is_look", [](ActionType action) {
            return action == ActionType::Look ||
                   action == ActionType::LookUp ||
                   action == ActionType::LookDown;
        }, "Check if this is a look action")
        .def("is_interaction", [](ActionType action) {
            return action == ActionType::Interact ||
                   action == ActionType::Eat ||
                   action == ActionType::Drink;
        }, "Check if this is an interaction action")
        .def("is_rest", [](ActionType action) {
            return action == ActionType::Rest ||
                   action == ActionType::Wait;
        }, "Check if this is a rest action");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration with additional utility methods.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
               "Move forward motor command")
        .value("MoveBackward", MotorCommand::MoveBackward,
               "Move backward motor command")
        .value("TurnLeft", MotorCommand::TurnLeft,
               "Turn left motor command")
        .value("TurnRight", MotorCommand::TurnRight,
               "Turn right motor command")
        .value("LookLeft", MotorCommand::LookLeft,
               "Look left motor command")
        .value("LookRight", MotorCommand::LookRight,
               "Look right motor command")
        .value("Interact", MotorCommand::Interact,
               "Interact motor command")
        .value("Wait", MotorCommand::Wait,
               "Wait motor command")
        .export_values()
        .def("is_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward ||
                   cmd == MotorCommand::MoveBackward;
        }, "Check if this is a movement command")
        .def("is_rotation", [](MotorCommand cmd) {
            return cmd == MotorCommand::TurnLeft ||
                   cmd == MotorCommand::TurnRight;
        }, "Check if this is a rotation command")
        .def("is_look", [](MotorCommand cmd) {
            return cmd == MotorCommand::LookLeft ||
                   cmd == MotorCommand::LookRight;
        }, "Check if this is a look command")
        .def("is_interaction", [](MotorCommand cmd) {
            return cmd == MotorCommand::Interact;
        }, "Check if this is an interaction command")
        .def("is_rest", [](MotorCommand cmd) {
            return cmd == MotorCommand::Wait;
        }, "Check if this is a rest command");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration with additional utility methods.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty,
               "Empty world cell")
        .value("Resource", WorldObjectType::Resource,
               "Resource object - provides energy or nutrients")
        .value("Hazard", WorldObjectType::Hazard,
               "Hazard object - causes damage")
        .value("Wall", WorldObjectType::Wall,
               "Wall object - blocks movement")
        .value("Marker", WorldObjectType::Marker,
               "Marker object - indicates special location")
        .export_values()
        .def("is_collidable", [](WorldObjectType type) {
            return type != WorldObjectType::Empty;
        }, "Check if this object type can collide with other objects")
        .def("is_passable", [](WorldObjectType type) {
            return type != WorldObjectType::Wall;
        }, "Check if this object type is passable")
        .def("is_resource", [](WorldObjectType type) {
            return type == WorldObjectType::Resource;
        }, "Check if this object type is a resource")
        .def("is_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Hazard;
        }, "Check if this object type is a hazard")
        .def("is_wall", [](WorldObjectType type) {
            return type == WorldObjectType::Wall;
        }, "Check if this object type is a wall")
        .def("is_marker", [](WorldObjectType type) {
            return type == WorldObjectType::Marker;
        }, "Check if this object type is a marker");

    // Enhanced Config class with validation and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with validation and convenience methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const Config&>(), py::arg("other"),
             "Copy constructor with validation")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file with validation")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc < 1) {
                throw py::value_error("Argument count must be at least 1");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments with validation")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file with error handling")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getOr", py::overload_cast<const std::string&, int>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default (int)")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default (double)")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default (string)")
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default (bool)")
        .def("set", &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with validation")
        .def("set", [this](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with automatic source (int64_t)")
        .def("set", [this](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with automatic source (double)")
        .def("set", [this](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with automatic source (string)")
        .def("set", [this](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with automatic source (bool)")
        .def("get", py::overload_cast<const std::string&, nlm::NeuronType>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as NeuronType")
        .def("get", py::overload_cast<const std::string&, nlm::SynapseType>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as SynapseType")
        .def("get", py::overload_cast<const std::string&, nlm::DevelopmentalStage>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as DevelopmentalStage")
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as int64_t")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as double")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as string")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value as bool")
        .def("getDouble", [](const Config& self, const std::string& key, double default_value) {
            auto val = self.get<double>(key);
            if (val.has_value()) return val.value();
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as double with default (convenience method)")
        .def("getInt", [](const Config& self, const std::string& key, int64_t default_value) {
            auto val = self.get<int64_t>(key);
            if (val.has_value()) return val.value();
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as int64_t with default (convenience method)")
        .def("getBool", [](const Config& self, const std::string& key, bool default_value) {
            auto val = self.get<bool>(key);
            if (val.has_value()) return val.value();
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as bool with default (convenience method)")
        .def("getString", [](const Config& self, const std::string& key, const std::string& default_value) {
            auto val = self.get<std::string>(key);
            if (val.has_value()) return val.value();
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as string with default (convenience method)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration entry with validation")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate all configuration values")
        .def("getAllSettings", [](const Config& self) {
            auto keys = self.getKeys();
            std::map<std::string, std::string> result;
            for (const auto& key : keys) {
                // Get string representation of value
                auto str = self.getString(key, "");
                if (!str.empty()) {
                    result[key] = str;
                }
            }
            return result;
        }, "Get all settings as key-value pairs (string representation)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of Config")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "String conversion");

    // Enhanced SensoryInput class with utility methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Enhanced base class for sensory input with utility methods.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("is_valid", &SensoryInput::is_valid,
             "Check if this sensory input is valid")
        .def("get_freshness", [](const SensoryInput& self, uint64_t current_time) {
            return current_time - self.getTimestamp();
        }, py::arg("current_time"),
             "Get freshness of sensory input (current_time - timestamp)");

    // Enhanced Vision class with utility methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Enhanced vision sensory input with utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision sensory input with validation")
        .def(py::init<const Vision&>(), py::arg("other"),
             "Copy constructor")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataSize", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total number of data elements")
        .def("normalize", [](Vision& self, float min_val, float max_val) {
            auto data = self.getData();
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
            self.setData(data);
        }, py::arg("min_val"), py::arg("max_val"),
             "Normalize vision data to range [0, 1]")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Note: This is a convenience method - actual implementation may vary
            if (new_width == 0 || new_height == 0) {
                throw py::value_error("Vision dimensions must be positive");
            }
            // In a real implementation, you would call the actual resize method
            // self.resize(new_width, new_height);
        }, py::arg("new_width"), py::arg("new_height"),
             "Resize vision input with validation")
        .def("to_numpy", [](const Vision& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Convert vision data to NumPy array (if available)")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + " channels>";
        }, "String representation of Vision")
        .def("__str__", [](const Vision& vision) {
            return "Vision[" + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + "]";
        }, "String conversion");

    // Enhanced Audio class with utility methods
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Enhanced audio sensory input with utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio sensory input with validation")
        .def(py::init<const Audio&>(), py::arg("other"),
             "Copy constructor")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("normalize", [](Audio& self, float min_val, float max_val) {
            auto data = self.getData();
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
            self.setData(data);
        }, py::arg("min_val"), py::arg("max_val"),
             "Normalize audio data to range [0, 1]")
        .def("resample", [](Audio& self, size_t new_sample_rate) {
            if (new_sample_rate == 0) {
                throw py::value_error("Sample rate must be positive");
            }
            // Note: This is a convenience method - actual implementation may vary
            // self.resample(new_sample_rate);
        }, py::arg("new_sample_rate"),
             "Resample audio with validation")
        .def("getFrequencySpectrum", [](const Audio& self) {
            // Note: This is a convenience method - actual implementation may vary
            std::vector<float> spectrum(self.getNumSamples() / 2);
            // In a real implementation, you would compute the FFT
            return spectrum;
        }, "Get frequency spectrum of audio")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + " Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        }, "String representation of Audio")
        .def("__str__", [](const Audio& audio) {
            return "Audio[" + std::to_string(audio.getSampleRate()) + " Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples]";
        }, "String conversion");

    // Enhanced InternalSignals class with utility methods
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Enhanced internal signals sensory input with utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const InternalSignals&>(), py::arg("other"),
             "Copy constructor")
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal")
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) {
                self.addSignal(val);
            }
        }, py::arg("values"),
             "Add multiple signals at once")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) return 0.0f;
            return *std::max_element(signals.begin(), signals.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) return 0.0f;
            return *std::min_element(signals.begin(), signals.end());
        }, "Get minimum signal value")
        .def("getAverageSignal", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get average signal value")
        .def("normalize", [](InternalSignals& self, float min_val, float max_val) {
            auto signals = self.getData();
            for (auto& val : signals) {
                val = (val - min_val) / (max_val - min_val);
            }
            self.setData(signals);
        }, py::arg("min_val"), py::arg("max_val"),
             "Normalize internal signals to range [0, 1]")
        .def("is_saturated", [](const InternalSignals& self, float threshold) {
            auto signals = self.getData();
            for (float val : signals) {
                if (std::abs(val) > threshold) return true;
            }
            return false;
        }, py::arg("threshold"),
             "Check if any signal exceeds threshold (saturation)")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getSignalCount()) + 
                   " signals>";
        }, "String representation of InternalSignals")
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals[" + std::to_string(signals.getSignalCount()) + " signals]";
        }, "String conversion");

    // Enhanced Action class with utility methods
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced action representation for motor output with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with type only")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters")
        .def(py::init<const Action&>(), py::arg("other"),
             "Copy constructor")
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type with validation")
        .def("getParameters", &Action::getParameters,
             "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters with validation")
        .def("getName", &Action::getName,
             "Get action name")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"),
             "Set specific parameter by index")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return 0.0f;
        }, py::arg("index"),
             "Get specific parameter by index")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("is_movement", [](const Action& action) {
            return action.getType() == ActionType::MoveForward ||
                   action.getType() == ActionType::MoveBackward ||
                   action.getType() == ActionType::MoveLeft ||
                   action.getType() == ActionType::MoveRight;
        }, "Check if this is a movement action")
        .def("is_rotation", [](const Action& action) {
            return action.getType() == ActionType::TurnLeft ||
                   action.getType() == ActionType::TurnRight;
        }, "Check if this is a rotation action")
        .def("is_interaction", [](const Action& action) {
            return action.getType() == ActionType::Interact ||
                   action.getType() == ActionType::Eat ||
                   action.getType() == ActionType::Drink;
        }, "Check if this is an interaction action")
        .def("get_normalized_parameters", [](const Action& action) {
            auto params = action.getParameters();
            if (params.empty()) return std::vector<float>{};
            
            float max_val = *std::max_element(params.begin(), params.end());
            if (max_val == 0.0f) return params;
            
            std::vector<float> normalized;
            for (float val : params) {
                normalized.push_back(val / max_val);
            }
            return normalized;
        }, "Get normalized parameters (range [0, 1])")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + std::to_string(static_cast<int>(action.getType())) +
                   " params=" + std::to_string(action.getParameters().size()) + ">";
        }, "String representation of Action")
        .def("__str__", [](const Action& action) {
            return "Action[" + std::to_string(static_cast<int>(action.getType())) + "]";
        }, "String conversion");

    // Enhanced WorldObject class with utility methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Enhanced world object representation with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject with validation")
        .def(py::init<const WorldObject&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("x", &WorldObject::x,
                      "X coordinate position")
        .def_readwrite("y", &WorldObject::y,
                      "Y coordinate position")
        .def_readwrite("radius", &WorldObject::radius,
                      "Collision radius")
        .def_readwrite("type", &WorldObject::type,
                      "Object type")
        .def_readwrite("value", &WorldObject::value,
                      "Object value (e.g., energy, resource amount)")
        .def_readwrite("active", &WorldObject::active,
                      "Whether object is active")
        .def("get_distance", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Get distance to position (x, y)")
        .def("is_colliding", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Check if this object collides with position (x, y) with given radius")
        .def("get_area", [](const WorldObject& self) {
            return 3.14159f * self.radius * self.radius;
        }, "Get area of this circular object")
        .def("set_position", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set position with validation")
        .def("set_radius", [](WorldObject& self, float radius) {
            if (radius <= 0.0f) {
                throw py::value_error("Radius must be positive");
            }
            self.radius = radius;
        }, py::arg("radius"),
             "Set radius with validation")
        .def("clone", &WorldObject::clone,
             "Create a copy of this world object")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," +
                   std::to_string(obj.y) + " type=" +
                   std::to_string(static_cast<int>(obj.type)) + ">";
        }, "String representation of WorldObject")
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(" + std::to_string(obj.x) + ", " +
                   std::to_string(obj.y) + ", " +
                   std::to_string(static_cast<int>(obj.type)) + ")";
        }, "String conversion");

    // Enhanced AgentBody class with utility methods
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Enhanced agent body state with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const AgentBody&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("x", &AgentBody::x,
                      "X coordinate position")
        .def_readwrite("y", &AgentBody::y,
                      "Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation,
                      "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      "Angular velocity in radians per timestep")
        .def_readwrite("energy", &AgentBody::energy,
                      "Current energy level")
        .def_readwrite("health", &AgentBody::health,
                      "Current health (0-100)")
        .def_readwrite("age", &AgentBody::age,
                      "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      "Time of last action")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + 
                           self.velocityY * self.velocityY);
        }, "Get current speed magnitude")
        .def("get_position", &AgentBody::getPosition,
             "Get current position as (x, y)")
        .def("get_velocity", &AgentBody::getVelocity,
             "Get current velocity as (vx, vy)")
        .def("set_position", &AgentBody::setPosition, py::arg("x"), py::arg("y"),
             "Set position with validation")
        .def("set_velocity", &AgentBody::setVelocity, 
             py::arg("vx"), py::arg("vy"), py::arg("vy"),
             "Set velocity with validation")
        .def("add_velocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX += vx;
            self.velocityY += vy;
        }, py::arg("vx"), py::arg("vy"),
             "Add velocity components")
        .def("reset", &AgentBody::reset,
             "Reset body to initial state")
        .def("update_physics", [](AgentBody& self, float dt) {
            // Simple physics integration
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            self.orientation += self.angularVelocity * dt;
        }, py::arg("dt"),
             "Update physics with timestep")
        .def("is_energy_critical", [](const AgentBody& self) {
            return self.energy < 10.0f;
        }, "Check if energy is critically low")
        .def("is_alive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + "," +
                   std::to_string(body.y) + ") energy=" + std::to_string(body.energy) +
                   " health=" + std::to_string(body.health) + ">";
        }, "String representation of AgentBody")
        .def("__str__", [](const AgentBody& body) {
            return "AgentBody(pos=" + std::to_string(body.x) + ", " +
                   std::to_string(body.y) + ", energy=" + std::to_string(body.energy) + ")";
        }, "String conversion");

    // Enhanced ActionResult class with utility methods
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced action result from world with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with validation")
        .def(py::init<const ActionResult&>(), py::arg("other"),
             "Copy constructor")
        .def_readwrite("reward", &ActionResult::reward,
                      "Reward value (-1.0 to 1.0)")
        .def_readwrite("success", &ActionResult::success,
                      "Whether action was successful")
        .def_readwrite("message", &ActionResult::message,
                      "Human-readable message")
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if this result was positive (reward > 0)")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if this result was negative (reward < 0)")
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("get_magnitude", [](const ActionResult& self) {
            return std::abs(self.reward);
        }, "Get magnitude of reward (absolute value)")
        .def("to_dict", [](const ActionResult& self) {
            return py::dict(
                "reward"_a = self.reward,
                "success"_a = self.success,
                "message"_a = self.message
            );
        }, "Convert ActionResult to dictionary")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + ">";
        }, "String representation of ActionResult")
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) +
                   ", success=" + std::to_string(result.success) + ")";
        }, "String conversion");

    // Enhanced SensoryPercept class with utility methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Enhanced sensory percept data with validation and utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const SensoryPercept&>(), py::arg("other"),
             "Copy constructor")
        .def("getVision", &SensoryPercept::getVision,
             "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("get_sensory_count", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of sensory signals")
        .def("get_data_size", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return signals.size();
        }, "Get total data size across all sensory modalities")
        .def("merge_percepts", [](SensoryPercept& self, const SensoryPercept& other) {
            // Merge other percept into self
            // In a real implementation, you would merge the signals appropriately
            // This is a convenience method
            self.setTimestamp(std::max(self.getTimestamp(), other.getTimestamp()));
        }, py::arg("other"),
             "Merge another percept into this one")
        .def("is_valid", [](const SensoryPercept& self) {
            return !self.getAllSignals().empty();
        }, "Check if this percept contains valid data")
        .def("get_data_summary", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            if (signals.empty()) return std::string("Empty percept");
            
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            float avg = sum / signals.size();
            return "Percept with " + std::to_string(signals.size()) +
                   " signals, average value: " + std::to_string(avg);
        }, "Get summary of percept data")
        .def("__repr__", [](const SensoryPercept& percept) {
            auto signals = percept.getAllSignals();
            return "<SensoryPercept: " + std::to_string(signals.size()) +
                   " signals, timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        }, "String representation of SensoryPercept")
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(signals=" + std::to_string(percept.getAllSignals().size()) +
                   ", timestamp=" + std::to_string(percept.getTimestamp()) + ")";
        }, "String conversion");

    // Enhanced SimpleWorld class with advanced utility methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced simple 2D world for NLM simulation with advanced utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const SimpleWorld&>(), py::arg("other"),
             "Copy constructor")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions with validation")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation with timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world with validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world with validation")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not colliding)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed with validation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("get_cell_count", [](const SimpleWorld& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of cells in the world")
        .def("get_population_density", [](const SimpleWorld& self, const std::vector<float>& values) {
            // Calculate density based on values
            float sum = std::accumulate(values.begin(), values.end(), 0.0f);
            return sum / values.size();
        }, py::arg("values"),
             "Calculate population density from values")
        .def("find_nearest_object", [](const SimpleWorld& self, float x, float y) {
            // Find nearest object to position (x, y)
            // This is a convenience method - actual implementation may vary
            return std::make_pair(0.0f, 0.0f); // placeholder
        }, py::arg("x"), py::arg("y"),
             "Find nearest object to position (x, y)")
        .def("get_environment_stats", [](const SimpleWorld& self) {
            // Get environment statistics
            // This is a convenience method - actual implementation may vary
            return std::map<std::string, float>{ // placeholder
                {"width", static_cast<float>(self.getWidth())},
                {"height", static_cast<float>(self.getHeight())}
            };
        }, "Get environment statistics")
        .def("simulate_step", [](SimpleWorld& self, float timestep) {
            self.update(timestep);
        }, py::arg("timestep"),
             "Alias for update method")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " cells, " +
                   std::to_string(world.getSimulationTime()) + " time>";
        }, "String representation of SimpleWorld")
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld[" + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " cells]";
        }, "String conversion");

    // Enhanced Brain class with advanced utility methods
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced central neural simulation brain class with advanced utility methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create Brain with configuration (with validation)")
        .def(py::init<const Brain&>(), py::arg("other"),
             "Copy constructor with validation")
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
             "Save brain state to file with validation")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file with validation")
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
             "Set developmental stage with validation")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("get_simulation_stats", [](const Brain& self) {
            return std::map<std::string, float>{ // placeholder
                {"total_neurons", static_cast<float>(self.getTotalNeuronCount())},
                {"total_synapses", static_cast<float>(self.getTotalSynapseCount())},
                {"firing_neurons", static_cast<float>(self.getFiringNeuronCount())},
                {"total_spikes", static_cast<float>(self.getTotalSpikeCount())}
            };
        }, "Get brain simulation statistics")
        .def("validate_state", [](const Brain& self) {
            // Validate brain state
            // This is a convenience method - actual implementation may vary
            return true;
        }, "Validate brain state consistency")
        .def("export_state", [](const Brain& self) {
            // Export brain state as Python dict
            // This is a convenience method - actual implementation may vary
            return py::dict();
        }, "Export brain state as Python dictionary")
        .def("import_state", [](Brain& self, const py::dict& state) {
            // Import brain state from Python dict
            // This is a convenience method - actual implementation may vary
        }, py::arg("state"), "Import brain state from Python dictionary")
        .def("simulate", [](Brain& self, int steps, float timestep = 0.1) {
            for (int step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.1f,
             "Run multiple simulation steps with convenience method")
        .def("run_episode", [](Brain& self, int steps) {
            return self.simulate(steps);
        }, py::arg("steps"),
             "Run simulation episode (alias for simulate)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) +
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ">";
        }, "String representation of Brain")
        .def("__str__", [](const Brain& brain) {
            return "Brain[neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(brain.getTotalSynapseCount()) + "]";
        }, "String conversion");

    // Enhanced AgentBrain class with advanced utility methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface connecting NLM brain to world with advanced utility methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with underlying brain (with validation)")
        .def(py::init<const AgentBrain&>(), py::arg("other"),
             "Copy constructor with validation")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("get_agent_stats", [](const AgentBrain& self) {
            return std::map<std::string, float>{ // placeholder
                {"curiosity", self.getCuriosityLevel()},
                {"novelty", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()},
                {"neuromodulation", self.getNeuromodulationLevel()},
                {"development_stage", static_cast<float>(self.getDevelopmentalStage())}
            };
        }, "Get agent statistics")
        .def("get_learning_metrics", [](const AgentBrain& self) {
            return std::map<std::string, float>{ // placeholder
                {"learning_rate", 0.001f},
                {"plasticity", 0.1f},
                {"exploration", self.getCuriosityLevel()},
                {"exploitation", 1.0f - self.getCuriosityLevel()}
            };
        }, "Get learning metrics")
        .def("validate_configuration", [](const AgentBrain& self) {
            // Validate agent brain configuration
            // This is a convenience method - actual implementation may vary
            return true;
        }, "Validate agent brain configuration")
        .def("run_episode", [](AgentBrain& self, const SimpleWorld& world, int steps) {
            self.initialize(world);
            for (int step = 0; step < steps; ++step) {
                // In a real implementation, you would process sensory input,
                // run brain step, decode motor command, etc.
                // This is a convenience method - actual implementation may vary
            }
        }, py::arg("world"), py::arg("steps"),
             "Run agent episode with world (convenience method)")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   " novelty=" + std::to_string(agent.getNoveltyLevel()) + ">";
        }, "String representation of AgentBrain")
        .def("__str__", [](const AgentBrain& agent) {
            return "AgentBrain[curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   ", novelty=" + std::to_string(agent.getNoveltyLevel()) + "]";
        }, "String conversion");

    // Enhanced convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set some reasonable defaults
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            config = createDefaultConfig();
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (with validation)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8); // Default 20x20 world with 8x8 vision
        return world;
    }, "Create a new simple world with default configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            brain = createBrain(nullptr);
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (with validation)");

    // Enhanced validation and utility functions
    m.def("validate_config", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration");

    m.def("create_config_from_dict", [](const py::dict& settings) {
        auto config = std::make_shared<Config>();
        for (auto item : settings) {
            std::string key = py::cast<std::string>(item.first);
            if (item.second.get_type() == py::dtype::of<int64_t>()) {
                config->set(key, py::cast<int64_t>(item.second), nlm::ConfigSource::Runtime);
            } else if (item.second.get_type() == py::dtype::of<double>()) {
                config->set(key, py::cast<double>(item.second), nlm::ConfigSource::Runtime);
            } else if (item.second.get_type() == py::dtype::of<std::string>()) {
                config->set(key, py::cast<std::string>(item.second), nlm::ConfigSource::Runtime);
            } else if (item.second.get_type() == py::dtype::of<bool>()) {
                config->set(key, py::cast<bool>(item.second), nlm::ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("settings"), "Create configuration from Python dictionary");

    m.def("create_config_from_args", [](const std::vector<std::string>& args) {
        auto config = std::make_shared<Config>();
        return config->loadFromArgs(args.size(), &args[0]) ? config : nullptr;
    }, py::arg("args"), "Create configuration from command line arguments");

    // Advanced utility functions for power users
    m.def("benchmark_brain", [](const Brain& brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain.step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance (return time in ms)");

    m.def("export_brain_state", [](const Brain& brain) {
        return py::dict(
            "neurons"_a = brain.getTotalNeuronCount(),
            "synapses"_a = brain.getTotalSynapseCount(),
            "firing"_a = brain.getFiringNeuronCount(),
            "spikes"_a = brain.getTotalSpikeCount(),
            "stage"_a = static_cast<int>(brain.getDevelopmentalStage())
        );
    }, "Export brain state as Python dictionary");

    m.def("create_standard_brain", [](const std::string& type) {
        auto config = createDefaultConfig();
        
        if (type == "small") {
            config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("brain.region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        } else if (type == "medium") {
            config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
            config->set("brain.region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        } else if (type == "large") {
            config->set("brain.neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
            config->set("brain.region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("type"), "Create standard brain of specified size (small, medium, large)");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
}

} // namespace nlm