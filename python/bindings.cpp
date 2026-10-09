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

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    m.def("createBrainConfig", 
          [](std::unordered_map<std::string, std::variant<int64_t, double, std::string>> settings) {
              auto config = std::make_shared<Config>();
              
              // Set default values first
              config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
              config->set("simulation_timestep", 0.001, ConfigSource::Default);
              config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
              config->set("connection_probability", 0.15f, ConfigSource::Default);
              config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
              config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
              config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
              config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
              config->set("pruning_rate", 0.00001f, ConfigSource::Default);
              
              // Override with user settings
              for (const auto& setting : settings) {
                  config->set(setting.first, setting.second);
              }
              
              return config;
          }, py::arg("settings") = std::unordered_map<std::string, std::variant<int64_t, double, std::string>>(),
          "Create a brain configuration with optional custom settings");

    m.def("createWorldConfig", 
          [](int width, int height, int visionWidth, int visionHeight) {
              auto config = std::make_shared<Config>();
              config->set("environment_width", width);
              config->set("environment_height", height);
              config->set("vision_width", visionWidth);
              config->set("vision_height", visionHeight);
              config->set("environment_name", std::string("GridWorld"));
              return config;
          }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
          "Create a configuration for a world environment");

    m.def("createPhase2Config", 
          [](int neuronCount, int regionCount, double timestep) {
              auto config = std::make_shared<Config>();
              config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
              config->set("simulation_timestep", timestep, ConfigSource::Default);
              config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
              config->set("connection_probability", 0.15f, ConfigSource::Default);
              config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
              config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
              config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
              config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
              config->set("pruning_rate", 0.00001f, ConfigSource::Default);
              return config;
          }, py::arg("neuronCount") = 500, py::arg("regionCount") = 1, py::arg("timestep") = 0.001,
          "Create a Phase 2 configuration (basic neural computation)");

    m.def("createPhase3Config", 
          [](int worldWidth, int worldHeight, int neuronCount, int regionCount) {
              auto config = std::make_shared<Config>();
              
              // Environment settings
              config->set("environment_width", worldWidth);
              config->set("environment_height", worldHeight);
              config->set("environment_name", std::string("GridWorld"));
              config->set("vision_width", 16);
              config->set("vision_height", 16);
              
              // Brain settings
              config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
              config->set("simulation_timestep", 0.001, ConfigSource::Default);
              config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
              config->set("connection_probability", 0.1f, ConfigSource::Default);
              
              // Neuromodulation
              config->set("enable_reward_modulation", true, ConfigSource::Default);
              config->set("enable_structural_plasticity", true, ConfigSource::Default);
              config->set("enable_development", true, ConfigSource::Default);
              config->set("enable_curiosity", true, ConfigSource::Default);
              
              return config;
          }, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20, 
          py::arg("neuronCount") = 500, py::arg("regionCount") = 2,
          "Create a Phase 3 configuration (world interaction)");

    m.def("createPhase4Config", 
          [](int neuronCount, bool enableLearning, bool enableDevelopment) {
              auto config = std::make_shared<Config>();
              config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
              config->set("simulation_timestep", 0.001, ConfigSource::Default);
              config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
              config->set("connection_probability", 0.05f, ConfigSource::Default);
              config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
              config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
              config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
              
              // Enable advanced features based on parameters
              config->set("enable_reward_modulation", enableLearning, ConfigSource::Default);
              config->set("enable_structural_plasticity", enableLearning, ConfigSource::Default);
              config->set("enable_development", enableDevelopment, ConfigSource::Default);
              config->set("enable_curiosity", true, ConfigSource::Default);
              
              return config;
          }, py::arg("neuronCount") = 500, py::arg("enableLearning") = true, 
          py::arg("enableDevelopment") = true,
          "Create a Phase 4 configuration (advanced cognition)");

    m.def("createDefaultTestConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set up a comprehensive default configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
        config->set("dopamine_baseline", 0.0f, ConfigSource::Default);
        config->set("reward_discount_factor", 0.99f, ConfigSource::Default);
        config->set("environment_width", 10, ConfigSource::Default);
        config->set("environment_height", 10, ConfigSource::Default);
        config->set("log_level", std::string("INFO"), ConfigSource::Default);
        config->set("log_to_file", false, ConfigSource::Default);
        config->set("max_simulation_steps", 10000LL, ConfigSource::Default);
        
        return config;
    }, "Create a comprehensive default configuration suitable for most tests");

    m.def("createQuickTestConfig", 
          [](int quickNeuronCount) {
              auto config = std::make_shared<Config>();
              config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
              config->set("simulation_timestep", 0.001, ConfigSource::Default);
              config->set("neuron_count", static_cast<int64_t>(quickNeuronCount), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
              config->set("connection_probability", 0.05f, ConfigSource::Default);
              config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
              config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
              config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
              config->set("pruning_rate", 0.000005f, ConfigSource::Default);
              config->set("log_level", std::string("WARNING"), ConfigSource::Default);
              return config;
          }, py::arg("quickNeuronCount") = 100,
          "Create a lightweight configuration for quick testing");

    m.def("createProductionConfig", 
          [](int productionNeuronCount) {
              auto config = std::make_shared<Config>();
              config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
              config->set("simulation_timestep", 0.01, ConfigSource::Default);  // Larger timestep for performance
              config->set("neuron_count", static_cast<int64_t>(productionNeuronCount), ConfigSource::Default);
              config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
              config->set("connection_probability", 0.02f, ConfigSource::Default);
              config->set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
              config->set("stdp_ltd_weight", 0.008f, ConfigSource::Default);
              config->set("stdp_time_constant", 50.0f, ConfigSource::Default);  // Slower plasticity
              config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
              config->set("pruning_rate", 0.000001f, ConfigSource::Default);
              config->set("log_level", std::string("ERROR"), ConfigSource::Default);  // Minimal logging
              config->set("max_simulation_steps", 5000LL, ConfigSource::Default);
              config->set("enable_memory_systems", true, ConfigSource::Default);
              config->set("enable_neuromodulation", true, ConfigSource::Default);
              config->set("enable_prediction_system", true, ConfigSource::Default);
              config->set("enable_cognition_systems", true, ConfigSource::Default);
              return config;
          }, py::arg("productionNeuronCount") = 1000,
          "Create a production-ready configuration optimized for performance");

    m.def("createDebugConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(0), ConfigSource::Default);
        config->set("simulation_timestep", 0.0001, ConfigSource::Default);  // Very small timestep for accuracy
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);  // Smaller for debugging
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.05f, ConfigSource::Default);  // Strong plasticity
        config->set("stdp_ltd_weight", 0.04f, ConfigSource::Default);
        config->set("stdp_time_constant", 10.0f, ConfigSource::Default);  // Faster plasticity
        config->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);  // High structural plasticity
        config->set("pruning_rate", 0.0001f, ConfigSource::Default);
        config->set("log_level", std::string("DEBUG"), ConfigSource::Default);  // Maximum verbosity
        config->set("log_to_file", true, ConfigSource::Default);
        config->set("max_simulation_steps", 20000LL, ConfigSource::Default);
        config->set("visualization_enabled", true, ConfigSource::Default);
        return config;
    }, "Create a debug configuration optimized for accuracy and detailed logging");

    m.def("createConfigFromEnvironment", 
          [](const std::string& envType) {
              auto config = std::make_shared<Config>();
              
              if (envType == "development" || envType == "dev") {
                  // Development environment: detailed logging, smaller simulations
                  config->set("log_level", std::string("DEBUG"), ConfigSource::Default);
                  config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
                  config->set("simulation_timestep", 0.001, ConfigSource::Default);
                  config->set("max_simulation_steps", 5000LL, ConfigSource::Default);
              } else if (envType == "production" || envType == "prod") {
                  // Production environment: optimized for performance
                  config->set("log_level", std::string("WARNING"), ConfigSource::Default);
                  config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                  config->set("simulation_timestep", 0.01, ConfigSource::Default);
                  config->set("max_simulation_steps", 10000LL, ConfigSource::Default);
              } else if (envType == "testing" || envType == "test") {
                  // Testing environment: comprehensive, reproducible
                  config->set("log_level", std::string("INFO"), ConfigSource::Default);
                  config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
                  config->set("simulation_timestep", 0.001, ConfigSource::Default);
                  config->set("max_simulation_steps", 10000LL, ConfigSource::Default);
              } else if (envType == "research" || envType == "research") {
                  // Research environment: maximum detail
                  config->set("log_level", std::string("DEBUG"), ConfigSource::Default);
                  config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                  config->set("simulation_timestep", 0.0005, ConfigSource::Default);
                  config->set("max_simulation_steps", 50000LL, ConfigSource::Default);
              } else {
                  throw std::runtime_error("Unknown environment type: " + envType);
              }
              
              return config;
          }, py::arg("envType"), 
          "Create a configuration optimized for a specific environment type");

    m.def("copyConfig", 
          [](const Config& source) {
              auto config = std::make_shared<Config>();
              // Copy all settings from source (simplified implementation)
              auto keys = source.getKeys();
              for (const auto& key : keys) {
                  auto value = source.getValue(key);
                  config->set(key, value);
              }
              return config;
          }, py::arg("source"),
          "Create a copy of an existing configuration");

    m.def("mergeConfigs", 
          [](const Config& baseConfig, const Config& overrideConfig) {
              auto config = std::make_shared<Config>();
              
              // Copy base config
              auto baseKeys = baseConfig.getKeys();
              for (const auto& key : baseKeys) {
                  auto value = baseConfig.getValue(key);
                  config->set(key, value);
              }
              
              // Override with override config
              auto overrideKeys = overrideConfig.getKeys();
              for (const auto& key : overrideKeys) {
                  auto value = overrideConfig.getValue(key);
                  config->set(key, value);
              }
              
              return config;
          }, py::arg("baseConfig"), py::arg("overrideConfig"),
          "Merge two configurations (base + override)");
