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

    // Add comprehensive network analysis tools to Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        
        // Network analysis and statistics methods
        .def("analyzeNetworkTopology", &Brain::analyzeNetworkTopology,
             "Analyze network topology and return structural metrics")
        .def("getBrainStateStatistics", &Brain::getBrainStateStatistics,
             "Get comprehensive brain state statistics (entropy, complexity, etc.)")
        .def("getLearningCurve", &Brain::getLearningCurve,
             py::arg("startStep"), py::arg("endStep"),
             "Get learning progress over time period")
        .def("getStructuralConnectivity", &Brain::getStructuralConnectivity,
             "Get structural connectivity matrix for visualization")
        .def("getNeuronTypeDistribution", &Brain::getNeuronTypeDistribution,
             "Get distribution of neuron types across the network")
        .def("getPlasticityMetrics", &Brain::getPlasticityMetrics,
             "Get current plasticity activity and rates")
        .def("detectLearning", &Brain::detectLearning,
             "Detect if meaningful learning is occurring")
        .def("exportNetworkData", &Brain::exportNetworkData,
             py::arg("format") = std::string("json"),
             "Export network data in specified format (json, csv, etc.)");

    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron unit)pbdoc")
        .def(py::init<>())
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("getResetPotential", &Neuron::getResetPotential, "Get reset potential")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("getFiringState", &Neuron::getFiringState, "Get firing state")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("getReversalPotential", &Neuron::getReversalPotential, "Get reversal potential")
        .def("getAHPCurrent", &Neuron::getAHPCurrent, "Get afterhyperpolarization current")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("v"), "Set membrane potential")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("v"), "Set resting potential")
        .def("setThreshold", &Neuron::setThreshold, py::arg("v"), "Set firing threshold")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("v"), "Set reset potential")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("g"), "Set leak conductance")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("period"), "Set refractory period")
        .def("setReversalPotential", &Neuron::setReversalPotential, py::arg("v"), "Set reversal potential")
        .def("setAHPCurrent", &Neuron::setAHPCurrent, py::arg("i"), "Set afterhyperpolarization current")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject current into neuron")
        .def("update", &Neuron::update, py::arg("dt"), "Update neuron state")
        .def("reset", &Neuron::reset, "Reset neuron to resting state")
        .def("checkForSpike", &Neuron::checkForSpike, "Check if neuron should fire")
        .def("getSpikingProbability", &Neuron::getSpikingProbability, py::arg("dt"), "Get probability of firing in time step")
        .def("getInputCurrent", &Neuron::getInputCurrent, "Get input current")
        .def("getMembraneCurrent", &Neuron::getMembraneCurrent, "Get membrane current")
        .def("getIonChannelStates", &Neuron::getIonChannelStates, "Get ion channel states for debugging")
        .def("exportState", &Neuron::exportState, "Export neuron state for serialization")
        .def("importState", &Neuron::importState, py::arg("state"), "Import neuron state from serialization");

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Connection between two neurons)pbdoc")
        .def(py::init<>())
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuronId", &Synapse::getSourceNeuronId, "Get source neuron ID")
        .def("getTargetNeuronId", &Synapse::getTargetNeuronId, "Get target neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("getReversalPotential", &Synapse::getReversalPotential, "Get reversal potential")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("setPlasticityFlags", &Synapse::setPlasticityFlags, py::arg("flags"), "Set plasticity flags")
        .def("setReversalPotential", &Synapse::setReversalPotential, py::arg("v"), "Set reversal potential")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("structural"), "Enable plasticity mechanisms")
        .def("disablePlasticity", &Synapse::disablePlasticity, "Disable all plasticity mechanisms")
        .def("update", &Synapse::update, py::arg("dt"), "Update synapse state")
        .def("getInputStrength", &Synapse::getInputStrength, "Get input strength to target neuron")
        .def("getOutputContribution", &Synapse::getOutputContribution, "Get contribution to source neuron")
        .def("getTransmissionDelay", &Synapse::getTransmissionDelay, "Get transmission delay for spiking")
        .def("getDepressionPotentiationRatio", &Synapse::getDepressionPotentiationRatio, "Get depression/potentiation ratio")
        .def("exportState", &Synapse::exportState, "Export synapse state for serialization")
        .def("importState", &Synapse::importState, py::arg("state"), "Import synapse state from serialization");

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Biological brain region containing neural populations)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("getRegionType", &NeuralRegion::getRegionType, "Get region type")
        .def("getNeuronCount", &NeuralRegion::getNeuronCount, "Get neuron count")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get synapse count")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("count"), py::arg("type"), "Add neuron population")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), "Get population by ID")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get population count")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons")
        .def("getAllSynapses", &NeuralRegion::getAllSynapses, "Get all synapses")
        .def("getSensoryNeurons", &NeuralRegion::getSensoryNeurons, "Get sensory neurons")
        .def("getMotorNeurons", &NeuralRegion::getMotorNeurons, "Get motor neurons")
        .def("getInternalNeurons", &NeuralRegion::getInternalNeurons, "Get internal neurons")
        .def("injectCurrentToType", &NeuralRegion::injectCurrentToType, py::arg("type"), py::arg("current"), "Inject current to neurons of type")
        .def("getAverageMembranePotential", &NeuralRegion::getAverageMembranePotential, "Get average membrane potential")
        .def("getFiringRate", &NeuralRegion::getFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionBalance", &NeuralRegion::getExcitationInhibitionBalance, "Get E/I balance")
        .def("exportNetworkConnectivity", &NeuralRegion::exportNetworkConnectivity, "Export network connectivity matrix")
        .def("importNetworkConnectivity", &NeuralRegion::importNetworkConnectivity, py::arg("connectivity"), "Import network connectivity matrix");

    // Factory methods and enhanced error handling
    m.def("createBrainWithRegions", [](std::shared_ptr<Config> config, 
                                     const std::vector<std::string>& regionNames,
                                     const std::vector<size_t>& neuronCounts,
                                     const std::vector<NeuronType>& neuronTypes) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain in factory method");
        }
        
        // Create regions based on parameters
        size_t totalNeurons = 0;
        for (size_t i = 0; i < regionNames.size(); ++i) {
            RegionId regionId = brain->addRegion(regionNames[i]);
            auto* region = brain->getRegion(regionId);
            if (region && neuronCounts.size() > i && neuronTypes.size() > i) {
                // Note: This is a simplified factory - real implementation would need
                // population creation with specific neuron counts and types
                region->addPopulation(neuronCounts[i], neuronTypes[i]);
            }
        }
        
        return brain;
    }, py::arg("config"), py::arg("region_names"), py::arg("neuron_counts"), py::arg("neuron_types"),
        "Create a brain with pre-configured regions");

    m.def("createConfigWithDefaults", []() {
        auto config = std::make_shared<Config>();
        
        // Set default values for a realistic neural simulation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return config;
    }, "Create a configuration with reasonable defaults");

    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        bool isValid = brain->validateBrainState();
        if (!isValid) {
            throw std::runtime_error("Brain state validation failed");
        }
        
        return true;
    }, py::arg("brain"), "Validate brain state and throw exception if invalid");

    m.def("createNeuronsWithTopology", [](size_t count, const std::vector<NeuronType>& types,
                                         const std::vector<RegionId>& regions,
                                         const std::vector<PopulationId>& populations) {
        std::vector<std::shared_ptr<Neuron>> neurons;
        neurons.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            auto neuron = std::make_shared<Neuron>();
            
            // Set neuron properties based on input
            if (i < types.size()) {
                neuron->setType(types[i]);
            }
            if (i < regions.size()) {
                // Note: This would require accessing region's population
                // For now, just set a placeholder
                neuron->setRegionId(regions[i]);
            }
            
            neurons.push_back(neuron);
        }
        
        return neurons;
    }, py::arg("count"), py::arg("types"), py::arg("regions"), py::arg("populations"),
        "Create a collection of neurons with specified topology");

    m.def("analyzeLearningProgress", [](std::shared_ptr<Brain> brain, 
                                       size_t windowSize) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        auto analysis = brain->analyzeLearningProgress(windowSize);
        return analysis;
    }, py::arg("brain"), py::arg("window_size"),
        "Analyze learning progress over a time window and return metrics");

    // Enhanced error handling with custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Performance and debugging utilities
    m.def("benchmarkNetworkAnalysis", [](std::shared_ptr<Brain> brain) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Perform various network analyses
        auto topology = brain->analyzeNetworkTopology();
        auto statistics = brain->getBrainStateStatistics();
        auto learningCurve = brain->getLearningCurve(0, brain->getTotalSpikeCount());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, double> results;
        results["topology_analysis_time_ms"] = static_cast<double>(duration.count());
        results["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        results["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        results["learning_score"] = statistics.learningScore;
        results["network_efficiency"] = topology.efficiency;
        
        return results;
    }, py::arg("brain"), "Benchmark network analysis performance");

    m.def("exportCompleteNetworkData", [](std::shared_ptr<Brain> brain,
                                        const std::string& baseFilename) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        // Export in multiple formats for comprehensive analysis
        brain->exportNetworkData("json", baseFilename + ".json");
        brain->exportNetworkData("csv", baseFilename + ".csv");
        brain->exportNetworkData("npy", baseFilename + ".npy");
        
        return std::vector<std::string>{
            baseFilename + ".json",
            baseFilename + ".csv",
            baseFilename + ".npy"
        };
    }, py::arg("brain"), py::arg("base_filename"),
        "Export complete network data in multiple formats for analysis");

    // Educational and research utilities
    m.def("createTutorialBrain", []() {
        auto config = std::make_shared<Config>();
        
        // Configure for educational/tutorial purposes
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Add simple tutorial regions
        brain->addRegion("SensoryRegion");
        brain->addRegion("MotorRegion");
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize tutorial brain");
        }
        
        return brain;
    }, "Create a brain configured for educational/tutorial purposes");

    m.def("createResearchBrain", [](const std::string& configFile = "configs/research.cfg") {
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(configFile)) {
            // Use research defaults if config file not found
            config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
            config->set("simulation_timestep", 0.0001, ConfigSource::Default); // Higher precision
            config->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default); // Larger network
            config->set("region_count", static_cast<int64_t>(10), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
            config->set("stdp_tau", 50.0f, ConfigSource::Default); // Different STDP parameters
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize research brain");
        }
        
        return brain;
    }, py::arg("config_file") = "configs/research.cfg", "Create a brain configured for research purposes");

    // Statistics and metrics utilities
    m.def("computeNetworkStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        std::map<std::string, double> stats;
        
        // Basic statistics
        stats["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["region_count"] = static_cast<double>(brain->getRegionCount());
        stats["active_neuron_ratio"] = static_cast<double>(brain->getActiveNeuronCount()) / 
                                      static_cast<double>(brain->getTotalNeuronCount());
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        // Performance metrics
        stats["spike_efficiency"] = static_cast<double>(brain->getTotalSpikeCount()) / 
                                  static_cast<double>(brain->getTotalNeuronCount() * 100);
        
        return stats;
    }, py::arg("brain"), "Compute comprehensive network statistics");

    m.def("visualizeNetworkTopology", [](std::shared_ptr<Brain> brain,
                                        const std::string& outputFile) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        auto connectivity = brain->getStructuralConnectivity();
        brain->exportNetworkData("graphml", outputFile);
        
        return std::map<std::string, std::string>{
            {"format", "graphml"},
            {"node_count", std::to_string(brain->getTotalNeuronCount())},
            {"edge_count", std::to_string(brain->getTotalSynapseCount())},
            {"output_file", outputFile}
        };
    }, py::arg("brain"), py::arg("output_file"), "Generate network topology visualization");

    // Advanced analysis functions
    m.def("detectNetworkDynamics", [](std::shared_ptr<Brain> brain,
                                    size_t analysisWindow) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        auto dynamics = brain->detectNetworkDynamics(analysisWindow);
        return dynamics;
    }, py::arg("brain"), py::arg("analysis_window"), "Detect network dynamics patterns");

    m.def("predictNetworkBehavior", [](std::shared_ptr<Brain> brain,
                                     size_t stepsAhead) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null");
        }
        
        auto predictions = brain->predictNetworkBehavior(stepsAhead);
        return predictions;
    }, py::arg("brain"), py::arg("steps_ahead"), "Predict future network behavior");


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
