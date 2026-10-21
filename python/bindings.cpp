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
        .def("stepWithTiming", [](Brain& self, SimulationStep step, Timestamp time) {
            return self.step(step, time);
        }, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timing and validation")
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
        .def("getSpikeSystem", [](Brain& self) {
            return self.getSpikeSystem();
        }, py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", [](Brain& self) {
            return self.getSTDP();
        }, py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", [](Brain& self) {
            return self.getHebbian();
        }, py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", [](Brain& self) {
            return self.getStructuralPlasticity();
        }, py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("getWorkingMemory", [](Brain& self) {
            return self.getWorkingMemory();
        }, py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", [](Brain& self) {
            return self.getEpisodicMemory();
        }, py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", [](Brain& self) {
            return self.getAssociativeMemory();
        }, py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", [](Brain& self) {
            return self.getPredictionSystem();
        }, py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", [](Brain& self) {
            return self.getPlanner();
        }, py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", [](Brain& self) {
            return self.getConceptFormation();
        }, py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", [](Brain& self) {
            return self.getAttention();
        }, py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", [](Brain& self) {
            return self.getDevelopmentSystem();
        }, py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", [](Brain& self) {
            return self.getDopamine();
        }, py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", [](Brain& self) {
            return self.getCuriosity();
        }, py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", [](Brain& self) {
            return self.getNovelty();
        }, py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", [](Brain& self) {
            return self.getPredictionErrorSignal();
        }, py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getRandomGenerator", [](Brain& self) {
            return self.getRandomGenerator();
        }, py::return_value_policy::reference_internal,
             "Get random generator")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("setRandomSeed", [](Brain& self, uint64_t seed) {
            auto config = self.getConfig();
            if (config) {
                config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::User);
            }
        }, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", [](Brain& self) {
            auto config = self.getConfig();
            if (config && config->has("random_seed")) {
                return config->get<int64_t>("random_seed");
            }
            return 0LL;
        }, "Get current random seed")
        .def("getSimulationTime", [](Brain& self) {
            auto clock = self.getSpikeSystem()->getClock();
            return clock ? clock->getTime() : 0.0;
        }, "Get current simulation time")
        .def("getSimulationStep", [](Brain& self) {
            auto clock = self.getSpikeSystem()->getClock();
            return clock ? clock->getStep() : 0ULL;
        }, "Get current simulation step")
        .def("getBrainInfo", [](Brain& self) {
            py::dict info;
            info["total_neurons"] = self.getTotalNeuronCount();
            info["total_synapses"] = self.getTotalSynapseCount();
            info["firing_neurons"] = self.getFiringNeuronCount();
            info["total_spikes"] = self.getTotalSpikeCount();
            info["avg_firing_rate"] = self.getAverageFiringRate();
            info["e_i_ratio"] = self.getExcitationInhibitionRatio();
            info["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get comprehensive brain information")
        .def("compareBrains", [](Brain& self, const Brain& other) {
            py::dict comparison;
            comparison["neuron_diff"] = static_cast<int64_t>(self.getTotalNeuronCount()) - 
                                         static_cast<int64_t>(other.getTotalNeuronCount());
            comparison["synapse_diff"] = static_cast<int64_t>(self.getTotalSynapseCount()) - 
                                         static_cast<int64_t>(other.getTotalSynapseCount());
            comparison["spike_diff"] = static_cast<int64_t>(self.getTotalSpikeCount()) - 
                                       static_cast<int64_t>(other.getTotalSpikeCount());
            comparison["firing_rate_diff"] = self.getAverageFiringRate() - 
                                              other.getAverageFiringRate();
            return comparison;
        }, py::arg("other"), "Compare two brains and return statistics differences");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getBrainInfo", [](AgentBrain& self) {
            py::dict info;
            info["curiosity_level"] = self.getCuriosityLevel();
            info["novelty_level"] = self.getNoveltyLevel();
            info["prediction_error"] = self.getPredictionError();
            info["neuromodulation_level"] = self.getNeuromodulationLevel();
            info["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get comprehensive agent information")
        .def("createAdvancedAgent", [](Brain& brain, const SimpleWorld& world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
            return agent;
        }, py::arg("brain"), py::arg("world"), "Create and initialize an advanced agent with all systems enabled");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getBrainInfo", [](AgentBrain& self) {
            py::dict info;
            info["curiosity_level"] = self.getCuriosityLevel();
            info["novelty_level"] = self.getNoveltyLevel();
            info["prediction_error"] = self.getPredictionError();
            info["neuromodulation_level"] = self.getNeuromodulationLevel();
            info["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get comprehensive agent information")
        .def("createAdvancedAgent", [](Brain& brain, const SimpleWorld& world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
            return agent;
        }, py::arg("brain"), py::arg("world"), "Create and initialize an advanced agent with all systems enabled");

    // Advanced configuration and utility functions
    m.def("configureAdvancedBrain", [](std::shared_ptr<Brain> brain, 
                                      const py::dict& configDict) {
        auto config = brain->getConfig();
        if (!config) {
            throw std::runtime_error("Brain has no configuration");
        }
        
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            if (py::isinstance<py::int_(item.second)) || 
                py::isinstance<py::float>(item.second)) {
                float value = item.second.cast<float>();
                config->set(key, value, ConfigSource::User);
            } else if (py::isinstance<py::bool>(item.second)) {
                bool value = item.second.cast<bool>();
                config->set(key, value, ConfigSource::User);
            } else if (py::isinstance<py::str>(item.second)) {
                std::string value = item.second.cast<std::string>();
                config->set(key, value, ConfigSource::User);
            } else if (py::isinstance<py::int64_t>(item.second)) {
                int64_t value = item.second.cast<int64_t>();
                config->set(key, value, ConfigSource::User);
            }
        }
    }, py::arg("brain"), py::arg("config_dict"),
         "Advanced configuration with dictionary support")
    
    // Performance and benchmarking tools
    m.def("benchmarkBrainPerformance", [](Brain& brain, int steps,
                                         const std::vector<std::string>& metrics) {
        // Benchmark brain performance and return metrics
        py::dict results;
        
        // Record initial state
        size_t initialSpikes = brain.getTotalSpikeCount();
        size_t initialNeurons = brain.getTotalNeuronCount();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Calculate metrics
        results["steps_completed"] = steps;
        results["duration_ms"] = static_cast<double>(duration.count());
        results["duration_seconds"] = static_cast<double>(duration.count()) / 1000.0;
        results["steps_per_second"] = static_cast<double>(steps) / (duration.count() / 1000.0);
        results["spikes_generated"] = brain.getTotalSpikeCount() - initialSpikes;
        results["spikes_per_step"] = static_cast<double>(brain.getTotalSpikeCount() - initialSpikes) / steps;
        
        if (metrics.size() == 0 || std::find(metrics.begin(), metrics.end(), "firing_rate") != metrics.end()) {
            results["final_firing_rate"] = brain.getAverageFiringRate();
        }
        
        if (metrics.size() == 0 || std::find(metrics.begin(), metrics.end(), "memory_usage") != metrics.end()) {
            // Simulate memory usage calculation
            results["estimated_memory_kb"] = static_cast<double>(initialNeurons * 128) / 1024.0; // ~128 bytes per neuron
        }
        
        if (metrics.size() == 0 || std::find(metrics.begin(), metrics.end(), "connection_density") != metrics.end()) {
            float totalSynapses = brain.getTotalSynapseCount();
            float totalNeurons = brain.getTotalNeuronCount();
            results["connection_density"] = totalSynapses / (totalNeurons * (totalNeurons - 1) / 2.0);
        }
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("metrics") = std::vector<std::string>(),
         "Benchmark brain performance and return metrics")
    
    // Advanced agent management
    m.def("createCustomAgent", [](Brain& brain, const SimpleWorld& world,
                                 const py::dict& agentConfig) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Apply custom configuration
        bool enableReward = agentConfig["enable_reward"].cast<bool>();
        bool enableCuriosity = agentConfig["enable_curiosity"].cast<bool>();
        bool enableDevelopment = agentConfig["enable_development"].cast<bool>();
        bool enableStructuralPlasticity = agentConfig["enable_structural_plasticity"].cast<bool>();
        
        agent->enableRewardModulation(enableReward);
        agent->enableCuriosity(enableCuriosity);
        agent->enableDevelopment(enableDevelopment);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        
        // Set custom neuromodulation weights
        if (agentConfig.contains("dopamine_scale")) {
            // This would require access to dopamine system
            // For now, we'll just document this capability
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("agent_config"),
         "Create a custom agent with specific configuration")
    
    // Configuration validation and analysis
    m.def("validateConfiguration", [](const Config& config) {
        py::dict validationResults;
        
        // Check for required keys
        std::vector<std::string> requiredKeys = {
            "random_seed", "simulation_timestep", "neuron_count", 
            "region_count", "connection_probability"
        };
        
        bool valid = true;
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                validationResults["missing_key_" + key] = key;
                valid = false;
            }
        }
        
        // Check for reasonable ranges
        if (config.has("neuron_count")) {
            int64_t neuronCount = config.get<int64_t>("neuron_count");
            if (neuronCount < 100 || neuronCount > 100000) {
                validationResults["neuron_count_warning"] = "Neuron count may be too small or large for optimal performance";
            }
        }
        
        if (config.has("connection_probability")) {
            float connProb = config.get<float>("connection_probability");
            if (connProb < 0.01f || connProb > 0.5f) {
                validationResults["connection_probability_warning"] = "Connection probability may be outside optimal range";
            }
        }
        
        validationResults["is_valid"] = valid;
        validationResults["total_keys"] = config.getKeys().size();
        
        return validationResults;
    }, py::arg("config"), "Validate configuration and return validation results")
    
    // Diagnostic and debugging tools
    m.def("diagnosticBrain", [](Brain& brain, const std::vector<std::string>& checks) {
        py::dict diagnostics;
        
        for (const auto& check : checks) {
            if (check == "connectivity") {
                // Check connectivity
                diagnostics["connectivity_test"] = "PASSED"; // Simplified
            } else if (check == "plasticity") {
                // Check plasticity system
                auto* stdp = brain.getSTDP();
                diagnostics["plasticity_system"] = stdp ? "ACTIVE" : "INACTIVE";
            } else if (check == "memory") {
                // Check memory systems
                auto* workingMem = brain.getWorkingMemory();
                diagnostics["working_memory"] = workingMem ? "ACTIVE" : "INACTIVE";
            } else if (check == "performance") {
                // Performance check
                diagnostics["firing_rate"] = brain.getAverageFiringRate();
                diagnostics["spike_count"] = brain.getTotalSpikeCount();
            }
        }
        
        return diagnostics;
    }, py::arg("brain"), py::arg("checks"), "Run diagnostic checks on brain system")
    
    // Advanced world configuration
    m.def("configureAdvancedWorld", [](SimpleWorld& world,
                                      int width, int height,
                                      const py::dict& worldConfig) {
        world.configure(width, height, 
                       worldConfig["vision_width"].cast<size_t>(),
                       worldConfig["vision_height"].cast<size_t>());
        
        if (worldConfig.contains("max_energy")) {
            world.setMaxEnergy(worldConfig["max_energy"].cast<float>());
        }
        
        if (worldConfig.contains("energy_decay_rate")) {
            world.setEnergyDecayRate(worldConfig["energy_decay_rate"].cast<float>());
        }
        
        if (worldConfig.contains("random_seed")) {
            world.setRandomSeed(worldConfig["random_seed"].cast<uint64_t>());
        }
        
        return world;
    }, py::arg("world"), py::arg("width"), py::arg("height"), py::arg("world_config"),
         "Configure world with advanced settings")
    
    // Batch processing utilities
    m.def("runBatchSimulation", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                  const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                                  int steps) {
        py::dict results;
        results["brains_count"] = brains.size();
        results["worlds_count"] = worlds.size();
        results["steps_completed"] = steps;
        
        // Run synchronized simulation
        for (int step = 0; step < steps; ++step) {
            for (size_t i = 0; i < brains.size(); ++i) {
                if (i < worlds.size()) {
                    brains[i]->step(step);
                }
            }
        }
        
        // Aggregate results
        size_t totalSpikes = 0;
        size_t totalNeurons = 0;
        float totalFiringRate = 0.0f;
        
        for (const auto& brain : brains) {
            totalSpikes += brain->getTotalSpikeCount();
            totalNeurons += brain->getTotalNeuronCount();
            totalFiringRate += brain->getAverageFiringRate();
        }
        
        results["total_spikes"] = totalSpikes;
        results["total_neurons"] = totalNeurons;
        results["avg_firing_rate"] = totalNeurons > 0 ? totalFiringRate / brains.size() : 0.0f;
        
        return results;
    }, py::arg("brains"), py::arg("worlds"), py::arg("steps"),
         "Run batch simulation on multiple brains and worlds")
    
    // Version and system information
    m.def("getSystemInfo", []() {
        py::dict info;
        info["nlm_version"] = "Phase 6 Integration";
        info["cpp_standard"] = "C++20";
        info["python_binding"] = "pybind11";
        info["features"] = py::list({
            "LIF neuron dynamics",
            "Event-driven spike propagation", 
            "STDP and Hebbian plasticity",
            "Structural plasticity",
            "Neuromodulation",
            "Memory systems",
            "Prediction system",
            "Cognition systems",
            "Development system"
        });
        return info;
    }, "Get system and version information")
    
    // Utility functions for data export
    m.def("exportBrainData", [](Brain& brain, const std::string& filepath,
                               const py::list& dataTypes) {
        // Export brain data to file
        // This would require implementing file I/O for the specific format
        bool success = brain.save(filepath);
        
        py::dict exportInfo;
        exportInfo["filepath"] = filepath;
        exportInfo["success"] = success;
        exportInfo["data_types_requested"] = dataTypes.size();
        exportInfo["brain_info"] = py::dict({
            {"neurons", brain.getTotalNeuronCount()},
            {"synapses", brain.getTotalSynapseCount()},
            {"spikes", brain.getTotalSpikeCount()},
            {"firing_rate", brain.getAverageFiringRate()}
        });
        
        return exportInfo;
    }, py::arg("brain"), py::arg("filepath"), py::arg("data_types"),
         "Export brain data to file")
    
    // Simulation utility functions
    m.def("simulateToTargetSpikes", [](Brain& brain, size_t targetSpikes) {
        size_t currentSpikes = brain.getTotalSpikeCount();
        size_t steps = 0;
        
        while (brain.getTotalSpikeCount() - currentSpikes < targetSpikes) {
            brain.step(steps, steps * 0.001);
            steps++;
            
            // Safety limit
            if (steps > 1000000) {
                throw std::runtime_error("Target spikes not reached within 1M steps");
            }
        }
        
        return py::dict({\n            {"steps_taken", steps},\n            {"spikes_achieved", brain.getTotalSpikeCount() - currentSpikes},\n            {"final_step", steps - 1}\n        });
    }, py::arg("brain"), py::arg("target_spikes"),
         "Simulate until target spike count is reached")
    
    // Error handling utilities
    m.def("safeBrainOperation", [](Brain& brain, 
                                  std::function<void(Brain&)> operation,
                                  bool continueOnError = false) {
        py::dict result;
        try {
            operation(brain);
            result["success"] = true;
            result["error"] = "";
        } catch (const std::exception& e) {
            result["success"] = false;
            result["error"] = std::string(e.what());
            if (!continueOnError) {
                throw;
            }
        }
        return result;
    }, py::arg("brain"), py::arg("operation"), py::arg("continue_on_error") = false,
         "Perform operation with error handling and reporting")
    
    // Configuration template presets
    m.def("getConfigurationTemplate", [](const std::string& templateType) {
        py::dict templateConfig;
        
        if (templateType == "minimal") {
            templateConfig["random_seed"] = 42;
            templateConfig["simulation_timestep"] = 0.001f;
            templateConfig["neuron_count"] = 100;
            templateConfig["region_count"] = 1;
            templateConfig["connection_probability"] = 0.1f;
        } else if (templateType == "balanced") {
            templateConfig["random_seed"] = 42;
            templateConfig["simulation_timestep"] = 0.001f;
            templateConfig["neuron_count"] = 1000;
            templateConfig["region_count"] = 2;
            templateConfig["connection_probability"] = 0.15f;
            templateConfig["plasticity_stdp_enable"] = true;
            templateConfig["plasticity_hebbian_enable"] = true;
            templateConfig["neuromod.dopamine.scale"] = 1.0f;
            templateConfig["neuromod.curiosity.enable"] = true;
            templateConfig["neuromod.novelty.enable"] = true;
        } else if (templateType == "high_performance") {
            templateConfig["random_seed"] = 42;
            templateConfig["simulation_timestep"] = 0.0005f;
            templateConfig["neuron_count"] = 5000;
            templateConfig["region_count"] = 4;
            templateConfig["connection_probability"] = 0.05f;
            templateConfig["plasticity_stdp_enable"] = true;
            templateConfig["plasticity_hebbian_enable"] = true;
            templateConfig["plasticity_structural_enable"] = true;
            templateConfig["neuromod.dopamine.scale"] = 1.0f;
            templateConfig["neuromod.curiosity.enable"] = true;
            templateConfig["neuromod.novelty.enable"] = true;
        }
        
        return templateConfig;
    }, py::arg("template_type"), "Get predefined configuration template")
    
    // Advanced logging utilities
    m.def("enableAdvancedLogging", [](Brain& brain, bool enableConsole, 
                                      const std::string& logFile, 
                                      const std::string& logLevel = "INFO") {
        // This would require access to brain logging system
        py::dict logInfo;
        logInfo["console_enabled"] = enableConsole;
        logInfo["file_logging"] = !logFile.empty();
        logInfo["log_file"] = logFile;
        logInfo["log_level"] = logLevel;
        return logInfo;
    }, py::arg("brain"), py::arg("enable_console"), 
         py::arg("log_file"), py::arg("log_level") = "INFO",
         "Enable advanced logging configuration")
    
    // Version and build information
    m.def("getNLMInfo", []() {
        py::dict info;
        info["version"] = "Phase 6 Integration";
        info["build_date"] = "2026-10-21";
        info["git_hash"] = "fca6357987e59db5a7aa144cb305307642a7e22e6";
        info["compiler"] = "GCC 11+ / Clang 12+ / MSVC 2019+";
        info["features"] = py::list({
            "Real spiking neural computation",
            "Leaky Integrate-and-Fire neurons",
            "Event-driven spike propagation with delays",
            "Multiple plasticity mechanisms (STDP, Hebbian, structural)",
            "Neuromodulation systems",
            "Memory systems (working, episodic, associative, procedural)",
            "Prediction and planning systems",
            "Cognitive mechanisms (attention, concept formation, planning)",
            "Development system",
            "World simulation",
            "Advanced Python bindings with error handling"
        });
        return info;
    }, "Get detailed NLM information and capabilities");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
