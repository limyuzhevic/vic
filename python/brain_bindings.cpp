#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../python/nlm/core/binding_helpers.hpp"

namespace py = pybind11;
namespace nlm {

void bindBrainTypes(py::module_& m) {
    m.doc() = R"pbdoc(
        Brain and Neural Types
        ------------------------
        Types and classes related to neural brain simulation.
    )pbdoc";

    // Neuron ID with better error handling
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides methods to get the neuron index and for equality comparison.
        NeuronId is a simple wrapper around uint64_t for type safety.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a NeuronId with the specified value. Must be less than 2^64-1.")
        .def_readwrite("value", &NeuronId::value, "Get or set the neuron ID value")
        .def("index", &NeuronId::index, "Get the neuron index (returns the value)")
        .def("__eq__", &NeuronId::operator==, py::is_operator(), "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, py::is_operator(), "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use as dictionary keys or in sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Get string representation of the NeuronId")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "Get string representation for easy printing");

    // Synapse ID with better error handling
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Provides methods to get the synapse index and for equality comparison.
        SynapseId is a simple wrapper around uint64_t for type safety.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a SynapseId with the specified value. Must be less than 2^64-1.")
        .def_readwrite("value", &SynapseId::value, "Get or set the synapse ID value")
        .def("index", &SynapseId::index, "Get the synapse index (returns the value)")
        .def("__eq__", &SynapseId::operator==, py::is_operator(), "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=, py::is_operator(), "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use as dictionary keys or in sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Get string representation of the SynapseId")
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        }, "Get string representation for easy printing");

    // Region ID with better error handling
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Provides methods to get the region index and for equality comparison.
        RegionId is a simple wrapper around uint64_t for type safety.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a RegionId with the specified value. Must be less than 2^64-1.")
        .def_readwrite("value", &RegionId::value, "Get or set the region ID value")
        .def("index", &RegionId::index, "Get the region index (returns the value)")
        .def("__eq__", &RegionId::operator==, py::is_operator(), "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=, py::is_operator(), "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use as dictionary keys or in sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Get string representation of the RegionId")
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        }, "Get string representation for easy printing");

    // Population ID with better error handling
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Provides methods to get the population index and for equality comparison.
        PopulationId is a simple wrapper around uint64_t for type safety.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create a PopulationId with the specified value. Must be less than 2^64-1.")
        .def_readwrite("value", &PopulationId::value, "Get or set the population ID value")
        .def("index", &PopulationId::index, "Get the population index (returns the value)")
        .def("__eq__", &PopulationId::operator==, py::is_operator(), "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=, py::is_operator(), "Check inequality with another PopulationId");

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the brain.
        
        Different neuron types have different characteristics and behaviors.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron that increases firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron that decreases firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron that affects overall brain state")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron that processes external input")
        .value("Motor", NeuronType::Motor, "Motor neuron that generates actions")
        .value("Internal", NeuronType::Internal, "Internal neuron for internal processing")
        .export_values()
        .doc() = "Enumeration of neuron types in the brain. Each type has different characteristics and behaviors.";

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types.
        
        Different synapse types have different mechanisms and effects.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse that increases firing probability")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse that decreases firing probability")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse that affects overall brain state")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse for fast, direct coupling")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse for electrical coupling")
        .export_values()
        .doc() = "Enumeration of synapse types. Each type has different mechanisms and effects on neural transmission.";

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of brain developmental stages.
        
        The brain develops and matures through distinct stages.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial brain state with default properties")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period with high plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage with refined connections")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable connections")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with potential decline")
        .export_values()
        .doc() = "Enumeration of brain developmental stages. The brain develops and matures through distinct phases with different characteristics.";

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons can be in different states that determine their activity level.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited and cannot fire")
        .export_values()
        .doc() = "Enumeration of neuron firing states. Different states determine the neuron's ability to fire and its activity level.";
}

void bindBrainClasses(py::module_& m) {
    // Config class with improved error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides configuration management for brain simulation parameters.
        Supports loading from files, command line arguments, and runtime settings.
        
        Examples:
            >>> import pynlm as nlm
            >>> config = nlm.Config()
            >>> config.set("brain.neuron_count", 100)
            >>> config.set("brain.synaptic_weight", 0.1)
            >>> print(config.summary())
            <Config: neuron_count=100, synaptic_weight=0.1, ...>
    )pbdoc")
        .def(py::init<>(), "Create a default configuration")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on error.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Returns True on success, False on error.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on error.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation of the Config.")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "Get string representation for easy printing");

    // Sensory input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        Provides interface for different types of sensory input including vision, audio, and internal signals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the input data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the sensory input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for the sensory input");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual processing.
        
        Represents visual input from the environment with width, height, and color channels.
        
        Examples:
            >>> import pynlm as nlm
            >>> vision = nlm.Vision(640, 480, 3)
            >>> data = list(range(640 * 480 * 3))
            >>> vision.setData(data)
            >>> print(f"Vision shape: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create Vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the vision data with the specified dimensions (width * height * channels)")
        .def("getWidth", &Vision::getWidth, "Get the width of the vision input in pixels")
        .def("getHeight", &Vision::getHeight, "Get the height of the vision input in pixels")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels (typically 3 for RGB)");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound processing.
        
        Represents audio input with sample rate and number of samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate of the audio input")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate of the audio input")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for brain state information.
        
        Represents internal brain state such as stress, arousal, or other neuromodulatory signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signal values");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents a motor action that the agent can perform, such as moving forward,
        looking in a direction, interacting with objects, etc.
        
        Examples:
            >>> import pynlm as nlm
            >>> action = nlm.Action(nlm.ActionType.MoveForward)
            >>> action = nlm.Action(nlm.ActionType.Custom, [0.5, 1.0])
            >>> print(f"Action type: {action.getType()}, parameters: {action.getParameters()}")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an Action with the specified type and default parameters")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an Action with the specified type and parameters")
        .def("getType", &Action::getType, "Get the type of the action")
        .def("setType", &Action::setType, py::arg("type"), "Set the type of the action")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a list")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the descriptive name of the action")
        .def("clone", &Action::clone, "Create a copy of this action");

    // World object
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the simulation environment.
        
        Represents objects in the world that the agent can interact with,
        such as resources, hazards, walls, or markers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate of the object")
        .def_readwrite("y", &WorldObject::y, "Y coordinate of the object")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object for collision detection")
        .def_readwrite("type", &WorldObject::type, "Type of the world object")
        .def_readwrite("value", &WorldObject::value, "Value associated with the object (e.g., health, energy)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active in the simulation");

    // Agent body
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state in the world.
        
        Represents the physical state of the agent in the simulation environment,
        including position, orientation, velocity, and physiological state.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position of the agent")
        .def_readwrite("y", &AgentBody::y, "Y position of the agent")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X component of velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y component of velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity in radians per second")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("reset", &AgentBody::reset, "Reset the agent body state to defaults");

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action performed in the world.
        
        Contains the outcome of an action including reward, success status, and message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success, and optional message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value received from performing the action")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the action outcome");

    // Sensory percept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data for agent processing.
        
        Contains all sensory inputs collected by the agent in a single time step,
        including vision, touch, internal signals, proprioception, and audio.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get the vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set the vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get the width of vision input")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get the height of vision input")
        .def("getTouch", &SensoryPercept::getTouch, "Get the touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set the touch input")
        .def("getInternal", &SensoryPercept::getInternal, "Get the internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set the internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get the proprioception input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set the proprioception input")
        .def("getAudio", &SensoryPercept::getAudio, "Get the audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set the audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the percept timestamp");

    // Simple world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simple 2D environment where the agent can move, interact with objects,
        and receive sensory input. The world manages physics, collisions, and agent state.
        
        Examples:
            >>> import pynlm as nlm
            >>> world = nlm.SimpleWorld(100, 100, 10, 10)
            >>> world.setAgentStart(50, 50)
            >>> percept = world.getSensoryPercept()
            >>> body = world.getAgentBody()
            >>> print(f"Agent at ({body.x}, {body.y})")
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state by the specified timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the agent's perspective")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the environment")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove the world object at the specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if the specified position is valid (within bounds and not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get the world width in pixels")
        .def("getHeight", &SimpleWorld::getHeight, "Get the world height in pixels")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get the maximum energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set the maximum energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get the energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set the energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get the current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set the random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the current random seed");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The core component of NLM that manages neural activity, learning, and action production.
        Contains regions of neurons, synapses, and implements neural dynamics and plasticity.
        
        Examples:
            >>> import pynlm as nlm
            >>> config = nlm.Config()
            >>> config.set("brain.neuron_count", 50)
            >>> brain = nlm.Brain(config)
            >>> brain.initialize()
            >>> percept = nlm.Vision(10, 10, 3)
            >>> brain.receiveSensoryInput(percept)
            >>> action = brain.produceAction()
            >>> print(f"Produced action: {action.getName()}")
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with the specified configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with only the current step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
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
             "Add a new neural region with optional name")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Agent brain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides interface for agents to interact with the brain simulation,
        processing sensory input and producing motor actions.
        
        Examples:
            >>> import pynlm as nlm
            >>> brain = nlm.Brain(config)
            >>> agent_brain = nlm.AgentBrain(brain)
            >>> agent_brain.initialize(world)
            >>> action = agent_brain.decodeMotorCommand()
            >>> print(f"Decoded action: {action.getName()}")
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with the specified brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");
}

void bindBrainFactories(py::module_& m) {
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.2);
        config->set("brain.membrane_potential_rest", -65.0);
        config->set("brain.membrane_potential_threshold", -50.0);
        config->set("brain.refractory_period", 5);
        config->set("brain.max_firing_rate", 200.0);
        config->set("brain.synaptic_weight", 0.1);
        return config;
    }, "Create a default configuration with typical values", py::arg("config") = nullptr);
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            config = factory::createDefaultConfig();
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create a new brain with configuration (uses default config if not provided)");
    
    m.def("createSimpleWorld", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        return factory::createSimpleWorld(width, height, visionWidth, visionHeight);
    }, py::arg("width").default_value(100), py::arg("height").default_value(100),
       py::arg("visionWidth").default_value(10), py::arg("visionHeight").default_value(10),
       "Create a new simple world with specified dimensions and vision parameters");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
}

void bindBrainAdvanced(py::module_& m) {
    // Advanced APIs for experienced users
    
    // Batch operations
    m.def("batchInjectCurrent", [](std::shared_ptr<Brain> brain,
                                 const std::vector<NeuronId>& neuronIds,
                                 double current) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        for (const auto& id : neuronIds) {
            TypeValidator::validateNeuronId(id);
            brain->injectCurrent(id, current);
        }
    }, py::arg("brain"), py::arg("neuron_ids"), py::arg("current"),
       "Inject current into multiple neurons simultaneously (batch operation)");
    
    m.def("batchProduceAction", [](std::shared_ptr<Brain> brain,
                                  const std::vector<Timestamp>& timestamps) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        std::vector<Action> actions;
        actions.reserve(timestamps.size());
        for (const auto& time : timestamps) {
            actions.push_back(brain->produceAction());
        }
        return actions;
    }, py::arg("brain"), py::arg("timestamps"),
       "Produce multiple actions for different time steps (batch operation)");
    
    // Visualization utilities
    m.def("getConnectivityMatrix", [](std::shared_ptr<Brain> brain) {
        return VisualizationUtils::getConnectivityMatrix(brain);
    }, py::arg("brain"), "Get brain connectivity matrix for visualization (numpy array)");
    
    m.def("getActivitySummary", [](std::shared_ptr<Brain> brain) {
        return VisualizationUtils::getActivitySummary(brain);
    }, py::arg("brain"), "Get brain activity summary as a dictionary");
    
    // NumPy integration utilities
    m.def("to_numpy_array", &numpy::to_numpy_array,
         "Convert std::vector<float> to numpy array");
    
    m.def("to_numpy_matrix", &numpy::to_numpy_matrix,
         "Convert std::vector<std::vector<float>> to numpy array");
    
    // Type validation utilities
    m.def("validateNeuronId", &TypeValidator::validateNeuronId,
         "Validate a neuron ID (throws exception if invalid)");
    
    m.def("validateRange", &TypeValidator::validateRange,
         "Validate that a value is within the specified range");
    
    m.def("validateNonNegative", &TypeValidator::validateNonNegative,
         "Validate that an integer value is non-negative");
    
    m.def("validatePositive", &TypeValidator::validatePositive,
         "Validate that a value is positive");
}

void bindBrainErrors(py::module_& m) {
    // Custom exception classes for Python
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
}

} // namespace nlm