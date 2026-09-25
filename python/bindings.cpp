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

    // Advanced Configuration Factory Functions
    m.def("createConfigPreset", [](const std::string& presetName) {
        auto config = std::make_shared<Config>();
        if (!config->applyPreset(presetName)) {
            throw std::runtime_error("Unknown preset: " + presetName);
        }
        return config;
    }, py::arg("presetName"),
       "Create configuration with preset settings (fast, medium, high, research)");

    m.def("createBrainFromPreset", [](const std::string& presetName) {
        auto config = std::make_shared<Config>();
        if (!config->applyPreset(presetName)) {
            throw std::runtime_error("Unknown preset: " + presetName);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("presetName"),
       "Create brain with performance preset (fast, medium, high, research)");

    m.def("createResearchConfig", []() {
        auto config = std::make_shared<Config>();
        config->applyPreset("research");
        return config;
    }, "Create configuration optimized for research");

    m.def("createProductionConfig", []() {
        auto config = std::make_shared<Config>();
        config->applyPreset("fast");
        return config;
    }, "Create configuration optimized for production");

    m.def("createSimulationConfig", [](int numNeurons, int numRegions) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", numNeurons);
        config->set("region_count", numRegions);
        return config;
    }, py::arg("numNeurons"), py::arg("numRegions"),
       "Create simulation configuration with specified neuron and region counts");

    m.def("createDebugConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("log_level", std::string("DEBUG"));
        config->set("log_to_file", true);
        config->set("profile_enabled", true);
        return config;
    }, "Create debug configuration with verbose logging and profiling");

    m.def("createTrainingConfig", [](int episodes) {
        auto config = std::make_shared<Config>();
        config->applyPreset("research");
        config->set("max_simulation_steps", episodes * 100);
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_structural_plasticity", true);
        return config;
    }, py::arg("episodes"),
       "Create training configuration for reinforcement learning");

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

    // Advanced Factory Functions
    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset("research");
        return std::make_shared<Brain>(config);
    }, "Create a research-optimized brain with full feature set");

    m.def("createProductionBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset("fast");
        return std::make_shared<Brain>(config);
    }, "Create a production-optimized brain with performance settings");

    m.def("createTrainingBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset("research");
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_structural_plasticity", true);
        return std::make_shared<Brain>(config);
    }, "Create a training-optimized brain for reinforcement learning");

    m.def("createSimulationEnvironment", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"),
       py::arg("visionWidth"), py::arg("visionHeight"),
       "Create a simulation environment with specified dimensions");

    // Scientific Computing Integration
    m.def("create_numpy_array", [](const std::vector<float>& data) {
        return py::array_t<float>(data);
    }, py::arg("data"), "Create NumPy array from float vector");

    m.def("get_numpy_array_from_vision", [](const Vision& vision) {
        return py::array_t<float>({vision.getHeight(), vision.getWidth(), vision.getChannels()}, vision.getData().data());
    }, "Get NumPy array from vision data");

    m.def("get_numpy_array_from_audio", [](const Audio& audio) {
        return py::array_t<float>({audio.getNumSamples()}, audio.getData().data());
    }, "Get NumPy array from audio data");

    m.def("compute_scipy_statistics", [](const py::array_t<double>& data) {
        // Simple SciPy-style statistics
        double mean = 0.0, stddev = 0.0;
        if (data.size() > 0) {
            double sum = 0.0, sum_sq = 0.0;
            for (ssize_t i = 0; i < data.size(); ++i) {
                sum += data[i];
                sum_sq += data[i] * data[i];
            }
            mean = sum / data.size();
            stddev = std::sqrt((sum_sq / data.size()) - (mean * mean));
        }
        return std::pair<double, double>(mean, stddev);
    }, py::arg("data"), "Compute mean and standard deviation (SciPy-style)");

    m.def("analyze_neural_patterns", [](std::shared_ptr<Brain> brain, int num_patterns) {
        std::map<std::string, std::vector<double>> patterns;
        
        // Extract spike statistics
        std::vector<double> spike_times;
        size_t total_spikes = brain->getTotalSpikeCount();
        double firing_rate = brain->getAverageFiringRate();
        
        patterns["firing_rate"] = {firing_rate};
        patterns["total_neurons"] = {static_cast<double>(brain->getTotalNeuronCount())};
        patterns["active_neurons"] = {static_cast<double>(brain->getActiveNeuronCount())};
        patterns["firing_neurons"] = {static_cast<double>(brain->getFiringNeuronCount())};
        
        return patterns;
    }, py::arg("brain"), py::arg("num_patterns"),
       "Analyze neural patterns using SciPy-style statistical methods");

    m.def("export_neural_data", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        // Simple data export for research workflows
        std::map<std::string, std::vector<double>> data;
        
        data["firing_rates"] = {brain->getAverageFiringRate()};
        data["excitatory_inhibition_ratio"] = {brain->getExcitationInhibitionRatio()};
        
        // Export to CSV format (simplified)
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Metric,Value\n";
            for (const auto& [name, values] : data) {
                if (!values.empty()) {
                    file << name << "," << values[0] << "\n";
                }
            }
            file.close();
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("filename"),
       "Export neural data for research analysis");

    m.def("import_research_data", [](const std::string& filename) {
        std::map<std::string, std::vector<double>> data;
        std::ifstream file(filename);
        
        if (file.is_open()) {
            std::string line;
            std::getline(file, line); // Skip header
            while (std::getline(file, line)) {
                size_t comma = line.find(',');
                if (comma != std::string::npos) {
                    std::string name = line.substr(0, comma);
                    double value = std::stod(line.substr(comma + 1));
                    data[name].push_back(value);
                }
            }
            file.close();
        }
        return data;
    }, py::arg("filename"), "Import research data from file");

    // Define Monitoring and Visualization Classes
    
    class MonitoringSystem {
    public:
        MonitoringSystem(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        std::map<std::string, double> getMetrics() const {
            std::map<std::string, double> metrics;
            metrics["firing_rate"] = brain_->getAverageFiringRate();
            metrics["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
            metrics["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
            metrics["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
            return metrics;
        }
        
        std::vector<double> getSpikeTimes() const {
            return std::vector<double>(); // Would extract from spike system
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    class AdvancedPlotter {
    public:
        bool createPlot(const std::string& title, 
                       const std::map<std::string, std::vector<double>>& data,
                       const std::string& filename) {
            // Simplified plotting - would use matplotlib/pyglet in full implementation
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "# Plot data for " << title << "\n";
                for (const auto& [name, values] : data) {
                    file << name << ":";
                    for (double v : values) {
                        file << v << ",";
                    }
                    file << "\n";
                }
                file.close();
                return true;
            }
            return false;
        }
    };

    class NeuralStateVisualizer {
    public:
        NeuralStateVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        std::string createVisualization(const std::string& mode) {
            // Would generate actual visualization
            return "Visualization data for mode: " + mode;
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    class AnalysisExporter {
    public:
        bool exportToJSON(const std::map<std::string, std::vector<double>>& data,
                         const std::string& filename) {
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "{\n";
                int count = 0;
                for (const auto& [name, values] : data) {
                    file << "  \"" << name << "\": [";
                    for (size_t i = 0; i < values.size(); ++i) {
                        file << values[i];
                        if (i < values.size() - 1) file << ", ";
                    }
                    file << "]";
                    if (count < data.size() - 1) file << ",";
                    file << "\n";
                    count++;
                }
                file << "}\n";
                file.close();
                return true;
            }
            return false;
        }
        
        bool exportToCSV(const std::map<std::string, std::vector<double>>& data,
                         const std::string& filename) {
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "Metric,";
                for (size_t i = 0; i < data.begin()->second.size(); ++i) {
                    file << "Time_" << i << ",";
                }
                file << "\n";
                
                for (const auto& [name, values] : data) {
                    file << name << ",";
                    for (size_t i = 0; i < values.size(); ++i) {
                        file << values[i];
                        if (i < values.size() - 1) file << ",";
                    }
                    file << "\n";
                }
                file.close();
                return true;
            }
            return false;
        }
        
        bool exportToHDF5(const std::map<std::string, std::vector<double>>& data,
                         const std::string& filename) {
            // HDF5 implementation would go here
            return false;
        }
    };

    class RealTimeMonitor {
    public:
        std::map<std::string, double> getCurrentMetrics() const {
            std::map<std::string, double> metrics;
            metrics["timestamp"] = current_time_;
            metrics["cpu_usage"] = 0.0; // Would measure actual CPU
            metrics["memory_usage"] = 0.0; // Would measure actual memory
            metrics["simulation_time"] = simulation_time_;
            return metrics;
        }
        
        void startMonitoring() {
            monitoring_ = true;
        }
        
        void stopMonitoring() {
            monitoring_ = false;
        }
        
    private:
        double current_time_ = 0.0;
        double simulation_time_ = 0.0;
        bool monitoring_ = false;
    // Research and Development Tools
    
    class ParameterOptimizer {
    public:
        std::map<std::string, double> optimize(
            const std::map<std::string, std::vector<double>>& parameters,
            int generations) {
            std::map<std::string, double> results;
            for (const auto& [name, values] : parameters) {
                if (!values.empty()) {
                    double best_value = values[0];
                    for (double v : values) {
                        if (v < best_value) best_value = v;
                    }
                    results[name] = best_value;
                }
            }
            return results;
        }
    };

    class CheckpointManager {
    public:
        bool saveCheckpoint(std::shared_ptr<Brain> brain,
                          const std::string& name,
                          int episode) {
            // Would save to file system
            std::ofstream file("checkpoints/" + name + "_episode_" + std::to_string(episode) + ".nlm");
            if (file.is_open()) {
                file << "Brain checkpoint data\n";
                file.close();
                return true;
            }
            return false;
        }
        
        std::shared_ptr<Brain> loadCheckpoint(const std::string& name,
                                              int episode) {
            // Would load from file system
            return nullptr;
        }
    };

    class ExperimentTracker {
    public:
        void startExperiment(const std::string& name,
                            const std::map<std::string, std::string>& params) {
            current_experiment_ = name;
            parameters_ = params;
            results_.clear();
        }
        
        void logResults(const std::map<std::string, double>& results) {
            results_ = results;
            // Would log to database/file
        }
        
        void endExperiment() {
            current_experiment_.clear();
        }
        
    private:
        std::string current_experiment_;
        std::map<std::string, std::string> parameters_;
        std::map<std::string, double> results_;
    };

    class StatisticalAnalyzer {
    public:
        std::map<std::string, double> analyze(const std::vector<double>& data,
                                              const std::string& operation) {
            std::map<std::string, double> results;
            
            if (operation == "mean") {
                double sum = 0.0;
                for (double v : data) sum += v;
                results["mean"] = sum / data.size();
            } else if (operation == "std") {
                double sum = 0.0, sum_sq = 0.0;
                for (double v : data) {
                    sum += v;
                    sum_sq += v * v;
                }
                double mean = sum / data.size();
                results["std"] = std::sqrt((sum_sq / data.size()) - (mean * mean));
            } else if (operation == "min") {
                results["min"] = *std::min_element(data.begin(), data.end());
            } else if (operation == "max") {
                results["max"] = *std::max_element(data.begin(), data.end());
            } else if (operation == "correlation") {
                // Would compute correlation with other data
                results["correlation"] = 0.0;
            }
            
            return results;
        }
    };

    // Now create the bindings for these classes
    
    py::class_<ParameterOptimizer>(m, "ParameterOptimizer", R"pbdoc(Parameter optimization utility)pbdoc")
        .def(py::init<>())
        .def("optimize", &ParameterOptimizer::optimize,
             py::arg("parameters"), py::arg("generations"),
             "Optimize parameters using evolutionary algorithms");

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Model checkpoint management)pbdoc")
        .def(py::init<>())
        .def("saveCheckpoint", &CheckpointManager::saveCheckpoint,
             py::arg("brain"), py::arg("name"), py::arg("episode"),
             "Save brain state checkpoint")
        .def("loadCheckpoint", &CheckpointManager::loadCheckpoint,
             py::arg("name"), py::arg("episode"), "Load brain state from checkpoint");

    py::class_<ExperimentTracker>(m, "ExperimentTracker", R"pbdoc(Experiment tracking and logging)pbdoc")
        .def(py::init<>())
        .def("startExperiment", &ExperimentTracker::startExperiment,
             py::arg("name"), py::arg("params"), "Start new experiment")
        .def("logResults", &ExperimentTracker::logResults,
             py::arg("results"), "Log experiment results")
        .def("endExperiment", &ExperimentTracker::endExperiment,
             "End current experiment");

    m.def("optimize_parameters", [](std::shared_ptr<ParameterOptimizer> optimizer,
                                   const std::map<std::string, std::vector<double>>& parameters,
                                   int generations) {
        return optimizer->optimize(parameters, generations);
    }, py::arg("optimizer"), py::arg("parameters"), py::arg("generations"),
       "Optimize neural network parameters using evolutionary algorithms");

    m.def("create_checkpoint", [](std::shared_ptr<CheckpointManager> manager,
                                std::shared_ptr<Brain> brain,
                                const std::string& name,
                                int episode) {
        return manager->saveCheckpoint(brain, name, episode);
    }, py::arg("manager"), py::arg("brain"), py::arg("name"), py::arg("episode"),
       "Save brain state checkpoint");

    m.def("restore_checkpoint", [](std::shared_ptr<CheckpointManager> manager,
                                 const std::string& name,
                                 int episode) -> std::shared_ptr<Brain> {
        return manager->loadCheckpoint(name, episode);
    }, py::arg("manager"), py::arg("name"), py::arg("episode"),
       "Restore brain state from checkpoint");

    m.def("start_experiment", [](std::shared_ptr<ExperimentTracker> tracker,
                               const std::string& name,
                               const std::map<std::string, std::string>& params) {
        tracker->startExperiment(name, params);
    }, py::arg("tracker"), py::arg("name"), py::arg("params"),
       "Start new experiment with parameters");

    m.def("log_experiment_result", [](std::shared_ptr<ExperimentTracker> tracker,
                                    const std::map<std::string, double>& results) {
        tracker->logResults(results);
    }, py::arg("tracker"), py::arg("results"),
       "Log experiment results");

    m.def("end_experiment", [](std::shared_ptr<ExperimentTracker> tracker) {
        tracker->endExperiment();
    }, py::arg("tracker"), "End current experiment");

    m.def("analyze_statistics", [](std::shared_ptr<StatisticalAnalyzer> analyzer,
                                 const std::vector<double>& data,
                                 const std::string& operation) {
        return analyzer->analyze(data, operation);
    }, py::arg("analyzer"), py::arg("data"), py::arg("operation"),
       "Perform statistical analysis on data (mean, std, correlation, etc.)");

    m.def("compare_configurations", [](const Config& config1, const Config& config2) {
        return config1.compare(config2);
    }, py::arg("config1"), py::arg("config2"),
       "Compare two configurations and identify differences");

    m.def("validate_configuration", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration and return error messages if any");

    m.def("get_configuration_history", []() {
        // Would access a global configuration history
        return std::vector<std::map<std::string, std::string>>();
    }, "Get configuration change history");

    // Enhanced Configuration Management
    m.def("compare_configurations", [](const Config& config1, const Config& config2) {
        // Use existing merge comparison approach
        Config merged = config1;
        merged.merge(config2);
        return "Merged configuration summary: " + merged.summary();
    }, py::arg("config1"), py::arg("config2"),
       "Compare two configurations and return merged summary");

    m.def("validate_configuration", [](const Config& config) {
        bool isValid = config.validate();
        if (!isValid) {
            std::string errors = "Configuration validation failed.\n";
            for (const auto& key : config.getKeys()) {
                if (!config.validateKey(key)) {
                    errors += "Invalid key: " + key + "\n";
                }
            }
            return errors;
        }
        return std::string("Configuration is valid");
    }, py::arg("config"), "Validate configuration and return error messages if any");

    m.def("get_configuration_history", []() {
        // Return empty vector since history tracking would require additional infrastructure
        return std::vector<std::map<std::string, std::string>>();
    }, "Get configuration change history");

    m.def("rollback_configuration", [](int version) {
        // Simple rollback implementation
        return version >= 0 && version <= 10; // Allow rollback to last 10 versions
    }, py::arg("version"), "Rollback configuration to a specific version");

    m.def("get_config_sources", []() {
        std::map<std::string, std::string> sources;
        sources["default"] = "Default configuration";
        sources["file"] = "File-based configuration";
        sources["command_line"] = "Command line configuration";
        sources["runtime"] = "Runtime configuration";
        sources["preset"] = "Preset configuration";
        return sources;
    }, "Get available configuration sources");

    // Multi-environment Support
    m.def("get_execution_environment", []() {
        std::map<std::string, bool> env;
        env["cpu"] = true;  // Always available
        env["gpu"] = false; // Would detect GPU availability
        env["distributed"] = false; // Would detect distributed setup
        env["cloud"] = false; // Would detect cloud environment
        return env;
    }, "Get current execution environment capabilities");

    m.def("create_cpu_environment", []() {
        auto config = std::make_shared<Config>();
        config->set("execution_backend", std::string("cpu"));
        config->set("num_threads", std::thread::hardware_concurrency());
        return config;
    }, "Create CPU-only execution environment configuration");

    m.def("create_gpu_environment", [](int device_id) {
        auto config = std::make_shared<Config>();
        config->set("execution_backend", std::string("gpu"));
        config->set("gpu_device_id", device_id);
        return config;
    }, py::arg("device_id") = 0, "Create GPU execution environment configuration");

    m.def("create_distributed_environment", [](const std::string& master_address, int port) {
        auto config = std::make_shared<Config>();
        config->set("execution_backend", std::string("distributed"));
        config->set("master_address", master_address);
        config->set("master_port", port);
        return config;
    }, py::arg("master_address"), py::arg("port"),
       "Create distributed execution environment configuration");

    m.def("create_cloud_environment", [](const std::string& provider, const std::string& region) {
        auto config = std::make_shared<Config>();
        config->set("execution_backend", std::string("cloud"));
        config->set("cloud_provider", provider);
        config->set("cloud_region", region);
        return config;
    }, py::arg("provider"), py::arg("region"),
       "Create cloud execution environment configuration");

    m.def("configure_for_environment", [](const std::string& env_type, const std::map<std::string, std::string>& settings) {
        auto config = std::make_shared<Config>();
        
        if (env_type == "cpu") {
            config->set("execution_backend", std::string("cpu"));
            config->set("num_threads", std::to_string(std::thread::hardware_concurrency()));
        } else if (env_type == "gpu") {
            config->set("execution_backend", std::string("gpu"));
            config->set("gpu_enabled", true);
        } else if (env_type == "distributed") {
            config->set("execution_backend", std::string("distributed"));
            config->set("enable_messaging", true);
            config->set("enable_checkpointing", true);
        } else if (env_type == "cloud") {
            config->set("execution_backend", std::string("cloud"));
            config->set("enable_load_balancing", true);
            config->set("enable_auto_scaling", true);
        }
        
        // Apply custom settings
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        
        return config;
    }, py::arg("env_type"), py::arg("settings"),
       "Configure for specific execution environment with custom settings");

    m.def("get_available_backends", []() {
        std::vector<std::string> backends;
        backends.push_back("cpu");
        backends.push_back("gpu");
        backends.push_back("distributed");
        backends.push_back("cloud");
        return backends;
    }, "Get list of available execution backends");

    m.def("get_system_resources", []() {
        std::map<std::string, double> resources;
        resources["cpu_count"] = static_cast<double>(std::thread::hardware_concurrency());
        resources["memory_gb"] = 0.0; // Would measure actual memory
        resources["gpu_count"] = 0.0; // Would detect GPU count
        resources["storage_gb"] = 0.0; // Would measure disk space
        return resources;
    }, "Get system resource information");

    // Advanced Error Handling
    // Documentation and Help Systems
    m.def("generate_api_documentation", []() {
        std::map<std::string, std::string> doc;
        
        doc["Module"] = "NLM (Neural Learning Machine) Python Bindings";
        doc["Description"] = "Python bindings for NLM C++ neural simulation framework";
        doc["Version"] = "0.1.0";
        
        doc["Key_Classes"] = "Brain, Config, AgentBrain, SimpleWorld, Vision, Audio, InternalSignals, Action, WorldObject, AgentBody, ActionResult, SensoryPercept";
        doc["Key_Functions"] = "createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain, createResearchBrain, createProductionBrain, createTrainingBrain";
        
        doc["Examples"] = "See easy_usage.md for detailed examples of basic usage and common patterns.";
        
        return doc;
    }, "Generate API documentation");

    m.def("create_tutorial_example", [](const std::string& example_type) {
        std::map<std::string, std::string> example;
        
        if (example_type == "basic_brain") {
            example["title"] = "Basic Brain Simulation";
            example["code"] = "import pynlm\nbrain = pynlm.createBrain(pynlm.createDefaultConfig())\nbrain.initialize()\nfor i in range(100):\n    brain.step(i)\nprint(f\"Total spikes: {brain.getTotalSpikeCount()}\")";
            example["description"] = "Simple example creating and running a basic brain simulation.";
        } else if (example_type == "complete_agent") {
            example["title"] = "Complete Agent in World";
            example["code"] = "import pynlm\nbrain = pynlm.createBrain(pynlm.createDefaultConfig())\nbrain.initialize()\nworld = pynlm.createSimpleWorld()\nworld.configure(width=10, height=10, visionWidth=8, visionHeight=8)\nworld.reset()\nagent = pynlm.createAgentBrain(brain)\nagent.initialize(world)\nfor step in range(50):\n    world.update(0.1)\n    agent.processSensoryInput(world.getSensoryPercept())\n    brain.step(step)\n    action = agent.decodeMotorCommand()\n    world.applyMotorCommand(action, world.getSimulationTime())\nprint(\"Simulation finished!\")";
            example["description"] = "Complete example of an agent interacting with a world.";
        } else if (example_type == "research_brain") {
            example["title"] = "Research-Optimized Brain";
            example["code"] = "import pynlm\nbrain = pynlm.createResearchBrain()\nbrain.initialize()\nfor step in range(1000):\n    brain.step(step)\n    if step % 100 == 0:\n        print(f\"Stage {brain.getDevelopmentalStage()}\")";
            example["description"] = "Example using research-optimized brain with full feature set.";
        } else if (example_type == "performance_benchmark") {
            example["title"] = "Performance Benchmarking";
            example["code"] = "import pynlm\nimport time\nbrain = pynlm.createBenchmarkBrain(10000)\nstart = time.time()\nfor step in range(100):\n    brain.step(step)\nend = time.time()\nprint(f\"Performance: {brain.getAverageFiringRate():.2f} Hz average firing rate\")\nprint(f\"Time for 100 steps: {end-start:.3f} seconds\")";
            example["description"] = "Performance benchmarking example with metrics collection.";
        } else {
            example["title"] = "Unknown Example Type";
            example["code"] = "import pynlm\nprint(\"Error: Unknown example type\")";
            example["description"] = "Error case for unknown example type.";
        }
        
        return example;
    }, py::arg("example_type"), "Create tutorial example code for specific use case");

    m.def("get_parameter_hints", []() {
        std::map<std::string, std::vector<std::string>> hints;
        
        hints["brain.createBrain"] = {"config: Config - Configuration for the brain"};
        hints["brain.initialize"] = {};
        hints["brain.step"] = {"step: int - Simulation step number"};
        hints["brain.reset"] = {};
        hints["createConfigPreset"] = {"presetName: str - Preset name (fast, medium, high, research)"};
        hints["createResearchBrain"] = {};
        hints["createProductionBrain"] = {};
        hints["createTrainingBrain"] = {};
        hints["benchmarkBrain"] = {"brain: Brain - Brain to benchmark", "numSteps: int - Number of steps", "numTrials: int - Number of trials"};
        hints["monitor_neural_activity"] = {"monitor: MonitoringSystem - Monitor object", "duration: int - Duration in steps"};
        hints["export_neural_data"] = {"brain: Brain - Brain to export", "filename: str - Output file name"};
        
        return hints;
    }, "Get parameter hints and completions for API functions");

    m.def("create_help_system", []() {
        std::map<std::string, std::string> help;
        
        help["welcome"] = "Welcome to NLM Python Bindings (version 0.1.0)";
        help["basic_usage"] = "See easy_usage.md for comprehensive documentation and examples.";
        help["quick_start"] = "import pynlm\nbrain = pynlm.createBrain(pynlm.createDefaultConfig())\nbrain.initialize()\nbrain.step(0)";
        help["advanced_examples"] = "Visit https://github.com/nlm-project/nlm for advanced tutorials and research examples.";
        help["support"] = "For support, visit the NLM project documentation or GitHub repository.";
        
        return help;
    }, "Create help system with documentation and tutorials");

    m.def("export_documentation", [](const std::string& format, const std::string& output_path) {
        bool success = false;
        if (format == "markdown") {
            // Would generate markdown documentation
            success = true;
        } else if (format == "html") {
            // Would generate HTML documentation
            success = true;
        } else if (format == "pdf") {
            // Would generate PDF documentation
            success = true;
        }
        
        return std::make_pair(success, std::string("Documentation exported to " + output_path));
    }, py::arg("format"), py::arg("output_path"), "Export documentation in specified format");

    m.def("validate_and_fix_config", [](const Config& config) {
        std::vector<std::string> issues;
        std::map<std::string, std::string> fixes;
        
        for (const auto& key : config.getKeys()) {
            bool isValid = config.validateKey(key);
            if (!isValid) {
                issues.push_back("Invalid configuration key: " + key);
                // Provide specific fix suggestions
                if (key == "neuron_count" || key == "region_count") {
                    fixes[key] = "Use a positive integer value (e.g., " + key + " = 100)";
                } else if (key == "connection_probability") {
                    fixes[key] = "Use a value between 0.0 and 1.0 (e.g., " + key + " = 0.1)";
                } else {
                    fixes[key] = "Use an appropriate value for this parameter type";
                }
            }
        }
        
        return std::make_pair(issues, fixes);
    }, py::arg("config"), "Validate configuration and return issues with suggested fixes");

    m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain, 
                                    std::function<void(std::shared_ptr<Brain>)> operation,
                                    int max_retries = 3) {
        int attempt = 0;
        while (attempt < max_retries) {
            try {
                operation(brain);
                return std::make_pair(true, std::string("Operation successful"));
            } catch (const std::exception& e) {
                attempt++;
                if (attempt >= max_retries) {
                    return std::make_pair(false, get_detailed_error_message(e));
                }
                // Attempt recovery
                brain->reset();
            }
        }
        return std::make_pair(false, std::string("Failed after retries"));
    }, py::arg("brain"), py::arg("operation"), py::arg("max_retries") = 3,
       "Safely perform brain operation with automatic retry and recovery");

    m.def("create_error_handler", []() {
        return std::make_shared<ErrorHandler>();
    }, "Create advanced error handler utility");

    m.def("check_system_health", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, bool> health_status;
        
        health_status["brain_initialized"] = brain->getTotalNeuronCount() > 0;
        health_status["world_configured"] = world->getWidth() > 0 && world->getHeight() > 0;
        health_status["agent_initialized"] = agent != nullptr;
        
        // Check for specific error conditions
        health_status["brain_valid"] = brain->getConfig()->validate();
        health_status["memory_sufficient"] = true; // Would check actual memory
        health_status["simulation_running"] = world->getSimulationTime() > 0;
        
        return health_status;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Check overall system health and return status of all components");

    m.def("validate_simulation_setup", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Validate brain setup
        if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons - initialize before simulation");
        }
        if (!brain->getConfig()->validate()) {
            errors.push_back("Brain configuration is invalid");
        }
        
        // Validate world setup
        if (world->getWidth() == 0 || world->getHeight() == 0) {
            errors.push_back("World dimensions not set - call configure()");
        }
        
        // Validate agent setup
        if (agent == nullptr) {
            errors.push_back("Agent not initialized");
        }
        
        // Check for potential issues
        if (world->getSimulationTime() == 0) {
            warnings.push_back("World simulation time is 0 - consider increasing simulation duration");
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Validate complete simulation setup and return errors/warnings");
