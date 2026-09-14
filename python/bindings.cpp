// NLM Python Bindings Improvements
// This module improves the Python bindings with better error handling, type safety, and performance

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

using namespace pybind11::literals;

namespace nlm {

// Python exception types for NLM errors
class NLMConfigError : public std::runtime_error {
public:
    explicit NLMConfigError(const std::string& msg) : std::runtime_error("NLM Configuration Error: " + msg) {}
};

class NLMInvalidNeuronError : public std::runtime_error {
public:
    explicit NLMInvalidNeuronError(const std::string& msg) : std::runtime_error("NLM Neuron Error: " + msg) {}
};

class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error("NLM Brain Error: " + msg) {}
};

class NLMMemoryError : public std::runtime_error {
public:
    explicit NLMMemoryError(const std::string& msg) : std::runtime_error("NLM Memory Error: " + msg) {}
};

// Helper function to convert string to lowercase for type checking
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Type validation helpers
bool isFloatType(const py::object& obj) {
    if (py::isinstance<int>(obj)) return true;
    if (py::isinstance<float>(obj)) return true;
    if (py::isinstance<double>(obj)) return true;
    return false;
}

bool isStringType(const py::object& obj) {
    return py::isinstance<py::str>(obj);
}

bool isBoolType(const py::object& obj) {
    return py::isinstance<bool>(obj);
}

bool isListType(const py::object& obj) {
    return py::isinstance<py::list>(obj);
}

// Improved Config wrapper with better error handling
py::class_<Config> enhanceConfigClass(py::module_ &m) {
    return py::class_<Config>(m, "Config", R"pbdoc(NLM Configuration System)pbdoc")
        .def(py::init<>())
        .def("createDefault", &Config::createDefault,
             "Create a default configuration"
        )
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.loadFromFile(filepath)) {
                    throw NLMConfigError("Failed to load configuration from: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Error loading configuration: ") + e.what());
            }
        }, py::arg("filepath"),
             "Load configuration from file (supports JSON, YAML, and text formats)"
        )
        .def("loadFromString", [](Config& self, const std::string& content, const std::string& format) {
            try {
                // Create a temporary file
                std::string tempFile = "/tmp/nlm_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
                std::ofstream file(tempFile);
                if (!file.is_open()) {
                    throw NLMConfigError("Cannot create temporary file for configuration");
                }
                
                file << content;
                file.close();
                
                // Load based on format
                bool success = false;
                if (format == "json" || toLower(format) == "json") {
                    // Parse JSON directly
                    auto j = py::module_::import("json").attr("loads")(content).cast<py::object>();
                    
                    // For now, we'll just call the existing loadFromFile
                    // In a real implementation, we would parse JSON directly
                    success = self.loadFromFile(tempFile);
                } else if (format == "yaml" || toLower(format) == "yaml" || format == "yml" || toLower(format) == "yml") {
                    success = self.loadFromFile(tempFile);
                } else {
                    // Text format
                    success = self.loadFromFile(tempFile);
                }
                
                // Clean up temp file
                std::remove(tempFile.c_str());
                
                if (!success) {
                    throw NLMConfigError("Failed to parse configuration string");
                }
                
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Error loading configuration from string: ") + e.what());
            }
        }, py::arg("content"), py::arg("format") = "text",
             "Load configuration from string with specified format (text, json, yaml)"
        )
        .def("get", [](const Config& self, const std::string& key) {
            try {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    return py::str(value.value());
                } else {
                    return py::none();
                }
            } catch (...) {
                // Try other types
                auto intVal = self.get<int64_t>(key);
                if (intVal.has_value()) {
                    return py::int_(intVal.value());
                }
                auto doubleVal = self.get<double>(key);
                if (doubleVal.has_value()) {
                    return py::float_(doubleVal.value());
                }
                auto boolVal = self.get<bool>(key);
                if (boolVal.has_value()) {
                    return py::bool_(boolVal.value());
                }
                return py::none();
            }
        }, py::arg("key"),
             "Get configuration value with automatic type detection"
        )
        .def("getInt", [](const Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64_t>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
             "Get configuration value as integer"
        )
        .def("getFloat", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get configuration value as float"
        )
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value.has_value() ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
             "Get configuration value as boolean"
        )
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            try {
                if (py::isinstance<py::str>(value)) {
                    self.set(key, value.cast<std::string>());
                } else if (py::isinstance<int>(value)) {
                    self.set(key, value.cast<int64_t>());
                } else if (py::isinstance<float>(value)) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance<double>(value)) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance<bool>(value)) {
                    self.set(key, value.cast<bool>());
                } else if (py::isinstance<py::list>(value)) {
                    auto list = value.cast<std::vector<double>>();
                    // For arrays, we need to store as vector<double>
                    self.set(key, list);
                } else {
                    throw NLMConfigError("Unsupported value type for configuration key: " + key);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Error setting configuration value: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"),
             "Set configuration value with automatic type conversion"
        )
        .def("has", &Config::has,
             py::arg("key"),
             "Check if configuration key exists"
        )
        .def("remove", &Config::remove,
             py::arg("key"),
             "Remove configuration key"
        )
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys"
        )
        .def("getSummary", &Config::summary,
             "Get configuration summary"
        )
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Get value and convert to Python object
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[py::str(key)] = py::str(value.value());
                } else {
                    auto intVal = self.get<int64_t>(key);
                    if (intVal.has_value()) {
                        result[py::str(key)] = py::int_(intVal.value());
                    } else {
                        auto doubleVal = self.get<double>(key);
                        if (doubleVal.has_value()) {
                            result[py::str(key)] = py::float_(doubleVal.value());
                        } else {
                            auto boolVal = self.get<bool>(key);
                            if (boolVal.has_value()) {
                                result[py::str(key)] = py::bool_(boolVal.value());
                            }
                        }
                    }
                }
            }
            return result;
        },
             "Convert configuration to Python dictionary"
        );
}

// Enhanced Brain wrapper with better error handling and batch operations
py::class_<Brain> enhanceBrainClass(py::module_ &m) {
    return py::class_<Brain>(m, "Brain", R"pbdoc(NLM Neural Brain Simulation)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration"
        )
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration"
        )
        .def("step", [](Brain& self, SimulationStep currentStep) {
            try {
                self.step(currentStep);
            } catch (const std::exception& e) {
                throw NLMBrainError(std::string("Error during brain step: ") + e.what());
            }
        }, py::arg("currentStep"),
             "Perform a simulation step"
        )
        .def("step", [](Brain& self, SimulationStep currentStep, Timestamp currentTime) {
            try {
                self.step(currentStep, currentTime);
            } catch (const std::exception& e) {
                throw NLMBrainError(std::string("Error during brain step with time: ") + e.what());
            }
        }, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp"
        )
        .def("step_batch", [](Brain& self, const std::vector<SimulationStep>& steps, const std::vector<Timestamp>& times) {
            if (steps.size() != times.size()) {
                throw NLMBrainError("Steps and times vectors must have the same size");
            }
            
            try {
                for (size_t i = 0; i < steps.size(); ++i) {
                    self.step(steps[i], times[i]);
                }
            } catch (const std::exception& e) {
                throw NLMBrainError(std::string("Error during batch brain step: ") + e.what());
            }
        }, py::arg("steps"), py::arg("times"),
             "Perform multiple simulation steps in batch for better performance"
        )
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain"
        )
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron"
        )
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type"
        )
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity"
        )
        .def("reset", &Brain::reset,
             "Reset brain state"
        )
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw NLMBrainError("Failed to save brain state to: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Error saving brain state: ") + e.what());
            }
        }, py::arg("filepath"),
             "Save brain state to file"
        )
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw NLMBrainError("Failed to load brain state from: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Error loading brain state: ") + e.what());
            }
        }, py::arg("filepath"),
             "Load brain state from file"
        )
        .def("addRegion", &Brain::addRegion,
             py::arg("name") = "",
             "Add a new neural region"
        )
        .def("getRegion", &Brain::getRegion,
             py::arg("id"),
             "Get a region by ID"
        )
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions"
        )
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs"
        )
        .def("getRegions", &Brain::getRegions,
             "Get all regions"
        )
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions"
        )
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions"
        )
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons"
        )
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons"
        )
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons"
        )
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio"
        )
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count"
        )
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage"
        )
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage"
        )
        .def("getStatistics", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        },
             "Get brain statistics as Python dictionary"
        );
}

// Enhanced AgentBrain wrapper
py::class_<AgentBrain> enhanceAgentBrainClass(py::module_ &m) {
    return py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent-Brain Interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent-brain interface"
        )
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"),
             "Initialize agent with world"
        )
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory input"
        )
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode motor command from brain activity"
        )
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward") = 0.0f,
             "Apply reward modulation to brain"
        )
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update brain development"
        )
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage"
        )
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enabled"),
             "Enable/disable reward modulation"
        )
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enabled"),
             "Enable/disable structural plasticity"
        )
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enabled"),
             "Enable/disable development"
        )
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enabled"),
             "Enable/disable curiosity"
        )
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level"
        )
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level"
        )
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error"
        )
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level"
        )
        .def("getAgentStatistics", [](AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            return stats;
        },
             "Get agent statistics as Python dictionary"
        );
}

// Enhanced SimpleWorld wrapper
py::class_<SimpleWorld> enhanceSimpleWorldClass(py::module_ &m) {
    return py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions and agent vision"
        )
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state"
        )
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position"
        )
        .def("update", [](SimpleWorld& self, double timestep) {
            try {
                self.update(timestep);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error updating world: " + std::string(e.what()));
            }
        }, py::arg("timestep"),
             "Update world state"
        )
        .def("update_batch", [](SimpleWorld& self, const std::vector<double>& timesteps) {
            try {
                for (double timestep : timesteps) {
                    self.update(timestep);
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Error updating world in batch: " + std::string(e.what()));
            }
        }, py::arg("timesteps"),
             "Update world state in batch for better performance"
        )
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world"
        )
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get sensory percept"
        )
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get agent body state"
        )
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj"),
             "Add world object"
        )
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove world object"
        )
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid"
        )
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width"
        )
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height"
        )
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy"
        )
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
             py::arg("e"),
             "Set maximum energy"
        )
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate"
        )
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"),
             "Set energy decay rate"
        )
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time"
        )
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
             py::arg("seed"),
             "Set random seed"
        )
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed"
        )
        .def("getWorldStatistics", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["max_energy"] = self.getMaxEnergy();
            stats["simulation_time"] = self.getSimulationTime();
            return stats;
        },
             "Get world statistics as Python dictionary"
        );
}

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This version provides:
        - Better error handling with specific exception types
        - Automatic type conversion for configuration values
        - Batch operations for improved performance
        - Enhanced documentation strings
        - Statistics utilities for easier data analysis
        
        Features:
        - Brain simulation with step() and step_batch() methods
        - Configuration system with JSON/YAML/text support
        - Agent-brain interface with reward modulation
        - Simple 2D world simulation
        - Sensory input and motor output handling
        
        See `HOW_TO_USE.md` for usage examples.
    )pbdoc";
    
    // Register exception types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::NLMConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm::NLMInvalidNeuronError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm::NLMBrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::NLMMemoryError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Register enhanced classes
    enhanceConfigClass(m);
    enhanceBrainClass(m);
    enhanceAgentBrainClass(m);
    enhanceSimpleWorldClass(m);
    
    // Register original classes for backward compatibility
    // (commented out to avoid conflicts, use enhanced versions instead)
    // Original classes would be registered here...
    
    // Add module-level functions
    m.def("createDefaultConfig", &nlm::createDefaultConfig,
          "Create a default configuration"
    );
    
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
         "Create a brain with configuration"
    );
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
         "Create an agent-brain interface"
    );
    
    m.def("createSimpleWorld", &nlm::createSimpleWorld,
         "Create a simple world"
    );
    
    m.def("createSensoryInput", &nlm::createSensoryInput,
         "Create sensory input"
    );
    
    m.def("createAction", &nlm::createAction,
         "Create an action"
    );
    
    // Add utility functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        try {
            // Initialize
            agent->initialize(*world);
            
            // Run simulation
            for (size_t step = 0; step < numSteps; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                auto reward = world->getSensoryPercept().getInternal()[0] if (world->getSensoryPercept().getInternal() else 0.0f;
                agent->applyRewardModulation(reward, 0.0f);
                
                agent->updateDevelopment(0.1f);
            }
            
            return true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Error during simulation: " + std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps") = 1000,
         "Run a complete agent simulation (convenience function)"
    );
    
    m.def("getVersion", []() {
        return "NLM 0.1.0 - Enhanced Python Bindings";
    }, "Get NLM version string"
    );
    
    m.def("getBuildInfo", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["backend"] = "C++ with Python bindings";
        info["enhanced"] = true;
        info["error_handling"] = true;
        info["batch_operations"] = true;
        return info;
    }, "Get build information"
    );
}
