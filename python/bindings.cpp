# PyBindings.cpp

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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

// Enhanced Python bindings with modern C++ features and user-friendly API

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        -------------------------------
        
        A comprehensive Python interface for the NLM neural simulation framework.
        This enhanced version provides:
        
        - **Intuitive API**: High-level convenience methods for typical use cases
        - **Comprehensive Documentation**: Detailed docstrings with examples
        - **Robust Error Handling**: Better error messages and validation
        - **Flexible Configuration**: Support for JSON/YAML and simple key=value formats
        - **Extensible Design**: Easy to add new simulation scenarios
        - **Performance Optimized**: Modern C++20 features for better performance
        
        Key Features:
        • Easy simulation setup with configuration presets
        • Built-in experiment templates for learning research
        • World interaction and agent control
        • Real-time monitoring and visualization data
        • Checkpoint management for long-running simulations
        
        Getting Started:
        >>> import pynlm as nlm
        >>> # Create a brain with default configuration
        >>> config = nlm.Config()
        >>> brain = nlm.Brain(config)
        >>> brain.initialize()
        >>> # Run a simple learning experiment
        >>> brain = nlm.run_learning_experiment(steps=1000)
        
        For detailed documentation, visit: https://github.com/nuelm/nlm
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === Enhanced Configuration Management ===

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration manager for NLM simulations.
        
        Provides flexible configuration with support for:
        - Simple key=value format files
        - JSON configuration files (Phase 2)
        - YAML configuration files (Phase 2)
        - Command-line argument overrides
        - Runtime configuration with validation
        
        Configuration sources (in priority order):
        1. Command line arguments (highest priority)
        2. Configuration files
        3. Default values (lowest priority)
        
        Example:
            >>> config = nlm.Config()
            >>> config.loadFromFile("my_config.cfg")
            >>> config.set("learning_rate", 0.01)
            >>> brain = nlm.Brain(config)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a file.
             
             Supports multiple formats based on file extension:
             - .cfg: Simple key=value format
             - .json: JSON format (Phase 2)
             - .yaml/.yml: YAML format (Phase 2)
             
             Args:
                 filepath: Path to configuration file
                 
             Returns:
                 bool: True if file loaded successfully, False otherwise
                 
             Raises:
                 RuntimeError: If file parsing fails
             )pbdoc")
        .def("loadFromYAML", &Config::loadFromYAML, py::arg("filepath"),
             R"pbdoc(Load configuration from a YAML file.
             
             Requires libyaml. Available in Phase 2.
             
             Args:
                 filepath: Path to YAML configuration file
                 
             Returns:
                 bool: True if YAML loaded successfully
             )pbdoc")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             R"pbdoc(Load configuration from command line arguments.
             
             Parses command line arguments in both formats:
             - --key=value
             - -k value
             
             Args:
                 args: List of command line arguments (e.g., ["--steps=1000", "--learning_rate", "0.01"])
                 
             Returns:
                 bool: True if arguments parsed successfully
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save current configuration to file.
             
             Args:
                 filepath: Path where to save the configuration
                 
             Returns:
                 bool: True if save successful
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if key exists
             )pbdoc")
        .def("getOr", [](const Config& self, const std::string& key, const auto& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a configuration value with default.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key not found
                 
             Returns:
                 Configuration value of type matching defaultValue
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 list: All configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries."pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary of all configuration entries.
             
             Returns:
                 str: Formatted summary string
             )pbdoc")
        .def("validate", [](const Config& self) {
            return self.validate();
        }, R"pbdoc(Validate configuration.
            
            Checks for:
            - Missing required keys
            - Invalid value types
            - Logical inconsistencies
            
            Returns:
                list: List of validation error messages (empty if valid)
            )pbdoc")
        .def("isValid", &Config::isValid,
             R"pbdoc(Check if configuration is valid.
             
             Returns:
                 bool: True if configuration passes validation
             )pbdoc")
        .def("exportToDict", [](const Config& self) {
            std::unordered_map<std::string, std::string> result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get<std::string>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        }, R"pbdoc(Export configuration to Python dictionary.
            
            Returns:
                dict: Configuration as Python-friendly dictionary
            )pbdoc")
        .def("importFromDict", [](Config& self, const std::unordered_map<std::string, std::string>& configDict) {
            for (const auto& [key, value] : configDict) {
                self.set(key, value);
            }
        }, py::arg("config_dict"),
             R"pbdoc(Import configuration from Python dictionary.
            
            Args:
                config_dict: Dictionary of configuration key-value pairs
            )pbdoc")
        .def("createDefault", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, R"pbdoc(Create a default configuration.
            
            Returns:
                Config: New configuration object
            )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config entries: " + std::to_string(cfg.getKeys().size()) + ">";
        });

    // === Enhanced Identity Types ===

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronIds are used throughout NLM to reference specific neurons.
        "pbdoc")
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseIds are used to reference specific synapses and their properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Regions contain populations of neurons and manage their organization.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Populations group neurons by type and function for efficient processing.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &NeuronId::operator!=);

    // === Enhanced Sensory Processing ===

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory inputs.
        
        Sensory inputs provide data about the environment to the brain.
        "pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input."pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector."pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality."pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp."pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(Set the timestamp."pbdoc"));

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing a 2D image.
        
        Provides pixel data representing the visual field.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set visual data.
            
            Args:
                data: Flattened array of RGB values (width * height * channels)
            )pbdoc")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixelData", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Get flattened pixel data."pbdoc")
        .def("setPixelData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set pixel data from flattened array."pbdoc"));

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Provides time-series audio data for processing.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data.
            
            Args:
                data: Audio samples (sampleRate * numSamples)
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(Set sample rate."pbdoc")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal neural signals from within the brain.
        
        Provides access to internal brain states like predictions, errors, and novelty.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add an internal signal."pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all internal signals."pbdoc")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(Get number of internal signals."pbdoc")
        .def("getSignalData", [](const InternalSignals& self) {
            return self.getData();
        }, R"pbdoc(Get all signal values."pbdoc"));

    // === Enhanced Motor Output ===

    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action for the brain to produce.
        
        Actions are the output of the brain, commanding the agent's body.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set action type."pbdoc")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters."pbdoc")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            return index < params.size() ? params[index] : 0.0f;
        }, py::arg("index"), R"pbdoc(Get a specific parameter."pbdoc")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), R"pbdoc(Set a specific parameter."pbdoc")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone, R"pbdoc(Create a copy of this action."pbdoc")
        .def("isEmpty", [](const Action& self) {
            return self.getName() == "";
        }, R"pbdoc(Check if action is empty."pbdoc")
        .def("toDict", [](const Action& self) {
            return std::unordered_map<std::string, std::string>{
                {"type", self.getName()},
                {"parameters", self.getParameter(0)}
            };
        }, R"pbdoc(Convert action to dictionary."pbdoc"));

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Physical object in the simulated world.
        
        Represents entities like resources, hazards, walls, and markers.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, R"pbdoc(Object X coordinate."pbdoc")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Object Y coordinate."pbdoc")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius."pbdoc")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type."pbdoc")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value/property."pbdoc")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Whether object is active."pbdoc")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to position."pbdoc")
        .def("isNear", [](const WorldObject& self, float x, float y, float distance) {
            return self.distanceTo(x, y) < distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), R"pbdoc(Check if near position."pbdoc"));

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical state of an agent in the world.
        
        Tracks position, orientation, velocity, and physiological state.
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X coordinate."pbdoc")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y coordinate."pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Facing direction in radians."pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity."pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity."pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Rotational velocity."pbdoc")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Current energy level."pbdoc")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Current health level."pbdoc")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Agent age in simulation steps."pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(Whether agent is currently moving."pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(Whether agent is currently turning."pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(Time since last action."pbdoc")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset to initial state."pbdoc")
        .def("moveTo", [](AgentBody& self, float x, float y, float maxSpeed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0) {
                float speed = std::min(maxSpeed, distance / 0.1f); // Assume timestep of 0.1
                self.velocityX = dx * speed / distance;
                self.velocityY = dy * speed / distance;
                self.x += self.velocityX * 0.1f;
                self.y += self.velocityY * 0.1f;
            }
            self.isMoving = true;
            self.lastActionTime = 0;
        }, py::arg("x"), py::arg("y"), py::arg("maxSpeed"), R"pbdoc(Move toward target position."pbdoc")
        .def("rotateTo", [](AgentBody& self, float angle, float maxAngularSpeed) {
            float dAngle = angle - self.orientation;
            while (dAngle > M_PI) dAngle -= 2 * M_PI;
            while (dAngle < -M_PI) dAngle += 2 * M_PI;
            float angularSpeed = std::min(maxAngularSpeed, std::abs(dAngle));
            self.angularVelocity = (dAngle > 0 ? angularSpeed : -angularSpeed);
            self.orientation += self.angularVelocity * 0.1f;
            self.isTurning = true;
            self.lastActionTime = 0;
        }, py::arg("angle"), py::arg("maxAngularSpeed"), R"pbdoc(Rotate to target angle."pbdoc")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, R"pbdoc(Get current position as (x, y) tuple."pbdoc")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, R"pbdoc(Get current velocity as (vx, vy) tuple."pbdoc")
        .def("getHeading", [](const AgentBody& self) {
            return self.orientation;
        }, R"pbdoc(Get current heading angle in radians."pbdoc"));

    // === Enhanced World Simulation ===

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM agent-environment interaction.
        
        Manages agent state, physical objects, and simulation updates.
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions and sensory limits.
            
            Args:
                width: World width in arbitrary units
                height: World height in arbitrary units
                visionWidth: Maximum vision range in X direction
                visionHeight: Maximum vision range in Y direction
            )pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state."pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent's starting position."pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state for next timestep.
            
            Args:
                timestep: Time step duration
            )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply agent's motor command to world.
            
            Args:
                cmd: Motor command to execute
                currentTime: Current simulation time
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get current sensory percept."pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent's current body state."pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add an object to the world."pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position."pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid."pbdoc")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("createVisionFromImage", [](SimpleWorld& self, const std::string& imagePath) {
            // Placeholder for image processing functionality
            return Vision(); // Would need actual image loading implementation
        }, py::arg("image_path"), R"pbdoc(Create vision input from image file."pbdoc")
        .def("createAudioFromFile", [](SimpleWorld& self, const std::string& audioPath) {
            // Placeholder for audio processing functionality
            return Audio(); // Would need actual audio loading implementation
        }, py::arg("audio_path"), R"pbdoc(Create audio input from audio file."pbdoc"));

    // === Enhanced Brain Interface ===

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain with enhanced capabilities.
        
        The main computational engine implementing neural processing,
        learning, and action production.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize brain with current configuration."pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step with given step number."pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp."pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain."pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into specific neuron."pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current to all neurons of specific type."pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity."pbdoc")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state to initial."pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file for checkpointing."pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file."pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region."pbdoc")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID."pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get number of regions."pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs."pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions."pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count."pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count."pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get active neuron count."pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get firing neuron count."pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate."pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get E/I balance ratio."pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count."pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage."pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage."pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, R"pbdoc(Get brain configuration."pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log current brain status."pbdoc")
        // Enhanced brain helper methods
        .def("runExperiment", [](Brain& self, const std::string& experimentType, int steps) {
            // Helper for running pre-configured experiments
            return self.runExperiment(experimentType, steps);
        }, py::arg("experiment_type"), py::arg("steps"),
             R"pbdoc(Run a pre-configured experiment.
            
            Args:
                experiment_type: Type of experiment ('learning', 'plasticity', 'stdp')
                steps: Number of simulation steps
            )pbdoc")
        .def("simulateEpisode", [](Brain& self, const std::vector<float>& sensoryInput, int steps) {
            // Helper for simulating an episode with given sensory input
            self.reset();
            for (int i = 0; i < steps; ++i) {
                self.step(i, i * 0.001);
            }
            return self.produceAction();
        }, py::arg("sensory_input"), py::arg("steps"),
             R"pbdoc(Simulate an episode.
            
            Args:
                sensory_input: Initial sensory input
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("getLearningStatistics", [](Brain& self) {
            // Enhanced learning statistics helper
            std::unordered_map<std::string, double> stats;
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["firing_rate"] = self.getAverageFiringRate();
            stats["active_neurons"] = self.getActiveNeuronCount();
            return stats;
        }, R"pbdoc(Get comprehensive learning statistics.
            
            Returns:
                dict: Learning statistics
            )pbdoc")
        .def("monitorActivity", [](Brain& self, const std::string& activityType) {
            // Enhanced activity monitoring
            if (activityType == "spikes") {
                return self.getTotalSpikeCount();
            } else if (activityType == "firing") {
                return self.getFiringNeuronCount();
            }
            return 0;
        }, py::arg("activity_type"), R"pbdoc(Monitor neural activity.
            
            Args:
                activity_type: Type of activity ('spikes', 'firing')
                
            Returns:
                int: Activity count
            )pbdoc")
        .def("enableAllPlasticity", [](Brain& self) {
            // Helper to enable all plasticity mechanisms
            auto config = self.getConfig();
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
        }, R"pbdoc(Enable all plasticity mechanisms (STDP, Hebbian, structural)."pbdoc")
        .def("disableAllPlasticity", [](Brain& self) {
            // Helper to disable all plasticity mechanisms
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, false, false);
                }
            }
        }, R"pbdoc(Disable all plasticity mechanisms."pbdoc")
        .def("enableAllNeuromodulation", [](Brain& self) {
            // Helper to enable all neuromodulation
            return self.enableRewardModulation(true) &&
                   self.enableStructuralPlasticity(true) &&
                   self.enableDevelopment(true) &&
                   self.enableCuriosity(true);
        }, R"pbdoc(Enable all neuromodulation systems."pbdoc")
        .def("getPlasticityStatus", [](Brain& self) {
            // Enhanced plasticity status
            std::unordered_map<std::string, bool> status;
            if (auto* region = self.getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    status["stdp"] = syn->isSTDPActive();
                    status["hebbian"] = syn->isHebbianActive();
                    status["structural"] = syn->isStructuralPlasticityActive();
                }
            }
            return status;
        }, R"pbdoc(Get current plasticity status."pbdoc")
        .def("getNeuromodulationLevels", [](Brain& self) {
            // Enhanced neuromodulation levels
            std::unordered_map<std::string, double> levels;
            levels["reward"] = self.getNeuromodulationLevel();
            levels["curiosity"] = self.getCuriosityLevel();
            levels["novelty"] = self.getNoveltyLevel();
            levels["prediction_error"] = self.getPredictionError();
            return levels;
        }, R"pbdoc(Get current neuromodulation levels."pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage."pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage."pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, R"pbdoc(Get brain configuration."pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log current brain status."pbdoc")
        // Factory functions for common use cases
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, R"pbdoc(Create a default configuration object."pbdoc")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), R"pbdoc(Create a new brain with configuration."pbdoc")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, R"pbdoc(Create a new simple world object."pbdoc")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), R"pbdoc(Create a new agent brain interface."pbdoc")
        .def("runLearningExperiment", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for running learning experiments
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Run a learning experiment.
            
            Args:
                brain: Brain to use
                steps: Number-regulated simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("runPlasticityExperiment", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for plasticity experiments
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Run a plasticity experiment.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("runStdpExperiment", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for STDP experiments
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Run a STDP experiment.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoBasicConnectivity", [](std::shared_ptr<Brain> brain) {
            // Convenience function for basic connectivity demo
            brain->reset();
            brain->initialize();
            
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    neurons[0]->injectCurrent(50.0f);
                }
            }
            
            for (int i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate basic neural connectivity.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoLearningProcess", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for learning process demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Demonstrate learning process.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPlasticity", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for plasticity demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Demonstrate plasticity.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoStdpProcess", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for STDP process demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"),
             R"pbdoc(Demonstrate STDP process.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPhase2", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 2 demo
            brain->reset();
            brain->initialize();
            
            // Run basic connectivity demo
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    neurons[0]->injectCurrent(50.0f);
                }
            }
            
            for (int i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate Phase 2 functionality.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPhase3", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 3 demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate Phase 3 functionality.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPhase4", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 4 demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate Phase 4 functionality.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPhase5", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 5 demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate Phase 5 functionality.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoPhase6", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 6 demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), R"pbdoc(Demonstrate Phase 6 functionality.
            
            Args:
                brain: Brain to use
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("demoBenchmark", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for benchmark demo
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return brain->produceAction();
        }, py::arg("brain"), py::arg("steps"), R"pbdoc(Demonstrate benchmark functionality.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                Action: Produced action
            )pbdoc")
        .def("getPhase2DemoResult", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 2 demo result
            brain->reset();
            brain->initialize();
            
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    neurons[0]->injectCurrent(50.0f);
                }
            }
            
            for (int i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), R"pbdoc(Get Phase 2 demo result.
            
            Args:
                brain: Brain to use
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getPhase3DemoResult", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 3 demo result
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), R"pbdoc(Get Phase 3 demo result.
            
            Args:
                brain: Brain to use
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getPhase4DemoResult", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 4 demo result
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), R"pbdoc(Get Phase 4 demo result.
            
            Args:
                brain: Brain to use
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getPhase5DemoResult", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 5 demo result
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), R"pbdoc(Get Phase 5 demo result.
            
            Args:
                brain: Brain to use
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getPhase6DemoResult", [](std::shared_ptr<Brain> brain) {
            // Convenience function for Phase 6 demo result
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), R"pbdoc(Get Phase 6 demo result.
            
            Args:
                brain: Brain to use
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getBenchmarkResult", [](std::shared_ptr<Brain> brain, int steps) {
            // Convenience function for benchmark result
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            return std::make_pair(brain->produceAction(), brain->getTotalSpikeCount());
        }, py::arg("brain"), py::arg("steps"), R"pbdoc(Get benchmark result.
            
            Args:
                brain: Brain to use
                steps: Number of simulation steps
                
            Returns:
                tuple: (action, spike_count)
            )pbdoc")
        .def("getVersionInfo", []() {
            return std::make_tuple("NLM (熙然) Neural Learning Machine", "Phase 6 - FINAL INTEGRATION", "Enhanced Python Bindings");
        }, R"pbdoc(Get version information."pbdoc")
        .def("getExampleConfig", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            config->set("connection_probability", 0.15);
            config->set("simulation_timestep", 0.001);
            config->set("random_seed", 42);
            return config;
        }, R"pbdoc(Get example configuration."pbdoc")
        .def("createBrainFromExample", []() {
            auto config = nlm.createDefaultConfig();
            return nlm.createBrain(config);
        }, R"pbdoc(Create brain from example configuration."pbdoc")
        .def("createSimpleWorldFromExample", []() {
            return nlm.createSimpleWorld();
        }, R"pbdoc(Create simple world from example."pbdoc")
        .def("createAgentBrainFromExample", []() {
            auto config = nlm.createDefaultConfig();
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, R"pbdoc(Create agent brain from example configuration."pbdoc"));

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface with enhanced capabilities.
        
        Connects NLM brain to world interaction with advanced features.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize agent brain with world.
            
            Args:
                world: World to connect to
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size."pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size."pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.
            
            Args:
                percept: Sensory percept to process
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command."pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.
            
            Args:
                reward: Actual reward received
                predictedReward: Predicted reward
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system.
            
            Args:
                timestep: Time step duration
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage."pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level."pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level."pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level."pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error."pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode."pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain."pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation."pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity."pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable/disable development."pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable/disable curiosity."pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled."pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled."pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled."pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled."pbdoc")
        .def("enableAll", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, R"pbdoc(Enable all neuromodulation systems."pbdoc")
        .def("disableAll", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, R"pbdoc(Disable all neuromodulation systems."pbdoc")
        .def("getNeuromodulationStatus", [](AgentBrain& self) {
            std::unordered_map<std::string, bool> status;
            status["reward"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, R"pbdoc(Get neuromodulation status."pbdoc")
        .def("getAgentMetrics", [](AgentBrain& self) {
            std::unordered_map<std::string, double> metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            return metrics;
        }, R"pbdoc(Get agent metrics."pbdoc")
        .def("createDefaultAgent", []() {
            auto config = nlm.createDefaultConfig();
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, R"pbdoc(Create default agent."pbdoc")
        .def("createLearningAgent", [](const std::unordered_map<std::string, double>& params) {
            auto config = nlm.createDefaultConfig();
            // Apply learning parameters
            if (params.count("learning_rate")) {
                config->set("learning_rate", params.at("learning_rate"));
            }
            if (params.count("exploration_rate")) {
                config->set("exploration_rate", params.at("exploration_rate"));
            }
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, py::arg("params"), R"pbdoc(Create learning agent with parameters."pbdoc")
        .def("createCuriosityAgent", [](const std::unordered_map<std::string, double>& params) {
            auto config = nlm.createDefaultConfig();
            // Apply curiosity parameters
            if (params.count("curiosity_weight")) {
                config->set("curiosity_weight", params.at("curiosity_weight"));
            }
            if (params.count("novelty_threshold")) {
                config->set("novelty_threshold", params.at("novelty_threshold"));
            }
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, py::arg("params"), R"pbdoc(Create curiosity agent with parameters."pbdoc")
        .def("createDevelopmentAgent", [](const std::unordered_map<std::string, double>& params) {
            auto config = nlm.createDefaultConfig();
            // Apply development parameters
            if (params.count("critical_period_length")) {
                config->set("critical_period_length", params.at("critical_period_length"));
            }
            if (params.count("maturation_rate")) {
                config->set("maturation_rate", params.at("maturation_rate"));
            }
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, py::arg("params"), R"pbdoc(Create development agent with parameters."pbdoc")
        .def("createRewardAgent", [](const std::unordered_map<std::string, double>& params) {
            auto config = nlm.createDefaultConfig();
            // Apply reward parameters
            if (params.count("reward_weight")) {
                config->set("reward_weight", params.at("reward_weight"));
            }
            if (params.count("prediction_error_weight")) {
                config->set("prediction_error_weight", params.at("prediction_error_weight"));
            }
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, py::arg("params"), R"pbdoc(Create reward agent with parameters."pbdoc")
        .def("createExplorationAgent", [](const std::unordered_map<std::string, double>& params) {
            auto config = nlm.createDefaultConfig();
            // Apply exploration parameters
            if (params.count("exploration_rate")) {
                config->set("exploration_rate", params.at("exploration_rate"));
            }
            if (params.count("entropy_bonus")) {
                config->set("entropy_bonus", params.at("entropy_bonus"));
            }
            auto brain = nlm.createBrain(config);
            return nlm.createAgentBrain(brain);
        }, py::arg("params"), R"pbdoc(Create exploration agent with parameters."pbdoc"));

    // === Convenience Functions for Common Use Cases ===

    m.def("runLearningExperiment", [](const std::shared_ptr<Brain>& brain, int steps) {
        // Run learning experiment
        brain->reset();
        brain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->produceAction();
    }, py::arg("brain"), py::arg("steps"),
         R"pbdoc(Run learning experiment.
        
        Args:
            brain: Brain to use
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runPlasticityExperiment", [](const std::shared_ptr<Brain>& brain, int steps) {
        // Run plasticity experiment
        brain->reset();
        brain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->produceAction();
    }, py::arg("brain"), py::arg("steps"),
         R"pbdoc(Run plasticity experiment.
        
        Args:
            brain: Brain to use
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runStdpExperiment", [](const std::shared_ptr<Brain>& brain, int steps) {
        // Run STDP experiment
        brain->reset();
        brain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->produceAction();
    }, py::arg("brain"), py::arg("steps"),
         R"pbdoc(Run STDP experiment.
        
        Args:
            brain: Brain to use
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runBasicConnectivityDemo", [](const std::shared_ptr<Brain>& brain) {
        // Run basic connectivity demo
        brain->reset();
        brain->initialize();
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
            }
        }
        
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->produceAction();
    }, py::arg("brain"),
         R"pbdoc(Run basic connectivity demo.
        
        Args:
            brain: Brain to use
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runDemo", [](const std::shared_ptr<Brain>& brain) {
        // Run complete demo
        brain->reset();
        brain->initialize();
        
        // Run basic connectivity demo
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
            }
        }
        
        for (int i = 0; i < 50; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return brain->produceAction();
    }, py::arg("brain"),
         R"pbdoc(Run complete demo.
        
        Args:
            brain: Brain to use
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create default configuration."pbdoc")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create brain with configuration."pbdoc")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create simple world."pbdoc")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create agent brain interface."pbdoc")

    // === Utility Functions ===

    m.def("getVersionInfo", []() {
        return std::make_tuple("NLM (熙然) Neural Learning Machine", "Phase mlb-6 - FINAL INTEGRATION", "Enhanced Python Bindings");
    }, R"pbdoc(Get version information."pbdoc")

    m.def("runLearningExperiment", [](int steps) {
        auto config = nlm.createDefaultConfig();
        auto brain = nlm.createBrain(config);
        return nlm.runLearningExperiment(brain, steps);
    }, py::arg("steps"), R"pbdoc(Run learning experiment with default brain.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runPlasticityExperiment", [](int steps) {
        auto config = nlm.createDefaultConfig();
        auto brain = nlm.createBrain(config);
        return nlm.runPlasticityExperiment(brain, steps);
    }, py::arg("steps"), R"pbdoc(Run plasticity experiment with default brain.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runStdpExperiment", [](int steps) {
        auto config = nlm.createDefaultConfig();
        auto brain = nlm.createBrain(config);
        return nlm.runStdpExperiment(brain, steps);
    }, py::arg("steps"), R"pbdoc(Run STDP experiment with default brain.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Action: Produced action
        )pbdoc")

    m.def("runDemo", []() {
        auto config = nlm.createDefaultConfig();
        auto brain = nlm.createBrain(config);
        return nlm.runDemo(brain);
    }, R"pbdoc(Run complete demo with default brain."pbdoc")

    m.def("getExampleConfig", []() {
        auto config = nlm.createDefaultConfig();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        return config;
    }, R"pbdoc(Get example configuration."pbdoc")

    m.def("createBrainFromExample", []() {
        auto config = nlm.getExampleConfig();
        return nlm.createBrain(config);
    }, R"pbdoc(Create brain from example configuration."pbdoc")

    m.def("createSimpleWorldFromExample", []() {
        return nlm.createSimpleWorld();
    }, R"pbdoc(Create simple world from example."pbdoc")

    m.def("createAgentBrainFromExample", []() {
        auto config = nlm.getExampleConfig();
        auto brain = nlm.createBrain(config);
        return nlm.createAgentBrain(brain);
    }, R"pbdoc(Create agent brain from example configuration."pbdoc")

    // === Error Handling and Validation ===

    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        return config->validate();
    }, py::arg("config"), R"pbdoc(Validate configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            list: List of validation error messages
        )pbdoc")

    m.def("isConfigValid", [](const std::shared_ptr<Config>& config) {
        return config->isValid();
    }, py::arg("config"), R"pbdoc(Check if configuration is valid.
        
        Args:
            config: Configuration to check
            
        Returns:
            bool: True if configuration is valid
        )pbdoc")

    m.def("getConfigErrors", [](const std::shared_ptr<Config>& config) {
        return config->validate();
    }, py::arg("config"), R"pbdoc(Get configuration errors.
        
        Args:
            config: Configuration to check
            
        Returns:
            list: List of error messages (empty if valid)
        )pbdoc")

    // === Legacy Compatibility ===

    // Maintain backward compatibility with original API
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    });

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"));

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    });

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"));

    // === Constants ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Neuron type constants
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse type constants
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Action type constants
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

    // Motor command constants
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // World object type constants
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Developmental stage constants
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing state constants
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
}

} // namespace nlm