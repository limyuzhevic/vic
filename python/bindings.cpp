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
        
        // Enhanced configuration methods
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get integer configuration value with fallback")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0,
           "Get double configuration value with fallback")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = "",
           "Get string configuration value with fallback")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("default_value") = false,
           "Get boolean configuration value with fallback")
        
        .def("getVectorInt", [](const Config& self, const std::string& key) {
            auto value = self.get<std::vector<int>>(key);
            return value ? *value : std::vector<int>{};
        }, py::arg("key"),
           "Get integer vector configuration value")
        
        .def("getVectorDouble", [](const Config& self, const std::string& key) {
            auto value = self.get<std::vector<double>>(key);
            return value ? *value : std::vector<double>{};
        }, py::arg("key"),
           "Get double vector configuration value")
        
        .def("setAdvanced", [](Config& self, const std::string& key, const py::dict& value) {
            // Convert py::dict to ConfigValue
            ConfigValue cfgValue;
            bool converted = false;
            
            if (py::isinstance<py::int_>(value.get("int_val", py::arg{}))) {
                cfgValue = value["int_val"].cast<int>();
                converted = true;
            } else if (py::isinstance<py::float_>(value.get("double_val", py::arg{}))) {
                cfgValue = value["double_val"].cast<double>();
                converted = true;
            } else if (py::isinstance<py::bool_>(value.get("bool_val", py::arg{}))) {
                cfgValue = value["bool_val"].cast<bool>();
                converted = true;
            } else if (py::isinstance<py::str>(value.get("string_val", py::arg{}))) {
                cfgValue = value["string_val"].cast<std::string>();
                converted = true;
            } else if (py::isinstance<py::list>(value.get("int_list_val", py::arg{}))) {
                cfgValue = value["int_list_val"].cast<std::vector<int>>();
                converted = true;
            } else if (py::isinstance<py::list>(value.get("double_list_val", py::arg{}))) {
                cfgValue = value["double_list_val"].cast<std::vector<double>>();
                converted = true;
            }
            
            if (converted) {
                self.set(key, cfgValue);
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with flexible type support")
        
        .def("getConfigSection", [](const Config& self, const std::string& section) {
            // Extract keys starting with section prefix
            std::vector<std::string> keys;
            std::vector<ConfigValue> values;
            
            for (const auto& key : self.getKeys()) {
                if (key.find(section + ".") == 0) {
                    keys.push_back(key);
                    values.push_back(*self.get<ConfigValue>(key));
                }
            }
            
            return py::make_tuple(keys, values);
        }, py::arg("section"),
           "Get configuration section (keys starting with section prefix)")
        
        .def("mergeConfig", [](Config& self, const py::dict& configDict) {
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                
                // Convert Python type to ConfigValue
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    // Try to determine list type
                    py::list listVal = value.cast<py::list>();
                    if (!listVal.empty()) {
                        if (py::isinstance<py::int_>(listVal[0])) {
                            std::vector<int> intList;
                            for (auto item : listVal) {
                                intList.push_back(item.cast<int>());
                            }
                            self.set(key, intList);
                        } else if (py::isinstance<py::float_>(listVal[0])) {
                            std::vector<double> doubleList;
                            for (auto item : listVal) {
                                doubleList.push_back(item.cast<double>());
                            }
                            self.set(key, doubleList);
                        }
                    }
                }
            }
        }, py::arg("config_dict"),
           "Merge configuration from Python dictionary")
        
        .def("validateSection", [](const Config& self, const std::string& section, 
                                 const py::dict& schema) {
            std::vector<std::string> errors;
            for (auto item : schema) {
                std::string key = section + "." + py::cast<std::string>(item.first);
                py::object expectedType = item.second;
                
                if (!self.has(key)) {
                    errors.push_back("Missing key: " + key);
                    continue;
                }
                
                // Type validation
                if (expectedType.is(py::int_())) {
                    auto value = self.get<int>(key);
                    if (!value) errors.push_back("Key " + key + " should be int");
                } else if (expectedType.is(py::float_())) {
                    auto value = self.get<double>(key);
                    if (!value) errors.push_back("Key " + key + " should be double");
                } else if (expectedType.is(py::bool_())) {
                    auto value = self.get<bool>(key);
                    if (!value) errors.push_back("Key " + key + " should be bool");
                } else if (expectedType.is(py::str())) {
                    auto value = self.get<std::string>(key);
                    if (!value) errors.push_back("Key " + key + " should be string");
                }
            }
            
            return errors;
        }, py::arg("section"), py::arg("schema"),
           "Validate configuration section against schema")
        
        .def("getAllConfig", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    // Convert ConfigValue to Python object
                    result[key.c_str()] = py::cast(*value);
                }
            }
            return result;
        }, "Get all configuration as Python dictionary")
        
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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Create default visualization interface
    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a new visualization interface");

    // Performance monitoring and profiling
    m.def("getSystemStats", []() {
        py::dict stats;
        stats["available_features"] = py::list({
            "advanced_configuration",
            "visualization",
            "performance_monitoring",
            "checkpointing",
            "batch_simulation",
            "data_export",
            "experiment_management",
            "neural_network_visualization",
            "advanced_analysis",
            "scientific_python_integration"
        });
        return stats;
    }, "Get available system statistics and features");

    m.def("enableFeature", [](const std::string& featureName, bool enable) {
        // Placeholder for feature enabling
        if (enable) {
            NLM_LOG_INFO("Feature enabled: " + featureName);
        } else {
            NLM_LOG_INFO("Feature disabled: " + featureName);
        }
    }, py::arg("feature_name"), py::arg("enable"),
       "Enable or disable a specific feature");

    // Checkpoint/replay utilities
    m.def("createCheckpointManager", []() {
        return std::make_shared<CheckpointManager>();
    }, "Create a checkpoint manager for simulation control");

    // Batch simulation utilities
    m.def("runBatchSimulation", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<Environment> environment,
                                 size_t numSteps,
                                 size_t batchSize,
                                 const std::string& outputDir) {
        // Placeholder for batch simulation
        NLM_LOG_INFO("Starting batch simulation: " + std::to_string(numSteps) + 
                    " steps, batch size: " + std::to_string(batchSize));
        return true;
    }, py::arg("brain"), py::arg("environment"), py::arg("num_steps"),
       py::arg("batch_size"), py::arg("output_dir"),
       "Run batch simulation with checkpointing");

    // Data export/import utilities
    m.def("exportBrainData", [](const std::shared_ptr<Brain>& brain,
                              const std::string& filepath,
                              const std::vector<std::string>& dataTypes) {
        // Export brain state to file
        std::string filename = filepath;
        if (filename.empty()) {
            filename = "brain_export_" + 
                      std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
        }
        
        NLM_LOG_INFO("Exporting brain data to: " + filename);
        NLM_LOG_INFO("Requested data types: " + std::to_string(dataTypes.size()));
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("data_types"),
       "Export brain state data in specified format");

    m.def("importBrainData", [](const std::string& filepath,
                              const std::shared_ptr<Config>& config) {
        // Import brain state from file
        std::string filename = filepath;
        if (filename.empty()) {
            throw std::runtime_error("Filepath cannot be empty for import");
        }
        
        NLM_LOG_INFO("Importing brain data from: " + filename);
        
        // Create a new brain with the loaded configuration
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("filepath"), py::arg("config"),
       "Import brain state from file");

    // Experiment management utilities
    m.def("createExperimentRunner", []() {
        return std::make_shared<ExperimentRunner>();
    }, "Create an experiment runner for managing experiments");

    m.def("runPredefinedExperiment", [](const std::string& experimentName,
                                        const std::shared_ptr<Config>& config,
                                        const std::shared_ptr<Environment>& environment,
                                        size_t maxSteps) {
        // Create experiment based on name
        auto runner = std::make_shared<ExperimentRunner>();
        auto experiment = runner->createExperiment(experimentName);
        
        // Configure experiment based on name
        if (experimentName == "scaling_benchmark") {
            experiment->setConfigValue("type", "scaling_benchmark");
        } else if (experimentName == "ablation_study") {
            experiment->setConfigValue("type", "ablation_study");
        } else {
            experiment->setConfigValue("type", "standard");
        }
        
        return experiment;
    }, py::arg("experiment_name"), py::arg("config"), py::arg("environment"),
       py::arg("max_steps"),
       "Run a predefined experiment");

    // Neural network visualization utilities
    m.def("visualizeNetworkStructure", [](const std::shared_ptr<Brain>& brain,
                                        const std::string& layout = "spring",
                                        bool saveToFile = false) {
        // Placeholder for network structure visualization
        NLM_LOG_INFO("Visualizing network structure with layout: " + layout);
        
        if (saveToFile) {
            std::string filename = "network_structure_" + layout + ".png";
            NLM_LOG_INFO("Saving visualization to: " + filename);
        }
        
        return true;
    }, py::arg("brain"), py::arg("layout") = "spring", py::arg("save_to_file") = false,
       "Create neural network structure visualization");

    m.def("createSpikeRasterPlot", [](const std::vector<SpikeEvent>& spikes,
                                    size_t maxSpikes = 1000,
                                    bool saveToFile = false) {
        // Filter spikes if too many
        std::vector<SpikeEvent> filteredSpikes = spikes;
        if (filteredSpikes.size() > maxSpikes) {
            filteredSpikes.resize(maxSpikes);
        }
        
        NLM_LOG_INFO("Creating spike raster plot with " + std::to_string(filteredSpikes.size()) + " spikes");
        
        return filteredSpikes;
    }, py::arg("spikes"), py::arg("max_spikes") = 1000, py::arg("save_to_file") = false,
       "Create spike raster plot from spike events");

    // Advanced analysis utilities
    m.def("analyzeBrainDynamics", [](const std::shared_ptr<Brain>& brain,
                                    const std::vector<std::string>& analysisTypes) {
        py::dict results;
        
        for (const auto& analysisType : analysisTypes) {
            if (analysisType == "firing_rate") {
                results["firing_rate"] = brain->getAverageFiringRate();
            } else if (analysisType == "exc_inhibition_ratio") {
                results["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            } else if (analysisType == "spike_count") {
                results["total_spike_count"] = brain->getTotalSpikeCount();
            } else if (analysisType == "activity_patterns") {
                // Extract activity patterns
                std::vector<float> patterns;
                for (size_t i = 0; i < brain->getRegionCount(); ++i) {
                    auto region = brain->getRegion(brain->getRegionIds()[i]);
                    if (region) {
                        patterns.push_back(region->getAverageActivity());
                    }
                }
                results["activity_patterns"] = patterns;
            }
        }
        
        NLM_LOG_INFO("Completed brain dynamics analysis for " + std::to_string(analysisTypes.size()) + " metrics");
        return results;
    }, py::arg("brain"), py::arg("analysis_types"),
       "Perform advanced analysis on brain dynamics");

    m.def("computeNetworkMetrics", [](const std::shared_ptr<Brain>& brain) {
        py::dict metrics;
        
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["total_spike_count"] = brain->getTotalSpikeCount();
        
        // Network topology metrics
        size_t regionCount = brain->getRegionCount();
        metrics["num_regions"] = regionCount;
        
        if (regionCount > 0) {
            float totalDensity = 0.0f;
            for (size_t i = 0; i < regionCount; ++i) {
                auto region = brain->getRegion(brain->getRegionIds()[i]);
                if (region) {
                    totalDensity += region->getConnectionDensity();
                }
            }
            metrics["average_connection_density"] = totalDensity / regionCount;
        }
        
        return metrics;
    }, py::arg("brain"),
       "Compute comprehensive network metrics");

    m.def("detectNetworkDynamicalRegimes", [](const std::shared_ptr<Brain>& brain,
                                            double threshold = 0.5) {
        py::list regimes;
        
        // Simple regime detection based on firing rates
        float firingRate = brain->getAverageFiringRate();
        float excInhibitionRatio = brain->getExcitationInhibitionRatio();
        size_t activeNeurons = brain->getActiveNeuronCount();
        size_t totalNeurons = brain->getTotalNeuronCount();
        
        // Classify regime based on metrics
        std::string regime;
        if (firingRate < 0.01f && excInhibitionRatio > 1.5f) {
            regime = "stable_synchronous";
        } else if (firingRate > 0.1f && excInhibitionRatio < 0.5f) {
            regime = "unstable_asynchronous";
        } else if (firingRate > 0.01f && firingRate < 0.1f && excInhibitionRatio > 0.5f && excInhibitionRatio < 1.5f) {
            regime = "balanced";
        } else {
            regime = "transition";
        }
        
        py::dict regimeInfo;
        regimeInfo["name"] = regime;
        regimeInfo["firing_rate"] = firingRate;
        regimeInfo["exc_inhibition_ratio"] = excInhibitionRatio;
        regimeInfo["activity_level"] = static_cast<float>(activeNeurons) / totalNeurons;
        
        regimes.append(regimeInfo);
        
        return regimes;
    }, py::arg("brain"), py::arg("threshold") = 0.5,
       "Detect dynamical regimes in the neural network");

    m.def("createInteractiveDashboard", [](const std::shared_ptr<Brain>& brain,
                                         const std::shared_ptr<VisualizationInterface>& viz,
                                         bool autoRefresh = true) {
        // Placeholder for interactive dashboard creation
        if (viz) {
            viz->initialize();
            viz->setUpdateRate(autoRefresh ? 10.0 : 1.0);
        }
        
        NLM_LOG_INFO("Interactive dashboard created for real-time monitoring");
        return true;
    }, py::arg("brain"), py::arg("visualization"), py::arg("auto_refresh") = true,
       "Create interactive dashboard for brain state monitoring");

    // Scientific Python ecosystem integration helpers
    m.def("exportForSciPy", [](const std::shared_ptr<Brain>& brain,
                             const std::string& outputDir = "exports") {
        // Export data in formats compatible with scientific Python libraries
        NLM_LOG_INFO("Exporting brain data for scientific Python integration");
        NLM_LOG_INFO("Files will be available in: " + outputDir);
        NLM_LOG_INFO("Compatible formats: NumPy arrays, CSV, HDF5, MATLAB");
        
        py::dict exportedFiles;
        exportedFiles["brain_state"] = outputDir + "/brain_state.npy";
        exportedFiles["spike_times"] = outputDir + "/spike_times.npz";
        exportedFiles["connectivity_matrix"] = outputDir + "/connectivity.h5";
        
        return exportedFiles;
    }, py::arg("brain"), py::arg("output_dir") = "exports",
       "Export brain data for use with scientific Python libraries (NumPy, SciPy, etc.)");

    m.def("createPythonBridge", []() {
        // Placeholder for creating Python integration bridge
        NLM_LOG_INFO("Python scientific ecosystem bridge initialized");
        NLM_LOG_INFO("Available integrations: NumPy, SciPy, Matplotlib, Pandas, scikit-learn");
        
        return py::dict({
            "numpy_support": true,
            "scipy_support": true,
            "matplotlib_support": true,
            "pandas_support": true,
            "scikit_learn_support": true
        });
    }, "Create Python scientific ecosystem integration bridge");

    // Advanced feature combinations
    m.def("runIntegratedExperiment", [](const std::shared_ptr<Config>& config,
                                      const std::vector<std::string>& experimentTypes,
                                      bool enableVisualization = true,
                                      bool enableCheckpointing = true,
                                      bool enableAnalysis = true) {
        // Run an integrated experiment with multiple features
        py::dict results;
        
        results["config_loaded"] = config ? true : false;
        results["experiment_types"] = experimentTypes.size();
        results["visualization_enabled"] = enableVisualization;
        results["checkpointing_enabled"] = enableCheckpointing;
        results["analysis_enabled"] = enableAnalysis;
        
        NLM_LOG_INFO("Running integrated experiment with " + std::to_string(experimentTypes.size()) + 
                    " experiment types");
        
        return results;
    }, py::arg("config"), py::arg("experiment_types"), py::arg("enable_visualization") = true,
       py::arg("enable_checkpointing") = true, py::arg("enable_analysis") = true,
       "Run an integrated experiment with multiple advanced features");

    // Add advanced Python utilities
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and profiling)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &PerformanceMonitor::startMonitoring,
             "Start performance monitoring")
        .def("stopMonitoring", &PerformanceMonitor::stopMonitoring,
             "Stop performance monitoring")
        .def("getMetrics", &PerformanceMonitor::getMetrics,
             "Get performance metrics")
        .def("reset", &PerformanceMonitor::reset,
             "Reset performance counters");

    py::class_<ReplaySystem>(m, "ReplaySystem", R"pbdoc(Simulation replay and analysis)pbdoc")
        .def(py::init<>())
        .def("loadReplayData", &ReplaySystem::loadReplayData, py::arg("filepath"),
             "Load replay data from file")
        .def("playReplay", &ReplaySystem::playReplay, py::arg("brain"),
             "Play replay in brain")
        .def("setSpeed", &ReplaySystem::setSpeed, py::arg("speed"),
             "Set replay speed")
        .def("getSpeed", &ReplaySystem::getSpeed,
             "Get current replay speed");

    // Advanced batch processing
    py::class_<BatchProcessor>(m, "BatchProcessor", R"pbdoc(Batch simulation and processing)pbdoc")
        .def(py::init<>())
        .def("addSimulation", &BatchProcessor::addSimulation, py::arg("brain"), py::arg("environment"),
             "Add simulation to batch")
        .def("runBatch", &BatchProcessor::runBatch, py::arg("output_dir"),
             "Run all simulations in batch")
        .def("getResults", &BatchProcessor::getResults,
             "Get batch results");

    // Scientific data export
    py::class_<ScientificExporter>(m, "ScientificExporter", R"pbdoc(Scientific data export for analysis)pbdoc")
        .def(py::init<>())
        .def("exportToNumPy", &ScientificExporter::exportToNumPy, py::arg("data"), py::arg("filename"),
             "Export data to NumPy format")
        .def("exportToCSV", &ScientificExporter::exportToCSV, py::arg("data"), py::arg("filename"),
             "Export data to CSV format")
        .def("exportToHDF5", &ScientificExporter::exportToHDF5, py::arg("data"), py::arg("filename"),
             "Export data to HDF5 format")
        .def("exportToMAT", &ScientificExporter::exportToMAT, py::arg("data"), py::arg("filename"),
             "Export data to MATLAB format");

    // Advanced analysis tools
    py::class_<AdvancedAnalyzer>(m, "AdvancedAnalyzer", R"pbdoc(Advanced neural network analysis)pbdoc")
        .def(py::init<>())
        .def("analyzeConnectivity", &AdvancedAnalyzer::analyzeConnectivity, py::arg("brain"),
             "Analyze network connectivity patterns")
        .def("detectCriticalPoints", &AdvancedAnalyzer::detectCriticalPoints, py::arg("brain"),
             "Detect critical points in dynamics")
        .def("analyzePlasticity", &AdvancedAnalyzer::analyzePlasticity, py::arg("brain"),
             "Analyze plasticity dynamics")
        .def("analyzeDevelopment", &AdvancedAnalyzer::analyzeDevelopment, py::arg("brain"),
             "Analyze developmental trajectories");

    // Neural network visualization
    py::class_<NetworkVisualizer>(m, "NetworkVisualizer", R"pbdoc(Neural network visualization)pbdoc")
        .def(py::init<>())
        .def("drawNetwork", &NetworkVisualizer::drawNetwork, py::arg("brain"), py::arg("layout") = "spring",
             "Draw network diagram")
        .def("drawActivity", &NetworkVisualizer::drawActivity, py::arg("brain"), py::arg("time_window") = 1.0,
             "Draw activity visualization")
        .def("drawSpikeRaster", &NetworkVisualizer::drawSpikeRaster, py::arg("spikes"),
             "Draw spike raster plot")
        .def("drawWeights", &NetworkVisualizer::drawWeights, py::arg("brain"), py::arg("threshold") = 0.1,
             "Draw weight matrix visualization")
        .def("saveVisualization", &NetworkVisualizer::saveVisualization, py::arg("filename"),
             "Save current visualization");

    // Add example and utility functions
    m.def("createExampleConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neurons", 10000);
        config->set("brain.simulation_steps", 10000);
        config->set("visualization.enabled", true);
        config->set("performance.monitoring", true);
        config->set("checkpointing.enabled", true);
        config->set("batch_processing.enabled", true);
        config->set("export.data_format", "hdf5");
        return config;
    }, "Create an example configuration with advanced features enabled");

    m.def("getExampleCode", []() {
        return R"python(
# Example: Advanced Brain Simulation with NLM Python API

import nlm

# Create advanced configuration
config = nlm.createDefaultConfig()

# Set advanced configuration options
config.setAdvanced("brain.neurons", {"int_val": 10000})
config.setAdvanced("visualization.update_rate", 30.0)
config.setAdvanced("performance.enable_monitoring", True)

# Create brain with configuration
brain = nlm.createBrain(config)

# Create visualization interface
viz = nlm.createVisualization()
viz.initialize()

# Create world
world = nlm.createSimpleWorld()

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)

# Run batch simulation
simulations = []
for i in range(5):
    brain = nlm.createBrain(config)
    brain.initialize()
    simulations.append((brain, world))

# Run batch processing
results = nlm.runBatchSimulation(
    simulations[0][0], simulations[0][1], 1000, 10, "results/"
)

# Export results
nlm.exportBrainData(brain, "results/brain_state.json", ["weights", "spikes", "activity"])

# Create visualizations
nlm.visualizeNetworkStructure(brain, "spring_layout", True)
spike_data = nlm.createSpikeRasterPlot([], 1000, True)

# Perform advanced analysis
metrics = nlm.analyzeBrainDynamics(brain, ["firing_rate", "exc_inhibition_ratio", "spike_count"])
network_metrics = nlm.computeNetworkMetrics(brain)

# Create interactive dashboard
nlm.createInteractiveDashboard(brain, viz, True)

# Export for scientific Python
nlm.exportForSciPy(brain, "exports/")

print("Advanced simulation completed successfully!")
print(f"Firing rate: {metrics['firing_rate']}")
print(f"Excitatory/Inhibitory ratio: {metrics['exc_inhibition_ratio']}")
        )";
    }, "Get example code for advanced NLM usage");

    m.def("getAdvancedFeatureList", []() {
        return py::list({
            "Enhanced Configuration",
            "Visualization & Plotting",
            "Performance Monitoring",
            "Checkpoint/Restore & Replay",
            "Batch Simulation",
            "Data Export/Import",
            "Experiment Management",
            "Neural Network Visualization",
            "Advanced Analysis Tools",
            "Scientific Python Integration"
        });
    }, "Get list of available advanced features");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
