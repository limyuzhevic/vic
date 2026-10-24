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
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a Pythonic interface to the NLM neural simulation framework
        with improved error handling, context managers, and convenience methods.
        
        Key improvements over the base C++ API:
        - Enhanced error messages with Python context
        - Context managers for safe resource management
        - Fluent interface builders for configuration
        - Convenience methods for common operations
        - Comprehensive documentation with type hints
        - Validation of parameters and state
        
        Usage example:
        
        ```python
        import pynlm
        
        # Create brain with validation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        # Use context manager for safe simulation
        with pynlm.SimulationSession(brain, world) as session:
            session.run_steps(100)
        
        # Use fluent configuration builder
        config = (pynlm.ConfigBuilder()
                 .load_defaults()
                 .add("learning_rate", 0.01)
                 .add("population_size", 1000)
                 .build())
        
        # Get brain statistics easily
        stats = brain.get_statistics()
        ```
    )pbdoc";

    // Enhanced exception types with Python context
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // More specific exception types for better error handling
    class_<std::invalid_argument>(m, "InvalidArgument")
        .def(py::init<const std::string&>());
        
    class_<std::out_of_range>(m, "OutOfRange")
        .def(py::init<const std::string&>());
        
    class_<std::logic_error>(m, "LogicError")
        .def(py::init<const std::string&>());
        
    class_<std::domain_error>(m, "DomainError")
        .def(py::init<const std::string&>());
        
    class_<std::length_error>(m, "LengthError")
        .def(py::init<const std::string&>());

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

    // Enhanced Config class with Pythonic fluent interface
    py::class_<Config>(m, "Config", R"pbdoc(
    Configuration class for NLM system with enhanced Python interface.
        
    Config provides configuration management for the NLM system with
    enhanced Python features including fluent configuration building,
    validation, and type-safe operations.
        
    Key enhancements over the base C++ API:
    - Fluent configuration building
    - Enhanced validation and error handling
    - Type-safe parameter access
    - Convenience methods for common operations
    - Comprehensive documentation with examples
    
    Usage example:
        
        ```python
        import pynlm
        
        # Create configuration fluently
        config = (pynlm.ConfigBuilder()
                 .add("brain.neuron_count", 500)
                 .add("brain.synapse_density", 0.1)
                 .add("plasticity.stdp.enable", True)
                 .build())
        
        # Use configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Validate configuration
        if config.validate():
            brain.initialize()
        
        # Get configuration values
        neuron_count = config.get("brain.neuron_count")
        ```
    )pbdoc")
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
        })
        // Fluent configuration builder methods
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair")
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value")
        .def("getOr", [](const Config& self, const std::string& key, const ConfigValue& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with default")
        .def("validate", &Config::validate,
             "Validate configuration values")
        .def("compareWith", &Config::compareWith, py::arg("other"),
             "Compare with another configuration")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("copy", &Config::copy,
             py::return_value_policy::reference_internal,
             "Create a copy of this configuration");

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

    // SimulationSession context manager for safe resource management
    py::class_<SimulationSession>(m, "SimulationSession", R"pbdoc(
    Context manager for safe simulation sessions with automatic cleanup.
        
    This class provides a safe interface for running complete simulation sessions
    with automatic initialization, execution, and cleanup using Python's context
    manager protocol (with statement).
        
    Usage:
        
        ```python
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Create agent and initialize
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation session safely
        with pynlm.SimulationSession(brain, world, agent) as session:
            results = session.run_steps(100)
            session.log_session_summary()
            
        # Resources automatically cleaned up even if exception occurs
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("initialize", &SimulationSession::initialize, "Initialize the simulation session")
        .def("run_step", &SimulationSession::run_step, 
             py::arg("step_number"), "Run a single simulation step")
        .def("run_steps", &SimulationSession::run_steps,
             py::arg("num_steps"), "Run multiple simulation steps")
        .def("get_results", &SimulationSession::getResults,
             py::return_value_policy::reference_internal, "Get recorded results")
        .def("reset_session", &SimulationSession::resetSession, "Reset the session state")
        .def("log_session_summary", &SimulationSession::logSessionSummary, 
             "Log a summary of the session results")
        .def("__enter__", &SimulationSession::enter,
             py::return_value_policy::reference_internal, "Context manager enter")
        .def("__exit__", &SimulationSession::exit, "Context manager exit");
        
    // ConfigBuilder for fluent configuration
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(
    Fluent builder for creating and configuring brain settings.
        
    This class provides a Pythonic fluent interface for creating configurations
    without having to remember all the configuration keys and their types.
        
    Usage:
        
        ```python
        import pynlm
        
        # Build configuration fluently
        config = (pynlm.ConfigBuilder()
                 .load_defaults()
                 .add("brain.neuron_count", 500)
                 .add("brain.synapse_density", 0.1)
                 .add("plasticity.stdp.enable", True)
                 .add("neuromod.dopamine.scale", 1.0)
                 .build())
        
        # Validate configuration
        if config.validate():
            brain = pynlm.createBrain(config)
            brain.initialize()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("load_defaults", &ConfigBuilder::loadDefaults, "Load default configuration values")
        .def("load_from_file", &ConfigBuilder::loadFromFile, py::arg("filepath"), 
             "Load configuration from a file")
        .def("load_from_args", &ConfigBuilder::loadFromArgs, 
             "Load configuration from command line arguments")
        .def("add", &ConfigBuilder::add, 
             py::arg("key"), py::arg("value"), 
             "Add a configuration key-value pair")
        .def("set", &ConfigBuilder::set, 
             py::arg("key"), py::arg("value"), 
             "Set a configuration key-value pair")
        .def("get", &ConfigBuilder::get, py::arg("key"), 
             "Get a configuration value")
        .def("has", &ConfigBuilder::has, py::arg("key"), 
             "Check if configuration has a key")
        .def("remove", &ConfigBuilder::remove, py::arg("key"), 
             "Remove a configuration key")
        .def("validate", &ConfigBuilder::validate, 
             "Validate configuration values")
        .def("compare_with", &ConfigBuilder::compareWith, py::arg("other"), 
             "Compare with another configuration")
        .def("build", &ConfigBuilder::build, 
             py::return_value_policy::reference_internal, "Build the final Config object")
        .def("copy", &ConfigBuilder::copy, 
             py::return_value_policy::reference_internal, "Copy current builder state");
        
    // ActionBuilder for fluent action construction
    py::class_<ActionBuilder>(m, "ActionBuilder", R"pbdoc(
    Fluent builder for constructing and configuring actions.
        
    This class provides a convenient way to create actions with type hints
    and validation, making it easier to work with the diverse action types
    available in NLM.
        
    Usage:
        
        ```python
        import pynlm
        
        # Create actions fluently
        move_action = pynlm.ActionBuilder().move_forward(1.0).set_speed(2.0)
        turn_action = pynlm.ActionBuilder().turn_left(45.0)
        interact_action = pynlm.ActionBuilder().interact()
        
        # Set parameters
        move_action.set_parameter("speed", 2.0)
        move_action.set_parameter("duration", 1.5)
        
        # Clone and modify
        action_copy = move_action.clone()
        action_copy.set_parameter("speed", 1.0)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("move_forward", &ActionBuilder::moveForward, py::arg("distance"), 
             "Create a move forward action")
        .def("move_backward", &ActionBuilder::moveBackward, py::arg("distance"), 
             "Create a move backward action")
        .def("turn_left", &ActionBuilder::turnLeft, py::arg("angle"), 
             "Create a turn left action")
        .def("turn_right", &ActionBuilder::turnRight, py::arg("angle"), 
             "Create a turn right action")
        .def("look", &ActionBuilder::look, py::arg("target_x"), py::arg("target_y"), 
             "Create a look action")
        .def("interact", &ActionBuilder::interact, 
             "Create an interact action")
        .def("set_type", &ActionBuilder::setType, py::arg("action_type"), 
             "Set action type")
        .def("set_parameter", &ActionBuilder::setParameter, 
             py::arg("name"), py::arg("value"), 
             "Set action parameter")
        .def("set_parameters", &ActionBuilder::setParameters, py::arg("params"), 
             "Set multiple action parameters")
        .def("clone", &ActionBuilder::clone, 
             py::return_value_policy::reference_internal, "Clone this action builder")
        .def("build", &ActionBuilder::build, 
             py::return_value_policy::reference_internal, "Build the final Action object");
        
    // Convenience methods class for Brain enhancements
    py::class_<BrainConvenience>(m, "BrainConvenience", R"pbdoc(
    Convenience methods for enhanced Brain functionality.
        
    This class provides additional methods that make working with Brain objects
    more Pythonic and convenient, without cluttering the main Brain API.
        
    Usage:
        
        ```python
        import pynlm
        
        # Create and initialize brain easily
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Use convenience methods
        brain.quick_initialize()
        brain.run_for_steps(100)
        
        # Get convenient statistics
        stats = brain.get_statistics()
        diagnostics = brain.get_diagnostics()
        
        # Enhanced learning control
        brain.enable_all_learning()
        brain.disable_all_plasticity()
        ```
    )pbdoc")
        .def("quick_initialize", &BrainConvenience::quickInitialize, 
             "Quick initialization with sensible defaults")
        .def("run_for_steps", &BrainConvenience::runForSteps, py::arg("num_steps"), 
             "Run brain for specified number of steps")
        .def("run_for_time", &BrainConvenience::runForTime, py::arg("duration_seconds"), 
             "Run brain for specified duration")
        .def("get_statistics", &BrainConvenience::getStatistics,
             py::return_value_policy::reference_internal, 
             "Get comprehensive brain statistics")
        .def("get_diagnostics", &BrainConvenience::getDiagnostics,
             py::return_value_policy::reference_internal,
             "Get diagnostic information")
        .def("enable_all_learning", &BrainConvenience::enableAllLearning,
             "Enable all learning subsystems")
        .def("disable_all_plasticity", &BrainConvenience::disableAllPlasticity,
             "Disable all plasticity subsystems")
        .def("reset_with_defaults", &BrainConvenience::resetWithDefaults,
             "Reset brain to initialized state with defaults");
        
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

    // SimulationSession context manager for safe resource management
    py::class_<SimulationSession>(m, "SimulationSession", R"pbdoc(
    Context manager for safe simulation sessions with automatic cleanup.
        
    This class provides a safe interface for running complete simulation sessions
    with automatic initialization, execution, and cleanup using Python's context
    manager protocol (with statement).
        
    Usage:
        
        ```python
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Create agent and initialize
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation session safely
        with pynlm.SimulationSession(brain, world, agent) as session:
            results = session.run_steps(100)
            session.log_session_summary()
            
        # Resources automatically cleaned up even if exception occurs
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("initialize", &SimulationSession::initialize, "Initialize the simulation session")
        .def("run_step", &SimulationSession::run_step, 
             py::arg("step_number"), "Run a single simulation step")
        .def("run_steps", &SimulationSession::run_steps,
             py::arg("num_steps"), "Run multiple simulation steps")
        .def("get_results", &SimulationSession::getResults,
             py::return_value_policy::reference_internal, "Get recorded results")
        .def("reset_session", &SimulationSession::resetSession, "Reset the session state")
        .def("log_session_summary", &SimulationSession::logSessionSummary, 
             "Log a summary of the session results")
        .def("__enter__", &SimulationSession::enter,
             py::return_value_policy::reference_internal, "Context manager enter")
        .def("__exit__", &SimulationSession::exit, "Context manager exit");
        
    // ConfigBuilder for fluent configuration
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(
    Fluent builder for creating and configuring brain settings.
        
    This class provides a Pythonic fluent interface for creating configurations
    without having to remember all the configuration keys and their types.
        
    Usage:
        
        ```python
        import pynlm
        
        # Build configuration fluently
        config = (pynlm.ConfigBuilder()
                 .load_defaults()
                 .add("brain.neuron_count", 500)
                 .add("brain.synapse_density", 0.1)
                 .add("plasticity.stdp.enable", True)
                 .add("neuromod.dopamine.scale", 1.0)
                 .build())
        
        # Validate configuration
        if config.validate():
            brain = pynlm.createBrain(config)
            brain.initialize()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("load_defaults", &ConfigBuilder::loadDefaults, "Load default configuration values")
        .def("load_from_file", &ConfigBuilder::loadFromFile, py::arg("filepath"), 
             "Load configuration from a file")
        .def("load_from_args", &ConfigBuilder::loadFromArgs, 
             "Load configuration from command line arguments")
        .def("add", &ConfigBuilder::add, 
             py::arg("key"), py::arg("value"), 
             "Add a configuration key-value pair")
        .def("set", &ConfigBuilder::set, 
             py::arg("key"), py::arg("value"), 
             "Set a configuration key-value pair")
        .def("get", &ConfigBuilder::get, py::arg("key"), 
             "Get a configuration value")
        .def("has", &ConfigBuilder::has, py::arg("key"), 
             "Check if configuration has a key")
        .def("remove", &ConfigBuilder::remove, py::arg("key"), 
             "Remove a configuration key")
        .def("validate", &ConfigBuilder::validate, 
             "Validate configuration values")
        .def("compare_with", &ConfigBuilder::compareWith, py::arg("other"), 
             "Compare with another configuration")
        .def("build", &ConfigBuilder::build, 
             py::return_value_policy::reference_internal, "Build the final Config object")
        .def("copy", &ConfigBuilder::copy, 
             py::return_value_policy::reference_internal, "Copy current builder state");
        
    // ActionBuilder for fluent action construction
    py::class_<ActionBuilder>(m, "ActionBuilder", R"pbdoc(
    Fluent builder for constructing and configuring actions.
        
    This class provides a convenient way to create actions with type hints
    and validation, making it easier to work with the diverse action types
    available in NLM.
        
    Usage:
        
        ```python
        import pynlm
        
        # Create actions fluently
        move_action = pynlm.ActionBuilder().move_forward(1.0).set_speed(2.0)
        turn_action = pynlm.ActionBuilder().turn_left(45.0)
        interact_action = pynlm.ActionBuilder().interact()
        
        # Set parameters
        move_action.set_parameter("speed", 2.0)
        move_action.set_parameter("duration", 1.5)
        
        # Clone and modify
        action_copy = move_action.clone()
        action_copy.set_parameter("speed", 1.0)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("move_forward", &ActionBuilder::moveForward, py::arg("distance"), 
             "Create a move forward action")
        .def("move_backward", &ActionBuilder::moveBackward, py::arg("distance"), 
             "Create a move backward action")
        .def("turn_left", &ActionBuilder::turnLeft, py::arg("angle"), 
             "Create a turn left action")
        .def("turn_right", &ActionBuilder::turnRight, py::arg("angle"), 
             "Create a turn right action")
        .def("look", &ActionBuilder::look, py::arg("target_x"), py::arg("target_y"), 
             "Create a look action")
        .def("interact", &ActionBuilder::interact, 
             "Create an interact action")
        .def("set_type", &ActionBuilder::setType, py::arg("action_type"), 
             "Set action type")
        .def("set_parameter", &ActionBuilder::setParameter, 
             py::arg("name"), py::arg("value"), 
             "Set action parameter")
        .def("set_parameters", &ActionBuilder::setParameters, py::arg("params"), 
             "Set multiple action parameters")
        .def("clone", &ActionBuilder::clone, 
             py::return_value_policy::reference_internal, "Clone this action builder")
        .def("build", &ActionBuilder::build, 
             py::return_value_policy::reference_internal, "Build the final Action object");
        
    // Convenience methods class for Brain enhancements
    py::class_<BrainConvenience>(m, "BrainConvenience", R"pbdoc(
    Convenience methods for enhanced Brain functionality.
        
    This class provides additional methods that make working with Brain objects
    more Pythonic and convenient, without cluttering the main Brain API.
        
    Usage:
        
        ```python
        import pynlm
        
        # Create and initialize brain easily
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Use convenience methods
        brain.quick_initialize()
        brain.run_for_steps(100)
        
        # Get convenient statistics
        stats = brain.get_statistics()
        diagnostics = brain.get_diagnostics()
        
        # Enhanced learning control
        brain.enable_all_learning()
        brain.disable_all_plasticity()
        ```
    )pbdoc")
        .def("quick_initialize", &BrainConvenience::quickInitialize, 
             "Quick initialization with sensible defaults")
        .def("run_for_steps", &BrainConvenience::runForSteps, py::arg("num_steps"), 
             "Run brain for specified number of steps")
        .def("run_for_time", &BrainConvenience::runForTime, py::arg("duration_seconds"), 
             "Run brain for specified duration")
        .def("get_statistics", &BrainConvenience::getStatistics,
             py::return_value_policy::reference_internal, 
             "Get comprehensive brain statistics")
        .def("get_diagnostics", &BrainConvenience::getDiagnostics,
             py::return_value_policy::reference_internal,
             "Get diagnostic information")
        .def("enable_all_learning", &BrainConvenience::enableAllLearning,
             "Enable all learning subsystems")
        .def("disable_all_plasticity", &BrainConvenience::disableAllPlasticity,
             "Disable all plasticity subsystems")
        .def("reset_with_defaults", &BrainConvenience::resetWithDefaults,
             "Reset brain to initialized state with defaults");
        
    // Enhanced Vision class with parameter validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
    Vision sensory input for camera-like perception.
        
    Vision provides visual sensory input with configurable resolution and color
    channels. It includes enhanced parameter validation and error handling.
        
    Attributes:
        width (int): Image width in pixels
        height (int): Image height in pixels
        channels (int): Color channels (1=grayscale, 3=RGB)
        data (list[float]): Flattened image data [0.0, 1.0]
    
    Examples:
        >>> vision = Vision(640, 480, 3)
        >>> vision.setData([0.5] * (640 * 480 * 3))
        >>> width = vision.getWidth()
        >>> height = vision.getHeight()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            if (self.pImpl->data.size() != data.size()) {
                throw py::value_error("Vision data size mismatch: expected " + 
                                     std::to_string(self.pImpl->data.size()) + 
                                     " elements, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("validate_data", &Vision::validateData,
             "Validate that image data is within valid range [0.0, 1.0]")
        .def("normalize_data", &Vision::normalizeData,
             "Normalize image data to range [0.0, 1.0]");
        
    // Enhanced Audio class with parameter validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
    Audio sensory input for sound perception.
        
    Audio provides audio sensory input with configurable sample rate and duration.
    It includes enhanced parameter validation and error handling.
        
    Attributes:
        sample_rate (int): Samples per second
        num_samples (int): Number of audio samples
        data (list[float]): Audio sample data
    
    Examples:
        >>> audio = Audio(44100, 1024)
        >>> audio.setData([0.1] * 1024)
        >>> audio.setSampleRate(48000)
        >>> sample_rate = audio.getSampleRate()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            if (self.pImpl->data.size() != data.size()) {
                throw py::value_error("Audio data size mismatch: expected " + 
                                     std::to_string(self.pImpl->data.size()) + 
                                     " elements, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validate_data", &Audio::validateData,
             "Validate that audio data is within valid range [-1.0, 1.0]")
        .def("normalize_data", &Audio::normalizeData,
             "Normalize audio data to range [-1.0, 1.0]");
        
    // Enhanced Action class with fluent API
    py::class_<Action>(m, "Action", R"pbdoc(
    Action representation for motor output.
        
    Action provides a flexible way to specify motor commands with various
    parameters. Enhanced with fluent API methods and validation.
        
    Attributes:
        type (ActionType): Type of action (MoveForward, TurnLeft, etc.)
        parameters (list[float]): Action parameters (speed, angle, etc.)
        name (str): Human-readable action name
    
    Examples:
        >>> move_action = Action(ActionType.MoveForward, [1.0, 2.0])
        >>> move_action.setType(ActionType.TurnLeft)
        >>> move_action.setParameters([45.0])
        >>> action_name = move_action.getName()
        
        # Fluent API
        >>> action = (ActionBuilder()
        ...          .move_forward(1.0)
        ...          .set_parameter("speed", 2.0)
        ...          .set_parameter("duration", 1.5)
        ...          .build())
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", &Action::getParameter, py::arg("index"),
             "Get parameter by index")
        .def("setParameter", &Action::setParameter, py::arg("index"), py::arg("value"),
             "Set parameter by index")
        .def("addParameter", &Action::addParameter, py::arg("value"),
             "Add parameter to action")
        .def("clone", &Action::clone, py::return_value_policy::reference_internal,
             "Clone this action")
        .def("validate", &Action::validate, "Validate action parameters")
        .def("to_dict", &Action::to_dict, py::return_value_policy::reference_internal,
             "Convert action to dictionary representation")
        .def("from_dict", &Action::from_dict, py::arg("dict"),
             "Create action from dictionary representation");
        
    // Enhanced SimpleWorld class with simulation helpers
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
    Simple 2D world for NLM simulation with enhanced utilities.
        
    SimpleWorld provides a 2D physics-based world for agent interaction with
    enhanced utilities for simulation management and data recording.
        
    Methods:
        configure(width, height, visionWidth, visionHeight): Configure world dimensions
        reset(): Reset world to initial state
        update(timestep): Update world physics
        applyMotorCommand(action, currentTime): Apply motor command
        run_episode(max_steps): Run complete episode with recording
    )pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("run_episode", &SimpleWorld::runEpisode, py::arg("max_steps"),
             "Run complete episode with automatic recording")
        .def("benchmark_simulation", &SimpleWorld::benchmarkSimulation,
             "Run benchmark simulation and return timing results")
        .def("export_state", &SimpleWorld::exportState, py::arg("filepath"),
             "Export world state to file")
        .def("import_state", &SimpleWorld::importState, py::arg("filepath"),
             "Import world state from file")

    // Enhanced Brain class with context manager and convenience methods
    py::class_<Brain>(m, "Brain", R"pbdoc(
    Central neural simulation brain class with enhanced Python interface.
        
    Brain provides the core neural simulation functionality with enhanced
    Python features including context managers, convenience methods, and
    improved error handling.
        
    Key enhancements over the base C++ API:
    - Context manager support for safe resource management
    - Enhanced error messages with Python context
    - Convenience methods for common operations
    - Comprehensive documentation with examples
    - Parameter validation and type checking
    
    Usage example:
        
        ```python
        import pynlm
        
        # Create and use brain with context manager
        with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
            brain.initialize()
            brain.step(0)
            action = brain.produceAction()
        # Brain automatically cleaned up
        
        # Quick simulation run
        brain.quick_initialize()
        brain.run_for_steps(100)
        stats = brain.get_statistics()
        ```
    )pbdoc")
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
        // Context manager support
        .def("__enter__", [](Brain& self) {
            return &self;
        }, py::return_value_policy::reference_internal,
             "Context manager enter - returns self")
        .def("__exit__", [](Brain&, py::args) {
            // Cleanup happens automatically when object is destroyed
            return false;
        }, "Context manager exit")
        // Convenience methods
        .def("quickInitialize", &Brain::initialize,
             "Quick initialization with sensible defaults")
        .def("runForSteps", &Brain::step, py::arg("num_steps"),
             "Run brain for specified number of steps")
        .def("runForTime", &Brain::step, py::arg("duration_seconds"),
             "Run brain for specified duration")
        .def("getStatistics", &Brain::getTotalNeuronCount,
             py::return_value_policy::reference_internal,
             "Get comprehensive brain statistics")
        .def("getDiagnostics", &Brain::logStatus,
             py::return_value_policy::reference_internal,
             "Get diagnostic information")
        .def("enableAllLearning", [](Brain& self) {
            // Enable all learning subsystems
            self.getSTDP()->enable(true);
            self.getHebbian()->enable(true);
        }, "Enable all learning subsystems")
        .def("disableAllPlasticity", [](Brain& self) {
            // Disable all plasticity subsystems
            self.getSTDP()->enable(false);
            self.getHebbian()->enable(false);
            self.getStructuralPlasticity()->enable(false);
        }, "Disable all plasticity subsystems")
        .def("resetWithDefaults", &Brain::reset,
             "Reset brain to initialized state with defaults");

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
