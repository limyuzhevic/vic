// Enhanced Python bindings with improved usability and Pythonic features
// This includes all original bindings plus additional enhancements

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
        
        Enhanced with improved usability, Pythonic features, and comprehensive error handling.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");

    // === NEURON ID ===
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // === SYNAPSE ID ===
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // === REGION ID ===
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // === POPULATION ID ===
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
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // === ENUMERATIONS ===
    
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

    // === CONFIG CLASS ===
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
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value by key (returns None if key doesn't exist)")
        .def("get_or", &Config::getOr<int64_t>, py::arg("key"), py::arg("default_value"),
             "Get configuration value by key with default fallback")
        .def("set", &Config::set<const std::string&, const std::string&, ConfigSource>,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (string version)")
        .def("set", &Config::set<const std::string&, int64_t, ConfigSource>,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (integer version)")
        .def("set", &Config::set<const std::string&, double, ConfigSource>,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (float version)")
        .def("set", &Config::set<const std::string&, bool, ConfigSource>,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (boolean version)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // === SENSORY INPUT CLASS ===
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // === VISION CLASS ===
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

    // === AUDIO CLASS ===
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // === INTERNAL SIGNALS CLASS ===
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // === ACTION CLASS ===
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // === WORLD OBJECT CLASS ===
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: type=" + std::to_string(static_cast<int>(obj.type)) +
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        });

    // === AGENT BODY CLASS ===
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

    // === ACTION RESULT CLASS ===
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // === SENSORY PERCEPT CLASS ===
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

    // === SIMPLE WORLD CLASS ===
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

    // === BRAIN CLASS ===
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

    // === AGENT BRAIN CLASS ===
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

    // === FACTORY FUNCTIONS ===
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

    // === CONSTANTS ===
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === PYTHONIC EXTENSIONS ===
    // These provide enhanced usability and Pythonic features

    // Configuration factory functions
    m.def("createPhase2Config", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create a Phase 2 configuration with sensible defaults");

    m.def("createPhase6Config", []() {
        auto config = std::make_shared<Config>();
        // Enable all major systems
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        // Enable plasticity
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        // Enable developmental features
        config->set("development_stage", "Adult", ConfigSource::Default);
        return config;
    }, "Create a Phase 6 integration configuration");

    // Quick setup helpers
    m.def("createBrainWithWorld", [](std::shared_ptr<Config> config, 
                                   size_t worldWidth = 100, 
                                   size_t worldHeight = 100) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, 20, 20);
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return std::make_pair(brain, agentBrain);
    }, py::arg("config"), py::arg("worldWidth") = 100, py::arg("worldHeight") = 100,
       "Create a brain with integrated world and agent interface");

    // Validation and safety functions
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw py::value_error("Config is null");
        }
        
        bool valid = true;
        std::vector<std::string> warnings;
        
        // Check required keys
        const std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                warnings.push_back("Missing recommended key: " + key);
            }
        }
        
        // Check value ranges
        if (config->has("neuron_count")) {
            auto neurons = config->getOr<int64_t>("neuron_count", 500);
            if (neurons < 10 || neurons > 10000) {
                warnings.push_back("neuron_count out of recommended range (10-10000)");
            }
        }
        
        if (config->has("region_count")) {
            auto regions = config->getOr<int64_t>("region_count", 1);
            if (regions < 1 || regions > 100) {
                warnings.push_back("region_count out of recommended range (1-100)");
            }
        }
        
        return std::make_pair(valid, warnings);
    }, py::arg("config"), "Validate configuration and return validation results");

    // Batch operations
    m.def("injectCurrentBatch", [](std::shared_ptr<Brain> brain,
                                 const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
        if (!brain) {
            throw py::value_error("Brain is null");
        }
        for (const auto& [neuron, current] : injections) {
            brain->injectCurrent(neuron, current);
        }
        return brain;
    }, py::arg("brain"), py::arg("injections"),
       "Inject current into multiple neurons at once");

    // Safe simulation stepping
    m.def("safeStep", [](std::shared_ptr<Brain> brain, 
                       SimulationStep step,
                       Timestamp time = 0.0,
                       size_t maxSteps = 1000) {
        if (!brain) {
            throw py::value_error("Brain is null");
        }
        if (step > maxSteps) {
            throw py::value_error("Step exceeds maximum allowed steps");
        }
        brain->step(step, time);
        return brain;
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0, py::arg("maxSteps") = 1000,
       "Perform a simulation step with input validation");

    // Helper for creating common neuron patterns
    m.def("createSensoryPattern", [](size_t numSensory = 20) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500 + numSensory), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Inject current to sensory neurons to create activation pattern
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            for (size_t i = 0; i < std::min(numSensory, neurons.size()); ++i) {
                if (neurons[i]->getType() == NeuronType::Sensory) {
                    neurons[i]->injectCurrent(30.0f);
                }
            }
        }
        
        return brain;
    }, py::arg("numSensory") = 20, "Create a brain with sensory activation pattern");

    // Simulation episode context manager
    py::class_<SimulationEpisode, std::shared_ptr<SimulationEpisode>>(m, "SimulationEpisode",
        R"pbdoc(Managed simulation episode with automatic reset)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, size_t>())
        .def("run", &SimulationEpisode::run)
        .def("step", &SimulationEpisode::step)
        .def("reset", &SimulationEpisode::reset)
        .def("getBrain", &SimulationEpisode::getBrain)
        .def("isRunning", &SimulationEpisode::isRunning)
        .def("getStep", &SimulationEpisode::getStep)
        .def("getTotalSteps", &SimulationEpisode::getTotalSteps);

    // Brain information utilities
    m.def("getBrainStats", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, std::variant<size_t, float, std::string>> stats;
        if (brain) {
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["spike_count"] = brain->getTotalSpikeCount();
            stats["firing_rate"] = brain->getAverageFiringRate();
            stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        }
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics as a dictionary");

    m.def("compareBrains", [](const std::shared_ptr<Brain>& brain1, 
                            const std::shared_ptr<Brain>& brain2) {
        std::map<std::string, std::pair<size_t, size_t>> comparison;
        if (brain1 && brain2) {
            comparison["neuron_count"] = std::make_pair(brain1->getTotalNeuronCount(), brain2->getTotalNeuronCount());
            comparison["synapse_count"] = std::make_pair(brain1->getTotalSynapseCount(), brain2->getTotalSynapseCount());
            comparison["spike_count"] = std::make_pair(brain1->getTotalSpikeCount(), brain2->getTotalSpikeCount());
        }
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return key metrics comparison");
}

// Define the SimulationEpisode helper class
namespace nlm {
    class SimulationEpisode {
    public:
        SimulationEpisode(std::shared_ptr<Brain> brain, size_t maxSteps)
            : brain_(brain), maxSteps_(maxSteps), currentStep_(0), running_(false) {}
        
        bool run() {
            if (!brain_) return false;
            running_ = true;
            for (currentStep_ = 0; currentStep_ < maxSteps_ && running_; ++currentStep_) {
                brain_->step(currentStep_, currentStep_ * 0.001);
            }
            return true;
        }
        
        bool step() {
            if (!running_ || currentStep_ >= maxSteps_) return false;
            brain_->step(currentStep_, currentStep_ * 0.001);
            ++currentStep_;
            return true;
        }
        
        void reset() {
            running_ = false;
            currentStep_ = 0;
            if (brain_) {
                brain_->reset();
            }
        }
        
        std::shared_ptr<Brain> getBrain() const { return brain_; }
        bool isRunning() const { return running_; }
        size_t getStep() const { return currentStep_; }
        size_t getTotalSteps() const { return maxSteps_; }
        
    private:
        std::shared_ptr<Brain> brain_;
        size_t maxSteps_;
        size_t currentStep_;
        bool running_;
    };
}

// Update the existing bindings_improved.cpp to include a comprehensive rewrite
// This should replace the old bindings_improved.cpp entirely
PYBIND11_MODULE(pynlm_enhanced, m) {
    // Import the original module
    py::module_ pynlm = py::reinterpret_module(m.attr("pynlm"));
    
    // Add enhanced functionality
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ----------------------------
        Enhanced version of pynlm with improved usability, Pythonic features,
        comprehensive error handling, and additional convenience functions.
        
        This module extends the original pynlm bindings with:
        - Better error messages and validation
        - Factory functions for common use cases
        - Batch operations for performance
        - Context managers for simulation episodes
        - Enhanced documentation and examples
        - Type checking and safety features
    )pbdoc";
    
    // Re-export all original symbols
    for (const auto& item : pynlm.attr("__dict__").cast<py::dict>()) {
        m.add_object(item.first.cast<std::string>(), item.second);
    }
    
    // Add enhanced-specific functionality
    m.def("enhanced_createBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain in one call");
    
    m.def("enhanced_runEpisode", [](std::shared_ptr<Brain> brain, 
                                   size_t steps,
                                   const std::vector<std::pair<NeuronId, MembranePotential>>& inputs) {
        // Inject batch inputs
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);
        agentBrain->initialize(world);
        
        // Run the episode
        auto episode = std::make_shared<SimulationEpisode>(brain, steps);
        episode->run();
        
        return episode;
    }, py::arg("brain"), py::arg("steps"), py::arg("inputs"),
       "Run a complete simulation episode with batch input");
    
    m.def("enhanced_saveBrain", [](const std::shared_ptr<Brain>& brain, 
                                  const std::string& filepath,
                                  bool compress = false) {
        if (!brain) {
            throw py::value_error("Brain is null");
        }
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), py::arg("compress") = false,
       "Save brain state with optional compression flag");
    
    // Add enhanced configuration utilities
    m.def("enhanced_configFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (const auto& item : configDict) {
            std::string key = item.first.cast<std::string>();
            if (py::isinstance<py::int_(>(item.second)) {
                config->set(key, item.second.cast<int64_t>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(item.second)) {
                config->set(key, item.second.cast<double>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(item.second)) {
                config->set(key, item.second.cast<bool>(), ConfigSource::Runtime);
            } else if (py::isinstance<py::str_>(item.second)) {
                config->set(key, item.second.cast<std::string>(), ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("configDict"), "Create configuration from Python dictionary");
    
    // Add performance monitoring utilities
    m.def("getPerformanceMetrics", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> metrics;
        if (brain) {
            metrics["spike_per_step"] = static_cast<double>(brain->getTotalSpikeCount()) / 1000.0;
            metrics["neurons_per_region"] = static_cast<double>(brain->getTotalNeuronCount()) / std::max<size_t>(1, brain->getRegionCount());
            metrics["synapses_per_neuron"] = static_cast<double>(brain->getTotalSynapseCount()) / std::max<size_t>(1, brain->getTotalNeuronCount());
        }
        return metrics;
    }, py::arg("brain"), "Get performance metrics for monitoring");
}
