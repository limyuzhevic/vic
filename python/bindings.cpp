#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

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

// Custom Python exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError("Initialization error: " + msg) {}
};

// Register exceptions with Python
void register_exceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<InitializationError>(m, "InitializationError");
}

// Helper functions for convenience
class PythonHelpers {
public:
    // Convenience function to run a complete simulation loop
    static void run_simulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        uint64_t steps,
        bool verbose = false
    ) {
        if (!brain || !brain->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        
        if (agent && world) {
            agent->initialize(*world);
        }
        
        for (uint64_t step = 0; step < steps; ++step) {
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                brain->step(step);
                
                if (agent) {
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
            } else {
                brain->step(step);
            }
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
    }
    
    // Create a pre-configured brain with common settings
    static std::shared_ptr<Brain> createBrainWithDefaults(
        uint64_t neuronCount = 1000,
        uint64_t regionCount = 1,
        float connectionProbability = 0.1f,
        bool enableLearning = true,
        bool enableDevelopment = false
    ) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", connectionProbability, ConfigSource::Default);
        
        if (enableLearning) {
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        }
        
        if (enableDevelopment) {
            config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
            config->set("development_pruning_rate", 0.0001, ConfigSource::Default);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain with default configuration");
        }
        return brain;
    }
    
    // Run an agent in a world for a specified duration
    static std::vector<std::string> run_agent_episode(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        uint64_t steps,
        bool verbose = false
    ) {
        std::vector<std::string> logs;
        
        try {
            if (!brain || !brain->initialize()) {
                throw InitializationError("Brain initialization failed");
            }
            
            if (agent && world) {
                agent->initialize(*world);
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
                agent->enableDevelopment(true);
            }
            
            for (uint64_t step = 0; step < steps; ++step) {
                if (world) {
                    world->update(0.1);
                    auto percept = world->getSensoryPercept();
                    if (agent) {
                        agent->processSensoryInput(percept);
                    }
                    brain->step(step);
                    
                    if (agent) {
                        auto action = agent->decodeMotorCommand();
                        world->applyMotorCommand(action, world->getSimulationTime());
                        
                        auto reward = world->getSensoryPercept().getInternal()[0];
                        if (agent) {
                            agent->applyRewardModulation(reward, 0.0);
                            agent->updateDevelopment(0.1);
                        }
                    }
                } else {
                    brain->step(step);
                }
                
                if (verbose && step % 50 == 0) {
                    std::string log = "Step " + std::to_string(step) + 
                                    ": Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                    ", Reward: " + std::to_string(world ? world->getSensoryPercept().getInternal()[0] : 0.0);
                    logs.push_back(log);
                }
            }
        } catch (const std::exception& e) {
            logs.push_back(std::string("ERROR: ") + e.what());
        }
        
        return logs;
    }
    
    // Get comprehensive brain statistics
    static std::map<std::string, double> getBrainStats(const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        
        if (!brain) {
            stats["error"] = -1.0;
            return stats;
        }
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["firing_neuron_count"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
        
        return stats;
    }
    
    // Enhanced configuration helper
    static std::shared_ptr<Config> createConfigFromDict(const std::map<std::string, std::string>& configDict) {
        auto config = std::make_shared<Config>();
        
        for (const auto& pair : configDict) {
            try {
                // Try to parse as float first
                size_t pos;
                float floatVal = std::stof(pair.second, &pos);
                if (pos == pair.second.length()) {
                    config->set(pair.first, floatVal, ConfigSource::Default);
                } else {
                    // Try as int
                    size_t pos2;
                    int64_t intVal = std::stoll(pair.second, &pos2);
                    if (pos2 == pair.second.length()) {
                        config->set(pair.first, intVal, ConfigSource::Default);
                    } else {
                        // Try as bool
                        if (pair.second == "true") {
                            config->set(pair.first, true, ConfigSource::Default);
                        } else if (pair.second == "false") {
                            config->set(pair.first, false, ConfigSource::Default);
                        } else {
                            throw ConfigError("Cannot parse value '" + pair.second + "' for key '" + pair.first + "'");
                        }
                    }
                }
            } catch (const std::exception& e) {
                throw ConfigError("Failed to set configuration " + pair.first + ": " + e.what());
            }
        }
        
        return config;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to neural network simulation,
        learning, and development capabilities. It includes custom exception
        handling, convenience methods, and advanced features for both
        beginners and expert users.
        
        Key Features:
        - Custom Python exceptions (NLMError, ConfigError, SimulationError, InitializationError)
        - Convenience methods for common simulation patterns
        - Enhanced configuration management
        - Comprehensive brain statistics
        - Error handling with informative messages
        - Advanced simulation control methods
        
        Usage Examples:
        >>> import pynlm
        >>> # Create brain with default configuration
        >>> brain = pynlm.createBrainWithDefaults(500)
        >>> # Run simulation with error handling
        >>> pynlm.run_simulation(brain, world, agent, 1000, verbose=True)
        >>> # Get brain statistics
        >>> stats = pynlm.getBrainStats(brain)
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    
    // Original NeuronId bindings
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

    // Original SynapseId bindings (continued...)
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

    // Original enum bindings (for brevity, keeping original structure)
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Original Config class with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        Enhanced configuration management with validation and error handling.
        
        The Config class allows you to:
        - Load/save configurations from files
        - Set/Get configuration values
        - Validate configuration settings
        - Get configuration summaries
        
        Configuration values can be:
        - Integers (e.g., "neuron_count": 1000)
        - Floats (e.g., "connection_probability": 0.1)
        - Booleans (e.g., "plasticity.stdp.enable": true)
        
        Example:
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("brain.neuron_count", 500)
        >>> config.set("plasticity.stdp.enable", True)
        >>> config.set("neuromod.dopamine.scale", 1.5)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file)
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if successful, False if file not found or invalid
                 
             Raises:
                 ConfigError: If configuration parsing fails
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments)
           
           Args:
               argc: Number of command line arguments
               argv: Array of command line argument strings
               
           Returns:
               bool: True if successful, False if parsing fails
               
           Raises:
               ConfigError: If argument parsing fails
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)
             
             Args:
                 filepath: Path where to save the configuration
                 
             Returns:
                 bool: True if successful, False if write fails
                 
             Raises:
                 ConfigError: If file writing fails
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists)
             
             Args:
                 key: Configuration key (dot-separated path)
                 
             Returns:
                 bool: True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)
             
             Returns:
                 list: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)
             
             Returns:
                 bool: True if successful, False if operation fails
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration)
             
             Returns:
                 str: Human-readable configuration summary
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class with better error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        Enhanced sensory input class with improved error handling and validation.
        
        The SensoryInput class represents input from the environment:
        - Vision data (image-like input)
        - Audio data (sound input)
        - Internal signals (internal brain state)
        
        Each sensory modality provides:
        - Raw data access
        - Dimensional information
        - Timestamps
        - Type identification
        
        Attributes:
            type: Sensory input type
            dimensions: Input size/dimensions
            timestamp: Time of input
        
        Example:
        >>> vision = pynlm.Vision(80, 60, 3)
        >>> audio = pynlm.Audio(44100, 1024)
        >>> internal = pynlm.InternalSignals()
        >>> internal.addSignal(0.5)
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Original Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        Visual sensory input for representing images/frames.
        
        Vision input represents 2D visual information with multiple channels:
        - width: Image width in pixels
        - height: Image height in pixels  
        - channels: Number of color channels (typically 3 for RGB)
        
        Data should be provided as a flat vector of floats, row-major order:
        [R0,G0,B0, R1,G1,B1, ...]
        
        Example:
        >>> vision = pynlm.Vision(32, 32, 3)
        >>> data = [float(r)/255.0 for r in range(32*32*3)]
        >>> vision.setData(data)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set vision data)
        
        Args:
            data: Vector of float values (RGB in row-major order)
        )pbdoc")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Original Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        Audio sensory input for representing sound signals.
        
        Audio input represents time-series sound data:
        - sampleRate: Sampling rate in Hz (e.g., 44100)
        - numSamples: Number of audio samples
        
        Data should be provided as a vector of float values representing
        amplitude samples in the range [-1.0, 1.0].
        
        Example:
        >>> audio = pynlm.Audio(8000, 1024)
        >>> audio.setSampleRate(44100)
        >>> audio.setData(sine_wave_data)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data)
        
        Args:
            data: Vector of float values representing audio samples
        )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        Internal brain signals representing internal state information.
        
        Internal signals capture the brain's internal processes:
        - Reward prediction errors
        - Neuromodulatory signals
        - Internal homeostasis values
        - Cognitive states
        
        Signals are stored as a vector of float values, accessible as time-series data.
        
        Example:
        >>> internal = pynlm.InternalSignals()
        >>> internal.addSignal(0.7)  # Reward prediction
        >>> internal.addSignal(0.3)  # Novelty
        >>> signals = internal.getData()
        >>> len(signals)  # Number of internal signals
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals);

    // Original Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        Motor action for controlling the agent in the environment.
        
        Actions can represent various types of motor behaviors:
        - Movement actions (MoveForward, MoveBackward, etc.)
        - Perception actions (Look, LookUp, etc.)
        - Interaction actions (Interact, Eat, etc.)
        - Custom actions
        
        Each action has:
        - type: The action type (from ActionType enum)
        - parameters: Additional action-specific parameters
        - name: Human-readable action name
        
        Actions can be cloned for use in different contexts.
        
        Example:
        >>> move_action = pynlm.Action(pynlm.ActionType.MoveForward)
        >>> turn_action = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
        >>> interact_action = pynlm.Action(pynlm.ActionType.Interact)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Original WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        Object in the simulation world with physical properties.
        
        World objects represent things in the environment:
        - Resources (food, energy, etc.)
        - Hazards (dangerous objects)
        - Walls (boundaries)
        - Markers (landmarks)
        
        Each object has:
        - Position (x, y coordinates)
        - Type (from WorldObjectType enum)
        - Value (functional value)
        - Radius (physical size)
        - Active state
        
        Objects can be added to/removed from the world.
        
        Example:
        >>> resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        >>> hazard = pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Hazard, radius=0.8)
        >>> wall = pynlm.WorldObject(0.0, 10.0, pynlm.WorldObjectType.Wall, radius=0.2)
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

    // Original AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        Physical state of the agent in the world.
        
        The agent body tracks:
        - Position (x, y coordinates)
        - Orientation (heading angle)
        - Velocity (linear and angular)
        - Physical properties (energy, health, age)
        - Movement state (isMoving, isTurning)
        - Timing information
        
        The body can be reset to initial state.
        
        Example:
        >>> body = pynlm.AgentBody()
        >>> body.x = 5.0
        >>> body.y = 5.0
        >>> body.orientation = 0.0
        >>> body.velocityX = 1.0
        >>> body.energy = 100.0
        >>> body.health = 1.0
        >>> body.age = 0
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

    // Original ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        Result of an agent's action in the environment.
        
        Action results capture the outcome of actions:
        - Reward (how good the outcome was)
        - Success (whether action completed successfully)
        - Message (descriptive text about the outcome)
        
        Action results are used for:
        - Reward modulation
        - Learning updates
        - Agent behavior adaptation
        
        Example:
        >>> result = pynlm.ActionResult(reward=1.0, success=True, message="Collected food")
        >>> print(f"Reward: {result.reward}, Success: {result.success}")
        >>> another_result = pynlm.ActionResult(reward=-0.5, success=False, message="Hit wall")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Original SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        Processed sensory input for the agent.
        
        A sensory percept contains all the information the agent receives
        from the environment:
        - Vision: Visual input (image data)
        - Touch: Tactile contact information
        - Proprioception: Body position/orientation
        - Audio: Auditory input
        - Internal: Internal brain state signals
        
        Percepts are timestamped and can be modified:
        - setVision(), setTouch(), etc.
        - getVision(), getTouch(), etc.
        
        The getAllSignals() method provides access to all sensory modalities
        as a single vector for convenience.
        
        Example:
        >>> percept = world.getSensoryPercept()
        >>> vision = percept.getVision()
        >>> touch = percept.getTouch()
        >>> audio = percept.getAudio()
        >>> internal = percept.getInternal()
        >>> all_signals = percept.getAllSignals()
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Original SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        A simple 2D grid-based world for agent simulation.
        
        The world provides:
        - Spatial environment (width x height)
        - Vision sensing (visionWidth x visionHeight)
        - Agent interaction capabilities
        - Physics simulation
        - Object management
        
        The world supports:
        - Configuration (dimensions, vision range)
        - Reset (return to initial state)
        - Updates (time evolution)
        - Motor commands (agent actions)
        - Sensory perception (what agent sees)
        - Object management (add/remove)
        
        Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        >>> world.reset()
        >>> world.setAgentStart(10.0, 10.0)
        >>> world.update(0.1)  # Update world by 0.1 time units
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions and vision range)
             
             Args:
                 width: World width in arbitrary units
                 height: World height in arbitrary units
                 visionWidth: Vision range along x-axis
                 visionHeight: Vision range along y-axis
             )pbdoc")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state by timestep duration)
             
             Args:
                 timestep: Time increment in seconds
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to world)
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get sensory percept for current state)
             
             Returns:
                 SensoryPercept: Current sensory information
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state)
             
             Returns:
                 AgentBody: Current agent physical state
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Original Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        The central neural simulation engine implementing spiking neural networks.
        
        The Brain class orchestrates all neural computation:
        - Real spiking neural dynamics (LIF neurons)
        - Event-driven spike propagation
        - Synaptic plasticity (STDP, Hebbian)
        - Neuromodulation
        - Memory systems
        - Prediction and cognition
        
        Key methods:
        - initialize(): Start neural computation
        - step(): Advance simulation by one time step
        - receiveSensoryInput(): Inject sensory information
        - produceAction(): Generate motor output
        - save/load(): Persistent state management
        
        The brain operates in real-time with event-driven updates.
        
        Example:
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> for step in range(1000):
        ...     brain.step(step)
        ...     action = brain.produceAction()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration)
             
             Returns:
                 bool: True if successful, False if initialization fails
                 
             Raises:
                 InitializationError: If initialization fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step)
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp)
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain)
             
             Args:
                 input: Sensory input data
                 
             Raises:
                 SimulationError: If input processing fails
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity)
             
             Returns:
                 Action: Motor action derived from neural activity
             )pbdoc")
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

    // Original AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        Interface between the NLM brain and the external world.
        
        The AgentBrain provides:
        - Sensory processing (converting world percepts to brain input)
        - Motor decoding (converting brain activity to actions)
        - Neuromodulation (reward, curiosity, novelty)
        - Development (brain maturation over time)
        - Subsystem control (enable/disable learning mechanisms)
        
        The agent brain bridges the gap between neural computation
        and real-world interaction, implementing the full brain-loop
        cycle: sense → process → act → learn.
        
        Example:
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> percept = world.getSensoryPercept()
        >>> agent.processSensoryInput(percept)
        >>> action = agent.decodeMotorCommand()
        >>> world.applyMotorCommand(action, time)
    )pbdoc")
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Original factory functions
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

    // Python helper functions
    m.def("run_simulation", &PythonHelpers::run_simulation,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("steps"), py::arg("verbose") = false,
          R"pbdoc(Run a complete agent simulation loop)
          
          This helper function runs the standard agent simulation loop:
          - Update world
          - Get sensory input from world
          - Process input through agent
          - Run brain step
          - Decode action from agent
          - Apply action to world
          
          Args:
              brain: The NLM brain
              world: The simulation world
              agent: The agent brain interface
              steps: Number of simulation steps to run
              verbose: If True, print progress messages
              
          Raises:
              InitializationError: If brain initialization fails
              NLMError: For other simulation errors
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> world = pynlm.createSimpleWorld()
          >>> agent = pynlm.createAgentBrain(brain)
          >>> pynlm.run_simulation(brain, world, agent, 1000, verbose=True)
          )pbdoc");

    m.def("createBrainWithDefaults", &PythonHelpers::createBrainWithDefaults,
          py::arg("neuronCount") = 1000,
          py::arg("regionCount") = 1,
          py::arg("connectionProbability") = 0.1f,
          py::arg("enableLearning") = true,
          py::arg("enableDevelopment") = false,
          R"pbdoc(Create brain with default configuration)
          
          This convenience function creates a brain with commonly used
          default settings, saving you from manually configuring
          every parameter.
          
          Args:
              neuronCount: Number of neurons (default: 1000)
              regionCount: Number of brain regions (default: 1)
              connectionProbability: Synaptic connectivity (default: 0.1)
              enableLearning: Enable plasticity mechanisms (default: true)
              enableDevelopment: Enable developmental processes (default: false)
              
          Returns:
              Brain: Configured and initialized brain
              
          Raises:
              InitializationError: If brain initialization fails
              
          Example:
          >>> brain = pynlm.createBrainWithDefaults(neuronCount=500, regionCount=2)
          >>> brain2 = pynlm.createBrainWithDefaults(enableDevelopment=True)
          )pbdoc");

    m.def("run_agent_episode", &PythonHelpers::run_agent_episode,
          py::arg("brain"), py::arg("world"), py::arg("agent"),
          py::arg("steps"), py::arg("verbose") = false,
          R"pbdoc(Run an agent episode with logging)
          
          Run a complete agent episode and capture logs of the simulation.
          This is useful for debugging and understanding agent behavior.
          
          Args:
              brain: The NLM brain
              world: The simulation world
              agent: The agent brain interface
              steps: Number of simulation steps to run
              verbose: If True, print progress messages
              
          Returns:
              list: Log messages from the simulation
              
          Raises:
              InitializationError: If brain initialization fails
              NLMError: For other simulation errors
              
          Example:
          >>> logs = pynlm.run_agent_episode(brain, world, agent, 500, verbose=True)
          >>> for log in logs:
          ...     print(log)
          )pbdoc");

    m.def("getBrainStats", &PythonHelpers::getBrainStats,
          py::arg("brain"),
          R"pbdoc(Get comprehensive brain statistics)
          
          Extract detailed statistics from a brain for analysis and debugging.
          
          Args:
              brain: The NLM brain
              
          Returns:
              dict: Dictionary of brain statistics including:
                - total_neurons: Total neuron count
                - total_synapses: Total synapse count
                - firing_neurons: Number of currently firing neurons
                - active_neurons: Number of active neurons
                - total_spikes: Total spike count
                - average_firing_rate: Average firing rate (Hz)
                - excitatory_inhibition_ratio: E/I balance
                - developmental_stage: Current developmental stage
                
          Example:
          >>> stats = pynlm.getBrainStats(brain)
          >>> print(f"Firing neurons: {stats['firing_neurons']}")
          >>> print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
          )pbdoc");

    m.def("createConfigFromDict", &PythonHelpers::createConfigFromDict,
          py::arg("configDict"),
          R"pbdoc(Create configuration from Python dictionary)
          
          Create an NLM configuration object from a Python dictionary,
          providing a convenient Pythonic way to configure the brain.
          
          Args:
              configDict: Dictionary with configuration keys and values.
                         Keys are configuration paths (e.g., "brain.neuron_count").
                         Values can be integers, floats, or booleans.
          
          Returns:
              Config: Configured configuration object
              
          Raises:
              ConfigError: If configuration values cannot be parsed
              
          Example:
          >>> config_dict = {
          ...     "brain.neuron_count": 2000,
          ...     "brain.synapse_density": 0.05,
          ...     "plasticity.stdp.enable": True,
          ...     "neuromod.curiosity.enable": True
          ... }
          >>> config = pynlm.createConfigFromDict(config_dict)
          )pbdoc");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Advanced features for expert users
    m.def("neural_network_explorer", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, py::object> exploration;
        
        // Get network topology
        exploration["total_neurons"] = brain->getTotalNeuronCount();
        exploration["total_synapses"] = brain->getTotalSynapseCount();
        exploration["firing_neurons"] = brain->getFiringNeuronCount();
        
        // Get neuron statistics by type
        std::map<std::string, size_t> neuronCounts;
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            for (const auto& neuron : region->getNeurons()) {
                std::string type;
                switch (neuron->getType()) {
                    case NeuronType::Excitatory: type = "excitatory"; break;
                    case NeuronType::Inhibitory: type = "inhibitory"; break;
                    case NeuronType::Modulatory: type = "modulatory"; break;
                    case NeuronType::Sensory: type = "sensory"; break;
                    case NeuronType::Motor: type = "motor"; break;
                    case NeuronType::Internal: type = "internal"; break;
                }
                neuronCounts[type]++;
            }
        }
        
        py::dict typeCounts;
        for (const auto& pair : neuronCounts) {
            typeCounts[py::str(pair.first)] = pair.second;
        }
        exploration["neuron_types"] = typeCounts;
        
        // Get connectivity statistics
        float totalWeight = 0.0f;
        float maxWeight = 0.0f;
        size_t connectedSynapses = 0;
        
        for (const auto& region : regions) {
            for (const auto& synapse : region->getSynapses()) {
                totalWeight += synapse->getWeight();
                if (synapse->getWeight() > maxWeight) {
                    maxWeight = synapse->getWeight();
                }
                if (synapse->getWeight() > 0.01f) {
                    connectedSynapses++;
                }
            }
        }
        
        exploration["total_synaptic_weight"] = totalWeight;
        exploration["max_synaptic_weight"] = maxWeight;
        exploration["connected_synapses"] = static_cast<double>(connectedSynapses);
        
        // Get activity statistics
        exploration["average_firing_rate"] = brain->getAverageFiringRate();
        exploration["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        exploration["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        // Get developmental information
        exploration["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return exploration;
    }, py::arg("brain"),
          R"pbdoc(Explore the neural network structure and statistics)
          
          Perform comprehensive analysis of the neural network topology,
          connectivity, and activity patterns. Useful for network visualization,
          debugging, and understanding network properties.
          
          Args:
              brain: The NLM brain to analyze
              
          Returns:
              dict: Dictionary containing:
                - total_neurons: Total neuron count
                - total_synapses: Total synapse count  
                - firing_neurons: Number of currently firing neurons
                - neuron_types: Dict with neuron type counts
                - total_synaptic_weight: Sum of all synaptic weights
                - max_synaptic_weight: Maximum synaptic weight
                - connected_synapses: Number of synapses with weight > 0.01
                - average_firing_rate: Average firing rate (Hz)
                - excitatory_inhibition_ratio: E/I balance ratio
                - total_spikes: Total spike count
                - developmental_stage: Current developmental stage (0-4)
                
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> exploration = pynlm.neural_network_explorer(brain)
          >>> print(f"Network has {exploration['total_neurons']} neurons")
          >>> print(f"E/I ratio: {exploration['excitatory_inhibition_ratio']:.3f}")
          >>> for neuron_type, count in exploration['neuron_types'].items():
          ...     print(f"{neuron_type}: {count} neurons")
          )pbdoc");

    m.def("save_brain_checkpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                     bool compress = false, bool validate = true) {
        if (!brain) {
            throw SimulationError("Cannot save null brain");
        }
        
        // Validate checkpoint path
        if (validate) {
            if (filepath.empty()) {
                throw ConfigError("Checkpoint filepath cannot be empty");
            }
            if (filepath.find('/') == std::string::npos && filepath.find('\\') == std::string::npos) {
                std::cout << "Warning: Checkpoint file '" << filepath << "' is in current directory" << std::endl;
            }
        }
        
        // Attempt to save with error handling
        try {
            if (!brain->save(filepath)) {
                throw SimulationError("Failed to save brain checkpoint to '" + filepath + "'");
            }
            
            // Optionally compress (this is a placeholder for actual compression)
            if (compress) {
                std::cout << "Warning: Compression not implemented yet for '" << filepath << "'" << std::endl;
            }
            
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Error saving checkpoint: ") + e.what());
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("compress") = false, py::arg("validate") = true,
          R"pbdoc(Save brain checkpoint with error handling and validation)
          
          Save the current brain state to a file for later restoration.
          Includes validation and error handling for robust checkpointing.
          
          Args:
              brain: The NLM brain to save
              filepath: Path where to save the checkpoint
              compress: If True, attempt to compress the checkpoint file
              validate: If True, validate file path before saving
              
          Returns:
              bool: True if successful
              
          Raises:
              ConfigError: If filepath validation fails
              SimulationError: If save operation fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> success = pynlm.save_brain_checkpoint(brain, "./checkpoints/brain_state.bin")
          >>> if success:
          ...     print("Checkpoint saved successfully")
          )pbdoc");

    m.def("load_brain_checkpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                     bool validate = true) {
        if (!brain) {
            throw SimulationError("Cannot load to null brain");
        }
        
        // Validate checkpoint path
        if (validate) {
            if (filepath.empty()) {
                throw ConfigError("Checkpoint filepath cannot be empty");
            }
        }
        
        // Attempt to load with error handling
        try {
            if (!brain->load(filepath)) {
                throw SimulationError("Failed to load brain checkpoint from '" + filepath + "'");
            }
            return true;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Error loading checkpoint: ") + e.what());
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("validate") = true,
          R"pbdoc(Load brain checkpoint with error handling and validation)
          
          Load a previously saved brain state from a file.
          Includes validation and error handling for robust checkpointing.
          
          Args:
              brain: The NLM brain to load into
              filepath: Path to the checkpoint file to load
              validate: If True, validate file path before loading
              
          Returns:
              bool: True if successful
              
          Raises:
              ConfigError: If filepath validation fails
              SimulationError: If load operation fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> success = pynlm.load_brain_checkpoint(brain, "./checkpoints/brain_state.bin")
          >>> if success:
          ...     print("Checkpoint loaded successfully")
          >>> # Verify brain is working
          >>> brain.step(0)
          )pbdoc");

    m.def("benchmark_brain_performance", [](std::shared_ptr<Brain> brain, uint64_t steps,
                                            bool warmup = true, bool detailed = false) {
        if (!brain) {
            throw SimulationError("Cannot benchmark null brain");
        }
        
        std::map<std::string, double> results;
        
        try {
            if (warmup) {
                // Warm-up phase
                std::cout << "Warming up brain..." << std::endl;
                for (uint64_t i = 0; i < std::min<uint64_t>(steps / 10, 1000); ++i) {
                    brain->step(i, i * 0.001);
                }
            }
            
            // Timing the benchmark
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (uint64_t i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end_time - start_time).count();
            
            results["total_steps"] = static_cast<double>(steps);
            results["total_time_seconds"] = duration;
            results["steps_per_second"] = steps / duration;
            results["simulation_time_per_step_ms"] = (duration / steps) * 1000.0;
            
            if (detailed) {
                // Get detailed statistics
                results["final_neurons_firing"] = static_cast<double>(brain->getFiringNeuronCount());
                results["total_spikes_generated"] = static_cast<double>(brain->getTotalSpikeCount());
                results["average_firing_rate"] = brain->getAverageFiringRate();
                results["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                
                // Performance metrics
                double ops_per_second = steps * (brain->getTotalNeuronCount() + brain->getTotalSynapseCount()) / duration;
                results["estimated_ops_per_second"] = ops_per_second;
            }
            
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Benchmark failed: ") + e.what());
        }
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("warmup") = true, py::arg("detailed") = false,
          R"pbdoc(Benchmark brain performance with detailed metrics)
          
          Run a performance benchmark measuring simulation speed and efficiency.
          Useful for comparing different configurations and measuring improvements.
          
          Args:
              brain: The NLM brain to benchmark
              steps: Number of simulation steps to run
              warmup: If True, run a warm-up phase to stabilize performance
              detailed: If True, collect additional detailed statistics
              
          Returns:
              dict: Performance metrics including:
                - total_steps: Number of simulation steps run
                - total_time_seconds: Total benchmark time in seconds
                - steps_per_second: Simulation speed (steps/s)
                - simulation_time_per_step_ms: Time per step (ms)
                - final_neurons_firing: Final firing neuron count (if detailed)
                - total_spikes_generated: Total spike count (if detailed)
                - average_firing_rate: Average firing rate (if detailed)
                - excitatory_inhibition_ratio: E/I balance (if detailed)
                - estimated_ops_per_second: Estimated operations per second (if detailed)
                
          Raises:
              SimulationError: If benchmark fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults(500)  # Smaller brain for faster benchmark
          >>> results = pynlm.benchmark_brain_performance(brain, 1000, detailed=True)
          >>> print(f"Speed: {results['steps_per_second']:.0f} steps/second")
          >>> print(f"Time per step: {results['simulation_time_per_step_ms']:.3f} ms")
          )pbdoc");

    m.def("create_advanced_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    const std::string& strategy = "default") {
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (world) {
            agent->initialize(*world);
            
            // Apply strategy-specific configuration
            if (strategy == "exploration") {
                agent->enableCuriosity(true);
                agent->enableRewardModulation(true);
                agent->enableStructuralPlasticity(false);  // Less risky exploration
            } else if (strategy == "exploitation") {
                agent->enableCuriosity(false);  // Focus on known rewards
                agent->enableRewardModulation(true);
                agent->enableDevelopment(false);  // Stable, mature agent
            } else if (strategy == "balanced") {
                agent->enableCuriosity(true);
                agent->enableRewardModulation(true);
                agent->enableDevelopment(true);
            } else if (strategy == "curiosity") {
                agent->enableCuriosity(true);
                agent->enableRewardModulation(false);  // Focus on exploration
                agent->enableDevelopment(true);
            } else if (strategy == "creative") {
                agent->enableCuriosity(true);
                agent->enableRewardModulation(true);
                agent->enableDevelopment(true);
                agent->enableStructuralPlasticity(true);  // Maximum adaptability
            }
            // "default" strategy applies no special configuration
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("strategy") = "default",
          R"pbdoc(Create an agent with strategy-specific configuration)
          
          Create an agent brain with predefined behavioral strategies.
          Different strategies configure neuromodulation and plasticity
          subsystems to produce different behavioral profiles.
          
          Args:
              brain: The NLM brain to control
              world: The simulation world (optional, can be set later)
              strategy: Behavioral strategy to apply. Options:
                - "default": Standard configuration
                - "exploration": Curiosity-driven exploration
                - "exploitation": Reward-focused exploitation  
                - "balanced": Balanced exploration/exploitation
                - "curiosity": Pure curiosity-driven behavior
                - "creative": Maximum adaptability and creativity
              
          Returns:
              AgentBrain: Configured agent brain interface
              
          Raises:
              InitializationError: If agent initialization fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> world = pynlm.createSimpleWorld()
          >>> world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
          >>> 
          >>> # Create different types of agents
          >>> curious_agent = pynlm.create_advanced_agent(brain, world, "curiosity")
          >>> balanced_agent = pynlm.create_advanced_agent(brain, world, "balanced")
          >>> creative_agent = pynlm.create_advanced_agent(brain, world, "creative")
          )pbdoc");

    m.def("simulate_to_development_stage", [](std::shared_ptr<Brain> brain,
                                            std::shared_ptr<SimpleWorld> world,
                                            std::shared_ptr<AgentBrain> agent,
                                            DevelopmentalStage targetStage,
                                            uint64_t maxSteps = 10000) {
        if (!brain) {
            throw InitializationError("Cannot simulate null brain");
        }
        
        if (!brain->initialize()) {
            throw InitializationError("Brain initialization failed");
        }
        
        std::vector<std::string> logs;
        logs.push_back(std::string("Starting simulation to reach developmental stage: ") + 
                      std::to_string(static_cast<int>(targetStage)));
        
        for (uint64_t step = 0; step < maxSteps; ++step) {
            // Update world if provided
            if (world) {
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process through agent if provided
                if (agent) {
                    agent->processSensoryInput(percept);
                    
                    // Apply reward modulation
                    auto internal = percept.getInternal();
                    float reward = internal.size() > 0 ? internal[0] : 0.0f;
                    agent->applyRewardModulation(reward, 0.0);
                    
                    // Update development
                    agent->updateDevelopment(0.1);
                }
                
                // Run brain step
                brain->step(step);
                
                // Check if target developmental stage reached
                if (agent && agent->getDevelopmentalStage() >= targetStage) {
                    logs.push_back(std::string("Target developmental stage reached at step ") + std::to_string(step));
                    break;
                }
            } else {
                brain->step(step);
            }
            
            // Log progress periodically
            if (step % 1000 == 0) {
                logs.push_back(std::string("Step ") + std::to_string(step) +
                              std::string(", Dev stage: ") + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
            }
        }
        
        return logs;
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
          py::arg("targetStage") = DevelopmentalStage::Adult,
          py::arg("maxSteps") = 10000,
          R"pbdoc(Simulate until reaching a target developmental stage)
          
          Run a simulation that progresses through developmental stages
          until reaching the specified target stage.
          
          Args:
              brain: The NLM brain to simulate
              world: The simulation world (optional)
              agent: The agent brain interface (optional)
              targetStage: Target developmental stage to reach
              maxSteps: Maximum simulation steps to attempt
              
          Returns:
              list: Log messages from the simulation
              
          Raises:
              InitializationError: If brain initialization fails
              SimulationError: If simulation fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults(enableDevelopment=True)
          >>> world = pynlm.createSimpleWorld()
 >>> agent = pynlm.create_advanced_agent(brain, world, "balanced")
          >>> logs = pynlm.simulate_to_development_stage(brain, world, agent, pynlm.DevelopmentalStage.Maturation, maxSteps=5000)
          >>> for log in logs:
          ...     print(log)
          )pbdoc");

    m.def("create_brain_from_template", [](const std::string& templateName,
                                          const std::map<std::string, std::string>& parameters) {
        auto config = std::make_shared<Config>();
        
        // Apply template configuration
        if (templateName == "small") {
            config->set("brain.neuron_count", 200, ConfigSource::Default);
            config->set("brain.synapse_density", 0.05, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        } else if (templateName == "medium") {
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("brain.synapse_density", 0.1, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        } else if (templateName == "large") {
            config->set("brain.neuron_count", 5000, ConfigSource::Default);
            config->set("brain.synapse_density", 0.15, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            config->set("neuromod.dopamine.scale", 1.5, ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
        } else if (templateName == "learning") {
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
        } else if (templateName == "stable") {
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("brain.synapse_density", 0.05, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", false, ConfigSource::Default);  // Less plasticity
            config->set("neuromod.dopamine.scale", 0.5, ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.0001, ConfigSource::Default);  // Slow development
        } else {
            throw ConfigError("Unknown brain template: '" + templateName + "'");
        }
        
        // Apply parameter overrides
        for (const auto& pair : parameters) {
            try {
                size_t pos;
                float floatVal = std::stof(pair.second, &pos);
                if (pos == pair.second.length()) {
                    config->set(pair.first, floatVal, ConfigSource::Default);
                } else {
                    size_t pos2;
                    int64_t intVal = std::stoll(pair.second, &pos2);
                    if (pos2 == pair.second.length()) {
                        config->set(pair.first, intVal, ConfigSource::Default);
                    } else {
                        if (pair.second == "true") {
                            config->set(pair.first, true, ConfigSource::Default);
                        } else if (pair.second == "false") {
                            config->set(pair.first, false, ConfigSource::Default);
                        } else {
                            throw ConfigError("Cannot parse parameter '" + pair.second + "' for '" + pair.first + "'");
                        }
                    }
                }
            } catch (const std::exception& e) {
                throw ConfigError("Failed to apply parameter '" + pair.first + "': " + e.what());
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain from template '" + templateName + "'");
        }
        
        return brain;
    }, py::arg("templateName"), py::arg("parameters") = std::map<std::string, std::string>(),
          R"pbdoc(Create brain from predefined template with optional parameter overrides)
          
          Create a brain using predefined configuration templates that represent
          different use cases and performance characteristics. Templates can be
          customized by overriding specific parameters.
          
          Args:
              templateName: Name of the template to use. Options:
                - "small": Small, fast brain for testing/debugging
                - "medium": Balanced brain for general use
                - "large": Large brain for high-fidelity simulation
                - "learning": Brain optimized for learning and adaptation
                - "stable": Brain optimized for stability and consistency
              parameters: Optional dictionary of configuration overrides.
                         Keys are configuration paths, values are strings
                         that will be parsed as appropriate types.
                         
          Returns:
              Brain: Configured and initialized brain
              
          Raises:
              ConfigError: If template name is unknown or parameter parsing fails
              InitializationError: If brain initialization fails
              
          Example:
          >>> import pynlm
          >>> # Create a medium-sized learning brain with custom parameters
          >>> params = {
          ...     "brain.neuron_count": 2000,
          ...     "neuromod.dopamine.scale": 2.0,
          ...     "development_synaptogenesis_rate": 0.002
          ... }
          >>> brain = pynlm.create_brain_from_template("learning", params)
          )pbdoc");

    m.def("export_experiment_data", [](const std::vector<std::string>& logs,
                                       const std::map<std::string, double>& metrics,
                                       const std::string& filename) {
        // Create experiment directory if it doesn't exist
        std::filesystem::path filepath(filename);
        if (filepath.parent_path().empty()) {
            filepath = "./experiments/" + filepath.string();
        }
        
        std::filesystem::create_directories(filepath.parent_path());
        
        // Export logs to file
        std::ofstream logFile(filepath.string() + ".txt");
        if (logFile.is_open()) {
            logFile << "=== NLM Experiment Data ===\n\n";
            logFile << "=== Logs ===\n";
            for (const auto& log : logs) {
                logFile << log << "\n";
            }
            logFile << "\n=== Metrics ===\n";
            for (const auto& pair : metrics) {
                logFile << pair.first << ": " << pair.second << "\n";
            }
            logFile.close();
        } else {
            throw SimulationError("Cannot write experiment data to '" + filepath.string() + "'");
        }
        
        // Also export metrics as JSON (simplified)
        std::ofstream jsonFile(filepath.string() + ".json");
        if (jsonFile.is_open()) {
            jsonFile << "{\n";
            bool first = true;
            for (const auto& pair : metrics) {
                if (!first) jsonFile << ",\n";
                jsonFile << "  \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            jsonFile << "\n}\n";
            jsonFile.close();
        }
        
        return std::vector<std::string>{
            "Experiment data exported successfully",
            "Logs: " + filepath.string() + ".txt",
            "Metrics: " + filepath.string() + ".json"
        };
    }, py::arg("logs"), py::arg("metrics"), py::arg("filename"),
          R"pbdoc(Export experiment data to files for analysis and sharing)
          
          Export simulation logs and metrics to files in a standard format
          for post-experiment analysis, visualization, and sharing with others.
          
          Args:
              logs: List of log messages from simulation
              metrics: Dictionary of performance metrics
              filename: Base filename (will create .txt and .json files)
              
          Returns:
              list: Summary messages of export operations
              
          Raises:
              SimulationError: If file writing fails
              
          Example:
          >>> import pynlm
          >>> brain = pynlm.createBrainWithDefaults()
          >>> results = pynlm.run_agent_episode(brain, world, agent, 1000)
          >>> stats = pynlm.getBrainStats(brain)
          >>> export_messages = pynlm.export_experiment_data(results, stats, "my_experiment_001")
          >>> for msg in export_messages:
          ...     print(msg)
          )pbdoc");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

