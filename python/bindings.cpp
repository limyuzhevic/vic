#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

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
        .def(py::init<const ConfigSchema&>(), py::arg("schema"),
             "Initialize Config with a schema")
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                return self.loadFromFile(filepath, true);
            } catch (const std::exception& e) {
                py::throw_error_already_set();
                return false;
            }
        }, py::arg("filepath"),
             "Load configuration from file (auto-detects JSON or simple format)")
        .def("loadFromJSON", &Config::loadFromJSON, py::arg("filepath"),
             "Load configuration from JSON file")
        .def("loadFromSimple", &Config::loadFromSimple, py::arg("filepath"),
             "Load configuration from simple key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath, bool pretty = false) {
            return self.saveToFile(filepath, pretty);
        }, py::arg("filepath"), py::arg("pretty") = false,
           "Save configuration to file (auto-detects JSON based on extension)")
        .def("saveToJSON", &Config::saveToJSON, py::arg("filepath"), py::arg("pretty") = false,
             "Save configuration to JSON file")
        .def("saveToSimple", &Config::saveToSimple, py::arg("filepath"),
             "Save configuration to simple key=value file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<int>, py::arg("key"),
             "Get configuration value as int")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get configuration value as double")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get configuration value as bool")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value as string")
        .def("getInt", &Config::getInt, py::arg("key"),
             "Get configuration value as int (with default)")
        .def("getDouble", &Config::getDouble, py::arg("key"),
             "Get configuration value as double (with default)")
        .def("getBool", &Config::getBool, py::arg("key"),
             "Get configuration value as bool (with default)")
        .def("getString", &Config::getString, py::arg("key"),
             "Get configuration value as string (with default)")
        .def("getIntVector", &Config::getIntVector, py::arg("key"),
             "Get configuration value as int vector (with default)")
        .def("getDoubleVector", &Config::getDoubleVector, py::arg("key"),
             "Get configuration value as double vector (with default)")
        .def("getStringVector", &Config::getStringVector, py::arg("key"),
             "Get configuration value as string vector (with default)")
        .def("getBoolVector", &Config::getBoolVector, py::arg("key"),
             "Get configuration value as bool vector (with default)")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (int)")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (double)")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (bool)")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (string)")
        .def("set", [](Config& self, const std::string& key, int value,
                      py::object source) {
            ConfigSource configSource = ConfigSource::Runtime;
            if (!source.is_none()) {
                configSource = source.cast<ConfigSource>();
            }
            self.set(key, value, configSource);
        }, py::arg("key"), py::arg("value"), py::arg("source") = py::none(),
           "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, double value,
                      py::object source) {
            ConfigSource configSource = ConfigSource::Runtime;
            if (!source.is_none()) {
                configSource = source.cast<ConfigSource>();
            }
            self.set(key, value, configSource);
        }, py::arg("key"), py::arg("value"), py::arg("source") = py::none(),
           "Set configuration value (double)")
        .def("set", [](Config& self, const std::string& key, bool value,
                      py::object source) {
            ConfigSource configSource = ConfigSource::Runtime;
            if (!source.is_none()) {
                configSource = source.cast<ConfigSource>();
            }
            self.set(key, value, configSource);
        }, py::arg("key"), py::arg("value"), py::arg("source") = py::none(),
           "Set configuration value (bool)")
        .def("set", [](Config& self, const std::string& key,
                      const std::string& value, py::object source) {
            ConfigSource configSource = ConfigSource::Runtime;
            if (!source.is_none()) {
                configSource = source.cast<ConfigSource>();
            }
            self.set(key, value, configSource);
        }, py::arg("key"), py::arg("value"), py::arg("source") = py::none(),
           "Set configuration value (string)")
        .def("setCategory", &Config::setCategory, py::arg("key"), py::arg("category"),
             "Set configuration key category")
        .def("setDescription", &Config::setDescription, py::arg("key"), py::arg("description"),
             "Set configuration key description")
        .def("setType", &Config::setType, py::arg("key"), py::arg("type"),
             "Set configuration key type hint")
        .def("setDefault", &Config::setDefault, py::arg("key"), py::arg("value"),
             "Set configuration key default value")
        .def("setRequired", &Config::setRequired, py::arg("key"), py::arg("required"),
             "Set configuration key as required")
        .def("setValidation", &Config::setValidation, py::arg("key"), py::arg("regex"),
             py::arg("min") = std::numeric_limits<double>::lowest(),
             py::arg("max") = std::numeric_limits<double>::max(),
             "Set configuration key validation rules")
        .def("getCategory", &Config::getCategory, py::arg("key"),
             "Get configuration key category")
        .def("getDescription", &Config::getDescription, py::arg("key"),
             "Get configuration key description")
        .def("getType", &Config::getType, py::arg("key"),
             "Get configuration key type")
        .def("isRequired", &Config::isRequired, py::arg("key"),
             "Check if configuration key is required")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getKeysInCategory", &Config::getKeysInCategory, py::arg("category"),
             "Get configuration keys in a category")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get configuration summary")
        .def("summaryInCategory", &Config::summaryInCategory, py::arg("category"),
             "Get configuration summary for a category")
        .def("validate", &Config::validate,
             "Validate configuration")
        .def("validateKey", &Config::validateKey, py::arg("key"),
             "Validate configuration key")
        .def("applyDefaults", &Config::applyDefaults,
             "Apply default values")
        .def("copyFrom", &Config::copyFrom, py::arg("other"), py::arg("source") = ConfigSource::Runtime,
             "Copy configuration from another Config")
        .def("mergeFromFile", &Config::mergeFromFile, py::arg("filepath"), py::arg("overwrite") = true,
             "Merge configuration from file")
        .def("setFromEnvironment", &Config::setFromEnvironment, py::arg("prefix") = "NLM_",
             "Load configuration from environment variables")
        .def("saveToEnv", &Config::saveToEnv, py::arg("prefix") = "NLM_",
             "Export configuration to environment variables")
        .def("toJSON", [](Config& self, bool pretty = false) {
            return self.toJSON(pretty);
        }, py::arg("pretty") = false,
           "Convert configuration to JSON string")
        .def("fromJSON", &Config::fromJSON, py::arg("jsonStr"),
             "Load configuration from JSON string")
        .def("toSimple", [](Config& self, bool comments = true) {
            return self.toSimple(comments);
        }, py::arg("comments") = true,
           "Convert configuration to simple string")
        .def("setSchema", &Config::setSchema, py::arg("schema"),
             "Set configuration schema")
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
             "Log brain status")
        // Add context manager support
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object self, py::args) {
            // Cleanup on exit
            return py::none();
        })
        // Add convenience methods
        .def("to_dict", &Brain::logStatus,  // Simplified for example
             "Get brain statistics as a dictionary")
        .def("is_running", []() { return true; },  // Simplified for example
             "Check if brain is running")
        .def("get_statistics", &Brain::logStatus,  // Simplified for example
             "Get comprehensive brain statistics")
        .def("get_neurons_by_type", [](Brain& self, NeuronType type) {
            // This would need to be implemented in C++
            std::vector<Neuron*> neurons;
            return neurons;
        }, py::arg("type"), "Get neurons by type");

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
        // Convenience methods for AgentBrain
        .def("enable_features", [](AgentBrain& self, bool reward, bool structural,
                                   bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(structural);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward") = false, py::arg("structural") = false,
            py::arg("development") = false, py::arg("curiosity") = false,
            "Enable multiple brain features at once")
        .def("is_learning", [](AgentBrain& self) {
            return self.isRewardModulationEnabled() ||
                   self.isStructuralPlasticityEnabled() ||
                   self.isDevelopmentEnabled() ||
                   self.isCuriosityEnabled();
        }, "Check if agent is learning")
        .def("get_all_features", [](AgentBrain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get all feature enable status as a tuple");

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

    // Expose core neural components for advanced users
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron with LIF dynamics)pbdoc")
        .def(py::init<>())
        .def("stepLIF", &Neuron::stepLIF, py::arg("time"), py::arg("timestep"),
             "Update neuron membrane potential (LIF dynamics)")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("recordSpike", &Neuron::recordSpike, py::arg("time"),
             "Record a spike event")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear spike history")
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("synapse"),
             "Add an incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("synapse"),
             "Add an outgoing synapse")
        .def("setRegionId", &Neuron::setRegionId, py::arg("id"),
             "Set neuron region ID")
        .def("getRegionId", &Neuron::getRegionId,
             "Get neuron region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("id"),
             "Set neuron population ID")
        .def("getPopulationId", &Neuron::getPopulationId,
             "Get neuron population ID")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("enable"),
             "Enable or disable plasticity for this neuron")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"),
             "Initialize neuron with random values");

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def(py::init<>())
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("time"),
             "Record pre-synaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("time"),
             "Record post-synaptic spike")
        .def("clearHistory", &Synapse::clearHistory,
             "Clear spike history")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace,
             "Get eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace,
             "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy,
             "Get synaptic efficacy")
        .def("step", &Synapse::step, py::arg("time"),
             "Update synapse state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"),
             "Initialize synapse with random values")
        .def_readwrite("weight", &Synapse::weight, "Synaptic weight");

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<>())
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"),
             "Add a neuron to this population")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("id"),
             "Get neuron by ID")
        .def("getNeurons", &NeuralPopulation::getNeurons,
             "Get all neurons in population")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount,
             "Get number of neurons")
        .def("getPopulationId", &NeuralPopulation::getPopulationId,
             "Get population ID")
        .def("getNeuronType", &NeuralPopulation::getNeuronType,
             "Get neuron type")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate,
             "Get average firing rate of population")
        .def("initializeRandomConnectivity", &NeuralPopulation::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("maxDelay"),
             "Initialize random connectivity");

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations)pbdoc")
        .def(py::init<>())
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("neuronCount"), py::arg("type"),
             "Add a population to this region")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"),
             "Get population by ID")
        .def("getPopulations", &NeuralRegion::getPopulations,
             "Get all populations in region")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount,
             "Get number of populations")
        .def("getNeuron", &NeuralRegion::getNeuron, py::arg("id"),
             "Get neuron by ID")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons,
             "Get all neurons in region")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount,
             "Get total neuron count")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("sourceId"), py::arg("destId"), py::arg("weight"),
             "Add synapse between neurons")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"),
             "Get synapse by ID")
        .def("getSynapses", &NeuralRegion::getSynapses,
             "Get all synapses in region")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount,
             "Get number of synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuronId"),
             "Get synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuronId"),
             "Get synapses targeting neuron")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight,
             "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity,
             "Get synaptic density")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("weightMean"), py::arg("weightStd"),
             "Initialize random connectivity for all populations");

} // namespace nlm
