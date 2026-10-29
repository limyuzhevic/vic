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
        ==============================================
        A Python binding for the NLM C++ neural simulation framework.

        This module provides a complete Python API for simulating artificial neural networks
        inspired by biological brains. The API includes classes for creating and managing
        neural networks (brains), interaction with virtual environments (worlds), and the
        agent system that connects brains to environments.

        Key Features:
        - Full control over neural network architecture
        - Event-driven spiking neural computation
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Neuromodulation for reward, curiosity, and novelty
        - Developmental processes
        - Memory systems (working, episodic, associative)
        - Prediction and planning capabilities
        - Complete environment interaction

        Usage Examples:
        ```python
        import pynlm

        # Create a basic brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Run simulation
        for step in range(100):
            brain.step(step)

        # More complex example with agent and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```

        See the documentation for detailed API reference and examples.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron ID types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.

        NeuronId provides a safe way to reference neurons within the brain simulation.
        It contains a unique value that can be used to look up specific neurons.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index part of the neuron ID")
        .def("__eq__", &NeuronId::operator==, "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two neuron IDs are different")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.

        SynapseId provides a safe way to reference synapses within the brain simulation.
        It contains a unique value that can be used to look up specific synapses.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index part of the synapse ID")
        .def("__eq__", &SynapseId::operator==, "Check if two synapse IDs are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two synapse IDs are different")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.

        RegionId provides a safe way to reference neural regions within the brain.
        Brain regions organize neurons into functional groups.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index part of the region ID")
        .def("__eq__", &RegionId::operator==, "Check if two region IDs are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two region IDs are different")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.

        PopulationId identifies groups of neurons that share common characteristics
        or functional roles within a brain region.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index part of the population ID")
        .def("__eq__", &PopulationId::operator==, "Check if two population IDs are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two population IDs are different");

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM brain model.

        Neurons are classified based on their functional role in the neural network.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Excitatory neurons that increase the firing probability of postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Inhibitory neurons that decrease the firing probability of postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory,
               "Modulatory neurons that affect overall brain state and plasticity")
        .value("Sensory", NeuronType::Sensory,
               "Sensory neurons that process external input from the environment")
        .value("Motor", NeuronType::Motor,
               "Motor neurons that generate commands for action execution")
        .value("Internal", NeuronType::Internal,
               "Internal neurons that maintain internal brain state")
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synaptic connection between neurons.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with direct current flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse for electrical coupling")
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Current developmental stage of the brain.

        The brain undergoes developmental stages that affect its plasticity and capabilities.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for sensory development")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase with optimized function")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable function")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining plasticity")
        .export_values();

    // Neuron firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest, not firing")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited and cannot fire")
        .export_values();

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be performed by an agent.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the direction of orientation")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the direction of orientation")
        .value("MoveLeft", ActionType::MoveLeft, "Turn left and move forward")
        .value("MoveRight", ActionType::MoveRight, "Turn right and move forward")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left without moving")
        .value("TurnRight", ActionType::TurnRight, "Turn right without moving")
        .value("Look", ActionType::Look, "Look in the direction of orientation")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Attempt to eat nearby food resources")
        .value("Drink", ActionType::Drink, "Attempt to drink water resources")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without taking any action")
        .value("Custom", ActionType::Custom, "Custom action type with specific parameters")
        .export_values();

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for action execution.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait/do nothing")
        .export_values();

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the world simulation.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space with no object")
        .value("Resource", WorldObjectType::Resource, "Resource that can be collected (food, water)")
        .value("Hazard", WorldObjectType::Hazard, "Hazardous object that causes damage")
        .value("Wall", WorldObjectType::Wall, "Physical barrier that cannot be passed")
        .value("Marker", WorldObjectType::Marker, "Marker object for reference or navigation")
        .export_values();

    // Configuration class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.

        The Config class manages all configuration parameters for the NLM brain simulation.
        It allows loading from files, command-line arguments, and programmatic modification.
        Configuration parameters control everything from neuron count to plasticity rules.
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Sensory input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the NLM system.

        SensoryInput represents data received from the environment, including vision,
        audio, proprioception, and internal signals. All sensory modalities inherit
        from this base class.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual perception.

        Vision represents visual data captured by the agent's sensors. It includes
        image data, dimensions, and can be processed by the brain for object recognition
        and scene understanding.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound perception.

        Audio represents sound data captured by the agent's sensors. It includes
        audio samples, sample rate, and can be processed by the brain for sound
        localization and pattern recognition.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for brain state information.

        InternalSignals represent the brain's internal state, including memory,
        prediction errors, neuromodulation levels, and other internal variables
        that can be used by the agent for decision making.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.

        Action represents a planned action that can be executed by the agent in the
        environment. It includes the action type and any associated parameters.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // World object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.

        WorldObject represents an object in the simulation world, including resources,
        hazards, walls, and markers. Objects have position, type, and associated values.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate of object center")
        .def_readwrite("y", &WorldObject::y, "Y coordinate of object center")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., energy, reward)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active in simulation");

    // Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the world.

        AgentBody represents the physical state of the agent in the world simulation,
        including position, orientation, velocity, energy, health, and other physical properties.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Current health level")
        .def_readwrite("age", &AgentBody::age, "Current age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values");

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.

        ActionResult contains information about the outcome of an action, including
        whether it was successful, any reward received, and any relevant messages.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about action result");

    // Sensory percept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data combining all sensory inputs.

        SensoryPercept represents the complete sensory input available to the agent,
        combining vision, touch, proprioception, internal signals, and audio into
        a unified percept for processing by the brain.
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

    // Simple world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.

        SimpleWorld represents the simulation environment where the agent operates.
        It provides methods for updating the world, handling agent actions, managing
        objects, and providing sensory input to the agent.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command from agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.

        Brain is the core component of NLM, representing an artificial neural network
        with spiking neurons, synapses, plasticity mechanisms, and learning capabilities.
        It provides the complete neural computation substrate for the agent.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with given configuration")
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

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.

        AgentBrain provides the interface between the NLM brain and the external world.
        It handles sensory processing, motor command decoding, reward modulation,
        and development, providing a complete agent abstraction for world interaction.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
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

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with all parameters set to sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with the given configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world simulation environment");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface for world interaction");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm