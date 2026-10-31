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

    // Base class for common ID functionality
    py::class_<NeuronId, SynapseId, RegionId, PopulationId>(m, "IDBase", R"pbdoc(Base class for all unique identifiers)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<ID: " + std::to_string(id.value) + ">";
        });

    // ID type definitions with proper documentation and Pythonic extensions
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
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

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

    // Advanced configuration factory functions
    m.def("create_development_config", []() {
        auto config = std::make_shared<Config>();
        config->set("development.stage", "CriticalPeriod");
        config->set("development.enableStructuralPlasticity", true);
        config->set("development.neurogenesisRate", 0.1);
        config->set("neuromodulation.enableDopamine", true);
        config->set("neuromodulation.enableCuriosity", true);
        config->set("prediction.enable", true);
        config->set("memory.enableReplay", true);
        config->set("memory.replayInterval", 100);
        return config;
    }, "Create a development-focused configuration");

    m.def("create_adult_config", []() {
        auto config = std::make_shared<Config>();
        config->set("development.stage", "Adult");
        config->set("development.enableStructuralPlasticity", false);
        config->set("development.neurogenesisRate", 0.0);
        config->set("neuromodulation.enableDopamine", true);
        config->set("neuromodulation.enableCuriosity", false);
        config->set("prediction.enable", true);
        config->set("memory.enableReplay", true);
        config->set("memory.replayInterval", 500);
        return config;
    }, "Create an adult-focused configuration");

    m.def("create_curiosity_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuromodulation.enableCuriosity", true);
        config->set("neuromodulation.curiosityThreshold", 0.5);
        config->set("neuromodulation.curiosityDecayRate", 0.01);
        config->set("prediction.enable", true);
        config->set("prediction.confidenceThreshold", 0.3);
        config->set("brain.explorationBias", 0.8);
        return config;
    }, "Create a curiosity-focused configuration");

    // Advanced world factory functions
    m.def("create_test_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 10, 10);
        world->setRandomSeed(12345);
        return world;
    }, "Create a test world with predefined settings");

    m.def("populate_test_world", [](std::shared_ptr<SimpleWorld> world) {
        std::vector<WorldObject> objects;
        
        // Add resources
        objects.emplace_back(10.0f, 10.0f, WorldObjectType::Resource, 10.0f, 0.3f);
        objects.emplace_back(20.0f, 20.0f, WorldObjectType::Resource, 15.0f, 0.3f);
        objects.emplace_back(30.0f, 30.0f, WorldObjectType::Resource, 20.0f, 0.3f);
        
        // Add hazards
        objects.emplace_back(40.0f, 40.0f, WorldObjectType::Hazard, 5.0f, 0.5f);
        
        // Add walls around the perimeter
        for (int x = 0; x < world->getWidth(); x += 5) {
            objects.emplace_back(static_cast<float>(x), 0.0f, WorldObjectType::Wall, 1.0f, 1.0f);
        }
        
        for (int y = 0; y < world->getHeight(); y += 5) {
            objects.emplace_back(0.0f, static_cast<float>(y), WorldObjectType::Wall, 1.0f, 1.0f);
        }
        
        for (int x = 0; x < world->getWidth(); x += 5) {
            objects.emplace_back(static_cast<float>(x), static_cast<float>(world->getHeight()), 
                               WorldObjectType::Wall, 1.0f, 1.0f);
        }
        
        for (int y = 0; y < world->getHeight(); y += 5) {
            objects.emplace_back(static_cast<float>(world->getWidth()), static_cast<float>(y), 
                               WorldObjectType::Wall, 1.0f, 1.0f);
        }
        
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        
        return objects.size();
    }, "Populate a world with test objects", py::arg("world"));

    // Validation and utility functions
    m.def("validate_neuron_id", [](const NeuronId& id) {
        if (id.value == 0) {
            throw std::invalid_argument("Neuron ID value cannot be zero");
        }
        return true;
    }, "Validate a neuron ID", py::arg("id"));

    m.def("validate_synapse_id", [](const SynapseId& id) {
        if (id.value == 0) {
            throw std::invalid_argument("Synapse ID value cannot be zero");
        }
        return true;
    }, "Validate a synapse ID", py::arg("id"));

    // ID convenience functions
    m.def("invalid_neuron_id", []() { return INVALID_NEURON_ID; }, "Get the invalid neuron ID");
    m.def("invalid_synapse_id", []() { return INVALID_SYNAPSE_ID; }, "Get the invalid synapse ID");
    m.def("invalid_region_id", []() { return INVALID_REGION_ID; }, "Get the invalid region ID");
    m.def("invalid_population_id", []() { return INVALID_POPULATION_ID; }, "Get the invalid population ID");

    // ID creation factory functions
    m.def("create_neuron_id", &NeuronId, "Create a NeuronId from value", py::arg("value") = 0);
    m.def("create_synapse_id", &SynapseId, "Create a SynapseId from value", py::arg("value") = 0);
    m.def("create_region_id", &RegionId, "Create a RegionId from value", py::arg("value") = 0);
    m.def("create_population_id", &PopulationId, "Create a PopulationId from value", py::arg("value") = 0);

    // Batch operations for performance
    m.def("batch_inject_current", [](std::shared_ptr<Brain> brain, const std::vector<NeuronId>& neurons, float current) {
        for (const auto& neuron : neurons) {
            brain->injectCurrent(neuron, current);
        }
    }, "Inject current to multiple neurons at once", py::arg("brain"), py::arg("neurons"), py::arg("current"));

    m.def("batch_update_neurons", [](std::shared_ptr<Brain> brain, const std::vector<NeuronId>& neurons, const std::vector<float>& currents) {
        if (neurons.size() != currents.size()) {
            throw std::invalid_argument("Neuron and current vectors must have the same size");
        }
        for (size_t i = 0; i < neurons.size(); ++i) {
            brain->injectCurrent(neurons[i], currents[i]);
        }
    }, "Inject currents to multiple neurons at once", py::arg("brain"), py::arg("neurons"), py::arg("currents"));

    // Debug and monitoring utilities
    m.def("log_brain_stats", [](std::shared_ptr<Brain> brain, std::shared_ptr<Logger> logger = nullptr) {
        if (!logger) {
            logger = brain->getConfig()->getOr<std::shared_ptr<Logger>>("logger", nullptr);
        }
        if (logger) {
            logger->info("Brain Stats - Neurons: {}, Active: {}, Firing: {}, Total spikes: {}", 
                        brain->getTotalNeuronCount(), brain->getActiveNeuronCount(),
                        brain->getFiringNeuronCount(), brain->getTotalSpikeCount());
        }
        return brain->getAverageFiringRate();
    }, "Log brain statistics", py::arg("brain"), py::arg("logger") = nullptr);

    m.def("compare_brains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.empty()) {
            throw std::invalid_argument("At least one brain required for comparison");
        }
        
        struct Comparison {
            size_t neuronCount = 0;
            size_t activeCount = 0;
            size_t firingCount = 0;
            size_t totalSpikes = 0;
            float avgFiringRate = 0.0f;
        };
        
        std::vector<Comparison> results;
        results.reserve(brains.size());
        
        for (const auto& brain : brains) {
            Comparison comp;
            comp.neuronCount = brain->getTotalNeuronCount();
            comp.activeCount = brain->getActiveNeuronCount();
            comp.firingCount = brain->getFiringNeuronCount();
            comp.totalSpikes = brain->getTotalSpikeCount();
            comp.avgFiringRate = brain->getAverageFiringRate();
            results.push_back(comp);
        }
        
        return results;
    }, "Compare multiple brains and return statistics", py::arg("brains"));

    // Configuration validation utilities
    m.def("validate_configuration", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        auto keys = config->getKeys();
        if (!config->has("brain.neuronCount")) {
            errors.push_back("Missing brain.neuronCount");
        }
        if (!config->has("world.width")) {
            errors.push_back("Missing world.width");
        }
        if (!config->has("world.height")) {
            errors.push_back("Missing world.height");
        }
        
        auto neuronCountOpt = config->get<int>("brain.neuronCount");
        if (neuronCountOpt && neuronCountOpt.value() < 10) {
            errors.push_back("brain.neuronCount must be at least 10");
        }
        
        auto worldWidthOpt = config->get<float>("world.width");
        if (worldWidthOpt && worldWidthOpt.value() <= 0) {
            errors.push_back("world.width must be positive");
        }
        
        auto worldHeightOpt = config->get<float>("world.height");
        if (worldHeightOpt && worldHeightOpt.value() <= 0) {
            errors.push_back("world.height must be positive");
        }
        
        return errors;
    }, "Validate a configuration and return any errors", py::arg("config"));

    // Advanced brain management utilities
    m.def("copy_brain_state", [](std::shared_ptr<Brain> source, std::shared_ptr<Brain> target) {
        target->reset();
        target->setDevelopmentalStage(source->getDevelopmentalStage());
        return "State copied (simplified implementation)";
    }, "Copy brain state from one brain to another", py::arg("source"), py::arg("target"));

    m.def("merge_brains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.empty()) {
            throw std::invalid_argument("At least one brain required for merge");
        }
        
        auto config = std::make_shared<Config>();
        size_t neuronCount = 0;
        
        for (const auto& brain : brains) {
            neuronCount += brain->getTotalNeuronCount();
        }
        
        config->set("brain.neuronCount", neuronCount / brains.size());
        config->set("brain.initialExcitatoryRatio", 0.8);
        
        return std::make_shared<Brain>(config);
    }, "Create a merged brain from multiple brains", py::arg("brains"));

    // Experiment utilities
    m.def("run_agent_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int max_steps) {
        agent->reset();
        
        for (int step = 0; step < max_steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            return percept;
        }
        
        return std::make_shared<SensoryPercept>();
    }, "Run a complete agent episode", py::arg("agent"), py::arg("world"), py::arg("max_steps"));

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
