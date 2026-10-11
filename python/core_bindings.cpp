// Python bindings for NLM core types and utilities
// This file contains bindings for fundamental types and utility classes

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

namespace py = pybind11;
namespace nlm {

void bindCoreTypes(py::module& m);
void bindConfig(py::module& m);
void bindRandom(py::module& m);
void bindLogger(py::module& m);
void bindSimulationClock(py::module& m);

void include_core_bindings(py::module& m) {
    bindCoreTypes(m);
    bindConfig(m);
    bindRandom(m);
    bindLogger(m);
    bindSimulationClock(m);
}

void bindCoreTypes(py::module& m) {
    // NeuronId - Unique identifier for neurons
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId represents a unique identifier for neurons in the NLM system.
        It's used to reference specific neurons within neural regions.
        
        Attributes:
            value (int): Internal identifier value
            
        Example:
            >>> import pynlm
            >>> neuron_id = pynlm.NeuronId(42)
            >>> print(neuron_id.value)
            42
        
        See Also:
            NeuronType, RegionId
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId - Unique identifier for synapses
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId represents a unique identifier for synapses in the NLM system.
        It's used to reference specific synaptic connections between neurons.
        
        Attributes:
            value (int): Internal identifier value
            
        Example:
            >>> import pynlm
            >>> synapse_id = pynlm.SynapseId(123)
            >>> print(synapse_id.value)
            123
        
        See Also:
            SynapseType, NeuronId
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of the synapse ID")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId - Unique identifier for brain regions
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId represents a unique identifier for neural regions in the NLM system.
        Brain regions contain groups of neurons and synapses.
        
        Attributes:
            value (int): Internal identifier value
            
        Example:
            >>> import pynlm
            >>> region_id = pynlm.RegionId(1)
            >>> print(region_id.value)
            1
        
        See Also:
            NeuralRegion, Brain, NeuronId
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of the region ID")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId - Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId represents a unique identifier for neuron populations
        within neural regions. Populations group neurons by type or function.
        
        Attributes:
            value (int): Internal identifier value
            
        Example:
            >>> import pynlm
            >>> pop_id = pynlm.PopulationId(10)
            >>> print(pop_id.value)
            10
        
        See Also:
            NeuralPopulation, NeuronType
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index,
             "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal")
        .def("__hash__", [](const PopulationId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Get hash value for use in sets and dictionaries");

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM system.
        
        Neuron types determine the function and properties of neurons:
        - Excitatory: Increases activity in connected neurons
        - Inhibitory: Decreases activity in connected neurons
        - Modulatory: Affects synaptic plasticity and learning
        - Sensory: Processes external sensory input
        - Motor: Generates motor commands
        - Internal: Internal representation and cognition
        
        Example:
            >>> import pynlm
            >>> neuron_type = pynlm.NeuronType.Excitatory
            >>> print(neuron_type)
            NeuronType.Excitatory
        
        See Also:
            Neuron, Brain, NeuronId
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Exciting neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibiting neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulating neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values()
        .doc() = "Enumeration of neuron types in the NLM system";

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synapse in the NLM system.
        
        Synapse types determine the function and properties of synaptic connections:
        - Excitatory: Increases postsynaptic neuron activity
        - Inhibitory: Decreases postsynaptic neuron activity
        - Modulatory: Affects synaptic plasticity
        - Electrical: Direct electrical coupling
        - GapJunction: Electrical synapse through gap junctions
        
        Example:
            >>> import pynlm
            >>> synapse_type = pynlm.SynapseType.Excitatory
            >>> print(synapse_type)
            SynapseType.Excitatory
        
        See Also:
            Synapse, Brain, SynapseId
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Exciting synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibiting synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulating synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values()
        .doc() = "Enumeration of synapse types in the NLM system";

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage of the NLM system.
        
        Developmental stages represent the maturation state of the neural system:
        - Initial: Newborn, primitive neural connections
        - CriticalPeriod: High plasticity, rapid learning
        - Maturation: Fine-tuning and specialization
        - Adult: Stable, optimized neural architecture
        - Aging: Gradual decline in plasticity
        
        Example:
            >>> import pynlm
            >>> stage = pynlm.DevelopmentalStage.CriticalPeriod
            >>> print(stage)
            DevelopmentalStage.CriticalPeriod
        
        See Also:
            DevelopmentSystem, Brain, plasticity
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values()
        .doc() = "Enumeration of developmental stages in the NLM system";

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Firing state of a neuron.
        
        Firing states represent the current activity level of neurons:
        - Resting: Not actively firing
        - Active: Currently firing action potentials
        - Refractory: Recoverable after firing
        - Inhibited: Temporarily suppressed
        
        Example:
            >>> import pynlm
            >>> state = pynlm.FiringState.Active
            >>> print(state)
            FiringState.Active
        
        See Also:
            Neuron, Brain, MembranePotential
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting firing state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory firing state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited firing state")
        .export_values()
        .doc() = "Enumeration of neuron firing states";

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of motor action.
        
        Action types represent the possible actions an agent can take:
        - MoveForward: Move forward in the environment
        - MoveBackward: Move backward in the environment
        - MoveLeft: Move left in the environment
        - MoveRight: Move right in the environment
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - Look: Look in current direction
        - LookUp: Look up
        - LookDown: Look down
        - Interact: Interact with environment objects
        - Eat: Consume food/resource
        - Drink: Consume liquid
        - Rest: Stop moving
        - Wait: Wait for next step
        - Custom: Custom action type
        
        Example:
            >>> import pynlm
            >>> action_type = pynlm.ActionType.MoveForward
            >>> print(action_type)
            ActionType.MoveForward
        
        See Also:
            Action, AgentBrain, MotorCommand
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
        .doc() = "Enumeration of action types in the NLM system";

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command.
        
        Motor commands represent the low-level actions that can be executed:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - LookLeft: Look left
        - LookRight: Look right
        - Interact: Interact with environment
        - Wait: Wait (no action)
        
        Example:
            >>> import pynlm
            >>> motor_cmd = pynlm.MotorCommand.MoveForward
            >>> print(motor_cmd)
            MotorCommand.MoveForward
        
        See Also:
            Action, ActionType, AgentBrain
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
        .doc() = "Enumeration of motor commands in the NLM system";

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the NLM world.
        
        World object types represent different types of entities in the environment:
        - Empty: No object
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Solid barrier
        - Marker: Special location or target
        
        Example:
            >>> import pynlm
            >>> obj_type = pynlm.WorldObjectType.Resource
            >>> print(obj_type)
            WorldObjectType.Resource
        
        See Also:
            WorldObject, SimpleWorld, AgentBody
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world object")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values()
        .doc() = "Enumeration of world object types in the NLM system";

    // Global constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
}

void bindConfig(py::module& m) {
    // Config class - Configuration system for NLM
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages all configuration parameters for the NLM system.
        It supports:
        - Setting and getting configuration values
        - Loading from JSON files
        - Loading from command line arguments
        - Summary and validation
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.stdp.enable", True)
            
        Configuration parameters are organized hierarchically:
        - brain.neuron_count: Number of neurons
        - brain.synapse_density: Synapse connectivity density
        - plasticity.stdp.enable: Enable/disable STDP
        - neuromod.dopamine.scale: Dopamine modulation scale
        - development.critical_period: Critical period duration
        
        Methods:
            loadFromFile(filepath): Load configuration from JSON file
            loadFromArgs(args): Load configuration from command line arguments
            saveToFile(filepath): Save configuration to JSON file
            has(key): Check if a configuration key exists
            get(key): Get a configuration value
            set(key, value): Set a configuration value
            getKeys(): Get all configuration keys
            clear(): Clear all configuration entries
            summary(): Get a summary string of the configuration
        
        Properties:
            All configuration keys are accessible as attributes
        
        Raises:
            ConfigurationError: If configuration operations fail
            RuntimeError: If file operations fail
            
        See Also:
            pynlm.createBrain, pynlm.createDefaultConfig
            pynlm.createSimpleWorld, pynlm.createAgentBrain
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get),
             py::arg("key"), "Get a configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value (string)")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value (bool)")
        .def("set", py::overload_cast<const std::string&, int64_t>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value (int64_t)")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value (double)")
        .def("set", py::overload_cast<const std::string&, float>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value (float)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of the configuration")
        .def("__getitem__", [](const Config& cfg, const std::string& key) {
            return cfg.get(key);
        }, "Get configuration value by key (supports dictionary-like access)")
        .def("__setitem__", [](Config& cfg, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                cfg.set(key, value.cast<int64_t>());
            } else if (py::isinstance<py::float_>(value)) {
                cfg.set(key, value.cast<float>());
            } else if (py::isinstance<py::bool_>(value)) {
                cfg.set(key, value.cast<bool>());
            } else if (py::isinstance<py::str>(value)) {
                cfg.set(key, value.cast<std::string>());
            } else {
                throw ConfigurationError("Unsupported type for configuration value");
            }
        }, "Set configuration value by key (supports dictionary-like access)")
        .def("__contains__", [](const Config& cfg, const std::string& key) {
            return cfg.has(key);
        }, "Check if a configuration key exists (supports 'in' operator)")
        .def("update", [](Config& cfg, const py::dict& new_config) {
            for (auto it : new_config) {
                std::string key = py::str(it.first);
                py::object value = it.second;
                if (py::isinstance<py::int_>(value)) {
                    cfg.set(key, value.cast<int64_t>());
                } else if (py::isinstance<py::float_>(value)) {
                    cfg.set(key, value.cast<float>());
                } else if (py::isinstance<py::bool_>(value)) {
                    cfg.set(key, value.cast<bool>());
                } else if (py::isinstance<py::str>(value)) {
                    cfg.set(key, value.cast<std::string>());
                }
            }
        }, py::arg("new_config"), "Update configuration from dictionary");
}

void bindRandom(py::module& m) {
    // Random generator functionality
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(
        Random number generator for NLM simulations.
        
        The RandomGenerator class provides pseudo-random number generation
        for simulations, ensuring reproducible results when seeded.
        
        Example:
            >>> import pynlm
            >>> rng = pynlm.RandomGenerator(42)
            >>> rng.getRandomFloat()
            0.637019
            >>> rng.getRandomInt(0, 100)
            67
        
        Methods:
            getRandomFloat(): Get random float in [0, 1)
            getRandomInt(min, max): Get random integer in [min, max)
            setSeed(seed): Set the random seed
            getSeed(): Get the current seed
        
        See Also:
            SimulationClock, Logger
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"),
             "Initialize with specific seed")
        .def("getRandomFloat", &RandomGenerator::getRandomFloat,
             "Get random float in [0, 1)")
        .def("getRandomInt", &RandomGenerator::getRandomInt,
             py::arg("min"), py::arg("max"),
             "Get random integer in [min, max)")
        .def("setSeed", &RandomGenerator::setSeed, py::arg("seed"),
             "Set the random seed")
        .def("getSeed", &RandomGenerator::getSeed,
             "Get the current random seed");
}

void bindLogger(py::module& m) {
    // Logger functionality
    py::class_<Logger>(m, "Logger", R"pbdoc(
        Logging system for NLM simulations.
        
        The Logger class provides logging infrastructure for NLM simulations,
        supporting different log levels and output formats.
        
        Example:
            >>> import pynlm
            >>> logger = pynlm.Logger()
            >>> console_logger = pynlm.ConsoleLogger(pynlm.LogLevel.Info)
            >>> logger.addLogger(console_logger)
            >>> logger.log(pynlm.LogLevel.Info, "Simulation started")
        
        Methods:
            addLogger(logger): Add a logger
            removeLogger(logger): Remove a logger
            log(level, message): Log a message
            setLevel(level): Set log level
            getLevel(): Get current log level
        
        See Also:
            LogLevel, ConsoleLogger, FileLogger
    )pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"),
             "Add a logger to the logging system")
        .def("removeLogger", &Logger::removeLogger, py::arg("logger"),
             "Remove a logger from the logging system")
        .def("log", py::overload_cast<LogLevel, const std::string&>(&Logger::log),
             py::arg("level"), py::arg("message"),
             "Log a message at the specified level")
        .def("setLevel", &Logger::setLevel, py::arg("level"),
             "Set the log level for this logger")
        .def("getLevel", &Logger::getLevel,
             "Get the current log level");

    // ConsoleLogger class
    py::class_<ConsoleLogger, Logger>(m, "ConsoleLogger", R"pbdoc(
        Console logger for NLM simulations.
        
        The ConsoleLogger class writes log messages to the console (stderr).
        
        Example:
            >>> import pynlm
            >>> console_logger = pynlm.ConsoleLogger(pynlm.LogLevel.Info)
            >>> console_logger.setFormat("[%(level)s] %(message)s")
        
        Attributes:
            level: Log level for this logger
            format: Message format string
            
        Methods:
            setFormat(format): Set the message format
            getFormat(): Get the current format
        
        See Also:
            Logger, FileLogger, LogLevel
    )pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"),
             "Initialize with log level")
        .def("setFormat", &ConsoleLogger::setFormat, py::arg("format"),
             "Set the message format")
        .def("getFormat", &ConsoleLogger::getFormat,
             "Get the current format");

    // LogLevel enumeration
    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(
        Log message level.
        
        Log levels determine the severity of log messages:
        - Trace: Very detailed debug information
        - Debug: Debug information
        - Info: General information
        - Warning: Warning messages
        - Error: Error messages
        - Critical: Critical error messages
        
        Example:
            >>> import pynlm
            >>> level = pynlm.LogLevel.Info
            >>> print(level)
            LogLevel.Info
        
        See Also:
            Logger, ConsoleLogger, FileLogger
    )pbdoc")
        .value("Trace", LogLevel::Trace, "Trace level messages")
        .value("Debug", LogLevel::Debug, "Debug level messages")
        .value("Info", LogLevel::Info, "Information level messages")
        .value("Warning", LogLevel::Warning, "Warning level messages")
        .value("Error", LogLevel::Error, "Error level messages")
        .value("Critical", LogLevel::Critical, "Critical level messages")
        .export_values()
        .doc() = "Enumeration of log levels for NLM logging system";
}

void bindSimulationClock(py::module& m) {
    // SimulationClock class
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(
        Simulation clock for NLM simulations.
        
        The SimulationClock class provides time management for NLM simulations,
        tracking simulation time and providing time-based utilities.
        
        Example:
            >>> import pynlm
            >>> clock = pynlm.SimulationClock(0.001)  # 1ms timestep
            >>> clock.getTime()
            0.0
            >>> clock.getStepCount()
            0
            >>> clock.update()
            >>> clock.getTime()
            0.001
        
        Attributes:
            timestep: Time step size
            
        Methods:
            getTime(): Get current simulation time
            getStepCount(): Get number of steps taken
            update(): Update clock by one timestep
            reset(): Reset clock to zero
            setTimestep(timestep): Set the time step size
            getTimestep(): Get the current time step size
        
        See Also:
            Brain, step, SimulationStep
    )pbdoc")
        .def(py::init<double>(), py::arg("timestep"),
             "Initialize with time step size")
        .def("getTime", &SimulationClock::getTime,
             "Get current simulation time")
        .def("getStepCount", &SimulationClock::getStepCount,
             "Get number of steps taken")
        .def("update", &SimulationClock::update,
             "Update clock by one timestep")
        .def("reset", &SimulationClock::reset,
             "Reset clock to zero")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("timestep"),
             "Set the time step size")
        .def("getTimestep", &SimulationClock::getTimestep,
             "Get the current time step size");
}
