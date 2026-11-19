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
        })
        // Enhanced validation methods
        .def("validate", &Config::validate, py::arg("useCase") = "",
             "Validate configuration for specific use case")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation errors")
        .def("hasRequiredKeys", &Config::hasRequiredKeys,
             "Check if configuration has required keys")
        .def("getValidationReport", &Config::getValidationReport,
             "Get validation report")
        .def("validateParameter", [](const Config& self, const std::string& key, double min, double max) {
            return self.validateParameter(key, min, max);
        }, py::arg("key"), py::arg("minValue"), py::arg("maxValue"),
           "Validate parameter is within range [min, max]")
        .def("validateParameter", [](const Config& self, const std::string& key, int min, int max) {
            return self.validateParameter(key, min, max);
        }, py::arg("key"), py::arg("minValue"), py::arg("maxValue"),
           "Validate parameter is within integer range [min, max]")
        .def("validateParameter", [](const Config& self, const std::string& key, bool required) {
            return self.validateParameter(key, required);
        }, py::arg("key"), py::arg("required"),
           "Validate required parameter exists");
        
        // Advanced configuration helpers
        .def("validateBrainConfig", [](Config& self) {
            std::vector<std::string> requiredKeys = {
                "brain.neuron_count",
                "brain.synapse_density", 
                "simulation_timestep"
            };
            return self.hasRequiredKeys(requiredKeys) && 
                   self.validateParameter("brain.neuron_count", 100, 10000) &&
                   self.validateParameter("brain.synapse_density", 0.01, 0.5) &&
                   self.validateParameter("simulation_timestep", 0.0, 0.1);
        }, "Validate typical brain configuration")
        .def("validateWorldConfig", [](Config& self) {
            std::vector<std::string> requiredKeys = {
                "environment_width",
                "environment_height",
                "visionWidth",
                "visionHeight"
            };
            return self.hasRequiredKeys(requiredKeys);
        }, "Validate world configuration")
        .def("createDefaultBrainConfig", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
            config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
            config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Default);
            config->set("brain.v_thresh", -50.0f, nlm::ConfigSource::Default);
            config->set("brain.v_rest", -70.0f, nlm::ConfigSource::Default);
            config->set("brain.tau_mem", 20.0f, nlm::ConfigSource::Default);
            config->set("brain.tau_ref", 2.0f, nlm::ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
            config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
            return config;
        }, "Create a default brain configuration")
        .def("createDefaultWorldConfig", []() {
            auto config = std::make_shared<Config>();
            config->set("environment_name", std::string("GridWorld"), nlm::ConfigSource::Default);
            config->set("environment_width", 10.0f, nlm::ConfigSource::Default);
            config->set("environment_height", 10.0f, nlm::ConfigSource::Default);
            config->set("visionWidth", 8, nlm::ConfigSource::Default);
            config->set("visionHeight", 8, nlm::ConfigSource::Default);
            config->set("maxEnergy", 100.0f, nlm::ConfigSource::Default);
            return config;
        }, "Create a default world configuration")
        .def("mergeConfigs", [](const Config& base, const Config& override) {
            auto merged = std::make_shared<Config>(base);
            for (const auto& key : override.getKeys()) {
                auto value = override.get<std::string>(key);
                if (value) {
                    merged->set(key, *value, nlm::ConfigSource::Runtime);
                }
            }
            return merged;
        }, py::arg("base"), py::arg("override"),
           "Merge two configurations (override takes precedence)");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

m.def("createAdvancedBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create an advanced brain with default settings");
        
        m.def("createSimulationConfig", [](int neuronCount, double timestep, int regions) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
            config->set("simulation_timestep", timestep, nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(regions), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
            return config;
        }, py::arg("neuronCount") = 1000, py::arg("timestep") = 0.001, py::arg("regions") = 2,
           "Create a simulation-ready configuration");
        
        m.def("runSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
            for (nlm::SimulationStep step = 0; step < steps; ++step) {
                brain->step(step);
            }
            return brain->getTotalSpikeCount();
        }, py::arg("brain"), py::arg("steps"),
           "Run a complete simulation and return spike count");
        
        m.def("analyzeBrainState", [](const std::shared_ptr<Brain>& brain) {
            std::map<std::string, py::object> analysis;
            analysis["neuron_count"] = brain->getTotalNeuronCount();
            analysis["synapse_count"] = brain->getTotalSynapseCount();
            analysis["firing_rate"] = brain->getAverageFiringRate();
            analysis["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
            analysis["total_spikes"] = brain->getTotalSpikeCount();
            return analysis;
        }, py::arg("brain"),
           "Analyze brain state and return key statistics");
        
        m.def("benchmarkConfiguration", [](const std::shared_ptr<Config>& config, size_t testSteps) {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain for benchmarking");
            }
            
            auto start = std::chrono::high_resolution_clock::now();
            for (nlm::SimulationStep step = 0; step < testSteps; ++step) {
                brain->step(step);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(end - start).count();
            double stepsPerSecond = testSteps / duration;
            
            std::map<std::string, double> results;
            results["steps_per_second"] = stepsPerSecond;
            results["spikes_per_step"] = brain->getTotalSpikeCount() / static_cast<double>(testSteps);
            results["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            results["synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            
            return results;
        }, py::arg("config"), py::arg("testSteps") = 1000,
           "Benchmark configuration performance");
        
        // Advanced error handling
        m.def("safeBrainOperation", [](std::shared_ptr<Brain> brain, size_t steps) {
            try {
                for (nlm::SimulationStep step = 0; step < steps; ++step) {
                    brain->step(step);
                }
                return brain->getTotalSpikeCount();
            } catch (const std::exception& e) {
                throw nlm::RuntimeError(std::string("Brain operation failed: ") + e.what());
            }
        }, py::arg("brain"), py::arg("steps"),
           "Safely run brain simulation with error handling");

            m.def("benchmarkSimulation", [](std::shared_ptr<Config> config, size_t steps) {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain for benchmarking");
            }
            
            // Warm up
            for (nlm::SimulationStep step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            // Actual benchmark
            auto start = std::chrono::high_resolution_clock::now();
            for (nlm::SimulationStep step = 100; step < steps; ++step) {
                brain->step(step);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(end - start).count();
            double stepsPerSecond = (steps - 100) / duration;
            double avgSpikesPerStep = brain->getTotalSpikeCount() / static_cast<double>(steps);
            
            return std::make_tuple(stepsPerSecond, avgSpikesPerStep, brain->getTotalSpikeCount());
        }, py::arg("config"), py::arg("steps"),
           "Benchmark simulation performance and return (steps_per_second, avg_spikes_per_step, total_spikes)");
        
        m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
            if (!brain->save(filepath)) {
                throw std::runtime_error("Failed to export brain state to " + filepath);
            }
        }, py::arg("brain"), py::arg("filepath"),
           "Export brain state (checkpoint) to file");
        
        m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
            if (!brain->load(filepath)) {
                throw std::runtime_error("Failed to import brain state from " + filepath);
            }
        }, py::arg("brain"), py::arg("filepath"),
           "Import brain state (checkpoint) from file");
        
        m.def("resetBrain", [](std::shared_ptr<Brain> brain) {
            brain->reset();
        }, py::arg("brain"),
           "Reset brain to initial state");
        
        m.def("initializeBrain", [](std::shared_ptr<Brain> brain) {
            brain->initialize();
        }, py::arg("brain"),
           "Initialize brain with current configuration");
        
        m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world, size_t steps) {
            // This is a simplified episode runner for testing
            for (nlm::SimulationStep step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            return brain->getTotalSpikeCount();
        }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
           "Run a complete episode with brain, agent, and world");
        
        // Advanced configuration utilities
        m.def("createPresetConfig", [](const std::string& presetName) {
            auto config = std::make_shared<Config>();
            
            if (presetName == "minimal") {
                config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
                config->set("simulation_timestep", 0.01, nlm::ConfigSource::Default);
            } else if (presetName == "standard") {
                config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
                config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
                config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
            } else if (presetName == "complex") {
                config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
                config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
                config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
                config->set("plasticity_learning_rate", 0.01, nlm::ConfigSource::Default);
            } else {
                throw std::runtime_error("Unknown preset name: " + presetName);
            }
            
            return config;
        }, py::arg("presetName"),
           "Create a preset configuration (minimal, standard, complex)");
        
        m.def("listPresets", []() {
            return std::vector<std::string>{"minimal", "standard", "complex"};
        }, "List available preset configuration names");
        
        // Advanced analysis and statistics
        m.def("computeNetworkMetrics", [](std::shared_ptr<Brain> brain) {
            std::map<std::string, double> metrics;
            
            // Basic statistics
            metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            metrics["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
            metrics["firing_neuron_percentage"] = 
                (metrics["total_neurons"] > 0) ? 
                100.0 * metrics["active_neurons"] / metrics["total_neurons"] : 0.0;
            
            // Network properties
            metrics["average_firing_rate"] = brain->getAverageFiringRate();
            metrics["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
            metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            metrics["spikes_per_neuron"] = 
                (metrics["total_neurons"] > 0) ? 
                metrics["total_spikes"] / metrics["total_neurons"] : 0.0;
            
            // Connectivity
            metrics["synapses_per_neuron"] = 
                (metrics["total_neurons"] > 0) ? 
                metrics["total_synapses"] / metrics["total_neurons"] : 0.0;
            
            return metrics;
        }, py::arg("brain"),
           "Compute comprehensive network metrics");
        
        m.def("analyzeSpikingPattern", [](std::shared_ptr<Brain> brain, size_t windowSize) {
            // This is a simplified analysis - real implementations would have more sophisticated analysis
            std::map<std::string, double> analysis;
            
            // Get current spike count
            analysis["current_spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
            analysis["current_firing_rate"] = brain->getAverageFiringRate();
            
            // For a real implementation, we would track spike history and analyze patterns
            // This is a placeholder for advanced spiking analysis
            return analysis;
        }, py::arg("brain"), py::arg("windowSize") = 100,
           "Analyze current spiking pattern (placeholder for advanced analysis)");
        
        // Configuration comparison utilities
        m.def("diffConfigs", [](const std::shared_ptr<Config>& config1, 
                               const std::shared_ptr<Config>& config2) {
            std::vector<std::string> differences;
            
            auto keys1 = config1->getKeys();
            auto keys2 = config2->getKeys();
            
            // Check for keys only in config1
            for (const auto& key : keys1) {
                if (!config2->has(key)) {
                    differences.push_back("config1 has key \"" + key + "\" but config2 does not");
                }
            }
            
            // Check for keys only in config2
            for (const auto& key : keys2) {
                if (!config1->has(key)) {
                    differences.push_back("config2 has key \"" + key + "\" but config1 does not");
                }
            }
            
            // Check for different values
            for (const auto& key : keys1) {
                if (config2->has(key)) {
                    auto value1 = config1->get<std::string>(key);
                    auto value2 = config2->get<std::string>(key);
                    if (value1 && value2 && *value1 != *value2) {
                        differences.push_back("Difference in \"" + key + "\": \"" + 
                                            (*value1) + "\" vs \"" + (*value2) + "\"");
                    }
                }
            }
            
            return differences;
        }, py::arg("config1"), py::arg("config2"),
           "Compare two configurations and return list of differences");
        
        m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, 
                                const std::shared_ptr<Config>& override) {
            auto merged = std::make_shared<Config>(base);
            for (const auto& key : override->getKeys()) {
                auto value = override->get<std::string>(key);
                if (value) {
                    merged->set(key, *value, nlm::ConfigSource::Runtime);
                }
            }
            return merged;
        }, py::arg("base"), py::arg("override"),
           "Merge two configurations (override takes precedence)");
        
        // Validation utilities
        m.def("validateConfigForUseCase", [](const std::shared_ptr<Config>& config, 
                                            const std::string& useCase) {
            bool isValid = config->validate(useCase);
            auto errors = config->getValidationErrors();
            return std::make_pair(isValid, errors);
        }, py::arg("config"), py::arg("useCase"),
           "Validate configuration for specific use case and return (is_valid, errors)");
        
        m.def("validateConfigComplete", [](const std::shared_ptr<Config>& config) {
            bool isValid = config->validate();
            auto errors = config->getValidationErrors();
            return std::make_pair(isValid, errors);
        }, py::arg("config"),
           "Validate configuration completeness and return (is_valid, errors)");

} // namespace nlm
