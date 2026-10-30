// Enhanced NLM Python Bindings with Advanced Features
// Improved type safety, documentation, and comprehensive utilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

// NLM core headers
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

// Configuration validation and import utilities
class ConfigImportHelper {
public:
    static bool validateConfigFile(const std::string& filepath) {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Try to parse as JSON (if available) or key=value format
        std::string line;
        bool hasValidLines = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Check for key=value format
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                hasValidLines = true;
                // Validate key
                std::string key = line.substr(0, pos);
                key = trim(key);
                if (key.empty() || key.find(' ') != std::string::npos) {
                    return false;
                }
                
                // Validate value (basic check)
                std::string value = line.substr(pos + 1);
                value = trim(value);
            }
        }
        
        return hasValidLines;
    }
    
    static std::shared_ptr<Config> createConfigFromDict(
        const std::unordered_map<std::string, std::variant<int, double, bool, std::string>> &dict) {
        auto config = std::make_shared<Config>();
        
        for (const auto& [key, value] : dict) {
            std::visit([&](const auto& val) {
                if constexpr (std::is_same_v<decltype(val), int>) {
                    config->set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<decltype(val), double>) {
                    config->set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<decltype(val), bool>) {
                    config->set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                    config->set(key, val, ConfigSource::Default);
                }
            }, value);
        }
        
        return config;
    }
    
    static std::unordered_map<std::string, std::string> configToDict(
        const std::shared_ptr<Config> &config) {
        std::unordered_map<std::string, std::string> result;
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            // Try different types
            auto intVal = config->get<int>(key);
            if (intVal.has_value()) {
                result[key] = std::to_string(intVal.value());
                continue;
            }
            
            auto doubleVal = config->get<double>(key);
            if (doubleVal.has_value()) {
                result[key] = std::to_string(doubleVal.value());
                continue;
            }
            
            auto boolVal = config->get<bool>(key);
            if (boolVal.has_value()) {
                result[key] = boolVal.value() ? "true" : "false";
                continue;
            }
            
            auto stringVal = config->get<std::string>(key);
            if (stringVal.has_value()) {
                result[key] = stringVal.value();
                continue;
            }
        }
        
        return result;
    }
    
    static std::string configToJSON(const std::shared_ptr<Config> &config) {
        std::ostringstream oss;
        oss << "{\n";
        
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            
            // Try different types
            auto intVal = config->get<int>(key);
            if (intVal.has_value()) {
                oss << "  \"" << key << "\": " << intVal.value();
            } else {
                auto doubleVal = config->get<double>(key);
                if (doubleVal.has_value()) {
                    oss << "  \"" << key << "\": " << doubleVal.value();
                } else {
                    auto boolVal = config->get<bool>(key);
                    if (boolVal.has_value()) {
                        oss << "  \"" << key << "\": " << (boolVal.value() ? "true" : "false");
                    } else {
                        auto stringVal = config->get<std::string>(key);
                        if (stringVal.has_value()) {
                            oss << "  \"" << key << "\": \"" << stringVal.value() << "\"";
                        }
                    }
                }
            }
            
            if (i < keys.size() - 1) {
                oss << ",\n";
            } else {
                oss << "\n";
            }
        }
        
        oss << "}\n";
        return oss.str();
    }
    
    static bool importConfigFromJSON(const std::string &json, std::shared_ptr<Config> &config) {
        try {
            // Simple JSON parsing (for common cases)
            size_t start = json.find('{');
            size_t end = json.rfind('}');
            
            if (start == std::string::npos || end == std::string::npos) {
                return false;
            }
            
            std::string jsonContent = json.substr(start + 1, end - start - 1);
            
            // Simple key:value parsing
            size_t pos = 0;
            while (pos < jsonContent.size()) {
                size_t colon = jsonContent.find(':', pos);
                if (colon == std::string::npos) {
                    break;
                }
                
                size_t comma = jsonContent.find(',', colon);
                if (comma == std::string::npos) {
                    comma = jsonContent.size();
                }
                
                std::string key = jsonContent.substr(pos, colon - pos);
                std::string value = jsonContent.substr(colon + 1, comma - colon - 1);
                
                // Trim
                key = trim(key);
                value = trim(value);
                
                // Remove quotes
                if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                    key = key.substr(1, key.size() - 2);
                }
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Parse value
                if (value == "true" || value == "false") {
                    config->set(key, value == "true", ConfigSource::File);
                } else if (value.find('.') != std::string::npos) {
                    try {
                        double doubleVal = std::stod(value);
                        config->set(key, doubleVal, ConfigSource::File);
                    } catch (...) {
                        config->set(key, value, ConfigSource::File);
                    }
                } else {
                    try {
                        int intVal = std::stoi(value);
                        config->set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        config->set(key, value, ConfigSource::File);
                    }
                }
                
                pos = comma + 1;
                while (pos < jsonContent.size() && (jsonContent[pos] == ' ' || jsonContent[pos] == '\t')) {
                    pos++;
                }
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

PYBIND11_MODULE(enhanced_nlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        Features:
        - Advanced configuration management with JSON/YAML support
        - Enhanced brain analysis and diagnostic tools
        - Comprehensive agent simulation utilities
        - Detailed performance monitoring and reporting
        - Episode-based learning and data management
        - Network optimization recommendations
        - Backward compatible with original bindings
        
        Installation:
            pip install pynlm-enhanced
            
        Basic usage:
            import enhanced_nlm
            
            # Create an enhanced brain
            config = enhanced_nlm.createDefaultConfig()
            brain = enhanced_nlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
                
            print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
            
        Advanced usage:
            import enhanced_nlm
            
            # Create enhanced configuration
            config = enhanced_nlm.createConfigFromDict({
                "brain.neuron_count": 5000,
                "brain.synapse_density": 0.15,
                "plasticity.stdp.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "development.enable": True
            })
            
            brain = enhanced_nlm.createBrain(config)
            
            # Get detailed network analysis
            report = enhanced_nlm.generateBrainReport(brain)
            print(report)
            
            # Check network health
            is_healthy = enhanced_nlm.isNetworkActive(brain)
            print(f"Network healthy: {is_healthy}")
            
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // Register advanced configuration helper functions
    m.def("validateConfigFile", &ConfigImportHelper::validateConfigFile,
          py::arg("filepath"),
          "Validate a configuration file (supports both key=value and JSON)");
    
    m.def("createConfigFromDict", &ConfigImportHelper::createConfigFromDict,
          py::arg("dict"),
          "Create a configuration from a dictionary with type safety");
    
    m.def("configToDict", &ConfigImportHelper::configToDict,
          py::arg("config"),
          "Convert configuration to dictionary with automatic type detection");
    
    m.def("configToJSON", &ConfigImportHelper::configToJSON,
          py::arg("config"),
          "Convert configuration to JSON string");
    
    m.def("importConfigFromJSON", &ConfigImportHelper::importConfigFromJSON,
          py::arg("json"), py::arg("config"),
          "Import configuration from JSON string");
    
    // Register basic brain types and utilities
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
    
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file (supports both key=value and JSON)")
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
        // Additional configuration methods for enhanced functionality
        .def("setFromDict", [](Config& self, const std::unordered_map<std::string, std::variant<int, double, bool, std::string>> &dict) {
            for (const auto& [key, value] : dict) {
                std::visit([&](const auto& val) {
                    if constexpr (std::is_same_v<decltype(val), int>) {
                        self.set(key, val, ConfigSource::Runtime);
                    } else if constexpr (std::is_same_v<decltype(val), double>) {
                        self.set(key, val, ConfigSource::Runtime);
                    } else if constexpr (std::is_same_v<decltype(val), bool>) {
                        self.set(key, val, ConfigSource::Runtime);
                    } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                        self.set(key, val, ConfigSource::Runtime);
                    }
                }, value);
            }
        }, py::arg("dict"),
           "Set multiple configuration values from a dictionary with automatic type inference");
    
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
    
    // Version
    m.attr("__version__") = "1.0.0";
    
    // Documentation
    m.def("getVersion", []() { return std::string("NLM Enhanced Python Bindings 1.0.0"); },
          "Get version information for enhanced NLM bindings");
    
    m.def("getDocumentation", []() {
        return std::string(
            "Enhanced NLM (Neural Learning Machine) Python Bindings\n"
            "===============================================\n\n"
            "This is the enhanced version of NLM Python bindings with advanced features:\n\n"
            "New Features:\n"
            "  - Advanced configuration management with JSON/YAML support\n"
            "  - Enhanced brain analysis and diagnostic tools\n"
            "  - Comprehensive agent simulation utilities\n"
            "  - Detailed performance monitoring and reporting\n"
            "  - Episode-based learning and data management\n"
            "  - Network optimization recommendations\n"
            "  - Backward compatible with original bindings\n\n"
            "Quick Start:\n"
            "  import enhanced_nlm\n"
            "  brain = enhanced_nlm.createBrain(enhanced_nlm.createDefaultConfig())\n"
            "  brain.initialize()\n"
            "  for step in range(1000): brain.step(step)\n"
            "  print(f'Simulation complete: {brain.getTotalSpikeCount()} spikes')\n\n"
            "Advanced Usage:\n"
            "  import enhanced_nlm\n"
            "  config = enhanced_nlm.createConfigFromDict({\n"
            "      'brain.neuron_count': 5000,\n"
            "      'plasticity.stdp.enable': True\n"
            "  })\n"
            "  brain = enhanced_nlm.createBrain(config)\n"
            "  report = enhanced_nlm.generateBrainReport(brain)\n"
            "  print(report)\n\n"
            "Key Functions:\n"
            "  - enhanced_nlm.createConfigFromDict(): Type-safe configuration\n"
            "  - enhanced_nlm.generateBrainReport(): Detailed brain analysis\n"
            "  - enhanced_nlm.runEpisode(): Complete agent simulation\n"
            "  - enhanced_nlm.analyzeBehavior(): Statistical analysis\n"
            "  - enhanced_nlm.setupEnvironment(): Environment configuration\n"
        );
    }, "Get documentation and examples for enhanced NLM bindings");
    
    // Error types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        }
    });
}

} // namespace nlm

// Helper function implementations
namespace nlm {
    std::string ConfigImportHelper::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
}