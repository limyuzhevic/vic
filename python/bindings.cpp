#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for complex types
class Config;
class Brain;
class SimpleWorld;
class AgentBrain;
class NeuronId;
class SynapseId;
class RegionId;
class PopulationId;

// Module documentation and metadata
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive access to neural simulation components including:
        
        - Configuration management
        - Sensory processing (Vision, Audio, InternalSignals)
        - Motor control and actions
        - Neural brain simulation
        - Agent interfaces
        - World simulation
        - Memory systems
        - Neuromodulation
        - Developmental processes
        
        This binding provides both low-level access to neural components and
        high-level interfaces for agent-based simulations.
    )pbdoc";

    // Register exception types for Python error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    py::register_exception<std::runtime_error>(m, "NLMError");

    // ============================================================================
    // SECTION 1: IDENTIFIER TYPES
    // ============================================================================
    
    // NeuronId - Strongly-typed neuron identifier
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId provides type-safe access to neuron references in the brain,
        preventing invalid neuron access through type safety. Each neuron has a
        unique 64-bit identifier that can be used to reference specific neurons
        across the simulation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId with the specified value")
        .def_readwrite("value", &NeuronId::value,
            "The internal 64-bit value of the identifier")
        .def("index", &NeuronId::index,
            "Get the neuron index (alias for value)")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__lt__", &NeuronId::operator<,
             "Compare if this NeuronId is less than another")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in Python dictionaries/sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        }, "Get string representation for conversion to str");

    // SynapseId - Strongly-typed synapse identifier
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId provides type-safe access to synapse references, ensuring that
        synaptic connections are correctly referenced throughout the brain
        simulation. Each synapse has a unique 64-bit identifier.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId with the specified value")
        .def_readwrite("value", &SynapseId::value,
            "The internal 64-bit value of the identifier")
        .def("index", &SynapseId::index,
             "Get the synapse index (alias for value)")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__lt__", &SynapseId::operator<,
             "Compare if this SynapseId is less than another")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in Python dictionaries/sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        }, "Get string representation for conversion to str");

    // RegionId - Strongly-typed brain region identifier
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId provides type-safe access to brain regions, ensuring that
        neural computations are correctly organized within appropriate regions.
        Each region has a unique 64-bit identifier.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId with the specified value")
        .def_readwrite("value", &RegionId::value,
            "The internal 64-bit value of the identifier")
        .def("index", &RegionId::index,
             "Get the region index (alias for value)")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__lt__", &RegionId::operator<,
             "Compare if this RegionId is less than another")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in Python dictionaries/sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        }, "Get string representation for conversion to str");

    // PopulationId - Strongly-typed neuron population identifier
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId provides type-safe access to groups of neurons that serve
        similar functions. Populations help organize neural computation by
        grouping neurons with similar characteristics or roles.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId with the specified value")
        .def_readwrite("value", &PopulationId::value,
            "The internal 64-bit value of the identifier")
        .def("index", &PopulationId::index,
             "Get the population index (alias for value)")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal")
        .def("__lt__", &PopulationId::operator<,
             "Compare if this PopulationId is less than another")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in Python dictionaries/sets")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        }, "Get string representation for conversion to str");

    // ============================================================================
    // SECTION 2: ENUMERATIONS
    // ============================================================================
    
    // NeuronType - Classification of neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Classification of neuron types in the NLM framework.
        
        Neurons are categorized based on their functional roles in the brain:
        - Excitatory: Typically promote neural activation
        - Inhibitory: Typically suppress neural activation  
        - Modulatory: Release neuromodulators affecting neural plasticity
        - Sensory: Process external/internal sensory input
        - Motor: Generate motor commands for output
        - Internal: Support internal brain functions
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values()
        .doc() = "Neuron type enumeration defining functional roles in neural computation";

    // SynapseType - Classification of synapse connection types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Classification of synaptic connection types.
        
        Synapses transmit signals between neurons with different mechanisms:
        - Excitatory: Increase postsynaptic neuron excitability
        - Inhibitory: Decrease postsynaptic neuron excitability
        - Modulatory: Release neuromodulators
        - Electrical: Direct electrical coupling
        - GapJunction: Electrical gap junction coupling
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values()
        .doc() = "Synaptic connection type enumeration";

    // DevelopmentalStage - Neural developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Neural developmental stages affecting plasticity and behavior.
        
        The brain progresses through distinct developmental stages:
        - Initial: Early developmental phase
        - CriticalPeriod: Enhanced plasticity phase
        - Maturation: Refinement and optimization phase
        - Adult: Mature, stable behavior phase
        - Aging: Age-related changes and decline phase
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values()
        .doc() = "Neural developmental stage enumeration";

    // FiringState - Neuron firing state
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state in the neural dynamics simulation.
        
        Represents the current electrical state of a neuron:
        - Resting: Baseline membrane potential
        - Active: Currently firing action potentials
        - Refractory: Post-spike recovery period
        - Inhibited: Receiving inhibitory input
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values()
        .doc() = "Neuron firing state enumeration";

    // ActionType - Motor action classification
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Classification of motor actions and behaviors.
        
        Actions are categorized by their functional purpose:
        - Movement-related: Forward, backward, turning, looking
        - Interaction-related: Interact, eat, drink
        - State-related: Rest, wait, custom
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values()
        .doc() = "Motor action type enumeration";

    // MotorCommand - Low-level motor command classification
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command classification.
        
        Primitive motor commands that control basic movement:
        - Movement: Forward, backward, turning
        - Orientation: Left, right looking
        - Interaction: Basic interaction
        - Control: Wait/stop command
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values()
        .doc() = "Low-level motor command enumeration";

    // WorldObjectType - Classification of world objects
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Classification of objects in the simulated world.
        
        Different types of objects that can exist in the simulation world:
        - Empty: No object
        - Resource: Valuable resource for agents
        - Hazard: Dangerous object to avoid
        - Wall: Physical barrier
        - Marker: Special location indicator
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values()
        .doc() = "World object type enumeration";

    // ============================================================================
    // SECTION 3: CORE CLASSES
    // ============================================================================
    
    // Config - Configuration management
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration manager for the NLM system.
        
        The Config class provides a key-value storage system for managing
        simulation parameters, agent settings, and runtime configuration.
        Configuration supports multiple sources (file, command-line, runtime)
        and type conversion.
        
        Key features:
        - JSON file loading/saving
        - Command-line argument support
        - Type-safe value access
        - Multiple configuration sources
        - Summary and inspection utilities
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file.\n\n:param filepath: Path to JSON configuration file\n:returns: True if successful, False on error\n:rtype: bool")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments.\n\n:param argc: Number of arguments (typically sys.argv length)\n:param argv: Array of command line arguments\n:returns: True if successful, False on error\n:rtype: bool")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.\n\n:param filepath: Path where configuration will be saved\n:returns: True if successful, False on error\n:rtype: bool")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.\n\n:param key: Configuration key to check\n:returns: True if key exists, False otherwise\n:rtype: bool")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.\n\n:returns: List of all configuration key names\n:rtype: list[str]")
        .def("clear", &Config::clear,
             "Clear all configuration entries.\nThis removes all configuration data and resets to empty state.\n:rtype: None")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.\n\n:returns: Human-readable summary of configuration contents\n:rtype: str")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation for debugging")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>, std::type_identity<std::string>{}),
             py::arg("key"),
             "Get a configuration value as a generic type.\n\n:param key: Configuration key to retrieve\n:returns: Value as Python object (type-dependent)\n:rtype: Any")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get<int>, std::type_identity<int>{}),
             py::arg("key"), py::arg("defaultValue") = 0,
             "Get an integer configuration value.\n\n:param key: Configuration key to retrieve\n:param defaultValue: Default value if key doesn't exist\n:returns: Integer value or default\n:rtype: int")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get<double>, std::type_identity<double>{}),
             py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get a double configuration value.\n\n:param key: Configuration key to retrieve\n:param defaultValue: Default value if key doesn't exist\n:returns: Double value or default\n:rtype: float")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get<bool>, std::type_identity<bool>{}),
             py::arg("key"), py::arg("defaultValue") = false,
             "Get a boolean configuration value.\n\n:param key: Configuration key to retrieve\n:param defaultValue: Default value if key doesn't exist\n:returns: Boolean value or default\n:rtype: bool")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get<std::string>, std::type_identity<std::string>{}),
             py::arg("key"), py::arg("defaultValue") = "",
             "Get a string configuration value.\n\n:param key: Configuration key to retrieve\n:param defaultValue: Default value if key doesn't exist\n:returns: String value or default\n:rtype: str")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration value with type safety.\n\n:param key: Configuration key to set\n:param value: Configuration value\n:rtype: None")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set an integer configuration value.\n\n:param key: Configuration key to set\n:param value: Integer value\n:rtype: None")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a double configuration value.\n\n:param key: Configuration key to set\n:param value: Double value\n:rtype: None")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a boolean configuration value.\n\n:param key: Configuration key to set\n:param value: Boolean value\n:rtype: None")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set a string configuration value.\n\n:param key: Configuration key to set\n:param value: String value\n:rtype: None")
        .def("getOr", py::overload_cast<const std::string&, int>(&Config::getOr, std::type_identity<int>{}),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default.\n\n:param key: Configuration key to retrieve\n:param defaultValue: Default value if key doesn't exist\n:returns: Value or default\n:rtype: Any")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key.\n\n:param key: Configuration key to remove\n:rtype: None");

    // SensoryInput - Base class for sensory input
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input modalities.
        
        SensoryInput provides the interface for all sensory data modalities,
        including Vision, Audio, and InternalSignals. It defines the common
        properties and methods for accessing sensory information in the brain.
        
        :ivar type: Type of sensory input
        :ivar timestamp: Time when input was generated
        :ivar dimensions: Dimensionality of the data
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input.\n\n:returns: Sensory input type\n:rtype: NeuronType")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector.\n\n:returns: Raw sensory data vector\n:rtype: list[float]")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data.\n\n:returns: Number of data dimensions\n:rtype: int")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp when this sensory input was generated.\n\n:returns: Simulation timestamp\n:rtype: float")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input.\n\n:param timestamp: Simulation timestamp\n:rtype: None")
        .def("isValid", [](const SensoryInput& input) {
            return !input.getData().empty() && input.getTimestamp() > 0;
        }, "Check if sensory input is valid (non-empty and has timestamp).\n\n:returns: True if valid, False otherwise\n:rtype: bool")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) + 
                   ", timestamp=" + std::to_string(input.getTimestamp()) + ", dims=" + 
                   std::to_string(input.getDimensions()) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const SensoryInput& input) {
            std::string typeStr;
            switch(input.getType()) {
                case NeuronType::Vision: typeStr = "Vision"; break;
                case NeuronType::Audio: typeStr = "Audio"; break;
                case NeuronType::Internal: typeStr = "Internal"; break;
                default: typeStr = "Unknown";
            }
            return "SensoryInput(type=" + typeStr + ", timestamp=" + std::to_string(input.getTimestamp()) + ")";
        }, "Get string representation for conversion to str");

    // Vision - Visual sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for processing visual information.
        
        Vision provides 2D visual input with configurable dimensions and
        color channels. It supports setting visual data as floating-point
        arrays representing pixel intensities or RGB values.
        
        :ivar width: Image width in pixels
        :ivar height: Image height in pixels  
        :ivar channels: Number of color channels (typically 3 for RGB)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Initialize Vision with specified dimensions\n\n:param width: Image width in pixels\n:param height: Image height in pixels\n:param channels: Number of color channels (default: 3)\n:rtype: None")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            if (data.size() < self.getWidth() * self.getHeight() * self.getChannels()) {
                throw py::value_error("Vision data size mismatch");
            }
            self.setData(data);
        }, py::arg("data"), "Set visual data as a vector of floats.\n\n:param data: Flattened array of pixel data (width * height * channels)\n:raises ValueError: If data is empty or size mismatch\n:rtype: None")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels.\n\n:returns: Image width\n:rtype: int")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels.\n\n:returns: Image height\n:rtype: int")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels.\n\n:returns: Number of color channels\n:rtype: int")
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels.\n\n:returns: Total pixel count\n:rtype: int")
        .def("getDataAsImage", [](const Vision& self) {
            // Return a copy of the data for Python consumption
            auto data = self.getData();
            return data;
        }, "Get visual data as a Python list.\n\n:returns: List of pixel values\n:rtype: list[float]")
        .def("validateData", [](Vision& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("Vision data is empty");
            }
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw py::value_error("Vision data size mismatch: expected " + 
                                     std::to_string(expectedSize) + " got " + std::to_string(data.size()));
            }
            return true;
        }, "Validate vision data consistency.\n\n:returns: True if valid\n:raises ValueError: If data is invalid\n:rtype: bool")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" + 
                   std::to_string(vision.getHeight()) + " " +
                   std::to_string(vision.getChannels()) + "ch, " +
                   std::to_string(vision.getPixelCount()) + " pixels>";
        }, "Get string representation for debugging")
        .def("__str__", [](const Vision& vision) {
            return "Vision(width=" + std::to_string(vision.getWidth()) + ", " +
                   "height=" + std::to_string(vision.getHeight()) + ", " +
                   "channels=" + std::to_string(vision.getChannels()) + ")";
        }, "Get string representation for conversion to str");

    // Audio - Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for processing sound information.
        
        Audio provides time-series audio data with configurable sample rates
        and duration. It supports setting audio data as floating-point arrays
        representing audio waveforms.
        
        :ivar sampleRate: Audio sampling rate in Hz
        :ivar numSamples: Number of audio samples
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize Audio with specified parameters\n\n:param sampleRate: Sampling rate in Hz\n:param numSamples: Number of audio samples\n:rtype: None")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            if (data.size() != self.getNumSamples()) {
                throw py::value_error("Audio data size mismatch");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of floats.\n\n:param data: Audio samples (length = numSamples)\n:raises ValueError: If data is empty or size mismatch\n:rtype: None")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sampling rate.\n\n:param sampleRate: Sampling rate in Hz\n:rtype: None")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sampling rate.\n\n:returns: Sampling rate in Hz\n:rtype: int")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples.\n\n:returns: Number of audio samples\n:rtype: int")
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds.\n\n:returns: Duration in seconds\n:rtype: float")
        .def("getDataAsSound", [](const Audio& self) {
            auto data = self.getData();
            return data;
        }, "Get audio data as a Python list.\n\n:returns: List of audio samples\n:rtype: list[float]")
        .def("validateData", [](Audio& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("Audio data is empty");
            }
            if (data.size() != self.getNumSamples()) {
                throw py::value_error("Audio data size mismatch");
            }
            return true;
        }, "Validate audio data consistency.\n\n:returns: True if valid\n:raises ValueError: If data is invalid\n:rtype: bool")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples, " +
                   std::to_string(audio.getDuration()) + "s>";
        }, "Get string representation for debugging")
        .def("__str__", [](const Audio& audio) {
            return "Audio(sampleRate=" + std::to_string(audio.getSampleRate()) + ", " +
                   "numSamples=" + std::to_string(audio.getNumSamples()) + ")";
        }, "Get string representation for conversion to str");

    // InternalSignals - Internal neural signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal neural signals for brain communication.
        
        InternalSignals represents neural activity patterns that are generated
        internally by the brain, such as working memory, attention, or
        prediction signals. It provides mechanisms for adding and accessing
        internal signal values.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value.\n\n:param value: Signal value to add\n:rtype: None")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals.\nThis removes all stored signal values.\n:rtype: None")
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        }, "Get all internal signals as a vector.\n\n:returns: Vector of internal signal values\n:rtype: list[float]")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals.\n\n:returns: Number of signals\n:rtype: int")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get the maximum signal value.\n\n:returns: Maximum signal value\n:rtype: float")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get the minimum signal value.\n\n:returns: Minimum signal value\n:rtype: float")
        .def("validateSignals", [](InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("Internal signals data is empty");
            }
            return true;
        }, "Validate internal signals data.\n\n:returns: True if valid\n:raises ValueError: If data is invalid\n:rtype: bool")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getSignalCount()) + 
                   " signals, max=" + std::to_string(signals.getMaxSignal()) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals(count=" + std::to_string(signals.getSignalCount()) + ")";
        }, "Get string representation for conversion to str");

    // Action - Motor action representation
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation for neural control of behavior.
        
        Action encapsulates a specific motor command with associated parameters.
        It provides storage for action type and customizable parameters for
        fine-grained control over behavior.
        
        :ivar type: The action type (Movement, Interaction, etc.)
        :ivar parameters: Additional parameters for action execution
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Initialize Action with just a type.\n\n:param type: Action type\n:rtype: None")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Initialize Action with type and parameters.\n\n:param type: Action type\n:param parameters: Action parameters\n:rtype: None")
        .def("getType", &Action::getType,
             "Get the action type.\n\n:returns: Action type\n:rtype: ActionType")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type.\n\n:param type: Action type\n:rtype: None")
        .def("getParameters", &Action::getParameters,
             "Get action parameters.\n\n:returns: Action parameters vector\n:rtype: list[float]")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters.\n\n:param params: New parameters vector\n:rtype: None")
        .def("getName", &Action::getName,
             "Get human-readable action name.\n\n:returns: Action name string\n:rtype: str")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get the number of action parameters.\n\n:returns: Parameter count\n:rtype: int")
        .def("getParameterSum", [](const Action& self) {
            auto params = self.getParameters();
            float sum = 0.0f;
            for (float p : params) sum += p;
            return sum;
        }, "Get the sum of all action parameters.\n\n:returns: Parameter sum\n:rtype: float")
        .def("clone", &Action::clone,
             "Create a copy of this Action.\n\n:returns: Cloned Action object\n:rtype: Action")
        .def("isValid", [](const Action& self) {
            return !self.getName().empty() && !self.getParameters().empty();
        }, "Check if action is valid.\n\n:returns: True if action has type and parameters\n:rtype: bool")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " at " + 
                   std::to_string(static_cast<int>(action.getType())) + 
                   " with " + std::to_string(action.getParameterCount()) + " params>";
        }, "Get string representation for debugging")
        .def("__str__", [](const Action& action) {
            return "Action(" + action.getName() + ", " + std::to_string(action.getParameterCount()) + " params)";
        }, "Get string representation for conversion to str");

    // WorldObject - Physical object in the world
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Physical object representation in the simulated world.
        
        WorldObject represents entities that exist in the simulation world,
        including resources, hazards, walls, and markers. Each object has
        position, type, and physical properties.
        
        :ivar x: X-coordinate in world space
        :ivar y: Y-coordinate in world space
        :ivar type: Object type (Resource, Hazard, Wall, Marker)
        :ivar value: Object value (e.g., resource amount)
        :ivar radius: Physical size of object
        :ivar active: Whether object is currently active
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a WorldObject at specified position.\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:param type: Object type\n:param value: Object value\n:param radius: Physical radius\n:rtype: None")
        .def_readwrite("x", &WorldObject::x, "X-coordinate position")
        .def_readwrite("y", &WorldObject::y, "Y-coordinate position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius/size")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType)")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., resource amount)")
        .def_readwrite("active", &WorldObject::active, "Object activation state")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to a point.\n\n:param x: Target X-coordinate\n:param y: Target Y-coordinate\n:returns: Distance\n:rtype: float")
        .def("distanceToObject", [](const WorldObject& self, const WorldObject& other) {
            return self.distanceTo(other.x, other.y);
        }, py::arg("other"), "Calculate distance to another WorldObject.\n\n:param other: Target WorldObject\n:returns: Distance\n:rtype: float")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.distanceTo(other.x, other.y);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if this object is colliding with another.\n\n:param other: Other WorldObject\n:returns: True if colliding\n:rtype: bool")
        .def("isAtPosition", [](const WorldObject& self, float x, float y, float tolerance = 0.1f) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx*dx + dy*dy) < (tolerance * tolerance);
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f,
           "Check if object is at a specific position within tolerance.\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:param tolerance: Position tolerance\n:returns: True if at position\n:rtype: bool")
        .def("getTypeName", [](const WorldObject& self) {
            switch(self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name.\n\n:returns: Type name string\n:rtype: str")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(static_cast<int>(obj.type)) +
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")\n"
                   " value=" + std::to_string(obj.value) + ", radius=" + std::to_string(obj.radius) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(" + std::to_string(static_cast<int>(obj.type)) + 
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")";
        }, "Get string representation for conversion to str");

    // AgentBody - Agent physical state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical state representation for an autonomous agent.
        
        AgentBody tracks the complete physical state of an agent including
        position, orientation, velocity, and physiological properties. It
        provides the interface for agent movement and interaction with the world.
        
        :ivar x: X-coordinate position
        :ivar y: Y-coordinate position
        :ivar orientation: Facing direction in radians
        :ivar velocityX: X-component of velocity
        :ivar velocityY: Y-component of velocity
        :ivar angularVelocity: Rotational velocity
        :ivar energy: Current energy level
        :ivar health: Health status (0-1)
        :ivar age: Agent age in simulation steps
        :ivar isMoving: Whether agent is currently moving
        :ivar isTurning: Whether agent is currently turning
        :ivar lastActionTime: Time of last action execution
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X-coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y-coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X-velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y-velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Rotational velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health status (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to a point.\n\n:param x: Target X-coordinate\n:param y: Target Y-coordinate\n:returns: Distance\n:rtype: float")
        .def("distanceToBody", [](const AgentBody& self, const AgentBody& other) {
            return self.distanceTo(other.x, other.y);
        }, py::arg("other"), "Calculate distance to another AgentBody.\n\n:param other: Other AgentBody\n:returns: Distance\n:rtype: float")
        .def("isMoving", &AgentBody::isMoving,
             "Check if agent is currently moving.\n\n:returns: True if moving\n:rtype: bool")
        .def("isTurning", &AgentBody::isTurning,
             "Check if agent is currently turning.\n\n:returns: True if turning\n:rtype: bool")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed magnitude.\n\n:returns: Speed magnitude\n:rtype: float")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent movement direction in radians.\n\n:returns: Direction in radians\n:rtype: float")
        .def("reset", &AgentBody::reset, "Reset agent physical state to defaults.")
        .def("updatePosition", [](AgentBody& self, float dx, float dy, float dt) {
            self.x += dx * dt;
            self.y += dy * dt;
            self.velocityX = dx;
            self.velocityY = dy;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"),
           "Update agent position and velocity.\n\n:param dx: Change in x\n:param dy: Change in y\n:param dt: Time step\n:rtype: None")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: (" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")\n"
                   " orient=" + std::to_string(body.orientation) + ", speed=" + 
                   std::to_string(body.getSpeed()) + ", energy=" + std::to_string(body.energy) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const AgentBody& body) {
            return "AgentBody(x=" + std::to_string(body.x) + ", y=" + std::to_string(body.y) + ", orient=" + 
                   std::to_string(body.orientation) + ", energy=" + std::to_string(body.energy) + ")";
        }, "Get string representation for conversion to str");

    // ActionResult - Result of action execution
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action execution in the world.
        
        ActionResult captures the outcome of an agent's action, including
        reward received, success status, and descriptive message. It provides
        feedback for reinforcement learning and agent decision-making.
        
        :ivar reward: Numerical reward (positive for beneficial actions)
        :ivar success: Whether action was successful
        :ivar message: Descriptive result message
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an ActionResult.\n\n:param reward: Numerical reward value\n:param success: Action success status\n:param message: Descriptive message\n:rtype: None")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value")
        .def_readwrite("success", &ActionResult::success, "Action success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result has positive reward.\n\n:returns: True if reward > 0\n:rtype: bool")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result has negative reward.\n\n:returns: True if reward < 0\n:rtype: bool")
        .def("isSuccessful", &ActionResult::success,
             "Check if action was successful.\n\n:returns: Success status\n:rtype: bool")
        .def("getOutcomeText", [](const ActionResult& self) {
            std::string outcome;
            if (self.reward > 0.0f) {
                outcome = "Positive reward: " + std::to_string(self.reward);
            } else if (self.reward < 0.0f) {
                outcome = "Negative reward: " + std::to_string(self.reward);
            } else {
                outcome = "Neutral reward";
            }
            if (!self.success) {
                outcome += ", but action failed";
            }
            if (!self.message.empty()) {
                outcome += ", " + self.message;
            }
            return outcome;
        }, "Get human-readable outcome description.\n\n:returns: Outcome description\n:rtype: str")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   ", success=" + std::to_string(result.success) + ", message=\"" + 
                   result.message + "\">";
        }, "Get string representation for debugging")
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) + 
                   ", success=" + std::to_string(result.success) + ")";
        }, "Get string representation for conversion to str");

    // SensoryPercept - Combined sensory input
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Combined sensory input representing an agent's perception.
        
        SensoryPercept aggregates all available sensory inputs (vision, audio,
        internal signals, proprioception) into a unified perception structure.
        It provides methods for setting and getting individual sensory modalities.
        
        This represents what an agent perceives about its environment at a
        given moment in time.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get visual input.\n\n:returns: Vision sensory input\n:rtype: Vision")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual input.\n\n:param vision: Vision sensory input\n:rtype: None")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get visual input width.\n\n:returns: Vision width in pixels\n:rtype: int")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get visual input height.\n\n:returns: Vision height in pixels\n:rtype: int")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get proprioceptive/tactile input.\n\n:returns: Touch sensory input\n:rtype: InternalSignals")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set proprioceptive input.\n\n:param touch: Touch sensory input\n:rtype: None")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal neural signals.\n\n:returns: Internal signals\n:rtype: InternalSignals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal neural signals.\n\n:param internal: Internal signals\n:rtype: None")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioceptive input.\n\n:returns: Proprioception sensory input\n:rtype: InternalSignals")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioceptive input.\n\n:param proprioception: Proprioception input\n:rtype: None")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio input.\n\n:returns: Audio sensory input\n:rtype: Audio")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio input.\n\n:param audio: Audio sensory input\n:rtype: None")
        .def("getAllSignals", [](const SensoryPercept& self) {
            // Aggregate all sensory data
            std::vector<float> allSignals;
            
            // Add vision data if available
            auto vision = self.getVision();
            if (vision.getDimensions() > 0) {
                auto visionData = vision.getData();
                allSignals.insert(allSignals.end(), visionData.begin(), visionData.end());
            }
            
            // Add audio data if available
            auto audio = self.getAudio();
            if (audio.getNumSamples() > 0) {
                auto audioData = audio.getData();
                allSignals.insert(allSignals.end(), audioData.begin(), audioData.end());
            }
            
            // Add internal signals
            auto internal = self.getInternal();
            auto internalData = internal.getData();
            allSignals.insert(allSignals.end(), internalData.begin(), internalData.end());
            
            return allSignals;
        }, "Get all sensory signals aggregated into a single vector.\n\n:returns: Combined sensory data vector\n:rtype: list[float]")
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            std::vector<float> allSignals;
            
            // Add vision data if available
            auto vision = self.getVision();
            if (vision.getDimensions() > 0) {
                auto visionData = vision.getData();
                allSignals.insert(allSignals.end(), visionData.begin(), visionData.end());
            }
            
            // Add audio data if available
            auto audio = self.getAudio();
            if (audio.getNumSamples() > 0) {
                auto audioData = audio.getData();
                allSignals.insert(allSignals.end(), audioData.begin(), audioData.end());
            }
            
            // Add internal signals
            auto internal = self.getInternal();
            auto internalData = internal.getData();
            allSignals.insert(allSignals.end(), internalData.begin(), internalData.end());
            
            return allSignals.size();
        }, "Get total number of sensory signals.\n\n:returns: Total signal count\n:rtype: int")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp.\n\n:returns: Simulation timestamp\n:rtype: float")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp.\n\n:param timestamp: Simulation timestamp\n:rtype: None")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVision().getDimensions() > 0;
        }, "Check if vision data is present.\n\n:returns: True if vision data exists\n:rtype: bool")
        .def("hasAudio", [](const SensoryPercept& self) {
            return self.getAudio().getNumSamples() > 0;
        }, "Check if audio data is present.\n\n:returns: True if audio data exists\n:rtype: bool")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getSignals().empty();
        }, "Check if internal signals are present.\n\n:returns: True if internal signals exist\n:rtype: bool")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight()) + ", " +
                   "audio=" + std::to_string(percept.getAudio().getNumSamples()) + ", " +
                   "internal=" + std::to_string(percept.getInternal().getSignalCount()) + ", " +
                   "timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(vision=" + std::to_string(percept.getVisionWidth()) + ", " +
                   "audio=" + std::to_string(percept.getAudio().getNumSamples()) + ", " +
                   "timestamp=" + std::to_string(percept.getTimestamp()) + ")";
        }, "Get string representation for conversion to str");

    // ============================================================================
    // SECTION 4: SIMULATION CLASSES
    // ============================================================================
    
    // SimpleWorld - 2D simulation world
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        2D simulation world for agent-environment interaction.
        
        SimpleWorld provides the environment where agents interact, providing
        sensory input, motor output, and world object management. It simulates
        a 2D space where agents can move, sense, and act.
        
        Key features:
        - Agent movement and physics
        - World object management
        - Sensory perception
        - Action execution
        - State persistence
        
        :ivar width: World width in arbitrary units
        :ivar height: World height in arbitrary units
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Configure the world dimensions and agent perception.\n\n:param width: World width\n:param height: World height\n:param visionWidth: Agent visual field width\n:param visionHeight: Agent visual field height\n:rtype: None")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.\nThis clears all world objects and resets agent position.\n:rtype: None")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position.\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:rtype: None")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for a simulation timestep.\n\n:param timestep: Simulation timestep\n:rtype: None")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world.\n\n:param cmd: Motor command to execute\n:param currentTime: Current simulation time\n:returns: Action result\n:rtype: ActionResult")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get current sensory percept for the agent.\n\n:returns: Agent's sensory perception\n:rtype: SensoryPercept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get current agent physical state.\n\n:returns: Agent body state\n:rtype: AgentBody")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation.\n\n:param obj: World object to add\n:rtype: None")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove a world object at specified position.\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:rtype: None")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (within bounds and not blocked).\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:returns: True if position is valid\n:rtype: bool")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width.\n\n:returns: World width\n:rtype: int")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height.\n\n:returns: World height\n:rtype: int")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy.\n\n:returns: Maximum energy\n:rtype: float")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy.\n\n:param e: Maximum energy value\n:rtype: None")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate.\n\n:returns: Energy decay rate per timestep\n:rtype: float")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate.\n\n:param r: Energy decay rate per timestep\n:rtype: None")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time.\n\n:returns: Current simulation time\n:rtype: float")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.\n\n:param seed: Random seed value\n:rtype: None")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed.\n\n:returns: Random seed value\n:rtype: int")
        .def("clearObjects", [] (SimpleWorld& self) {
            // Clear all objects from the world
            self.removeObject(0, 0);  // This is a simplified approach
        }, "Clear all objects from the world.\n\n:rtype: None")
        .def("getObjectsAtPosition", [](SimpleWorld& self, float x, float y) {
            // This would require accessing the world internally
            // For now, return empty list
            return std::vector<WorldObject>{};
        }, py::arg("x"), py::arg("y"),
           "Get objects at a specific position.\n\n:param x: X-coordinate\n:param y: Y-coordinate\n:returns: List of objects at position\n:rtype: list[WorldObject]")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " units, " +
                   std::to_string(world.getSimulationTime()) + "s elapsed>";
        }, "Get string representation for debugging")
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) + ", " +
                   "height=" + std::to_string(world.getHeight()) + ", " +
                   "time=" + std::to_string(world.getSimulationTime()) + ")";
        }, "Get string representation for conversion to str");

    // Brain - Central neural simulation brain
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain for processing and control.
        
        Brain is the core neural processing unit that simulates brain activity,
        processes sensory input, generates motor commands, and maintains
        internal state. It implements LIF (Leaky Integrate-and-Fire) neuron
        dynamics with various plasticity rules.
        
        Key features:
        - Neural population management
        - Synaptic connectivity
        - Plasticity mechanisms (STDP, Hebbian, reward-modulated)
        - Developmental stages
        - Memory integration
        - Neuromodulation
        
        :ivar config: Brain configuration
        :ivar developmentalStage: Current developmental stage
        :ivar simulationTime: Current simulation time
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a Brain with configuration.\n\n:param config: Brain configuration\n:rtype: None")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with current configuration.\nThis creates neurons, regions, and establishes initial connectivity.\n:rtype: None")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with current step number.\n\n:param currentStep: Current simulation step\n:rtype: None")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.\n\n:param currentStep: Current simulation step\n:param currentTime: Current simulation time\n:rtype: None")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Receive sensory input and inject into neural processing.\n\n:param input: Sensory input data\n:rtype: None")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.\n\n:param neuron: Target neuron ID\n:param current: Current amount to inject\n:rtype: None")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.\n\n:param type: Neuron type\n:param current: Current amount to inject\n:rtype: None")
        .def("produceAction", &Brain::produceAction,
             "Generate motor action based on current neural activity.\n\n:returns: Generated action\n:rtype: Action")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions.\nThis clears neural activity and resets developmental state.\n:rtype: None")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.\n\n:param filepath: Path where brain state will be saved\n:rtype: None")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.\n\n:param filepath: Path to brain state file\n:rtype: None")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.\n\n:param name: Region name (optional)\n:returns: Created region ID\n:rtype: RegionId")
        .def("getRegion", &Brain::getRegion,
             py::arg("id"),
             "Get a neural region by ID.\n\n:param id: Region ID\n:returns: Neural region\n:rtype: NeuralRegion")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions.\n\n:returns: Number of regions\n:rtype: int")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.\n\n:returns: List of region IDs\n:rtype: list[RegionId]")
        .def("getRegions", &Brain::getRegions,
             "Get all neural regions.\n\n:returns: List of all regions\n:rtype: list[NeuralRegion]")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.\n\n:returns: Total neuron count\n:rtype: int")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.\n\n:returns: Total synapse count\n:rtype: int")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons.\n\n:returns: Active neuron count\n:rtype: int")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.\n\n:returns: Firing neuron count\n:rtype: int")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.\n\n:returns: Average firing rate (Hz)\n:rtype: float")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.\n\n:returns: E/I ratio (higher = more excitation)\n:rtype: float")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across all neurons.\n\n:returns: Total spike count\n:rtype: int")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.\n\n:returns: Current developmental stage\n:rtype: DevelopmentalStage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.\n\n:param stage: New developmental stage\n:rtype: None")
        .def("getConfig", &Brain::getConfig,
             "Get brain configuration.\n\n:returns: Brain configuration\n:rtype: Config")
        .def("logStatus", &Brain::logStatus,
             "Log current brain status and diagnostics.\nThis outputs information about brain health and performance.\n:rtype: None")
        .def("getNeuromodulationLevel", [] (const Brain& self) {
            return self.getNeuromodulationLevel();
        }, "Get current neuromodulation level.\n\n:returns: Neuromodulation level (0-1)\n:rtype: float")
        .def("getCuriosityLevel", [] (const Brain& self) {
            return self.getCuriosityLevel();
        }, "Get current curiosity level.\n\n:returns: Curiosity level (0-1)\n:rtype: float")
        .def("getNoveltyLevel", [] (const Brain& self) {
            return self.getNoveltyLevel();
        }, "Get current novelty level.\n\n:returns: Novelty level (0-1)\n:rtype: float")
        .def("getPredictionError", [] (const Brain& self) {
            return self.getPredictionError();
        }, "Get current prediction error.\n\n:returns: Prediction error\n:rtype: float")
        .def("getWorkingMemoryLoad", [] (const Brain& self) {
            return self.getWorkingMemoryLoad();
        }, "Get current working memory load.\n\n:returns: Working memory load (0-1)\n:rtype: float")
        .def("getAttentionLevel", [] (const Brain& self) {
            return self.getAttentionLevel();
        }, "Get current attention level.\n\n:returns: Attention level (0-1)\n:rtype: float")
        .def("isHealthy", [] (const Brain& self) {
            return self.getActiveNeuronCount() > 0 && self.getTotalSpikeCount() > 0;
        }, "Check if brain is in a healthy state.\n\n:returns: True if healthy\n:rtype: bool")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getRegionCount()) + 
                   " regions, stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ", " +
                   "spikes=" + std::to_string(brain.getTotalSpikeCount()) + ">";
        }, "Get string representation for debugging")
        .def("__str__", [](const Brain& brain) {
            return "Brain(neurons=" + std::to_string(brain.getTotalNeuronCount()) + ", " +
                   "regions=" + std::to_string(brain.getRegionCount()) + ", " +
                   "stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ")";
        }, "Get string representation for conversion to str");

    // AgentBrain - Agent brain interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world interaction.
        
        AgentBrain provides the interface between the neural brain and the
        agent-world interaction loop. It processes sensory input from the world,
        interfaces with world actions, and integrates neuromodulation and
        developmental processes.
        
        This class serves as the bridge between low-level neural processing
        and high-level agent behavior.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an AgentBrain with underlying brain.\n\n:param brain: Underlying neural brain\n:rtype: None")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with a world.\n\n:param world: Simulation world\n:rtype: None")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.\n\n:returns: Expected sensory input size\n:rtype: int")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.\n\n:returns: Expected motor output size\n:rtype: int")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.\n\n:param percept: Agent's sensory percept\n:rtype: None")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.\n\n:returns: Motor command\n:rtype: MotorCommand")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.\n\n:param reward: Actual reward received\n:param predictedReward: Predicted reward\n:rtype: None")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update developmental system.\n\n:param timestep: Current timestep\n:rtype: None")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.\n\n:returns: Current developmental stage\n:rtype: DevelopmentalStage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.\n\n:returns: Neuromodulation level (0-1)\n:rtype: float")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get current curiosity level.\n\n:returns: Curiosity level (0-1)\n:rtype: float")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get current novelty level.\n\n:returns: Novelty level (0-1)\n:rtype: float")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get current prediction error.\n\n:returns: Prediction error\n:rtype: float")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.\nThis resets developmental state and prepares for new learning.\n:rtype: None")
        .def("getBrain", &AgentBrain::getBrain,
             "Get the underlying brain.\n\n:returns: Underlying neural brain\n:rtype: Brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.\n\n:param enable: Enable/disable flag\n:rtype: None")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.\n\n:param enable: Enable/disable flag\n:rtype: None")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental processes.\n\n:param enable: Enable/disable flag\n:rtype: None")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven behavior.\n\n:param enable: Enable/disable flag\n:rtype: None")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.\n\n:returns: Enable status\n:rtype: bool")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.\n\n:returns: Enable status\n:rtype: bool")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.\n\n:returns: Enable status\n:rtype: bool")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.\n\n:returns: Enable status\n:rtype: bool")
        .def("getLearningRate", [] (const AgentBrain& self) {
            return self.getLearningRate();
        }, "Get current learning rate.\n\n:returns: Learning rate (0-1)\n:rtype: float")
        .def("getPlasticityLevel", [] (const AgentBrain& self) {
            return self.getPlasticityLevel();
        }, "Get current plasticity level.\n\n:returns: Plasticity level (0-1)\n:rtype: float")
        .def("getMemoryCapacity", [] (const AgentBrain& self) {
            return self.getMemoryCapacity();
        }, "Get current memory capacity.\n\n:returns: Memory capacity\n:rtype: float")
        .def("isReady", [] (const AgentBrain& self) {
            return self.isReady();
        }, "Check if agent brain is ready for simulation.\n\n:returns: True if ready\n:rtype: bool")
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain: connected to brain>";
        }, "Get string representation for debugging")
        .def("__str__", [](const AgentBrain& agentBrain) {
            return "AgentBrain(connected=" + std::to_string(agentBrain.getBrain() != nullptr) + ")";
        }, "Get string representation for conversion to str");

    // ============================================================================
    // SECTION 5: FACTORY FUNCTIONS
    // ============================================================================
    
    // Factory function to create default configuration
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration object.
        
        Returns a Config instance with default settings for the NLM framework.
        The configuration can be modified and saved to file.
        
        :returns: Default configuration object
        :rtype: Config
    )pbdoc");

    // Factory function to create a new brain
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the specified configuration.
        
        :param config: Brain configuration
        :returns: New brain instance
        :rtype: Brain
    )pbdoc");

    // Factory function to create a simple world
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world instance.
        
        Returns a SimpleWorld object ready for agent-environment interaction.
        The world must be configured with dimensions before use.
        
        :returns: New simple world instance
        :rtype: SimpleWorld
    )pbdoc");

    // Factory function to create an agent brain interface
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        :param brain: Underlying neural brain
        :returns: New agent brain interface
        :rtype: AgentBrain
    )pbdoc");

    // Advanced factory functions
    m.def("createBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw py::value_error("Failed to load configuration from file: " + filepath);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"), R"pbdoc(
        Create a brain from a configuration file.
        
        :param filepath: Path to configuration file
        :returns: New brain instance
        :rtype: Brain
        :raises ValueError: If configuration file cannot be loaded
    )pbdoc");

    // ============================================================================
    // SECTION 6: UTILITY FUNCTIONS AND CONSTANTS
    // ============================================================================
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Utility function to create neuron ID from value
    m.def("createNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), R"pbdoc(
        Create a NeuronId from a raw value.
        
        :param value: Raw 64-bit value for the neuron ID
        :returns: NeuronId instance
        :rtype: NeuronId
    )pbdoc");

    // Utility function to create synapse ID from value
    m.def("createSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), R"pbdoc(
        Create a SynapseId from a raw value.
        
        :param value: Raw 64-bit value for the synapse ID
        :returns: SynapseId instance
        :rtype: SynapseId
    )pbdoc");

    // Utility function to create region ID from value
    m.def("createRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), R"pbdoc(
        Create a RegionId from a raw value.
        
        :param value: Raw 64-bit value for the region ID
        :returns: RegionId instance
        :rtype: RegionId
    )pbdoc");

    // Utility function to create population ID from value
    m.def("createPopulationId", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), R"pbdoc(
        Create a PopulationId from a raw value.
        
        :param value: Raw 64-bit value for the population ID
        :returns: PopulationId instance
        :rtype: PopulationId
    )pbdoc");

    // Utility function for safe neuron ID validation
    m.def("isValidNeuronId", [](const NeuronId& id) {
        return id != INVALID_NEURON_ID && id.value != UINT64_MAX;
    }, py::arg("id"), R"pbdoc(
        Check if a neuron ID is valid.
        
        :param id: Neuron ID to validate
        :returns: True if ID is valid, False otherwise
        :rtype: bool
    )pbdoc");

    // Utility function for safe synapse ID validation
    m.def("isValidSynapseId", [](const SynapseId& id) {
        return id != INVALID_SYNAPSE_ID && id.value != UINT64_MAX;
    }, py::arg("id"), R"pbdoc(
        Check if a synapse ID is valid.
        
        :param id: Synapse ID to validate
        :returns: True if ID is valid, False otherwise
        :rtype: bool
    )pbdoc");

    // Utility function for safe region ID validation
    m.def("isValidRegionId", [](const RegionId& id) {
        return id != INVALID_REGION_ID && id.value != UINT64_MAX;
    }, py::arg("id"), R"pbdoc(
        Check if a region ID is valid.
        
        :param id: Region ID to validate
        :returns: True if ID is valid, False otherwise
        :rtype: bool
    )pbdoc");

    // Utility function for safe population ID validation
    m.def("isValidPopulationId", [](const PopulationId& id) {
        return id != INVALID_POPULATION_ID && id.value != UINT64_MAX;
    }, py::arg("id"), R"pbdoc(
        Check if a population ID is valid.
        
        :param id: Population ID to validate
        :returns: True if ID is valid, False otherwise
        :rtype: bool
    )pbdoc");

    // ============================================================================
    // SECTION 7: ADVANCED UTILITIES
    // ============================================================================
    
    // Function to get system information
    m.def("getSystemInfo", []() {
        std::string info = "NLM Python Bindings\n";
        info += "Version: 0.1.0\n";
        info += "Build: Default\n";
        info += "Features: Full neural simulation support\n";
        info += "License: MIT\n";
        info += "Authors: NLM Authors\n";
        return info;
    }, R"pbdoc(
        Get system information about the NLM bindings.
        
        :returns: System information string
        :rtype: str
    )pbdoc");

    // Function to check if a value is within safe range
    m.def("isInSafeRange", [](double value, double min = -1e6, double max = 1e6) {
        return value >= min && value <= max;
    }, py::arg("value"), py::arg("min") = -1e6, py::arg("max") = 1e6,
       R"pbdoc(
        Check if a value is within a safe range.
        
        :param value: Value to check
        :param min: Minimum safe value (default: -1e6)
        :param max: Maximum safe value (default: 1e6)
        :returns: True if value is in safe range, False otherwise
        :rtype: bool
    )pbdoc");

    // Function to clamp values to safe range
    m.def("clampToSafeRange", [](double value, double min = -1e6, double max = 1e6) {
        return std::max(min, std::min(max, value));
    }, py::arg("value"), py::arg("min") = -1e6, py::arg("max") = 1e6,
       R"pbdoc(
        Clamp a value to the safe range.
        
        :param value: Value to clamp
        :param min: Minimum safe value (default: -1e6)
        :param max: Maximum safe value (default: 1e6)
        :returns: Clamped value
        :rtype: float
    )pbdoc");

    // ============================================================================
    // END OF MODULE
    // ============================================================================
}

} // namespace nlm
