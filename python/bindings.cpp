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
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision sensor")
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
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic convenience methods
        .def("placeResource", &SimpleWorld::addObject,
             "Place a resource object (convenience method)")
        .def("placeHazard", &SimpleWorld::addObject,
             "Place a hazard object (convenience method)")
        .def("removeObjectAt", &SimpleWorld::removeObject,
             "Remove object at given position (alias for removeObject)")
        .def("isOccupied", &SimpleWorld::isValidPosition,
             "Check if position is occupied by object (convenience method)");

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // Phase 5 and Phase 6 AgentBrain methods
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace (Phase 5)")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
             "Get vision-gated reflex bonus count (Phase 5)")
        .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
             "Get hazard suppression count (Phase 5)")
        .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
             "Get winner-take-all pulse count (Phase 5)")
        // Phase 6 text-as-a-sense functionality
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
             "Encode text as sensory pattern (Phase 6)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates (Phase 6)")
        .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
             "Get planner override count (Phase 6)")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
             "Get planner agreement count (Phase 6)")
        .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
             "Get planner decision count (Phase 6)")
        .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
             "Get concept bias count (Phase 6)")
        .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
             "Get self bias count (Phase 6)")
        .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
             "Get social bias count (Phase 6)")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get concept count (Phase 6)")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self model confidence (Phase 6)")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get social observation count (Phase 6)")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent's action (Phase 6)")
        // Pythonic convenience methods
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities for diagnostics");

    // Global helper functions - Basic factory functions
    m.def("encodeText", &AgentBrain::encodeText, py::arg("text"),
          "Encode text as sensory pattern (Phase 6) - static helper");

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

    // Advanced factory functions - Complete example setups
    m.def("createExampleSetup", []() {
        // Create a complete example setup for easy testing
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete example setup with brain, world, and agent");

    m.def("createLearningSetup", [](bool enableCuriosity = true, bool enableRewardModulation = true,
                                   bool enableStructuralPlasticity = false) {
        // Create a setup optimized for learning and development
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30.0f, 30.0f, 16, 16);
        world->reset();
        world->setAgentStart(15.0f, 15.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure learning features
        agent->enableCuriosity(enableCuriosity);
        agent->enableRewardModulation(enableRewardModulation);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("enableCuriosity") = true, py::arg("enableRewardModulation") = true,
       py::arg("enableStructuralPlasticity") = false,
       "Create a learning-optimized setup with configurable features");

    m.def("createQuickTestSetup", []() {
        // Minimal setup for quick testing
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10.0f, 10.0f, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a minimal quick-test setup");

    // Sensory input factory functions
    m.def("createVisionInput", [](size_t width, size_t height, size_t channels = 3,
                                 const std::vector<float>& data = {}) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        if (!data.empty()) {
            vision->setData(data);
        }
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       py::arg("data") = std::vector<float>(),
       "Create vision sensory input");

    m.def("createAudioInput", [](size_t sampleRate, size_t numSamples,
                                const std::vector<float>& data = {}) {
        auto audio = std::make_unique<Audio>(sampleRate, numSamples);
        if (!data.empty()) {
            audio->setData(data);
        }
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"),
       py::arg("data") = std::vector<float>(),
       "Create audio sensory input");

    m.def("createInternalSignalsInput", [](const std::vector<float>& signals = {}) {
        auto internal = std::make_unique<InternalSignals>();
        for (float signal : signals) {
            internal->addSignal(signal);
        }
        return internal;
    }, py::arg("signals") = std::vector<float>(),
       "Create internal signals sensory input");

    // Motor command creation functions
    m.def("createMoveCommand", [](const std::string& direction, float power = 1.0f) {
        ActionType type;
        if (direction == "forward" || direction == "MoveForward") type = ActionType::MoveForward;
        else if (direction == "backward" || direction == "MoveBackward") type = ActionType::MoveBackward;
        else if (direction == "left" || direction == "MoveLeft") type = ActionType::MoveLeft;
        else if (direction == "right" || direction == "MoveRight") type = ActionType::MoveRight;
        else if (direction == "turn_left" || direction == "TurnLeft") type = ActionType::TurnLeft;
        else if (direction == "turn_right" || direction == "TurnRight") type = ActionType::TurnRight;
        else if (direction == "look" || direction == "Look") type = ActionType::Look;
        else if (direction == "interact" || direction == "Interact") type = ActionType::Interact;
        else type = ActionType::Wait;
        
        std::vector<float> params = {power};
        return std::make_unique<Action>(type, params);
    }, py::arg("direction"), py::arg("power") = 1.0f,
       "Create a movement motor command with power parameter");

    // Configuration helper functions
    m.def("createConfigWithDefaults", [](int neuronCount = 100, int regionCount = 2,
                                        double connectionProb = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
        config->set("connection_probability", connectionProb, nlm::ConfigSource::Default);
        return config;
    }, py::arg("neuronCount") = 100, py::arg("regionCount") = 2,
       py::arg("connectionProb") = 0.1,
       "Create a configuration with specified parameters");

    // Simulation utility functions
    m.def("runSimulationSteps", [](std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  size_t numSteps, double timestep = 0.1,
                                  bool showProgress = false) {
        if (!agent || !world) {
            throw std::runtime_error("Agent or world is null");
        }
        
        if (showProgress) {
            std::cout << "Running simulation for " << numSteps << " steps..." << std::endl;
        }
        
        for (size_t step = 0; step < numSteps; ++step) {
            if (showProgress && step % 100 == 0) {
                std::cout << "Step " << step << "/" << numSteps << std::endl;
            }
            
            // Update world
            world->update(timestep);
            
            // Get sensory percept
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            agent->getBrain()->step(static_cast<nlm::SimulationStep>(step));
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Optional: apply reward modulation based on outcome
            const auto& body = world->getAgentBody();
            float reward = body.energy > 0 ? 0.0f : -1.0f;  // Simple reward: energy-based
            agent->applyRewardModulation(reward, agent->getPredictionError());
        }
        
        if (showProgress) {
            std::cout << "Simulation completed!" << std::endl;
        }
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("timestep") = 0.1,
       py::arg("showProgress") = false,
       "Run a complete simulation with agent and world");

    // Diagnostic and analysis functions
    m.def("printBrainStats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            std::cout << "Error: Brain is null" << std::endl;
            return;
        }
        brain->logStatus();
    }, py::arg("brain"), "Print brain statistics and status");

    m.def("formatSimulationStats", [](const std::shared_ptr<AgentBrain>& agent,
                                     const std::shared_ptr<SimpleWorld>& world) {
        std::ostringstream oss;
        if (agent && agent->getBrain()) {
            oss << "Brain Stats:" << std::endl;
            oss << "  Total Neurons: " << agent->getBrain()->getTotalNeuronCount() << std::endl;
            oss << "  Active Neurons: " << agent->getBrain()->getActiveNeuronCount() << std::endl;
            oss << "  Firing Neurons: " << agent->getBrain()->getFiringNeuronCount() << std::endl;
            oss << "  Total Spikes: " << agent->getBrain()->getTotalSpikeCount() << std::endl;
            oss << "  Average Firing Rate: " << agent->getBrain()->getAverageFiringRate() << std::endl;
            oss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        }
        if (world) {
            oss << "\nWorld Stats:" << std::endl;
            oss << "  Width: " << world->getWidth() << std::endl;
            oss << "  Height: " << world->getHeight() << std::endl;
            oss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
            oss << "  Agent Energy: " << world->getAgentBody().energy << std::endl;
            oss << "  Agent Health: " << world->getAgentBody().health << std::endl;
        }
        if (agent) {
            oss << "\nAgent Stats:" << std::endl;
            oss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
            oss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
            oss << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
            oss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
            oss << "  Thought Trace: " << agent->getThoughtTrace() << std::endl;
        }
        return oss.str();
    }, py::arg("agent"), py::arg("world"),
       "Get formatted statistics for brain, world, and agent");

} // namespace nlm
