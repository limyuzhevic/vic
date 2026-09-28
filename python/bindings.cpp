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
        });

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The Brain class implements a complete spiking neural network simulation with
        multiple memory systems, plasticity mechanisms, neuromodulation, and cognitive
        functions. It simulates the fundamental computation of biological brains using
        Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation.
        
        Core Features:
        - 500+ neurons with realistic LIF dynamics
        - Multiple interconnected brain regions
        - Working, episodic, and associative memory systems
        - STDP and Hebbian synaptic plasticity
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Developmental stages affecting plasticity
        - Structural plasticity (synaptogenesis/pruning)
        - Prediction and planning systems
        - Concept formation and attention
        - Checkpoint system for state persistence
        - Comprehensive logging and statistics
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create Brain instance with configuration.\n\n"
             "Parameters:\n"
             "  config: Configuration object containing brain parameters (neuron count, "
             "plasticity rates, etc.). Use createDefaultConfig() for standard setup.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.\n\n"
             "This creates all neurons, regions, and internal systems. Must be called "
             "before any simulation steps. Can be called multiple times to reset.\n\n"
             "Returns:\n"
             "  True if initialization succeeded, False otherwise.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.\n\n"
             "Parameters:\n"
             "  currentStep: Step number for logging and timing purposes.\n\n"
             "This advances all neural dynamics by one timestep, including:\n"
             "- Neuron membrane potential updates and spike generation\n"
             "- Synaptic input and propagation\n"
             "- Plasticity updates based on neural activity\n"
             "- Memory system updates\n"
             "- Neuromodulation effects")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.\n\n"
             "Parameters:\n"
             "  currentStep: Step number for logging.\n"
             "  currentTime: Current simulation time in seconds.\n\n"
             "Use this version when precise timing is needed for experiments or logging.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.\n\n"
             "Parameters:\n"
             "  input: Sensory input data (Vision, Audio, InternalSignals, etc.).\n\n"
             "This injects external sensory information into sensory neurons, providing"
             "the brain with data about the environment to process.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.\n\n"
             "Parameters:\n"
             "  neuron: Target neuron ID.\n"
             "  current: Current magnitude in nanoAmperes (nA).\n\n"
             "Positive current depolarizes the neuron, negative current hyperpolarizes it.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.\n\n"
             "Parameters:\n"
             "  type: Neuron type (Sensory, Motor, Excitatory, Inhibitory, etc.).\n"
             "  current: Current magnitude in nanoAmperes (nA).\n\n"
             "Useful for bulk sensory input or motor control signals.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.\n\n"
             "Returns:\n"
             "  Action: Motor action selected based on brain activity patterns.\n\n"
             "This interprets the brain's motor neuron activity and converts it into"
             "a structured Action object with type and parameters.")
        .def("reset", &Brain::reset,
             "Reset brain state.\n\n"
             "This clears all neural activity, memory contents, and internal state,
             "putting the brain back to its initial configuration. Useful for starting"
             "a new episode or experiment.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.\n\n"
             "Parameters:\n"
             "  filepath: Path to save the checkpoint file.\n\n"
             "Saves all neural weights, synaptic connections, memory contents, and "
             "internal state for later restoration with load().")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.\n\n"
             "Parameters:\n"
             "  filepath: Path to load the checkpoint file from.\n\n"
             "Restores all neural weights, synaptic connections, memory contents, and "
             "internal state from a previously saved checkpoint.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.\n\n"
             "Parameters:\n"
             "  name: Optional name for the region. If empty, a default name is generated.\n\n"
             "Returns:\n"
             "  RegionId: Unique identifier for the new region.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.\n\n"
             "Parameters:\n"
             "  id: Region identifier returned by addRegion().\n\n"
             "Returns:\n"
             "  Pointer to the NeuralRegion object, or nullptr if not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.\n\n"
             "Returns:\n"
             "  Number of neural regions in the brain.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.\n\n"
             "Returns:\n"
             "  Vector of all region identifiers.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.\n\n"
             "Returns:\n"
             "  Vector of pointers to all neural regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.\n\n"
             "Returns:\n"
             "  Total number of neurons in the brain.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.\n\n"
             "Returns:\n"
             "  Total number of synaptic connections.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.\n\n"
             "Returns:\n"
             "  Number of neurons currently firing.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.\n\n"
             "Returns:\n"
             "  Number of neurons with firing state Active.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.\n\n"
             "Returns:\n"
             "  Average firing rate in spikes per second.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.\n\n"
             "Returns:\n"
             "  Ratio of excitatory to inhibitory synaptic influence (typically 4-10:1).")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.\n\n"
             "Returns:\n"
             "  Total number of spikes generated since initialization.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.\n\n"
             "Returns:\n"
             "  Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging).")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.\n\n"
             "Parameters:\n"
             "  stage: Developmental stage to set.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.\n\n"
             "Returns:\n"
             "  Reference to the configuration object used by this brain.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.\n\n"
             "This prints current brain statistics including neuron counts, firing rates,"
             " developmental stage, and system status for debugging and monitoring.");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        The AgentBrain class serves as the bridge between the NLM brain and the external world.
        It handles sensory transduction (converting world percepts to brain inputs) and
        motor decoding (converting neural activity to world actions).
        
        Key features:
        - Processes sensory information and injects it into the brain
        - Decodes brain motor activity into executable motor commands
        - Applies neuromodulation based on reward signals
        - Manages developmental changes over time
        - Supports exploration through curiosity-driven behavior
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain instance with a brain. The brain must be initialized first.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with a world for interaction.\n\n"
             "Parameters:\n"
             "  world: The world instance to interact with. This provides the environment\n"
             "         for sensory input and motor output.\n\n"
             "The agent will use the world's dimensions and percept system to process\n"
             "sensory information and guide behavior.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size for configuration.\n\n"
             "Returns:\n"
             "  Total number of input units the brain expects (typically vision + touch + internal + proprioception)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size for configuration.\n\n"
             "Returns:\n"
             "  Number of available motor actions (typically 6: forward, backward, turn left/right, interact, wait)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.\n\n"
             "Parameters:\n"
             "  percept: Sensory percept from the world containing vision, touch, internal, and proprioception data.\n\n"
             "This method converts world-level sensory data into neural current injections,\n"
             "feeding the percept into the brain's neural network for processing.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.\n\n"
             "Returns:\n"
             "  MotorCommand: Selected motor action based on neural activity patterns.\n\n"
             "This method analyzes the activity in motor neuron populations and selects\n"
             "the most appropriate action, with curiosity-driven exploration when enabled.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.\n\n"
             "Parameters:\n"
             "  reward: Actual reward received from the environment.\n"
             "  predictedReward: Expected reward based on agent's predictions.\n\n"
             "This implements dopamine-based learning by adjusting synaptic weights\n"
             "based on prediction errors, enabling adaptive behavior.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system based on elapsed time.\n\n"
             "Parameters:\n"
             "  timestep: Time elapsed since last development update (in simulation time units).\n\n"
             "This method manages brain development over time, including changes to\n"
             "plasticity rates, synaptic stability, and overall neural organization.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage of the brain.\n\n"
             "Returns:\n"
             "  DevelopmentalStage: Current stage (Initial, CriticalPeriod, Maturation, Adult, Aging).")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.\n\n"
             "Returns:\n"
             "  Float value representing current neuromodulation state (-1.0 to 1.0),")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.\n\n"
             "Returns:\n"
             "  Float value representing exploration drive (0.0 to 1.0).")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.\n\n"
             "Returns:\n"
             "  Float value representing sensitivity to new stimuli (0.0 to 1.0).")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.\n\n"
             "Returns:\n"
             "  Float value representing difference between expected and actual outcomes.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.\n\n"
             "This clears neuromodulation state, novelty memory, and other agent-specific\n"
             "variables, preparing for a fresh simulation episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object.\n\n"
             "Returns:\n"
             "  Shared pointer to the brain instance managed by this agent.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.\n\n"
             "Parameters:\n"
             "  enable: True to enable reward-based learning, false to disable.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.\n\n"
             "Parameters:\n"
             "  enable: True to allow synapse formation/elimination, false to disable.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental changes.\n\n"
             "Parameters:\n"
             "  enable: True to allow age-dependent changes, false to freeze development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration.\n\n"
             "Parameters:\n"
             "  enable: True to enable exploration behavior, false for pure exploitation.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.\n\n"
             "Returns:\n"
             "  True if reward-based learning is active, false otherwise.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.\n\n"
             "Returns:\n"
             "  True if synapse formation/elimination is active, false otherwise.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.\n\n"
             "Returns:\n"
             "  True if age-dependent changes are active, false if development is frozen.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity-driven exploration is enabled.\n\n"
             "Returns:\n"
             "  True if exploration behavior is active, false for pure exploitation.");

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
