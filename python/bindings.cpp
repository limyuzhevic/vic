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

    // Enhanced Python bindings for Config with Pythonic API
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
        // Pythonic convenience methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set a float configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set a double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        // Pythonic get methods
        .def("get", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val.has_value()) return val.value();
            throw std::runtime_error("Key not found: " + key);
        }, py::arg("key"),
           "Get a string configuration value")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a string configuration value with default")
        .def("get", [](const Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get an integer configuration value with default")
        .def("get", [](const Config& self, const std::string& key, float defaultValue) {
            auto val = self.get<float>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a float configuration value with default")
        .def("get", [](const Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a boolean configuration value with default")
        // Batch operations
        .def("update", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = py::cast<std::string>(item.first);
                auto value = py::cast<py::object>(item.second);
                if (value.is(py::int_())) {
                    self.set(key, py::cast<int>(value), ConfigSource::User);
                } else if (value.is(py::float_())) {
                    self.set(key, py::cast<float>(value), ConfigSource::User);
                } else if (value.is(py::bool_())) {
                    self.set(key, py::cast<bool>(value), ConfigSource::User);
                } else if (value.is(py::str())) {
                    self.set(key, py::cast<std::string>(value), ConfigSource::User);
                }
            }
        }, py::arg("updates"),
           "Update multiple configuration values from a dictionary")
        .def("toDict", [](const Config& self) {
            py::dict result;
            for (const auto& entry : self.getKeys()) {
                auto val = self.get<std::string>(entry);
                if (val.has_value()) {
                    result[entry] = val.value();
                } else {
                    auto intVal = self.get<int>(entry);
                    if (intVal.has_value()) {
                        result[entry] = intVal.value();
                    } else {
                        auto floatVal = self.get<float>(entry);
                        if (floatVal.has_value()) {
                            result[entry] = floatVal.value();
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary"),
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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // AgentBrain convenience methods
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Python convenience functions for easier scripting
    m.def("createDefaultBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        return std::make_shared<Brain>(config);
    }, "Create a default brain with typical settings");

    m.def("createCustomBrain", [](size_t neuronCount, size_t regionCount, float connectionProb, int64_t seed) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProb);
        config->set("random_seed", seed);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 500, py::arg("region_count") = 1,
       py::arg("connection_prob") = 0.1f, py::arg("seed") = 42,
       "Create a brain with custom parameters");

    // Easy simulation setup
    m.def("setupSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        if (world) {
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(*world);
            return agentBrain;
        }
        return std::shared_ptr<AgentBrain>(nullptr);
    }, py::arg("brain"), py::arg("world") = nullptr,
       "Initialize a brain and optionally create an agent brain interface");

    // Common operation shortcuts
    m.def("runStep", [](std::shared_ptr<Brain> brain, size_t steps = 1) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps") = 1,
       "Run multiple simulation steps and return total spike count");

    m.def("injectRandomSensoryInput", [](std::shared_ptr<Brain> brain, float intensity = 1.0f) {
        if (auto* region = brain->getRegion(RegionId(1)); region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory) {
                    neuron->injectCurrent(intensity * 10.0f);
                }
            }
        }
    }, py::arg("brain"), py::arg("intensity") = 1.0f,
       "Inject random sensory input to all sensory neurons");

    m.def("getNetworkStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        return stats;
    }, "Get comprehensive network statistics");

    // Development and exploration helpers
    m.def("exploreNetworkStructure", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> regions;
        auto ids = brain->getRegionIds();
        for (const auto& id : ids) {
            regions.push_back("Region " + std::to_string(id.value()));
        }
        return regions;
    }, "Get list of region names in the brain");

    // Easy configuration presets
    m.def("getConfigurationPresets", []() {
        std::vector<std::string> presets;
        presets.push_back("minimal");
        presets.push_back("balanced");
        presets.push_back("high_plasticity");
        presets.push_back("low_activity");
        return presets;
    }, "Get available configuration preset names");

    m.def("applyConfigurationPreset", [](std::shared_ptr<Config> config, const std::string& preset) {
        config->clear();
        if (preset == "minimal") {
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("connection_probability", 0.05f);
        } else if (preset == "balanced") {
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("connection_probability", 0.1f);
            config->set("stdp_ltp_weight", 0.01f);
        } else if (preset == "high_plasticity") {
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("connection_probability", 0.15f);
            config->set("stdp_ltp_weight", 0.05f);
            config->set("synaptogenesis_rate", 0.001f);
        } else if (preset == "low_activity") {
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("connection_probability", 0.05f);
            config->set("stdp_ltp_weight", 0.005f);
        }
    }, "Apply a predefined configuration preset");

    // Advanced analysis and exploration tools
    m.def("analyzeLearningTrajectory", [](std::shared_ptr<Brain> brain, const std::vector<std::pair<size_t, Timestamp>>& timeline) {
        std::map<size_t, std::map<std::string, double>> trajectory;
        
        for (const auto& [step, time] : timeline) {
            brain->step(step, time);
            std::map<std::string, double> snapshot;
            snapshot["spike_count"] = brain->getTotalSpikeCount();
            snapshot["firing_rate"] = brain->getAverageFiringRate();
            snapshot["active_neurons"] = brain->getActiveNeuronCount();
            snapshot["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            
            trajectory[step] = snapshot;
        }
        
        return trajectory;
    }, py::arg("brain"), py::arg("timeline"),
       "Analyze how the brain evolves over time");

    m.def("findActiveNeurons", [](std::shared_ptr<Brain> brain, size_t step, Timestamp time) {
        brain->step(step, time);
        std::vector<NeuronId> activeNeurons;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                if (neuron->isFiring()) {
                    activeNeurons.push_back(neuron->getId());
                }
            }
        }
        
        return activeNeurons;
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
       "Get IDs of all currently firing neurons");

    // Brain state exploration
    m.def("getSensoryNeurons", [](std::shared_ptr<Brain> brain) {
        std::vector<NeuronId> sensoryNeurons;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory) {
                    sensoryNeurons.push_back(neuron->getId());
                }
            }
        }
        
        return sensoryNeurons;
    }, "Get IDs of all sensory neurons");

    m.def("getMotorNeurons", [](std::shared_ptr<Brain> brain) {
        std::vector<NeuronId> motorNeurons;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Motor) {
                    motorNeurons.push_back(neuron->getId());
                }
            }
        }
        
        return motorNeurons;
    }, "Get IDs of all motor neurons");

    // Educational tools for understanding the brain
    m.def("createEducationalDemoBrain", []() {
        auto config = std::make_shared<Config>();
        // Smaller brain for educational demos
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.2f);
        config->set("random_seed", static_cast<int64_t>(12345));
        
        // Enable all learning systems for educational demos
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.025f);
        config->set("synaptogenesis_rate", 0.0002f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create some test activity
        if (auto* region = brain->getRegion(RegionId(1)); region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory) {
                    neuron->injectCurrent(20.0f);
                }
            }
        }
        
        return brain;
    }, "Create a smaller brain with learning enabled for educational demos");

    m.def("simulateLearningExperiment", [](std::shared_ptr<Brain> brain, size_t initialSteps, size_t learningSteps, float stimulusIntensity) {
        // Record initial state
        size_t initialSpikes = brain->getTotalSpikeCount();
        auto initialWeights = std::vector<float>();
        if (auto* region = brain->getRegion(RegionId(1)); region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Initial period
        for (size_t i = 0; i < initialSteps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Learning phase with stimulus
        for (size_t i = initialSteps; i < initialSteps + learningSteps; ++i) {
            // Inject stimulus to sensory neurons to trigger learning
            if (auto* region = brain->getRegion(RegionId(1)); region) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory) {
                        neuron->injectCurrent(stimulusIntensity);
                    }
                }
            }
            
            brain->step(i, i * 0.001);
        }
        
        // Record final state
        size_t finalSpikes = brain->getTotalSpikeCount();
        auto finalWeights = std::vector<float>();
        if (auto* region = brain->getRegion(RegionId(1)); region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate learning metrics
        std::map<std::string, double> metrics;
        metrics["initial_spikes"] = initialSpikes;
        metrics["final_spikes"] = finalSpikes;
        metrics["spike_change"] = finalSpikes - initialSpikes;
        
        if (!initialWeights.empty() && !finalWeights.empty()) {
            double initialMeanWeight = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0) / initialWeights.size();
            double finalMeanWeight = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0) / finalWeights.size();
            metrics["initial_mean_weight"] = initialMeanWeight;
            metrics["final_mean_weight"] = finalMeanWeight;
            metrics["weight_change"] = finalMeanWeight - initialMeanWeight;
        }
        
        return metrics;
    }, "Simulate a learning experiment and return metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
