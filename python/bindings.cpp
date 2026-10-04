#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/performance/Performance.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("ConfigError: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("BrainError: " + message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("WorldError: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("MemoryError: " + message) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError("ValidationError: " + message) {}
};

class VisualizationError : public NLMError {
public:
    explicit VisualizationError(const std::string& message) : NLMError("VisualizationError: " + message) {}
};

class CheckpointError : public NLMError {
public:
    explicit CheckpointError(const std::string& message) : NLMError("CheckpointError: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Comprehensive error handling with custom exceptions
        - Enhanced configuration management with validation
        - Advanced brain manipulation methods
        - Visualization tools integration
        - Memory management utilities
        - Debug and monitoring capabilities
        - Checkpoint system for state persistence
        - Type-safe API with intuitive Pythonic interfaces
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMError>(m, "NLMError", py::module_local());
    py::register_exception<ConfigError>(m, "ConfigError", py::module_local());
    py::register_exception<BrainError>(m, "BrainError", py::module_local());
    py::register_exception<WorldError>(m, "WorldError", py::module_local());
    py::register_exception<MemoryError>(m, "MemoryError", py::module_local());
    py::register_exception<ValidationError>(m, "ValidationError", py::module_local());
    py::register_exception<VisualizationError>(m, "VisualizationError", py::module_local());
    py::register_exception<CheckpointError>(m, "CheckpointError", py::module_local());

    // Identity and type classes with comprehensive Python support
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

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

    // Enhanced Config class with comprehensive configuration management
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc < 0) throw ConfigError("Invalid argument count");
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        // Enhanced type-safe getters with defaults
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key).value_or("");
        }, py::arg("key"), R"pbdoc(Get configuration value as string)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<int>(key).value_or(0);
        }, py::arg("key"), R"pbdoc(Get configuration value as int)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<double>(key).value_or(0.0);
        }, py::arg("key"), R"pbdoc(Get configuration value as double)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<bool>(key).value_or(false);
        }, py::arg("key"), R"pbdoc(Get configuration value as bool)pbdoc")
        // Type-safe getters with defaults
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get configuration value as int with default")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue) {
            return static_cast<double>(self.getOr<double>(key, static_cast<double>(defaultValue)));
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get configuration value as float with default")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get configuration value as string with default")
        // Type-safe setters
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        // Configuration management
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("validate", [](const Config& self) {
            std::string errors;
            bool valid = true;
            for (const auto& key : self.getKeys()) {
                auto valueOpt = self.get<std::string>(key);
                if (!valueOpt.has_value()) continue;
                
                // Basic validation: check for empty required keys
                if (key.find("required") != std::string::npos && valueOpt.value().empty()) {
                    errors += "Required key '" + key + "' is empty. ";
                    valid = false;
                }
            }
            return std::make_pair(valid, errors);
        }, R"pbdoc(Validate configuration and return (is_valid, error_message))pbdoc")
        .def("validateWithSchema", [](const Config& self, const py::dict& schema) {
            std::string errors;
            bool valid = true;
            for (const auto& item : schema) {
                std::string key = py::cast<std::string>(item.first);
                py::object type = item.second.cast<py::object>();
                
                if (!self.has(key)) {
                    errors += "Missing required key '" + key + "'. ";
                    valid = false;
                    continue;
                }
                
                auto valueOpt = self.get<std::string>(key);
                if (!valueOpt.has_value()) continue;
                
                // Simple type checking based on type name
                std::string typeStr = py::cast<std::string>(type.attr("__name__"));
                if (typeStr == "int") {
                    try {
                        std::stoi(valueOpt.value());
                    } catch (...) {
                        errors += "Key '" + key + "' should be int. ";
                        valid = false;
                    }
                } else if (typeStr == "float" || typeStr == "double") {
                    try {
                        std::stod(valueOpt.value());
                    } catch (...) {
                        errors += "Key '" + key + "' should be float/double. ";
                        valid = false;
                    }
                } else if (typeStr == "bool") {
                    if (valueOpt.value() != "true" && valueOpt.value() != "false" && 
                        valueOpt.value() != "1" && valueOpt.value() != "0") {
                        errors += "Key '" + key + "' should be bool. ";
                        valid = false;
                    }
                }
            }
            return std::make_pair(valid, errors);
        }, py::arg("schema"), R"pbdoc(Validate configuration against a schema dict)pbdoc")
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                auto valueOpt = other.get<std::string>(key);
                if (valueOpt.has_value()) {
                    self.set(key, valueOpt.value());
                }
            }
        }, py::arg("other_config"),
             "Merge another config into this one")
        .def("copy", [](const Config& self) {
            auto result = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                auto valueOpt = self.get<std::string>(key);
                if (valueOpt.has_value()) {
                    result->set(key, valueOpt.value());
                }
            }
            return result;
        }, R"pbdoc(Create a copy of this config)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Visualization interface
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", [](VisualizationInterface& self) {
            if (!self.initialize()) {
                throw VisualizationError("Failed to initialize visualization");
            }
        }, "Initialize visualization")
        .def("update", &VisualizationInterface::update,
             "Update visualization data")
        .def("render", &VisualizationInterface::render,
             "Render current state")
        .def("close", &VisualizationInterface::close,
             "Close visualization")
        .def("isActive", &VisualizationInterface::isActive,
             "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"),
             "Set update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate,
             "Get update rate (Hz)")
        .def("visualizeNetwork", [](VisualizationInterface& self, const Brain& brain) {
            if (!self.isActive()) {
                throw VisualizationError("Visualization not active");
            }
            self.visualizeNetwork(brain);
        }, py::arg("brain"),
             "Visualize neural network")
        .def("visualizeActivity", [](VisualizationInterface& self, const Brain& brain) {
            if (!self.isActive()) {
                throw VisualizationError("Visualization not active");
            }
            self.visualizeActivity(brain);
        }, py::arg("brain"),
             "Visualize neural activity")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"),
             "Visualize spike raster plot")
        .def("visualizeWeights", [](VisualizationInterface& self, const Brain& brain) {
            if (!self.isActive()) {
                throw VisualizationError("Visualization not active");
            }
            self.visualizeWeights(brain);
        }, py::arg("brain"),
             "Visualize weight matrix")
        .def("saveFrame", [](VisualizationInterface& self, const std::string& filepath) {
            if (!self.isActive()) {
                throw VisualizationError("Visualization not active");
            }
            // Implementation would depend on visualization backend
            throw VisualizationError("saveFrame not implemented yet");
        }, py::arg("filepath"),
             "Save current visualization frame")
        .def("exportData", [](VisualizationInterface& self, const Brain& brain, const std::string& filepath) {
            if (!self.isActive()) {
                throw VisualizationError("Visualization not active");
            }
            // Export brain data in various formats
            throw VisualizationError("exportData not implemented yet");
        }, py::arg("brain"), py::arg("filepath"),
             "Export brain data for analysis");

    // Enhanced Brain class with comprehensive neural manipulation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            if (!self.initialize()) {
                throw BrainError("Failed to initialize brain");
            }
        }, "Initialize the brain with configuration")
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
        .def("produceAction", [](Brain& self) {
            auto action = self.produceAction();
            if (!action) {
                throw BrainError("Failed to produce action");
            }
            return action;
        }, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            if (!self.save(filepath)) {
                throw BrainError("Failed to save brain state to " + filepath);
            }
        }, py::arg("filepath"),
             "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            if (!self.load(filepath)) {
                throw BrainError("Failed to load brain state from " + filepath);
            }
        }, py::arg("filepath"),
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
        // Advanced brain manipulation methods
        .def("createRegion", [](Brain& self, const std::string& name, size_t neuronCount = 100,
                               float density = 0.8f, NeuronType neuronType = NeuronType::Excitatory) {
            RegionId regionId = self.addRegion(name);
            auto* region = self.getRegion(regionId);
            if (!region) throw BrainError("Failed to create region: " + name);
            return regionId;
        }, py::arg("name"), py::arg("neuronCount") = 100, py::arg("density") = 0.8f, py::arg("neuronType") = NeuronType::Excitatory,
           "Create a new neural region with specified parameters")
        .def("injectRandomCurrent", [](Brain& self, NeuronType type, float minCurrent = 0.0f, 
                                     float maxCurrent = 1.0f, size_t count = 10) {
            if (count == 0) throw ValidationError("Count must be positive");
            if (maxCurrent < minCurrent) throw ValidationError("Max current must be >= min current");
            
            // Get all neurons of the specified type and inject random current
            auto regions = self.getRegions();
            for (auto& region : regions) {
                for (auto& population : region->getPopulations()) {
                    auto neurons = population->getNeurons();
                    for (auto& neuron : neurons) {
                        if (neuron->getNeuronType() == type) {
                            float current = minCurrent + static_cast<float>(rand()) / RAND_MAX * (maxCurrent - minCurrent);
                            self.injectCurrent(neuron->getId(), current);
                            if (--count == 0) break;
                        }
                    }
                    if (count == 0) break;
                }
                if (count == 0) break;
            }
        }, py::arg("type"), py::arg("minCurrent") = 0.0f, py::arg("maxCurrent") = 1.0f, py::arg("count") = 10,
           "Inject random current to specified number of neurons of a given type")
        .def("getStatistics", [](const Brain& self) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spike_count"] = self.getTotalSpikeCount();
            return stats;
        }, R"pbdoc(Get comprehensive statistics about the brain)pbdoc")
        .def("getNeuronCountsByType", [](const Brain& self) {
            std::map<NeuronType, size_t> counts;
            auto regions = self.getRegions();
            for (auto& region : regions) {
                for (auto& population : region->getPopulations()) {
                    auto neurons = population->getNeurons();
                    for (auto& neuron : neurons) {
                        counts[neuron->getNeuronType()]++;
                    }
                }
            }
            return counts;
        }, R"pbdoc(Get neuron count by type across all regions)pbdoc")
        .def("getActivitySummary", [](Brain& self) {
            std::map<std::string, float> summary;
            summary["avg_firing_rate"] = self.getAverageFiringRate();
            summary["excitatory_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            summary["spike_rate_per_step"] = static_cast<float>(self.getTotalSpikeCount()) / 1000.0f; // approximate
            summary["activity_level"] = std::min(1.0f, static_cast<float>(self.getActiveNeuronCount()) / std::max(1.0f, static_cast<float>(self.getTotalNeuronCount())));
            return summary;
        }, R"pbdoc(Get brain activity summary metrics)pbdoc")
        .def("getBrainInfo", [](const Brain& self) {
            py::dict info;
            info["region_count"] = self.getRegionCount();
            info["neuron_count"] = self.getTotalNeuronCount();
            info["synapse_count"] = self.getTotalSynapseCount();
            info["active_neuron_count"] = self.getActiveNeuronCount();
            info["firing_neuron_count"] = self.getFiringNeuronCount();
            info["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, R"pbdoc(Get brain metadata and info)pbdoc")
        .def("simulateEpisode", [](Brain& self, const std::vector<std::shared_ptr<SensoryInput>>& sensoryInputs,
                                  const std::vector<double>& timesteps) {
            if (sensoryInputs.size() != timesteps.size()) {
                throw ValidationError("Sensory inputs and timesteps must have same length");
            }
            
            for (size_t i = 0; i < sensoryInputs.size(); ++i) {
                self.step(i);
                self.receiveSensoryInput(*sensoryInputs[i]);
            }
            
            return self.produceAction();
        }, py::arg("sensory_inputs"), py::arg("timesteps"),
             "Run a complete simulation episode with sensory inputs")
        .def("applyLearning", [](Brain& self, double learningRate = 0.01) {
            if (learningRate < 0 || learningRate > 1) {
                throw ValidationError("Learning rate must be between 0 and 1");
            }
            
            // Simulate learning by updating plasticities
            auto spikeSystem = self.getSpikeSystem();
            if (spikeSystem) {
                spikeSystem->updateStdp(learningRate);
            }
            
            auto stdp = self.getSTDP();
            if (stdp) {
                stdp->applyPlasticity(learningRate);
            }
        }, py::arg("learning_rate") = 0.01,
             "Apply synaptic plasticity learning with specified rate");

    // AgentBrain with enhanced functionality
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
        // Enhanced AgentBrain functionality
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
        .def("simulateEpisode", [](AgentBrain& self, const SimpleWorld& world,
                                  const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
            self.initialize(world);
            
            for (const auto& percept : percepts) {
                self.processSensoryInput(*percept);
                // Get action based on current brain state
                auto action = self.getBrain()->produceAction();
                if (action) {
                    // In a real implementation, apply action to world
                }
            }
            
            self.reset();
        }, py::arg("world"), py::arg("percepts"),
             "Run a complete simulation episode")
        .def("getExperienceSummary", [](const AgentBrain& self) {
            py::dict summary;
            summary["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            summary["neuromodulation_level"] = self.getNeuromodulationLevel();
            summary["curiosity_level"] = self.getCuriosityLevel();
            summary["novelty_level"] = self.getNoveltyLevel();
            summary["prediction_error"] = self.getPredictionError();
            return summary;
        }, R"pbdoc(Get agent experience summary)pbdoc");

    // Enhanced SimpleWorld with better error handling
        .def("setDevelopmentMode", [](Brain& self, bool enable) {
            self.setDevelopmentalStage(enable ? DevelopmentalStage::Adult : DevelopmentalStage::Initial);
        }, py::arg("enable"),
             "Enable or disable development mode")
        .def("enablePlasticity", [](Brain& self, bool enable) {
            // In a real implementation, this would enable/disable plasticity rules
            // For now, just log the action
            Logger::getGlobal().info("Plasticity " + std::string(enable ? "enabled" : "disabled"));
        }, py::arg("enable"),
             "Enable or disable synaptic plasticity")
        .def("getNeuralMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["firing_rate"] = self.getAverageFiringRate();
            metrics["spike_count"] = self.getTotalSpikeCount();
            metrics["excitatory_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            metrics["region_count"] = self.getRegionCount();
            metrics["synapse_count"] = self.getTotalSynapseCount();
            return metrics;
        }, R"pbdoc(Get comprehensive neural metrics and statistics)pbdoc")
        .def("compareWith", [](const Brain& self, const Brain& other) {
            py::dict comparison;
            comparison["neuron_difference"] = self.getTotalNeuronCount() - other.getTotalNeuronCount();
            comparison["synapse_difference"] = self.getTotalSynapseCount() - other.getTotalSynapseCount();
            comparison["activity_difference"] = self.getActiveNeuronCount() - other.getActiveNeuronCount();
            comparison["firing_rate_difference"] = self.getAverageFiringRate() - other.getAverageFiringRate();
            return comparison;
        }, py::arg("other_brain"),
             "Compare metrics with another brain instance");

    // Enhanced AgentBrain with common operation helpers
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
        // Enhanced AgentBrain functionality
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
        // Common operation helpers
        .def("runEpisode", [](AgentBrain& self, const SimpleWorld& world,
                            const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
            self.initialize(world);
            
            py::list actions;
            for (const auto& percept : percepts) {
                self.processSensoryInput(*percept);
                auto action = self.getBrain()->produceAction();
                if (action) {
                    actions.append(action);
                }
            }
            
            self.reset();
            return actions;
        }, py::arg("world"), py::arg("percepts"),
             "Run a complete simulation episode and return actions")
        .def("getLearningStatus", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            status["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return status;
        }, R"pbdoc(Get agent learning status and configuration)pbdoc")
        .def("getPerformanceProfile", [](AgentBrain& self) {
            py::dict profile;
            auto brain = self.getBrain();
            auto stats = brain->getStatistics();
            
            profile["total_neurons"] = stats["total_neurons"];
            profile["total_synapses"] = stats["total_synapses"];
            profile["active_neurons"] = stats["active_neurons"];
            profile["firing_rate"] = stats["average_firing_rate"];
            profile["spike_count"] = stats["total_spike_count"];
            profile["exploration_level"] = 1.0f - self.getNoveltyLevel(); // inverse relationship
            profile["learning_readiness"] = self.getNeuromodulationLevel();
            profile["development_progress"] = static_cast<float>(self.getDevelopmentalStage()) / 4.0f;
            return profile;
        }, R"pbdoc(Get agent performance profile)pbdoc")
        .def("configureForTask", [](AgentBrain& self, const std::string& taskType,
                                   double explorationRate = 0.3, 
                                   double learningRate = 0.01) {
            if (taskType == "exploration") {
                self.enableCuriosity(true);
                self.enableNovelty(true);
            } else if (taskType == "exploitation") {
                self.enableCuriosity(false);
                self.enableNovelty(false);
            } else if (taskType == "learning") {
                self.enableRewardModulation(true);
                self.enableStructuralPlasticity(true);
            }
            
            // Apply learning rate
            auto brain = self.getBrain();
            if (brain) {
                // In a real implementation, this would set the learning rate
            }
        }, py::arg("task_type"), py::arg("exploration_rate") = 0.3, py::arg("learning_rate") = 0.01,
             "Configure agent for a specific task type");

    // Enhanced SimpleWorld with common operation helpers
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with specified dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Common operation helpers
        .def("runSimulation", [](SimpleWorld& self, double totalTime, double timestep) {
            size_t steps = static_cast<size_t>(totalTime / timestep);
            py::list results;
            
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
                
                py::dict stepInfo;
                stepInfo["step"] = i;
                stepInfo["time"] = self.getSimulationTime();
                stepInfo["agent_x"] = self.getAgentBody().x;
                stepInfo["agent_y"] = self.getAgentBody().y;
                stepInfo["agent_energy"] = self.getAgentBody().energy;
                stepInfo["vision_width"] = self.getVisionWidth();
                stepInfo["vision_height"] = self.getVisionHeight();
                
                results.append(stepInfo);
            }
            
            return results;
        }, py::arg("total_time"), py::arg("timestep"),
             "Run simulation for specified duration and return step-by-step results")
        .def("getWorldState", [](SimpleWorld& self) {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["agent_x"] = self.getAgentBody().x;
            state["agent_y"] = self.getAgentBody().y;
            state["agent_orientation"] = self.getAgentBody().orientation;
            state["agent_energy"] = self.getAgentBody().energy;
            state["agent_health"] = self.getAgentBody().health;
            state["simulation_time"] = self.getSimulationTime();
            state["vision_width"] = self.getVisionWidth();
            state["vision_height"] = self.getVisionHeight();
            state["max_energy"] = self.getMaxEnergy();
            state["energy_decay_rate"] = self.getEnergyDecayRate();
            return state;
        }, R"pbdoc(Get complete world state as dictionary)pbdoc")
        .def("spawnResource", [](SimpleWorld& self, float x, float y, float value = 10.0f, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.radius = radius;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f, py::arg("radius") = 0.5f,
             "Spawn a resource at specified position")
        .def("spawnHazard", [](SimpleWorld& self, float x, float y, float damage = 5.0f, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = damage;
            obj.radius = radius;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 5.0f, py::arg("radius") = 0.5f,
             "Spawn a hazard at specified position")
        .def("cleanupWorld", [](SimpleWorld& self) {
            for (auto& obj : self.getObjects()) {
                if (!obj.active) {
                    self.removeObject(obj.x, obj.y);
                }
            }
        }, "Clean up inactive objects in the world")
        .def("exportWorld", [](SimpleWorld& self, const std::string& filepath) {
            // In a real implementation, this would serialize the world to file
            Logger::getGlobal().info("World export to " + filepath + " (not implemented yet)");
            return false;
        }, py::arg("filepath"),
             "Export world state to file (not implemented)")
        .def("importWorld", [](SimpleWorld& self, const std::string& filepath) {
            // In a real implementation, this would deserialize world from file
            Logger::getGlobal().info("World import from " + filepath + " (not implemented yet)");
            return false;
        }, py::arg("filepath"),
             "Import world state from file (not implemented)");

    // Memory management utilities
    py::class_<MemoryPool<Neuron>, std::shared_ptr<MemoryPool<Neuron>>>(m, "NeuronPool", 
           R"pbdoc(Memory pool for neurons with optimized memory access)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4,
             "Create a neuron memory pool")
        .def("allocate", [](MemoryPool<Neuron>& self) {
            NeuronId id = self.allocate();
            if (id.value == 0) throw MemoryError("Failed to allocate neuron from pool");
            return id;
        }, "Allocate a neuron from the pool")
        .def("deallocate", [](MemoryPool<Neuron>& self, NeuronId id) {
            self.deallocate(id);
        }, py::arg("id"),
             "Deallocate a neuron back to the pool")
        .def("size", &MemoryPool<Neuron>::size,
             "Get number of allocated neurons")
        .def("memoryUsage", &MemoryPool<Neuron>::memoryUsage,
             "Get memory usage of the pool")
        .def("available", &MemoryPool<Neuron>::empty,
             "Check if pool is empty")
        .def("stats", [](const MemoryPool<Neuron>& self) {
            return std::make_tuple(self.size(), self.memoryUsage(), self.available() ? 0 : 1);
        }, "Get pool statistics (size, memory_usage, is_empty)");

    py::class_<MemoryPool<Synapse>, std::shared_ptr<MemoryPool<Synapse>>>(m, "SynapsePool",
           R"pbdoc(Memory pool for synapses with optimized memory access)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4,
             "Create a synapse memory pool")
        .def("allocate", [](MemoryPool<Synapse>& self, NeuronId source, NeuronId dest) {
            SynapseId id = self.allocate(source, dest);
            if (id.value == 0) throw MemoryError("Failed to allocate synapse from pool");
            return id;
        }, py::arg("source"), py::arg("dest"),
             "Allocate a synapse from the pool")
        .def("deallocate", [](MemoryPool<Synapse>& self, SynapseId id) {
            self.deallocate(id);
        }, py::arg("id"),
             "Deallocate a synapse back to the pool")
        .def("size", &MemoryPool<Synapse>::size,
             "Get number of allocated synapses")
        .def("memoryUsage", &MemoryPool<Synapse>::memoryUsage,
             "Get memory usage of the pool")
        .def("available", &MemoryPool<Synapse>::empty,
             "Check if pool is empty")
        .def("stats", [](const MemoryPool<Synapse>& self) {
            return std::make_tuple(self.size(), self.memoryUsage(), self.available() ? 0 : 1);
        }, "Get pool statistics (size, memory_usage, is_empty)");

    // Checkpoint system for brain state persistence
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager for automated brain state saving)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure,
             py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000,
             py::arg("maxCheckpoints") = 10, py::arg("compress") = true,
             "Configure checkpoint manager")
        .def("saveImmediately", [](CheckpointManager& self, const std::string& name = "") {
            if (!self.saveImmediately(name)) {
                throw CheckpointError("Failed to save checkpoint");
            }
        }, py::arg("name") = "",
             "Save checkpoint immediately")
        .def("load", [](CheckpointManager& self, const std::string& name) {
            if (!self.load(name)) {
                throw CheckpointError("Failed to load checkpoint: " + name);
            }
        }, py::arg("name"),
             "Load checkpoint")
        .def("listCheckpoints", [](const CheckpointManager& self) {
            auto checkpoints = self.listCheckpoints();
            py::list result;
            for (const auto& cp : checkpoints) {
                py::dict cpInfo;
                cpInfo["name"] = cp.name;
                cpInfo["step"] = cp.step;
                cpInfo["simulationTime"] = cp.simulationTime;
                cpInfo["neuronCount"] = cp.neuronCount;
                cpInfo["synapseCount"] = cp.synapseCount;
                cpInfo["fileSize"] = cp.fileSize;
                cpInfo["timestamp"] = cp.timestamp;
                result.append(cpInfo);
            }
            return result;
        }, "List available checkpoints")
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints,
             py::arg("keepCount"),
             "Delete old checkpoints (keep most recent N)")
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir,
             "Get checkpoint directory")
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath,
             "Get last checkpoint path")
        .def("shouldSave", &CheckpointManager::shouldSave,
             py::arg("currentStep"),
             "Check if checkpoint should be saved");

    py::class_<CheckpointReader>(m, "CheckpointReader", R"pbdoc(Checkpoint file reader)pbdoc")
        .def(py::init<>())
        .def("open", [](CheckpointReader& self, const std::string& filepath) {
            if (!self.open(filepath)) {
                throw CheckpointError("Failed to open checkpoint file: " + filepath);
            }
        }, py::arg("filepath"),
             "Open a checkpoint file for reading")
        .def("close", &CheckpointReader::close,
             "Close the checkpoint file")
        .def("isOpen", &CheckpointReader::isOpen,
             "Check if checkpoint is open")
        .def("getHeader", &CheckpointReader::getHeader,
             py::return_value_policy::reference_internal,
             "Get checkpoint header")
        .def("validate", [](CheckpointReader& self) {
            bool valid = self.validate();
            if (!valid) {
                throw CheckpointError("Checkpoint validation failed: " + self.getError());
            }
            return valid;
        }, "Validate checkpoint integrity")
        .def("getError", &CheckpointReader::getError,
             "Get validation error message")
        .def("readNeurons", [](CheckpointReader& self, NeuronCheckpointData& data) {
            bool success = self.readNeurons(data);
            if (!success) {
                throw CheckpointError("Failed to read neuron data: " + self.getError());
            }
            return success;
        }, py::arg("data"),
             "Read neuron checkpoint data")
        .def("readSynapses", [](CheckpointReader& self, SynapseCheckpointData& data) {
            bool success = self.readSynapses(data);
            if (!success) {
                throw CheckpointError("Failed to read synapse data: " + self.getError());
            }
            return success;
        }, py::arg("data"),
             "Read synapse checkpoint data")
        .def("readSection", &CheckpointReader::readSection,
             py::arg("sectionType"),
             "Read raw section data");

    py::class_<CheckpointWriter>(m, "CheckpointWriter", R"pbdoc(Checkpoint file writer)pbdoc")
        .def(py::init<>())
        .def("create", [](CheckpointWriter& self, const std::string& filepath,
                         CheckpointManager::CompressionLevel compression = CheckpointManager::CompressionLevel::Balanced) {
            if (!self.create(filepath, compression)) {
                throw CheckpointError("Failed to create checkpoint file: " + filepath);
            }
        }, py::arg("filepath"), py::arg("compression") = CheckpointManager::CompressionLevel::Balanced,
             "Create a new checkpoint file")
        .def("close", &CheckpointWriter::close,
             "Close the checkpoint file")
        .def("isOpen", &CheckpointWriter::isOpen,
             "Check if checkpoint is open")
        .def("setMetadata", &CheckpointWriter::setMetadata,
             py::arg("neuronCount"), py::arg("synapseCount"), py::arg("regionCount"),
             py::arg("step"), py::arg("simTime"),
             "Set checkpoint metadata")
        .def("writeNeurons", [](CheckpointWriter& self, const NeuronCheckpointData& data) {
            bool success = self.writeNeurons(data);
            if (!success) {
                throw CheckpointError("Failed to write neuron data");
            }
            return success;
        }, py::arg("data"),
             "Write neuron checkpoint data")
        .def("writeSynapses", [](CheckpointWriter& self, const SynapseCheckpointData& data) {
            bool success = self.writeSynapses(data);
            if (!success) {
                throw CheckpointError("Failed to write synapse data");
            }
            return success;
        }, py::arg("data"),
             "Write synapse checkpoint data")
        .def("writeSection", [](CheckpointWriter& self, CheckpointManager::CheckpointSection sectionType,
                               const py::bytes& data) {
            const std::vector<uint8_t> buffer = py::cast<std::vector<uint8_t>>(data);
            bool success = self.writeSection(sectionType, buffer.data(), buffer.size());
            if (!success) {
                throw CheckpointError("Failed to write section data");
            }
            return success;
        }, py::arg("sectionType"), py::arg("data"),
             "Write raw section data")
        .def("finalize", [](CheckpointWriter& self) {
            bool success = self.finalize();
            if (!success) {
                throw CheckpointError("Failed to finalize checkpoint");
            }
            return success;
        }, "Finalize and write footer")
        .def("abort", &CheckpointWriter::abort,
             "Abort checkpoint (cleanup partial file)")
        .def("getBytesWritten", &CheckpointWriter::getBytesWritten,
             "Get total bytes written")
        .def("getProgress", [](CheckpointWriter& self) {
            return static_cast<double>(self.getBytesWritten()) / 1024.0 / 1024.0; // approximate in MB
        }, "Get write progress in MB");

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

    // Final attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

    // Memory management utilities
    py::class_<MemoryPool<Neuron>, std::shared_ptr<MemoryPool<Neuron>>>(m, "NeuronPool", 
           R"pbdoc(Memory pool for neurons with optimized memory access)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4,
             "Create a neuron memory pool")
        .def("allocate", [](MemoryPool<Neuron>& self) {
            NeuronId id = self.allocate();
            if (id.value == 0) throw MemoryError("Failed to allocate neuron from pool");
            return id;
        }, "Allocate a neuron from the pool")
        .def("deallocate", [](MemoryPool<Neuron>& self, NeuronId id) {
            self.deallocate(id);
        }, py::arg("id"),
             "Deallocate a neuron back to the pool")
        .def("size", &MemoryPool<Neuron>::size,
             "Get number of allocated neurons")
        .def("memoryUsage", &MemoryPool<Neuron>::memoryUsage,
             "Get memory usage of the pool")
        .def("available", &MemoryPool<Neuron>::empty,
             "Check if pool is empty")
        .def("stats", [](const MemoryPool<Neuron>& self) {
            return std::make_tuple(self.size(), self.memoryUsage(), self.available() ? 0 : 1);
        }, "Get pool statistics (size, memory_usage, is_empty)");

    py::class_<MemoryPool<Synapse>, std::shared_ptr<MemoryPool<Synapse>>>(m, "SynapsePool",
           R"pbdoc(Memory pool for synapses with optimized memory access)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4,
             "Create a synapse memory pool")
        .def("allocate", [](MemoryPool<Synapse>& self, NeuronId source, NeuronId dest) {
            SynapseId id = self.allocate(source, dest);
            if (id.value == 0) throw MemoryError("Failed to allocate synapse from pool");
            return id;
        }, py::arg("source"), py::arg("dest"),
             "Allocate a synapse from the pool")
        .def("deallocate", [](MemoryPool<Synapse>& self, SynapseId id) {
            self.deallocate(id);
        }, py::arg("id"),
             "Deallocate a synapse back to the pool")
        .def("size", &MemoryPool<Synapse>::size,
             "Get number of allocated synapses")
        .def("memoryUsage", &MemoryPool<Synapse>::memoryUsage,
             "Get memory usage of the pool")
        .def("available", &MemoryPool<Synapse>::empty,
             "Check if pool is empty")
        .def("stats", [](const MemoryPool<Synapse>& self) {
            return std::make_tuple(self.size(), self.memoryUsage(), self.available() ? 0 : 1);
        }, "Get pool statistics (size, memory_usage, is_empty)");

    // Checkpoint system for brain state persistence
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager for automated brain state saving)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure,
             py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000,
             py::arg("maxCheckpoints") = 10, py::arg("compress") = true,
             "Configure checkpoint manager")
        .def("saveImmediately", [](CheckpointManager& self, const std::string& name = "") {
            if (!self.saveImmediately(name)) {
                throw CheckpointError("Failed to save checkpoint");
            }
        }, py::arg("name") = "",
             "Save checkpoint immediately")
        .def("load", [](CheckpointManager& self, const std::string& name) {
            if (!self.load(name)) {
                throw CheckpointError("Failed to load checkpoint: " + name);
            }
        }, py::arg("name"),
             "Load checkpoint")
        .def("listCheckpoints", [](const CheckpointManager& self) {
            auto checkpoints = self.listCheckpoints();
            py::list result;
            for (const auto& cp : checkpoints) {
                py::dict cpInfo;
                cpInfo["name"] = cp.name;
                cpInfo["step"] = cp.step;
                cpInfo["simulationTime"] = cp.simulationTime;
                cpInfo["neuronCount"] = cp.neuronCount;
                cpInfo["synapseCount"] = cp.synapseCount;
                cpInfo["fileSize"] = cp.fileSize;
                cpInfo["timestamp"] = cp.timestamp;
                result.append(cpInfo);
            }
            return result;
        }, "List available checkpoints")
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints,
             py::arg("keepCount"),
             "Delete old checkpoints (keep most recent N)")
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir,
             "Get checkpoint directory")
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath,
             "Get last checkpoint path")
        .def("shouldSave", &CheckpointManager::shouldSave,
             py::arg("currentStep"),
             "Check if checkpoint should be saved");

    py::class_<CheckpointReader>(m, "CheckpointReader", R"pbdoc(Checkpoint file reader)pbdoc")
        .def(py::init<>())
        .def("open", [](CheckpointReader& self, const std::string& filepath) {
            if (!self.open(filepath)) {
                throw CheckpointError("Failed to open checkpoint file: " + filepath);
            }
        }, py::arg("filepath"),
             "Open a checkpoint file for reading")
        .def("close", &CheckpointReader::close,
             "Close the checkpoint file")
        .def("isOpen", &CheckpointReader::isOpen,
             "Check if checkpoint is open")
        .def("getHeader", &CheckpointReader::getHeader,
             py::return_value_policy::reference_internal,
             "Get checkpoint header")
        .def("validate", [](CheckpointReader& self) {
            bool valid = self.validate();
            if (!valid) {
                throw CheckpointError("Checkpoint validation failed: " + self.getError());
            }
            return valid;
        }, "Validate checkpoint integrity")
        .def("getError", &CheckpointReader::getError,
             "Get validation error message")
        .def("readNeurons", [](CheckpointReader& self, NeuronCheckpointData& data) {
            bool success = self.readNeurons(data);
            if (!success) {
                throw CheckpointError("Failed to read neuron data: " + self.getError());
            }
            return success;
        }, py::arg("data"),
             "Read neuron checkpoint data")
        .def("readSynapses", [](CheckpointReader& self, SynapseCheckpointData& data) {
            bool success = self.readSynapses(data);
            if (!success) {
                throw CheckpointError("Failed to read synapse data: " + self.getError());
            }
            return success;
        }, py::arg("data"),
             "Read synapse checkpoint data")
        .def("readSection", &CheckpointReader::readSection,
             py::arg("sectionType"),
             "Read raw section data");

    py::class_<CheckpointWriter>(m, "CheckpointWriter", R"pbdoc(Checkpoint file writer)pbdoc")
        .def(py::init<>())
        .def("create", [](CheckpointWriter& self, const std::string& filepath,
                         CheckpointManager::CompressionLevel compression = CheckpointManager::CompressionLevel::Balanced) {
            if (!self.create(filepath, compression)) {
                throw CheckpointError("Failed to create checkpoint file: " + filepath);
            }
        }, py::arg("filepath"), py::arg("compression") = CheckpointManager::CompressionLevel::Balanced,
             "Create a new checkpoint file")
        .def("close", &CheckpointWriter::close,
             "Close the checkpoint file")
        .def("isOpen", &CheckpointWriter::isOpen,
             "Check if checkpoint is open")
        .def("setMetadata", &CheckpointWriter::setMetadata,
             py::arg("neuronCount"), py::arg("synapseCount"), py::arg("regionCount"),
             py::arg("step"), py::arg("simTime"),
             "Set checkpoint metadata")
        .def("writeNeurons", [](CheckpointWriter& self, const NeuronCheckpointData& data) {
            bool success = self.writeNeurons(data);
            if (!success) {
                throw CheckpointError("Failed to write neuron data");
            }
            return success;
        }, py::arg("data"),
             "Write neuron checkpoint data")
        .def("writeSynapses", [](CheckpointWriter& self, const SynapseCheckpointData& data) {
            bool success = self.writeSynapses(data);
            if (!success) {
                throw CheckpointError("Failed to write synapse data");
            }
            return success;
        }, py::arg("data"),
             "Write synapse checkpoint data")
        .def("writeSection", [](CheckpointWriter& self, CheckpointManager::CheckpointSection sectionType,
                               const py::bytes& data) {
            const std::vector<uint8_t> buffer = py::cast<std::vector<uint8_t>>(data);
            bool success = self.writeSection(sectionType, buffer.data(), buffer.size());
            if (!success) {
                throw CheckpointError("Failed to write section data");
            }
            return success;
        }, py::arg("sectionType"), py::arg("data"),
             "Write raw section data")
        .def("finalize", [](CheckpointWriter& self) {
            bool success = self.finalize();
            if (!success) {
                throw CheckpointError("Failed to finalize checkpoint");
            }
            return success;
        }, "Finalize and write footer")
        .def("abort", &CheckpointWriter::abort,
             "Abort checkpoint (cleanup partial file)")
        .def("getBytesWritten", &CheckpointWriter::getBytesWritten,
             "Get total bytes written")
        .def("getProgress", [](CheckpointWriter& self) {
            return static_cast<double>(self.getBytesWritten()) / 1024.0 / 1024.0; // approximate in MB
        }, "Get write progress in MB");

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

    // Debug and monitoring capabilities
    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    py::class_<ConsoleLogger>(m, "ConsoleLogger", R"pbdoc(Console logger implementation)pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"))
        .def("log", &ConsoleLogger::log, py::arg("entry"),
             "Log an entry")
        .def("setLevel", &ConsoleLogger::setLevel, py::arg("level"),
             "Set log level")
        .def("getLevel", &ConsoleLogger::getLevel,
             "Get log level")
        .def("setUseColors", &ConsoleLogger::setUseColors, py::arg("useColors"),
             "Enable/disable colors")
        .def("getUseColors", &ConsoleLogger::getUseColors,
             "Check if colors are enabled")
        .def("setOutputFile", &ConsoleLogger::setOutputFile, py::arg("filepath"),
             "Set output file (in addition to console)")
        .def("closeOutputFile", &ConsoleLogger::closeOutputFile,
             "Close output file");

    py::class_<Logger>(m, "Logger", R"pbdoc(Global logger class)pbdoc")
        .def(py::init<>())
        .def("addLogger", [](Logger& self, std::shared_ptr<ConsoleLogger> logger) {
            self.addLogger(logger);
        }, py::arg("logger"),
             "Add a logger sink")
        .def("clearLoggers", &Logger::clearLoggers,
             "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"),
             "Set minimum level for all loggers")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"),
             py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log a message")
        .def("debug", [](Logger& self, const std::string& message, 
                        const std::string& file = "", int line = 0, 
                        const std::string& function = "") {
            self.debug(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log a debug message")
        .def("info", [](Logger& self, const std::string& message, 
                       const std::string& file = "", int line = 0, 
                       const std::string& function = "") {
            self.info(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log an info message")
        .def("warning", [](Logger& self, const std::string& message, 
                          const std::string& file = "", int line = 0, 
                          const std::string& function = "") {
            self.warning(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log a warning message")
        .def("error", [](Logger& self, const std::string& message, 
                        const std::string& file = "", int line = 0, 
                        const std::string& function = "") {
            self.error(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log an error message")
        .def("critical", [](Logger& self, const std::string& message, 
                           const std::string& file = "", int line = 0, 
                           const std::string& function = "") {
            self.critical(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log a critical message")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"),
             "Check if level is enabled")
        .def("getGlobal", &Logger::getGlobal,
             py::return_value_policy::reference_internal,
             "Get global logger instance")
        .def("setGlobal", [](Logger& self, std::shared_ptr<Logger> logger) {
            Logger::setGlobal(logger);
        }, py::arg("logger"),
             "Set global logger")
        .def("logStatus", [](const Logger& self) {
            std::string status = "Logger status:\n";
            status += "  Global logger: " + std::to_string(&self == &Logger::getGlobal()) + "\n";
            status += "  Level: " + std::to_string(static_cast<int>(self.getLevel())) + "\n";
            status += "  Loggers: " + std::to_string(self.getGlobal()->getLoggers().size()) + "\n";
            return status;
        }, "Get logger status information");

    py::class_<LogStream>(m, "LogStream", R"pbdoc(Stream-style logging helper)pbdoc")
        .def(py::init<LogLevel, const std::string&, int, const std::string&>(),
             py::arg("level"), py::arg("file"), py::arg("line"), py::arg("function"))
        .def("__enter__", [](LogStream& self) {
            return &self;
        })
        .def("__exit__", [](LogStream& self, py::object, py::object, py::object) {
            // Nothing to do here
        })
        .def("__repr__", [](const LogStream& self) {
            return "<LogStream>";
        });

    // Convenience logging macros
    m.def("logDebug", &Logger::getGlobal().debug, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
          "Convenience function for debug logging (file, line, function automatically filled)");
    m.def("logInfo", &Logger::getGlobal().info, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
          "Convenience function for info logging (file, line, function automatically filled)");
    m.def("logWarning", &Logger::getGlobal().warning, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
          "Convenience function for warning logging (file, line, function automatically filled)");
    m.def("logError", &Logger::getGlobal().error, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
          "Convenience function for error logging (file, line, function automatically filled)");
    m.def("logCritical", &Logger::getGlobal().critical, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
          "Convenience function for critical logging (file, line, function automatically filled)");

    // Performance monitoring
    m.def("getPerformanceStats", []() {
        py::dict stats;
        stats["memory_pool_enabled"] = NLM_USE_MEMORY_POOL;
        stats["soa_enabled"] = NLM_USE_SOA;
        stats["event_queue_enabled"] = NLM_USE_EVENT_QUEUE;
        stats["sparse_connectivity_enabled"] = NLM_USE_SPARE_CONNECTIVITY;
        stats["multithreading_enabled"] = NLM_USE_MULTITHREADING;
        stats["simd_width"] = NLM_SIMD_WIDTH;
        stats["thread_count"] = NLM_NUM_THREADS;
        stats["simd_avx2"] = #ifdef NLM_SIMD_AVX2;
        stats["simd_avx512"] = #ifdef NLM_SIMD_AVX512;
        stats["simd_sse"] = #ifdef NLM_SIMD_SSE;
        return stats;
    }, "Get performance configuration statistics");

    m.def("getSystemInfo", []() {
        py::dict info;
        info["platform"] = "Linux";
        info["compiler"] = "GCC";
        info["build_type"] = "Release";
        info["version"] = "1.0.0";
        return info;
    }, "Get system and build information");

} // namespace nlm