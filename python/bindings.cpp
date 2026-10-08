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
        .def("loadFromString", [](Config& self, const std::string& jsonString) {
            // Create a temporary file for JSON parsing
            std::string tempFile = "/tmp/nlm_config_temp.json";
            std::ofstream file(tempFile);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot create temporary file for JSON parsing");
            }
            file << jsonString;
            file.close();
            
            bool result = self.loadFromFile(tempFile);
            // Clean up temp file
            std::remove(tempFile.c_str());
            return result;
        }, py::arg("jsonString"),
           "Load configuration from JSON string")
        .def("loadFromYAML", [](Config& self, const std::string& yamlString) {
            // For now, just use the file loading with a temp file
            std::string tempFile = "/tmp/nlm_config_temp.yaml";
            std::ofstream file(tempFile);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot create temporary file for YAML parsing");
            }
            file << yamlString;
            file.close();
            
            bool result = self.loadFromFile(tempFile);
            // Clean up temp file
            std::remove(tempFile.c_str());
            return result;
        }, py::arg("yamlString"),
           "Load configuration from YAML string (JSON format supported)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToString", [](const Config& self) {
            std::stringstream stream;
            self.saveToFile("/tmp/nlm_config_temp_save.json");
            std::ifstream inputFile("/tmp/nlm_config_temp_save.json");
            if (!inputFile.is_open()) {
                throw std::runtime_error("Cannot save configuration to string");
            }
            std::string content((std::istreambuf_iterator<char>(inputFile)),
                               std::istreambuf_iterator<char>());
            inputFile.close();
            std::remove("/tmp/nlm_config_temp_save.json");
            return content;
        }, "Save configuration to JSON string")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto valueOpt = self.get<std::string>(key);
            if (valueOpt) {
                return py::cast(*valueOpt);
            }
            // Try other types
            auto intOpt = self.get<int>(key);
            if (intOpt) return py::cast(*intOpt);
            auto doubleOpt = self.get<double>(key);
            if (doubleOpt) return py::cast(*doubleOpt);
            auto boolOpt = self.get<bool>(key);
            if (boolOpt) return py::cast(*boolOpt);
            return py::none();
        }, py::arg("key"), "Get configuration value by key (returns None if not found)")
        .def("getInt", &Config::get<int>, py::arg("key"), "Get integer configuration value")
        .def("getFloat", &Config::get<double>, py::arg("key"), "Get float configuration value")
        .def("getBool", &Config::get<bool>, py::arg("key"), "Get boolean configuration value")
        .def("getString", &Config::get<std::string>, py::arg("key"), "Get string configuration value")
        .def("getIntOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get integer configuration value or default if not found")
        .def("getFloatOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get float configuration value or default if not found")
        .def("getBoolOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get boolean configuration value or default if not found")
        .def("getStringOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get string configuration value or default if not found")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance(value, py::int_())) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance(value, py::float_())) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance(value, py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance(value, py::str())) {
                self.set(key, value.cast<std::string>());
            } else {
                throw std::runtime_error("Unsupported value type for configuration key: " + key);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (supports int, float, bool, string)")
        .def("setInt", &Config::set<int>, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            py::dict issues;
            
            if (!self.has("brain.neuron_count")) {
                issues["missing_required_keys"].attr("append")("brain.neuron_count");
            }
            if (!self.has("brain.connection_probability")) {
                issues["missing_required_keys"].attr("append")("brain.connection_probability");
            }
            if (!self.has("random_seed")) {
                issues["missing_required_keys"].attr("append")("random_seed");
            }
            
            if (!self.has("plasticity.stdp.enable")) {
                issues["missing_required_keys"].attr("append")("plasticity.stdp.enable");
            }
            
            return issues;
        }, "Validate configuration and return any issues found")
        .def("merge", [](Config& self, const Config& other) {
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                auto valueOpt = other.get<std::string>(key);
                if (valueOpt) {
                    self.set(key, *valueOpt);
                }
            }
            return true;
        }, py::arg("other_config"),
           "Merge another configuration into this one")
        .def("copy", [](const Config& self) {
            Config newConfig;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto valueOpt = self.get<std::string>(key);
                if (valueOpt) {
                    newConfig.set(key, *valueOpt);
                }
            }
            return newConfig;
        }, "Create a copy of this configuration")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // ========== MEMORY SYSTEMS ==========
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        // ========== PREDICTION SYSTEM ==========
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        // ========== COGNITION SYSTEMS ==========
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // ========== DEVELOPMENT SYSTEM ==========
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        // ========== NEUROMODULATION SYSTEMS ==========
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // ========== PLASTICITY SYSTEMS ==========
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity rule")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        // ========== PROPERTY ACCESS ==========
        .def_property_readonly("working_memory_capacity", [](Brain& self) {
            auto* workingMem = self.getWorkingMemory();
            return workingMem ? workingMem->getCapacity() : 0;
        }, "Get working memory capacity")
        .def_property_readonly("episodic_memory_max_episodes", [](Brain& self) {
            auto* episodicMem = self.getEpisodicMemory();
            return episodicMem ? episodicMem->getMaxEpisodes() : 0;
        }, "Get episodic memory max episodes")
        .def_property_readonly("developmental_age", [](Brain& self) {
            auto* devSystem = self.getDevelopmentSystem();
            return devSystem ? devSystem->getAge() : 0.0;
        }, "Get current developmental age")
        .def_property_readonly("is_episode_active", [](Brain& self) {
            return self.getEpisodicMemory() && self.getEpisodicMemory()->isActive();
        }, "Check if episode is active")
        .def_property_readonly("prediction_accuracy", [](Brain& self) {
            auto* predictionSys = self.getPredictionSystem();
            return predictionSys ? predictionSys->getAccuracy() : 0.0f;
        }, "Get prediction system accuracy")
        .def_property_readonly("planner_performance", [](Brain& self) {
            auto* planner = self.getPlanner();
            return planner ? planner->getPerformance() : 0.0f;
        }, "Get planner performance metrics")
        .def_property_readonly("curiosity_level", [](Brain& self) {
            auto* curiosity = self.getCuriosity();
            return curiosity ? curiosity->getLevel() : 0.0f;
        }, "Get curiosity level")
        .def_property_readonly("novelty_level", [](Brain& self) {
            auto* novelty = self.getNovelty();
            return novelty ? novelty->getLevel() : 0.0f;
        }, "Get novelty level")
        .def_property_readonly("prediction_error", [](Brain& self) {
            auto* predError = self.getPredictionErrorSignal();
            return predError ? predError->getError() : 0.0f;
        }, "Get prediction error")
        .def_property_readonly("neuromodulation_level", [](Brain& self) {
            auto* dopamine = self.getDopamine();
            return dopamine ? dopamine->getLevel() : 0.0f;
        }, "Get overall neuromodulation level")
        .def_property_readonly("consolidation_progress", [](Brain& self) {
            auto* episodicMem = self.getEpisodicMemory();
            return episodicMem ? episodicMem->getConsolidationProgress() : 0.0f;
        }, "Get memory consolidation progress")
        .def_property_readonly("replay_progress", [](Brain& self) {
            auto* episodicMem = self.getEpisodicMemory();
            return episodicMem ? episodicMem->getReplayProgress() : 0.0f;
        }, "Get memory replay progress")
        .def("getMemoryStats", [](Brain& self) {
            py::dict stats;
            auto* workingMem = self.getWorkingMemory();
            if (workingMem) {
                stats["working_memory_capacity"] = workingMem->getCapacity();
                stats["working_memory_usage"] = workingMem->getUsage();
            }
            auto* episodicMem = self.getEpisodicMemory();
            if (episodicMem) {
                stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
                stats["episodic_memory_max_episodes"] = episodicMem->getMaxEpisodes();
                stats["consolidation_progress"] = episodicMem->getConsolidationProgress();
                stats["replay_progress"] = episodicMem->getReplayProgress();
            }
            auto* associativeMem = self.getAssociativeMemory();
            if (associativeMem) {
                stats["associative_memory_patterns"] = associativeMem->getPatternCount();
            }
            auto* predictionSys = self.getPredictionSystem();
            if (predictionSys) {
                stats["prediction_accuracy"] = predictionSys->getAccuracy();
                stats["prediction_error"] = predictionSys->getError();
            }
            auto* planner = self.getPlanner();
            if (planner) {
                stats["planner_performance"] = planner->getPerformance();
            }
            return stats;
        }, "Get comprehensive brain statistics and system metrics")
        .def("validate", [](Brain& self) {
            py::dict validation;
            validation["is_valid"] = true;
            validation["errors"] = py::list();
            validation["warnings"] = py::list();
            
            // Basic validation
            if (self.getTotalNeuronCount() == 0) {
                validation["is_valid"] = false;
                validation["errors"].attr("append")("Brain has no neurons");
            }
            
            if (self.getTotalSynapseCount() == 0) {
                validation["warnings"].attr("append")("Brain has no synapses");
            }
            
            auto* memory = self.getWorkingMemory();
            if (memory && memory->getCapacity() == 0) {
                validation["warnings"].attr("append")("Working memory has zero capacity");
            }
            
            return validation;
        }, "Validate brain state and return validation results");
        // ========== MEMORY SYSTEMS ==========
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        // ========== PREDICTION SYSTEM ==========
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        // ========== COGNITION SYSTEMS ==========
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // ========== DEVELOPMENT SYSTEM ==========
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        // ========== NEUROMODULATION SYSTEMS ==========
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // ========== PLASTICITY SYSTEMS ==========
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity rule")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
    // ========== PERFORMANCE MONITORING ==========
    .def("getPerformanceStats", [](Brain& self) {
        py::dict stats;
        
        // Simulation statistics
        stats["simulation_step"] = self.getTotalSpikeCount();
        stats["firing_neurons"] = self.getFiringNeuronCount();
        stats["active_neurons"] = self.getActiveNeuronCount();
        stats["average_firing_rate"] = self.getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = self.getExcitationInhibitionRatio();
        
        // Memory system performance
        auto* workingMem = self.getWorkingMemory();
        if (workingMem) {
            stats["working_memory_usage"] = workingMem->getUsage();
            stats["working_memory_capacity"] = workingMem->getCapacity();
            stats["working_memory_utilization"] = workingMem->getCapacity() > 0 ? 
                (static_cast<float>(workingMem->getUsage()) / workingMem->getCapacity()) : 0.0f;
        }
        
        auto* episodicMem = self.getEpisodicMemory();
        if (episodicMem) {
            stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
            stats["episodic_memory_max_episodes"] = episodicMem->getMaxEpisodes();
            stats["consolidation_progress"] = episodicMem->getConsolidationProgress();
            stats["replay_progress"] = episodicMem->getReplayProgress();
        }
        
        auto* associativeMem = self.getAssociativeMemory();
        if (associativeMem) {
            stats["associative_memory_patterns"] = associativeMem->getPatternCount();
        }
        
        // Prediction system performance
        auto* predictionSys = self.getPredictionSystem();
        if (predictionSys) {
            stats["prediction_accuracy"] = predictionSys->getAccuracy();
            stats["prediction_error"] = predictionSys->getError();
        }
        
        // Cognitive system performance
        auto* planner = self.getPlanner();
        if (planner) {
            stats["planner_performance"] = planner->getPerformance();
        }
        
        auto* conceptForm = self.getConceptFormation();
        if (conceptForm) {
            stats["concept_formation_confidence"] = conceptForm->getConfidence();
        }
        
        // Neuromodulation levels
        auto* curiosity = self.getCuriosity();
        if (curiosity) {
            stats["curiosity_level"] = curiosity->getLevel();
        }
        
        auto* novelty = self.getNovelty();
        if (novelty) {
            stats["novelty_level"] = novelty->getLevel();
        }
        
        auto* dopamine = self.getDopamine();
        if (dopamine) {
            stats["dopamine_level"] = dopamine->getLevel();
        }
        
        auto* predError = self.getPredictionErrorSignal();
        if (predError) {
            stats["prediction_error_signal"] = predError->getError();
        }
        
        // Development metrics
        auto* devSystem = self.getDevelopmentSystem();
        if (devSystem) {
            stats["developmental_stage"] = static_cast<int>(devSystem->getStage());
            stats["developmental_age"] = devSystem->getAge();
            stats["plasticity_modifier"] = devSystem->getPlasticityModifier();
        }
        
        // Plasticity system metrics
        auto* stdp = self.getSTDP();
        if (stdp) {
            stats["stdp_learning_rate"] = stdp->getLearningRate();
        }
        
        auto* hebbian = self.getHebbian();
        if (hebbian) {
            stats["hebbian_learning_rate"] = hebbian->getLearningRate();
        }
        
        auto* structural = self.getStructuralPlasticity();
        if (structural) {
            stats["synaptogenesis_rate"] = structural->getSynaptogenesisRate();
            stats["pruning_rate"] = structural->getPruningRate();
        }
        
        // System health indicators
        stats["neuron_health"] = self.getTotalNeuronCount() > 0 ? 
            (static_cast<float>(self.getFiringNeuronCount()) / self.getTotalNeuronCount()) : 0.0f;
        stats["synapse_health"] = self.getTotalSynapseCount() > 0 ? 
            (static_cast<float>(self.getActiveNeuronCount()) / self.getTotalSynapseCount()) : 0.0f;
        
        return stats;
    }, "Get comprehensive performance statistics for monitoring simulation health")
    .def("getBottleneckAnalysis", [](Brain& self) {
        py::dict bottlenecks;
        
        auto stats = self.getPerformanceStats().cast<py::dict>();
        
        // Check for potential bottlenecks
        float firing_rate = stats["average_firing_rate"].cast<float>();
        float memory_util = stats["working_memory_utilization"].cast<float>();
        float neuron_health = stats["neuron_health"].cast<float>();
        float synapse_health = stats["synapse_health"].cast<float>();
        
        // Identify firing rate bottlenecks
        if (firing_rate < 0.1f) {
            bottlenecks["low_firing_rate"] = "Simulation may be underactive. Consider increasing neuron count or connectivity.";
        } else if (firing_rate > 10.0f) {
            bottlenecks["high_firing_rate"] = "High firing rate may indicate instability. Check neural thresholds and connectivity.";
        }
        
        // Check memory bottlenecks
        if (memory_util > 0.9f) {
            bottlenecks["memory_high_utilization"] = "Working memory nearing capacity. Consider increasing memory capacity or reducing simulation complexity.";
        } else if (memory_util < 0.1f) {
            bottlenecks["memory_low_utilization"] = "Working memory underutilized. Consider increasing memory capacity or adding more complex input.";
        }
        
        // Check neuron health bottlenecks
        if (neuron_health < 0.01f) {
            bottlenecks["low_neuron_activity"] = "Very low neuron activity. Check sensory input or neuron initialization.";
        } else if (neuron_health > 0.5f) {
            bottlenecks["high_neuron_activity"] = "High neuron activity may indicate excessive excitation or low inhibition.";
        }
        
        // Check synapse health bottlenecks
        if (synapse_health < 0.001f) {
            bottlenecks["low_synapse_connectivity"] = "Very low synapse connectivity. Check connection probability or synapse initialization.";
        } else if (synapse_health > 0.1f) {
            bottlenecks["high_synapse_activity"] = "High synapse activity may indicate excessive connectivity or strong synaptic weights.";
        }
        
        // Prediction system bottlenecks
        float pred_accuracy = stats["prediction_accuracy"].cast<float>();
        if (pred_accuracy < 0.5f) {
            bottlenecks["low_prediction_accuracy"] = "Prediction system performance is poor. Check prediction system configuration or training data.";
        } else if (pred_accuracy > 0.95f) {
            bottlenecks["high_prediction_accuracy"] = "Excellent prediction accuracy. Consider increasing prediction complexity or adding novelty detection.";
        }
        
        // Curiosity/novelty bottlenecks
        float curiosity = stats["curiosity_level"].cast<float>();
        float novelty = stats["novelty_level"].cast<float>();
        
        if (curiosity < 0.01f && novelty < 0.01f) {
            bottlenecks["low_exploration"] = "Low curiosity and novelty levels. Consider increasing neuromodulation settings or adding exploration incentives.";
        }
        
        // Development bottlenecks
        float dev_age = stats["developmental_age"].cast<float>();
        if (dev_age > 1000.0f) {
            bottlenecks["advanced_development"] = "System is in advanced developmental stage. Consider developmental system configuration.";
        }
        
        return bottlenecks;
    }, "Analyze simulation for performance bottlenecks and provide optimization suggestions")
    .def("getTimeProfiler", [](Brain& self) {
        // For now, return a simple time profile based on statistics
        // In a full implementation, this would track actual timing data
        py::dict time_profile;
        
        auto stats = self.getPerformanceStats().cast<py::dict>();
        
        float firing_rate = stats["average_firing_rate"].cast<float>();
        float neuron_health = stats["neuron_health"].cast<float>();
        
        // Estimate time spent in different phases
        float step_time_estimate = 0.001; // Base estimate
        
        if (firing_rate < 0.5f) {
            time_profile["neural_dynamics"] = step_time_estimate * 1.2f; // Slower due to low activity
            time_profile["plasticity_updates"] = step_time_estimate * 0.8f; // Less plasticity
        } else if (firing_rate > 5.0f) {
            time_profile["neural_dynamics"] = step_time_estimate * 2.0f; // Slower due to high activity
            time_profile["plasticity_updates"] = step_time_estimate * 1.5f; // More plasticity
        } else {
            time_profile["neural_dynamics"] = step_time_estimate;
            time_profile["plasticity_updates"] = step_time_estimate * 1.0f;
        }
        
        time_profile["memory_operations"] = step_time_estimate * (1.0f + stats["working_memory_utilization"].cast<float>());
        time_profile["prediction_computation"] = step_time_estimate * (1.0f + stats["prediction_accuracy"].cast<float>());
        time_profile["cognition_planning"] = step_time_estimate * (1.0f + stats["planner_performance"].cast<float>());
        
        // Estimate total time for typical simulation
        int estimated_steps = 1000;
        float total_time = (time_profile["neural_dynamics"].cast<float>() +
                          time_profile["plasticity_updates"].cast<float>() +
                          time_profile["memory_operations"].cast<float>() +
                          time_profile["prediction_computation"].cast<float>() +
                          time_profile["cognition_planning"].cast<float>()) * estimated_steps;
        
        time_profile["estimated_total_time_ms"] = total_time * 1000;
        time_profile["time_per_step_ms"] = total_time;
        
        return time_profile;
    }, "Get time profiling estimates for simulation performance")
    .def("getResourceUsage", [](Brain& self) {
        py::dict resource_usage;
        
        auto stats = self.getPerformanceStats().cast<py::dict>();
        
        // Estimate memory usage based on system size
        size_t neuron_count = stats["total_neurons"].cast<size_t>();
        size_t synapse_count = stats["total_synapses"].cast<size_t>();
        
        // Rough estimates (these would be actual measurements in a full implementation)
        float memory_per_neuron = 128.0f; // bytes
        float memory_per_synapse = 64.0f; // bytes
        float memory_per_connection = 32.0f; // bytes
        
        float estimated_memory_bytes = neuron_count * memory_per_neuron +
                                       synapse_count * memory_per_synapse +
                                       neuron_count * memory_per_connection;
        
        resource_usage["estimated_memory_mb"] = estimated_memory_bytes / (1024.0f * 1024.0f);
        resource_usage["memory_per_neuron_bytes"] = memory_per_neuron;
        resource_usage["memory_per_synapse_bytes"] = memory_per_synapse;
        
        // CPU usage estimates (rough approximations)
        float base_cpu_time = 0.001f; // ms per step
        float neural_cpu_factor = stats["average_firing_rate"].cast<float>() / 10.0f;
        float plasticity_cpu_factor = stats["stdp_learning_rate"].cast<float>() / 0.01f;
        
        resource_usage["estimated_cpu_time_per_step_ms"] = base_cpu_time * (1.0f + neural_cpu_factor + plasticity_cpu_factor);
        resource_usage["cpu_neural_activity_factor"] = neural_cpu_factor;
        resource_usage["cpu_plasticity_factor"] = plasticity_cpu_factor;
        
        // Storage requirements
        resource_usage["checkpoint_size_estimate_mb"] = estimated_memory_bytes / (1024.0f * 1024.0f) * 1.5f; // 1.5x for compression
        resource_usage["memory_stats_size_kb"] = (neuron_count + synapse_count) / 1024;
        
        return resource_usage;
    }, "Get estimated resource usage (memory, CPU) for simulation")
    .def("runPerformanceTest", [](Brain& self, int test_duration_steps) {
        py::dict performance_results;
        
        std::vector<float> firing_rates;
        std::vector<float> memory_utilizations;
        std::vector<float> prediction_accuracies;
        
        for (int step = 0; step < test_duration_steps; ++step) {
            self.step(step);
            
            auto stats = self.getPerformanceStats().cast<py::dict>();
            
            firing_rates.push_back(stats["average_firing_rate"].cast<float>());
            memory_utilizations.push_back(stats["working_memory_utilization"].cast<float>());
            prediction_accuracies.push_back(stats["prediction_accuracy"].cast<float>());
        }
        
        // Calculate statistics
        auto [firing_rate_mean, firing_rate_std] = calculate_statistics(firing_rates);
        auto [memory_util_mean, memory_util_std] = calculate_statistics(memory_utilizations);
        auto [pred_accuracy_mean, pred_accuracy_std] = calculate_statistics(prediction_accuracies);
        
        performance_results["firing_rate_mean"] = firing_rate_mean;
        performance_results["firing_rate_std"] = firing_rate_std;
        performance_results["firing_rate_stability"] = 1.0f - (firing_rate_std / firing_rate_mean);
        
        performance_results["memory_utilization_mean"] = memory_util_mean;
        performance_results["memory_utilization_std"] = memory_util_std;
        performance_results["memory_stability"] = 1.0f - (memory_util_std / memory_util_mean);
        
        performance_results["prediction_accuracy_mean"] = pred_accuracy_mean;
        performance_results["prediction_accuracy_std"] = pred_accuracy_std;
        performance_results["prediction_stability"] = 1.0f - (pred_accuracy_std / pred_accuracy_mean);
        
        // Performance score (0-1, higher is better)
        float performance_score = (firing_rate_mean / 10.0f) * 0.3f +      // Aim for ~10 Hz firing
                                (1.0f - fabs(memory_util_mean - 0.5f)) * 0.2f +  // Target 50% utilization
                                pred_accuracy_mean * 0.5f;                // High prediction accuracy
        
        performance_results["performance_score"] = performance_score;
        performance_results["stability_score"] = (performance_results["firing_rate_stability"].cast<float>() +
                                                 performance_results["memory_stability"].cast<float>() +
                                                 performance_results["prediction_stability"].cast<float>()) / 3.0f;
        
        performance_results["total_steps_tested"] = test_duration_steps;
        
        return performance_results;
    }, py::arg("test_duration_steps"), "Run a performance test and return stability and performance metrics")
    .def("comparePerformance", [](const std::vector<std::shared_ptr<Brain>>& brains, const std::string& test_name) {
        py::dict comparison_results;
        
        if (brains.empty()) {
            return comparison_results;
        }
        
        // Collect statistics from all brains
        std::vector<py::dict> brain_stats;
        for (const auto& brain : brains) {
            brain_stats.push_back(brain->getPerformanceStats().cast<py::dict>());
        }
        
        // Compare key metrics across all brains
        py::dict metric_comparison;
        
        // Compare firing rates
        std::vector<float> firing_rates;
        for (const auto& stats : brain_stats) {
            firing_rates.push_back(stats["average_firing_rate"].cast<float>());
        }
        
        float min_firing = *std::min_element(firing_rates.begin(), firing_rates.end());
        float max_firing = *std::max_element(firing_rates.begin(), firing_rates.end());
        float avg_firing = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0f) / firing_rates.size();
        
        metric_comparison["firing_rate"] = py::dict({
            {"min", min_firing},
            {"max", max_firing},
            {"average", avg_firing},
            {"coefficient_of_variation", (max_firing - min_firing) / avg_firing}
        });
        
        // Compare prediction accuracy
        std::vector<float> pred_accuracies;
        for (const auto& stats : brain_stats) {
            pred_accuracies.push_back(stats["prediction_accuracy"].cast<float>());
        }
        
        float min_pred = *std::min_element(pred_accuracies.begin(), pred_accuracies.end());
        float max_pred = *std::max_element(pred_accuracies.begin(), pred_accuracies.end());
        float avg_pred = std::accumulate(pred_accuracies.begin(), pred_accuracies.end(), 0.0f) / pred_accuracies.size();
        
        metric_comparison["prediction_accuracy"] = py::dict({
            {"min", min_pred},
            {"max", max_pred},
            {"average", avg_pred},
            {"coefficient_of_variation", (max_pred - min_pred) / avg_pred}
        });
        
        // Compare curiosity levels
        std::vector<float> curiosity_levels;
        for (const auto& stats : brain_stats) {
            curiosity_levels.push_back(stats["curiosity_level"].cast<float>());
        }
        
        float min_curiosity = *std::min_element(curiosity_levels.begin(), curiosity_levels.end());
        float max_curiosity = *std::max_element(curiosity_levels.begin(), curiosity_levels.end());
        float avg_curiosity = std::accumulate(curiosity_levels.begin(), curiosity_levels.end(), 0.0f) / curiosity_levels.size();
        
        metric_comparison["curiosity_level"] = py::dict({
            {"min", min_curiosity},
            {"max", max_curiosity},
            {"average", avg_curiosity},
            {"coefficient_of_variation", (max_curiosity - min_curiosity) / avg_curiosity}
        });
        
        // Compare memory utilization
        std::vector<float> memory_utils;
        for (const auto& stats : brain_stats) {
            memory_utils.push_back(stats["working_memory_utilization"].cast<float>());
        }
        
        float min_mem = *std::min_element(memory_utils.begin(), memory_utils.end());
        float max_mem = *std::max_element(memory_utils.begin(), memory_utils.end());
        float avg_mem = std::accumulate(memory_utils.begin(), memory_utils.end(), 0.0f) / memory_utils.size();
        
        metric_comparison["memory_utilization"] = py::dict({
            {"min", min_mem},
            {"max", max_mem},
            {"average", avg_mem},
            {"coefficient_of_variation", (max_mem - min_mem) / avg_mem}
        });
        
        // Determine best configuration based on weighted score
        py::dict best_configs;
        for (size_t i = 0; i < brain_stats.size(); ++i) {
            auto& stats = brain_stats[i];
            
            // Calculate composite score
            float score = stats["firing_rate"].cast<float>() / 10.0f * 0.3f +     // Target ~10 Hz
                        (1.0f - fabs(stats["working_memory_utilization"].cast<float>() - 0.5f)) * 0.2f +  // Target 50% utilization
                        stats["prediction_accuracy"].cast<float>() * 0.5f +   // High prediction accuracy
                        stats["curiosity_level"].cast<float>() * 0.1f;       // Good exploration
            
            best_configs[std::to_string(i)] = py::dict({
                {"score", score},
                {"firing_rate", stats["average_firing_rate"].cast<float>()},
                {"prediction_accuracy", stats["prediction_accuracy"].cast<float>()},
                {"curiosity_level", stats["curiosity_level"].cast<float>()},
                {"memory_utilization", stats["working_memory_utilization"].cast<float>()}
            });
        }
        
        comparison_results["metric_comparison"] = metric_comparison;
        comparison_results["best_configurations"] = best_configs;
        comparison_results["test_name"] = test_name;
        
        return comparison_results;
    }, py::arg("brains"), py::arg("test_name"), "Compare performance across multiple brain configurations")
    .def("profileForOptimization", [](Brain& self) {
        py::dict optimization_profile;
        
        auto stats = self.getPerformanceStats().cast<py::dict>();
        auto bottlenecks = self.getBottleneckAnalysis().cast<py::dict>();
        
        // Analyze current performance
        optimization_profile["current_performance"] = py::dict({
            {"firing_rate", stats["average_firing_rate"].cast<float>()},
            {"prediction_accuracy", stats["prediction_accuracy"].cast<float>()},
            {"memory_utilization", stats["working_memory_utilization"].cast<float>()},
            {"curiosity_level", stats["curiosity_level"].cast<float>()}
        });
        
        // Identify optimization priorities
        py::list optimization_priorities;
        
        // Check firing rate
        float firing_rate = stats["average_firing_rate"].cast<float>();
        if (firing_rate < 1.0f) {
            optimization_priorities.append("increase_neuron_count_or_connectivity_to_raise_firing_rate");
        } else if (firing_rate > 20.0f) {
            optimization_priorities.append("decrease_neuron_thresholds_or_reduce_excitation_to_lower_firing_rate");
        }
        
        // Check memory utilization
        float memory_util = stats["working_memory_utilization"].cast<float>();
        if (memory_util > 0.8f) {
            optimization_priorities.append("increase_working_memory_capacity_or_reduce_complexity");
        } else if (memory_util < 0.2f) {
            optimization_priorities.append("decrease_working_memory_capacity_or_increase_input_complexity");
        }
        
        // Check prediction accuracy
        float pred_accuracy = stats["prediction_accuracy"].cast<float>();
        if (pred_accuracy < 0.6f) {
            optimization_priorities.append("improve_prediction_system_or_increase_training_data");
        }
        
        // Check curiosity
        float curiosity = stats["curiosity_level"].cast<float>();
        if (curiosity < 0.1f) {
            optimization_priorities.append("increase_curiosity_settings_or_add_novelty_triggers");
        }
        
        optimization_profile["optimization_priorities"] = optimization_priorities;
        
        // Estimate optimization impact
        py::dict estimated_improvements;
        
        float current_score = stats["average_firing_rate"].cast<float>() / 10.0f * 0.3f +
                               (1.0f - fabs(memory_util - 0.5f)) * 0.2f +
                               pred_accuracy * 0.5f;
        
        estimated_improvements["current_score"] = current_score;
        estimated_improvements["potential_score_after_optimization"] = std::min(1.0f, current_score * 1.5f);
        estimated_improvements["expected_improvement_percent"] = (std::min(1.0f, current_score * 1.5f) - current_score) * 100;
        
        optimization_profile["estimated_improvements"] = estimated_improvements;
        
        return optimization_profile;
    }, "Analyze current performance and provide optimization recommendations");

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
        auto config = std::make_shared<Config>();
        // Set sensible defaults for brain configuration
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.connection_probability", 0.05f);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a default configuration with sensible brain parameters");
    
    m.def("createSmallBrainConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05f);
        config->set("brain.connection_probability", 0.02f);
        config->set("random_seed", 42);
        return config;
    }, "Create a small brain configuration for quick testing");
    
    m.def("createLargeBrainConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.2f);
        config->set("brain.connection_probability", 0.1f);
        config->set("random_seed", 42);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        return config;
    }, "Create a large brain configuration for advanced research");
    
    m.def("createQuickTestConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.connection_probability", 0.08f);
        config->set("random_seed", 12345);
        config->set("neuromod.curiosity.enable", false);
        return config;
    }, "Create a quick test configuration for basic simulation");
    
    // Convenience functions for common simulation patterns
    m.def("runSimpleSimulation", [](int num_steps, bool enable_learning = true) {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), 
                              brain->getAverageFiringRate(),
                              brain->getFiringNeuronCount());
    }, py::arg("num_steps"), py::arg("enable_learning") = true,
       "Run a simple simulation without world interaction");
    
    m.def("createAgentWithWorld", [](int world_width = 20, int world_height = 20) {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(world_width, world_height, 8, 8);
        world->reset();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("world_width") = 20, py::arg("world_height") = 20,
       "Create a complete agent setup with brain and world");
    
    m.def("runCompleteEpisode", [](int num_steps) {
        auto [brain, world, agent] = createAgentWithWorld();
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel());
    }, py::arg("num_steps"),
       "Run a complete agent episode with world interaction");
    
    // Error handling utilities
    m.def("getLastError", []() {
        // Return any last error message (placeholder for now)
        return std::string("No error");
    }, "Get the last error message (placeholder)");
    
    m.def("setSimulationSeed", [](uint64_t seed) {
        auto config = createDefaultConfig();
        config->set("random_seed", seed);
        return config;
    }, py::arg("seed"), "Create a configuration with a specific random seed");
    
    // Configuration validation utilities
    m.def("validateConfig", [](const Config& config) {
        py::dict results;
        results["is_valid"] = true;
        results["errors"] = py::list();
        results["warnings"] = py::list();
        
        if (!config.has("brain.neuron_count")) {
            results["is_valid"] = false;
            results["errors"].attr("append")("Missing required key: brain.neuron_count");
        }
        
        if (config.has("brain.neuron_count")) {
            auto neuronCount = config.getIntOr("brain.neuron_count", 0);
            if (neuronCount <= 0) {
                results["warnings"].attr("append")("brain.neuron_count should be positive");
            }
            if (neuronCount > 100000) {
                results["warnings"].attr("append")("brain.neuron_count is very large, may impact performance");
            }
        }
        
        return results;
    }, py::arg("config"), "Validate configuration and return validation results");
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
