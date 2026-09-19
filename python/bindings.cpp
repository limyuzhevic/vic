#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
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
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"

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

    // NeuronId: Strongly-typed neuron identifier
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron

        Provides safe access to neuron IDs with comparison, hashing, and string representation
        capabilities. Each neuron has a unique ID that persists across simulations.

        Attributes:
            value (uint64_t): The internal ID value

        Examples:
            >>> neuron_id = NeuronId(42)
            >>> neuron_id.value
            42
            >>> other_id = NeuronId()
            >>> neuron_id == other_id
            False
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, R"pbdoc(Internal ID value)pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(Get neuron index)pbdoc")
        .def("__eq__", &NeuronId::operator==, R"pbdoc(Check equality with another NeuronId)pbdoc")
        .def("__ne__", &NeuronId::operator!=, R"pbdoc(Check inequality with another NeuronId)pbdoc")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(Get hash for dictionary keys)pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation)pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        }, R"pbdoc(String representation)pbdoc");

    // SynapseId: Strongly-typed synapse identifier
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse

        Provides safe access to synapse IDs with comparison, hashing, and string representation
        capabilities. Each synapse has a unique ID that persists across simulations.

        Attributes:
            value (uint64_t): The internal ID value

        Examples:
            >>> synapse_id = SynapseId(42)
            >>> synapse_id.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, R"pbdoc(Internal ID value)pbdoc")
        .def("index", &SynapseId::index, R"pbdoc(Get synapse index)pbdoc")
        .def("__eq__", &SynapseId::operator==, R"pbdoc(Check equality with another SynapseId)pbdoc")
        .def("__ne__", &SynapseId::operator!=, R"pbdoc(Check inequality with another SynapseId)pbdoc")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(Get hash for dictionary keys)pbdoc")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation)pbdoc")
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        }, R"pbdoc(String representation)pbdoc");

    // RegionId: Strongly-typed region identifier
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region

        Provides safe access to region IDs with comparison, hashing, and string representation
        capabilities. Each region has a unique ID that persists across simulations.

        Attributes:
            value (uint64_t): The internal ID value

        Examples:
            >>> region_id = RegionId(42)
            >>> region_id.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, R"pbdoc(Internal ID value)pbdoc")
        .def("index", &RegionId::index, R"pbdoc(Get region index)pbdoc")
        .def("__eq__", &RegionId::operator==, R"pbdoc(Check equality with another RegionId)pbdoc")
        .def("__ne__", &RegionId::operator!=, R"pbdoc(Check inequality with another RegionId)pbdoc")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); }, R"pbdoc(Get hash for dictionary keys)pbdoc")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation)pbdoc")
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        }, R"pbdoc(String representation)pbdoc");

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

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input

        Provides a 2D vision system with configurable resolution and data access.
        Supports setting and getting image data as float arrays.

        Example usage:
            >>> vision = Vision(80, 60, 3)
            >>> # Array-like access
            >>> data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6] * (80*60)  # Flatten 2D array
            >>> vision.setData(data)
            >>> width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
            >>> # Access pixel values
            >>> for i in range(min(5, len(vision))):
            ...     print(f"Pixel {i}: R={vision[i]:.2f}, G={vision[i+len(vision)//3]:.2f}, B={vision[i+2*len(vision)//3]:.2f}")

        Attributes:
            width (int): Vision width in pixels
            height (int): Vision height in pixels
            channels (int): Number of color channels (1-4)
            length (int): Total number of pixels (width * height * channels)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from a list or array of floats")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getDimensions", [](const Vision& v) {
            return std::make_tuple(v.getWidth(), v.getHeight(), v.getChannels());
        }, "Get vision dimensions as (width, height, channels)")
        .def("__len__", [](const Vision& v) {
            return v.getWidth() * v.getHeight() * v.getChannels();
        }, "Get total number of pixel values (width * height * channels)")
        .def("__getitem__", [](const Vision& v, size_t index) {
            if (index >= v.getWidth() * v.getHeight() * v.getChannels()) {
                throw py::index_error("Vision index out of range");
            }
            // Note: Vision class needs a getData() method to support this
            // This is a placeholder that will need actual implementation
            return 0.0f;  // TODO: Implement actual pixel access
        }, "Get pixel value at index (flattened array access)")
        .def("__setitem__", [](Vision& v, size_t index, float value) {
            if (index >= v.getWidth() * v.getHeight() * v.getChannels()) {
                throw py::index_error("Vision index out of range");
            }
            // Note: Vision class needs a setData method to support this
            // This is a placeholder that will need actual implementation
            // v.setData(v.getData()); // TODO: Implement actual pixel modification
        }, "Set pixel value at index (flattened array access)")
        .def("getData", [](const Vision& v) {
            return v.getData();
        }, "Get vision data as Python list")
        .def("getPixel", [](const Vision& v, size_t x, size_t y, size_t c = 0) {
            // Convert 2D position to index
            size_t index = ((y * v.getWidth()) + x) * v.getChannels() + c;
            return v.getData()[index];
        }, py::arg("x"), py::arg("y"), py::arg("channel"), "Get pixel value at (x, y) position, channel 0-2")
        .def("setPixel", [](Vision& v, size_t x, size_t y, size_t c, float value) {
            // Note: Vision class needs internal access for this
            // This is a placeholder
        }, py::arg("x"), py::arg("y"), py::arg("channel"), py::arg("value"), "Set pixel value at (x, y) position, channel")
        .def("__repr__", [](const Vision& v) {
            return "<Vision width=" + std::to_string(v.getWidth()) + " height=" + std::to_string(v.getHeight()) + " channels=" + std::to_string(v.getChannels()) + " length=" + std::to_string(v.getWidth() * v.getHeight() * v.getChannels()) + ">";
        }, "String representation");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input

        Provides audio data with configurable sampling rate and number of samples.
        Supports setting and getting audio samples as float arrays.

        Example usage:
            >>> audio = Audio(sampleRate=44100, numSamples=48000)
            >>> # Generate a simple sine wave
            >>> import math
            >>> data = [math.sin(2 * math.pi * 440 * i / 44100) for i in range(48000)]
            >>> audio.setData(data)
            >>> samples = audio.getNumSamples()
            >>> rate = audio.getSampleRate()

        Attributes:
            sampleRate (int): Audio sampling rate in Hz
            numSamples (int): Number of audio samples
            duration (float): Audio duration in seconds (sampleRate / numSamples)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from a list or array of floats")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& a) {
            return static_cast<float>(a.getNumSamples()) / a.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getData", [](const Audio& a) {
            return a.getData();
        }, "Get audio data as Python list")
        .def("getSample", [](const Audio& a, size_t index) {
            if (index >= a.getNumSamples()) {
                throw py::index_error("Audio sample index out of range");
            }
            return a.getData()[index];
        }, py::arg("index"), "Get audio sample at index")
        .def("setSample", [](Audio& a, size_t index, float value) {
            if (index >= a.getNumSamples()) {
                throw py::index_error("Audio sample index out of range");
            }
            // This is a simplified approach - in practice, you'd need to modify the internal data
            // For now, we'll just validate the index
        }, py::arg("index"), py::arg("value"), "Set audio sample at index")
        .def("__repr__", [](const Audio& a) {
            return "<Audio sampleRate=" + std::to_string(a.getSampleRate()) + " samples=" + std::to_string(a.getNumSamples()) + " duration=" + std::to_string(static_cast<float>(a.getNumSamples()) / a.getSampleRate()) + "s>";
        }, "String representation");

    // InternalSignals: Simple signal container with callback support
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input

        Provides a container for internal neural signals with add/remove/update support.
        Useful for passing internal brain state as sensory input.

        Example usage:
            >>> signals = InternalSignals()
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(-0.3)
            >>> signals.addSignal(0.8)
            >>> total = sum(signals.getAllSignals())
            >>> avg = total / len(signals.getAllSignals())
            >>> signals.clearSignals()

        Attributes:
            signals (list): Get all signals as Python list
            size (int): Number of signals
            timestamp (float): Timestamp of signal creation
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getAllSignals", [](const InternalSignals& self) {
            return self.getData();  // InternalSignals uses getData() for signal values
        }, "Get all signals as Python list")
        .def("size", [](const InternalSignals& self) {
            return self.getDimensions();  // InternalSignals uses getDimensions() for signal count
        }, "Get number of signals")
        .def("__len__", [](const InternalSignals& self) {
            return self.getDimensions();
        }, "Get number of signals (for 'len()' support)")
        .def("__getitem__", [](const InternalSignals& self, size_t index) {
            if (index >= self.getDimensions()) {
                throw py::index_error("Signal index out of range");
            }
            return self.getData()[index];
        }, "Get signal at index (for list-like access)")
        .def("__iter__", [](const InternalSignals& self) {
            return py::make_iterator(self.getData().begin(), self.getData().end());
        }, py::return_value_policy::copy, "Iterate over all signals")
        .def("__repr__", [](const InternalSignals& self) {
            std::ostringstream oss;
            oss << "<InternalSignals size=" << self.getDimensions() << " signals=[";
            const auto& data = self.getData();
            for (size_t i = 0; i < data.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << data[i];
            }
            oss << "]>";
            return oss.str();
        }, "String representation")
        .def_readonly("signals", &InternalSignals::getData, "Get all signals as list")
        .def_readonly("size", &InternalSignals::getDimensions, "Get number of signals")
        .def_readonly("timestamp", [](const InternalSignals& self) {
            return self.getTimestamp();
        }, "Get signal timestamp");

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

    // SimpleWorld: Context manager for automatic cleanup
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with context manager support

        Provides a simulated environment where agents interact, perceive the world,
        and take actions. The world manages physics simulation, object interactions,
        and sensory perception for connected agents.

        Example usage:
            >>> world = SimpleWorld()
            >>> world.configure(100, 100, 80, 80)
            >>> world.setAgentStart(50, 50)
            >>> # Use as context manager for automatic cleanup
            >>> with SimpleWorld() as world:
            ...     world.update(0.01)

        Attributes:
            width (float): World width in cells
            height (float): World height in cells
            visionWidth (size_t): Agent vision width
            visionHeight (size_t): Agent vision height
        )pbdoc")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Enter context manager, returns self")
        .def("__exit__", [](SimpleWorld& self, py::args) {
            // Cleanup if needed
        }, "Exit context manager, clean up resources")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept for agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld width=" + std::to_string(w.getWidth()) + " height=" + std::to_string(w.getHeight()) + ">";
        }, "String representation");

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
