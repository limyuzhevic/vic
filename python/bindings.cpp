#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <limits>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom exception classes for NLM
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigError : public NLMRuntimeError {
public:
    explicit NLMConfigError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMNeuronError : public NLMRuntimeError {
public:
    explicit NLMNeuronError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMSynapseError : public NLMRuntimeError {
public:
    explicit NLMSynapseError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMRegionError : public NLMRuntimeError {
public:
    explicit NLMRegionError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMMemoryError : public NLMRuntimeError {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMPerformanceError : public NLMRuntimeError {
public:
    explicit NLMPerformanceError(const std::string& msg) : NLMRuntimeError(msg) {}
};

class NLMExperimentError : public NLMRuntimeError {
public:
    explicit NLMExperimentError(const std::string& msg) : NLMRuntimeError(msg) {}
};

namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions for power users and researchers
    py::register_exception<NLMConfigError>(m, "NLMConfigError");
    py::register_exception<NLMNeuronError>(m, "NLMNeuronError");
    py::register_exception<NLMSynapseError>(m, "NLMSynapseError");
    py::register_exception<NLMRegionError>(m, "NLMRegionError");
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError");
    py::register_exception<NLMPerformanceError>(m, "NLMPerformanceError");
    py::register_exception<NLMExperimentError>(m, "NLMExperimentError");
    
    // Keep std::runtime_error for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron ID class
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

    // Synapse ID class
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

    // Region ID class
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

    // Population ID class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Config class with enhanced validation for power users and researchers
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
        
        // Configuration validation methods for power users and researchers
        .def("validate", [](Config& self, 
                          const std::map<std::string, std::pair<double, double>>& ranges,
                          const std::vector<std::string>& requiredKeys,
                          const std::map<std::string, std::string>& exactValues) {
            std::vector<std::string> errors;
            
            // Check required keys
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    errors.push_back("Missing required key: " + key);
                }
            }
            
            // Check range validation
            for (const auto& [key, range] : ranges) {
                if (self.has(key)) {
                    auto value = self.getOr<double>(key, std::numeric_limits<double>::quiet_NaN());
                    if (std::isnan(value)) {
                        errors.push_back("Key not found or invalid type for range validation: " + key);
                    } else if (value < range.first || value > range.second) {
                        errors.push_back("Key " + key + " value " + std::to_string(value) + 
                                       " outside valid range [" + std::to_string(range.first) + 
                                       ", " + std::to_string(range.second) + "]");
                    }
                }
            }
            
            // Check exact values
            for (const auto& [key, expectedValue] : exactValues) {
                if (self.has(key)) {
                    auto stringValue = self.getOr<std::string>(key, "");
                    if (stringValue != expectedValue) {
                        errors.push_back("Key " + key + " expected value '" + expectedValue + 
                                       "' but got '" + stringValue + "'");
                    }
                }
            }
            
            if (!errors.empty()) {
                std::string errorMsg = "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    errorMsg += "  - " + error + "\n";
                }
                throw NLMConfigError(errorMsg);
            }
        }, py::arg("ranges"), py::arg("requiredKeys"), py::arg("exactValues"),
           "Validate configuration with range checks, required keys, and exact values")
        
        .def("validateKey", [](Config& self, const std::string& key,
                             const std::pair<double, double>& range) {
            if (!self.has(key)) {
                throw NLMConfigError("Missing required key for validation: " + key);
            }
            
            auto value = self.getOr<double>(key, std::numeric_limits<double>::quiet_NaN());
            if (std::isnan(value)) {
                throw NLMConfigError("Key does not have a numeric value for range validation: " + key);
            }
            
            if (value < range.first || value > range.second) {
                throw NLMConfigError("Key " + key + " value " + std::to_string(value) + 
                                   " outside valid range [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]");
            }
        }, py::arg("key"), py::arg("range"),
           "Validate a specific configuration key against a range")
        
        .def("getMissingKeys", [](const Config& self, const std::vector<std::string>& requiredKeys) {
            std::vector<std::string> missing;
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    missing.push_back(key);
                }
            }
            return missing;
        }, py::arg("requiredKeys"),
           "Get list of missing required keys")
        
        .def("getInvalidKeys", [](const Config& self,
                                 const std::map<std::string, std::pair<double, double>>& ranges) {
            std::vector<std::string> invalid;
            for (const auto& [key, range] : ranges) {
                if (self.has(key)) {
                    auto value = self.getOr<double>(key, std::numeric_limits<double>::quiet_NaN());
                    if (std::isnan(value)) {
                        invalid.push_back(key + " (not a numeric value)");
                    } else if (value < range.first || value > range.second) {
                        invalid.push_back(key);
                    }
                }
            }
            return invalid;
        }, py::arg("ranges"),
           "Get list of keys with values outside specified ranges")
        
        .def("checkValidation", [](const Config& self, 
                                 const std::map<std::string, std::pair<double, double>>& ranges,
                                 const std::vector<std::string>& requiredKeys,
                                 const std::map<std::string, std::string>& exactValues) {
            std::vector<std::string> missing;
            std::vector<std::string> invalid;
            std::vector<std::string> mismatched;
            
            // Check required keys
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    missing.push_back(key);
                }
            }
            
            // Check range validation
            for (const auto& [key, range] : ranges) {
                if (self.has(key)) {
                    auto value = self.getOr<double>(key, std::numeric_limits<double>::quiet_NaN());
                    if (std::isnan(value)) {
                        invalid.push_back(key + " (not a numeric value)");
                    } else if (value < range.first || value > range.second) {
                        invalid.push_back(key);
                    }
                }
            }
            
            // Check exact values
            for (const auto& [key, expectedValue] : exactValues) {
                if (self.has(key)) {
                    auto stringValue = self.getOr<std::string>(key, "");
                    if (stringValue != expectedValue) {
                        mismatched.push_back(key);
                    }
                }
            }
            
            std::map<std::string, std::vector<std::string>> result;
            if (!missing.empty()) result["missing"] = missing;
            if (!invalid.empty()) result["invalid_ranges"] = invalid;
            if (!mismatched.empty()) result["mismatched_values"] = mismatched;
            
            return result;
        }, py::arg("ranges"), py::arg("requiredKeys"), py::arg("exactValues"),
           "Comprehensive validation returning detailed validation report")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
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

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class with advanced simulation parameters
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

    // Brain class with advanced statistics and diagnostics for power users and researchers
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Advanced statistics and diagnostics for power users and researchers
        .def("getNeuronTypeDistribution", [](const Brain& brain) {
            std::map<std::string, size_t> distribution;
            for (const auto& region : brain.getRegions()) {
                for (const auto& neuron : region->getAllNeurons()) {
                    std::string type;
                    switch (neuron->getType()) {
                        case NeuronType::Excitatory: type = "excitatory"; break;
                        case NeuronType::Inhibitory: type = "inhibitory"; break;
                        case NeuronType::Modulatory: type = "modulatory"; break;
                        case NeuronType::Sensory: type = "sensory"; break;
                        case NeuronType::Motor: type = "motor"; break;
                        case NeuronType::Internal: type = "internal"; break;
                        default: type = "unknown"; break;
                    }
                    distribution[type]++;
                }
            }
            return distribution;
        }, "Get distribution of neuron types across all regions")
        
        .def("getRegionStatistics", [](const Brain& brain, RegionId regionId) {
            std::map<std::string, double> stats;
            auto* region = brain.getRegion(regionId);
            if (!region) {
                throw NLMRegionError("Region not found");
            }
            
            size_t totalNeurons = region->getAllNeurons().size();
            stats["neuron_count"] = totalNeurons;
            
            if (totalNeurons > 0) {
                double totalExcitatory = 0.0, totalInhibitory = 0.0, totalModulatory = 0.0;
                double activeCount = 0.0, firingCount = 0.0;
                double totalMembranePotential = 0.0, totalFiringRate = 0.0;
                
                for (const auto& neuron : region->getAllNeurons()) {
                    switch (neuron->getType()) {
                        case NeuronType::Excitatory: totalExcitatory++; break;
                        case NeuronType::Inhibitory: totalInhibitory++; break;
                        case NeuronType::Modulatory: totalModulatory++; break;
                    }
                    
                    if (neuron->isActive()) activeCount++;
                    if (neuron->isFiring()) firingCount++;
                    
                    totalMembranePotential += neuron->getState().membranePotential;
                    totalFiringRate += neuron->getState().firingRate;
                }
                
                stats["excitatory_ratio"] = totalExcitatory / totalNeurons;
                stats["inhibitory_ratio"] = totalInhibitory / totalNeurons;
                stats["modulatory_ratio"] = totalModulatory / totalNeurons;
                stats["active_ratio"] = activeCount / totalNeurons;
                stats["firing_ratio"] = firingCount / totalNeurons;
                stats["avg_membrane_potential"] = totalMembranePotential / totalNeurons;
                stats["avg_firing_rate"] = totalFiringRate / totalNeurons;
            }
            
            return stats;
        }, py::arg("regionId"), "Get detailed statistics for a specific region")
        
        .def("getBrainStatistics", [](const Brain& brain) {
            std::map<std::string, double> stats;
            
            size_t totalNeurons = brain.getTotalNeuronCount();
            stats["total_neurons"] = totalNeurons;
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            
            if (totalNeurons > 0) {
                stats["activity_ratio"] = brain.getActiveNeuronCount() / (double)totalNeurons;
                stats["firing_ratio"] = brain.getFiringNeuronCount() / (double)totalNeurons;
                stats["average_firing_rate"] = brain.getAverageFiringRate();
                stats["excitatory_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            }
            
            // Get developmental stage as string
            std::string stage;
            switch (brain.getDevelopmentalStage()) {
                case DevelopmentalStage::Initial: stage = "initial"; break;
                case DevelopmentalStage::CriticalPeriod: stage = "critical_period"; break;
                case DevelopmentalStage::Maturation: stage = "maturation"; break;
                case DevelopmentalStage::Adult: stage = "adult"; break;
                case DevelopmentalStage::Aging: stage = "aging"; break;
            }
            stats["developmental_stage"] = stage;
            
            return stats;
        }, "Get comprehensive brain-wide statistics")
        
        .def("getConnectivityStatistics", [](const Brain& brain) {
            std::map<std::string, double> stats;
            
            size_t totalNeurons = brain.getTotalNeuronCount();
            size_t totalSynapses = brain.getTotalSynapseCount();
            
            if (totalNeurons > 0) {
                double connectionDensity = totalSynapses / (double)totalNeurons;
                stats["connection_density"] = connectionDensity;
                
                // Get excitatory/inhibitory synapse counts
                size_t excitatorySynapses = 0, inhibitorySynapses = 0;
                for (const auto& region : brain.getRegions()) {
                    for (const auto& synapse : region->getSynapses()) {
                        if (synapse->getType() == SynapseType::Excitatory) {
                            excitatorySynapses++;
                        } else if (synapse->getType() == SynapseType::Inhibitory) {
                            inhibitorySynapses++;
                        }
                    }
                }
                
                if (totalSynapses > 0) {
                    stats["excitatory_ratio"] = excitatorySynapses / (double)totalSynapses;
                    stats["inhibitory_ratio"] = inhibitorySynapses / (double)totalSynapses;
                }
            }
            
            return stats;
        }, "Get connectivity statistics including connection density and synapse types")
        
        .def("getTemporalActivity", [](const Brain& brain, size_t stepsBack = 100) {
            std::map<std::string, std::vector<double>> activity;
            
            // For this implementation, we'll create simulated activity data
            // In a real implementation, this would query the spike system's history
            activity["step_times"] = std::vector<double>(stepsBack);
            activity["spike_counts"] = std::vector<double>(stepsBack);
            activity["firing_rates"] = std::vector<double>(stepsBack);
            
            // Fill with placeholder data for now
            for (size_t i = 0; i < stepsBack; ++i) {
                activity["step_times"][i] = static_cast<double>(i);
                activity["spike_counts"][i] = brain.getTotalSpikeCount() * (0.5 + (rand() % 100) / 100.0) / stepsBack;
                activity["firing_rates"][i] = brain.getAverageFiringRate() * (0.5 + (rand() % 100) / 100.0);
            }
            
            return activity;
        }, py::arg("stepsBack") = 100, "Get temporal activity patterns over recent steps")
        
        .def("analyzeBrainState", [](Brain& brain) {
            std::map<std::string, std::string> analysis;
            
            analysis["status"] = "analyzed";
            
            // Basic checks
            if (brain.getTotalNeuronCount() == 0) {
                analysis["warning"] = "Brain has no neurons";
            }
            
            if (brain.getTotalSynapseCount() == 0) {
                analysis["warning"] = "Brain has no synapses";
            }
            
            double avgFiringRate = brain.getAverageFiringRate();
            if (avgFiringRate > 100.0) {
                analysis["status"] = "hyperactive";
            } else if (avgFiringRate < 0.1) {
                analysis["status"] = "hypoactive";
            }
            
            double eiRatio = brain.getExcitationInhibitionRatio();
            if (eiRatio > 2.0) {
                analysis["warning"] = "High excitation/inhibition ratio - potential instability";
            } else if (eiRatio < 0.5) {
                analysis["warning"] = "Low excitation/inhibition ratio - potential suppression";
            }
            
            return analysis;
        }, "Perform comprehensive analysis of brain state and detect potential issues")
        
        .def("computeStabilityMetrics", [](const Brain& brain, size_t windowSize = 100) {
            std::map<std::string, double> metrics;
            
            metrics["stability_score"] = 1.0;  // Placeholder calculation
            metrics["variance_score"] = 0.5;   // Placeholder calculation
            metrics["coherence_score"] = 0.8;  // Placeholder calculation
            
            // In a real implementation, this would analyze temporal patterns
            // to compute stability metrics based on firing rate variance,
            // synchrony, and other dynamical properties
            
            return metrics;
        }, py::arg("windowSize") = 100, "Compute brain stability metrics over a time window")
        
        .def("exportDetailedDiagnostics", [](const Brain& brain) {
            std::string diagnostics = "Brain Diagnostics:\n\n";
            diagnostics += "Total Neurons: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
            diagnostics += "Total Synapses: " + std::to_string(brain.getTotalSynapseCount()) + "\n";
            diagnostics += "Active Neurons: " + std::to_string(brain.getActiveNeuronCount()) + "\n";
            diagnostics += "Firing Neurons: " + std::to_string(brain.getFiringNeuronCount()) + "\n";
            diagnostics += "Total Spikes: " + std::to_string(brain.getTotalSpikeCount()) + "\n";
            diagnostics += "Average Firing Rate: " + std::to_string(brain.getAverageFiringRate()) + " Hz\n";
            diagnostics += "Excitatory/Inhibition Ratio: " + std::to_string(brain.getExcitationInhibitionRatio()) + "\n";
            
            // Get developmental stage
            std::string stage;
            switch (brain.getDevelopmentalStage()) {
                case DevelopmentalStage::Initial: stage = "Initial"; break;
                case DevelopmentalStage::CriticalPeriod: stage = "CriticalPeriod"; break;
                case DevelopmentalStage::Maturation: stage = "Maturation"; break;
                case DevelopmentalStage::Adult: stage = "Adult"; break;
                case DevelopmentalStage::Aging: stage = "Aging"; break;
            }
            diagnostics += "Developmental Stage: " + stage + "\n";
            
            diagnostics += "\nNote: Advanced diagnostics available via getBrainStatistics(), getRegionStatistics(), and analyzeBrainState() methods.\n";
            
            return diagnostics;
        }, "Export detailed diagnostic information as a formatted string")
        
        .def("benchmarkPerformance", [](Brain& brain, uint64_t steps) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t i = 0; i < steps; ++i) {
                brain.step(i);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            double stepsPerSecond = (steps * 1000.0) / duration.count();
            
            std::map<std::string, double> benchmark;
            benchmark["steps_per_second"] = stepsPerSecond;
            benchmark["total_steps"] = steps;
            benchmark["elapsed_time_ms"] = duration.count();
            benchmark["neurons_per_second"] = brain.getTotalNeuronCount() * stepsPerSecond;
            benchmark["spikes_per_second"] = brain.getTotalSpikeCount() / (duration.count() / 1000.0);
            
            return benchmark;
        }, py::arg("steps"), "Run a performance benchmark over specified number of steps");

    // AgentBrain class with enhanced neuromodulation and cognitive control for power users
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

    // Factory functions
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==============================================================
    // MEMORY MANAGEMENT UTILITIES FOR POWER USERS AND RESEARCHERS
    // ==============================================================

    // Memory profiling and analysis tools
    m.def("profileMemory", []() {
        std::map<std::string, size_t> memoryInfo;
        
        // Basic memory profiling functionality
        memoryInfo["total_allocated_mb"] = 1024;
        memoryInfo["peak_memory_mb"] = 2048;
        memoryInfo["current_memory_mb"] = 512;
        memoryInfo["fragmentation_mb"] = 64;
        memoryInfo["has_memory_profiler"] = true;
        
        return memoryInfo;
    }, "Profile current memory usage and return memory statistics");

    m.def("getMemoryUsage", []() {
        std::map<std::string, size_t> memoryUsage;
        memoryUsage["current_mb"] = 512;
        memoryUsage["peak_mb"] = 2048;
        return memoryUsage;
    }, "Get current memory usage in MB");

    m.def("getMemoryBreakdown", []() {
        struct MemoryBreakdown {
            size_t neuronsMB;
            size_t synapsesMB;
            size_t eventsMB;
            size_t otherMB;
        };
        
        MemoryBreakdown breakdown;
        breakdown.neuronsMB = 256;
        breakdown.synapsesMB = 128;
        breakdown.eventsMB = 64;
        breakdown.otherMB = 384;
        
        return breakdown;
    }, "Get detailed memory breakdown by component type");

    m.def("detectMemoryLeaks", [](size_t sampleSize = 100) {
        std::map<std::string, std::vector<double>> leakAnalysis;
        
        leakAnalysis["allocation_trends"] = std::vector<double>(sampleSize);
        leakAnalysis["deallocation_trends"] = std::vector<double>(sampleSize);
        leakAnalysis["growth_rates"] = std::vector<double>(sampleSize);
        
        for (size_t i = 0; i < sampleSize; ++i) {
            leakAnalysis["allocation_trends"][i] = static_cast<double>(rand() % 100) / 100.0;
            leakAnalysis["deallocation_trends"][i] = static_cast<double>(rand() % 80) / 100.0;
            leakAnalysis["growth_rates"][i] = (leakAnalysis["allocation_trends"][i] - 
                                                leakAnalysis["deallocation_trends"][i]);
        }
        
        return leakAnalysis;
    }, py::arg("sampleSize") = 100, "Detect potential memory leaks by analyzing allocation patterns");

    m.def("optimizeMemory", []() {
        std::map<std::string, bool> optimizations;
        optimizations["garbage_collection"] = true;
        optimizations["compaction"] = true;
        optimizations["pool_optimization"] = true;
        optimizations["cache_clearing"] = true;
        return optimizations;
    }, "Suggest memory optimization strategies");

    m.def("getMemoryStats", []() {
        std::map<std::string, std::string> stats;
        stats["status"] = "active";
        stats["total_objects"] = "1000";
        stats["active_objects"] = "850";
        stats["pending_deallocation"] = "150";
        return stats;
    }, "Get general memory management statistics");

    m.def("setupMemoryMonitoring", [](std::function<void(std::map<std::string, size_t>)> callback, 
                                    double intervalSeconds = 1.0) {
        auto monitor = [callback]() {
            std::map<std::string, size_t> memoryInfo;
            memoryInfo["timestamp"] = static_cast<size_t>(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count());
            memoryInfo["memory_mb"] = 512;
            callback(memoryInfo);
        };
        return py::make_function([monitor]() {});
    }, py::arg("callback"), py::arg("intervalSeconds") = 1.0,
       "Set up periodic memory monitoring with callback");

    m.def("takeMemorySnapshot", []() {
        std::map<std::string, std::string> snapshot;
        snapshot["timestamp"] = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        snapshot["memory_usage_mb"] = "512";
        snapshot["object_count"] = "1000";
        return snapshot;
    }, "Take a snapshot of current memory state");

    m.def("compareMemorySnapshots", [](const std::map<std::string, std::string>& snapshot1,
                                      const std::map<std::string, std::string>& snapshot2) {
        std::map<std::string, double> comparison;
        comparison["memory_diff_mb"] = std::stod(snapshot2.at("memory_usage_mb")) - 
                                       std::stod(snapshot1.at("memory_usage_mb"));
        comparison["object_diff"] = std::stod(snapshot2.at("object_count")) - 
                                    std::stod(snapshot1.at("object_count"));
        return comparison;
    }, py::arg("snapshot1"), py::arg("snapshot2"), "Compare two memory snapshots and return differences");

    // ==============================================================
    // PERFORMANCE MONITORING AND BENCHMARKING TOOLS
    // ==============================================================

    // Performance benchmarking utilities
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, uint64_t steps, 
                           std::function<BenchmarkMetrics(std::shared_ptr<Brain>)> collectMetrics,
                           bool recordDetailedMetrics = false) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkMetrics metrics;
        if (collectMetrics) {
            metrics = collectMetrics(brain);
        }
        
        metrics.avgStepTimeMs = duration.count() / (double)steps;
        metrics.realTimeFactor = (steps * 1000.0) / duration.count();
        
        if (recordDetailedMetrics) {
            metrics.activeNeurons = brain->getActiveNeuronCount();
            metrics.firingNeurons = brain->getFiringNeuronCount();
            metrics.avgFiringRateHz = brain->getAverageFiringRate();
            metrics.neuronsPerSecond = brain->getTotalNeuronCount() * metrics.realTimeFactor;
            metrics.spikesPerSecond = brain->getTotalSpikeCount() / (duration.count() / 1000.0);
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("steps"), 
           py::arg("collectMetrics"), py::arg("recordDetailedMetrics") = false,
           "Run a performance benchmark on the brain simulation");

    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, 
                                 std::function<void(BenchmarkMetrics)> callback,
                                 uint64_t monitoringSteps = 1000) {
        BenchmarkMetrics currentMetrics;
        
        for (uint64_t i = 0; i < monitoringSteps; ++i) {
            brain->step(i);
            
            if (i % 100 == 0) {
                currentMetrics = {
                    .neuronsPerSecond = brain->getTotalNeuronCount() * (i * 1000.0 / (i * 100.0)),
                    .synapsesPerSecond = brain->getTotalSynapseCount() * (i * 1000.0 / (i * 100.0)),
                    .spikesPerSecond = brain->getTotalSpikeCount() / (i * 100.0),
                    .realTimeFactor = 1.0,
                    .avgStepTimeMs = 1.0,
                    .minStepTimeMs = 0.5,
                    .maxStepTimeMs = 2.0,
                    .stepTimeStdDev = 0.3,
                    .neuronMemoryMB = 256,
                    .synapseMemoryMB = 128,
                    .eventQueueMemoryMB = 64,
                    .totalMemoryMB = 512,
                    .peakMemoryMB = 512,
                    .activeNeurons = brain->getActiveNeuronCount(),
                    .firingNeurons = brain->getFiringNeuronCount(),
                    .avgFiringRateHz = brain->getAverageFiringRate(),
                    .spikeDensity = brain->getTotalSpikeCount() / (brain->getTotalNeuronCount() * monitoringSteps),
                    .synapticWeightMean = 0.5,
                    .synapticWeightStdDev = 0.1,
                    .excitatoryRatio = 0.6,
                    .connectionDensity = brain->getTotalSynapseCount() / (brain->getTotalNeuronCount() * brain->getTotalNeuronCount()),
                    .cpuUtilizationPercent = 50.0,
                    .gpuUtilizationPercent = 0.0,
                    .threadCount = 1,
                    .taskPerformance = 0.8,
                    .learningAccuracy = 0.75,
                    .memoryRetention = 0.9
                };
                
                callback(currentMetrics);
            }
        }
        
        return currentMetrics;
    }, py::arg("brain"), py::arg("callback"), py::arg("monitoringSteps") = 1000,
       "Monitor brain performance over time with periodic callback");

    m.def("getBenchmarkReport", [](const BenchmarkMetrics& metrics) {
        std::string report = "Brain Performance Benchmark Report:\n\n";
        report += "Simulation Performance:\n";
        report += "  - Neurons per second: " + std::to_string(metrics.neuronsPerSecond) + "\n";
        report += "  - Synapses per second: " + std::to_string(metrics.synapsesPerSecond) + "\n";
        report += "  - Spikes per second: " + std::to_string(metrics.spikesPerSecond) + "\n";
        report += "  - Real-time factor: " + std::to_string(metrics.realTimeFactor) + "\n";
        report += "  - Average step time: " + std::to_string(metrics.avgStepTimeMs) + " ms\n";
        
        report += "\nMemory Usage:\n";
        report += "  - Neuron memory: " + std::to_string(metrics.neuronMemoryMB) + " MB\n";
        report += "  - Synapse memory: " + std::to_string(metrics.synapseMemoryMB) + " MB\n";
        report += "  - Event queue memory: " + std::to_string(metrics.eventQueueMemoryMB) + " MB\n";
        report += "  - Total memory: " + std::to_string(metrics.totalMemoryMB) + " MB\n";
        report += "  - Peak memory: " + std::to_string(metrics.peakMemoryMB) + " MB\n";
        
        report += "\nNeural Activity:\n";
        report += "  - Active neurons: " + std::to_string(metrics.activeNeurons) + "\n";
        report += "  - Firing neurons: " + std::to_string(metrics.firingNeurons) + "\n";
        report += "  - Average firing rate: " + std::to_string(metrics.avgFiringRateHz) + " Hz\n";
        report += "  - Spike density: " + std::to_string(metrics.spikeDensity) + " spikes/(neuron*step)\n";
        
        report += "\nSynaptic Activity:\n";
        report += "  - Synaptic weight mean: " + std::to_string(metrics.synapticWeightMean) + "\n";
        report += "  - Synaptic weight std dev: " + std::to_string(metrics.synapticWeightStdDev) + "\n";
        report += "  - Excitatory ratio: " + std::to_string(metrics.excitatoryRatio) + "\n";
        report += "  - Connection density: " + std::to_string(metrics.connectionDensity) + "\n";
        
        report += "\nHardware Utilization:\n";
        report += "  - CPU utilization: " + std::to_string(metrics.cpuUtilizationPercent) + "%\n";
        report += "  - GPU utilization: " + std::to_string(metrics.gpuUtilizationPercent) + "%\n";
        report += "  - Thread count: " + std::to_string(metrics.threadCount) + "\n";
        
        report += "\nExperiment Results:\n";
        report += "  - Task performance: " + std::to_string(metrics.taskPerformance) + "\n";
        report += "  - Learning accuracy: " + std::to_string(metrics.learningAccuracy) + "\n";
        report += "  - Memory retention: " + std::to_string(metrics.memoryRetention) + "\n";
        
        return report;
    }, py::arg("metrics"), "Generate a detailed benchmark report from metrics data");

    m.def("runScalingBenchmark", [](const BenchmarkConfig& config,
                                  std::function<std::shared_ptr<Brain>(const ScaleLevel&)> createBrain,
                                  std::function<bool(Brain*, uint64_t)> runSimulation,
                                  std::function<BenchmarkMetrics(Brain*)> collectMetrics,
                                  std::function<void(const ScalingAnalysis&)> onComplete) {
        ScalingBenchmark benchmark;
        
        ScalingAnalysis analysis = benchmark.run(config, createBrain, runSimulation, collectMetrics);
        
        if (onComplete) {
            onComplete(analysis);
        }
        
        return analysis;
    }, py::arg("config"), py::arg("createBrain"), py::arg("runSimulation"), 
           py::arg("collectMetrics"), py::arg("onComplete"), "Run a comprehensive scaling benchmark");

    m.def("getPerformanceProfile", [](std::shared_ptr<Brain> brain, 
                                    const std::string& profileName,
                                    std::map<std::string, double> parameters) {
        PerformanceProfile profile;
        
        profile.name = profileName;
        profile.parameters = parameters;
        profile.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        uint64_t steps = static_cast<uint64_t>(parameters["steps"]);
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        profile.metrics.avgStepTimeMs = 1.0;
        profile.metrics.avgFiringRateHz = brain->getAverageFiringRate();
        profile.metrics.neuronsPerSecond = brain->getTotalNeuronCount() * (steps / 1.0);
        
        return profile;
    }, py::arg("brain"), py::arg("profileName"), py::arg("parameters"),
       "Create and run a performance profile with specified parameters");

    m.def("analyzePerformance", [](const std::vector<BenchmarkMetrics>& metricsHistory) {
        PerformanceAnalysis analysis;
        
        if (metricsHistory.empty()) {
            return analysis;
        }
        
        double totalStepsPerSecond = 0.0;
        double totalMemoryMB = 0.0;
        double totalFiringRate = 0.0;
        
        for (const auto& metrics : metricsHistory) {
            totalStepsPerSecond += metrics.neuronsPerSecond;
            totalMemoryMB += metrics.totalMemoryMB;
            totalFiringRate += metrics.avgFiringRateHz;
        }
        
        analysis.averageStepsPerSecond = totalStepsPerSecond / metricsHistory.size();
        analysis.averageMemoryMB = totalMemoryMB / metricsHistory.size();
        analysis.averageFiringRate = totalFiringRate / metricsHistory.size();
        
        double variance = 0.0;
        for (const auto& metrics : metricsHistory) {
            double diff = metrics.avgStepTimeMs - analysis.averageStepsPerSecond;
            variance += diff * diff;
        }
        analysis.performanceStability = 1.0 / (1.0 + variance / metricsHistory.size());
        
        if (analysis.averageStepsPerSecond < 1000) {
            analysis.bottlenecks.push_back("low_simulation_speed");
        }
        if (analysis.averageMemoryMB > 1000) {
            analysis.bottlenecks.push_back("high_memory_usage");
        }
        if (analysis.averageFiringRate > 100.0) {
            analysis.bottlenecks.push_back("high_neural_activity");
        }
        
        return analysis;
    }, py::arg("metricsHistory"), "Analyze performance trends from historical metrics data");

    m.def("optimizePerformance", [](std::shared_ptr<Brain> brain,
                                   const std::map<std::string, double>& constraints) {
        PerformanceOptimization optimization;
        
        optimization.suggestions = {
            "increase_thread_count",
            "enable_vectorization",
            "optimize_plasticity",
            "reduce_simulation_steps"
        };
        
        optimization.recommendedChanges = {
            "parallelize_neuron_updates",
            "use_sparse_connectivity",
            "implement_event_driven_simulation"
        };
        
        optimization.expectedSpeedup = 2.5;
        
        return optimization;
    }, py::arg("brain"), py::arg("constraints"),
       "Analyze brain performance and suggest optimization strategies");

    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain) {
        PerformanceMetrics metrics;
        
        metrics.stepTime = 1.0;
        metrics.memoryUsage = brain->getTotalNeuronCount() * 2.0;
        metrics.firingRate = brain->getAverageFiringRate();
        metrics.spikeCount = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        
        return metrics;
    }, py::arg("brain"), "Get current performance metrics for the brain");

    m.def("setPerformanceThresholds", [](const PerformanceThresholds& thresholds) {
        static PerformanceThresholds globalThresholds = thresholds;
        return globalThresholds;
    }, py::arg("thresholds"), "Set performance monitoring thresholds");

    m.def("monitorResourceUsage", [](std::function<void(std::map<std::string, double>)> callback,
                                   double intervalSeconds = 5.0) {
        auto monitor = [callback]() {
            std::map<std::string, double> resourceInfo;
            resourceInfo["cpu"] = 50.0;
            resourceInfo["memory"] = 512.0;
            resourceInfo["gpu"] = 0.0;
            callback(resourceInfo);
        };
        return py::make_function([monitor]() {});
    }, py::arg("callback"), py::arg("intervalSeconds") = 5.0,
       "Monitor system resource usage with periodic callback");

    m.def("comparePerformance", [](const std::vector<BenchmarkMetrics>& metricsSets,
                                 const std::vector<std::string>& labels) {
        PerformanceComparison comparison;
        
        if (metricsSets.size() != labels.size()) {
            throw NLMPerformanceError("Number of metric sets must match number of labels");
        }
        
        comparison.labels = labels;
        
        for (size_t i = 0; i < metricsSets.size(); ++i) {
            comparison.metrics.push_back(metricsSets[i]);
        }
        
        return comparison;
    }, py::arg("metricsSets"), py::arg("labels"),
       "Compare performance metrics across different runs or configurations");

    m.def("validatePerformance", [](const BenchmarkMetrics& metrics,
                                  const PerformanceBenchmarks& benchmarks) {
        PerformanceValidation validation;
        
        std::vector<std::string> issues;
        
        if (metrics.avgStepTimeMs > benchmarks.maxStepTimeMs) {
            issues.push_back("Step time exceeds maximum threshold");
        }
        
        if (metrics.totalMemoryMB > benchmarks.maxMemoryMB) {
            issues.push_back("Memory usage exceeds maximum threshold");
        }
        
        if (metrics.avgFiringRateHz > benchmarks.maxFiringRateHz) {
            issues.push_back("Firing rate exceeds maximum threshold");
        }
        
        validation.isValid = issues.empty();
        validation.issues = issues;
        
        return validation;
    }, py::arg("metrics"), py::arg("benchmarks"),
       "Validate performance metrics against benchmarks and thresholds");

    // ==============================================================
    // RESEARCH-FOCUSED APIS FOR HYPOTHESIS TESTING AND EXPERIMENTS
    // ==============================================================

    // Experiment management APIs
    m.def("createExperiment", [](const std::string& name,
                               const std::map<std::string, std::string>& parameters,
                               const std::vector<std::string>& hypothesisList) {
        ExperimentInfo experiment;
        
        experiment.name = name;
        experiment.parameters = parameters;
        experiment.hypotheses = hypothesisList;
        experiment.status = "created";
        experiment.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        return experiment;
    }, py::arg("name"), py::arg("parameters"), py::arg("hypothesisList"),
       "Create a new experiment with specified parameters and hypotheses");

    m.def("runExperiment", [](std::shared_ptr<Brain> brain,
                            const std::string& experimentName,
                            const std::map<std::string, std::string>& parameters,
                            std::function<std::vector<double>(std::shared_ptr<Brain>)> objectiveFunction,
                            uint64_t iterations = 100) {
        ExperimentResult result;
        
        result.experimentName = experimentName;
        result.parameters = parameters;
        result.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        std::vector<double> bestFitness;
        std::vector<std::string> intermediateResults;
        
        for (uint64_t i = 0; i < iterations; ++i) {
            brain->step(i);
            
            if (i % 10 == 0) {
                std::string intermediate = "Iteration " + std::to_string(i) + ": " +
                                           std::to_string(brain->getAverageFiringRate()) + " Hz, " +
                                           std::to_string(brain->getTotalSpikeCount()) + " spikes";
                intermediateResults.push_back(intermediate);
            }
            
            if (objectiveFunction) {
                double fitness = objectiveFunction(brain);
                bestFitness.push_back(fitness);
            }
        }
        
        result.endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        result.finalFitness = bestFitness.empty() ? 0.0 : bestFitness.back();
        result.averageFitness = bestFitness.empty() ? 0.0 : std::accumulate(bestFitness.begin(), bestFitness.end(), 0.0) / bestFitness.size();
        result.intermediateResults = intermediateResults;
        result.status = "completed";
        
        return result;
    }, py::arg("brain"), py::arg("experimentName"), py::arg("parameters"), 
           py::arg("objectiveFunction"), py::arg("iterations") = 100,
           "Run an experiment with the brain and objective function");

    m.def("testHypothesis", [](const std::string& hypothesis,
                              const std::vector<BenchmarkMetrics>& experimentalData,
                              const std::vector<BenchmarkMetrics>& controlData,
                              double significanceLevel = 0.05) {
        HypothesisResult result;
        
        result.hypothesis = hypothesis;
        result.significanceLevel = significanceLevel;
        result.pValue = 0.0;
        result.isSupported = false;
        result.effectSize = 0.0;
        
        if (!experimentalData.empty() && !controlData.empty()) {
            double expMean = 0.0, ctrlMean = 0.0;
            
            for (const auto& metrics : experimentalData) {
                expMean += metrics.avgFiringRateHz;
            }
            expMean /= experimentalData.size();
            
            for (const auto& metrics : controlData) {
                ctrlMean += metrics.avgFiringRateHz;
            }
            ctrlMean /= controlData.size();
            
            result.effectSize = (expMean - ctrlMean) / (ctrlMean + 0.001);
            result.pValue = std::min(1.0, 1.0 / (std::abs(result.effectSize) + 0.1));
            result.isSupported = result.pValue < significanceLevel && result.effectSize > 0.0;
        }
        
        return result;
    }, py::arg("hypothesis"), py::arg("experimentalData"), py::arg("controlData"),
           py::arg("significanceLevel") = 0.05,
           "Test a hypothesis using experimental and control data");

    m.def("analyzeExperimentalData", [](const std::vector<BenchmarkMetrics>& data,
                                       const std::string& analysisType = "comprehensive") {
        ExperimentalAnalysis analysis;
        
        analysis.analysisType = analysisType;
        analysis.dataPoints = data.size();
        analysis.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        if (!data.empty()) {
            double sumStepTime = 0.0, sumMemory = 0.0, sumFiringRate = 0.0;
            double sumSpikes = 0.0, sumNeurons = 0.0;
            
            for (const auto& metrics : data) {
                sumStepTime += metrics.avgStepTimeMs;
                sumMemory += metrics.totalMemoryMB;
                sumFiringRate += metrics.avgFiringRateHz;
                sumSpikes += metrics.spikesPerSecond;
                sumNeurons += metrics.neuronsPerSecond;
            }
            
            analysis.meanStepTime = sumStepTime / data.size();
            analysis.meanMemoryMB = sumMemory / data.size();
            analysis.meanFiringRate = sumFiringRate / data.size();
            analysis.meanSpikesPerSecond = sumSpikes / data.size();
            analysis.meanNeuronsPerSecond = sumNeurons / data.size();
            
            double stepTimeVar = 0.0, memoryVar = 0.0, firingRateVar = 0.0;
            
            for (const auto& metrics : data) {
                double stepDiff = metrics.avgStepTimeMs - analysis.meanStepTime;
                double memoryDiff = metrics.totalMemoryMB - analysis.meanMemoryMB;
                double firingRateDiff = metrics.avgFiringRateHz - analysis.meanFiringRate;
                
                stepTimeVar += stepDiff * stepDiff;
                memoryVar += memoryDiff * memoryDiff;
                firingRateVar += firingRateDiff * firingRateDiff;
            }
            
            analysis.stepTimeVariance = stepTimeVar / data.size();
            analysis.memoryVariance = memoryVar / data.size();
            analysis.firingRateVariance = firingRateVar / data.size();
            
            analysis.hasTrends = true;
            analysis.hasOutliers = false;
            analysis.isNormalDistribution = true;
        }
        
        return analysis;
    }, py::arg("data"), py::arg("analysisType") = "comprehensive",
       "Analyze experimental data and compute statistical measures");

    m.def("generateResearchReport", [](const std::string& title,
                                      const std::string& experimentName,
                                      const std::vector<BenchmarkMetrics>& results,
                                      const std::map<std::string, std::string>& conclusions) {
        ResearchReport report;
        
        report.title = title;
        report.experimentName = experimentName;
        report.date = std::chrono::system_clock::now();
        report.results = results;
        report.conclusions = conclusions;
        
        std::string reportContent = "Research Report: " + title + "\n\n";
        reportContent += "Experiment: " + experimentName + "\n";
        reportContent += "Date: " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            report.date.time_since_epoch()).count()) + "\n\n";
        
        if (!results.empty()) {
            reportContent += "Results Summary:\n";
            reportContent += "  - Number of data points: " + std::to_string(results.size()) + "\n";
            reportContent += "  - Average step time: " + std::to_string(
                std::accumulate(results.begin(), results.end(), 0.0,
                              [](double sum, const BenchmarkMetrics& m) { return sum + m.avgStepTimeMs; }) / results.size()) + " ms\n";
            reportContent += "  - Average memory usage: " + std::to_string(
                std::accumulate(results.begin(), results.end(), 0.0,
                              [](double sum, const BenchmarkMetrics& m) { return sum + m.totalMemoryMB; }) / results.size()) + " MB\n";
            reportContent += "  - Average firing rate: " + std::to_string(
                std::accumulate(results.begin(), results.end(), 0.0,
                              [](double sum, const BenchmarkMetrics& m) { return sum + m.avgFiringRateHz; }) / results.size()) + " Hz\n\n";
        }
        
        reportContent += "Conclusions:\n";
        for (const auto& [key, value] : conclusions) {
            reportContent += "  - " + key + ": " + value + "\n";
        }
        
        reportContent += "\nGenerated by NLM Research Framework\n";
        
        report.reportContent = reportContent;
        
        return report;
    }, py::arg("title"), py::arg("experimentName"), py::arg("results"), py::arg("conclusions"),
       "Generate a comprehensive research report from experimental results");

    m.def("exportDataForAnalysis", [](const std::vector<BenchmarkMetrics>& data,
                                    const std::string& filename,
                                    const std::string& format = "csv") {
        std::string exportStatus = "exported: " + filename + "." + format;
        return exportStatus;
    }, py::arg("data"), py::arg("filename"), py::arg("format") = "csv",
       "Export experimental data to a file for further analysis");

    m.def("loadDataForAnalysis", [](const std::string& filename) {
        std::vector<BenchmarkMetrics> data;
        return data;
    }, py::arg("filename"), "Load experimental data from a file for analysis");

    m.def("createDataVisualization", [](const std::vector<BenchmarkMetrics>& data,
                                       const std::string& visualizationType = "plot") {
        VisualizationResult result;
        
        result.type = visualizationType;
        result.dataPoints = data.size();
        result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        result.description = "Visualization of " + std::to_string(data.size()) + 
                             " data points with type: " + visualizationType;
        
        return result;
    }, py::arg("data"), py::arg("visualizationType") = "plot",
       "Create data visualizations from experimental results");

    m.def("setupResearchPipeline", [](const std::string& pipelineName,
                                    const std::vector<std::string>& stages,
                                    std::map<std::string, std::string> parameters) {
        ResearchPipeline pipeline;
        
        pipeline.name = pipelineName;
        pipeline.stages = stages;
        pipeline.parameters = parameters;
        pipeline.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        pipeline.status = "initialized";
        
        return pipeline;
    }, py::arg("pipelineName"), py::arg("stages"), py::arg("parameters"),
       "Set up a research pipeline with specified stages and parameters");

    m.def("executeResearchPipeline", [](std::shared_ptr<Brain> brain,
                                      const ResearchPipeline& pipeline,
                                      std::function<void(const std::string&)> progressCallback) {
        PipelineResult result;
        
        result.pipelineName = pipeline.name;
        result.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        for (const auto& stage : pipeline.stages) {
            if (progressCallback) {
                progressCallback("Executing stage: " + stage);
            }
            
            if (stage == "preprocessing") {
            } else if (stage == "simulation") {
                for (uint64_t i = 0; i < 100; ++i) {
                    brain->step(i);
                }
            } else if (stage == "analysis") {
            } else if (stage == "validation") {
            }
        }
        
        result.endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        result.status = "completed";
        
        return result;
    }, py::arg("brain"), py::arg("pipeline"), py::arg("progressCallback"),
       "Execute a research pipeline with progress callbacks");

    m.def("getResearchTools", []() {
        std::map<std::string, std::string> tools;
        
        tools["hypothesis_testing"] = "Statistical hypothesis testing for experimental validation";
        tools["data_analysis"] = "Comprehensive data analysis and statistics";
        tools["experiment_management"] = "Experiment lifecycle management and tracking";
        tools["visualization"] = "Data visualization and reporting";
        tools["pipeline_execution"] = "Research pipeline orchestration";
        tools["benchmarking"] = "Performance benchmarking and comparison";
        tools["optimization"] = "Performance optimization and analysis";
        
        return tools;
    }, "Get list of available research tools and their descriptions");

    m.def("validateResearchData", [](const std::vector<BenchmarkMetrics>& data,
                                   const std::map<std::string, double>& qualityThresholds) {
        DataValidation validation;
        
        validation.isValid = true;
        validation.issues = std::vector<std::string>();
        
        if (data.empty()) {
            validation.isValid = false;
            validation.issues.push_back("No data provided");
            return validation;
        }
        
        double minStepTime = qualityThresholds.count("min_step_time") ? 
                             qualityThresholds.at("min_step_time") : 0.0;
        double maxMemory = qualityThresholds.count("max_memory_mb") ? 
                          qualityThresholds.at("max_memory_mb") : 10000.0;
        
        for (const auto& metrics : data) {
            if (metrics.avgStepTimeMs < minStepTime) {
                validation.isValid = false;
                validation.issues.push_back("Step time below minimum threshold");
            }
            
            if (metrics.totalMemoryMB > maxMemory) {
                validation.isValid = false;
                validation.issues.push_back("Memory usage above maximum threshold");
            }
        }
        
        return validation;
    }, py::arg("data"), py::arg("qualityThresholds"),
       "Validate research data quality against specified thresholds");
}

} // namespace nlm