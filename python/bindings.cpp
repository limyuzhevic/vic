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
        
        This module provides Python interfaces to the NLM neural simulation system,
        including classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Strongly-typed neural identifiers (NeuronId, SynapseId, RegionId, PopulationId)
        - Complete neural simulation with spiking neurons and plasticity
        - Sensory processing (vision, audio, internal signals)
        - Motor control and world interaction
        - Development and neuromodulation systems
        - Simulation configuration and checkpointing
        
        Usage example:
        ```python
        from pynlm import createDefaultConfig, createBrain, createSimpleWorld
        
        # Create a default configuration
        config = createDefaultConfig()
        
        # Create a brain with the configuration
        brain = createBrain(config)
        
        # Create a simulation world
        world = createSimpleWorld()
        
        # Initialize and run simulation
        brain.initialize()
        
        for step in range(100):
            world.update(0.1)
            brain.step(step, world.getSimulationTime())
        ```
        
        For detailed documentation of individual classes and methods, see the class
        and method docstrings below.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        This is a strongly-typed identifier that ensures type safety when
        referencing neurons. It wraps a uint64_t value and provides comparison
        and hashing operations for use as dictionary keys.
        
        Attributes:
            value (int): The underlying uint64_t value of the neuron ID.
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> neuron_id.value
            42
            >>> print(neuron_id)
            <NeuronId: 42>
            >>> hash(neuron_id)  # Can be used as dictionary key
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the index of this neuron ID (alias for value).")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId.")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId.")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash of this NeuronId for use in dictionaries and sets.")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Return string representation of the neuron ID.")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "Return string representation of the neuron ID value.");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        This is a strongly-typed identifier that ensures type safety when
        referencing synapses. It wraps a uint64_t value and provides comparison
        and hashing operations for use as dictionary keys.
        
        Attributes:
            value (int): The underlying uint64_t value of the synapse ID.
        
        Examples:
            >>> synapse_id = SynapseId(100)
            >>> synapse_id.value
            100
            >>> print(synapse_id)
            <SynapseId: 100>
            >>> hash(synapse_id)  # Can be used as dictionary key
            100
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get the index of this synapse ID (alias for value).")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId.")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId.")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash of this SynapseId for use in dictionaries and sets.")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Return string representation of the synapse ID.")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "Return string representation of the synapse ID value.");

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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

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
        
        This class provides a key-value store for neural simulation configuration
        parameters. It supports loading from and saving to JSON files, as well as
        command-line argument parsing.
        
        The configuration supports various neural network parameters including
        neuron counts, connectivity patterns, plasticity rules, developmental
        settings, and simulation parameters.
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("brain_config.json")
            >>> config.has("network.neuron_count")
            True
            >>> config.get("network.neuron_count", 1000)
            1000
            >>> config.summary()
            '<Config: network.neuron_count=1000, simulation.timestep=0.01, ...>'
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                return self.loadFromFile(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file. Raises RuntimeError if file not found or invalid JSON.)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments. Raises RuntimeError on parsing error.)pbdoc")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                return self.saveToFile(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file. Raises RuntimeError if file cannot be written.)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key (str): Configuration key in dot notation (e.g., "network.neuron_count").
             
             Returns:
                 bool: True if key exists, False otherwise.
             
             Examples:
                 >>> config.has("network.neuron_count")
                 True
                 >>> config.has("nonexistent.key")
                 False
             )pbdoc")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.get<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(Get a string configuration value.
           
           Args:
               key (str): Configuration key in dot notation.
               defaultValue (str): Default value if key doesn't exist.
           
           Returns:
               str: Configuration value or default if key doesn't exist.
           
           Examples:
               >>> config.get("network.neuron_type", "excitatory")
               'excitatory'
           )pbdoc")
        .def("get", [](Config& self, const std::string& key, int defaultValue) {
            return self.get<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(Get an integer configuration value.
           
           Args:
               key (str): Configuration key in dot notation.
               defaultValue (int): Default value if key doesn't exist.
           
           Returns:
               int: Configuration value or default if key doesn't exist.
           
           Examples:
               >>> config.get("network.neuron_count", 1000)
               1000
           )pbdoc")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            return self.get<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(Get a float configuration value.
           
           Args:
               key (str): Configuration key in dot notation.
               defaultValue (float): Default value if key doesn't exist.
           
           Returns:
               float: Configuration value or default if key doesn't exist.
           
           Examples:
               >>> config.get("simulation.timestep", 0.01)
               0.01
           )pbdoc")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            return self.get<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(Get a boolean configuration value.
           
           Args:
               key (str): Configuration key in dot notation.
               defaultValue (bool): Default value if key doesn't exist.
           
           Returns:
               bool: Configuration value or default if key doesn't exist.
           
           Examples:
               >>> config.get("plasticity.enabled", False)
               False
           )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 list[str]: List of all configuration keys in dot notation.
             
             Examples:
                 >>> config.getKeys()
                 ['network.neuron_count', 'network.excitatory_ratio', ...]
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             Examples:
                 >>> config.clear()
                 >>> config.getKeys()
                 []
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable summary of configuration parameters.
             
             Examples:
                 >>> config.summary()
                 '<Config: network.neuron_count=1000, simulation.timestep=0.01, ...>'
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Return string representation of the configuration.");

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
            // Input validation
            size_t expected_size = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expected_size) {
                throw py::value_error("Vision data size mismatch: expected " + std::to_string(expected_size) +
                                     " elements for " + std::to_string(self.getWidth()) + "x" +
                                     std::to_string(self.getHeight()) + " grid with " +
                                     std::to_string(self.getChannels()) + " channels, got " +
                                     std::to_string(data.size()));
            }
            // Validate data range
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i] < 0.0f || data[i] > 1.0f) {
                    throw py::value_error("Vision data value out of range [0.0, 1.0] at index " +
                                         std::to_string(i) + ": " + std::to_string(data[i]));
                }
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData)
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDimensions", &Vision::getDimensions)
        .def("getType", &Vision::getType)
        .def("getTimestamp", &Vision::getTimestamp)
        .def("setTimestamp", [](Vision& self, double timestamp) {
            // Input validation for timestamp
            if (timestamp < 0.0) {
                throw py::value_error("Vision timestamp must be non-negative: " + std::to_string(timestamp));
            }
            self.setTimestamp(timestamp);
        }, py::arg("timestamp"))
        .def("__repr__", [](const Vision& vision) {
            std::ostringstream oss;
            oss << "<Vision: " << vision.getWidth() << "x" << vision.getHeight()
                << " channels=" << vision.getChannels()
                << " timestamp=" << vision.getTimestamp() << ">";
            return oss.str();
        })
        .def("__str__", [](const Vision& vision) {
            return "Vision(width=" + std::to_string(vision.getWidth()) +
                   ", height=" + std::to_string(vision.getHeight()) +
                   ", channels=" + std::to_string(vision.getChannels()) + ")";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            // Input validation
            size_t expected_size = self.getNumSamples();
            if (data.size() != expected_size) {
                throw py::value_error("Audio data size mismatch: expected " + std::to_string(expected_size) +
                                     " samples for " + std::to_string(self.getSampleRate()) +
                                     " Hz, got " + std::to_string(data.size()));
            }
            // Validate data range
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i] < -1.0f || data[i] > 1.0f) {
                    throw py::value_error("Audio data value out of range [-1.0, 1.0] at index " +
                                         std::to_string(i) + ": " + std::to_string(data[i]));
                }
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            if (sampleRate == 0) {
                throw py::value_error("Audio sample rate must be positive, got " + std::to_string(sampleRate));
            }
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getData", &Audio::getData)
        .def("getDimensions", &Audio::getDimensions)
        .def("getType", &Audio::getType)
        .def("getTimestamp", &Audio::getTimestamp)
        .def("setTimestamp", [](Audio& self, double timestamp) {
            // Input validation for timestamp
            if (timestamp < 0.0) {
                throw py::value_error("Audio timestamp must be non-negative: " + std::to_string(timestamp));
            }
            self.setTimestamp(timestamp);
        }, py::arg("timestamp"))
        .def("__repr__", [](const Audio& audio) {
            std::ostringstream oss;
            oss << "<Audio: sample_rate=" << audio.getSampleRate()
                << " samples=" << audio.getNumSamples()
                << " timestamp=" << audio.getTimestamp() << ">";
            return oss.str();
        })
        .def("__str__", [](const Audio& audio) {
            return "Audio(sample_rate=" + std::to_string(audio.getSampleRate()) +
                   ", samples=" + std::to_string(audio.getNumSamples()) + ")";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getData", &InternalSignals::getData)
        .def("getDimensions", &InternalSignals::getDimensions)
        .def("getType", &InternalSignals::getType)
        .def("getTimestamp", &InternalSignals::getTimestamp)
        .def("setTimestamp", &InternalSignals::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const InternalSignals& signals) {
            std::ostringstream oss;
            oss << "<InternalSignals: dimension=" << signals.getDimensions()
                << " timestamp=" << signals.getTimestamp() << ">";
            return oss.str();
        })
        .def("__str__", [](const InternalSignals& signals) {
            return "InternalSignals(dimension=" + std::to_string(signals.getDimensions()) + ")";
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            std::ostringstream oss;
            oss << "<Action: type=" << static_cast<int>(action.getType())
                << " params_size=" << action.getParameters().size() << ">";
            return oss.str();
        })
        .def("__str__", [](const Action& action) {
            return "Action(type=" + std::to_string(static_cast<int>(action.getType())) +
                   ", params_size=" + std::to_string(action.getParameters().size()) + ")";
        });

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
        .def("__repr__", [](const WorldObject& obj) {
            std::ostringstream oss;
            oss << "<WorldObject: x=" << obj.x << ", y=" << obj.y
                << ", type=" << static_cast<int>(obj.type)
                << ", value=" << obj.value << ", active=" << (obj.active ? "True" : "False") << ">";
            return oss.str();
        });

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
        .def("reset", &AgentBody::reset)
        .def("__repr__", [](const AgentBody& body) {
            std::ostringstream oss;
            oss << "<AgentBody: pos=(" << body.x << ", " << body.y << ")"
                << " orientation=" << body.orientation << " rad"
                << " energy=" << body.energy << " health=" << body.health
                << " age=" << body.age
                << " moving=" << (body.isMoving ? "True" : "False")
                << " turning=" << (body.isTurning ? "True" : "False") << ">";
            return oss.str();
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::ostringstream oss;
            oss << "<ActionResult: reward=" << result.reward
                << " success=" << (result.success ? "True" : "False")
                << " message=\"" << result.message << "\">";
            return oss.str();
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            std::ostringstream oss;
            oss << "<SensoryPercept: vision=(" << percept.getVisionWidth() << "x" << percept.getVisionHeight() << ")"
                << " touch_size=" << percept.getTouch().size()
                << " internal_size=" << percept.getInternal().size()
                << " proprioception_size=" << percept.getProprioception().size()
                << " audio_size=" << percept.getAudio().size()
                << " timestamp=" << percept.getTimestamp() << ">";
            return oss.str();
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            try {
                self.configure(width, height, visionWidth, visionHeight);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
            try {
                return self.applyMotorCommand(cmd, currentTime);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("cmd"), py::arg("currentTime"))
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            std::ostringstream oss;
            oss << "<SimpleWorld: width=" << world.getWidth() << " height=" << world.getHeight()
                << " agent=(" << world.getAgentBody().x << ", " << world.getAgentBody().y << ")"
                << " time=" << world.getSimulationTime() << ">";
            return oss.str();
        });

        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                return self.initialize();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            try {
                self.injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, MembranePotential current) {
            try {
                self.injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", [](Brain& self) {
            try {
                return self.produceAction();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                return self.save(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
             "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                return self.load(filepath);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("filepath"),
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
        .def("__repr__", [](const Brain& brain) {
            std::ostringstream oss;
            oss << "<Brain: regions=" << brain.getRegionCount()
                << " neurons=" << brain.getTotalNeuronCount()
                << " synapses=" << brain.getTotalSynapseCount()
                << " stage=" << static_cast<int>(brain.getDevelopmentalStage())
                << " active_neurons=" << brain.getActiveNeuronCount() << ">";
            return oss.str();
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            try {
                self.initialize(world);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](AgentBrain& self) {
            try {
                return self.decodeMotorCommand();
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                self.applyRewardModulation(reward, predictedReward);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw py::error_already_set();
            }
        }, py::arg("timestep"),
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
        .def("__repr__", [](const AgentBrain& agentBrain) {
            std::ostringstream oss;
            oss << "<AgentBrain: neuromodulation_level=" << agentBrain.getNeuromodulationLevel()
                << " curiosity_level=" << agentBrain.getCuriosityLevel()
                << " novelty_level=" << agentBrain.getNoveltyLevel()
                << " prediction_error=" << agentBrain.getPredictionError()
                << " stage=" << static_cast<int>(agentBrain.getDevelopmentalStage()) << ">";
            return oss.str();
        });

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
