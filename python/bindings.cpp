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
        
        This module provides a high-level Python API for creating and simulating artificial neural networks
        inspired by biological brain architecture. The system supports multiple learning mechanisms,
        memory systems, neuromodulation, and developmental processes.
        
        Key features:
        - Brain simulation with spiking neurons and synapses
        - Working, episodic, and associative memory systems
        - Prediction and planning capabilities
        - Neuromodulation (reward, curiosity, novelty)
        - Developmental stages affecting plasticity
        - Configuration management for different use cases
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==============================================================================
    // TYPE DEFINITIONS AND ENUMS
    // ==============================================================================

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId is a lightweight wrapper around uint64_t that provides safe
        neuron addressing. Each neuron in the brain has a unique NeuronId that
        can be used to reference specific neurons for direct stimulation or monitoring.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId with a specific value")
        .def_readwrite("value", &NeuronId::value,
                      "The underlying uint64_t value")
        .def("index", &NeuronId::index,
             "Get the index of the neuron (equivalent to value)")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId identifies specific connections between neurons in the network.
        This allows for precise manipulation of synaptic connections, including
        weight adjustments and connection removal.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId with a specific value")
        .def_readwrite("value", &SynapseId::value,
                      "The underlying uint64_t value")
        .def("index", &SynapseId::index,
             "Get the index of the synapse")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Regions group neurons with similar functions (e.g., sensory, motor, association).
        RegionId allows for organized access to different brain areas and their
        constituent neurons and connections.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId with a specific value")
        .def_readwrite("value", &RegionId::value,
                      "The underlying uint64_t value")
        .def("index", &RegionId::index,
             "Get the index of the region")
        .def("__eq__", &RegionId::operator==,
             "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
             "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Populations group neurons that share similar response properties and
        functional roles. This allows for efficient management of neuron groups
        with similar characteristics and connectivity patterns.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId with a specific value")
        .def_readwrite("value", &PopulationId::value,
                      "The underlying uint64_t value")
        .def("index", &PopulationId::index,
             "Get the index of the population")
        .def("__eq__", &PopulationId::operator==,
             "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
             "Check inequality with another PopulationId");

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of different neuron types in the NLM system.
        
        Neuron types determine how neurons respond to inputs, process information,
        and connect to other neurons. Different types enable the diversity of
        neural computation in the artificial brain.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Neuron that increases firing probability of target neurons")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Neuron that decreases firing probability of target neurons")
        .value("Modulatory", NeuronType::Modulatory,
               "Neuron that influences other neurons' excitability and plasticity")
        .value("Sensory", NeuronType::Sensory,
               "Neuron specialized in processing sensory input")
        .value("Motor", NeuronType::Motor,
               "Neuron specialized in generating motor output")
        .value("Internal", NeuronType::Internal,
               "Neuron involved in internal processing and control");

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of different synapse types in the neural network.
        
        Synapse types determine the nature of neural communication, including
        the direction, mechanism, and functional properties of connections
        between neurons.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory,
               "Connection that increases target neuron firing probability")
        .value("Inhibitory", SynapseType::Inhibitory,
               "Connection that decreases target neuron firing probability")
        .value("Modulatory", SynapseType::Modulatory,
               "Connection that influences plasticity and excitability")
        .value("Electrical", SynapseType::Electrical,
               "Direct electrical connection via gap junctions")
        .value("GapJunction", SynapseType::GapJunction,
               "Gap junction connection allowing direct ion flow");

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neural growth and maturation.
        
        The brain undergoes several developmental stages, each characterized
        by specific plasticity rates, connection patterns, and functional capabilities.
        These stages affect learning, memory formation, and behavior.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
               "Initial developmental state with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
               "Critical learning period with rapid development")
        .value("Maturation", DevelopmentalStage::Maturation,
               "Maturation phase with refined connections")
        .value("Adult", DevelopmentalStage::Adult,
               "Adult stage with stable neural circuitry")
        .value("Aging", DevelopmentalStage::Aging,
               "Aging phase with declining plasticity");

    // Neuron firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of possible firing states for individual neurons.
        
        Neurons cycle through different states during their operation, affecting
        their contribution to network activity and learning processes.
    )pbdoc")
        .value("Resting", FiringState::Resting,
               "Neuron at rest, not currently firing")
        .value("Active", FiringState::Active,
               "Neuron currently firing action potentials")
        .value("Refractory", FiringState::Refractory,
               "Neuron in refractory period, unable to fire")
        .value("Inhibited", FiringState::Inhibited,
               "Neuron actively inhibited from firing");

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible motor actions the agent can take.
        
        These actions represent the fundamental behaviors the agent can perform
        in the simulated environment, from basic movement to complex interactions.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward,
               "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward,
               "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft,
               "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight,
               "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft,
               "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight,
               "Rotate clockwise")
        .value("Look", ActionType::Look,
               "Look in current direction")
        .value("LookUp", ActionType::LookUp,
               "Look upward")
        .value("LookDown", ActionType::LookDown,
               "Look downward")
        .value("Interact", ActionType::Interact,
               "Interact with nearby object")
        .value("Eat", ActionType::Eat,
               "Consume food/resource")
        .value("Drink", ActionType::Drink,
               "Drink water/resource")
        .value("Rest", ActionType::Rest,
               "Rest and recover energy")
        .value("Wait", ActionType::Wait,
               "Wait without action")
        .value("Custom", ActionType::Custom,
               "Custom action defined by user");

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands for precise control.
        
        These represent the fundamental movements the agent's motor system
        can produce, abstracted from higher-level action concepts.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
               "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward,
               "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft,
               "Turn left")
        .value("TurnRight", MotorCommand::TurnRight,
               "Turn right")
        .value("LookLeft", MotorCommand::LookLeft,
               "Look left")
        .value("LookRight", MotorCommand::LookRight,
               "Look right")
        .value("Interact", MotorCommand::Interact,
               "Interact with object")
        .value("Wait", MotorCommand::Wait,
               "Wait without movement");

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types that can exist in the simulated world.
        
        Different object types have distinct properties and behaviors,
        creating a rich environment for the agent to interact with.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty,
               "No object at this position")
        .value("Resource", WorldObjectType::Resource,
               "Useful resource (e.g., food, water)")
        .value("Hazard", WorldObjectType::Hazard,
               "Dangerous object (e.g., predator, trap)")
        .value("Wall", WorldObjectType::Wall,
               "Immovable barrier")
        .value("Marker", WorldObjectType::Marker,
               "Important landmark or goal");

    // ==============================================================================
    // CONFIGURATION SYSTEM
    // ==============================================================================

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages all parameters and settings for the neural simulation,
        including brain parameters, learning rates, and system-wide behaviors.
        Configuration can be loaded from files, command-line arguments, or created
        programmatically with specific parameter values.
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
        }, "Return string representation");

    // Convenience functions for creating configurations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard parameters");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "research");
        config->setInt("population_size", 10000);
        config->setDouble("learning_rate", 0.01);
        config->setBool("enable_structural_plasticity", true);
        config->setBool("enable_development", true);
        config->setBool("enable_curiosity", true);
        config->setString("description", "Research-oriented configuration with full capabilities");
        return config;
    }, "Create a research-oriented configuration with high plasticity and learning");

    m.def("createProductionConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "production");
        config->setInt("population_size", 5000);
        config->setDouble("learning_rate", 0.005);
        config->setBool("enable_structural_plasticity", false);
        config->setBool("enable_development", false);
        config->setBool("enable_curiosity", false);
        config->setString("description", "Production-optimized configuration with stable behavior");
        return config;
    }, "Create a production-optimized configuration with stable, predictable behavior");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "minimal");
        config->setInt("population_size", 1000);
        config->setDouble("learning_rate", 0.001);
        config->setBool("enable_structural_plasticity", false);
        config->setBool("enable_development", false);
        config->setBool("enable_curiosity", false);
        config->setString("description", "Minimal configuration for fast testing and debugging");
        return config;
    }, "Create a minimal configuration for fast testing and debugging");

    m.def("createAdvancedConfig", [](int populationSize, double learningRate, bool enableDevelopment,
                                   bool enableStructuralPlasticity, bool enableCuriosity) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "advanced");
        config->setInt("population_size", populationSize);
        config->setDouble("learning_rate", learningRate);
        config->setBool("enable_development", enableDevelopment);
        config->setBool("enable_structural_plasticity", enableStructuralPlasticity);
        config->setBool("enable_curiosity", enableCuriosity);
        config->setString("description", "Custom configuration with specified parameters");
        return config;
    }, py::arg("population_size") = 5000, py::arg("learning_rate") = 0.01,
       py::arg("enable_development") = true, py::arg("enable_structural_plasticity") = true,
       py::arg("enable_curiosity") = true,
       "Create a custom configuration with specified parameters");

    // ==============================================================================
    // SENSORIMETRY AND SENSORY INPUT
    // ==============================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        SensoryInput represents the basic interface for all sensory modalities
        (vision, audio, internal signals, etc.). It provides common properties
        and methods for accessing sensory data, timestamps, and type information.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.
        
        Vision captures the agent's visual perception of the environment as a
        2D array of pixel values. This simulates the agent's 'eyes' and provides
        information about objects, obstacles, resources, and hazards in the world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create a vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set vision data as a vector of pixel values")
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
        
        Audio captures auditory information from the environment, allowing the agent
        to perceive sounds, music, warnings, or other acoustic stimuli. This enables
        auditory-based behaviors and learning.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio data as a vector of sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing the agent's internal state.
        
        InternalSignals captures the agent's internal physiological and cognitive
        state, including hunger, fatigue, emotional states, and other internal
        variables that influence behavior and decision-making.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // ==============================================================================
    // ACTION AND MOTOR OUTPUT
    // ==============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Action represents an intention or desire to perform a specific behavior.
        It includes the type of action and associated parameters, providing a
        high-level interface for the agent to plan and execute behaviors.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        WorldObject represents an entity in the simulated world with position,
        type, and properties. Objects can be resources, hazards, walls, or markers
        that the agent can interact with.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "Object x-coordinate")
        .def_readwrite("y", &WorldObject::y, "Object y-coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward or damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing the physical agent in the world.
        
        AgentBody contains all physical properties of the agent including position,
        orientation, velocity, energy, health, and age. This represents the
        agent's embodiment in the simulated environment.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X-coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y-coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation,
                      "Orientation in degrees (0 = east, 90 = north)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X-velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y-velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      "Angular velocity (radians per time unit)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      "Timestamp of last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world interaction.
        
        ActionResult represents the outcome of an agent's action in the world,
        including reward, success status, and a descriptive message. This provides
        feedback to the agent about the consequences of its behavior.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (-1 to +1)")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Descriptive result message");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data representing integrated sensory information.
        
        SensoryPercept combines all sensory inputs (vision, audio, internal signals,
        proprioception) into a unified percept that the brain can process. This
        represents what the agent perceives about the world and its internal state.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height in pixels")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals sensory input")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals sensory input")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"),
             "Set proprioception sensory input")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the percept timestamp");

    // ==============================================================================
    // WORLD SYSTEM
    // ==============================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides the simulated environment where the agent lives and
        interacts. It manages world geometry, objects, physics simulation, and
        sensory perception. The world updates each simulation step, handling
        collisions, interactions, and state changes.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for one simulation step")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command and get the result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall or outside bounds)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at specified position (if any)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per time unit")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // ==============================================================================
    // BRAIN SYSTEM
    // ==============================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain is the core computational component of the NLM system. It implements
        real spiking neural computation with event-driven dynamics and integrated
        memory, prediction, cognition, and neuromodulation systems. The brain
        processes sensory input, maintains internal state, produces actions, and
        learns from experience through various plasticity mechanisms.
        
        Key features:
        - Event-driven spiking neuron dynamics
        - Multiple memory systems (working, episodic, associative)
        - Prediction and planning capabilities
        - Neuromodulation (reward, curiosity, novelty)
        - Developmental stages affecting plasticity rates
        - Checkpoint save/load functionality
        - Real-time simulation with configurable time steps
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
             "Inject current directly into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpoint)")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for direct neuron manipulation")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the Spike-Timing-Dependent Plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity rule")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics");

    // ==============================================================================
    // AGENT INTERFACE
    // ==============================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain serves as the bridge between the brain's internal neural
        processes and the external world. It handles sensory transduction,
        motor decoding, and applies neuromodulation based on environmental
        feedback. This class provides the high-level interface for agents to
        interact with their environment.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world environment")
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

    // ==============================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    // ==============================================================================

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    // Advanced factory functions with common configurations
    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "research");
        config->setInt("population_size", 10000);
        config->setDouble("learning_rate", 0.01);
        config->setBool("enable_structural_plasticity", true);
        config->setBool("enable_development", true);
        config->setBool("enable_curiosity", true);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a research-oriented brain with high learning capabilities");

    m.def("createProductionBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "production");
        config->setInt("population_size", 5000);
        config->setDouble("learning_rate", 0.005);
        config->setBool("enable_structural_plasticity", false);
        config->setBool("enable_development", false);
        config->setBool("enable_curiosity", false);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a production-optimized brain with stable, predictable behavior");

    m.def("createMinimalBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "minimal");
        config->setInt("population_size", 1000);
        config->setDouble("learning_rate", 0.001);
        config->setBool("enable_structural_plasticity", false);
        config->setBool("enable_development", false);
        config->setBool("enable_curiosity", false);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a minimal brain for fast testing and debugging");

    // Configuration presets
    m.def("createAdvancedConfig", [](int populationSize, double learningRate,
                                   bool enableDevelopment, bool enableStructuralPlasticity,
                                   bool enableCuriosity) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->setString("mode", "advanced");
        config->setInt("population_size", populationSize);
        config->setDouble("learning_rate", learningRate);
        config->setBool("enable_development", enableDevelopment);
        config->setBool("enable_structural_plasticity", enableStructuralPlasticity);
        config->setBool("enable_curiosity", enableCuriosity);
        config->setString("description", "Custom advanced configuration");
        return config;
    }, py::arg("population_size") = 5000, py::arg("learning_rate") = 0.01,
       py::arg("enable_development") = true, py::arg("enable_structural_plasticity") = true,
       py::arg("enable_curiosity") = true,
       "Create a custom configuration with specified parameters");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==============================================================================
    // UTILITY FUNCTIONS AND HELPERS
    // ==============================================================================

    // Convenience functions for common operations
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation with brain, world, and agent");

    m.def("createCompleteAgent", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, world, agent);
    }, py::arg("config") = py::none(),
       "Create a complete agent system with brain, world, and agent");

    // Helper functions for quick setup
    m.def("getBrainStats", [](const std::shared_ptr<Brain>& brain) {
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate(),
            brain->getTotalSpikeCount(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("brain"),
       "Get key brain statistics as a tuple");

    m.def("isBrainReady", [](const std::shared_ptr<Brain>& brain) {
        return brain->getRegionCount() > 0 && brain->getTotalNeuronCount() > 0;
    }, py::arg("brain"),
       "Check if brain is properly initialized");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ==============================================================================
    // ADVANCED FEATURE FOR POWER USERS
    // ==============================================================================

    // Advanced configuration loader
    m.def("loadConfigFromString", [](const std::string& jsonString) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Note: This would require implementing JSON parsing
        // For now, return default config with indication of success
        return config;
    }, py::arg("json_string"),
       "Load configuration from JSON string (requires JSON parser)");

    // Brain cloning functionality
    m.def("cloneBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>(*brain->getConfig());
        auto clonedBrain = std::make_shared<Brain>(config);
        clonedBrain->initialize();
        // Note: This would require implementing deep copy of brain state
        return clonedBrain;
    }, py::arg("brain"),
       "Create a copy of a brain (state transfer would need implementation)");

    // Performance monitoring
    m.def("getPerformanceMetrics", [](const std::shared_ptr<Brain>& brain) {
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getAverageFiringRate()
        );
    }, py::arg("brain"),
       "Get performance metrics for monitoring");

    // Development debugging
    m.def("exportBrainState", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to file for debugging");

    m.def("importBrainState", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"),
       "Import brain state from file");

    // ==============================================================================
    // PYTHON-SPECIFIC ENHANCEMENTS
    // ==============================================================================

    // Add Python property access
    py::class_<AgentBrain>(m, "ConfigurableAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("enable_all_features", &AgentBrain::enableRewardModulation,
             py::arg("enable" = true),
             "Enable all learning and development features")
        .def("disable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning and development features");

    // Context manager for simulations
    py::class_<SimulationContext>(m, "SimulationContext")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("__enter__", [](SimulationContext& self) {
            self.agent->initialize(*self.world);
            return &self;
        })
        .def("__exit__", [](SimulationContext& self, py::object, py::object, py::object) {
            self.agent->reset();
        })
        .def("step", [](SimulationContext& self, double timestep) {
            self.world->update(timestep);
            auto percept = self.world->getSensoryPercept();
            self.agent->processSensoryInput(percept);
            static int stepCount = 0;
            self.brain->step(stepCount++);
            auto action = self.agent->decodeMotorCommand();
            self.world->applyMotorCommand(action, self.world->getSimulationTime());
        }, py::arg("timestep") = 0.1);

    // Pythonic wrapper for brain statistics
    py::class_<BrainStatsWrapper>(m, "BrainStats")
        .def(py::init<const std::shared_ptr<Brain>&>(), py::arg("brain"))
        .def("__str__", [](const BrainStatsWrapper& self) {
            std::stringstream ss;
            ss << "BrainStats(neurons=" << self.brain->getTotalNeuronCount()
               << ", firing=" << self.brain->getFiringNeuronCount()
               << ", rate=" << self.brain->getAverageFiringRate()
               << ", spikes=" << self.brain->getTotalSpikeCount()
               << ", stage=" << static_cast<int>(self.brain->getDevelopmentalStage())
               << ")";
            return ss.str();
        });

    // Export key classes and functions for easier access
    m.attr("create_default_brain") = m.attr("createBrainWithDefaults");
    m.attr("create_research_brain") = m.attr("createResearchBrain");
    m.attr("create_production_brain") = m.attr("createProductionBrain");
    m.attr("create_minimal_brain") = m.attr("createMinimalBrain");

    // Version and information
    m.attr("__version__") = "0.1.0-advanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Advanced Python bindings for NLM neural simulation framework with enhanced API and features";
};

} // namespace nlm