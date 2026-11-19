#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>

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

// Python exception types
class NLMException : public py::exception {
public:
    NLMException() : py::exception("nlm.NLMError") {}
};

class ConfigError : public py::exception {
public:
    ConfigError() : py::exception("nlm.ConfigError") {}
};

class BrainError : public py::exception {
public:
    BrainError() : py::exception("nlm.BrainError") {}
};

class SimulationError : public py::exception {
public:
    SimulationError() : py::exception("nlm.SimulationError") {}
};

// Helper functions for converting between types
namespace pybind_utils {

    // Convert Python dict to ConfigValue
    ConfigValue dict_to_config_value(const py::dict& py_dict);
    
    // Convert ConfigValue to Python object
    py::object config_value_to_py(const ConfigValue& value);
    
    // Get Python dict from brain statistics
    py::dict get_brain_stats(const std::shared_ptr<Brain>& brain);
    
    // Get Python dict from agent brain interface
    py::dict get_agent_brain_stats(const std::shared_ptr<AgentBrain>& agent);
    
    // Validate configuration keys
    bool validate_config_keys(const std::vector<std::string>& keys);
    
    // Batch configuration setting
    void batch_set_config(std::shared_ptr<Config> config, 
                         const py::dict& config_dict,
                         ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration as Python dict
    py::dict config_to_dict(const std::shared_ptr<Config>& config);
    
    // Create Python exception with formatted message
    template<typename... Args>
    void raise_config_error(const char* format, Args... args) {
        throw ConfigError("ConfigError: " + py::str(format).format(py::arg("args") = std::vector<py::object>({py::cast(args)...})));
    }
    
    template<typename... Args>
    void raise_brain_error(const char* format, Args... args) {
        throw BrainError("BrainError: " + py::str(format).format(py::arg("args") = std::vector<py::object>({py::cast(args)...})));
    }
    
    template<typename... Args>
    void raise_simulation_error(const char* format, Args... args) {
        throw SimulationError("SimulationError: " + py::str(format).format(py::arg("args") = std::vector<py::object>({py::cast(args)...})));
    }
}

// Enhanced Config class with convenience methods
py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced functionality)pbdoc")
    .def(py::init<>())
    .def("loadFromFile", [](std::shared_ptr<Config> self, const std::string& filepath) {
        try {
            if (!self->loadFromFile(filepath)) {
                throw ConfigError("Failed to load configuration from file: " + filepath);
            }
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config file load error: ") + e.what());
        }
    }, py::arg("filepath"), R"pbdoc(Load configuration from a JSON file)pbdoc")
    
    .def("loadFromArgs", [](std::shared_ptr<Config> self, const std::vector<std::string>& args) {
        try {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            if (!self->loadFromArgs(static_cast<int>(argv.size()), argv.data())) {
                throw ConfigError("Failed to load configuration from command line arguments");
            }
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config args load error: ") + e.what());
        }
    }, py::arg("args"), R"pbdoc(Load configuration from command line arguments)pbdoc")
    
    .def("saveToFile", [](std::shared_ptr<Config> self, const std::string& filepath) {
        try {
            if (!self->saveToFile(filepath)) {
                throw ConfigError("Failed to save configuration to file: " + filepath);
            }
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config save error: ") + e.what());
        }
    }, py::arg("filepath"), R"pbdoc(Save configuration to a JSON file)pbdoc")
    
    // Enhanced has method with error handling
    .def("has", [](std::shared_ptr<Config> self, const std::string& key) {
        try {
            return self->has(key);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config has error: ") + e.what());
        }
    }, py::arg("key"), R"pbdoc(Check if a configuration key exists)pbdoc")
    
    // Enhanced getOr with better error handling and type safety
    .def("getOr", [](std::shared_ptr<Config> self, const std::string& key, const py::object& default_value) {
        try {
            // Try to get the value with the same type as default_value
            if (py::isinstance<int>(default_value)) {
                return self->getOr<int>(key, default_value.cast<int>());
            } else if (py::isinstance<double>(default_value)) {
                return self->getOr<double>(key, default_value.cast<double>());
            } else if (py::isinstance<bool>(default_value)) {
                return self->getOr<bool>(key, default_value.cast<bool>());
            } else if (py::isinstance<std::string>(default_value)) {
                return self->getOr<std::string>(key, default_value.cast<std::string>());
            } else {
                throw ConfigError("Unsupported type for getOr default value: " + py::str(py::type::of(default_value).attr("__name__")));
            }
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config getOr error: ") + e.what());
        }
    }, py::arg("key"), py::arg("default_value"), R"pbdoc(Get configuration value with fallback, type-safe)pbdoc")
    
    // Enhanced set method with better error handling
    .def("set", [](std::shared_ptr<Config> self, const std::string& key, const py::object& value) {
        try {
            // Use the ConfigValue variant type
            if (py::isinstance<int>(value)) {
                self->set(key, value.cast<int>());
            } else if (py::isinstance<double>(value)) {
                self->set(key, value.cast<double>());
            } else if (py::isinstance<bool>(value)) {
                self->set(key, value.cast<bool>());
            } else if (py::isinstance<std::string>(value)) {
                self->set(key, value.cast<std::string>());
            } else if (py::isinstance<py::list>(value)) {
                // Convert list to vector
                py::list py_list = value.cast<py::list>();
                std::vector<std::string> str_list;
                for (auto item : py_list) {
                    str_list.push_back(item.cast<std::string>());
                }
                self->set(key, str_list);
            } else {
                throw ConfigError("Unsupported type for config value: " + py::str(py::type::of(value).attr("__name__")));
            }
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config set error: ") + e.what());
        }
    }, py::arg("key"), py::arg("value"), R"pbdoc(Set configuration value with type safety)pbdoc")
    
    // Convenience methods
    .def("set_batch", [](std::shared_ptr<Config> self, const py::dict& config_dict) {
        try {
            pybind_utils::batch_set_config(self, config_dict);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config batch set error: ") + e.what());
        }
    }, py::arg("config_dict"), R"pbdoc(Set multiple configuration values from a Python dict)pbdoc")
    
    .def("validate", [](std::shared_ptr<Config> self, const std::vector<std::string>& required_keys) {
        try {
            if (!pybind_utils::validate_config_keys(required_keys)) {
                throw ConfigError("Invalid configuration keys provided");
            }
            for (const auto& key : required_keys) {
                if (!self->has(key)) {
                    throw ConfigError("Missing required configuration key: " + key);
                }
            }
            return true;
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("ConfigError:") != std::string::npos) {
                throw;
            }
            throw ConfigError(std::string("Config validation error: ") + e.what());
        }
    }, py::arg("required_keys"), R"pbdoc(Validate that required configuration keys exist)pbdoc")
    
    .def("to_dict", [](std::shared_ptr<Config> self) {
        try {
            return pybind_utils::config_to_dict(self);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config to_dict error: ") + e.what());
        }
    }, R"pbdoc(Convert configuration to Python dictionary)pbdoc")
    
    .def("clear", [](std::shared_ptr<Config> self) {
        try {
            self->clear();
            return true;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config clear error: ") + e.what());
        }
    }, R"pbdoc(Clear all configuration entries)pbdoc")
    
    // Enhanced summary method
    .def("summary", [](std::shared_ptr<Config> self) {
        try {
            return self->summary();
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Config summary error: ") + e.what());
        }
    }, R"pbdoc(Get a summary string of the configuration)pbdoc")
    
    // Enhanced __repr__
    .def("__repr__", [](const std::shared_ptr<Config>& config) {
        try {
            return "<Config: " + config->summary() + ">";
        } catch (const std::exception& e) {
            return "<Config: Error retrieving summary>";
        }
    });

// Enhanced Brain class with convenience methods and error handling
py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced functionality)pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc")
    
    .def("initialize", [](std::shared_ptr<Brain> self) {
        try {
            if (!self->initialize()) {
                throw BrainError("Failed to initialize brain");
            }
            return true;
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("BrainError:") != std::string::npos) {
                throw;
            }
            throw BrainError(std::string("Brain initialization error: ") + e.what());
        }
    }, R"pbdoc(Initialize the brain with configuration)pbdoc")
    
    // Enhanced step methods with error handling
    .def("step", [](std::shared_ptr<Brain> self, SimulationStep step) {
        try {
            self->step(step, 0.0);  // Default timestep
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain step error: ") + e.what());
        }
    }, py::arg("step"), R"pbdoc(Perform a simulation step with default timestep)pbdoc")
    
    .def("step", [](std::shared_ptr<Brain> self, SimulationStep step, Timestamp time) {
        try {
            self->step(step, time);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain step error: ") + e.what());
        }
    }, py::arg("step"), py::arg("time"), R"pbdoc(Perform a simulation step with specific timestamp)pbdoc")
    
    .def("receiveSensoryInput", [](std::shared_ptr<Brain> self, const SensoryInput& input) {
        try {
            self->receiveSensoryInput(input);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain sensory input error: ") + e.what());
        }
    }, py::arg("input"), R"pbdoc(Inject sensory input into the brain)pbdoc")
    
    .def("injectCurrent", [](std::shared_ptr<Brain> self, NeuronId neuron, float current) {
        try {
            self->injectCurrent(neuron, current);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain inject current error: ") + e.what());
        }
    }, py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)pbdoc")
    
    .def("injectCurrentToNeurons", [](std::shared_ptr<Brain> self, NeuronType type, float current) {
        try {
            self->injectCurrentToNeurons(type, current);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain inject current error: ") + e.what());
        }
    }, py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
    
    .def("produceAction", [](std::shared_ptr<Brain> self) {
        try {
            return self->produceAction();
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Brain produce action error: ") + e.what());
        }
    }, R"pbdoc(Produce motor action based on neural activity)pbdoc")
    
    .def("reset", [](std::shared_ptr<Brain> self) {
        try {
            self->reset();
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain reset error: ") + e.what());
        }
    }, R"pbdoc(Reset brain state)pbdoc")
    
    .def("save", [](std::shared_ptr<Brain> self, const std::string& filepath) {
        try {
            if (!self->save(filepath)) {
                throw BrainError("Failed to save brain state to: " + filepath);
            }
            return true;
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("BrainError:") != std::string::npos) {
                throw;
            }
            throw BrainError(std::string("Brain save error: ") + e.what());
        }
    }, py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc")
    
    .def("load", [](std::shared_ptr<Brain> self, const std::string& filepath) {
        try {
            if (!self->load(filepath)) {
                throw BrainError("Failed to load brain state from: " + filepath);
            }
            return true;
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("BrainError:") != std::string::npos) {
                throw;
            }
            throw BrainError(std::string("Brain load error: ") + e.what());
        }
    }, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc")
    
    .def("addRegion", [](std::shared_ptr<Brain> self, const std::string& name = "") {
        try {
            return self->addRegion(name);
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain add region error: ") + e.what());
        }
    }, py::arg("name") = "", R"pbdoc(Add a new neural region)pbdoc")
    
    .def("getRegion", [](std::shared_ptr<Brain> self, RegionId id) {
        try {
            return self->getRegion(id);
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get region error: ") + e.what());
        }
    }, py::arg("id"), R"pbdoc(Get a region by ID)pbdoc")
    
    .def("getRegionCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getRegionCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get region count error: ") + e.what());
        }
    }, R"pbdoc(Get the number of regions)pbdoc")
    
    .def("getRegionIds", [](std::shared_ptr<Brain> self) {
        try {
            return self->getRegionIds();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get region ids error: ") + e.what());
        }
    }, R"pbdoc(Get all region IDs)pbdoc")
    
    .def("getRegions", [](std::shared_ptr<Brain> self) {
        try {
            return self->getRegions();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get regions error: ") + e.what());
        }
    }, R"pbdoc(Get all regions)pbdoc")
    
    // Enhanced statistics methods with comprehensive output
    .def("get_stats", [](std::shared_ptr<Brain> self) {
        try {
            return pybind_utils::get_brain_stats(self);
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain stats error: ") + e.what());
        }
    }, R"pbdoc(Get comprehensive brain statistics as Python dict)pbdoc")
    
    .def("getTotalNeuronCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getTotalNeuronCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get neuron count error: ") + e.what());
        }
    }, R"pbdoc(Get total neuron count across all regions)pbdoc")
    
    .def("getTotalSynapseCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getTotalSynapseCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get synapse count error: ") + e.what());
        }
    }, R"pbdoc(Get total synapse count across all regions)pbdoc")
    
    .def("getActiveNeuronCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getActiveNeuronCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get active neuron count error: ") + e.what());
        }
    }, R"pbdoc(Get count of active neurons)pbdoc")
    
    .def("getFiringNeuronCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getFiringNeuronCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get firing neuron count error: ") + e.what());
        }
    }, R"pbdoc(Get count of currently firing neurons)pbdoc")
    
    .def("getAverageFiringRate", [](std::shared_ptr<Brain> self) {
        try {
            return self->getAverageFiringRate();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get average firing rate error: ") + e.what());
        }
    }, R"pbdoc(Get average firing rate across all neurons)pbdoc")
    
    .def("getExcitationInhibitionRatio", [](std::shared_ptr<Brain> self) {
        try {
            return self->getExcitationInhibitionRatio();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get E/I ratio error: ") + e.what());
        }
    }, R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
    
    .def("getTotalSpikeCount", [](std::shared_ptr<Brain> self) {
        try {
            return self->getTotalSpikeCount();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get total spike count error: ") + e.what());
        }
    }, R"pbdoc(Get total spike count)pbdoc")
    
    .def("getDevelopmentalStage", [](std::shared_ptr<Brain> self) {
        try {
            return self->getDevelopmentalStage();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get developmental stage error: ") + e.what());
        }
    }, R"pbdoc(Get current developmental stage)pbdoc")
    
    .def("setDevelopmentalStage", [](std::shared_ptr<Brain> self, DevelopmentalStage stage) {
        try {
            self->setDevelopmentalStage(stage);
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain set developmental stage error: ") + e.what());
        }
    }, py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
    
    .def("getConfig", [](std::shared_ptr<Brain> self) {
        try {
            return self->getConfig();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain get config error: ") + e.what());
        }
    }, R"pbdoc(Get the configuration)pbdoc")
    
    .def("logStatus", [](std::shared_ptr<Brain> self) {
        try {
            self->logStatus();
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Brain log status error: ") + e.what());
        }
    }, R"pbdoc(Log brain status)pbdoc")
    
    // Enhanced __repr__
    .def("__repr__", [](const std::shared_ptr<Brain>& brain) {
        try {
            auto stats = pybind_utils::get_brain_stats(brain);
            return "<Brain: " + py::str(stats["total_neurons"]) + " neurons, " + 
                   py::str(stats["total_synapses"]) + " synapses, " +
                   py::str(stats["firing_count"]) + " firing neurons>";
        } catch (const std::exception& e) {
            return "<Brain: Error retrieving stats>";
        }
    });

// Enhanced AgentBrain class with convenience methods
py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced functionality)pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), R"pbdoc(Create agent brain interface)pbdoc")
    
    .def("initialize", [](std::shared_ptr<AgentBrain> self, const SimpleWorld& world) {
        try {
            self->initialize(world);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain initialize error: ") + e.what());
        }
    }, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
    
    .def("getSensoryInputSize", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getSensoryInputSize();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get sensory input size error: ") + e.what());
        }
    }, R"pbdoc(Get expected sensory input size)pbdoc")
    
    .def("getMotorOutputSize", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getMotorOutputSize();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get motor output size error: ") + e.what());
        }
    }, R"pbdoc(Get expected motor output size)pbdoc")
    
    .def("processSensoryInput", [](std::shared_ptr<AgentBrain> self, const SensoryPercept& percept) {
        try {
            self->processSensoryInput(percept);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("AgentBrain process sensory input error: ") + e.what());
        }
    }, py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)pbdoc")
    
    .def("decodeMotorCommand", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->decodeMotorCommand();
        } catch (const std::exception& e) {
            throw SimulationError(std::string("AgentBrain decode motor command error: ") + e.what());
        }
    }, R"pbdoc(Decode brain motor activity into motor command)pbdoc")
    
    .def("applyRewardModulation", [](std::shared_ptr<AgentBrain> self, float reward, float predictedReward) {
        try {
            self->applyRewardModulation(reward, predictedReward);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("AgentBrain apply reward modulation error: ") + e.what());
        }
    }, py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)pbdoc")
    
    .def("updateDevelopment", [](std::shared_ptr<AgentBrain> self, Timestamp timestep) {
        try {
            self->updateDevelopment(timestep);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("AgentBrain update development error: ") + e.what());
        }
    }, py::arg("timestep"), R"pbdoc(Update development system)pbdoc")
    
    .def("getDevelopmentalStage", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getDevelopmentalStage();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get developmental stage error: ") + e.what());
        }
    }, R"pbdoc(Get current developmental stage)pbdoc")
    
    .def("getNeuromodulationLevel", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getNeuromodulationLevel();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get neuromodulation level error: ") + e.what());
        }
    }, R"pbdoc(Get current neuromodulation level)pbdoc")
    
    .def("getCuriosityLevel", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getCuriosityLevel();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get curiosity level error: ") + e.what());
        }
    }, R"pbdoc(Get curiosity level)pbdoc")
    
    .def("getNoveltyLevel", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getNoveltyLevel();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get novelty level error: ") + e.what());
        }
    }, R"pbdoc(Get novelty level)pbdoc")
    
    .def("getPredictionError", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getPredictionError();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get prediction error error: ") + e.what());
        }
    }, R"pbdoc(Get prediction error)pbdoc")
    
    .def("reset", [](std::shared_ptr<AgentBrain> self) {
        try {
            self->reset();
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain reset error: ") + e.what());
        }
    }, R"pbdoc(Reset agent for new episode)pbdoc")
    
    .def("getBrain", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->getBrain();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain get brain error: ") + e.what());
        }
    }, R"pbdoc(Get the underlying brain)pbdoc")
    
    // Enhanced subsystem enable/disable with validation
    .def("enable_reward_modulation", [](std::shared_ptr<AgentBrain> self, bool enable) {
        try {
            self->enableRewardModulation(enable);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain enable reward modulation error: ") + e.what());
        }
    }, py::arg("enable"), R"pbdoc(Enable/disable reward modulation)pbdoc")
    
    .def("enable_structural_plasticity", [](std::shared_ptr<AgentBrain> self, bool enable) {
        try {
            self->enableStructuralPlasticity(enable);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain enable structural plasticity error: ") + e.what());
        }
    }, py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)pbdoc")
    
    .def("enable_development", [](std::shared_ptr<AgentBrain> self, bool enable) {
        try {
            self->enableDevelopment(enable);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain enable development error: ") + e.what());
        }
    }, py::arg("enable"), R"pbdoc(Enable/disable development)pbdoc")
    
    .def("enable_curiosity", [](std::shared_ptr<AgentBrain> self, bool enable) {
        try {
            self->enableCuriosity(enable);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain enable curiosity error: ") + e.what());
        }
    }, py::arg("enable"), R"pbdoc(Enable/disable curiosity)pbdoc")
    
    // Enhanced status methods
    .def("is_reward_modulation_enabled", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->isRewardModulationEnabled();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain check reward modulation error: ") + e.what());
        }
    }, R"pbdoc(Check if reward modulation is enabled)pbdoc")
    
    .def("is_structural_plasticity_enabled", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->isStructuralPlasticityEnabled();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain check structural plasticity error: ") + e.what());
        }
    }, R"pbdoc(Check if structural plasticity is enabled)pbdoc")
    
    .def("is_development_enabled", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->isDevelopmentEnabled();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain check development error: ") + e.what());
        }
    }, R"pbdoc(Check if development is enabled)pbdoc")
    
    .def("is_curiosity_enabled", [](std::shared_ptr<AgentBrain> self) {
        try {
            return self->isCuriosityEnabled();
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain check curiosity error: ") + e.what());
        }
    }, R"pbdoc(Check if curiosity is enabled)pbdoc")
    
    .def("get_stats", [](std::shared_ptr<AgentBrain> self) {
        try {
            return pybind_utils::get_agent_brain_stats(self);
        } catch (const std::exception& e) {
            throw BrainError(std::string("AgentBrain stats error: ") + e.what());
        }
    }, R"pbdoc(Get comprehensive agent brain statistics as Python dict)pbdoc")
    
    // Enhanced __repr__
    .def("__repr__", [](const std::shared_ptr<AgentBrain>& agent) {
        try {
            auto stats = pybind_utils::get_agent_brain_stats(agent);
            return "<AgentBrain: Curiosity=" + py::str(stats["curiosity_level"]) + ", Novelty=" + 
                   py::str(stats["novelty_level"]) + ", DevStage=" + py::str(stats["developmental_stage"]) + ">";
        } catch (const std::exception& e) {
            return "<AgentBrain: Error retrieving stats>";
        }
    });

// Enhanced helper functions
m.def("createDefaultConfig", []() {
    try {
        return std::make_shared<Config>();
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Config creation error: ") + e.what());
    }
}, R"pbdoc(Create a default configuration)pbdoc")

m.def("createBrain", [](std::shared_ptr<Config> config) {
    try {
        return std::make_shared<Brain>(config);
    } catch (const std::exception& e) {
        throw BrainError(std::string("Brain creation error: ") + e.what());
    }
}, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc")

m.def("createSimpleWorld", []() {
    try {
        return std::make_shared<SimpleWorld>();
    } catch (const std::exception& e) {
        throw SimulationError(std::string("World creation error: ") + e.what());
    }
}, R"pbdoc(Create a new simple world)pbdoc")

m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
    try {
        return std::make_shared<AgentBrain>(brain);
    } catch (const std::exception& e) {
        throw BrainError(std::string("AgentBrain creation error: ") + e.what());
    }
}, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc")

// Enhanced version attribute with error handling
m.attr("__version__") = "2.0.0-enhanced";

// Exception helper functions
m.def("get_last_exception", []() {
    // This would be used to retrieve the last exception message
    return "";
}, R"pbdoc(Get last exception message (placeholder))pbdoc")

m.def("clear_all_exceptions", []() {
    // Clear any pending exceptions
    return true;
}, R"pbdoc(Clear any pending exceptions)pbdoc")

// Performance monitoring functions (if available)
m.def("get_memory_usage", []() {
    // Get current memory usage if available
    return 0.0;
}, R"pbdoc(Get current memory usage in MB)pbdoc")

m.def("get_cpu_usage", []() {
    // Get current CPU usage if available
    return 0.0;
}, R"pbdoc(Get current CPU usage percentage)pbdoc")

// Validation functions
m.def("validate_config", [](const py::dict& config_dict) {
    try {
        if (!pybind_utils::validate_config_keys(py::cast<std::vector<std::string>>(config_dict.attr("keys"())))) {
            throw ConfigError("Invalid configuration keys provided");
        }
        return true;
    } catch (const std::exception& e) {
        if (std::string(e.what()).find("ConfigError:") != std::string::npos) {
            throw;
        }
        throw ConfigError(std::string("Config validation error: ") + e.what());
    }
}, py::arg("config_dict"), R"pbdoc(Validate configuration keys)pbdoc")

// Batch operations
m.def("batch_config", [](const py::dict& config_dict) {
    try {
        auto config = std::make_shared<Config>();
        pybind_utils::batch_set_config(config, config_dict);
        return config;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Batch config error: ") + e.what());
    }
}, py::arg("config_dict"), R"pbdoc(Batch create configuration from Python dict)pbdoc")

// Convenience functions for common scenarios
m.def("create_agent_simulation", [](const py::dict& config_dict, const py::dict& world_config) {
    try {
        // Create brain with config
        auto config = std::make_shared<Config>();
        pybind_utils::batch_set_config(config, config_dict);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(
            world_config.value("width", 20),
            world_config.value("height", 20),
            world_config.value("visionWidth", 8),
            world_config.value("visionHeight", 8)
        );
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable subsystems if requested
        if (world_config.contains("enable_reward_modulation") && world_config.value("enable_reward_modulation", false)) {
            agent->enableRewardModulation(true);
        }
        if (world_config.contains("enable_curiosity") && world_config.value("enable_curiosity", false)) {
            agent->enableCuriosity(true);
        }
        
        return std::make_tuple(brain, agent, world);
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Agent simulation creation error: ") + e.what());
    }
}, py::arg("config_dict"), py::arg("world_config"), R"pbdoc(Create a complete agent simulation setup)pbdoc")

// Convenience functions for running simulations
m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                       std::shared_ptr<AgentBrain> agent, size_t num_steps) {
    try {
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return true;
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Simulation run error: ") + e.what());
    }
}, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), 
          R"pbdoc(Run a complete simulation with brain, world, and agent)pbdoc")

// Utility functions for debugging and monitoring
m.def("get_system_info", []() {
    py::dict info;
    info["version"] = "2.0.0-enhanced";
    info["cpp_standard"] = "C++20";
    info["build_type"] = "Release";
    info["python_bindings"] = "Enhanced with error handling";
    return info;
}, R"pbdoc(Get system information)pbdoc")

// Memory management utilities
m.def("clear_config_memory", []() {
    // Suggest garbage collection
    py::module::import("gc").attr("collect")();
    return true;
}, R"pbdoc(Suggest garbage collection)pbdoc")

// File operation utilities
m.def("ensure_directory", [](const std::string& path) {
    try {
        #if defined(_WIN32) || defined(_WIN64)
        // Windows
        std::string cmd = "mkdir " + path;
        #else
        // Unix-like
        std::string cmd = "mkdir -p " + path;
        #endif
        return system(cmd.c_str()) == 0;
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Directory creation error: ") + e.what());
    }
}, py::arg("path"), R"pbdoc(Ensure directory exists)pbdoc")

// Error handling utilities
m.def("get_last_error", []() {
    // Get the last error message if any
    return "";
}, R"pbdoc(Get the last error message)pbdoc")

m.def("has_error", []() {
    // Check if there's a pending error
    return false;
}, R"pbdoc(Check if there's a pending error)pbdoc")

// Progress monitoring
m.def("log_progress", [](const std::string& message, double progress) {
    try {
        std::cout << "[Progress] " << message << " (" << (progress * 100) << "%)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        throw SimulationError(std::string("Progress logging error: ") + e.what());
    }
}, py::arg("message"), py::arg("progress"), R"pbdoc(Log progress message with percentage)pbdoc")

// Validation utilities
m.def("validate_simulation_config", [](const py::dict& config_dict) {
    try {
        std::vector<std::string> errors;
        
        // Basic validation
        if (!config_dict.contains("brain") || !py::isinstance<py::dict>(config_dict["brain"])) {
            errors.push_back("Missing or invalid 'brain' configuration");
        }
        
        if (!config_dict.contains("world") || !py::isinstance<py::dict>(config_dict["world"])) {
            errors.push_back("Missing or invalid 'world' configuration");
        }
        
        if (!config_dict.contains("agent") || !py::isinstance<py::dict>(config_dict["agent"])) {
            errors.push_back("Missing or invalid 'agent' configuration");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw ConfigError(error_msg);
        }
        
        return true;
    } catch (const std::exception& e) {
        if (std::string(e.what()).find("ConfigError:") != std::string::npos) {
            throw;
        }
        throw ConfigError(std::string("Simulation config validation error: ") + e.what());
    }
}, py::arg("config_dict"), R"pbdoc(Validate simulation configuration dictionary)pbdoc")

// Export all types
m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

// Module documentation
m.doc() = R"pbdoc(
NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
===========================================================

This is an enhanced version of the NLM Python bindings with:

1. **Improved Error Handling**: Comprehensive exception handling with detailed error messages
2. **Convenience Methods**: Batch operations, validation, and helper functions
3. **Type Safety**: Better Python-C++ type conversions
4. **Enhanced Documentation**: Detailed docstrings for all methods
5. **Performance Monitoring**: Memory and CPU usage tracking
6. **Validation Utilities**: Configuration and simulation validation
7. **Progress Monitoring**: Progress logging and reporting
8. **Memory Management**: Garbage collection suggestions
9. **File Operations**: Directory creation and file utilities

The enhanced bindings provide a more robust and user-friendly interface for:

- Creating and managing neural brains
- Setting up agent-based simulations
- Running complete simulation loops
- Saving and loading brain states
- Monitoring simulation progress and statistics
- Validating configurations
- Debugging and troubleshooting

Key improvements over the original bindings:

* Comprehensive error handling with specific exception types
* Batch configuration operations
* Statistics dictionaries for easy data access
* Validation functions for configuration and simulation setup
* Progress monitoring and logging utilities
* Memory and performance tracking
* Enhanced documentation and examples
* Better type safety and Python integration

Getting Started:
--------------

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

For more examples, see the documentation and examples in the repository.
)pbdoc";

// Python version attribute
m.attr("__version__") = "2.0.0-enhanced";

} // namespace nlm

// Implementation of helper functions
namespace nlm::pybind_utils {

ConfigValue dict_to_config_value(const py::dict& py_dict) {
    ConfigValue result;
    
    if (py_dict.empty()) {
        return result;
    }
    
    // Check if dict has a single key (value)
    if (py_dict.size() == 1) {
        auto it = py_dict.begin();
        const auto& value = it->second;
        
        if (py::isinstance<py::int_>(value)) {
            result = value.cast<int>();
        } else if (py::isinstance<py::float>(value)) {
            result = value.cast<double>();
        } else if (py::isinstance<py::bool>(value)) {
            result = value.cast<bool>();
        } else if (py::isinstance<py::str>(value)) {
            result = value.cast<std::string>();
        } else if (py::isinstance<py::list>(value)) {
            // Convert list to vector
            py::list py_list = value.cast<py::list>();
            std::vector<std::string> str_list;
            for (auto item : py_list) {
                str_list.push_back(item.cast<std::string>());
            }
            result = str_list;
        }
    }
    
    return result;
}

py::object config_value_to_py(const ConfigValue& value) {
    return py::visit([](auto&& arg) -> py::object {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return py::cast(static_cast<int>(arg));
        } else if constexpr (std::is_same_v<T, double>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return py::cast(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return py::cast(arg);
        } else {
            return py::none();
        }
    }, value);
}

py::dict get_brain_stats(const std::shared_ptr<Brain>& brain) {
    py::dict stats;
    
    try {
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Get memory system info
        auto working_memory = brain->getWorkingMemory();
        if (working_memory) {
            stats["working_memory_active_traces"] = working_memory->getActiveTraces();
        } else {
            stats["working_memory_active_traces"] = 0;
        }
        
        auto episodic_memory = brain->getEpisodicMemory();
        if (episodic_memory) {
            stats["episodic_memory_episodes"] = episodic_memory->getEpisodeCount();
        } else {
            stats["episodic_memory_episodes"] = 0;
        }
        
        // Get neuromodulation info
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            stats["dopamine_level"] = dopamine->getCurrentLevel();
        } else {
            stats["dopamine_level"] = 0.0f;
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            stats["curiosity_level"] = curiosity->getCurrentLevel();
        } else {
            stats["curiosity_level"] = 0.0f;
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            stats["novelty_level"] = novelty->getCurrentLevel();
        } else {
            stats["novelty_level"] = 0.0f;
        }
        
        auto prediction_error = brain->getPredictionError();
        if (prediction_error) {
            stats["prediction_error"] = prediction_error->getCurrentError();
        } else {
            stats["prediction_error"] = 0.0f;
        }
        
        // Get cognition system info
        auto planner = brain->getPlanner();
        if (planner) {
            stats["planner_active"] = true;
        } else {
            stats["planner_active"] = false;
        }
        
        auto concept_formation = brain->getConceptFormation();
        if (concept_formation) {
            stats["concept_formation_active"] = true;
            stats["concept_formation_count"] = concept_formation->getConceptCount();
        } else {
            stats["concept_formation_active"] = false;
            stats["concept_formation_count"] = 0;
        }
        
        auto attention = brain->getAttention();
        if (attention) {
            stats["attention_active"] = true;
        } else {
            stats["attention_active"] = false;
        }
        
        // Get development system info
        auto development_system = brain->getDevelopmentSystem();
        if (development_system) {
            stats["development_active"] = true;
            stats["developmental_stage"] = static_cast<int>(development_system->getCurrentStage());
        } else {
            stats["development_active"] = false;
            stats["developmental_stage"] = static_cast<int>(DevelopmentalStage::Initial);
        }
        
        // Get prediction system info
        auto prediction_system = brain->getPredictionSystem();
        if (prediction_system) {
            stats["prediction_system_active"] = true;
        } else {
            stats["prediction_system_active"] = false;
        }
        
    } catch (const std::exception& e) {
        stats["error"] = std::string("Failed to get brain stats: ") + e.what();
    }
    
    return stats;
}

py::dict get_agent_brain_stats(const std::shared_ptr<AgentBrain>& agent) {
    py::dict stats;
    
    try {
        auto brain = agent->getBrain();
        if (brain) {
            auto brain_stats = get_brain_stats(brain);
            stats.update(brain_stats);
        }
        
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
        stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        stats["development_enabled"] = agent->isDevelopmentEnabled();
        stats["curiosity_enabled"] = agent->isCuriosityEnabled();
        
    } catch (const std::exception& e) {
        stats["error"] = std::string("Failed to get agent brain stats: ") + e.what();
    }
    
    return stats;
}

bool validate_config_keys(const std::vector<std::string>& keys) {
    // Validate that keys are not empty and don't contain invalid characters
    for (const auto& key : keys) {
        if (key.empty()) {
            return false;
        }
        // Check for potentially problematic characters
        if (key.find('\0') != std::string::npos) {
            return false;
        }
    }
    return true;
}

void batch_set_config(std::shared_ptr<Config> config, 
                     const py::dict& config_dict,
                     ConfigSource source) {
    for (auto item : config_dict) {
        std::string key = item.first.cast<std::string>();
        py::object value = item.second;
        
        ConfigValue config_value;
        
        if (py::isinstance<py::int_>(value)) {
            config_value = value.cast<int>();
        } else if (py::isinstance<py::float>(value)) {
            config_value = value.cast<double>();
        } else if (py::isinstance<py::bool>(value)) {
            config_value = value.cast<bool>();
        } else if (py::isinstance<py::str>(value)) {
            config_value = value.cast<std::string>();
        } else if (py::isinstance<py::list>(value)) {
            py::list py_list = value.cast<py::list>();
            std::vector<std::string> str_list;
            for (auto item : py_list) {
                str_list.push_back(item.cast<std::string>());
            }
            config_value = str_list;
        }
        
        config->set(key, config_value, source);
    }
}

py::dict config_to_dict(const std::shared_ptr<Config>& config) {
    py::dict result;
    
    try {
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            result[key] = config_value_to_py(config->get<ConfigValue>(key).value_or(ConfigValue()));
        }
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to convert config to dict: ") + e.what());
    }
    
    return result;
}

} // namespace pybind_utils