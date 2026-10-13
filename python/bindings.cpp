#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>

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

// Custom Python exception for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a comprehensive Python interface for the NLM neural
        simulation framework, enabling researchers and developers to create,
        simulate, and experiment with artificial neural systems using Python.
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Sensory-motor integration
        - Memory systems (working, episodic, associative)
        - Developmental processes
        - Neuromodulation (dopamine, curiosity, novelty)
        - Action planning and decision making
        
        Quick Start Example:
        ```python
        import pynlm
        
        # Create a default configuration
        config = pynlm.Config()
        
        # Create a neural brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run simulation for 100 steps
        for i in range(100):
            brain.step(i, i * 0.001)
            
        # Check learning
        print(f"Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
        ```
    )pbdoc";

    // Register NLM-specific exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Neuron identifier with comprehensive functionality
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>(), R"pbdoc(
            Create a default neuron identifier.
            
            Returns:
                NeuronId: A new neuron identifier with default values.
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a neuron identifier with a specific value.
            
            Args:
                value: The unique identifier value for the neuron.
                
            Returns:
                NeuronId: A neuron identifier with the specified value.
                
            Examples:
                >>> NeuronId(123)
                <NeuronId: 123>
                >>> id = NeuronId()
                >>> id.value
                0
        )pbdoc")
        .def_readwrite("value", &NeuronId::value, R"pbdoc(
            The unique identifier value for this neuron.
            
            Read-write property that can be directly accessed or modified.
            
            Returns:
                int: The neuron identifier value.
                
            Examples:
                >>> id = NeuronId(42)
                >>> id.value
                42
                >>> id.value = 100
                >>> id.value
                100
        )pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(
            Get the integer index representation of this neuron identifier.
            
            Returns:
                int: The integer index (same as value).
                
            Examples:
                >>> id = NeuronId(999)
                >>> id.index()
                999
        )pbdoc")
        .def("__eq__", &NeuronId::operator==, py::is_operator(), R"pbdoc(
            Check equality with another neuron identifier.
            
            Args:
                other: Another NeuronId to compare with.
                
            Returns:
                bool: True if identifiers have the same value.
                
            Examples:
                >>> id1 = NeuronId(42)
                >>> id2 = NeuronId(42)
                >>> id1 == id2
                True
                >>> id3 = NeuronId(99)
                >>> id1 == id3
                False
        )pbdoc")
        .def("__ne__", &NeuronId::operator!=, py::is_operator(), R"pbdoc(
            Check inequality with another neuron identifier.
            
            Args:
                other: Another NeuronId to compare with.
                
            Returns:
                bool: True if identifiers have different values.
                
            Examples:
                >>> id1 = NeuronId(42)
                >>> id2 = NeuronId(99)
                >>> id1 != id2
                True
                >>> id3 = NeuronId(42)
                >>> id1 != id3
                False
        )pbdoc")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(
            Return hash of the neuron identifier for use in dictionaries and sets.
            
            Returns:
                int: Hash value for this identifier.
                
            Examples:
                >>> id1 = NeuronId(42)
                >>> id2 = NeuronId(42)
                >>> hash(id1) == hash(id2)
                True
        )pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(
            Return string representation of the neuron identifier.
            
            Returns:
                str: String representation of the identifier.
                
            Examples:
                >>> id = NeuronId(123)
                >>> repr(id)
                '<NeuronId: 123>'
        )pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(
            Return string representation of the neuron identifier value.
            
            Returns:
                str: String representation of the identifier value.
                
            Examples:
                >>> id = NeuronId(456)
                >>> str(id)
                '456'
        )pbdoc");

    // Synapse identifier with comprehensive functionality
    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>(), R"pbdoc(
            Create a default synapse identifier.
            
            Returns:
                SynapseId: A new synapse identifier with default values.
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a synapse identifier with a specific value.
            
            Args:
                value: The unique identifier value for the synapse.
                
            Returns:
                SynapseId: A synapse identifier with the specified value.
                
            Examples:
                >>> SynapseId(123)
                <SynapseId: 123>
                >>> id = SynapseId()
                >>> id.value
                0
        )pbdoc")
        .def_readwrite("value", &SynapseId::value, R"pbdoc(
            The unique identifier value for this synapse.
            
            Read-write property that can be directly accessed or modified.
            
            Returns:
                int: The synapse identifier value.
                
            Examples:
                >>> id = SynapseId(42)
                >>> id.value
                42
                >>> id.value = 100
                >>> id.value
                100
        )pbdoc")
        .def("index", &SynapseId::index, R"pbdoc(
            Get the integer index representation of this synapse identifier.
            
            Returns:
                int: The integer index (same as value).
                
            Examples:
                >>> id = SynapseId(999)
                >>> id.index()
                999
        )pbdoc")
        .def("__eq__", &SynapseId::operator==, py::is_operator(), R"pbdoc(
            Check equality with another synapse identifier.
            
            Args:
                other: Another SynapseId to compare with.
                
            Returns:
                bool: True if identifiers have the same value.
                
            Examples:
                >>> id1 = SynapseId(42)
                >>> id2 = SynapseId(42)
                >>> id1 == id2
                True
                >>> id3 = SynapseId(99)
                >>> id1 == id3
                False
        )pbdoc")
        .def("__ne__", &SynapseId::operator!=, py::is_operator(), R"pbdoc(
            Check inequality with another synapse identifier.
            
            Args:
                other: Another SynapseId to compare with.
                
            Returns:
                bool: True if identifiers have different values.
                
            Examples:
                >>> id1 = SynapseId(42)
                >>> id2 = SynapseId(99)
                >>> id1 != id2
                True
                >>> id3 = SynapseId(42)
                >>> id1 != id3
                False
        )pbdoc")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(
            Return hash of the synapse identifier for use in dictionaries and sets.
            
            Returns:
                int: Hash value for this identifier.
                
            Examples:
                >>> id1 = SynapseId(42)
                >>> id2 = SynapseId(42)
                >>> hash(id1) == hash(id2)
                True
        )pbdoc")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(
            Return string representation of the synapse identifier.
            
            Returns:
                str: String representation of the identifier.
                
            Examples:
                >>> id = SynapseId(123)
                >>> repr(id)
                '<SynapseId: 123>'
        )pbdoc")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(
            Return string representation of the synapse identifier value.
            
            Returns:
                str: String representation of the identifier value.
                
            Examples:
                >>> id = SynapseId(456)
                >>> str(id)
                '456'
        )pbdoc");

    // Region identifier with comprehensive functionality
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>(), R"pbdoc(
            Create a default region identifier.
            
            Returns:
                RegionId: A new region identifier with default values.
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a region identifier with a specific value.
            
            Args:
                value: The unique identifier value for the region.
                
            Returns:
                RegionId: A region identifier with the specified value.
                
            Examples:
                >>> RegionId(123)
                <RegionId: 123>
                >>> id = RegionId()
                >>> id.value
                0
        )pbdoc")
        .def_readwrite("value", &RegionId::value, R"pbdoc(
            The unique identifier value for this region.
            
            Read-write property that can be directly accessed or modified.
            
            Returns:
                int: The region identifier value.
                
            Examples:
                >>> id = RegionId(42)
                >>> id.value
                42
                >>> id.value = 100
                >>> id.value
                100
        )pbdoc")
        .def("index", &RegionId::index, R"pbdoc(
            Get the integer index representation of this region identifier.
            
            Returns:
                int: The integer index (same as value).
                
            Examples:
                >>> id = RegionId(999)
                >>> id.index()
                999
        )pbdoc")
        .def("__eq__", &RegionId::operator==, py::is_operator(), R"pbdoc(
            Check equality with another region identifier.
            
            Args:
                other: Another RegionId to compare with.
                
            Returns:
                bool: True if identifiers have the same value.
                
            Examples:
                >>> id1 = RegionId(42)
                >>> id2 = RegionId(42)
                >>> id1 == id2
                True
                >>> id3 = RegionId(99)
                >>> id1 == id3
                False
        )pbdoc")
        .def("__ne__", &RegionId::operator!=, py::is_operator(), R"pbdoc(
            Check inequality with another region identifier.
            
            Args:
                other: Another RegionId to compare with.
                
            Returns:
                bool: True if identifiers have different values.
                
            Examples:
                >>> id1 = RegionId(42)
                >>> id2 = RegionId(99)
                >>> id1 != id2
                True
                >>> id3 = RegionId(42)
                >>> id1 != id3
                False
        )pbdoc")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(
            Return hash of the region identifier for use in dictionaries and sets.
            
            Returns:
                int: Hash value for this identifier.
                
            Examples:
                >>> id1 = RegionId(42)
                >>> id2 = RegionId(42)
                >>> hash(id1) == hash(id2)
                True
        )pbdoc")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(
            Return string representation of the region identifier.
            
            Returns:
                str: String representation of the identifier.
                
            Examples:
                >>> id = RegionId(123)
                >>> repr(id)
                '<RegionId: 123>'
        )pbdoc")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(
            Return string representation of the region identifier value.
            
            Returns:
                str: String representation of the identifier value.
                
            Examples:
                >>> id = RegionId(456)
                >>> str(id)
                '456'
        )pbdoc");

    // Population identifier with comprehensive functionality
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>(), R"pbdoc(
            Create a default population identifier.
            
            Returns:
                PopulationId: A new population identifier with default values.
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a population identifier with a specific value.
            
            Args:
                value: The unique identifier value for the population.
                
            Returns:
                PopulationId: A population identifier with the specified value.
                
            Examples:
                >>> PopulationId(123)
                <PopulationId: 123>
                >>> id = PopulationId()
                >>> id.value
                0
        )pbdoc")
        .def_readwrite("value", &PopulationId::value, R"pbdoc(
            The unique identifier value for this population.
            
            Read-write property that can be directly accessed or modified.
            
            Returns:
                int: The population identifier value.
                
            Examples:
                >>> id = PopulationId(42)
                >>> id.value
                42
                >>> id.value = 100
                >>> id.value
                100
        )pbdoc")
        .def("index", &PopulationId::index, R"pbdoc(
            Get the integer index representation of this population identifier.
            
            Returns:
                int: The integer index (same as value).
                
            Examples:
                >>> id = PopulationId(999)
                >>> id.index()
                999
        )pbdoc")
        .def("__eq__", &PopulationId::operator==, py::is_operator(), R"pbdoc(
            Check equality with another population identifier.
            
            Args:
                other: Another PopulationId to compare with.
                
            Returns:
                bool: True if identifiers have the same value.
                
            Examples:
                >>> id1 = PopulationId(42)
                >>> id2 = PopulationId(42)
                >>> id1 == id2
                True
                >>> id3 = PopulationId(99)
                >>> id1 == id3
                False
        )pbdoc")
        .def("__ne__", &PopulationId::operator!=, py::is_operator(), R"pbdoc(
            Check inequality with another population identifier.
            
            Args:
                other: Another PopulationId to compare with.
                
            Returns:
                bool: True if identifiers have different values.
                
            Examples:
                >>> id1 = PopulationId(42)
                >>> id2 = PopulationId(99)
                >>> id1 != id2
                True
                >>> id3 = PopulationId(42)
                >>> id1 != id3
                False
        )pbdoc")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(
            Return hash of the population identifier for use in dictionaries and sets.
            
            Returns:
                int: Hash value for this identifier.
                
            Examples:
                >>> id1 = PopulationId(42)
                >>> id2 = PopulationId(42)
                >>> hash(id1) == hash(id2)
                True
        )pbdoc")
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(
            Return string representation of the population identifier.
            
            Returns:
                str: String representation of the identifier.
                
            Examples:
                >>> id = PopulationId(123)
                >>> repr(id)
                '<PopulationId: 123>'
        )pbdoc")
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(
            Return string representation of the population identifier value.
            
            Returns:
                str: String representation of the identifier value.
                
            Examples:
                >>> id = PopulationId(456)
                >>> str(id)
                '456'
        )pbdoc");

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

    // Enhanced configuration class with Pythonic interface
    py::class_<Config>(m, "Config")
        .def(py::init<>(), R"pbdoc(
            Create a new configuration object with default settings.
            
            The Config class provides a key-value storage system for neural
            simulation parameters. It supports multiple value types including
            integers, floats, strings, booleans, and arrays.
            
            Returns:
                Config: A new configuration object.
                
            Examples:
                >>> config = Config()
                >>> config.set("neuron_count", 100)
                >>> config.get("neuron_count")
                100
                >>> config.get_or("random_seed", 42)
                42
                
                # Configuration can be saved and loaded
                >>> config.save_to_file("settings.json")
                >>> new_config = Config()
                >>> new_config.load_from_file("settings.json")
        )pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
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
