#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "context_managers_impl.hpp"
#include "documentation.hpp"
#include "exceptions_impl.hpp"

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
        
        This module includes enhanced Pythonic API with context managers,
        comprehensive documentation, and improved error handling.
    )pbdoc";

    // Register enhanced Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::class_<PythonNLMError>(m, "PythonNLMError", py::module_local())
        .def(py::init<const std::string&>());
    py::class_<TypeError>(m, "TypeError", py::module_local())
        .def(py::init<const std::string&>());
    py::class_<ValueError>(m, "ValueError", py::module_local())
        .def(py::init<const std::string&>());
    py::class_<RuntimeError>(m, "RuntimeError", py::module_local())
        .def(py::init<const std::string&>());
    py::class_<InvalidOperationError>(m, "InvalidOperationError", py::module_local())
        .def(py::init<const std::string&>());
    py::class_<ValidationError>(m, "ValidationError", py::module_local())
        .def(py::init<const std::string&>());

    // Context manager classes for resource management
    py::class_<ConfigContextManager>(m, "ConfigContextManager",
        "Context manager for automatic Config resource management."
        " Ensures proper cleanup and prevents resource leaks.")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](ConfigContextManager& self, std::shared_ptr<Config> config) {
            return self.acquire(config).get();
        })
        .def("__exit__", [](ConfigContextManager& self, py::object) {
            self.release();
        })
        .def("isAcquired", &ConfigContextManager::isAcquired);

    py::class_<BrainContextManager>(m, "BrainContextManager",
        "Context manager for automatic Brain resource management."
        " Provides safe initialization and cleanup of brain resources.")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](BrainContextManager& self, std::shared_ptr<Brain> brain) {
            return self.acquire(brain).get();
        })
        .def("__exit__", [](BrainContextManager& self, py::object) {
            self.release();
        })
        .def("isAcquired", &BrainContextManager::isAcquired);

    py::class_<AgentBrainContextManager>(m, "AgentBrainContextManager",
        "Context manager for automatic AgentBrain resource management."
        " Ensures proper cleanup of agent brain resources.")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agent_brain"))
        .def("__enter__", [](AgentBrainContextManager& self, std::shared_ptr<AgentBrain> agent_brain) {
            return self.acquire(agent_brain).get();
        })
        .def("__exit__", [](AgentBrainContextManager& self, py::object) {
            self.release();
        })
        .def("isAcquired", &AgentBrainContextManager::isAcquired);

    py::class_<WorldContextManager>(m, "WorldContextManager",
        "Context manager for automatic SimpleWorld resource management."
        " Provides safe initialization and cleanup of world resources.")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("__enter__", [](WorldContextManager& self, std::shared_ptr<SimpleWorld> world) {
            return self.acquire(world).get();
        })
        .def("__exit__", [](WorldContextManager& self, py::object) {
            self.release();
        })
        .def("isAcquired", &WorldContextManager::isAcquired);

    // Enhanced Config class with Pythonic API and context manager support
    py::class_<Config>(m, "Config", Documentation::getConfigDocumentation())
        .def(py::init<>())
        .def(py::init<const Config&>(), py::arg("other"))
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on failure.")
        .def("set", [](Config& self, const std::string& key, const ConfigValue& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with type validation.")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value.")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value.")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration value.")
        .def("get", [](const Config& self, const std::string& key) {
            return safeExecute([&]() { return self.get<ConfigValue>(key); });
        }, py::arg("key"),
           "Get configuration value. Returns None if key doesn't exist.")
        .def("getOr", [](const Config& self, const std::string& key, const ConfigValue& defaultValue) {
            return self.getOr<ConfigValue>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value or return default if key doesn't exist.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("validateConfiguration", &Config::validateConfiguration,
             "Validate configuration. Returns True if valid, False otherwise.")
        .def("getValidationReport", &Config::getValidationReport,
             "Get validation report as string.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__eq__", [](const Config& a, const Config& b) {
            return a.summary() == b.summary();
        }, "Compare two configurations.")
        .def("clone", [](const Config& self) -> std::shared_ptr<Config> {
            return Cloner::clone(std::make_shared<Config>(self));
        }, "Create a copy of this configuration.")
        .def("copy", [](const Config& self) -> std::shared_ptr<Config> {
            return self.clone();
        }, "Create a copy of this configuration (alias for clone).")
        .def_property_readonly("keys", &Config::getKeys,
                              "Get all configuration keys (property).")
        .def("to_dict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    result[key] = py::cast(*value);
                }
            }
            return result;
        }, "Convert configuration to Python dictionary.")
        .def("from_dict", [](Config& self, const py::dict& d) {
            for (auto it : d) {
                std::string key = py::cast<std::string>(it.first);
                auto value = py::cast<ConfigValue>(it.second);
                self.set(key, value, ConfigSource::Runtime);
            }
        }, py::arg("dict"),
             "Update configuration from Python dictionary.")
        .def("to_json", [](const Config& self) {
            return Serializer::toJson(self);
        }, "Serialize configuration to JSON string.")
        .def("from_json", [](Config& self, const std::string& json) {
            return Serializer::fromJson(self, json);
        }, py::arg("json"),
             "Deserialize configuration from JSON string. Returns True on success.")
        .def("__enter__", [](Config& self) -> Config& {
            return self;
        })
        .def("__exit__", [](Config& self, py::object) {
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
    
    The SensoryInput class provides a base interface for all sensory modalities
    (vision, audio, internal signals) in the NLM framework.
    
    Properties:
        - Thread-safe timestamp management
        - Type-safe data access methods
        - Comprehensive dimension querying
    
    Usage examples:
    
    .. code-block:: python
    
        # Create vision sensory input
        vision = Vision(100, 100)
        
        # Set data
        vision.setData([float] * 10000)
        
        # Get properties
        print(f"Width: {vision.getWidth()}")
        print(f"Height: {vision.getHeight()}")
    
    Note:
        - All data is managed as float vectors for consistency
        - Timestamps are stored as microseconds since epoch
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def_property("timestamp", &SensoryInput::getTimestamp, &SensoryInput::setTimestamp,
                     "Get/set the timestamp (property)");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input
    
    The Vision class represents visual sensory input as grayscale intensity maps.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create vision with default parameters
        vision = Vision()
        
        # Create vision with specific dimensions
        vision = Vision(640, 480, 3)
        
        # Set image data
        data = [float] * (640 * 480 * 3)
        vision.setData(data)
        
        # Access properties
        print(f"Resolution: {vision.getWidth()}x{vision.getHeight()}")
        print(f"Channels: {vision.getChannels()}")
    
    Properties:
        - Channel count defaults to 3 (RGB)
        - Data is stored as interleaved float values
        - Memory efficient for large images
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def_property_readonly("width", &Vision::getWidth, "Image width in pixels (property)")
        .def_property_readonly("height", &Vision::getHeight, "Image height in pixels (property)")
        .def_property_readonly("channels", &Vision::getChannels, "Number of color channels (property)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
    
    The Audio class represents audio sensory input as sampled sound waveforms.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create audio with default parameters
        audio = Audio()
        
        # Create audio with specific sample rate and duration
        audio = Audio(44100, 1024)  # 44.1kHz, 1024 samples
        
        # Set audio samples
        samples = [float] * 1024
        audio.setData(samples)
        
        # Access audio properties
        print(f"Sample rate: {audio.getSampleRate()} Hz")
        print(f"Duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
    
    Properties:
        - Sample rate defaults to typical audio frequencies
        - Data stored as 32-bit float samples
        - Supports mono and stereo (interleaved) formats
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_property("sample_rate", &Audio::getSampleRate, &Audio::setSampleRate,
                     "Audio sample rate in Hz (property)")
        .def_property_readonly("num_samples", &Audio::getNumSamples,
                              "Number of audio samples (property)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
    
    The InternalSignals class represents neural and physiological signals
    from the agent's internal state and neuromodulation systems.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create internal signals
        signals = InternalSignals()
        
        # Add signal values
        signals.addSignal(0.5)  # Dopamine level
        signals.addSignal(0.3)  # Novelty level
        signals.addSignal(-0.2) # Prediction error
        
        # Access signals
        data = signals.getData()
        print(f"Number of signals: {len(data)}")
        
        # Clear signals for new episode
        signals.clearSignals()
    
    Properties:
        - Signals stored as floating-point values
        - Zero-based indexing for signal access
        - Efficient bulk operations for large signal sets
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
    
    The Action class represents motor commands and their associated parameters.
    Actions are the output of the neural system that drive agent behavior in the world.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create action from type only
        action = Action(ActionType.MoveForward)
        
        # Create action with parameters
        action = Action(ActionType.Custom, [0.5, 0.8, 1.2])
        
        # Access and modify action
        print(f"Action type: {action.getType()}")
        print(f"Action name: {action.getName()}")
        
        # Get/set parameters
        params = action.getParameters()
        action.setParameters([0.1, 0.9])
        
        # Clone action
        cloned_action = action.clone()
    
    Properties:
        - Parameter values are stored as float vectors
        - Action types are validated against ActionType enum
        - Serialization support for checkpointing
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def_property("type", &Action::getType, &Action::setType,
                     "Action type (property)")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Action parameters (property)");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
    
    The WorldObject class represents objects in the simulated world.
    Objects include resources, hazards, walls, and markers for the agent.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create empty world object
        obj = WorldObject()
        
        # Create resource object
        resource = WorldObject(x=10.0, y=20.0, type=WorldObjectType.Resource, value=5.0)
        
        # Create hazard object
        hazard = WorldObject(x=50.0, y=50.0, type=WorldObjectType.Hazard, value=-2.0)
        
        # Access object properties
        print(f"Position: ({obj.x}, {obj.y})")
        print(f"Type: {obj.type}")
        print(f"Active: {obj.active}")
    
    Properties:
        - Objects have position (x, y) and radius for collision detection
        - Each object has a type and value (reward/damage)
        - Active flag indicates whether object is currently present
    )pbdoc")
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state
    
    The AgentBody class represents the physical state of the agent in the world.
    This includes position, orientation, velocity, energy, and health metrics.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create agent body with default state
        body = AgentBody()
        
        # Create agent body with specific properties
        body = AgentBody()
        body.x = 10.0
        body.y = 20.0
        body.orientation = 45.0
        body.velocityX = 1.5
        body.velocityY = 0.5
        body.energy = 100.0
        body.health = 80.0
        
        # Access properties
        print(f"Position: ({body.x}, {body.y})")
        print(f"Orientation: {body.orientation} degrees")
        print(f"Energy: {body.energy}/{body.getMaxEnergy()}")
        
        # Check state
        print(f"Is moving: {body.isMoving}")
        print(f"Is turning: {body.isTurning}")
    
    Properties:
        - Position and orientation in world coordinates
        - Velocity components for physics simulation
        - Energy system with decay and regeneration
        - Health and age tracking
    )pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
    
    The ActionResult class contains the outcome of an agent's action in the world.
    This includes reward, success status, and descriptive message.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create successful action result
        result = ActionResult(reward=10.0, success=True, message="Collected resource")
        
        # Create failed action result
        result = ActionResult(reward=-5.0, success=False, message="Hit wall")
        
        # Create empty result
        result = ActionResult()
        
        # Access result properties
        print(f"Reward: {result.reward}")
        print(f"Success: {result.success}")
        print(f"Message: {result.message}")
    
    Properties:
        - Reward can be positive (good) or negative (bad)
        - Success boolean indicates if action completed as intended
        - Message provides human-readable description of outcome
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data
    
    The SensoryPercept class aggregates all sensory inputs from the current
    simulation timestep into a single percept vector for the agent brain.
    
    Usage examples:
    
    .. code-block:: python
    
        # Create sensory percept
        percept = SensoryPercept()
        
        # Set individual sensory modalities
        vision = Vision(100, 100)
        percept.setVision(vision)
        
        touch = InternalSignals()
        touch.addSignal(0.7)
        percept.setTouch(touch)
        
        # Get all signals as combined vector
        all_signals = percept.getAllSignals()
        print(f"Percept dimension: {len(all_signals)}")
        
        # Get individual components
        vision_data = percept.getVision()
        internal_data = percept.getInternal()
        
        # Set timestamp
        percept.setTimestamp(time.time() * 1000000)  # microseconds
    
    Properties:
        - Combines all sensory modalities into unified representation
        - Timestamp tracks when percept was generated
        - Efficient access to individual components
    )pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def_property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp,
                     "Percept timestamp (property)");

    py::class_<SimpleWorld>(m, "SimpleWorld", Documentation::getSimpleWorldDocumentation())
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def_property("width", &SimpleWorld::getWidth, nullptr,
                     "World width in simulation units (property)")
        .def_property("height", &SimpleWorld::getHeight, nullptr,
                     "World height in simulation units (property)")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Maximum energy (property)")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate (property)");

    py::class_<Brain>(m, "Brain", Documentation::getBrainDocumentation())
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
             "Log brain status")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getDevelopmentStage", &Brain::getDevelopmentalStage,
             "Get developmental stage")
        .def("setDevelopmentStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", Documentation::getAgentBrainDocumentation())
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
