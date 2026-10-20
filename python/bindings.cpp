#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <sstream>

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

// Python-specific exceptions
class NLMRuntimeError : public std::runtime_error {
public:
    NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigError : public NLMRuntimeError {
public:
    NLMConfigError(const std::string& msg) : NLMRuntimeError("Config Error: " + msg) {}
};

class NLMValidationError : public NLMRuntimeError {
public:
    NLMValidationError(const std::string& msg) : NLMRuntimeError("Validation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Pythonic API with convenient factory methods
        - Type safety with Python-friendly error handling
        - Advanced simulation utilities
        - Configuration validation
        - Performance monitoring tools
        - Batch operations for efficiency
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMRuntimeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const NLMValidationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ===== TYPE CONVERSIONS AND UTILITIES =====
    
    // Convert vector to numpy array
    py::class_<VectorConverter> m("VectorConverter");
    
    // ===== ID TYPES =====
    
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
        .def("to_dict", [](const NeuronId& id) {
            return py::dict("value"_a = id.value);
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

    // ===== ENUMERATIONS =====
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .value("Exc", NeuronType::Excitatory)
        .value("Inh", NeuronType::Inhibitory);

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .value("Exc", SynapseType::Excitatory)
        .value("Inh", SynapseType::Inhibitory);

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .value("Infant", DevelopmentalStage::Initial)
        .value("Young", DevelopmentalStage::CriticalPeriod)
        .value("Mature", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Senior", DevelopmentalStage::Aging);

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .value("Rest", FiringState::Resting)
        .value("Fire", FiringState::Active)
        .value("Ref", FiringState::Refractory);

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
        .export_values()
        .value("Forward", ActionType::MoveForward)
        .value("Backward", ActionType::MoveBackward)
        .value("Left", ActionType::MoveLeft)
        .value("Right", ActionType::MoveRight)
        .value("Look", ActionType::Look)
        .value("Interact", ActionType::Interact);

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .value("Forward", MotorCommand::MoveForward)
        .value("Backward", MotorCommand::MoveBackward)
        .value("Left", MotorCommand::TurnLeft)
        .value("Right", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight);

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall);

    // ===== CONFIGURATION =====
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        Provides methods to configure and validate NLM brain settings.
        Includes convenience methods for common configurations.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const Config&>(), py::arg("other"))
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
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get(key);
            if (!value) {
                throw NLMConfigError("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get a configuration value by key")
        .def("getWithDefault", [](Config& self, const std::string& key, auto defaultValue) {
            auto value = self.get(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set integer configuration")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set float configuration")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set string configuration")
        .def("setVector", [](Config& self, const std::string& key, const std::vector<double>& values) {
            self.set(key, values, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("values"), "Set vector configuration")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("to_dict", [](const Config& cfg) {
            py::dict result;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                auto value = cfg.get(key);
                if (value) {
                    // Try to determine type and convert
                    result[key] = *value;
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = py::cast<std::string>(item.first);
                auto value = item.second;
                
                if (py::isinstance(value, py::int_())) {
                    self.set(key, py::cast<int64_t>(value), nlm::ConfigSource::Default);
                } else if (py::isinstance(value, py::float_())) {
                    self.set(key, py::cast<double>(value), nlm::ConfigSource::Default);
                } else if (py::isinstance(value, py::str_())) {
                    self.set(key, py::cast<std::string>(value), nlm::ConfigSource::Default);
                } else if (py::isinstance(value, py::list_())) {
                    auto vec = py::cast<std::vector<double>>(value);
                    self.set(key, vec, nlm::ConfigSource::Default);
                } else {
                    throw NLMConfigError("Unsupported type for key: " + key);
                }
            }
        }, py::arg("dict"), "Load configuration from Python dictionary")
        .def("validate", [](const Config& cfg) {
            // Basic validation - can be extended
            auto keys = cfg.getKeys();
            std::vector<std::string> errors;
            
            if (!cfg.has("neuron_count")) {
                errors.push_back("Missing required key: neuron_count");
            } else {
                auto neuronCount = cfg.get<int64_t>("neuron_count");
                if (!neuronCount || *neuronCount <= 0) {
                    errors.push_back("Invalid neuron_count: must be positive");
                }
            }
            
            if (!errors.empty()) {
                throw NLMValidationError(errors.front());
            }
            
            return true;
        }, "Validate configuration and return True if valid")
        .def("clone", [](const Config& cfg) -> std::shared_ptr<Config> {
            auto newConfig = std::make_shared<Config>();
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                auto value = cfg.get(key);
                if (value) {
                    // Re-create with default source
                    if (auto intVal = std::get_if<int64_t>(&*value)) {
                        newConfig->set(key, *intVal, nlm::ConfigSource::Default);
                    } else if (auto floatVal = std::get_if<double>(&*value)) {
                        newConfig->set(key, *floatVal, nlm::ConfigSource::Default);
                    } else if (auto stringVal = std::get_if<std::string>(&*value)) {
                        newConfig->set(key, *stringVal, nlm::ConfigSource::Default);
                    } else if (auto vecVal = std::get_if<std::vector<double>>(&*value)) {
                        newConfig->set(key, *vecVal, nlm::ConfigSource::Default);
                    }
                }
            }
            return newConfig;
        }, "Create a copy of this configuration");

    // ===== FACTORY METHODS FOR CONVENIENCE =====
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");
    
    m.def("createSimulationConfig", [](int64_t neuronCount = 1000, int64_t regionCount = 4, 
                                     float connectionProbability = 0.1, uint64_t randomSeed = 42) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Default);
        config->set("region_count", regionCount, nlm::ConfigSource::Default);
        config->set("connection_probability", connectionProbability, nlm::ConfigSource::Default);
        config->set("random_seed", randomSeed, nlm::ConfigSource::Default);
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 4, 
       py::arg("connectionProbability") = 0.1, py::arg("randomSeed") = 42,
       "Create a configuration optimized for simulation");
    
    m.def("createSmallBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("random_seed", uint64_t(123), nlm::ConfigSource::Default);
        return config;
    }, "Create a small configuration for testing and rapid prototyping");
    
    m.def("createLargeBrainConfig", [](int64_t neuronCount = 10000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(8), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
        config->set("random_seed", uint64_t(456), nlm::ConfigSource::Default);
        return config;
    }, py::arg("neuronCount") = 10000, "Create a large configuration for realistic simulations");

    // ===== BRAIN FACTORY METHODS =====
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw NLMConfigError("Config cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMRuntimeError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a brain with the given configuration");
    
    m.def("createBrainFromDict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        config->from_dict(configDict);
        return std::make_shared<Brain>(config);
    }, py::arg("configDict"), "Create a brain from a Python dictionary configuration");

    // ===== BRAIN CLASS WITH ENHANCED FUNCTIONALITY =====
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        Core neural computation engine with spiking neurons, plasticity, and memory systems.
        Supports advanced features like neuromodulation, prediction, and cognition.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepWithTime", [](Brain& brain, int step) {
            brain.step(step, brain.getCurrentTime());
        }, py::arg("step"), "Perform a simulation step with automatic time tracking")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("injectCurrentBatch", [](Brain& brain, const py::dict& currents) {
            for (auto item : currents) {
                NeuronId neuron = py::cast<NeuronId>(item.first);
                double current = py::cast<double>(item.second);
                brain.injectCurrent(neuron, current);
            }
        }, py::arg("currents"), "Inject current to multiple neurons from dictionary")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("resetWithBackup", [](Brain& brain) {
            brain.save("/tmp/brain_backup.bin");
            brain.reset();
        }, "Reset brain state and create backup")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("loadWithValidation", [](Brain& brain, const std::string& filepath) {
            if (!brain.load(filepath)) {
                throw NLMRuntimeError("Failed to load brain state");
            }
            return true;
        }, py::arg("filepath"), "Load brain state with validation")
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
        .def("getCurrentTime", &Brain::getCurrentTime,
             "Get current simulation time")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStatistics", [](Brain& brain) {
            return py::dict(
                "total_neurons"_a = brain.getTotalNeuronCount(),
                "total_synapses"_a = brain.getTotalSynapseCount(),
                "active_neurons"_a = brain.getActiveNeuronCount(),
                "firing_neurons"_a = brain.getFiringNeuronCount(),
                "total_spikes"_a = brain.getTotalSpikeCount(),
                "avg_firing_rate"_a = brain.getAverageFiringRate(),
                "ei_ratio"_a = brain.getExcitationInhibitionRatio()
            );
        }, "Get comprehensive brain statistics as dictionary")
        .def("getStatisticsForExport", [](Brain& brain) {
            // More detailed statistics for export/import
            auto stats = brain.getStatistics();
            stats["simulation_time"] = brain.getCurrentTime();
            stats["development_stage"] = py::cast(brain.getDevelopmentalStage());
            return stats;
        }, "Get statistics for export/import")
        .def("isInitialized", [](Brain& brain) {
            return brain.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("estimateMemoryUsage", [](Brain& brain) {
            // Rough estimate of memory usage
            size_t neuronMemory = brain.getTotalNeuronCount() * sizeof(float) * 10; // Approximate
            size_t synapseMemory = brain.getTotalSynapseCount() * sizeof(float) * 4;
            return neuronMemory + synapseMemory;
        }, "Estimate memory usage in bytes")
        .def("exportState", [](Brain& brain) {
            py::dict state;
            state["neuron_count"] = brain.getTotalNeuronCount();
            state["synapse_count"] = brain.getTotalSynapseCount();
            state["firing_count"] = brain.getFiringNeuronCount();
            state["total_spikes"] = brain.getTotalSpikeCount();
            state["avg_firing_rate"] = brain.getAverageFiringRate();
            state["ei_ratio"] = brain.getExcitationInhibitionRatio();
            state["development_stage"] = py::cast(brain.getDevelopmentalStage());
            state["current_time"] = brain.getCurrentTime();
            return state;
        }, "Export brain state as Python dictionary")
        .def("importState", [](Brain& brain, const py::dict& state) {
            // Import state from dictionary (for testing/debugging)
            // Note: This doesn't actually restore state, just validates format
            if (!state.contains("neuron_count")) {
                throw NLMValidationError("Missing neuron_count in state");
            }
            // Could extend to actually restore state
            return true;
        }, py::arg("state"), "Import brain state from dictionary (validation only)")
        .def("runSimulation", [](Brain& brain, int numSteps) {
            for (int i = 0; i < numSteps; ++i) {
                brain.step(i);
            }
            return brain.getTotalSpikeCount();
        }, py::arg("numSteps"), "Run simulation for specified steps and return total spikes")
        .def("runControlledSimulation", [](Brain& brain, int numSteps, float timestep = 0.001f) {
            Timestamp currentTime = 0.0;
            for (int i = 0; i < numSteps; ++i) {
                brain.step(i, currentTime);
                currentTime += timestep;
            }
            return brain.getTotalSpikeCount();
        }, py::arg("numSteps"), py::arg("timestep") = 0.001f, "Run controlled simulation with time tracking");

    // ===== AGENT BRAIN CLASS WITH ENHANCED FUNCTIONALITY =====
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        High-level interface for agent-based simulations with sensory processing and action selection.
    )pbdoc")
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
        .def("processSensoryInputBatch", [](AgentBrain& agent, const py::list& percepts) {
            for (auto item : percepts) {
                SensoryPercept percept = py::cast<SensoryPercept>(item);
                agent.processSensoryInput(percept);
            }
        }, py::arg("percepts"), "Process multiple sensory inputs")
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
        .def("getPerformanceMetrics", [](AgentBrain& agent) {
            return py::dict(
                "neuromodulation"_a = agent.getNeuromodulationLevel(),
                "curiosity"_a = agent.getCuriosityLevel(),
                "novelty"_a = agent.getNoveltyLevel(),
                "prediction_error"_a = agent.getPredictionError(),
                "development_stage"_a = py::cast(agent.getDevelopmentalStage())
            );
        }, "Get performance metrics as dictionary")
        .def("createEpisode", [](AgentBrain& agent, const py::dict& episodeConfig) {
            // Reset agent for new episode
            agent.reset();
            
            // Apply episode-specific configuration if provided
            if (episodeConfig.contains("enable_reward_modulation")) {
                agent.enableRewardModulation(py::cast<bool>(episodeConfig["enable_reward_modulation"]));
            }
            if (episodeConfig.contains("enable_curiosity")) {
                agent.enableCuriosity(py::cast<bool>(episodeConfig["enable_curiosity"]));
            }
            
            return true;
        }, py::arg("episodeConfig") = py::dict(), "Create a new episode with optional configuration");

    // ===== SIMPLE WORLD CLASS WITH ENHANCED FUNCTIONALITY =====
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        Manages agent physics, interactions, and sensory input processing.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("configureWithDefaults", [](SimpleWorld& world) {
            world.configure(100, 100, 16, 16); // Reasonable defaults
        }, "Configure with reasonable defaults")
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
        .def("getPerformanceMetrics", [](SimpleWorld& world) {
            return py::dict(
                "width"_a = world.getWidth(),
                "height"_a = world.getHeight(),
                "simulation_time"_a = world.getSimulationTime(),
                "agent_x"_a = world.getAgentBody().x,
                "agent_y"_a = world.getAgentBody().y,
                "agent_energy"_a = world.getAgentBody().energy,
                "agent_health"_a = world.getAgentBody().health
            );
        }, "Get world performance metrics as dictionary")
        .def("simulateEpisode", [](SimpleWorld& world, AgentBrain& agent, int steps) {
            agent.reset();
            
            for (int i = 0; i < steps; ++i) {
                world.update(0.1f); // 100ms timestep
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                // Note: Brain step would be called elsewhere
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
            }
            
            return world.getSimulationTime();
        }, py::arg("agent"), py::arg("steps"), "Simulate an episode with agent");

    // ===== SENSORY INPUT CLASS =====
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        Abstract base class for sensory modalities (vision, audio, internal signals).
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        2D visual input with width, height, and color channels.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromNumpy", [](Vision& self, py::array_t<float> data) {
            auto buf = data.request();
            std::vector<float> vec(buf.size);
            std::copy(static_cast<float*>(buf.ptr), 
                     static_cast<float*>(buf.ptr) + buf.size, vec.begin());
            self.setData(vec);
        }, py::arg("data"), "Set vision data from numpy array")
        .def("getDataAsNumpy", &Vision::getData)
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_dict", [](const Vision& v) {
            return py::dict(
                "width"_a = v.getWidth(),
                "height"_a = v.getHeight(),
                "channels"_a = v.getChannels(),
                "data"_a = v.getData()
            );
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        Audio input with sample rate and number of samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        Internal state signals (proprioception, emotional states, etc.).
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getSignals)
        .def("addSignalBatch", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) {
                self.addSignal(val);
            }
        }, py::arg("values"), "Add multiple signals at once");

    // ===== ACTION CLASS =====
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        Motor action with type and parameters for world interaction.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("updateParameters", [](Action& self, const py::dict& params) {
            std::vector<float> vec;
            for (auto item : params) {
                vec.push_back(py::cast<float>(item.second));
            }
            self.setParameters(vec);
        }, py::arg("params"), "Update action parameters from dictionary")
        .def("getParameterValue", [](const Action& self, const std::string& name) {
            auto params = self.getParameters();
            if (name == "speed" && params.size() > 0) return params[0];
            if (name == "rotation" && params.size() > 1) return params[1];
            if (name == "intensity" && params.size() > 2) return params[2];
            return 0.0f;
        }, py::arg("name"), "Get specific parameter value by name")
        .def("to_dict", [](const Action& a) {
            return py::dict(
                "type"_a = py::cast(a.getType()),
                "parameters"_a = a.getParameters(),
                "name"_a = a.getName()
            );
        });

    // ===== ADVANCED UTILITIES =====
    
    m.def("createVisionFromArray", [](py::array_t<float> data, size_t width, size_t height, size_t channels = 3) {
        auto buf = data.request();
        std::vector<float> vec(buf.size);
        std::copy(static_cast<float*>(buf.ptr), 
                 static_cast<float*>(buf.ptr) + buf.size, vec.begin());
        
        Vision vision(width, height, channels);
        vision.setData(vec);
        return vision;
    }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create Vision sensory input from numpy array");
    
    m.def("createActionFromDict", [](const py::dict& actionDict) {
        ActionType type = py::cast<ActionType>(actionDict["type"]);
        std::vector<float> params;
        if (actionDict.contains("parameters")) {
            params = py::cast<std::vector<float>>(actionDict["parameters"]);
        }
        return Action(type, params);
    }, py::arg("actionDict"), "Create Action from Python dictionary");
    
    m.def("createSensoryPerceptFromVision", [](const Vision& vision) {
        SensoryPercept percept;
        percept.setVision(vision);
        return percept;
    }, py::arg("vision"), "Create SensoryPercept from Vision input");
    
    // ===== CONSTANTS AND FACTORIES =====
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // ===== PERFORMANCE MONITORING =====
    
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return py::dict(
            "steps"_a = steps,
            "duration_ms"_a = duration.count(),
            "steps_per_second"_a = (steps * 1000.0) / duration.count(),
            "total_spikes"_a = brain->getTotalSpikeCount()
        );
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");
    
    // ===== EXPERIMENTATION HELPERS =====
    
    m.def("runStandardExperiment", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int steps) {
        agent->reset();
        world->reset();
        
        float totalReward = 0.0f;
        int stepsCompleted = 0;
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply simple reward system
            float reward = action.getType() == ActionType::MoveForward ? 0.1f : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            totalReward += reward;
            stepsCompleted++;
        }
        
        return py::dict(
            "steps_completed"_a = stepsCompleted,
            "total_reward"_a = totalReward,
            "avg_reward"_a = totalReward / stepsCompleted,
            "final_time"_a = world->getSimulationTime()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), "Run a standard experiment");
    
    // ===== UTILITY FUNCTIONS =====
    
    m.def("compareBrains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, int steps) {
        py::dict comparison;
        
        for (int i = 0; i < steps; ++i) {
            brain1->step(i);
            brain2->step(i);
        }
        
        comparison["brain1_neuron_count"] = brain1->getTotalNeuronCount();
        comparison["brain2_neuron_count"] = brain2->getTotalNeuronCount();
        comparison["brain1_spike_count"] = brain1->getTotalSpikeCount();
        comparison["brain2_spike_count"] = brain2->getTotalSpikeCount();
        comparison["brain1_firing_rate"] = brain1->getAverageFiringRate();
        comparison["brain2_firing_rate"] = brain2->getAverageFiringRate();
        comparison["brain1_ei_ratio"] = brain1->getExcitationInhibitionRatio();
        comparison["brain2_ei_ratio"] = brain2->getExcitationInhibitionRatio();
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), py::arg("steps"), "Compare two brains after running them for same number of steps");
    
    m.def("createRandomConfig", [](uint64_t seed = 42) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", seed, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        return config;
    }, py::arg("seed") = 42, "Create a random configuration with specified seed");
    
    m.def("validateBrainConfiguration", [](const Config& config) {
        auto keys = config.getKeys();
        std::vector<std::string> errors;
        
        if (!config.has("neuron_count")) {
            errors.push_back("neuron_count is required");
        } else {
            auto neuronCount = config.get<int64_t>("neuron_count");
            if (!neuronCount || *neuronCount <= 0) {
                errors.push_back("neuron_count must be positive");
            }
        }
        
        if (!config.has("region_count")) {
            errors.push_back("region_count is required");
        }
        
        if (!errors.empty()) {
            throw NLMValidationError("Configuration validation failed: " + errors.front());
        }
        
        return true;
    }, py::arg("config"), "Validate brain configuration and return True if valid");

} // namespace nlm
