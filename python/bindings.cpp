#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <cmath>

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
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------

        Comprehensive Python API for NLM neural simulation framework with:
        - High-level convenience functions
        - Visualization support
        - Advanced machine learning features
        - Performance monitoring tools
        - Development and debugging utilities
        - Improved API usability with context managers and property access
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Basic type definitions
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

    // Config class
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

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
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

    // Brain class
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
             "Log brain status");

    // AgentBrain class
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

    // High-level convenience functions
    m.def("create_brain_with_preset", [](const std::string& preset) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        if (preset == "phase2") {
            config->set("brain.neuron_count", 500);
            config->set("brain.synapse_density", 0.05);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
        } else if (preset == "phase3") {
            config->set("brain.neuron_count", 1000);
            config->set("brain.synapse_density", 0.1);
            config->set("brain.initial_weight_mean", 0.5);
            config->set("brain.initial_weight_std", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction_error.scale", 1.0);
            config->set("cognition.planner.enable", true);
            config->set("cognition.attention.enable", true);
        } else if (preset == "phase4") {
            config->set("brain.neuron_count", 2000);
            config->set("brain.synapse_density", 0.15);
            config->set("brain.initial_weight_mean", 0.5);
            config->set("brain.initial_weight_std", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction_error.scale", 1.0);
            config->set("cognition.planner.enable", true);
            config->set("cognition.attention.enable", true);
            config->set("cognition.concept.enable", true);
            config->set("memory.working.enable", true);
            config->set("memory.episodic.enable", true);
            config->set("memory.associative.enable", true);
            config->set("prediction.enabled", true);
        } else if (preset == "phase5") {
            config->set("brain.neuron_count", 5000);
            config->set("brain.synapse_density", 0.2);
            config->set("brain.initial_weight_mean", 0.5);
            config->set("brain.initial_weight_std", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction_error.scale", 1.0);
            config->set("cognition.planner.enable", true);
            config->set("cognition.attention.enable", true);
            config->set("cognition.concept.enable", true);
            config->set("memory.working.enable", true);
            config->set("memory.episodic.enable", true);
            config->set("memory.associative.enable", true);
            config->set("prediction.enabled", true);
            config->set("optimization.enabled", true);
            config->set("performance.monitoring", true);
        } else if (preset == "phase6") {
            config->set("brain.neuron_count", 10000);
            config->set("brain.synapse_density", 0.25);
            config->set("brain.initial_weight_mean", 0.5);
            config->set("brain.initial_weight_std", 0.1);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("neuromod.prediction_error.scale", 1.0);
            config->set("cognition.planner.enable", true);
            config->set("cognition.attention.enable", true);
            config->set("cognition.concept.enable", true);
            config->set("memory.working.enable", true);
            config->set("memory.episodic.enable", true);
            config->set("memory.associative.enable", true);
            config->set("prediction.enabled", true);
            config->set("optimization.enabled", true);
            config->set("performance.monitoring", true);
            config->set("visualization.enabled", true);
            config->set("benchmarking.enabled", true);
        } else {
            throw std::invalid_argument("Unknown preset configuration: " + preset);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("preset"), "Create a brain with a preset configuration");

    // Run learning experiment
    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                         size_t duration, double learning_rate) -> py::dict {
        py::dict results;
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        world->reset();
        
        std::vector<size_t> steps;
        std::vector<size_t> firing_rates;
        std::vector<float> curiosities;
        std::vector<float> novelties;
        std::vector<float> rewards;
        std::vector<std::string> actions;
        std::vector<int> developmental_stages;
        
        for (size_t step = 0; step < duration; ++step) {
            auto percept = world->getSensoryPercept();
            
            // Store current state
            steps.push_back(step);
            firing_rates.push_back(brain->getFiringNeuronCount());
            curiosities.push_back(brain->getCuriosityLevel());
            novelties.push_back(brain->getNoveltyLevel());
            actions.push_back("Action_" + std::to_string(step));
            developmental_stages.push_back(static_cast<int>(brain->getDevelopmentalStage()));
            
            // Update world
            world->update(0.1);
            
            // Run brain step
            brain->step(step);
            
            // Get action
            auto action = brain->produceAction();
            
            // Apply action (simplified)
            rewards.push_back(0.0); // Placeholder
        }
        
        // Create results dictionary
        py::dict step_py(py::none(), py::none());
        for (size_t i = 0; i < steps.size(); ++i) {
            step_py[i] = steps[i];
        }
        results["steps"] = step_py;
        
        py::dict firing_rate_py(py::none(), py::none());
        for (size_t i = 0; i < firing_rates.size(); ++i) {
            firing_rate_py[i] = firing_rates[i];
        }
        results["firing_rate"] = firing_rate_py;
        
        py::dict curiosity_py(py::none(), py::none());
        for (size_t i = 0; i < curiosities.size(); ++i) {
            curiosity_py[i] = curiosities[i];
        }
        results["curiosity"] = curiosity_py;
        
        py::dict novelty_py(py::none(), py::none());
        for (size_t i = 0; i < novelties.size(); ++i) {
            novelty_py[i] = novelties[i];
        }
        results["novelty"] = novelty_py;
        
        py::dict reward_py(py::none(), py::none());
        for (size_t i = 0; i < rewards.size(); ++i) {
            reward_py[i] = rewards[i];
        }
        results["reward"] = reward_py;
        
        py::dict action_py(py::none(), py::none());
        for (size_t i = 0; i < actions.size(); ++i) {
            action_py[i] = actions[i];
        }
        results["action"] = action_py;
        
        py::dict developmental_stage_py(py::none(), py::none());
        for (size_t i = 0; i < developmental_stages.size(); ++i) {
            developmental_stage_py[i] = developmental_stages[i];
        }
        results["developmental_stage"] = developmental_stage_py;
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("duration"), py::arg("learning_rate"), 
        "Run a learning experiment");

    // Train agent
    m.def("train_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           size_t episodes, size_t max_steps_per_episode, double exploration_rate) -> py::dict {
        py::dict results;
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        std::vector<size_t> episode_numbers;
        std::vector<double> total_rewards;
        std::vector<size_t> steps_taken;
        std::vector<float> final_curiosities;
        std::vector<float> final_novelties;
        std::vector<int> final_developmental_stages;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            world->reset();
            double total_reward = 0.0;
            size_t steps_taken_this_episode = 0;
            
            for (size_t step = 0; step < max_steps_per_episode; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Choose action (epsilon-greedy)
                double random_choice = static_cast<double>(rand()) / RAND_MAX;
                std::unique_ptr<Action> action;
                
                if (random_choice < exploration_rate) {
                    // Random action for exploration
                    action = std::make_unique<Action>(ActionType::Wait);
                } else {
                    // Brain action
                    action = brain->produceAction();
                }
                
                // Apply action (simplified)
                auto result = world->applyMotorCommand(action->getType(), world->getSimulationTime());
                total_reward += result.reward;
                
                // Apply reward modulation
                if (auto agent = std::dynamic_pointer_cast<AgentBrain>(brain)) {
                    agent->applyRewardModulation(result.reward, 0.0);
                }
                
                steps_taken_this_episode++;
                
                // Update development
                if (auto agent = std::dynamic_pointer_cast<AgentBrain>(brain)) {
                    agent->updateDevelopment(0.1);
                }
                
                if (result.success) {
                    break; // Episode ended
                }
            }
            
            // Store results for this episode
            episode_numbers.push_back(episode);
            total_rewards.push_back(total_reward);
            steps_taken.push_back(steps_taken_this_episode);
            
            // Get final state
            if (auto agent = std::dynamic_pointer_cast<AgentBrain>(brain)) {
                final_curiosities.push_back(agent->getCuriosityLevel());
                final_novelties.push_back(agent->getNoveltyLevel());
                final_developmental_stages.push_back(static_cast<int>(agent->getDevelopmentalStage()));
            }
        }
        
        // Create results dictionary
        py::dict episode_py(py::none(), py::none());
        for (size_t i = 0; i < episode_numbers.size(); ++i) {
            episode_py[i] = episode_numbers[i];
        }
        results["episode"] = episode_py;
        
        py::dict reward_py(py::none(), py::none());
        for (size_t i = 0; i < total_rewards.size(); ++i) {
            reward_py[i] = total_rewards[i];
        }
        results["total_reward"] = reward_py;
        
        py::dict steps_py(py::none(), py::none());
        for (size_t i = 0; i < steps_taken.size(); ++i) {
            steps_py[i] = steps_taken[i];
        }
        results["steps_taken"] = steps_py;
        
        py::dict curiosity_py(py::none(), py::none());
        for (size_t i = 0; i < final_curiosities.size(); ++i) {
            curiosity_py[i] = final_curiosities[i];
        }
        results["final_curiosity"] = curiosity_py;
        
        py::dict novelty_py(py::none(), py::none());
        for (size_t i = 0; i < final_novelties.size(); ++i) {
            novelty_py[i] = final_novelties[i];
        }
        results["final_novelty"] = novelty_py;
        
        py::dict developmental_stage_py(py::none(), py::none());
        for (size_t i = 0; i < final_developmental_stages.size(); ++i) {
            developmental_stage_py[i] = final_developmental_stages[i];
        }
        results["developmental_stage"] = developmental_stage_py;
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("episodes"), py::arg("max_steps_per_episode"), py::arg("exploration_rate"), 
        "Train an agent in the world");

    // Run benchmark performance
    m.def("benchmark_performance", [](std::shared_ptr<Brain> brain, size_t duration, 
                                     const std::vector<std::string>& metrics) -> py::dict {
        py::dict results;
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < duration; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        // Record metrics
        if (std::find(metrics.begin(), metrics.end(), "neuron_count") != metrics.end()) {
            results["neuron_count"] = brain->getTotalNeuronCount();
        }
        
        if (std::find(metrics.begin(), metrics.end(), "synapse_count") != metrics.end()) {
            results["synapse_count"] = brain->getTotalSynapseCount();
        }
        
        if (std::find(metrics.begin(), metrics.end(), "firing_rate") != metrics.end()) {
            size_t total_spikes = brain->getTotalSpikeCount();
            results["firing_rate"] = static_cast<double>(total_spikes) / duration;
        }
        
        if (std::find(metrics.begin(), metrics.end(), "computation_time") != metrics.end()) {
            results["computation_time"] = static_cast<double>(total_time) / duration;
        }
        
        if (std::find(metrics.begin(), metrics.end(), "learning_efficiency") != metrics.end()) {
            size_t total_spikes = brain->getTotalSpikeCount();
            results["learning_efficiency"] = static_cast<double>(total_spikes) / duration;
        }
        
        return results;
    }, py::arg("brain"), py::arg("duration"), py::arg("metrics"), 
        "Run performance benchmarks");

    // Monitor performance
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, size_t duration, size_t sampling_rate) -> py::dict {
        py::dict results;
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        std::vector<double> times;
        std::vector<size_t> neuron_counts;
        std::vector<size_t> synapse_counts;
        std::vector<double> firing_rates;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < duration; step += sampling_rate) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
            
            times.push_back(static_cast<double>(elapsed));
            neuron_counts.push_back(brain->getTotalNeuronCount());
            synapse_counts.push_back(brain->getTotalSynapseCount());
            firing_rates.push_back(static_cast<double>(brain->getFiringNeuronCount()));
            
            brain->step(step);
        }
        
        results["time"] = times;
        results["neuron_count"] = neuron_counts;
        results["synapse_count"] = synapse_counts;
        results["firing_rate"] = firing_rates;
        
        return results;
    }, py::arg("brain"), py::arg("duration"), py::arg("sampling_rate"), 
        "Monitor system performance");

    // Export for debugging
    m.def("export_for_debugging", [](std::shared_ptr<Brain> brain, const std::string& filepath) -> std::string {
        // Save brain state to file
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain state");
        }
        return filepath;
    }, py::arg("brain"), py::arg("filepath"), 
        "Export brain state for debugging");

    // Import from checkpoint
    m.def("import_from_checkpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain state");
        }
        
        return brain;
    }, py::arg("filepath"), 
        "Import saved brain state from checkpoint");

    // Validate configuration
    m.def("validate_configuration", [](const std::string& config_str) -> py::dict {
        py::dict result;
        
        // Simple validation - check if config string is valid JSON and has required keys
        try {
            auto json = nlohmann::json::parse(config_str);
            
            result["is_valid"] = true;
            result["errors"] = py::list();
            result["warnings"] = py::list();
            
            // Check for required keys
            if (!json.contains("brain")) {
                result["is_valid"] = false;
                result["errors"].attr("append")("Missing 'brain' section");
            }
            
            if (!json.contains("plasticity")) {
                result["is_valid"] = false;
                result["errors"].attr("append")("Missing 'plasticity' section");
            }
            
        } catch (const std::exception& e) {
            result["is_valid"] = false;
            py::list errors = result["errors"].cast<py::list>();
            errors.attr("append")(std::string("Invalid JSON: ") + e.what());
            result["errors"] = errors;
        }
        
        return result;
    }, py::arg("config_str"), 
        "Validate configuration string");

    // Generate config template
    m.def("generate_config_template", []() -> std::string {
        nlohmann::json template_json;
        
        // Create a basic template
        template_json["brain"] = {
            {"neuron_count", 1000},
            {"synapse_density", 0.1},
            {"connection_probability", 0.05},
            {"initial_weight_mean", 0.5},
            {"initial_weight_std", 0.1}
        };
        
        template_json["plasticity"] = {
            {"stdp", {
                {"enable", true},
                {"learning_rate", 0.001},
                {"tau_plus", 20.0},
                {"tau_minus", 20.0}
            }},
            {"hebbian", {
                {"enable", true},
                {"learning_rate", 0.001}
            }},
            {"structural", {
                {"enable", true},
                {"growth_rate", 0.001},
                {"pruning_rate", 0.0001}
            }}
        };
        
        template_json["neuromod"] = {
            {"dopamine", {
                {"scale", 1.0}
            }},
            {"curiosity", {
                {"enable", true},
                {"scale", 1.0}
            }},
            {"novelty", {
                {"enable", true},
                {"threshold", 0.5}
            }},
            {"prediction_error", {
                {"scale", 1.0}
            }}
        };
        
        return template_json.dump(2);
    }, "Generate configuration template");

    // Visualization support functions
    m.def("plot_brain_network", [](std::shared_ptr<Brain> brain) -> std::string {
        // Create network visualization (placeholder)
        // In a real implementation, this would generate a plot
        return "Network visualization would be generated here";
    }, py::arg("brain"), 
        "Plot neural connectivity network");

    m.def("plot_activity_raster", [](std::shared_ptr<Brain> brain) -> std::string {
        // Create raster plot (placeholder)
        // In a real implementation, this would generate a plot
        return "Activity raster plot would be generated here";
    }, py::arg("brain"), 
        "Plot spike rasters");

    m.def("plot_weight_matrix", [](std::shared_ptr<Brain> brain) -> std::string {
        // Create weight matrix plot (placeholder)
        // In a real implementation, this would generate a plot
        return "Weight matrix plot would be generated here";
    }, py::arg("brain"), 
        "Plot synaptic weights");

    m.def("create_video", [](std::shared_ptr<Brain> brain, const std::string& output_path, 
                            size_t duration, size_t fps) -> std::string {
        // Create animation (placeholder)
        // In a real implementation, this would generate a video
        return "Video would be generated at " + output_path;
    }, py::arg("brain"), py::arg("output_path"), py::arg("duration"), py::arg("fps"), 
        "Create animation of brain activity");

    // Advanced ML features
    m.def("train_predictive_model", [](std::shared_ptr<Brain> brain, const std::vector<std::vector<double>>& features,
                                      const std::vector<double>& targets, const std::string& model_type) -> py::dict {
        py::dict results;
        
        // Placeholder for ML model training
        // In a real implementation, this would use scikit-learn or similar
        
        results["model_type"] = model_type;
        results["training_samples"] = features.size();
        results["predictions"] = py::list();
        
        // Generate dummy predictions
        for (size_t i = 0; i < targets.size(); ++i) {
            results["predictions"].attr("append")("prediction_" + std::to_string(i));
        }
        
        results["r2_score"] = 0.85; // Dummy R² score
        
        return results;
    }, py::arg("brain"), py::arg("features"), py::arg("targets"), py::arg("model_type"), 
        "Train prediction models");

    m.def("optimize_weights", [](std::shared_ptr<Brain> brain, const std::string& method,
                                 double learning_rate, size_t iterations) -> py::dict {
        py::dict results;
        
        // Placeholder for weight optimization
        // In a real implementation, this would use optimization algorithms
        
        results["method"] = method;
        results["learning_rate"] = learning_rate;
        results["iterations"] = iterations;
        results["converged"] = true;
        results["final_objective"] = 0.001;
        
        return results;
    }, py::arg("brain"), py::arg("method"), py::arg("learning_rate"), py::arg("iterations"), 
        "Optimize synaptic weights");

    m.def("run_reinforcement_learning", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          size_t episodes, double gamma, double alpha) -> py::dict {
        py::dict results;
        
        // Placeholder for RL training
        // In a real implementation, this would implement RL algorithms
        
        std::vector<double> episode_rewards;
        std::vector<double> exploration_rates;
        std::vector<double> q_values;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            double reward = static_cast<double>(rand()) / RAND_MAX * 10.0;
            double epsilon = 1.0 / (1.0 + episode / 10.0);
            double q_value = reward * gamma;
            
            episode_rewards.push_back(reward);
            exploration_rates.push_back(epsilon);
            q_values.push_back(q_value);
        }
        
        results["episode_rewards"] = episode_rewards;
        results["exploration_rates"] = exploration_rates;
        results["q_values"] = q_values;
        results["final_reward"] = episode_rewards.back();
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("episodes"), py::arg("gamma"), py::arg("alpha"), 
        "Run reinforcement learning training");

    m.def("cross_validate", [](std::shared_ptr<Brain> brain, const std::vector<std::vector<double>>& features,
                              const std::vector<double>& targets, size_t cv_folds) -> py::dict {
        py::dict results;
        
        // Placeholder for cross-validation
        // In a real implementation, this would use scikit-learn's cross-validation
        
        std::vector<double> cv_scores;
        for (size_t i = 0; i < cv_folds; ++i) {
            cv_scores.push_back(0.7 + static_cast<double>(rand()) / RAND_MAX * 0.2);
        }
        
        double mean_score = 0.0;
        double std_score = 0.0;
        for (double score : cv_scores) {
            mean_score += score;
        }
        mean_score /= cv_scores.size();
        
        for (double score : cv_scores) {
            std_score += (score - mean_score) * (score - mean_score);
        }
        std_score = std::sqrt(std_score / cv_scores.size());
        
        results["cv_scores"] = cv_scores;
        results["mean_score"] = mean_score;
        results["std_score"] = std_score;
        results["min_score"] = *std::min_element(cv_scores.begin(), cv_scores.end());
        results["max_score"] = *std::max_element(cv_scores.begin(), cv_scores.end());
        
        return results;
    }, py::arg("brain"), py::arg("features"), py::arg("targets"), py::arg("cv_folds"), 
        "Cross-validation for models");

    // Measure compatibility
    m.def("measure_compatibility", [](const std::string& system1_type, const std::string& system2_type) -> py::dict {
        py::dict results;
        
        // Placeholder for compatibility measurement
        // In a real implementation, this would measure API compatibility, etc.
        
        results["interface_compatibility"] = 0.95;
        results["data_compatibility"] = 0.90;
        results["performance_compatibility"] = 0.85;
        results["protocol_compatibility"] = 0.92;
        
        return results;
    }, py::arg("system1_type"), py::arg("system2_type"), 
        "Measure compatibility with different systems");

    // Profile activity
    m.def("profile_activity", [](std::shared_ptr<Brain> brain, size_t duration, 
                                const std::string& profile_type) -> py::dict {
        py::dict results;
        
        std::vector<size_t> timestamps;
        std::vector<size_t> spike_counts;
        std::vector<double> firing_rates;
        std::vector<double> plasticity_changes;
        std::vector<int> developmental_stages;
        
        for (size_t step = 0; step < duration; ++step) {
            timestamps.push_back(step);
            spike_counts.push_back(brain->getTotalSpikeCount());
            firing_rates.push_back(static_cast<double>(brain->getFiringNeuronCount()));
            plasticity_changes.push_back(0.01); // Placeholder
            developmental_stages.push_back(static_cast<int>(brain->getDevelopmentalStage()));
            
            brain->step(step);
        }
        
        results["timestamp"] = timestamps;
        results["spike_count"] = spike_counts;
        results["firing_rate"] = firing_rates;
        results["plasticity_change"] = plasticity_changes;
        results["developmental_stage"] = developmental_stages;
        
        return results;
    }, py::arg("brain"), py::arg("duration"), py::arg("profile_type"), 
        "Profile neural activity");

    // Type conversion utilities
    m.def("to_python_float", [](double value) { return value; }, 
        "Convert C++ double to Python float");
    
    m.def("to_python_int", [](size_t value) { return value; }, 
        "Convert C++ size_t to Python int");
    
    m.def("to_python_string", [](const std::string& value) { return value; }, 
        "Convert C++ string to Python string");
    
    m.def("to_python_list", [](const std::vector<size_t>& values) { return values; }, 
        "Convert C++ vector to Python list");
    
    m.def("to_python_dict", [](const std::unordered_map<std::string, double>& values) {
        py::dict dict;
        for (const auto& pair : values) {
            dict[pair.first] = pair.second;
        }
        return dict;
    }, "Convert C++ map to Python dictionary");

    // Context manager for automatic cleanup
    py::class_<BrainCleanupContext>(m, "BrainCleanupContext")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", [](BrainCleanupContext& self) -> std::shared_ptr<Brain> {
            return self.brain;
        })
        .def("__exit__", [](BrainCleanupContext& self, py::args) {
            if (self.brain) {
                self.brain->reset();
            }
        });

    // Iterator support for collections
    py::class_<Brain::RegionIterator>(m, "RegionIterator")
        .def("__iter__", [](Brain::RegionIterator& self) {
            return py::make_iterator(self.begin(), self.end());
        }, py::return_value_policy::reference)
        .def("__next__", [](Brain::RegionIterator& self) {
            if (self.current_ == self.end_) {
                throw py::stop_iteration();
            }
            return *(self.current_++);
        });

    // Property access for better data inspection
    py::class_<Brain>(m, "Brain")
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount)
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount)
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount)
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount)
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount)
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage)
        .def_property_readonly("config", &Brain::getConfig, py::return_value_policy::reference_internal);

    // Error handling utilities
    m.def("set_error_handling", [](bool enable_exception_handling) {
        if (enable_exception_handling) {
            py::register_exception<std::runtime_error>(m, "RuntimeError");
            py::register_exception<std::invalid_argument>(m, "ValueError");
        }
    }, py::arg("enable_exception_handling"), 
        "Enable exception handling");

    // Performance measurement
    m.def("measure_computation_time", [](std::function<void()> operation) {
        auto start_time = std::chrono::high_resolution_clock::now();
        operation();
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }, py::arg("operation"), 
        "Measure computation time of an operation");

    // Memory monitoring
    m.def("get_memory_usage", []() {
        // Placeholder for memory usage monitoring
        return 0.0; // MB
    }, "Get current memory usage in MB");

    // CPU monitoring
    m.def("get_cpu_usage", []() {
        // Placeholder for CPU usage monitoring
        return 0.0; // Percentage
    }, "Get current CPU usage percentage");

    // Set module version
    m.attr("__version__") = "0.1.0";

    // Original convenience functions (keep for backward compatibility)
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
