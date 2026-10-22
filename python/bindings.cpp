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

    // Advanced configuration factory functions
    m.def("createTrainingConfig", [](const std::string& trainingType, int epochs) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("training_type", trainingType, ConfigSource::Runtime);
        config->set("epochs", epochs, ConfigSource::Runtime);
        config->set("learning_rate", 0.001, ConfigSource::Runtime);
        config->set("batch_size", 32, ConfigSource::Runtime);
        config->set("optimizer", "adam", ConfigSource::Runtime);
        return config;
    }, py::arg("trainingType"), py::arg("epochs"), "Create a training configuration");

    m.def("createDevelopmentConfig", [](float plasticityRate, bool enableStructuralPlasticity) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("plasticity_rate", plasticityRate, ConfigSource::Runtime);
        config->set("structural_plasticity_enabled", enableStructuralPlasticity, ConfigSource::Runtime);
        config->set("development_enabled", true, ConfigSource::Runtime);
        config->set("curiosity_enabled", true, ConfigSource::Runtime);
        return config;
    }, py::arg("plasticityRate"), py::arg("enableStructuralPlasticity"), "Create a development configuration");

    m.def("createProductionConfig", [](int memorySize, float explorationRate) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("working_memory_size", memorySize, ConfigSource::Runtime);
        config->set("exploration_rate", explorationRate, ConfigSource::Runtime);
        config->set("novelty_detection_enabled", true, ConfigSource::Runtime);
        config->set("prediction_enabled", true, ConfigSource::Runtime);
        config->set("attention_enabled", true, ConfigSource::Runtime);
        return config;
    }, py::arg("memorySize"), py::arg("explorationRate"), "Create a production configuration");

    // Enhanced brain factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainFromConfigFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"), "Create a brain from a configuration file");

    m.def("createTrainingBrain", [](int neuronCount, int regionCount, float learningRate) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        config->set("region_count", regionCount, ConfigSource::Runtime);
        config->set("learning_rate", learningRate, ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        config->set("plasticity_rate", 0.01f, ConfigSource::Runtime);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), py::arg("regionCount"), py::arg("learningRate"), "Create a training brain with optimal settings");

    m.def("createDevelopmentBrain", [](int neuronCount, float maturationRate) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        config->set("region_count", 2, ConfigSource::Runtime);
        config->set("maturation_rate", maturationRate, ConfigSource::Runtime);
        config->set("development_enabled", true, ConfigSource::Runtime);
        config->set("curiosity_enabled", true, ConfigSource::Runtime);
        config->set("novelty_detection_enabled", true, ConfigSource::Runtime);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), py::arg("maturationRate"), "Create a development brain with maturation support");

    // Enhanced world factory functions
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createSimpleWorldWithDefaults", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setAgentStart(width/2, height/2);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Create a simple world with default configuration");

    m.def("createTrainingWorld", [](int width, int height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setAgentStart(2, 2);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        world->setRandomSeed(42);
        return world;
    }, py::arg("width"), py::arg("height"), "Create a training world with optimal settings");

    m.def("createSurvivalWorld", [](int width, int height, float hazardDensity) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setAgentStart(width/3, height/3);
        world->setMaxEnergy(50.0f);
        world->setEnergyDecayRate(0.02f);
        
        // Add hazards randomly
        srand(42);
        for (int i = 0; i < hazardDensity * width * height; ++i) {
            int wx = rand() % width;
            int wy = rand() % height;
            world->addObject(WorldObject(wx, wy, WorldObjectType::Hazard, 1.0f, 0.3f));
        }
        
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("hazardDensity"), "Create a survival world with hazards");

    // Agent brain factory functions
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all learning capabilities by default
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent with default learning enabled");

    m.def("createExpertAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning capabilities for expert agent
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an expert agent with all capabilities enabled");

    // Complete simulation setup factory
    m.def("createCompleteSimulationSetup", [](const std::string& worldType, int steps, bool enableLearning) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500, ConfigSource::Runtime);
        config->set("region_count", 2, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        if (worldType == "training") {
            world->configure(50, 50, 8, 8);
            world->setAgentStart(25, 25);
            world->setMaxEnergy(100.0f);
        } else if (worldType == "survival") {
            world->configure(100, 100, 8, 8);
            world->setAgentStart(50, 50);
            world->setMaxEnergy(50.0f);
        } else {
            world->configure(30, 30, 8, 8);
            world->setAgentStart(15, 15);
        }
        world->setRandomSeed(42);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("worldType"), py::arg("steps"), py::arg("enableLearning"), "Create a complete simulation setup with all components");

    // Quick setup convenience functions
    m.def("quickSimulation", [](int steps, int visionSize) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        return createCompleteSimulationSetup("training", steps, true);
    }, py::arg("steps"), py::arg("visionSize"), "Quick setup for a standard simulation");

    m.def("quickSurvivalSimulation", [](int steps) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        return createCompleteSimulationSetup("survival", steps, true);
    }, py::arg("steps"), "Quick setup for a survival simulation");

    // === UTILITY FUNCTIONS ===

    // Simulation utility functions
    m.def("runSimulationSteps", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int steps, bool enableVisualization) -> float {
        float totalReward = 0.0f;
        int successfulActions = 0;
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get percept and process it
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Agent thinks and produces action
            agent->decodeMotorCommand();
            
            // Apply action to world
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            if (agent->isRewardModulationEnabled()) {
                agent->applyRewardModulation(1.0f, 0.5f);
                totalReward += 1.0f;
                successfulActions++;
            }
            
            if (enableVisualization && step % 10 == 0) {
                agent->getBrain()->logStatus();
            }
        }
        
        return successfulActions / static_cast<float>(steps);
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("enableVisualization"), "Run a complete simulation with steps");

    m.def("runTrainingSimulation", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int steps) -> std::map<std::string, float> {
        std::map<std::string, float> metrics;
        metrics["total_steps"] = steps;
        metrics["successful_actions"] = 0;
        metrics["total_reward"] = 0.0f;
        metrics["avg_firing_rate"] = 0.0f;
        metrics["neuron_count"] = agent->getBrain()->getTotalNeuronCount();
        metrics["synapse_count"] = agent->getBrain()->getTotalSynapseCount();
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->decodeMotorCommand();
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (agent->isRewardModulationEnabled()) {
                agent->applyRewardModulation(1.0f, 0.5f);
                metrics["successful_actions"]++;
                metrics["total_reward"] += 1.0f;
            }
        }
        
        if (steps > 0) {
            metrics["success_rate"] = metrics["successful_actions"] / static_cast<float>(steps);
        } else {
            metrics["success_rate"] = 0.0f;
        }
        
        return metrics;
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), "Run a training simulation and return metrics");

    // Brain state management utilities
    m.def("saveBrainToFile", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to save brain to file: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to a file");

    m.def("loadBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain from file: " + filepath);
        }
        
        return brain;
    }, py::arg("filepath"), "Load brain state from a file");

    // Comparison and analysis utilities
    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) -> std::map<std::string, double> {
        std::map<std::string, double> comparison;
        
        comparison["neuron_count_match"] = (brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount()) ? 1.0 : 0.0;
        comparison["synapse_count_match"] = (brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount()) ? 1.0 : 0.0;
        comparison["firing_rate_ratio"] = brain1->getAverageFiringRate() / (brain2->getAverageFiringRate() > 0 ? brain2->getAverageFiringRate() : 1.0);
        comparison["excitement_ratio"] = brain1->getExcitationInhibitionRatio() / (brain2->getExcitationInhibitionRatio() > 0 ? brain2->getExcitationInhibitionRatio() : 1.0);
        comparison["total_spike_ratio"] = static_cast<double>(brain1->getTotalSpikeCount()) / (brain2->getTotalSpikeCount() > 0 ? brain2->getTotalSpikeCount() : 1);
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return metrics");

    // Configuration utilities
    m.def("createConfigFromParams", [](std::map<std::string, std::variant<int, double, bool, std::string>> params) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : params) {
            if (std::holds_alternative<int>(pair.second)) {
                config->set(pair.first, std::get<int>(pair.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<double>(pair.second)) {
                config->set(pair.first, std::get<double>(pair.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<bool>(pair.second)) {
                config->set(pair.first, std::get<bool>(pair.second), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::string>(pair.second)) {
                config->set(pair.first, std::get<std::string>(pair.second), ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("params"), "Create a configuration from parameter map");

    // Advanced agent utilities
    m.def("createSuperAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, bool enableAllFeatures) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all capabilities
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        if (enableAllFeatures) {
            // Enable additional advanced features via development system
            auto devStage = brain->getDevelopmentalStage();
            brain->setDevelopmentalStage(devStage);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableAllFeatures"), "Create a super agent with all capabilities");

    m.def("createSuperAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, bool enableAllFeatures) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all capabilities
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        if (enableAllFeatures) {
            // Enable additional advanced features via development system
            auto devStage = brain->getDevelopmentalStage();
            brain->setDevelopmentalStage(devStage);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableAllFeatures"), "Create a super agent with all capabilities");

    // Batch processing utilities
    m.def("batchRunSimulations", [](std::vector<std::shared_ptr<AgentBrain>> agents, std::vector<std::shared_ptr<SimpleWorld>> worlds, int steps) -> std::vector<std::map<std::string, float>> {
        std::vector<std::map<std::string, float>> results;
        
        for (size_t i = 0; i < agents.size() && i < worlds.size(); ++i) {
            results.push_back(runTrainingSimulation(agents[i], worlds[i], steps));
        }
        
        return results;
    }, py::arg("agents"), py::arg("worlds"), py::arg("steps"), "Run multiple simulations in batch");

    // === HELPER CLASSES AND UTILITIES ===

    // Simple statistics calculator
    m.def("calculateStats", [](const std::vector<float>& values) -> std::map<std::string, float> {
        std::map<std::string, float> stats;
        
        if (values.empty()) {
            stats["count"] = 0;
            stats["mean"] = 0.0f;
            stats["min"] = 0.0f;
            stats["max"] = 0.0f;
            stats["std"] = 0.0f;
            return stats;
        }
        
        float sum = 0.0f;
        float min = values[0];
        float max = values[0];
        
        for (float val : values) {
            sum += val;
            if (val < min) min = val;
            if (val > max) max = val;
        }
        
        float mean = sum / values.size();
        float variance = 0.0f;
        
        for (float val : values) {
            float diff = val - mean;
            variance += diff * diff;
        }
        
        float std_dev = sqrt(variance / values.size());
        
        stats["count"] = values.size();
        stats["mean"] = mean;
        stats["min"] = min;
        stats["max"] = max;
        stats["std"] = std_dev;
        
        return stats;
    }, py::arg("values"), "Calculate basic statistics for a vector of values");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Performance optimization functions (not exposed to Python, for internal use)
namespace nlm {

// Optimized configuration creation with move semantics
inline std::shared_ptr<Config> createOptimizedConfig(const std::unordered_map<std::string, ConfigValue>& params) {
    auto config = std::make_shared<Config>();
    for (const auto& pair : params) {
        config->set(pair.first, pair.second, ConfigSource::Runtime);
    }
    return config;
}

// Memory-efficient batch configuration creation
inline std::vector<std::shared_ptr<Config>> createConfigBatch(
    const std::vector<std::unordered_map<std::string, ConfigValue>>& paramsList) {
    std::vector<std::shared_ptr<Config>> configs;
    configs.reserve(paramsList.size());
    
    for (const auto& params : paramsList) {
        configs.push_back(createOptimizedConfig(params));
    }
    
    return configs;
}

// Optimized statistics calculation with better memory usage
inline std::map<std::string, float> optimizedCalculateStats(const std::vector<float>& values) {
    std::map<std::string, float> stats;
    
    if (values.empty()) {
        stats["count"] = 0;
        stats["mean"] = 0.0f;
        stats["min"] = 0.0f;
        stats["max"] = 0.0f;
        stats["std"] = 0.0f;
        return stats;
    }
    
    // Use single pass for mean and variance (Welford's algorithm)
    float mean = values[0];
    float M2 = 0.0f;
    float min = values[0];
    float max = values[0];
    
    for (size_t i = 1; i < values.size(); ++i) {
        float val = values[i];
        float delta = val - mean;
        mean += delta / (i + 1);
        float delta2 = val - mean;
        M2 += delta * delta2;
        
        if (val < min) min = val;
        if (val > max) max = val;
    }
    
    float variance = M2 / values.size();
    float std_dev = sqrt(variance);
    
    stats["count"] = values.size();
    stats["mean"] = mean;
    stats["min"] = min;
    stats["max"] = max;
    stats["std"] = std_dev;
    
    return stats;
}

// Memory-efficient simulation step execution
inline float optimizedRunSimulationSteps(
    std::shared_ptr<AgentBrain> agent, 
    std::shared_ptr<SimpleWorld> world, 
    int steps, 
    bool enableVisualization) {
    
    float successful_actions = 0.0f;
    
    // Pre-allocate references for faster access
    auto& processInput = agent->processSensoryInput;
    auto& decodeCommand = agent->decodeMotorCommand;
    auto& applyModulation = agent->applyRewardModulation;
    auto& isRewardEnabled = agent->isRewardModulationEnabled;
    auto& brain = agent->getBrain();
    
    for (int step = 0; step < steps; ++step) {
        world->update(0.1f);
        auto percept = world->getSensoryPercept();
        processInput(percept);
        decodeCommand();  // Ensure motor neurons are active
        
        auto action = decodeCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (isRewardEnabled()) {
            applyModulation(1.0f, 0.5f);
            successful_actions += 1.0f;
        }
        
        if (enableVisualization && step % 10 == 0) {
            brain->logStatus();
        }
    }
    
    return successful_actions / steps;
}

// Optimized comparison function
inline std::map<std::string, double> optimizedCompareBrains(
    std::shared_ptr<Brain> brain1, 
    std::shared_ptr<Brain> brain2) {
    
    std::map<std::string, double> comparison;
    
    // Use direct access for better performance
    comparison["neuron_count_match"] = (
        brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount()) ? 1.0 : 0.0;
    
    comparison["synapse_count_match"] = (
        brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount()) ? 1.0 : 0.0;
    
    double firingRate1 = brain1->getAverageFiringRate();
    double firingRate2 = brain2->getAverageFiringRate();
    comparison["firing_rate_ratio"] = firingRate1 / (firingRate2 > 0 ? firingRate2 : 1.0);
    
    double eiratio1 = brain1->getExcitationInhibitionRatio();
    double eiratio2 = brain2->getExcitationInhibitionRatio();
    comparison["excitement_ratio"] = eiratio1 / (eiratio2 > 0 ? eiratio2 : 1.0);
    
    comparison["total_spike_ratio"] = static_cast<double>(brain1->getTotalSpikeCount()) / 
                                       (brain2->getTotalSpikeCount() > 0 ? brain2->getTotalSpikeCount() : 1);
    
    return comparison;
}

// Memory-efficient batch processing
inline std::vector<std::map<std::string, float>> optimizedBatchRunSimulations(
    const std::vector<std::shared_ptr<AgentBrain>>& agents,
    const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
    int steps) {
    
    std::vector<std::map<std::string, float>> results;
    results.reserve(std::min(agents.size(), worlds.size()));
    
    for (size_t i = 0; i < agents.size() && i < worlds.size(); ++i) {
        results.push_back(optimizedRunSimulationSteps(agents[i], worlds[i], steps, false));
    }
    
    return results;
}

} // namespace nlm
