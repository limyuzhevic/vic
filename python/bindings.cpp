#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

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

    //=============================================================================
    // Identity and Type Definitions
    //=============================================================================
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides methods to access and compare neuron IDs, and Pythonic behavior
        for hashing and representation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             R"pbdoc(Get the index portion of the neuron ID.
             
             Returns:
                 int: Index portion (lower 32 bits) of the neuron ID
             )pbdoc")
        .def("region", &NeuronId::region,
             R"pbdoc(Get the region ID portion of the neuron ID.
             
             Returns:
                 RegionId: Region ID portion (upper 32 bits) of the neuron ID
             )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Provides methods to access and compare synapse IDs, and Pythonic behavior
        for hashing and representation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             R"pbdoc(Get the index portion of the synapse ID.
             
             Returns:
                 int: Index portion (lower 32 bits) of the synapse ID
             )pbdoc")
        .def("region", &SynapseId::region,
             R"pbdoc(Get the region ID portion of the synapse ID.
             
             Returns:
                 RegionId: Region ID portion (upper 32 bits) of the synapse ID
             )pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Provides methods to access and compare region IDs, and Pythonic behavior
        for hashing and representation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             R"pbdoc(Get the index portion of the region ID.
             
             Returns:
                 int: Index portion (lower 32 bits) of the region ID
             )pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    //=============================================================================
    // Enumerations
    //=============================================================================
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

    //=============================================================================
    // Configuration Class
    //=============================================================================
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides methods to load, save, and manage configuration parameters
        as key-value pairs. Supports JSON file I/O and command-line argument parsing.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file.
                 
             Raises:
                 RuntimeError: If the file cannot be read or is not valid JSON.
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Number of command line arguments.
               argv: Array of command line argument strings.
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where the JSON configuration should be saved.
                 
             Raises:
                 RuntimeError: If the file cannot be written.
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check.
                 
             Returns:
                 bool: True if the key exists, False otherwise.
             )pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get(key);
        }, py::arg("key"),
           R"pbdoc(Get a configuration value.
           
           Args:
               key: Configuration key to retrieve.
               
           Returns:
               str: Configuration value.
               
           Raises:
               key_error: If the key does not exist.
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set a configuration value.
           
           Args:
               key: Configuration key to set.
               value: Configuration value to set.
           )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 list: List of all configuration keys.
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 str: Summary string.
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        })
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        });

    //=============================================================================
    // Sensory Input Classes
    //=============================================================================
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Provides virtual interface for different sensory modalities like
        vision, audio, and internal signals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data.
        
        Provides methods to access and manipulate image-like data with
        width, height, and color channel information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data.
             
             Args:
                 data: Flattened array of pixel values (width * height * channels).
                 
             Raises:
                 value_error: If the data vector is empty.
             )pbdoc")
        .def("getData", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Get vision data.
        
        Returns:
            list: Copy of pixel data as a list of floats.
        )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get image width in pixels.
             
             Returns:
                 int: Image width.
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get image height in pixels.
             
             Returns:
                 int: Image height.
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels.
             
             Returns:
                 int: Number of channels (1=grayscale, 3=RGB, 4=RGBA).
             )pbdoc")
        .def("getTotalPixels", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, R"pbdoc(Get total number of pixels.
        
        Returns:
            int: Total pixels (width * height).
        )pbdoc")
        .def("validate", [](const Vision& self) {
            return self.getWidth() > 0 && self.getHeight() > 0 && self.getChannels() > 0;
        }, R"pbdoc(Validate vision data.
        
        Returns:
            bool: True if dimensions are valid.
        )pbdoc");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Provides methods to access and manipulate audio sample data with
        sample rate and number of samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data.
             
             Args:
                 data: Array of audio sample values.
                 
             Raises:
                 value_error: If the data vector is empty.
             )pbdoc")
        .def("getData", [](const Audio& self) {
            return self.getData();
        }, R"pbdoc(Get audio data.
        
        Returns:
            list: Copy of audio samples as a list of floats.
        )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set audio sample rate.
             
             Args:
                 sampleRate: Sample rate in Hz.
                 
             Raises:
                 value_error: If sample rate is zero or negative.
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get audio sample rate.
             
             Returns:
                 int: Sample rate in Hz.
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of audio samples.
             
             Returns:
                 int: Number of samples.
             )pbdoc")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, R"pbdoc(Get audio duration in seconds.
        
        Returns:
            float: Duration in seconds.
        )pbdoc")
        .def("validate", [](const Audio& self) {
            return self.getSampleRate() > 0 && self.getNumSamples() > 0;
        }, R"pbdoc(Validate audio data.
        
        Returns:
            bool: True if data is valid.
        )pbdoc");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing neural activity.
        
        Provides methods to access and manipulate internal signal data.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add a signal value.
             
             Args:
                 value: Signal value to add.
             )pbdoc")
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        }, R"pbdoc(Get all signal values.
        
        Returns:
            list: List of all signal values.
        )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all signal values.)pbdoc")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(Get number of signals.
        
        Returns:
            int: Number of signals.
        )pbdoc")
        .def("getMeanSignal", [](const InternalSignals& self) {
            const auto& signals = self.getData();
            if (signals.empty()) return 0.0f;
            float sum = 0.0f;
            for (float val : signals) sum += val;
            return sum / signals.size();
        }, R"pbdoc(Get mean signal value.
        
        Returns:
            float: Mean signal value.
        )pbdoc")
        .def("validate", [](const InternalSignals& self) {
            return true;
        }, R"pbdoc(Validate internal signals.
        
        Returns:
            bool: True (validation always passes).
        )pbdoc");

    //=============================================================================
    // Action Class
    //=============================================================================
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Provides methods to create and manipulate motor actions with type
        and parameter information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Create an action with specified type.
             
             Args:
                 type: Type of action to create.
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Create an action with type and parameters.
             
             Args:
                 type: Type of action.
                 parameters: Action parameters as a list of floats.
             )pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(Get action type.
             
             Returns:
                 ActionType: Action type.
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set action type.
             
             Args:
                 type: Action type.
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get action parameters.
             
             Returns:
                 list: Copy of parameters as a list of floats.
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters.
             
             Args:
                 params: Parameters as a list of floats.
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get human-readable action name.
             
             Returns:
                 str: Action name.
             )pbdoc")
        .def("getDescription", &Action::getDescription,
             R"pbdoc(Get action description.
             
             Returns:
                 str: Action description.
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a copy of this action.
             
             Returns:
                 Action: Copy of this action.
             )pbdoc")
        .def("isEmpty", [](const Action& self) {
            return self.getParameters().empty();
        }, R"pbdoc(Check if action has no parameters.
        
        Returns:
            bool: True if action has no parameters.
        )pbdoc")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"),
             R"pbdoc(Add a parameter to the action.
             
             Args:
                 value: Parameter value to add.
             )pbdoc")
        .def("removeParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            params.erase(params.begin() + index);
            self.setParameters(params);
        }, py::arg("index"),
             R"pbdoc(Remove a parameter by index.
             
             Args:
                 index: Index of parameter to remove.
                 
             Raises:
                 index_error: If index is out of range.
             )pbdoc");

    //=============================================================================
    // World Object Class
    //=============================================================================
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents objects in the simulated world with position, type, and properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x,
                      R"pbdoc(X coordinate of the world object.)pbdoc")
        .def_readwrite("y", &WorldObject::y,
                      R"pbdoc(Y coordinate of the world object.)pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
                      R"pbdoc(Radius of the world object.)pbdoc")
        .def_readwrite("type", &WorldObject::type,
                      R"pbdoc(Type of the world object.)pbdoc")
        .def_readwrite("value", &WorldObject::value,
                      R"pbdoc(Value associated with the object.)pbdoc")
        .def_readwrite("active", &WorldObject::active,
                      R"pbdoc(Whether the object is active.)pbdoc")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Calculate Euclidean distance to another point.
           
           Args:
               x: X coordinate of target point.
               y: Y coordinate of target point.
               
           Returns:
               float: Distance.
           )pbdoc")
        .def("isWithinRadius", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= radius * radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
           R"pbdoc(Check if object is within radius of a point.
           
           Args:
               x: X coordinate of target point.
               y: Y coordinate of target point.
               radius: Radius to check.
               
           Returns:
               bool: True if within radius.
           )pbdoc")
        .def("to_dict", [](const WorldObject& self) {
            return py::dict(
                "x", self.x,
                "y", self.y,
                "radius", self.radius,
                "type", self.type,
                "value", self.value,
                "active", self.active
            );
        }, R"pbdoc(Convert world object to dictionary.
        
        Returns:
            dict: Dictionary representation.
        )pbdoc");

    //=============================================================================
    // Agent Body Class
    //=============================================================================
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state.
        
        Represents the physical state of an agent in the simulated world,
        including position, velocity, orientation, and energy/health metrics.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      R"pbdoc(X coordinate of the agent.)pbdoc")
        .def_readwrite("y", &AgentBody::y,
                      R"pbdoc(Y coordinate of the agent.)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
                      R"pbdoc(Orientation in radians.)pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      R"pbdoc(X component of velocity.)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      R"pbdoc(Y component of velocity.)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      R"pbdoc(Angular velocity in radians per second.)pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
                      R"pbdoc(Current energy level.)pbdoc")
        .def_readwrite("maxEnergy", &AgentBody::maxEnergy,
                      R"pbdoc(Maximum energy level.)pbdoc")
        .def_readwrite("health", &AgentBody::health,
                      R"pbdoc(Current health level.)pbdoc")
        .def_readwrite("maxHealth", &AgentBody::maxHealth,
                      R"pbdoc(Maximum health level.)pbdoc")
        .def_readwrite("age", &AgentBody::age,
                      R"pbdoc(Age in simulation steps.)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      R"pbdoc(Whether the agent is moving.)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      R"pbdoc(Whether the agent is turning.)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      R"pbdoc(Time since last action.)pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset body state to initial values.)pbdoc")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, R"pbdoc(Get linear speed.
        
        Returns:
            float: Linear speed.
        )pbdoc")
        .def("getMaxSpeed", [](const AgentBody& self) {
            return std::sqrt(self.maxEnergy * self.maxEnergy);
        }, R"pbdoc(Get maximum linear speed.
        
        Returns:
            float: Maximum linear speed.
        )pbdoc")
        .def("getAngle", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, R"pbdoc(Get movement angle in radians.
        
        Returns:
            float: Movement angle in radians.
        )pbdoc")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health >= self.maxHealth * 0.1f;
        }, R"pbdoc(Check if agent is healthy.
        
        Returns:
            bool: True if health is at least 10% of maximum.
        )pbdoc")
        .def("getEnergyRatio", [](const AgentBody& self) {
            return self.maxEnergy > 0 ? self.energy / self.maxEnergy : 0.0f;
        }, R"pbdoc(Get energy ratio (0-1).
        
        Returns:
            float: Energy ratio.
        )pbdoc")
        .def("getHealthRatio", [](const AgentBody& self) {
            return self.maxHealth > 0 ? self.health / self.maxHealth : 0.0f;
        }, R"pbdoc(Get health ratio (0-1).
        
        Returns:
            float: Health ratio.
        )pbdoc")
        .def("getSensoryPercept", [](const AgentBody& self) {
            return std::make_shared<SensoryPercept>();
        }, R"pbdoc(Create a new sensory percept for this agent.
        
        Returns:
            SensoryPercept: New sensory percept.
        )pbdoc");

    //=============================================================================
    // Action Result Class
    //=============================================================================
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world.
        
        Represents the result of an agent's action, including reward,
        success status, and explanatory message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward,
                      R"pbdoc(Reward value from the action.)pbdoc")
        .def_readwrite("success", &ActionResult::success,
                      R"pbdoc(Whether the action was successful.)pbdoc")
        .def_readwrite("message", &ActionResult::message,
                      R"pbdoc(Message describing the action result.)pbdoc")
        .def("to_dict", [](const ActionResult& self) {
            return py::dict(
                "reward", self.reward,
                "success", self.success,
                "message", self.message
            );
        }, R"pbdoc(Convert action result to dictionary.
        
        Returns:
            dict: Dictionary representation.
        )pbdoc")
        .def("__repr__", [](const ActionResult& ar) {
            std::string successStr = ar.success ? "Success" : "Failure";
            return "<ActionResult: " + successStr + " (reward=" + std::to_string(ar.reward) + ">";
        });

    //=============================================================================
    // Sensory Percept Class
    //=============================================================================
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data.
        
        Contains all sensory inputs for an agent including vision, audio,
        internal signals, touch, and proprioception.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::copy,
             R"pbdoc(Get vision data.
             
             Returns:
                 Vision: Vision sensory data.
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision data.
             
             Args:
                 vision: Vision sensory data.
             )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width.
             
             Returns:
                 int: Vision width.
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height.
             
             Returns:
                 int: Vision height.
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             py::return_value_policy::copy,
             R"pbdoc(Get touch data.
             
             Returns:
                 Vision: Touch sensory data.
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch data.
             
             Args:
                 touch: Touch sensory data.
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             py::return_value_policy::copy,
             R"pbdoc(Get internal signals.
             
             Returns:
                 InternalSignals: Internal signal data.
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals.
             
             Args:
                 internal: Internal signal data.
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             py::return_value_policy::copy,
             R"pbdoc(Get proprioception data.
             
             Returns:
                 Vision: Proprioception data.
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception data.
             
             Args:
                 proprioception: Proprioception data.
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             py::return_value_policy::copy,
             R"pbdoc(Get audio data.
             
             Returns:
                 Audio: Audio sensory data.
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio data.
             
             Args:
                 audio: Audio sensory data.
             )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             py::return_value_policy::copy,
             R"pbdoc(Get all sensory signals.
             
             Returns:
                 list: List of all signals.
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get sensory percept timestamp.
             
             Returns:
                 int: Timestamp.
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set sensory percept timestamp.
             
             Args:
                 timestamp: Timestamp.
             )pbdoc")
        .def("validate", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, R"pbdoc(Validate sensory percept.
        
        Returns:
            bool: True if percept is valid.
        )pbdoc")
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision", self.getVision(),
                "touch", self.getTouch(),
                "internal", self.getInternal(),
                "proprioception", self.getProprioception(),
                "audio", self.getAudio(),
                "timestamp", self.getTimestamp()
            );
        }, R"pbdoc(Convert sensory percept to dictionary.
        
        Returns:
            dict: Dictionary representation.
        )pbdoc")
        .def("__repr__", [](const SensoryPercept& sp) {
            return "<SensoryPercept: vision=[" + std::to_string(sp.getVisionWidth()) + "," +
                   std::to_string(sp.getVisionHeight()) + "]>";
        });

    //=============================================================================
    // Simple World Class
    //=============================================================================
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a basic 2D world with agents, objects, and simulation
        dynamics including physics and collision detection.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions and vision.
             
             Args:
                 width: World width.
                 height: World height.
                 visionWidth: Vision width for agents.
                 visionHeight: Vision height for agents.
                 
             Raises:
                 value_error: If dimensions are invalid.
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world state.)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.
             
             Args:
                 x: Starting X coordinate.
                 y: Starting Y coordinate.
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world simulation.
             
             Args:
                 timestep: Time step for simulation.
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply a motor command to the agent.
             
             Args:
                 cmd: Motor command to apply.
                 currentTime: Current simulation time.
                 
             Returns:
                 ActionResult: Result of applying the command.
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy,
             R"pbdoc(Get sensory percept for the agent.
             
             Returns:
                 SensoryPercept: Copy of the agent's sensory percept.
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy,
             R"pbdoc(Get agent body state.
             
             Returns:
                 AgentBody: Copy of the agent body state.
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add a world object.
             
             Args:
                 obj: World object to add.
                 
             Raises:
                 value_error: If object is invalid.
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.
             
             Args:
                 x: X coordinate.
                 y: Y coordinate.
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid.
             
             Args:
                 x: X coordinate.
                 y: Y coordinate.
                 
             Returns:
                 bool: True if position is valid.
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.
             
             Returns:
                 int: World width.
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.
             
             Returns:
                 int: World height.
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy.
             
             Returns:
                 float: Maximum energy.
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy.
             
             Args:
                 e: Maximum energy value.
                 
             Raises:
                 value_error: If energy is negative.
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.
             
             Returns:
                 float: Energy decay rate.
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.
             
             Args:
                 r: Decay rate.
                 
             Raises:
                 value_error: If rate is negative.
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get simulation time.
             
             Returns:
                 int: Simulation time.
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed.
             
             Args:
                 seed: Random seed.
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed.
             
             Returns:
                 int: Random seed.
             )pbdoc")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, R"pbdoc(Get agent position.
        
        Returns:
            tuple: (x, y) position.
        )pbdoc")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || x > self.getWidth() || y < 0 || y > self.getHeight()) {
                throw py::value_error("Position out of bounds");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Set agent position.
           
           Args:
               x: X coordinate.
               y: Y coordinate.
               
           Raises:
               value_error: If position is out of bounds.
           )pbdoc")
        .def("getObjectCount", [](const SimpleWorld& self) {
            return 0;  // Placeholder - actual implementation would need access to internal state
        }, R"pbdoc(Get number of objects in world.
        
        Returns:
            int: Number of objects.
        )pbdoc")
        .def("clearObjects", &SimpleWorld::reset,
             R"pbdoc(Clear all objects from world.)pbdoc")
        .def("validate", [](const SimpleWorld& self) {
            return self.getWidth() > 0 && self.getHeight() > 0;
        }, R"pbdoc(Validate world configuration.
        
        Returns:
            bool: True if world is valid.
        )pbdoc");

    //=============================================================================
    // Brain Class
    //=============================================================================
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Provides the core neural simulation functionality including neuron activation,
        synaptic transmission, learning, and action production based on neural activity.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create a brain with configuration.
             
             Args:
                 config: Configuration object.
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
              R"pbdoc(Perform a simulation step.
              
              Args:
                  currentStep: Current simulation step.
              )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
              R"pbdoc(Perform a simulation step with timestamp.
              
              Args:
                  currentStep: Current simulation step.
                  currentTime: Current simulation time.
              )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.)pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity.)
             
             Returns:
                 Action: Produced action.
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::copy,
             R"pbdoc(Get a region by ID.)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions.)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.)pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::copy,
             R"pbdoc(Get all regions.)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.)pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::copy,
             R"pbdoc(Get the configuration.)pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status.)pbdoc")
        .def("getStats", [](const Brain& self) {
            return py::dict(
                "neuronCount", self.getTotalNeuronCount(),
                "synapseCount", self.getTotalSynapseCount(),
                "activeCount", self.getActiveNeuronCount(),
                "firingCount", self.getFiringNeuronCount(),
                "totalSpikes", self.getTotalSpikeCount(),
                "avgFiringRate", self.getAverageFiringRate(),
                "eiRatio", self.getExcitationInhibitionRatio()
            );
        }, R"pbdoc(Get brain statistics.
        
        Returns:
            dict: Dictionary with brain statistics.
        )pbdoc")
        .def("validate", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0 && self.getTotalSynapseCount() >= 0;
        }, R"pbdoc(Validate brain state.
        
        Returns:
            bool: True if brain is valid.
        )pbdoc");

    //=============================================================================
    // Agent Brain Class
    //=============================================================================
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides methods to interface between the neural brain and the simulated
        world, including sensory processing, motor decoding, and reward modulation.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create an agent brain with underlying brain.
             
             Args:
                 brain: Underlying brain object.
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world.)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.)
             
             Returns:
                 Action: Decoded motor command.
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system.)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::copy,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable or disable development.)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable or disable curiosity.)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.)pbdoc")
        .def("getStatus", [](const AgentBrain& self) {
            return py::dict(
                "developmentalStage", self.getDevelopmentalStage(),
                "neuromodulationLevel", self.getNeuromodulationLevel(),
                "curiosityLevel", self.getCuriosityLevel(),
                "noveltyLevel", self.getNoveltyLevel(),
                "predictionError", self.getPredictionError(),
                "rewardModulationEnabled", self.isRewardModulationEnabled(),
                "structuralPlasticityEnabled", self.isStructuralPlasticityEnabled(),
                "developmentEnabled", self.isDevelopmentEnabled(),
                "curiosityEnabled", self.isCuriosityEnabled()
            );
        }, R"pbdoc(Get agent brain status.
        
        Returns:
            dict: Dictionary with status information.
        )pbdoc")
        .def("validate", [](const AgentBrain& self) {
            return self.getBrain() != nullptr;
        }, R"pbdoc(Validate agent brain state.
        
        Returns:
            bool: True if valid.
        )pbdoc");

    //=============================================================================
    // Factory Functions
    //=============================================================================
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface.")

    //=============================================================================
    // Global Constants
    //=============================================================================
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    //=============================================================================
    // Utility Functions
    //=============================================================================
    m.def("validateAll", [](const py::dict& configDict) {
        try {
            auto config = std::make_shared<Config>();
            
            if (configDict.contains("width")) {
                config->set("width", std::to_string(py::cast<int>(configDict["width"])));
            }
            if (configDict.contains("height")) {
                config->set("height", std::to_string(py::cast<int>(configDict["height"])));
            }
            
            return true;
        } catch (const std::exception& e) {
            throw py::value_error("Validation failed: " + std::string(e.what()));
        }
    }, py::arg("configDict"),
       R"pbdoc(Validate configuration dictionary.
       
       Args:
           configDict: Configuration dictionary.
           
       Returns:
           bool: True if valid.
           
       Raises:
           value_error: If validation fails.
       )pbdoc")

    m.def("printVersion", []() {
        return "NLM Python Bindings v1.0.0";
    }, R"pbdoc(Print version information.
    
    Returns:
        str: Version information.
    )pbdoc");

    m.def("getMemoryUsage", []() {
        return "Memory usage information not available";
    }, R"pbdoc(Get memory usage information.
    
    Returns:
        str: Memory usage information.
    )pbdoc");
}

} // namespace nlm