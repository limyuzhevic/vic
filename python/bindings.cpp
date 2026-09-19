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
             "Load configuration from a JSON file. Configuration keys include:\n"
             "  - neuron_count: Total number of neurons (default: 1000)\n"
             "  - region_count: Number of brain regions (default: 1)\n"
             "  - connection_probability: Connection probability (default: 0.1)\n"
             "  - stdp_ltp_weight: STDP LTP weight (default: 0.01)\n"
             "  - stdp_ltd_weight: STDP LTD weight (default: 0.012)\n"
             "  - stdp_tau: STDP time constant (default: 20.0)\n"
             "  - synaptogenesis_rate: Synaptogenesis rate (default: 0.0001)\n"
             "  - pruning_rate: Pruning rate (default: 0.00001)\n"
             "  - simulation_timestep: Time step duration (default: 0.001)\n"
             "  - replay_interval: Memory replay interval (default: 100)\n"
             "  - consolidation_interval: Memory consolidation interval (default: 1000)\n"
             "  - checkpoint_dir: Directory for checkpoints (default: './checkpoints')")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments.\n"
           "Example: --neuron-count 2000 --region-count 2 --connection-probability 0.05")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.\n"
             "The saved configuration can be reloaded later using loadFromFile.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.\n"
             "Common keys: 'neuron_count', 'region_count', 'connection_probability'")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a Python list.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration showing all current settings.")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration.\n"
             "The config should contain settings like neuron_count, region_count, "
             "connection_probability, etc. Use createDefaultConfig() for default settings.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.\n"
             "Must be called before using the brain. Creates neurons and sets up "
             "all integrated systems (memory, prediction, cognition, neuromodulation).")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.\n"
             "This advances the neural dynamics for one time step, including:\n"
             "  - Spike generation and propagation\n"
             "  - Memory updates\n"
             "  - Plasticity changes\n"
             "  - Neuromodulation effects\n"
             "  - Development updates")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit timestamp.\n"
             "Same as step(currentStep) but allows specifying time for precision.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.\n"
             "The input will be distributed across sensory neurons to influence processing.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron.\n"
             "This can be used for external stimulation or debugging.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.\n"
             "Types: NeuronType.Sensory, NeuronType.Motor, NeuronType.Internal")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.\n"
             "Uses the motor neuron population with highest average activity to determine action.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions.\n"
             "Useful for starting a new simulation episode.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.\n"
             "Saves neural activity, synapse weights, and memory states for later loading.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.\n"
             "Restores all neural and memory states. Requires initialize() before loading.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.\n"
             "Regions organize neurons into functional areas. Name is optional.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.\n"
             "Returns None if region ID doesn't exist.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a Python list.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a Python list.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons (potential firing).")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (spikes/second).")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.\n"
             "Higher values indicate more excitation than inhibition.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since brain initialization.")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (spikes waiting to be processed).")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.\n"
             "Stages: Initial, CriticalPeriod, Maturation, Adult, Aging")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage manually.\n"
             "Useful for testing different developmental phases.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration used to create this brain.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status including neuron count, firing rate, memory usage, etc.\n"
             "Useful for debugging and monitoring simulations.");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface.\n"
             "The agent brain connects the neural brain to a world simulation, "
             "enabling perception, action, and learning.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with a world.\n"
             "Must be called after creating the agent. Establishes the "
             "brain-world connection for simulation.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.\n"
             "Returns the dimensionality of sensory input that should be "
             "provided by the world.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.\n"
             "Returns the dimensionality of motor commands that the brain "
             "will produce.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.\n"
             "The agent receives sensory information from the world, "
             "converts it to neural input, and injects it into the brain "
             "for processing.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.\n"
             "The agent's motor neurons are analyzed to produce an "
             "action that can be applied to the world.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.\n"
             "The agent learns from experience by adjusting brain "
             "excitability and plasticity based on rewards and prediction errors.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.\n"
             "Advances the brain's development stage, affecting plasticity "
             "rates and capabilities.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.\n"
             "Brain development stages: Initial, CriticalPeriod, Maturation, "
             "Adult, Aging")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.\n"
             "Combines effects of dopamine, curiosity, novelty, and other "
             "modulators on brain activity.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.\n"
             "Higher values indicate more exploratory behavior and "
             "seeking of novel experiences.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.\n"
             "Measures how novel recent sensory input is compared to "
             "expectations.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.\n"
             "Difference between predicted and actual sensory input. "
             "Drives learning and adaptation.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.\n"
             "Clears agent state to start a new simulation episode "
             "with fresh brain configuration.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.\n"
             "Returns the NLM brain instance used by this agent.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.\n"
             "When enabled, the agent learns from rewards and can "
             "adjust behavior based on outcomes.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.\n"
             "When enabled, the brain can grow new connections and "
             "eliminate old ones, changing its structure over time.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development.\n"
             "When enabled, the brain matures over time, changing "
             "its capabilities and plasticity rates.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration.\n"
             "When enabled, the agent seeks novel and interesting "
             "experiences to learn about the world.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.");

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

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError)
        .def("reset", &PredictionError::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection for curiosity)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("update", &Novelty::update, py::arg("value"))
        .def("isNovel", &Novelty::isNovel);

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager for saving/loading brain state)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure, py::arg("directory"), py::arg("maxCheckpoints") = 10,
             py::arg("maxCheckpointSize") = 1024, py::arg("compressionEnabled") = true)
        .def("save", &CheckpointManager::save, py::arg("step"), py::arg("time"),
             "Save a checkpoint at the current simulation step.")
        .def("load", &CheckpointManager::load, py::arg("step"), py::arg("time"),
             "Load a checkpoint for the given simulation step.")
        .def("getCurrentCheckpoint", &CheckpointManager::getCurrentCheckpoint)
        .def("getCheckpointCount", &CheckpointManager::getCheckpointCount)
        .def("clear", &CheckpointManager::clear)
        .def("update", &CheckpointManager::update, py::arg("step"), py::arg("time"));

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with all standard settings.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with the given configuration.\n"
       "Example:\n"
       "  config = createDefaultConfig()\n"
       "  config.set('neuron_count', 2000)\n"
       "  brain = createBrain(config)\n"
       "  brain.initialize()")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple 2D world for agent simulation.\n"
       "The world provides sensory input and motor output interfaces."

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface.\n"
       "The agent brain connects the neural brain to a world simulation,"
       " enabling perception, action, and learning.")

    // Constants for easy use
    m.attr("VERSION") = "0.1.0";
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;

    // Helper functions for common operations
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        agent->initialize(*world);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step, step * 0.1);
            agent->decodeMotorCommand();
            world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete agent episode with the given brain, world, and agent.\n"
       "This is a convenience function that combines the standard simulation loop.");

    // Constants for neuron types
    m.def("getNeuronTypes", []() {
        py::list types;
        types.append(NeuronType::Excitatory);
        types.append(NeuronType::Inhibitory);
        types.append(NeuronType::Sensory);
        types.append(NeuronType::Motor);
        types.append(NeuronType::Internal);
        types.append(NeuronType::Modulatory);
        return types;
    }, "Get all available neuron types as a Python list.");

    // Constants for action types
    m.def("getActionTypes", []() {
        py::list actions;
        actions.append(ActionType::MoveForward);
        actions.append(ActionType::MoveBackward);
        actions.append(ActionType::TurnLeft);
        actions.append(ActionType::TurnRight);
        actions.append(ActionType::Look);
        actions.append(ActionType::Interact);
        actions.append(ActionType::Eat);
        actions.append(ActionType::Drink);
        actions.append(ActionType::Rest);
        actions.append(ActionType::Wait);
        return actions;
    }, "Get all available action types as a Python list.");

    // Constants for developmental stages
    m.def("getDevelopmentalStages", []() {
        py::list stages;
        stages.append(DevelopmentalStage::Initial);
        stages.append(DevelopmentalStage::CriticalPeriod);
        stages.append(DevelopmentalStage::Maturation);
        stages.append(DevelopmentalStage::Adult);
        stages.append(DevelopmentalStage::Aging);
        return stages;
    }, "Get all developmental stages as a Python list.");

    // Workspace for temporary files
    m.def("getWorkspacePath", []() {
        return std::string("/tmp/agent_054dd096-c881-4c5c-986a-52448d1ab7b1");
    }, "Get the workspace path for temporary files and checkpoints.");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
