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
        })
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        }, "Get number of configuration entries")
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, "Check if key exists in configuration")
        .def("__getitem__", [](Config& self, const std::string& key) {
            // Try to get as double first
            auto values = self.getValues(key);
            if (!values.empty() && std::holds_alternative<double>(values[0])) {
                return py::cast(std::get<double>(values[0]));
            }
            // Try to get as string
            if (!values.empty() && std::holds_alternative<std::string>(values[0])) {
                return py::cast(std::get<std::string>(values[0]));
            }
            // Try to get as int64_t
            if (!values.empty() && std::holds_alternative<int64_t>(values[0])) {
                return py::cast(std::get<int64_t>(values[0]));
            }
            // Default to empty string if not found
            return py::str("");
        }, "Get configuration value by key")
        .def("__setitem__", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, "Set configuration value as double")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, "Set configuration value as string")
        .def("__iter__", [](Config& self) {
            auto keys = self.getKeys();
            return py::make_iterator(keys.begin(), keys.end());
        }, "Iterate over configuration keys");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")

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
        .def("__enter__", [](std::shared_ptr<Brain>& self) {
            self->initialize();
            return self;
        }, "Enter the context manager and initialize the brain")
        .def("__exit__", [](std::shared_ptr<Brain>& self, py::object, py::object, py::object) {
            self->reset();
        }, "Exit the context manager and reset the brain");

    m.def("createBrainContext",
        [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        },
        py::arg("config"), "Create and automatically initialize a brain (for use with context manager)");

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

    // Register custom exception classes for better error handling
    py::register_exception<nlm::ConfigException>(m, "ConfigError");
    py::register_exception<nlm::BrainException>(m, "BrainError");
    py::register_exception<nlm::WorldException>(m, "WorldError");
    py::register_exception<nlm::AgentException>(m, "AgentError");
    py::register_exception<nlm::MemoryException>(m, "MemoryError");
    py::register_exception<nlm::PlasticityException>(m, "PlasticityError");
    py::register_exception<nlm::NeuromodulationException>(m, "NeuromodulationError");
    py::register_exception<nlm::CognitionException>(m, "CognitionError");
    py::register_exception<nlm::DevelopmentException>(m, "DevelopmentError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    m.def("check_version", 
        [](const std::string& version) -> bool {
            return nlm::checkVersion(version);
        },
        py::arg("version"), "Check if current version meets minimum required version");

    m.def("get_version", 
        []() -> std::string {
            return "0.1.0";
        }, "Get current pynlm version");

    m.def("list_classes", 
        []() {
            py::dict classes;
            classes["Config"] = m.attr("Config");
            classes["Brain"] = m.attr("Brain");
            classes["AgentBrain"] = m.attr("AgentBrain");
            classes["SimpleWorld"] = m.attr("SimpleWorld");
            classes["SensoryInput"] = m.attr("SensoryInput");
            classes["Action"] = m.attr("Action");
            classes["Vision"] = m.attr("Vision");
            classes["Audio"] = m.attr("Audio");
            classes["InternalSignals"] = m.attr("InternalSignals");
            return classes;
        }, "List all available classes");

    m.def("get_documentation", 
        []() -> std::string {
            return """NLM Python Bindings Documentation

The NLM (Neural Learning Machine) Python bindings provide access to the
NLM C++ neural simulation framework with a Pythonic interface.

Available Classes:
- Config: Brain configuration management
- Brain: Central neural simulation engine
- AgentBrain: Interface between brain and world
- SimpleWorld: 2D physics simulation environment
- Vision: Visual sensory input
- Audio: Audio sensory input
- InternalSignals: Internal brain state signals
- Action: Motor actions

Each class has methods for:
- Creation and initialization
- State management (step, reset, save/load)
- Sensory processing (vision, audio, internal signals)
- Action production and application
- Statistics and monitoring

See the NLM documentation for more details on neural simulation concepts.
""";
        }, "Get documentation string");

    m.def("simulate_with_stats",
        [](std::shared_ptr<Brain> brain, nlm::SimulationStep num_steps, bool initialize) {
            if (initialize) {
                brain->initialize();
            }
            
            nlm::SimulationStep max_step = static_cast<nlm::SimulationStep>(num_steps);
            nlm::SimulationStep active_steps = 0;
            nlm::SpikeCount total_spikes = 0;
            
            for (nlm::SimulationStep step = 0; step < max_step; ++step) {
                brain->step(step);
                active_steps++;
                total_spikes += brain->getTotalSpikeCount();
            }
            
            py::dict stats;
            stats["steps"] = num_steps;
            stats["active_steps"] = active_steps;
            stats["final_firing"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain->getAverageFiringRate();
            stats["ei_ratio"] = brain->getExcitationInhibitionRatio();
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["region_count"] = brain->getRegionCount();
            
            return stats;
        },
        py::arg("brain"), py::arg("num_steps"), py::arg("initialize") = true,
        "Run simulation with detailed statistics");

    m.def("run_complete_agent_simulation",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, nlm::SimulationStep num_steps,
           bool enable_all_subsystems) {
            brain->initialize();
            agent->initialize(*world);
            
            if (enable_all_subsystems) {
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
                agent->enableDevelopment(true);
                agent->enableStructuralPlasticity(true);
            }
            
            for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            py::dict stats;
            stats["steps"] = num_steps;
            stats["final_firing"] = brain->getFiringNeuronCount();
            stats["curiosity"] = agent->getCuriosityLevel();
            stats["novelty"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["neuromodulation"] = agent->getNeuromodulationLevel();
            stats["dev_stage"] = brain->getDevelopmentalStage();
            stats["reward"] = agent->getBrain()->getConfig().getDouble("neuromod.dopamine.scale", 1.0);
            
            return stats;
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
                py::arg("num_steps"), py::arg("enable_all_subsystems") = true,
        "Run complete agent simulation with all subsystems enabled");

    m.def("benchmark_brain",
        [](std::shared_ptr<Brain> brain, nlm::SimulationStep num_steps) {
            nlm::Timestamp start_time = std::chrono::high_resolution_clock::now();
            brain->initialize();
            
            for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
                brain->step(step);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            double steps_per_second = (num_steps * 1000.0) / duration.count();
            
            py::dict results;
            results["steps"] = num_steps;
            results["duration_ms"] = duration.count();
            results["steps_per_second"] = steps_per_second;
            results["avg_firing_rate"] = brain->getAverageFiringRate();
            results["neuron_count"] = brain->getTotalNeuronCount();
            
            return results;
        },
        py::arg("brain"), py::arg("num_steps"),
        "Benchmark brain performance");

    m.def("create_neural_network",
        [](nlm::SimulationStep num_neurons, nlm::SimulationStep num_regions,
           double connection_probability, double initial_weight_mean) {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(num_neurons));
            config->set("brain.region_count", static_cast<int64_t>(num_regions));
            config->set("brain.connection_probability", connection_probability);
            config->set("brain.initial_weight_mean", initial_weight_mean);
            
            return std::make_shared<Brain>(config);
        },
        py::arg("num_neurons"), py::arg("num_regions"),
                py::arg("connection_probability") = 0.1,
                py::arg("initial_weight_mean") = 0.5,
        "Create a neural network with specified parameters");

    m.def("export_neural_network",
        [](std::shared_ptr<Brain> brain, const std::string& filename) {
            brain->save(filename);
        },
        py::arg("brain"), py::arg("filename"),
        "Export neural network to file");

    m.def("import_neural_network",
        [](const std::string& filename) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->load(filename);
            return brain;
        },
        py::arg("filename"),
        "Import neural network from file");
 
        [](std::shared_ptr<Brain> brain, nlm::SimulationStep num_steps, bool initialize) {
            if (initialize) {
                brain->initialize();
            }
            
            nlm::SimulationStep max_step = static_cast<nlm::SimulationStep>(num_steps);
            for (nlm::SimulationStep step = 0; step < max_step; ++step) {
                brain->step(step);
            }
            
            py::dict stats;
            stats["steps"] = num_steps;
            stats["final_firing"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain->getAverageFiringRate();
            stats["ei_ratio"] = brain->getExcitationInhibitionRatio();
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            
            return stats;
        },
        py::arg("brain"), py::arg("num_steps"), py::arg("initialize") = true,
        "Run a simple brain simulation and return statistics");
    
    m.def("run_agent_simulation",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, nlm::SimulationStep num_steps,
           bool enable_learning) {
            brain->initialize();
            agent->initialize(*world);
            
            if (enable_learning) {
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
                agent->enableDevelopment(true);
            }
            
            for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            py::dict stats;
            stats["steps"] = num_steps;
            stats["final_firing"] = brain->getFiringNeuronCount();
            stats["curiosity"] = agent->getCuriosityLevel();
            stats["novelty"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["dev_stage"] = brain->getDevelopmentalStage();
            
            return stats;
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"), 
                py::arg("num_steps"), py::arg("enable_learning") = true,
        "Run a complete agent simulation with brain-world interaction");
    
    m.def("create_complete_brain",
        [](py::object config_dict) {
            std::shared_ptr<Config> config = std::make_shared<Config>();
            
            if (!config_dict.is_none()) {
                py::dict config_py = config_dict.cast<py::dict>();
                for (auto it : config_py) {
                    std::string key = py::str(it.first);
                    std::string value_str = py::str(it.second);
                    
                    // Try to parse as number first, then as string
                    try {
                        double value_num = std::stod(value_str);
                        config->set(key, value_num);
                    } catch (...) {
                        config->set(key, value_str);
                    }
                }
            }
            
            std::shared_ptr<Brain> brain = std::make_shared<Brain>(config);
            std::shared_ptr<SimpleWorld> world = std::make_shared<SimpleWorld>();
            std::shared_ptr<AgentBrain> agent = std::make_shared<AgentBrain>(brain);
            
            py::tuple result;
            result = py::make_tuple(brain, agent, world);
            return result;
        },
        py::arg("config_dict") = py::none(),
        "Create a complete brain-world-agent setup with optional configuration");
    
    m.def("save_simulation_results",
        [](const std::string& filename, py::dict results) {
            import_pybind11_json();
            auto result_dict = results.cast<py::dict>();
            std::ofstream file(filename);
            nlohmann::json j = result_dict;
            file << j.dump(2);
        },
        py::arg("filename"), py::arg("results"),
        "Save simulation results to a JSON file");
    
    m.def("load_simulation_results",
        [](const std::string& filename) {
            import_pybind11_json();
            std::ifstream file(filename);
            nlohmann::json j = nlohmann::json::parse(file);
            return j.get_pybind11_dict();
        },
        py::arg("filename"),
        "Load simulation results from a JSON file");
}

// Helper function to import pybind11_json
void import_pybind11_json() {
    py::module_ json_module = py::module_::import("json");
}

} // namespace nlm
