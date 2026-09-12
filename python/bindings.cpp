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
        
        This module provides bindings for the NLM simulation framework, allowing
        users to create and interact with neural networks using Python.
        
        Key features:
        - Neural brain simulation with configurable plasticity rules
        - Sensory and motor processing capabilities  
        - Agent-based behavior with neuromodulation
        - World simulation for agent-environment interaction
        - Pythonic API with context managers and error handling
        
        Example usage:
        ```python
        import nlm
        
        # Create a default configuration
        config = nlm.createDefaultConfig()
        
        # Create a brain with the configuration
        brain = nlm.createBrain(config)
        
        # Create a world
        world = nlm.createSimpleWorld()
        
        # Create an agent brain interface
        agent_brain = nlm.createAgentBrain(brain)
        
        # Context manager support (where applicable)
        with world:
            # Perform simulation steps
            brain.step(current_step)
        ```
        
        For detailed documentation, visit: https://github.com/nlm-project/nlm-python-bindings
        
        Version: 1.0.0
        Author: NLM Development Team
        License: MIT
    )pbdoc";

    // Set module version
    m.attr("__version__") = "1.0.0";
    
    // Register runtime error exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Custom exception for configuration errors
    py::register_exception<std::invalid_argument>(m, "ConfigurationError");
    
    // Custom exception for brain/operation errors
    py::register_exception<std::logic_error>(m, "OperationError");

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

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        This class manages simulation parameters and settings for the NLM framework.
        Configuration is loaded from JSON files, command line arguments, or defaults.
        
        Attributes:
            Configuration keys are stored as string-value pairs.
            Common keys include: random_seed, neuron_count, simulation_timestep, etc.
        
        Example:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
            >>> config.get("neuron_count")
            1000
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises ConfigurationError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::cast<std::exception>(e);
            }
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Raises ConfigurationError on invalid args.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises ConfigurationError on write failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns bool.")
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value by key. Returns the stored value.")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value. Use get() to retrieve it later.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a vector of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", &Config::get, py::arg("key"),
             "Get configuration value using Python-style indexing: config['key']")
        .def("__setitem__", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value using Python-style assignment: config['key'] = value")
        .def("__contains__", &Config::has, py::arg("key"),
             "Check if configuration has a key using 'in' operator: 'key' in config");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Abstract base class for sensory input in the NLM system.
        
        Provides interface for various sensory modalities (vision, audio, internal signals).
        Each concrete implementation provides access to specific sensory data.
        
        Methods:
            getType(): Return the sensory input type (Vision, Audio, etc.)
            getData(): Return the raw sensory data as a vector
            getDimensions(): Return the dimensionality of the data
            getTimestamp(): Return the timestamp of the data
            setTimestamp(timestamp): Set the timestamp
        
        Example:
            >>> vision = Vision(width=80, height=60, channels=3)
            >>> vision.setData(pixel_data)
            >>> timestamp = vision.getTimestamp()
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input as string.")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats.")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality as tuple (width, height, ...).")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the data as double.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: " + std::to_string(input.getTimestamp()) + "s>";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing 2D image data.
        
        Provides access to visual information from the simulated environment.
        Data is stored as a flattened array of pixel values.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (1=grayscale, 3=RGB, 4=RGBA)
        
        Example:
            >>> vision = Vision(width=80, height=60, channels=3)
            >>> data = vision.getData()  # numpy array or list
            >>> vision.setData(new_data)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create Vision with specified dimensions.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Cannot set empty vision data");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Raises error if data is empty.")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels.")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels.")
        .def("getChannels", &Vision::getChannels, "Get number of color channels.")
        .def("getPixel", [](const Vision& self, size_t y, size_t x, size_t c) {
            if (y >= self.getHeight() || x >= self.getWidth() || c >= self.getChannels()) {
                throw std::out_of_range("Pixel coordinates out of bounds");
            }
            size_t idx = ((y * self.getWidth() + x) * self.getChannels()) + c;
            return self.getData()[idx];
        }, py::arg("y"), py::arg("x"), py::arg("c"), "Get pixel value at (x,y) for channel c (0-indexed).")
        .def("setPixel", [](Vision& self, size_t y, size_t x, size_t c, float value) {
            if (y >= self.getHeight() || x >= self.getWidth() || c >= self.getChannels()) {
                throw std::out_of_range("Pixel coordinates out of bounds");
            }
            size_t idx = ((y * self.getWidth() + x) * self.getChannels()) + c;
            const auto& data = self.getData();
            std::vector<float> newData = data;
            newData[idx] = value;
            self.setData(newData);
        }, py::arg("y"), py::arg("x"), py::arg("c"), py::arg("value"), "Set pixel value at (x,y) for channel c.");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Provides access to audio information from the simulated environment.
        Data is stored as a sequence of audio samples.
        
        Attributes:
            sampleRate: Audio sampling rate in Hz
            numSamples: Number of audio samples
        
        Example:
            >>> audio = Audio(sampleRate=44100, numSamples=1024)
            >>> samples = audio.getData()  # List of audio samples
            >>> audio.setData(new_samples)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio with specified sample rate and sample count.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Cannot set empty audio data");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Raises error if data is empty.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the audio sample rate in Hz.")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples.")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds.");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing neural state data.
        
        Provides access to internal neural state information (membrane potentials,
        firing rates, synaptic currents, etc.) from the brain simulation.
        
        Methods:
            addSignal(value): Add a new internal signal value
            clearSignals(): Clear all signal data
            getData(): Return all signals as a vector
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value. Value should be in physiological range.")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signal data.")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals stored.")
        .def("getAverage", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get the average value of all internal signals.");

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output in the NLM system.
        
        Represents an action that an agent can take in the simulated environment.
        Actions include movements (move forward, turn), sensory operations (look),
        and interactions (interact, eat, drink).
        
        Attributes:
            type: The action type (ActionType enum)
            parameters: Vector of float parameters (e.g., duration, intensity)
        
        Methods:
            getType(): Return the ActionType
            setType(type): Set the action type
            getParameters(): Return the parameter vector
            setParameters(params): Set the parameter vector
            getName(): Return human-readable action name
            clone(): Create a copy of this action
        
        Example:
            >>> action = Action(ActionType.MoveForward, [1.0])
            >>> action.getName()
            "MoveForward"
            >>> action.setParameters([2.0])
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with specified type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters.")
        .def("getType", &Action::getType, "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type.")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a vector.")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters.")
        .def("getName", &Action::getName, "Get the human-readable action name.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM simulation environment.
        
        Represents objects in the 2D world simulation (resources, hazards, walls, markers).
        Objects have position, type, and physical properties.
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            type: Object type (WorldObjectType enum)
            value: Object value (e.g., nutrition for food, danger for hazards)
            radius: Object radius for collision detection
            active: Whether the object is currently active
        
        Example:
            >>> obj = WorldObject(x=5.0, y=5.0, type=WorldObjectType.Resource, value=10.0)
            >>> obj.x
            5.0
            >>> obj.type.name
            "Resource"
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object at position with type and value.")
        .def_readwrite("x", &WorldObject::x, "X coordinate in world space.")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in world space.")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType).")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., nutrition, danger).")
        .def_readwrite("radius", &WorldObject::radius, "Object radius for collision detection.")
        .def_readwrite("active", &WorldObject::active, "Whether the object is currently active.")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point (x, y).")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (self.radius + other.radius);
        }, "Check if this object is colliding with another object.");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation in the NLM system.
        
        Represents the physical state of an agent in the simulated environment.
        Includes position, orientation, velocity, and physiological state.
        
        Attributes:
            x, y: Position coordinates in world space
            orientation: Heading angle in radians (0 = +X, π/2 = +Y)
            velocityX, velocityY: Linear velocity components
            angularVelocity: Angular velocity (radians per timestep)
            energy: Current energy level
            health: Current health level
            age: Agent age in simulation steps
            isMoving: Whether the agent is currently moving
            isTurning: Whether the agent is currently turning
            lastActionTime: Timestamp of last action taken
        
        Methods:
            reset(): Reset all state to initial values
            distanceTo(x, y): Calculate distance to world coordinates
        
        Example:
            >>> body = AgentBody()
            >>> body.x = 5.0
            >>> body.y = 5.0
            >>> body.setOrientation(math.pi/4)
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate in world space.")
        .def_readwrite("y", &AgentBody::y, "Y coordinate in world space.")
        .def("setOrientation", [](AgentBody& self, double angle) {
            self.orientation = angle;
            while (self.orientation < 0) self.orientation += 2 * M_PI;
            while (self.orientation >= 2 * M_PI) self.orientation -= 2 * M_PI;
        }, py::arg("angle"), "Set orientation angle in radians (0-2π).")
        .def("getOrientationDegrees", [](const AgentBody& self) {
            return self.orientation * 180.0 / M_PI;
        }, "Get orientation angle in degrees.")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component.")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component.")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity in rad/timestep.")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level.")
        .def_readwrite("health", &AgentBody::health, "Current health level.")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps.")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving.")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning.")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action.")
        .def("reset", &AgentBody::reset, "Reset all agent state to initial values.")
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point (x, y).")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Calculate current speed.")
        .def("getDirection", [](const AgentBody& self) {
            if (std::abs(self.velocityX) < 1e-10 && std::abs(self.velocityY) < 1e-10) {
                return self.orientation;
            }
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get current movement direction in radians.")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: (" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")>";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world interaction in the NLM system.
        
        Represents the outcome of an agent's action in the environment.
        Contains reward value, success status, and descriptive message.
        
        Attributes:
            reward: Numerical reward value (positive for good outcomes, negative for bad)
            success: Whether the action was successful
            message: Human-readable description of the outcome
        
        Example:
            >>> result = ActionResult(reward=10.0, success=True, message="Found food!")
            >>> result.reward
            10.0
            >>> result.success
            True
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message.")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value.")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful.")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about outcome.")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result represents a positive outcome.")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result represents a negative outcome.")
        .def("__repr__", [](const ActionResult& result) {
            std::string successStr = result.success ? "Success" : "Failed";
            return "<ActionResult: " + successStr + " (" + std::to_string(result.reward) + ")>";
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data from the NLM simulation environment.
        
        Contains the complete sensory input from the agent's perspective,
        including visual, tactile, internal, and audio information.
        
        Attributes (via getters):
            vision: Vision sensory input (2D image data)
            touch: Touch sensory input (contact detection)
            internal: Internal signals sensory input (neural state)
            proprioception: Proprioception sensory input (body state)
            audio: Audio sensory input (sound data)
            timestamp: Time when this percept was generated
        
        Methods:
            getVision(), setVision(percept): Get/set visual input
            getTouch(), setTouch(percept): Get/set tactile input
            getInternal(), setInternal(percept): Get/set internal signals
            getProprioception(), setProprioception(percept): Get/set proprioception
            getAudio(), setAudio(percept): Get/set audio input
            getAllSignals(): Get all sensory signals as dictionary
            getTimestamp(), setTimestamp(timestamp): Get/set timestamp
        
        Example:
            >>> percept = SensoryPercept()
            >>> vision = Vision(width=80, height=60)
            >>> percept.setVision(vision)
            >>> percept.getTimestamp()
            123.456
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get the vision sensory input. Returns Vision object.")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the vision sensory input. Takes Vision object.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the width of the visual field in pixels.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the height of the visual field in pixels.")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get the touch sensory input.")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the touch sensory input.")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get the internal signals sensory input.")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals sensory input.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get the proprioception sensory input.")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioception sensory input.")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get the audio sensory input.")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio sensory input.")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a dictionary-like structure.")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the timestamp of when this percept was generated.")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of when this percept was generated.")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: " + std::to_string(percept.getTimestamp()) + "s>";
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simulated environment where agents can interact.
        World contains agents, objects, and physics simulation.
        
        Methods:
            configure(width, height, visionWidth, visionHeight): Initialize world dimensions
            reset(): Reset world to initial state
            setAgentStart(x, y): Set starting position for agent
            update(timestep): Advance simulation by one timestep
            applyMotorCommand(cmd, currentTime): Apply agent motor command
            getSensoryPercept(): Get agent's current sensory input
            getAgentBody(): Get agent's body state
            addObject(obj): Add object to world
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid (not a wall)
            getWidth(), getHeight(): Get world dimensions
        
        Attributes (via methods):
            maxEnergy: Maximum agent energy capacity
            energyDecayRate: Rate at which energy decreases over time
            simulationTime: Current simulation time
            randomSeed: Random number generator seed
        
        Example:
            >>> world = SimpleWorld()
            >>> world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
            >>> world.setAgentStart(50, 50)
            >>> percept = world.getSensoryPercept()
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision parameters.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the starting position for the agent.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Advance the simulation by one timestep.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command from the agent to the world.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the agent's perspective.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's current body state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove the object at world coordinates (x, y).")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid (not occupied by a wall or hazard).")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in pixels.")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in pixels.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy capacity.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum agent energy capacity.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the rate at which energy decays over time.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the rate at which energy decays over time.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time in seconds.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random number generator seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the current random seed.")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " @ " +
                   std::to_string(world.getSimulationTime()) + "s>";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The core neural network simulation system in NLM.
        Manages neurons, synapses, regions, and the complete neural computation.
        
        Methods:
            initialize(): Initialize the brain with configuration
            step(currentStep): Perform a simulation step
            step(currentStep, currentTime): Perform a simulation step with timestamp
            receiveSensoryInput(input): Inject sensory input into the brain
            injectCurrent(neuron, current): Inject current into specific neuron
            injectCurrentToNeurons(type, current): Inject current into all neurons of a type
            produceAction(): Produce motor action based on neural activity
            reset(): Reset brain state
            save(filepath): Save brain state to file
            load(filepath): Load brain state from file
            addRegion(name): Add a new neural region
            getRegion(id): Get a region by ID
            getRegionCount(): Get the number of regions
            getRegionIds(): Get all region IDs
            getRegions(): Get all regions
            getTotalNeuronCount(): Get total neuron count
            getTotalSynapseCount(): Get total synapse count
            getActiveNeuronCount(): Get count of active neurons
            getFiringNeuronCount(): Get count of currently firing neurons
            getAverageFiringRate(): Get average firing rate
            getExcitationInhibitionRatio(): Get E/I balance
            getTotalSpikeCount(): Get total spike count
            getDevelopmentalStage(): Get current developmental stage
            setDevelopmentalStage(stage): Set developmental stage
            getConfig(): Get the configuration object
            logStatus(): Log brain status information
        
        Example:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.step(current_step)
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Raises OperationError if config is invalid.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Must be called before simulation.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step using default timestamp.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with specific timestamp.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Raises OperationError if neuron invalid.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset, "Reset brain state to initial values.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises OperationError on write failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises OperationError on read failure.")
        .def("addRegion", &Brain::addRegion, py::arg("name"),
             "Add a new neural region with specified name.")
        .def("getRegion", &Brain::getRegion,
             py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID. Raises OperationError if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as vector.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all neural regions as vector.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of neurons currently firing spikes.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (spikes per second).")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get the balance ratio of excitation to inhibition in the network.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total number of spikes generated by the brain.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get the current developmental stage of the brain.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set the developmental stage of the brain.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object associated with this brain.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status information to the configured output.")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + brain.getConfig().summary() + ">";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        High-level interface that processes sensory input and produces motor commands
        for agents in the simulated environment. Acts as the bridge between the
        low-level neural simulation and the agent's behavior.
        
        Methods:
            initialize(world): Initialize with the world object
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory input and inject to brain
            decodeMotorCommand(): Decode brain activity into motor command
            applyRewardModulation(reward, predictedReward): Apply reward-based learning
            updateDevelopment(timestep): Update developmental systems
            getDevelopmentalStage(): Get current developmental stage
            getNeuromodulationLevel(): Get current neuromodulation level
            getCuriosityLevel(): Get curiosity level
            getNoveltyLevel(): Get novelty level
            getPredictionError(): Get prediction error
            reset(): Reset agent for new episode
            getBrain(): Get the underlying brain object
            enableRewardModulation(enable): Enable/disable reward modulation
            enableStructuralPlasticity(enable): Enable/disable structural plasticity
            enableDevelopment(enable): Enable/disable development
            enableCuriosity(enable): Enable/disable curiosity
            isRewardModulationEnabled(): Check if reward modulation is enabled
            isStructuralPlasticityEnabled(): Check if structural plasticity is enabled
            isDevelopmentEnabled(): Check if development is enabled
            isCuriosityEnabled(): Check if curiosity is enabled
        
        Example:
            >>> brain = Brain(config)
            >>> world = SimpleWorld()
            >>> agent_brain = AgentBrain(brain)
            >>> agent_brain.initialize(world)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface for the given brain.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with the world simulation.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get the expected size of sensory input vectors.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get the size of expected motor output vectors.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input and inject into the brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain neural activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation to the brain.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update developmental systems based on simulation timestep.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get the current developmental stage of the agent.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get the current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get the current curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get the current novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get the current prediction error.")
        .def("reset", &AgentBrain::reset, "Reset the agent brain for a new episode.")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying Brain object.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable developmental processes.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + agent.getBrain()->getConfig().summary() + ">";
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
    }, "Create a default configuration for the NLM simulation",
    py::call_guard<py::gil_scoped_release>());

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), 
    "Create a new brain with the given configuration",
    py::call_guard<py::gil_scoped_release>());

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for simulation",
    py::call_guard<py::gil_scoped_release>());

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), 
    "Create a new agent brain interface for the given brain",
    py::call_guard<py::gil_scoped_release>());

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
