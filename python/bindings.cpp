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

    // Note: The ActionType enum is used internally by the brain's cognition systems.
    // For motor output, use MotorCommand which is the actual interface to the physical
    // or virtual agent's movement systems. MotorCommand provides a cleaner, more
    // restricted set of commands that match the available actions in the motor system.

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
    // Load from Python list of strings (simplified command-line interface)
    // Handles --key=value and -key value formats
    .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    self.set(key, value, ConfigSource::CommandLine);
                }
            } else if (arg.size() > 1 && arg[0] == '-') {
                // -key value format (value should be next argument if provided here)
                // Note: This simplified interface expects --key=value or -key with separate value
                // For multi-value arguments, users should provide them as "key=val1,val2"
                self.set(arg.substr(1), "PLACEHOLDER", ConfigSource::CommandLine);
            }
        }
        return true;
    }, py::arg("args"),
       "Load configuration from Python list of strings. Supports --key=value and -key formats")
    
    // Convenience: Load from dictionary (JSON-like interface)
    .def("loadFromDict", [](Config& self, const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            
            py::object value = item.second;
            if (value.is_none()) {
                self.set(key, std::string(""), ConfigSource::CommandLine);
            } else if (value.is(py::bool_())) {
                self.set(key, py::cast<bool>(value), ConfigSource::CommandLine);
            } else if (value.is(py::int_())) {
                self.set(key, py::cast<int>(value), ConfigSource::CommandLine);
            } else if (value.is(py::float_())) {
                self.set(key, py::cast<double>(value), ConfigSource::CommandLine);
            } else {
                self.set(key, py::cast<std::string>(value), ConfigSource::CommandLine);
            }
        }
        return true;
    }, py::arg("dict"),
       "Load configuration from Python dictionary")
    
    // Convenience: Set multiple config values from dictionary
    .def("applyConfig", [](Config& self, const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            
            py::object value = item.second;
            if (value.is_none()) {
                self.set(key, std::string(""));
            } else if (value.is(py::bool_())) {
                self.set(key, py::cast<bool>(value));
            } else if (value.is(py::int_())) {
                self.set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                self.set(key, py::cast<double>(value));
            } else {
                self.set(key, py::cast<std::string>(value));
            }
        }
    }, py::arg("dict"),
       "Apply configuration from Python dictionary (runtime settings)")
    
    // Enhanced config methods with better error handling
    .def("setWithSource", [](Config& self, const std::string& key, const py::object& value, ConfigSource source) {
        if (value.is_none()) {
            self.set(key, std::string(""), source);
        } else if (value.is(py::bool_())) {
            self.set(key, py::cast<bool>(value), source);
        } else if (value.is(py::int_())) {
            self.set(key, py::cast<int>(value), source);
        } else if (value.is(py::float_())) {
            self.set(key, py::cast<double>(value), source);
        } else {
            self.set(key, py::cast<std::string>(value), source);
        }
    }, py::arg("key"), py::arg("value"), py::arg("source"),
       "Set configuration value with explicit source")
    
    // Helper to get config values with type conversion
    .def("get", [](Config& self, const std::string& key) {
        if (self.has(key)) {
            // Try common types in order
            auto intVal = self.get<int>(key);
            if (intVal.has_value()) {
                return py::cast(intVal.value());
            }
            auto doubleVal = self.get<double>(key);
            if (doubleVal.has_value()) {
                return py::cast(doubleVal.value());
            }
            auto boolVal = self.get<bool>(key);
            if (boolVal.has_value()) {
                return py::cast(boolVal.value());
            }
            auto stringVal = self.get<std::string>(key);
            if (stringVal.has_value()) {
                return py::cast(stringVal.value());
            }
        }
        throw py::key_error("Key not found: " + key);
    }, py::arg("key"),
       "Get configuration value, automatically inferring type (int, double, bool, string)")
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

    // Batch operations for better performance
    .def("setBatch", [](Config& self, const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            if (value.is_none()) {
                self.set(key, std::string(""));
            } else if (value.is(py::bool_())) {
                self.set(key, py::cast<bool>(value));
            } else if (value.is(py::int_())) {
                self.set(key, py::cast<int>(value));
            } else if (value.is(py::float_())) {
                self.set(key, py::cast<double>(value));
            } else {
                self.set(key, py::cast<std::string>(value));
            }
        }
    }, py::arg("dict"),
       "Set multiple configuration values from dictionary (runtime)")
    
    // Config convenience methods
    .def("getBool", &Config::getOr<bool>, py::arg("key"), py::arg("default"),
         "Get boolean configuration value with default")
    .def("getInt", &Config::getOr<int>, py::arg("key"), py::arg("default"),
         "Get integer configuration value with default")
    .def("getDouble", &Config::getOr<double>, py::arg("key"), py::arg("default"),
         "Get double configuration value with default")
    .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("default"),
         "Get string configuration value with default")
    .def("hasKey", &Config::has, py::arg("key"),
         "Check if configuration key exists")
    .def("getKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("setBool", [](Config& self, const std::string& key, bool value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       "Set boolean configuration value")
    .def("setInt", [](Config& self, const std::string& key, int value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       "Set integer configuration value")
    .def("setDouble", [](Config& self, const std::string& key, double value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       "Set double configuration value")
    .def("setString", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, value);
    }, py::arg("key"), py::arg("value"),
       "Set string configuration value")
    
    // Config source helper methods
    .def("getSourceString", [](const Config& self, const std::string& key) {
        auto it = std::find_if(self.pImpl->entries.begin(), self.pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != self.pImpl->entries.end()) {
            switch (it->source) {
                case ConfigSource::Default: return std::string("Default");
                case ConfigSource::File: return std::string("File");
                case ConfigSource::CommandLine: return std::string("CommandLine");
                case ConfigSource::Runtime: return std::string("Runtime");
                default: return std::string("Unknown");
            }
        }
        return std::string("NotFound");
    }, py::arg("key"),
       "Get configuration source as string for debugging")
    
    // Enhanced Brain wrapper methods with better error handling
    .def("stepWithTime", &Brain::step,
         py::arg("currentStep"), py::arg("currentTime"),
         "Perform simulation step with explicit timestamp (currentStep, currentTime)")
    .def("createBrain", &Brain::initialize,
         py::arg("config"),
         "Initialize brain with configuration")
    .def("getBrainStats", [](const Brain& brain) {
        py::dict stats;
        stats["totalNeurons"] = brain.getTotalNeuronCount();
        stats["totalSynapses"] = brain.getTotalSynapseCount();
        stats["firingNeurons"] = brain.getFiringNeuronCount();
        stats["totalSpikes"] = brain.getTotalSpikeCount();
        stats["avgFiringRate"] = brain.getAverageFiringRate();
        stats["eiRatio"] = brain.getExcitationInhibitionRatio();
        return stats;
    }, "Get brain statistics as dictionary")
    .def("getRegionStats", [](const Brain& brain) {
        py::list regions;
        for (const auto& id : brain.getRegionIds()) {
            if (auto* region = brain.getRegion(id)) {
                py::dict regionInfo;
                regionInfo["id"] = id.index();
                regionInfo["name"] = "Region_" + std::to_string(id.index());
                regionInfo["neuronCount"] = region->getTotalNeuronCount();
                regionInfo["populationCount"] = region->getPopulationCount();
                regionInfo["synapseCount"] = region->getSynapses().size();
                regions.append(regionInfo);
            }
        }
        return regions;
    }, "Get statistics for all brain regions")
    .def("resetBrain", &Brain::reset,
         "Reset brain state to initial conditions")
    .def("saveBrain", &Brain::save, py::arg("filepath"),
         "Save brain state to file")
    .def("loadBrain", &Brain::load, py::arg("filepath"),
         "Load brain state from file")
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
         py::arg("stage"),
         "Set developmental stage")
    .def("getConfig", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         "Get brain configuration")
    .def("logStatus", &Brain::logStatus,
         "Log brain status and statistics")
    
    // AgentBrain wrapper methods with enhanced functionality
    .def("getAgentStats", [](const AgentBrain& agent) {
        py::dict stats;
        stats["visionNeurons"] = agent.getMotorGroupActivities()[0].size;
        stats["touchNeurons"] = agent.getMotorGroupActivities()[1].size;
        stats["internalNeurons"] = agent.getMotorGroupActivities()[2].size;
        stats["curiosity"] = agent.getCuriosityLevel();
        stats["novelty"] = agent.getNoveltyLevel();
        stats["predictionError"] = agent.getPredictionError();
        stats["neuromodulation"] = agent.getNeuromodulationLevel();
        return stats;
    }, "Get agent statistics as dictionary")
    .def("getMotorActivities", &AgentBrain::getMotorGroupActivities,
         "Get motor group activities")
    .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
         "Get thought trace (human-readable perception/action/recognition trace)")
    .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
         "Get internal neuron firing rates (for text association recall)")
    .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
         "Get self-model confidence (how reliably the agent knows its own effects)")
    .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
         "Get number of social observations (for social learning)")
    .def("getConceptCount", &AgentBrain::getConceptCount,
         "Get number of learned concepts")
    .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
         "Get number of planner overrides (how often planner suggested but brain did something else)")
    .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
         "Get number of concept biases applied")
    .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
         "Get number of self-model biases applied")
    .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
         "Get number of social biases applied")
    .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
         "Get number of vision-gated reflex bonuses applied")
    .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
         "Get number of hazard suppression events")
    .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
         "Get number of winner-take-all lateral inhibition pulses")
    .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
         "Get number of times planner actually decided")
    .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
         "Get number of times planner agreed with decoded action")
    .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
         "Get number of planner overrides (advisory ignored)")
    .def("enableAllSystems", [](AgentBrain& agent) {
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
    }, "Enable all neuromodulation and plasticity systems")
    .def("disableAllSystems", [](AgentBrain& agent) {
        agent.enableRewardModulation(false);
        agent.enableStructuralPlasticity(false);
        agent.enableDevelopment(false);
        agent.enableCuriosity(false);
    }, "Disable all neuromodulation and plasticity systems")
    .def("resetAgent", &AgentBrain::reset,
         "Reset agent for new episode")
    .def("getBrain", &AgentBrain::getBrain,
         py::return_value_policy::reference_internal,
         "Get underlying brain pointer")
    .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
         "Get neuromodulation level")
    .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
         "Get curiosity level")
    .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
         "Get novelty level")
    .def("getPredictionError", &AgentBrain::getPredictionError,
         "Get prediction error")
    
    // SimpleWorld wrapper methods
    .def("getWorldStats", [](const SimpleWorld& world) {
        py::dict stats;
        stats["width"] = world.getWidth();
        stats["height"] = world.getHeight();
        stats["simulationTime"] = world.getSimulationTime();
        stats["maxEnergy"] = world.getMaxEnergy();
        return stats;
    }, "Get world statistics as dictionary")
    .def("getAgentBody", [](const SimpleWorld& world) {
        const AgentBody& body = world.getAgentBody();
        py::dict bodyInfo;
        bodyInfo["x"] = body.x;
        bodyInfo["y"] = body.y;
        bodyInfo["orientation"] = body.orientation;
        bodyInfo["velocityX"] = body.velocityX;
        bodyInfo["velocityY"] = body.velocityY;
        bodyInfo["angularVelocity"] = body.angularVelocity;
        bodyInfo["energy"] = body.energy;
        bodyInfo["health"] = body.health;
        bodyInfo["age"] = body.age;
        bodyInfo["isMoving"] = body.isMoving;
        bodyInfo["isTurning"] = body.isTurning;
        return bodyInfo;
    }, "Get agent body state as dictionary")
    .def("getSensoryPercept", [](const SimpleWorld& world) {
        const SensoryPercept& percept = world.getSensoryPercept();
        py::dict perceptInfo;
        
        // Vision
        const auto& vision = percept.getVision();
        perceptInfo["vision"] = py::list(vision.begin(), vision.end());
        
        // Touch
        const auto& touch = percept.getTouch();
        perceptInfo["touch"] = py::list(touch.begin(), touch.end());
        
        // Internal signals
        const auto& internal = percept.getInternal();
        perceptInfo["internal"] = py::list(internal.begin(), internal.end());
        
        // Proprioception
        const auto& proprio = percept.getProprioception();
        perceptInfo["proprioception"] = py::list(proprio.begin(), proprio.end());
        
        return perceptInfo;
    }, "Get sensory percept as dictionary")
    .def("updateWorld", &SimpleWorld::update, py::arg("timestep"),
         "Update world state")
    .def("applyAction", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("currentTime"),
         "Apply motor command to world")
    .def("addWorldObject", &SimpleWorld::addObject, py::arg("obj"),
         "Add object to world")
    .def("removeWorldObject", &SimpleWorld::removeObject,
         py::arg("x"), py::arg("y"),
         "Remove object from world")
    .def("isValidPosition", &SimpleWorld::isValidPosition,
         py::arg("x"), py::arg("y"),
         "Check if position is valid")
    .def("configureWorld", &SimpleWorld::configure,
         py::arg("width"), py::arg("height"),
         py::arg("visionWidth"), py::arg("visionHeight"),
         "Configure world dimensions")
    .def("resetWorld", &SimpleWorld::reset,
         "Reset world to initial state")
    .def("setAgentStart", &SimpleWorld::setAgentStart,
         py::arg("x"), py::arg("y"),
         "Set agent starting position")
    .def("setRandomSeed", &SimpleWorld::setRandomSeed,
         py::arg("seed"),
         "Set random seed for reproducible simulation")
    
    // Convenience functions
    .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default configuration")
    
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create new brain with configuration")
    
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create new simple world")
    
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface for brain")
    .def("version", []() { return std::string("NLM Phase 6.0"); }, "Get NLM version string")
    .def("buildInfo", []() {
        py::dict info;
        info["name"] = "NLM - Neural Learning Machine";
        info["version"] = "6.0 (Phase 6: Final Integration)";
        info["description"] = "An experimental artificial developmental brain";
        info["features"] = std::vector<std::string>{
            "Integrated memory systems (working, episodic, associative)",
            "Neuromodulation (dopamine, curiosity, novelty, prediction error)",
            "Prediction system",
            "Cognition systems (planner, concept formation, attention)",
            "Development system",
            "Structural plasticity",
            "Replay and consolidation",
            "Checkpoint save/load",
            "Phase 6 text-as-a-sense"
        };
        return info;
    }, "Get build information and features")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
