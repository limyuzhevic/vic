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
        
        A Python binding for the NLM C++ neural simulation framework that provides
        access to a biologically-inspired artificial brain capable of learning
        and adaptation through neural dynamics, memory systems, and neuromodulation.
        
        **Overview**
        -----------
        NLM is an experimental neural system that simulates brain-like computation
        using spiking neurons, synapses, and integrated memory/cognition systems.
        Unlike traditional deep learning models, NLM implements real neural dynamics
        and develops capabilities through experience, similar to biological brains.
        
        **Available Classes**
        ---------------------
        * :class:`Brain` - Central neural simulation brain with integrated memory systems
        * :class:`Config` - Configuration management for brain parameters
        * :class:`AgentBrain` - Interface connecting brain to world for agent behavior
        * :class:`SimpleWorld` - Simple 2D world for agent-environment interaction
        * :class:`Vision` - Visual sensory input processing
        * :class:`Audio` - Audio sensory input processing
        * :class:`InternalSignals` - Internal neural signal sensory input
        * :class:`SensoryInput` - Base class for all sensory inputs
        * :class:`Action` - Motor command representation
        * :class:`WorldObject` - Objects in the simulation world
        * :class:`AgentBody` - Agent state and physics
        * :class:`ActionResult` - Result of an action in the world
        * :class:`SensoryPercept` - Processed sensory data for brain input
        
        * :class:`NeuronId` - Unique identifier for a neuron
        * :class:`SynapseId` - Unique identifier for a synapse
        * :class:`RegionId` - Unique identifier for a brain region
        * :class:`PopulationId` - Unique identifier for a neuron population
        
        **Enumeration Types**
        ---------------------
        * :enum:`NeuronType` - Types of neurons (Excitatory, Inhibitory, Sensory, Motor, etc.)
        * :enum:`SynapseType` - Types of synapses (Excitatory, Inhibitory, Modulatory, etc.)
        * :enum:`DevelopmentalStage` - Brain development stages (Initial, CriticalPeriod, etc.)
        * :enum:`FiringState` - Neural firing states (Resting, Active, Refractory, etc.)
        * :enum:`ActionType` - Action types for agent behavior
        * :enum:`MotorCommand` - Low-level motor commands
        * :enum:`WorldObjectType` - Types of objects in the world
        
        **Basic Usage Example**
        ----------------------
        ```python
        # Create a default configuration
        from pynlm import createDefaultConfig
        config = createDefaultConfig()
        
        # Customize configuration
        config.set("neuron_count", 500)
        config.set("region_count", 4)
        
        # Create and initialize the brain
        from pynlm import createBrain
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation steps
        for step in range(100):
            brain.step(step)
        
        # Create world and agent
        from pynlm import createSimpleWorld, createAgentBrain
        world = createSimpleWorld()
        agent_brain = createAgentBrain(brain)
        
        # Create a simple environment
        from pynlm import Vision
        vision_data = [0.1, 0.2, 0.3, 0.4] * 25  # 100 pixels, 4 channels
        vision = Vision(10, 10, 4)
        vision.setData(vision_data)
        
        # Process sensory input and act
        agent_brain.processSensoryInput(vision)
        action = brain.produceAction()
        
        # Clean up
        brain.logStatus()
        ```
        
        **Module Functions**
        -------------------
        * :func:`createDefaultConfig` - Create a default configuration
        * :func:`createBrain` - Create a new brain with configuration
        * :func:`createSimpleWorld` - Create a new simulation world
        * :func:`createAgentBrain` - Create an agent brain interface
        
        **Module Constants**
        --------------------
        * :data:`INVALID_NEURON_ID` - Invalid neuron identifier
        * :data:`INVALID_SYNAPSE_ID` - Invalid synapse identifier
        * :data:`INVALID_REGION_ID` - Invalid region identifier
        * :data:`INVALID_POPULATION_ID` - Invalid population identifier
        
        **Documentation**
        -----------------
        For detailed documentation, examples, and API reference, see the NLM documentation:
        https://github.com/your-repo/docs
        
        This module is part of the NLM (熙然) research project investigating
        computational brain-like systems. Please see the scientific documentation
        for more details on the underlying architecture and limitations.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file\n\n"
             "Args:\n"
             "    filepath: Path to the JSON configuration file\n\n"
             "Returns:\n"
             "    True if configuration was loaded successfully, False otherwise\n\n"
             "Examples:\n"
             "    >>> config = Config()\n"
             "    >>> config.loadFromFile('config.json')\n"
             "    >>> print(config.get('neuron_count'))")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments\n\n"
           "Args:\n"
           "    argc: Number of command line arguments\n"
           "    argv: Array of command line argument strings\n\n"
           "Returns:\n"
           "    True if arguments were processed successfully, False otherwise")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file\n\n"
             "Args:\n"
             "    filepath: Path where the JSON configuration will be saved\n\n"
             "Returns:\n"
             "    True if configuration was saved successfully, False otherwise")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists\n\n"
             "Args:\n"
             "    key: Configuration key to check\n\n"
             "Returns:\n"
             "    True if the key exists, False otherwise\n\n"
             "Examples:\n"
             "    >>> config.has('neuron_count')\n"
             "    True")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys\n\n"
             "Returns:\n"
             "    List of all configuration keys\n\n"
             "Examples:\n"
             "    >>> keys = config.getKeys()\n"
             "    >>> print(keys)")
        .def("clear", &Config::clear,
             "Clear all configuration entries\n\n"
             "Examples:\n"
             "    >>> config.clear()\n"
             "    >>> config.getKeys()  # Empty list")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration\n\n"
             "Returns:\n"
             "    String containing configuration summary\n\n"
             "Examples:\n"
             "    >>> summary = config.summary()\n"
             "    >>> print(summary)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp\n\n"
             "Args:\n"
             "    timestamp: The timestamp to set\n\n"
             "Examples:\n"
             "    >>> input.setTimestamp(100)");

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
             "Initialize the brain with configuration\n\n"
             "This method creates the neural architecture including regions, neurons, and initial connections.\n"
             "It reads configuration parameters and sets up the complete brain structure.\n\n"
             "Returns:\n"
             "    True if initialization succeeded, False otherwise\n\n"
             "Examples:\n"
             "    >>> brain = createBrain(config)\n"
             "    >>> if brain.initialize():\n"
             "    ...     print('Brain initialized successfully')\n"
             "    ... else:\n"
             "    ...     print('Brain initialization failed')")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step\n\n"
             "This method advances the neural simulation by one time step, processing:\n"
             "1. Processing pending delayed spike events\n"
             "2. Updating neuron membrane potentials (LIF dynamics)\n"
             "3. Detecting spikes and scheduling outgoing spike events\n"
             "4. Updating working memory\n"
             "5. Applying neuromodulation effects\n"
             "6. Applying plasticity rules (STDP, Hebbian)\n"
             "7. Updating episodic memory\n"
             "8. Updating prediction system\n"
             "9. Updating attention system\n"
             "10. Updating concept formation\n"
             "11. Applying structural plasticity\n"
             "12. Replaying important memories\n"
             "13. Applying development effects\n"
             "14. Collecting statistics\n\n"
             "Args:\n"
             "    currentStep: Current simulation step number\n\n"
             "Examples:\n"
             "    >>> brain.step(step)")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit timestamp\n\n"
             "This method is similar to the single-argument step() method but\n"
             "includes an explicit currentTime parameter.\n\n"
             "Args:\n"
             "    currentStep: Current simulation step number\n"
             "    currentTime: Current simulation time in seconds\n\n"
             "Examples:\n"
             "    >>> brain.step(step, time)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain\n\n"
             "This method processes sensory input from the environment\n"
             "and injects it into the brain's sensory neurons.\n\n"
             "Args:\n"
             "    input: Sensory input data (Vision, Audio, or InternalSignals)\n\n"
             "Examples:\n"
             "    >>> vision = Vision(10, 10, 4)\n"
             "    >>> vision.setData([0.1, 0.2, 0.3, 0.4] * 25)\n"
             "    >>> brain.receiveSensoryInput(vision)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron\n\n"
             "Args:\n"
             "    neuron: Target neuron ID\n"
             "    current: Current to inject in millivolts\n\n"
             "Examples:\n"
             "    >>> brain.injectCurrent(neuron_id, 5.0)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type\n\n"
             "Args:\n"
             "    type: Neuron type (e.g., NeuronType.Sensory, NeuronType.Motor)\n"
             "    current: Current to inject in millivolts\n\n"
             "Examples:\n"
             "    >>> brain.injectCurrentToNeurons(NeuronType.Sensory, 2.0)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity\n\n"
             "This method analyzes motor neuron activity and selects an appropriate action.\n"
             "The action selection is based on which motor neuron population has the highest activity.\n\n"
             "Returns:\n"
             "    Action object representing the selected action\n\n"
             "Examples:\n"
             "    >>> action = brain.produceAction()\n"
             "    >>> print(f'Action type: {action.getType()}')")
        .def("reset", &Brain::reset,
             "Reset brain state\n\n"
             "This method resets the brain to its initial state, clearing all:\n"
             "- Neural activity and membrane potentials\n"
             "- Working memory traces\n"
             "- Episodic memory episodes\n"
             "- Synaptic eligibility traces\n"
             "- Developmental stage\n\n"
             "Examples:\n"
             "    >>> brain.reset()")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)\n\n"
             "This method serializes the complete brain state including:\n"
             "- All neuron states and membrane potentials\n"
             "- All synapse weights and delays\n"
             "- Current simulation time and step\n"
             "- Regional metadata\n\n"
             "Args:\n"
             "    filepath: Path where the brain state will be saved\n\n"
             "Returns:\n"
             "    True if brain state was saved successfully, False otherwise\n\n"
             "Examples:\n"
             "    >>> brain.save('brain_checkpoint.nlm')")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file\n\n"
             "This method restores a previously saved brain state from a checkpoint file.\n"
             "It restores all neural and memory system states.\n\n"
             "Args:\n"
             "    filepath: Path to the saved brain state file\n\n"
             "Returns:\n"
             "    True if brain state was loaded successfully, False otherwise\n\n"
             "Examples:\n"
             "    >>> brain.load('brain_checkpoint.nlm')")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region\n\n"
             "Args:\n"
             "    name: Optional name for the region (defaults to empty string)\n\n"
             "Returns:\n"
             "    ID of the newly created region\n\n"
             "Examples:\n"
             "    >>> region_id = brain.addRegion('Cortex')")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID\n\n"
             "Args:\n"
             "    id: Region ID to retrieve\n\n"
             "Returns:\n"
             "    NeuralRegion object if found, None otherwise\n\n"
             "Examples:\n"
             "    >>> region = brain.getRegion(region_id)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain\n\n"
             "Returns:\n"
             "    Number of regions\n\n"
             "Examples:\n"
             "    >>> count = brain.getRegionCount()")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs\n\n"
             "Returns:\n"
             "    List of all region IDs\n\n"
             "Examples:\n"
             "    >>> ids = brain.getRegionIds()")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions\n\n"
             "Returns:\n"
             "    List of all neural regions\n\n"
             "Examples:\n"
             "    >>> regions = brain.getRegions()")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions\n\n"
             "Returns:\n"
             "    Total number of neurons in the brain\n\n"
             "Examples:\n"
             "    >>> total_neurons = brain.getTotalNeuronCount()")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions\n\n"
             "Returns:\n"
             "    Total number of synapses in the brain\n\n"
             "Examples:\n"
             "    >>> total_synapses = brain.getTotalSynapseCount()")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons\n\n"
             "Returns:\n"
             "    Number of neurons with non-zero membrane potential\n\n"
             "Examples:\n"
             "    >>> active = brain.getActiveNeuronCount()")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons\n\n"
             "Returns:\n"
             "    Number of neurons that have spiked in this step\n\n"
             "Examples:\n"
             "    >>> firing = brain.getFiringNeuronCount()")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons\n\n"
             "Returns:\n"
             "    Average firing rate in spikes per second\n\n"
             "Examples:\n"
             "    >>> avg_rate = brain.getAverageFiringRate()")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio\n\n"
             "Returns:\n"
             "    Ratio of total excitatory weight to inhibitory weight\n"
             "    Returns infinity if no inhibition, 0 if no excitation\n\n"
             "Examples:\n"
             "    >>> e_i_ratio = brain.getExcitationInhibitionRatio()")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count\n\n"
             "Returns:\n"
             "    Total number of spikes since brain initialization\n\n"
             "Examples:\n"
             "    >>> total_spikes = brain.getTotalSpikeCount()")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage\n\n"
             "Returns:\n"
             "    Current developmental stage\n\n"
             "Examples:\n"
             "    >>> stage = brain.getDevelopmentalStage()")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage\n\n"
             "Args:\n"
             "    stage: Developmental stage to set\n\n"
             "Examples:\n"
             "    >>> brain.setDevelopmentalStage(DevelopmentalStage.Adult)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration\n\n"
             "Returns:\n"
             "    Configuration object used to initialize this brain\n\n"
             "Examples:\n"
             "    >>> config = brain.getConfig()")
        .def("logStatus", &Brain::logStatus,
             "Log brain status\n\n"
             "This method logs detailed information about the current brain state\n"
             "including neural statistics, memory system status, and neuromodulation levels.\n\n"
             "Examples:\n"
             "    >>> brain.logStatus()");

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
