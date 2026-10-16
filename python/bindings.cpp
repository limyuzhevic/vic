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
        // Pythonic convenience methods
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get configuration value as string (convenience method)")
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer value with default")
        .def("get_float", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float value with default")
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false, "Get boolean value with default")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string value")
        .def("merge_from", [](Config& self, const Config& other) {
            // This is a simplified merge - for a complete implementation, you'd need access to internal state
            for (const auto& key : other.getKeys()) {
                auto value = other.get<std::string>(key);
                if (value.has_value()) {
                    self.set(key, *value, nlm::ConfigSource::Runtime);
                }
            }
        }, py::arg("other"), "Merge configuration from another Config object")
        .def("validate", [](const Config& self, const std::vector<std::string>& requiredKeys) {
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    return false;
                }
            }
            return true;
        }, py::arg("requiredKeys"), "Validate that all required keys exist")
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
        .def("clone", &Action::clone)
        // Pythonic convenience methods
        .def("to_dict", [](const Action& self) {
            return py::dict(
                "type", self.getType(),
                "parameters", self.getParameters(),
                "name", self.getName()
            );
        }, "Convert action to Python dictionary")
        .def("set_param", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("add_param", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add parameter to action")
        .def("remove_param", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params.erase(params.begin() + index);
                self.setParameters(params);
            }
        }, py::arg("index"), "Remove parameter at index")
        .def("__repr__", [](const Action& self) {
            std::stringstream ss;
            ss << "<Action: " << self.getName() << " type=" << static_cast<int>(self.getType()) << ">";
            return ss.str();
        });

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic convenience methods
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision", self.getVision(),
                "touch", self.getTouch(),
                "internal", self.getInternal(),
                "proprioception", self.getProprioception(),
                "audio", self.getAudio(),
                "timestamp", self.getTimestamp()
            );
        }, "Convert percept to Python dictionary")
        .def("__repr__", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "<SensoryPercept at " << self.getTimestamp() << ">";
            return ss.str();
        });

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

    // Module-level convenience functions
    m.def("create_demo_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set some default demo values
        config->set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
        config->set("brain.simulation_steps", 1000, nlm::ConfigSource::Runtime);
        config->set("world.width", 100.0f, nlm::ConfigSource::Runtime);
        config->set("world.height", 100.0f, nlm::ConfigSource::Runtime);
        config->set("world.vision_width", 10, nlm::ConfigSource::Runtime);
        config->set("world.vision_height", 10, nlm::ConfigSource::Runtime);
        config->set("neuromodulation.reward_scale", 1.0f, nlm::ConfigSource::Runtime);
        config->set("neuromodulation.curiosity_scale", 0.1f, nlm::ConfigSource::Runtime);
        return config;
    }, "Create a demo configuration with default values for testing");

    m.def("create_simple_demo_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        // Add some default regions
        brain->addRegion("sensory");
        brain->addRegion("motor");
        brain->addRegion("association");
        return brain;
    }, py::arg("config"), "Create a demo brain with default configuration");

    m.def("create_simple_demo_world", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 10, 10);
        world->setAgentStart(50.0f, 50.0f);
        world->setRandomSeed(42);
        return world;
    }, "Create a demo world with default settings");

    // Advanced simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                             size_t steps, float timestep = 0.01f) {
        for (size_t i = 0; i < steps; ++i) {
            if (world) {
                auto percept = world->getSensoryPercept();
                brain->receiveSensoryInput(percept);
            }
            brain->step(i, static_cast<Timestamp>(i * timestep * 1000.0f));
        }
        return py::dict(
            "total_steps", steps,
            "final_step", steps - 1,
            "status", "completed"
        );
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.01f,
       "Run a complete simulation episode");

    m.def("save_brain_state", [](const std::string& filepath, std::shared_ptr<Brain> brain) {
        return brain->save(filepath);
    }, py::arg("filepath"), py::arg("brain"), "Save brain state to file");

    m.def("load_brain_state", [](const std::string& filepath, std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        }
        throw std::runtime_error("Failed to load brain state from " + filepath);
    }, py::arg("filepath"), py::arg("config"), "Load brain state from file");

    m.def("create_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain connected to a world");

    // Error and validation utilities
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        
        py::list errors;
        // Check for required keys
        const std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.simulation_steps"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                errors.append("Missing required key: " + key);
            }
        }
        
        // Check for valid ranges
        if (config->has("brain.neuron_count")) {
            auto neuronCount = config->getOr<int>("brain.neuron_count", 0);
            if (neuronCount < 10) {
                errors.append("Neuron count should be at least 10");
            } else if (neuronCount > 100000) {
                errors.append("Neuron count should not exceed 100,000");
            }
        }
        
        if (config->has("brain.simulation_steps")) {
            auto steps = config->getOr<size_t>("brain.simulation_steps", 0);
            if (steps == 0) {
                errors.append("Simulation steps should be positive");
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    // Performance and debugging utilities
    m.def("profile_brain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return py::dict(
            "total_steps", steps,
            "elapsed_time_ms", duration.count(),
            "steps_per_second", static_cast<double>(steps) / (duration.count() / 1000.0),
            "avg_time_per_step_ms", static_cast<double>(duration.count()) / steps
        );
    }, py::arg("brain"), py::arg("steps"), "Profile brain performance over specified steps");

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add some useful constants
    m.attr("DEFAULT_SIMULATION_DT") = py::cast(0.01f);
    m.attr("DEFAULT_WORLD_SIZE") = py::cast(100.0f);
    m.attr("DEFAULT_VISION_RANGE") = py::cast(10);

    m.attr("DEVELOPMENTAL_STAGE_INITIAL") = py::cast(nlm::DevelopmentalStage::Initial);
    m.attr("DEVELOPMENTAL_STAGE_CRITICAL_PERIOD") = py::cast(nlm::DevelopmentalStage::CriticalPeriod);
    m.attr("DEVELOPMENTAL_STAGE_MATURATION") = py::cast(nlm::DevelopmentalStage::Maturation);
    m.attr("DEVELOPMENTAL_STAGE_ADULT") = py::cast(nlm::DevelopmentalStage::Adult);
    m.attr("DEVELOPMENTAL_STAGE_AGING") = py::cast(nlm::DevelopmentalStage::Aging);

    m.attr("NEURON_TYPE_EXCITATORY") = py::cast(nlm::NeuronType::Excitatory);
    m.attr("NEURON_TYPE_INHIBITORY") = py::cast(nlm::NeuronType::Inhibitory);
    m.attr("NEURON_TYPE_MODULATORY") = py::cast(nlm::NeuronType::Modulatory);
    m.attr("NEURON_TYPE_SENSORY") = py::cast(nlm::NeuronType::Sensory);
    m.attr("NEURON_TYPE_MOTOR") = py::cast(nlm::NeuronType::Motor);
    m.attr("NEURON_TYPE_INTERNAL") = py::cast(nlm::NeuronType::Internal);

    m.attr("ACTION_TYPE_MOVE_FORWARD") = py::cast(nlm::ActionType::MoveForward);
    m.attr("ACTION_TYPE_MOVE_BACKWARD") = py::cast(nlm::ActionType::MoveBackward);
    m.attr("ACTION_TYPE_TURN_LEFT") = py::cast(nlm::ActionType::TurnLeft);
    m.attr("ACTION_TYPE_TURN_RIGHT") = py::cast(nlm::ActionType::TurnRight);
    m.attr("ACTION_TYPE_LOOK") = py::cast(nlm::ActionType::Look);
    m.attr("ACTION_TYPE_INTERACT") = py::cast(nlm::ActionType::Interact);
    m.attr("ACTION_TYPE_REST") = py::cast(nlm::ActionType::Rest);
    m.attr("ACTION_TYPE_WAIT") = py::cast(nlm::ActionType::Wait);

    m.attr("WORLD_OBJECT_EMPTY") = py::cast(nlm::WorldObjectType::Empty);
    m.attr("WORLD_OBJECT_RESOURCE") = py::cast(nlm::WorldObjectType::Resource);
    m.attr("WORLD_OBJECT_HAZARD") = py::cast(nlm::WorldObjectType::Hazard);
    m.attr("WORLD_OBJECT_WALL") = py::cast(nlm::WorldObjectType::Wall);
    m.attr("WORLD_OBJECT_MARKER") = py::cast(nlm::WorldObjectType::Marker);
}

} // namespace nlm

