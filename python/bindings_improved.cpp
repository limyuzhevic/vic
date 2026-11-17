// NLM Python Bindings - Improved Structure
// This file contains all the pybind11 bindings for the NLM (Neural Learning Machine) library
// Organized into logical sections for better maintainability and documentation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

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
        NLM (Neural Learning Machine) - Python Bindings
        -----------------------------------------------
        A comprehensive Python interface for the NLM neural simulation framework.
        
        This module provides classes for:
        - Brain: The central neural computation engine
        - Config: Configuration management
        - AgentBrain: Interface between brain and world
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Sensor data handling
        - Action: Motor output commands
        - AgentBody: Agent state representation
        
        The NLM brain implements real spiking neural networks using
        Leaky Integrate-and-Fire (LIF) dynamics, STDP plasticity,
        and integrated memory/prediction systems.
    )pbdoc";

    // Python exception types for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    // Register identifier types
    registerIdTypes(m);
    
    // Register enumeration types
    registerEnumTypes(m);
    
    // Register configuration classes
    registerConfigClasses(m);
    
    // Register sensory input classes
    registerSensoryInputClasses(m);
    
    // Register motor classes
    registerMotorClasses(m);
    
    // Register world classes
    registerWorldClasses(m);
    
    // Register brain classes
    registerBrainClasses(m);
    
    // Register agent classes
    registerAgentClasses(m);
    
    // Register factory functions
    registerFactoryFunctions(m);
    
    // Register global constants and utilities
    registerGlobalUtilities(m);
}

// Helper functions to organize bindings
void registerIdTypes(py::module_& m) {
    // NeuronId binding
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Each neuron in the brain has a unique ID that can be used to reference it.
        
        Attributes:
            value (int): Internal numeric value of the neuron ID.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, R"pbdoc(
            Get the numeric index of this neuron ID.
            
            Returns:
                int: The numeric index.
        )pbdoc")
        .def("__eq__", &NeuronId::operator==, R"pbdoc(
            Check equality with another NeuronId.
            
            Args:
                other: Another NeuronId to compare against.
                
            Returns:
                bool: True if IDs are equal.
        )pbdoc")
        .def("__ne__", &NeuronId::operator!=, R"pbdoc(
            Check inequality with another NeuronId.
            
            Args:
                other: Another NeuronId to compare against.
                
            Returns:
                bool: True if IDs are not equal.
        )pbdoc")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId binding
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Each synaptic connection between neurons has a unique ID.
        
        Attributes:
            value (int): Internal numeric value of the synapse ID.
    )pbdoc")
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

    // RegionId binding
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region.
        
        Each brain region has a unique ID for organization and access.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); });

    // PopulationId binding
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Neurons are organized into populations based on type and function.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

void registerEnumTypes(py::module_& m) {
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Types of neurons in the NLM system.
        
        Neurons are categorized by their functional role in the network.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that promotes firing in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that suppresses firing in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that influences plasticity and neuromodulation")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives sensory input from the environment")
        .value("Motor", NeuronType::Motor, "Neuron that produces motor output commands")
        .value("Internal", NeuronType::Internal, "Neuron that operates within the internal brain network")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Types of synaptic connections between neurons.
        
        Synapses can be excitatory, inhibitory, or other specialized types.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Synaptic connection that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Synaptic connection that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Synaptic connection that influences plasticity")
        .value("Electrical", SynapseType::Electrical, "Gap junction connection allowing direct ion flow")
        .value("GapJunction", SynapseType::GapJunction, "Electrical synapse allowing direct current flow")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stages of the NLM brain.
        
        The brain goes through distinct developmental phases affecting its behavior and learning capacity.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for sensory learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase with refined connections")
        .value("Adult", DevelopmentalStage::Adult, "Adult phase with stable, learned connections")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase with declining plasticity")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Represents the internal state of neuron activity.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is not firing and recovering")
        .value("Active", FiringState::Active, "Neuron is currently firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited by external factors")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Types of actions that can be performed in the world.
        
        Actions represent possible movements or interactions with the environment.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left while stationary")
        .value("TurnRight", ActionType::TurnRight, "Turn right while stationary")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat available food resource")
        .value("Drink", ActionType::Drink, "Drink available water resource")
        .value("Rest", ActionType::Rest, "Rest and conserve energy")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom user-defined action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor commands for action execution.
        
        Motor commands are the fundamental units of motor output.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Do nothing (wait)")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Types of objects that can exist in the simulation world.
        
        Objects represent entities within the simulation environment.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space (no object)")
        .value("Resource", WorldObjectType::Resource, "Food or consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object causing damage")
        .value("Wall", WorldObjectType::Wall, "Obstacle that cannot be passed through")
        .value("Marker", WorldObjectType::Marker, "Special point or landmark")
        .export_values();
}

void registerConfigClasses(py::module_& m) {
    // Config class binding
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration management for the NLM system.
        
        The Config class provides a key-value store for simulation parameters
        and settings. Configuration can be loaded from files, command line,
        or programmatically.
        
        Configuration values can be strings, integers, floats, doubles, or booleans.
        
        Example:
            config = Config()
            config.set("neuron_count", 1000)
            config.set("connection_probability", 0.1)
            config.set("random_seed", 42)
            
            # Get values with type safety
            neuron_count = config.get_or("neuron_count", 1000)
            if config.has("sensory_input_size"):
                size = config.get("sensory_input_size")
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON/YAML format)")
        .def("loadFromArgs", &Config::loadFromArgs, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a formatted summary of all configuration values")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ConfigSource enum
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(
        Source of configuration values.
        
        Helps track the origin of configuration settings for precedence.
    )pbdoc")
        .value("Default", ConfigSource::Default, "Default configuration value")
        .value("File", ConfigSource::File, "Value loaded from file")
        .value("CommandLine", ConfigSource::CommandLine, "Value from command line")
        .export_values();
}

void registerSensoryInputClasses(py::module_& m) {
    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input types.
        
        SensoryInput represents data from the environment that can be processed
        by the brain. This is an abstract base class.
        
        Subclasses include Vision, Audio, and InternalSignals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(
            Get the type of this sensory input.
            
            Returns:
                int: The sensory input type.
        )pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(
            Get the raw sensory data as a vector of floats.
            
            Returns:
                list[float]: The sensory data vector.
        )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(
            Get the dimensionality of the sensory data.
            
            Returns:
                int: Number of dimensions in the data.
        )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(
            Get the timestamp when this sensory input was created.
            
            Returns:
                float: The timestamp in simulation time units.
        )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(
            Set the timestamp for this sensory input.
            
            Args:
                timestamp: The new timestamp in simulation time units.
        )pbdoc");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing camera-like vision data.
        
        Vision captures a 2D grid of light intensities representing what
        the agent sees in its environment.
        
        Attributes:
            width (int): Width of the vision grid in pixels.
            height (int): Height of the vision grid in pixels.
            channels (int): Number of color channels (typically 3 for RGB).
        
        Example:
            vision = Vision(10, 10)  # 10x10 grayscale vision
            vision.setData(list_of_floats)  # Set vision data
            width = vision.getWidth()
            height = vision.getHeight()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(
            Set the vision data.
            
            Args:
                data: Vector of float values representing pixel intensities.
                      Expected size: width * height * channels.
        )pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(
            Get the width of the vision grid.
            
            Returns:
                int: Vision width in pixels.
        )pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(
            Get the height of the vision grid.
            
            Returns:
                int: Vision height in pixels.
        )pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(
            Get the number of color channels.
            
            Returns:
                int: Number of color channels.
        )pbdoc");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Audio captures sound wave data for processing by the brain.
        
        Attributes:
            sampleRate (int): Number of samples per second.
            numSamples (int): Total number of audio samples.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(
            Set the audio data.
            
            Args:
                data: Vector of float values representing audio samples.
        )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(
            Set the audio sample rate.
            
            Args:
                sampleRate: Samples per second.
        )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(
            Get the audio sample rate.
            
            Returns:
                int: Number of samples per second.
        )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(
            Get the number of audio samples.
            
            Returns:
                int: Total number of audio samples.
        )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal neural signals representing brain state.
        
        InternalSignals captures the brain's own internal activity,
        such as neurotransmitter levels, hormone concentrations, etc.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(
            Add a signal value to the internal signals.
            
            Args:
                value: The signal value to add.
        )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(
            Clear all internal signal values.
        )pbdoc");
}

void registerMotorClasses(py::module_& m) {
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor output command representing an action to be executed.
        
        Actions specify what the agent should do in the world, including
        movement and interaction with objects.
        
        Attributes:
            type (ActionType): The type of action.
            parameters (list[float]): Optional parameters for the action.
        
        Example:
            action = Action(ActionType.MoveForward, [1.0, 0.5])  # Move forward with speed 1.0, duration 0.5
            action_type = action.getType()
            params = action.getParameters()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, R"pbdoc(
            Get the type of this action.
            
            Returns:
                ActionType: The action type.
        )pbdoc")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(
            Set the type of this action.
            
            Args:
                type: The new action type.
        )pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(
            Get the action parameters.
            
            Returns:
                list[float]: The action parameters.
        )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(
            Set the action parameters.
            
            Args:
                params: New parameters vector.
        )pbdoc")
        .def("getName", &Action::getName, R"pbdoc(
            Get a human-readable name for this action.
            
            Returns:
                str: Human-readable action name.
        )pbdoc")
        .def("clone", &Action::clone, R"pbdoc(
            Create a copy of this action.
            
            Returns:
                Action: A copy of this action.
        )pbdoc");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Representation of an object in the simulation world.
        
        World objects represent entities that exist in the simulation environment,
        such as resources, hazards, walls, etc.
        
        Attributes:
            x (float): X-coordinate of the object.
            y (float): Y-coordinate of the object.
            type (WorldObjectType): Type of the object.
            value (float): Value or strength of the object.
            radius (float): Radius of the object (for circular objects).
            active (bool): Whether the object is currently active.
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
}

void registerWorldClasses(py::module_& m) {
    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical state representation of the agent in the world.
        
        AgentBody tracks the agent's position, orientation, velocity, and
        physiological state such as energy and health.
        
        Attributes:
            x (float): X-coordinate position.
            y (float): Y-coordinate position.
            orientation (float): Direction the agent is facing (radians).
            velocityX (float): Velocity in X direction.
            velocityY (float): Velocity in Y direction.
            angularVelocity (float): Rotational velocity.
            energy (float): Current energy level.
            health (float): Current health level.
            age (float): Age in simulation time units.
            isMoving (bool): Whether the agent is currently moving.
            isTurning (bool): Whether the agent is currently turning.
            lastActionTime (float): Time since last action.
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
        .def("reset", &AgentBody::reset, R"pbdoc(
            Reset the agent body to its initial state.
        )pbdoc");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.
        
        ActionResult captures the outcome of an action, including any
        reward received and whether the action was successful.
        
        Attributes:
            reward (float): Reward value from the action (-1 to +1).
            success (bool): Whether the action succeeded.
            message (str): Optional descriptive message about the result.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic grid-based environment for agent simulation,
        including sensory input, action execution, and object management.
        
        Example:
            world = SimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            # Add a resource at position (5, 5)
            resource = WorldObject(WorldObjectType.Resource, 5, 5, value=1.0)
            world.addObject(resource)
            
            # Get agent's view
            percept = world.getSensoryPercept()
            
            # Execute an action
            action = Action(ActionType.MoveForward)
            world.applyMotorCommand(action, world.getSimulationTime())
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world by a timestep (simulating physics)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command and get the action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the agent's perspective")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove an object at the given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (within bounds and not occupied)")
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
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory perception from all modalities.
        
        SensoryPercept combines vision, touch, proprioception, and internal
        signals into a single percept that can be processed by the agent brain.
        
        The percept represents what the agent experiences at a given moment.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(
            Get visual sensory input.
            
            Returns:
                Vision: The vision component of the percept.
        )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), R"pbdoc(
            Set the visual sensory input.
            
            Args:
                vision: Vision sensory input object.
        )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(
            Get the width of the visual field.
            
            Returns:
                int: Vision width in pixels.
        )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(
            Get the height of the visual field.
            
            Returns:
                int: Vision height in pixels.
        )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(
            Get tactile/proprioceptive input.
            
            Returns:
                list[float]: Touch sensor values.
        )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), R"pbdoc(
            Set the tactile/proprioceptive input.
            
            Args:
                touch: List of touch sensor values.
        )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(
            Get internal signals (e.g., hormones, neurotransmitters).
            
            Returns:
                list[float]: Internal signal values.
        )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), R"pbdoc(
            Set internal signals.
            
            Args:
                internal: List of internal signal values.
        )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(
            Get proprioceptive (body position) input.
            
            Returns:
                list[float]: Proprioception sensor values.
        )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), R"pbdoc(
            Set proprioceptive input.
            
            Args:
                proprioception: List of proprioception sensor values.
        )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(
            Get auditory sensory input.
            
            Returns:
                Audio: The audio component of the percept.
        )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), R"pbdoc(
            Set the auditory sensory input.
            
            Args:
                audio: Audio sensory input object.
        )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(
            Get all sensory signals as a combined vector.
            
            Returns:
                list[float]: Combined sensory signal vector.
        )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(
            Get the timestamp of this percept.
            
            Returns:
                float: The timestamp.
        )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), R"pbdoc(
            Set the timestamp of this percept.
            
            Args:
                timestamp: The new timestamp.
        )pbdoc");
}

void registerBrainClasses(py::module_& m) {
    // Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain implementing spiking neural computation.
        
        The Brain class implements a complete artificial brain using Leaky
        Integrate-and-Fire (LIF) neurons with event-driven spike propagation,
        synaptic plasticity, and integrated memory systems.
        
        The brain operates as a closed-loop system processing sensory input,
        generating motor output, and learning from experience through
        neuromodulation and plasticity.
        
        Example:
            config = Config()
            brain = Brain(config)
            brain.initialize()
            
            # Process sensory input
            brain.receiveSensoryInput(vision_input)
            
            # Step the brain forward in time
            brain.step(current_step)
            
            # Get motor action
            action = brain.produceAction()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with default timestamp")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with specific timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics");
}

void registerAgentClasses(py::module_& m) {
    // AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Interface connecting NLM brain to the world.
        
        AgentBrain handles the transduction of sensory input into neural signals
        and the decoding of neural activity into motor commands. It acts as the
        bridge between the brain and the external world.
        
        AgentBrain provides the primary Python API for using NLM in applications,
        abstracting away the low-level neural computation details.
        
        Example:
            brain = Brain(config)
            agent = AgentBrain(brain)
            world = SimpleWorld()
            agent.initialize(world)
            
            # Main simulation loop
            for step in range(num_steps):
                world.update(0.1)  # Update world physics
                percept = world.getSensoryPercept()  # What agent sees
                agent.processSensoryInput(percept)  # Brain processes it
                brain.step(step)  # Brain computes response
                action = agent.decodeMotorCommand()  # Brain decides action
                world.applyMotorCommand(action, time)  # Do action
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
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // AgentBrain::MotorGroupActivity struct
    py::class_<AgentBrain::MotorGroupActivity>(m, "MotorGroupActivity", R"pbdoc(
        Information about a motor neuron group during decoding.
        
        MotorGroupActivity provides diagnostic information about which
        motor neuron groups are most active during action selection.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &AgentBrain::MotorGroupActivity::name)
        .def_readwrite("activity", &AgentBrain::MotorGroupActivity::activity)
        .def_readwrite("refractoryCount", &AgentBrain::MotorGroupActivity::refractoryCount)
        .def_readwrite("size", &AgentBrain::MotorGroupActivity::size)
        .def_readwrite("firingRate", &AgentBrain::MotorGroupActivity::firingRate)
        .def("__repr__", [](const AgentBrain::MotorGroupActivity& mga) {
            return "<MotorGroupActivity: " + std::string(mga.name) + 
                   " activity=" + std::to_string(mga.activity) + " rate=" + 
                   std::to_string(mga.firingRate) + ">";
        });

    // Method to get motor group activities
    m.def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
          py::arg("self"), R"pbdoc(
        Get detailed information about motor group activities.
        
        Returns:
            list[MotorGroupActivity]: List of motor group activities in decode order.
        )pbdoc");

    // Additional AgentBrain methods
    m.def("getThoughtTrace", &AgentBrain::getThoughtTrace, R"pbdoc(
        Get the most recent thought trace.
        
        Returns:
            str: Human-readable description of the last thought process.
    )pbdoc");

    m.def("getLearnedPreference", &AgentBrain::getLearnedPreference, R"pbdoc(
        Get the learned preference trait.
        
        Returns:
            float: Learned preference trait (-1 to +1).
    )pbdoc");

    m.def("encodeText", &AgentBrain::encodeText, py::arg("text"), R"pbdoc(
        Encode text into a sparse pattern for phase 6 text association.
        
        Args:
            text: Text string to encode.
            
        Returns:
            list[int]: Indices of neurons that should be activated for this text.
    )pbdoc");

    m.def("getInternalFiringRates", &AgentBrain::getInternalFiringRates, R"pbdoc(
        Get internal firing rates for recall probe.
        
        Returns:
            list[float]: Firing rates of internal (hidden) neurons.
    )pbdoc");

    // Diagnostic counters
    m.def("getReflexBonusCount", &AgentBrain::getReflexBonusCount, R"pbdoc(
        Get count of vision-gated reflex bonuses.
        
        Returns:
            int: Number of reflex bonuses that occurred.
    )pbdoc");

    m.def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount, R"pbdoc(
        Get count of hazard suppress events.
        
        Returns:
            int: Numberampingi suppressor events that occurred.
    )pbdoc");

    m.def("getWtaPulseCount", &AgentBrain::getWtaPulseCount, R"pbdoc(
        Get count of winner-take-all pulses.
        
        Returns:
            int: Number of WTA pulses that occurred.
    )pbdoc");

    // Planning counters
    m.def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount, R"pbdoc(
        Get count of planner overrides.
        
        Returns:
            int: Number of planner overrides.
    )pbdoc");

    m.def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount, R"pbdoc(
        Get count of planner agreements.
        
        Returns:
            int: Number of planner agreements.
    )pbdoc");

    m.def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount, R"pbdoc(
        Get count of planner decisions.
        
        Returns:
            int: Number of planner decisions.
    )pbdoc");

    m.def("getConceptBiasCount", &AgentBrain::getConceptBiasCount, R"pbdoc(
        Get count of concept biases.
        
        Returns:
            int: Number of concept biases.
    )pbdoc");

    m.def("getSelfBiasCount", &AgentBrain::getSelfBiasCount, R"pbdoc(
        Get count of self-model biases.
        
        Returns:
            int: Number of self-model biases.
    )pbdoc");

    m.def("getSocialBiasCount", &AgentBrain::getSocialBiasCount, R"pbdoc(
        Get count of social learning biases.
        
        Returns:
            int: Number of social learning biases.
    )pbdoc");

    // Cognitive system information
    m.def("getConceptCount", &AgentBrain::getConceptCount, R"pbdoc(
        Get number of learned concepts.
        
        Returns:
            int: Number of learned concepts.
    )pbdoc");

    m.def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence, R"pbdoc(
        Get self-model confidence level.
        
        Returns:
            float: Self-model confidence (0 to 1).
    )pbdoc");

    m.def("getSocialObservationCount", &AgentBrain::getSocialObservationCount, R"pbdoc(
        Get count of social observations.
        
        Returns:
            int: Number of social observations.
    )pbdoc");

    // Social learning method
    m.def("observeOtherAgent", &AgentBrain::observeOtherAgent,
          py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"), R"pbdoc(
        Observe another agent's action and sensory change.
        
        Args:
            observed: Motor command observed from the other agent.
            beforeVision: Vision before the observation.
            afterVision: Vision after the observation.
    )pbdoc");
}

void registerFactoryFunctions(py::module_& m) {
    // Factory functions for creating default objects
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration object.
        
        Returns:
            Config: New configuration with default settings.
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the given configuration.
        
        Args:
            config: Configuration to use for the brain.
            
        Returns:
            Brain: New brain instance.
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world simulation environment.
        
        Returns:
            SimpleWorld: New world instance.
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: Brain instance to connect to the world.
            
        Returns:
            AgentBrain: New agent brain instance.
    )pbdoc");
}

void registerGlobalUtilities(py::module_& m) {
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("getVersion", []() {
        return std::string("NLM 2.0.0 (Phase 6 Integration)");
    }, R"pbdoc(
        Get the NLM version string.
        
        Returns:
            str: Version information.
    )pbdoc");

    m.def("printBanner", []() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }, R"pbdoc(
        Print the NLM banner to stdout.
    )pbdoc");
}

} // namespace nlm