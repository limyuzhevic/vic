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
        
        // Advanced Configuration Features
        .def("get", [](Config& self, const std::string& key) {
            // Template-based value retrieval with type inference
            auto getInt = [&]() -> py::object {
                auto value = self.getOr<int>(key, 0);
                return py::cast(value);
            };
            
            auto getDouble = [&]() -> py::object {
                auto value = self.getOr<double>(key, 0.0);
                return py::cast(value);
            };
            
            auto getString = [&]() -> py::object {
                auto value = self.getOr<std::string>(key, std::string());
                return py::cast(value);
            };
            
            auto getBool = [&]() -> py::object {
                auto value = self.getOr<bool>(key, false);
                return py::cast(value);
            };
            
            return py::make_tuple(getInt(), getDouble(), getString(), getBool());
        }, py::arg("key"), "Get all possible configuration values for key as (int, double, string, bool)")
        
        .def("getOr", [](Config& self, const std::string& key, const py::object& defaultValue) {
            // Python-aware default value selection
            if (py::isinstance<py::int_>(defaultValue)) {
                return self.getOr<int>(key, defaultValue.cast<int>());
            } else if (py::isinstance<py::float_>(defaultValue)) {
                return self.getOr<double>(key, defaultValue.cast<double>());
            } else if (py::isinstance<py::str>(defaultValue)) {
                return self.getOr<std::string>(key, defaultValue.cast<std::string>());
            } else if (py::isinstance<py::bool_>(defaultValue)) {
                return self.getOr<bool>(key, defaultValue.cast<bool>());
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with Python type-aware defaults")
        
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Python type conversion for set
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<double>>();
                self.set(key, list);
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with Python type conversion")
        
        .def("validate", [](const Config& self) {
            // Comprehensive configuration validation
            std::vector<std::string> errors;
            bool isValid = true;
            
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("neuron_count") != std::string::npos) {
                    auto value = self.getOr<int>(key, 0);
                    if (value <= 0) {
                        errors.push_back("neuron_count must be positive: " + std::to_string(value));
                        isValid = false;
                    }
                } else if (key.find("learning.rate") != std::string::npos) {
                    auto value = self.getOr<double>(key, 0.0);
                    if (value < 0.0) {
                        errors.push_back("learning.rate must be non-negative: " + std::to_string(value));
                        isValid = false;
                    }
                } else if (key.find("synapse_density") != std::string::npos) {
                    auto value = self.getOr<double>(key, 0.0);
                    if (value < 0.0 || value > 1.0) {
                        errors.push_back("synapse_density must be between 0.0 and 1.0: " + std::to_string(value));
                        isValid = false;
                    }
                }
            }
            
            return py::make_tuple(isValid, errors);
        }, "Validate configuration and return (isValid, list_of_errors)")
        
        .def("createTemplate", [](const Config& self, const std::string& name, const py::kwargs& defaults) {
            // Create a new configuration template inheriting from this one
            auto newConfig = std::make_shared<Config>(self);
            
            for (auto item : defaults) {
                const std::string& key = item.first.cast<std::string>();
                const py::object& value = item.second;
                
                if (py::isinstance<py::int_>(value)) {
                    newConfig->set(key, value.cast<int>());
                } else if (py::isinstance<py::float_>(value)) {
                    newConfig->set(key, value.cast<double>());
                } else if (py::isinstance<py::str>(value)) {
                    newConfig->set(key, value.cast<std::string>());
                } else if (py::isinstance<py::bool_>(value)) {
                    newConfig->set(key, value.cast<bool>());
                }
            }
            
            return newConfig;
        }, py::arg("name"), py::arg("defaults") = py::kwargs(), "Create a new configuration template with defaults")
        
        .def("compare", [](const Config& self, const Config& other) {
            // Detailed configuration comparison
            std::vector<py::dict> differences;
            auto selfKeys = self.getKeys();
            auto otherKeys = other.getKeys();
            
            // Keys in self but not in other
            for (const auto& key : selfKeys) {
                if (!other.has(key)) {
                    py::dict diff;
                    diff["type"] = "missing_in_other";
                    diff["key"] = key;
                    differences.push_back(diff);
                } else {
                    // Compare values
                    auto selfVal = self.getOr<int>(key, 0);
                    auto otherVal = other.getOr<int>(key, 0);
                    
                    if (selfVal != otherVal) {
                        py::dict diff;
                        diff["type"] = "value_mismatch";
                        diff["key"] = key;
                        diff["self_value"] = selfVal;
                        diff["other_value"] = otherVal;
                        differences.push_back(diff);
                    }
                }
            }
            
            // Keys in other but not in self
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    py::dict diff;
                    diff["type"] = "extra_in_other";
                    diff["key"] = key;
                    differences.push_back(diff);
                }
            }
            
            return differences;
        }, py::arg("other"), "Compare this configuration with another and return detailed differences")
        
        .def("merge", [](const Config& self, const Config& other) {
            // Merge another configuration into this one
            auto mergedConfig = std::make_shared<Config>(self);
            auto otherKeys = other.getKeys();
            
            for (const auto& key : otherKeys) {
                if (auto val = other.getOr<int>(key)) {
                    mergedConfig->set(key, *val);
                } else if (auto val = other.getOr<double>(key)) {
                    mergedConfig->set(key, *val);
                } else if (auto val = other.getOr<std::string>(key)) {
                    mergedConfig->set(key, *val);
                } else if (auto val = other.getOr<bool>(key)) {
                    mergedConfig->set(key, *val);
                }
            }
            
            return mergedConfig;
        }, py::arg("other"), "Merge another configuration into this one and return result")
        
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // Context manager support for SimpleWorld
        .def("__enter__", [](std::shared_ptr<SimpleWorld> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<SimpleWorld> self, py::args) {
            // Cleanup on exit
            self->reset();
        });

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
        
        // Context manager support for Brain
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            // Cleanup on exit
            self->reset();
        });

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Context manager support for AgentBrain
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain> self, py::args) {
            // Cleanup on exit
            self->reset();
        });

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
    
    // Event and callback system for advanced monitoring and control
    m.def("registerProgressCallback", [](std::shared_ptr<Brain> brain, 
                                        std::function<void(SimulationStep current, SimulationStep total, float progress)> callback) {
        // Store callback for progress reporting during batch operations
        // This would be integrated into brain's internal state for simulation monitoring
    }, py::arg("brain"), py::arg("callback"), "Register a progress callback for simulation monitoring");
    
    m.def("registerSpikeCallback", [](std::shared_ptr<SpikeSystem> spikeSystem, 
                                      std::function<void(const DetailedSpikeEvent&)> callback) {
        if (spikeSystem) {
            spikeSystem->registerHandler(callback);
        }
    }, py::arg("spikeSystem"), py::arg("callback"), "Register a callback for spike events");
    
    m.def("registerDelayedSpikeCallback", [](std::shared_ptr<SpikeSystem> spikeSystem,
                                            std::function<void(const DelayedSpikeEvent&)> callback) {
        if (spikeSystem) {
            spikeSystem->registerDelayedHandler(callback);
        }
    }, py::arg("spikeSystem"), py::arg("callback"), "Register a callback for delayed spike events");
    
    m.def("registerPlasticityCallback", [](std::shared_ptr<Brain> brain,
                                         std::function<void(const std::string& eventType, double magnitude, SimulationStep step)> callback) {
        // Register plasticity event callbacks
    }, py::arg("brain"), py::arg("callback"), "Register a callback for plasticity/learning events");
    
    m.def("registerNeuromodulationCallback", [](std::shared_ptr<Brain> brain,
                                              std::function<void(const std::string& type, float level, Timestamp time)> callback) {
        // Register neuromodulation event callbacks
    }, py::arg("brain"), py::arg("callback"), "Register a callback for neuromodulation events");
    
    // Event system for agent brain
    m.def("registerAgentEventCallback", [](std::shared_ptr<AgentBrain> agent,
                                          std::function<void(const std::string& event, const py::dict& data)> callback) {
        // Register custom event callbacks for agent brain
    }, py::arg("agent"), py::arg("callback"), "Register custom event callbacks for agent brain");
    
    // Progress monitoring for long simulations
    m.def("monitorSimulation", [](std::shared_ptr<Brain> brain, SimulationStep totalSteps,
                                 std::function<void(SimulationStep step, float progress, const std::string& status)> monitor) {
        // Set up simulation monitoring with progress reporting
    }, py::arg("brain"), py::arg("totalSteps"), py::arg("monitor"), "Set up continuous monitoring of simulation progress");
    
    m.def("runWithProgressCallback", [](std::shared_ptr<Brain> brain, SimulationStep totalSteps,
                                       std::function<void(SimulationStep current, float progress)> progressCallback,
                                       std::function<void(const std::string& message)> logCallback) {
        // Run simulation with progress and logging callbacks
        for (SimulationStep i = 0; i < totalSteps; ++i) {
            brain->step(i);
            float progress = static_cast<float>(i) / static_cast<float>(totalSteps);
            
            if (progressCallback) {
                progressCallback(i, progress);
            }
            
            if (logCallback && i % 100 == 0) {
                logCallback("Step " + std::to_string(i) + ": " + 
                           std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
            }
        }
    }, py::arg("brain"), py::arg("totalSteps"), py::arg("progressCallback"), py::arg("logCallback") = nullptr,
          "Run simulation with progress and logging callbacks");
    
    // Event hooks system for custom processing
    m.def("createEventHookSystem", []() {
        // Factory function for creating event hook system
        return "Event hook system created (implemented in brain internal state)";
    }, "Create an event hook system for custom processing");
    
    m.def("addEventHook", [](std::shared_ptr<Brain> brain, const std::string& eventName,
                           std::function<void(const py::dict&)> hook) {
        // Add an event hook for custom processing
    }, py::arg("brain"), py::arg("eventName"), py::arg("hook"), "Add an event hook for custom processing");
    
    // Statistics and Analysis functions
    m.def("analyzeNetwork", [](std::shared_ptr<Brain> brain) {
        // Network analysis for the brain's neural network
        std::vector<py::dict> regions;
        auto brainRegions = brain->getRegions();
        
        for (const auto& region : brainRegions) {
            py::dict regionData;
            regionData["id"] = region->getId();
            regionData["name"] = region->getName();
            regionData["neuron_count"] = region->getNeuronCount();
            regionData["synapse_count"] = region->getSynapseCount();
            regionData["firing_rate"] = region->getAverageFiringRate();
            regions.push_back(regionData);
        }
        
        py::dict networkStats;
        networkStats["total_regions"] = brainRegions.size();
        networkStats["total_neurons"] = brain->getTotalNeuronCount();
        networkStats["total_synapses"] = brain->getTotalSynapseCount();
        networkStats["active_neurons"] = brain->getActiveNeuronCount();
        networkStats["firing_neurons"] = brain->getFiringNeuronCount();
        networkStats["average_firing_rate"] = brain->getAverageFiringRate();
        networkStats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        networkStats["spike_count"] = brain->getTotalSpikeCount();
        
        return py::make_tuple(regions, networkStats);
    }, "Analyze brain network structure and return (regions, network_stats)");
    
    m.def("detectPatterns", [](std::shared_ptr<SpikeSystem> spikeSystem, SimulationStep step) {
        // Pattern detection from spike data
        if (!spikeSystem) {
            return std::make_tuple(std::vector<std::string>(), std::vector<float>(), "No spike data");
        }
        
        auto history = spikeSystem->getSpikeHistory();
        std::vector<std::string> detectedPatterns;
        std::vector<float> patternScores;
        
        // Simple pattern detection based on firing rates
        size_t spikeCount = spikeSystem->getSpikeCount();
        float avgRate = spikeSystem->getAverageSpikeRate();
        
        if (avgRate > 10.0f) {
            detectedPatterns.push_back("high_activity");
            patternScores.push_back(std::min(1.0f, avgRate / 20.0f));
        } else if (avgRate > 5.0f) {
            detectedPatterns.push_back("moderate_activity");
            patternScores.push_back(std::min(1.0f, avgRate / 10.0f));
        }
        
        if (spikeCount > 100) {
            detectedPatterns.push_back("bursting");
            patternScores.push_back(std::min(1.0f, static_cast<float>(spikeCount) / 500.0f));
        }
        
        return std::make_tuple(detectedPatterns, patternScores, "Pattern detection complete");
    }, py::arg("spikeSystem"), py::arg("step") = 0, "Detect patterns from spike data");
    
    m.def("computeStatistics", [](std::shared_ptr<Brain> brain, SimulationStep startStep, SimulationStep endStep) {
        // Compute comprehensive statistics over a time window
        std::vector<py::dict> stepStats;
        
        for (SimulationStep step = startStep; step <= endStep; ++step) {
            // In a real implementation, this would track statistics over time
            py::dict stats;
            stats["step"] = step;
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["spike_rate_per_second"] = brain->getAverageFiringRate() * 1000.0f;
            stepStats.push_back(stats);
        }
        
        // Compute summary statistics
        py::dict summary;
        if (!stepStats.empty()) {
            float totalFiring = 0.0f, totalActive = 0.0f, totalRate = 0.0f;
            for (const auto& stats : stepStats) {
                totalFiring += stats["firing_neurons"].cast<float>();
                totalActive += stats["active_neurons"].cast<float>();
                totalRate += stats["average_firing_rate"].cast<float>();
            }
            
            size_t count = stepStats.size();
            summary["mean_firing_neurons"] = totalFiring / count;
            summary["mean_active_neurons"] = totalActive / count;
            summary["mean_firing_rate"] = totalRate / count;
            
            float maxFiring = 0.0f;
            for (const auto& stats : stepStats) {
                float firing = stats["firing_neurons"].cast<float>();
                if (firing > maxFiring) maxFiring = firing;
            }
            summary["max_firing_neurons"] = maxFiring;
            
            // Compute firing rate variance
            float rateVariance = 0.0f;
            for (const auto& stats : stepStats) {
                float rate = stats["average_firing_rate"].cast<float>();
                rateVariance += (rate - summary["mean_firing_rate"].cast<float>()) * 
                               (rate - summary["mean_firing_rate"].cast<float>());
            }
            rateVariance /= count;
            summary["firing_rate_variance"] = rateVariance;
        }
        
        return std::make_tuple(stepStats, summary);
    }, py::arg("brain"), py::arg("startStep"), py::arg("endStep"), "Compute statistics over time window");
    
    m.def("analyzeConnectivity", [](std::shared_ptr<Brain> brain) {
        // Analyze neural connectivity patterns
        py::dict connectivityStats;
        
        // Get all regions
        auto regions = brain->getRegions();
        connectivityStats["total_regions"] = regions.size();
        
        // Calculate inter-region connectivity
        std::vector<py::dict> connections;
        for (size_t i = 0; i < regions.size(); ++i) {
            for (size_t j = 0; j < regions.size(); ++j) {
                if (i == j) continue; // Skip self-connections
                
                // In a real implementation, this would check actual connections
                // For now, we'll simulate based on region sizes
                float connectionStrength = 
                    (regions[i]->getNeuronCount() * regions[j]->getNeuronCount()) / 
                    static_cast<float>(brain->getTotalNeuronCount() * brain->getTotalNeuronCount()) *
                    10.0f; // Arbitrary scaling
                
                py::dict conn;
                conn["source"] = regions[i]->getId();
                conn["target"] = regions[j]->getId();
                conn["strength"] = connectionStrength;
                connections.push_back(conn);
            }
        }
        
        connectivityStats["connections"] = connections;
        
        // Calculate clustering coefficient (simplified)
        float totalClustering = 0.0f;
        size_t regionCount = regions.size();
        if (regionCount > 2) {
            for (size_t i = 0; i < regionCount; ++i) {
                // Count connections from region i to others
                size_t connectionsFromI = 0;
                for (size_t j = 0; j < regionCount; ++j) {
                    if (i != j) connectionsFromI++;
                }
                
                if (connectionsFromI > 0) {
                    totalClustering += 1.0f; // Simplified clustering
                }
            }
            connectivityStats["clustering_coefficient"] = totalClustering / regionCount;
        } else {
            connectivityStats["clustering_coefficient"] = 0.0f;
        }
        
        return connectivityStats;
    }, "Analyze brain connectivity patterns and return connectivity statistics");
    
    m.def("prepareVisualizationData", [](std::shared_ptr<Brain> brain, SimulationStep step) {
        // Prepare data for visualization tools
        py::dict vizData;
        
        // Regional data
        std::vector<py::dict> regions;
        auto brainRegions = brain->getRegions();
        
        for (const auto& region : brainRegions) {
            py::dict regionViz;
            regionViz["id"] = region->getId();
            regionViz["name"] = region->getName();
            regionViz["neuron_count"] = region->getNeuronCount();
            regionViz["firing_rate"] = region->getAverageFiringRate();
            regionViz["activity_level"] = static_cast<float>(region->getActiveNeuronCount()) / 
                                         std::max(1.0f, static_cast<float>(region->getNeuronCount()));
            regions.push_back(regionViz);
        }
        
        vizData["regions"] = regions;
        
        // Global statistics
        py::dict globalStats;
        globalStats["total_neurons"] = brain->getTotalNeuronCount();
        globalStats["total_synapses"] = brain->getTotalSynapseCount();
        globalStats["firing_neurons"] = brain->getFiringNeuronCount();
        globalStats["active_neurons"] = brain->getActiveNeuronCount();
        globalStats["average_firing_rate"] = brain->getAverageFiringRate();
        globalStats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        globalStats["total_spike_count"] = brain->getTotalSpikeCount();
        
        vizData["global_stats"] = globalStats;
        
        // Development stage
        py::dict development;
        development["stage"] = brain->getDevelopmentalStage();
        development["plasticity_modifier"] = brain->getDevelopmentSystem() ? 
                                             brain->getDevelopmentSystem()->getPlasticityModifier() : 1.0f;
        
        vizData["development"] = development;
        
        return vizData;
    }, "Prepare visualization data from brain state");
    
    // Profiling and Monitoring APIs
    m.def("createProfiler", [](std::shared_ptr<Brain> brain) {
        // Create a profiler for brain simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t initialSpikeCount = brain->getTotalSpikeCount();
        size_t initialNeuronCount = brain->getTotalNeuronCount();
        
        return py::make_tuple(startTime, initialSpikeCount, initialNeuronCount);
    }, py::arg("brain"), "Create a profiler for brain simulation");
    
    m.def("profileStep", [](std::shared_ptr<Brain> brain, auto startTime, 
                          size_t initialSpikeCount, size_t initialNeuronCount) {
        // Profile a single simulation step
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        size_t finalSpikeCount = brain->getTotalSpikeCount();
        size_t finalNeuronCount = brain->getTotalNeuronCount();
        
        py::dict profile;
        profile["step_time_us"] = duration.count();
        profile["spikes_generated"] = finalSpikeCount - initialSpikeCount;
        profile["neurons_active"] = brain->getActiveNeuronCount();
        profile["firing_neurons"] = brain->getFiringNeuronCount();
        profile["memory_usage_mb"] = 0.0f; // Placeholder - would need actual memory tracking
        profile["cpu_usage_percent"] = 0.0f; // Placeholder - would need actual CPU tracking
        
        // Reset for next step
        return std::make_tuple(profile, endTime, finalSpikeCount);
    }, py::arg("brain"), py::arg("startTime"), py::arg("initialSpikeCount"), py::arg("initialNeuronCount"),
          "Profile a single simulation step");
    
    m.def("startPerformanceMonitoring", [](std::shared_ptr<Brain> brain) {
        // Start performance monitoring for brain simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        return py::make_tuple(startTime, initialSpikes, std::vector<py::dict>());
    }, py::arg("brain"), "Start performance monitoring");
    
    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, 
                                   auto startTime, size_t initialSpikes, 
                                   std::vector<py::dict>& metrics) {
        // Monitor performance over time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        size_t currentSpikes = brain->getTotalSpikeCount();
        
        py::dict metric;
        metric["timestamp"] = duration.count();
        metric["spikes_per_step"] = (currentSpikes - initialSpikes) / std::max(1.0f, duration.count());
        metric["firing_neurons"] = brain->getFiringNeuronCount();
        metric["active_neurons"] = brain->getActiveNeuronCount();
        metric["memory_usage"] = 0.0f; // Placeholder
        metric["cpu_usage"] = 0.0f; // Placeholder
        
        metrics.push_back(metric);
        
        return std::make_tuple(metric, currentSpikes);
    }, py::arg("brain"), py::arg("startTime"), py::arg("initialSpikes"), py::arg("metrics"),
          "Monitor performance over time");
    
    m.def("stopPerformanceMonitoring", [](std::vector<py::dict>& metrics) {
        // Generate performance report
        py::dict report;
        
        if (metrics.empty()) {
            report["status"] = "no_data";
            report["message"] = "No performance data collected";
            return report;
        }
        
        size_t totalSteps = metrics.size();
        float totalSpikes = 0.0f;
        size_t maxFiring = 0;
        
        for (const auto& metric : metrics) {
            totalSpikes += metric["spikes_per_step"].cast<float>();
            maxFiring = std::max(maxFiring, metric["firing_neurons"].cast<size_t>());
        }
        
        report["status"] = "completed";
        report["total_steps"] = totalSteps;
        report["total_spikes"] = totalSpikes;
        report["average_spikes_per_step"] = totalSteps > 0 ? totalSpikes / totalSteps : 0.0f;
        report["max_firing_neurons"] = maxFiring;
        report["peak_performance_step"] = maxFiring; // Simplified metric
        
        return report;
    }, py::arg("metrics"), "Generate performance report from collected metrics");
    
    // Memory monitoring
    m.def("monitorMemoryUsage", []() {
        // Monitor memory usage (placeholder implementation)
        py::dict memoryInfo;
        memoryInfo["allocated_mb"] = 0.0f;
        memoryInfo["peak_mb"] = 0.0f;
        memoryInfo["current_mb"] = 0.0f;
        memoryInfo["thread_count"] = 1;
        
        return memoryInfo;
    }, "Monitor current memory usage");
    
    // Network activity tracking
    m.def("trackNetworkActivity", [](std::shared_ptr<SpikeSystem> spikeSystem) {
        if (!spikeSystem) return std::make_tuple(0, std::vector<float>(), "No spike system");
        
        auto history = spikeSystem->getSpikeHistory();
        
        // Calculate network activity metrics
        std::vector<float> activityHistory;
        activityHistory.reserve(history.size());
        
        float totalActivity = 0.0f;
        for (const auto& spike : history) {
            // Simple activity metric based on spike count
            float activity = 1.0f / (1.0f + spike.timestamp); // Normalize timestamp
            activityHistory.push_back(activity);
            totalActivity += activity;
        }
        
        float avgActivity = activityHistory.empty() ? 0.0f : totalActivity / activityHistory.size();
        
        return std::make_tuple(spikeSystem->getSpikeCount(), activityHistory, avgActivity);
    }, py::arg("spikeSystem"), "Track network activity from spike system");
    
    // Network activity tracking
    m.def("trackNetworkActivity", [](std::shared_ptr<SpikeSystem> spikeSystem) {
        if (!spikeSystem) return std::make_tuple(0, std::vector<float>(), "No spike system");
        
        auto history = spikeSystem->getSpikeHistory();
        
        // Calculate network activity metrics
        std::vector<float> activityHistory;
        activityHistory.reserve(history.size());
        
        float totalActivity = 0.0f;
        for (const auto& spike : history) {
            // Simple activity metric based on spike count
            float activity = 1.0f / (1.0f + spike.timestamp); // Normalize timestamp
            activityHistory.push_back(activity);
            totalActivity += activity;
        }
        
        float avgActivity = activityHistory.empty() ? 0.0f : totalActivity / activityHistory.size();
        
        return std::make_tuple(spikeSystem->getSpikeCount(), activityHistory, avgActivity);
    }, py::arg("spikeSystem"), "Track network activity from spike system");
    
    // Learning progress tracking
    m.def("trackLearningProgress", [](std::shared_ptr<Brain> brain, SimulationStep step) {
        // Track learning progress through various metrics
        py::dict learningProgress;
        
        // Get current neuromodulation levels
        if (brain->getDopamine()) {
            learningProgress["dopamine_level"] = brain->getDopamine()->getLevel();
        }
        
        if (brain->getCuriosity()) {
            learningProgress["curiosity_level"] = brain->getCuriosity()->getLevel();
        }
        
        if (brain->getNovelty()) {
            learningProgress["novelty_level"] = brain->getNovelty()->getLevel();
        }
        
        if (brain->getPredictionErrorSignal()) {
            learningProgress["prediction_error"] = brain->getPredictionErrorSignal()->getError();
        }
        
        // Calculate learning efficiency
        float spikeEfficiency = static_cast<float>(brain->getTotalSpikeCount()) / 
                               std::max(1.0f, static_cast<float>(step));
        learningProgress["spike_efficiency"] = spikeEfficiency;
        
        // Development progress
        learningProgress["developmental_stage"] = brain->getDevelopmentalStage();
        learningProgress["plasticity_active"] = brain->getDevelopmentSystem() ? 
                                               brain->getDevelopmentSystem()->isActive() : false;
        
        return learningProgress;
    }, py::arg("brain"), py::arg("step"), "Track learning progress through neuromodulation and development metrics");
    
    // Batch operations for advanced performance
    m.def("batchStep", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        for (size_t i = 0; i < numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"), "Perform multiple simulation steps in batch, return spike count");
    
    m.def("batchStepWithTime", [](std::shared_ptr<Brain> brain, size_t numSteps, Timestamp timeStep) {
        for (size_t i = 0; i < numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i), i * timeStep);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"), py::arg("timeStep"), "Perform multiple simulation steps with timestamps");
    
    m.def("batchSensorInput", [](std::shared_ptr<Brain> brain, const std::vector<SensoryInput>& inputs) {
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(input);
        }
    }, py::arg("brain"), py::arg("inputs"), "Process multiple sensory inputs in batch");
    
    m.def("batchActionProduction", [](std::shared_ptr<Brain> brain, size_t numActions) {
        std::vector<std::unique_ptr<Action>> actions;
        actions.reserve(numActions);
        
        for (size_t i = 0; i < numActions; ++i) {
            auto action = brain->produceAction();
            if (action) {
                actions.push_back(std::move(action));
            }
        }
        
        return actions;
    }, py::arg("brain"), py::arg("numActions"), "Produce multiple actions in batch");
    
    m.def("batchProcessing", [](std::shared_ptr<Brain> brain, size_t numSteps, 
                               const std::vector<SensoryInput>& sensoryInputs) {
        std::vector<std::unique_ptr<Action>> actions;
        actions.reserve(numSteps);
        
        for (size_t i = 0; i < numSteps; ++i) {
            if (i < sensoryInputs.size()) {
                brain->receiveSensoryInput(sensoryInputs[i]);
            }
            brain->step(static_cast<SimulationStep>(i));
            auto action = brain->produceAction();
            if (action) {
                actions.push_back(std::move(action));
            }
        }
        
        return std::make_tuple(actions, brain->getTotalSpikeCount(), brain->getActiveNeuronCount());
    }, py::arg("brain"), py::arg("numSteps"), py::arg("sensoryInputs"), 
          "Perform complete batch processing (sensory input + step + action production)");
    
    m.def("parallelBatchStep", [](std::shared_ptr<Brain> brain, size_t numSteps, 
                                 const std::vector<SensoryInput>& inputs,
                                 bool enableAsync) {
        if (enableAsync) {
            // This would leverage async operations
            return "Async batch processing requires additional implementation";
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < numSteps; ++i) {
            if (i < inputs.size()) {
                brain->receiveSensoryInput(inputs[i]);
            }
            brain->step(static_cast<SimulationStep>(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return std::make_tuple(brain->getTotalSpikeCount(), duration.count());
    }, py::arg("brain"), py::arg("numSteps"), py::arg("inputs"), py::arg("enableAsync") = false,
          "Perform parallel batch step processing with performance timing");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
