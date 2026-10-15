#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
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

// Custom Python exceptions for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

// Helper function to create detailed error messages
template<typename... Args>
std::string formatError(const char* format, Args&&... args) {
    std::stringstream ss;
    ss << "NLM Error: ";
    // Simple formatting - in production might use fmt::format
    // This is a simplified approach
    size_t pos = 0;
    const char* ptr = format;
    while (*ptr) {
        if (*ptr == '%' && *(ptr + 1)) {
            ptr++;
            switch (*ptr) {
                case 's': {
                    const char* arg = std::forward<Args>(args)...;
                    ss << arg;
                    break;
                }
                case 'd': {
                    int arg = std::forward<Args>(args)...;
                    ss << arg;
                    break;
                }
                case 'f': {
                    double arg = std::forward<Args>(args)...;
                    ss << arg;
                    break;
                }
            }
        } else {
            ss << *ptr;
        }
        ptr++;
    }
    return ss.str();
}

// Context manager for Brain
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void __enter__() {
        if (!brain_->initialize()) {
            throw BrainError("Failed to initialize brain in context manager");
        }
    }
    
    void __exit__(int, const char*, int, const char*) {
        brain_->reset();
    }
    
    std::shared_ptr<Brain> brain() { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for AgentBrain
class AgentBrainContextManager {
public:
    AgentBrainContextManager(std::shared_ptr<AgentBrain> agent) : agent_(agent) {}
    
    void __enter__() {
        // AgentBrain doesn't have automatic initialization, but we can check it's valid
        if (!agent_ || !agent_->getBrain()) {
            throw BrainError("Invalid AgentBrain in context manager");
        }
    }
    
    void __exit__(int, const char*, int, const char*) {
        agent_->reset();
    }
    
    std::shared_ptr<AgentBrain> agent() { return agent_; }
    
private:
    std::shared_ptr<AgentBrain> agent_;
};

// Enhanced Config class with builder pattern
class ConfigBuilder {
public:
    ConfigBuilder() {
        config_ = std::make_shared<Config>();
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& setBrainConfig(int neuron_count, int region_count = 1) {
        config_->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
        config_->set("brain.region_count", static_cast<int64_t>(region_count), ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& setLearningConfig(double learning_rate, double stdp_rate = 0.001) {
        config_->set("plasticity.stdp.learning_rate", learning_rate, ConfigSource::Runtime);
        config_->set("plasticity.stdp.tau_plus", stdp_rate, ConfigSource::Runtime);
        config_->set("plasticity.stdp.tau_minus", stdp_rate, ConfigSource::Runtime);
        return *this;
    }
    
    ConfigBuilder& setNeuromodulationConfig(bool curiosity, bool novelty) {
        config_->set("neuromod.curiosity.enable", curiosity, ConfigSource::Runtime);
        config_->set("neuromod.novelty.enable", novelty, ConfigSource::Runtime);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Performance profiler
class BrainProfiler {
public:
    struct PerformanceStats {
        double total_time_ms = 0.0;
        int steps_completed = 0;
        int spikes_generated = 0;
        double average_firing_rate = 0.0;
        double memory_usage_mb = 0.0;
    };
    
    BrainProfiler(std::shared_ptr<Brain> brain) : brain_(brain), start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void record_step() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
        stats_.total_time_ms += duration;
        stats_.steps_completed++;
        start_time_ = now;
        stats_.spikes_generated = brain_->getTotalSpikeCount();
        stats_.average_firing_rate = brain_->getAverageFiringRate();
    }
    
    PerformanceStats collect() const {
        return stats_;
    }
    
    std::string report() const {
        std::stringstream ss;
        ss << "Brain Performance Report:\n";
        ss << "  Steps completed: " << stats_.steps_completed << "\n";
        ss << "  Total time: " << stats_.total_time_ms << " ms\n";
        if (stats_.steps_completed > 0) {
            ss << "  Average time per step: " << (stats_.total_time_ms / stats_.steps_completed) << " ms\n";
        }
        ss << "  Total spikes: " << stats_.spikes_generated << "\n";
        ss << "  Average firing rate: " << stats_.average_firing_rate << " Hz\n";
        ss << "  Memory usage: " << stats_.memory_usage_mb << " MB\n";
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    PerformanceStats stats_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        ---------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        New features in this edition:
        - Context managers for automatic resource management
        - Batch operations for improved performance
        - Configuration builder pattern for easy setup
        - Performance profiling and monitoring
        - Enhanced error handling with custom exceptions
        - Convenience methods and Pythonic API
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);

    // ===== Enhanced Configuration ====
    
    // Enhanced Config class
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
             py::arg("filepath"), 
             "Load configuration from a JSON or YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>, py::const_),
             py::arg("key"), 
             "Get a configuration value (returns None if not found)")
        .def("getInt", &Config::template get<int>, py::arg("key"),
             "Get an integer configuration value")
        .def("getDouble", &Config::template get<double>, py::arg("key"),
             "Get a double configuration value")
        .def("getBool", &Config::template get<bool>, py::arg("key"),
             "Get a boolean configuration value")
        .def("getString", &Config::template get<std::string>, py::arg("key"),
             "Get a string configuration value")
        .def("getIntOr", py::overload_cast<const std::string&, int>(&Config::getOr<int>, py::const_),
             py::arg("key"), py::arg("defaultValue"),
             "Get an integer configuration value or default")
        .def("getDoubleOr", py::overload_cast<const std::string&, double>(&Config::getOr<double>, py::const_),
             py::arg("key"), py::arg("defaultValue"),
             "Get a double configuration value or default")
        .def("getBoolOr", py::overload_cast<const std::string&, bool>(&Config::getOr<bool>, py::const_),
             py::arg("key"), py::arg("defaultValue"),
             "Get a boolean configuration value or default")
        .def("getStringOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>, py::const_),
             py::arg("key"), py::arg("defaultValue"),
             "Get a string configuration value or default")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a string configuration value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set an integer configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a double configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a boolean configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", [](const Config& self) {
            std::map<std::string, std::string> dict;
            for (const auto& key : self.getKeys()) {
                auto value = self.get<std::string>(key);
                if (value) {
                    dict[key] = *value;
                }
            }
            return dict;
        }, "Convert configuration to a Python dictionary")
        .def("validate", [](const Config& self, const std::map<std::string, std::string>& schema) {
            // Basic validation - in production would use a proper schema validation library
            std::vector<std::string> errors;
            for (const auto& [key, expected_type] : schema) {
                if (!self.has(key)) {
                    errors.push_back("Missing required key: " + key);
                }
            }
            if (!errors.empty()) {
                throw ConfigError("Configuration validation failed:\n" + 
                                 std::accumulate(errors.begin(), errors.end(), std::string(),
                                 [](const std::string& a, const std::string& b) { return a + "\n" + b; }));
            }
            return true;
        }, py::arg("schema"), "Validate configuration against a schema")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===== Configuration Builder Pattern =====
    
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Configuration builder for easy setup of NLM configurations)pbdoc")
        .def(py::init<>())
        .def("set", py::overload_cast<const std::string&, const std::string&>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"))
        .def("set", py::overload_cast<const std::string&, int>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"))
        .def("set", py::overload_cast<const std::string&, double>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"))
        .def("set", py::overload_cast<const std::string&, bool>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"))
        .def("setBrainConfig", &ConfigBuilder::setBrainConfig,
             py::arg("neuron_count"), py::arg("region_count") = 1,
             "Set brain configuration with neuron count and region count")
        .def("setLearningConfig", &ConfigBuilder::setLearningConfig,
             py::arg("learning_rate"), py::arg("stdp_rate") = 0.001,
             "Set learning configuration with STDP parameters")
        .def("setNeuromodulationConfig", &ConfigBuilder::setNeuromodulationConfig,
             py::arg("curiosity"), py::arg("novelty"),
             "Set neuromodulation configuration")
        .def("build", &ConfigBuilder::build,
             "Build the configuration and return a Config object");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfigBuilder", []() {
        return std::make_shared<ConfigBuilder>();
    }, "Create a configuration builder for easy setup");

    // ===== Enhanced Brain Class =====
    
    // Context manager for Brain
    py::class_<BrainContextManager>(m, "BrainContext", R"pbdoc(Context manager for Brain with automatic initialization and cleanup)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainContextManager::__enter__)
        .def("__exit__", &BrainContextManager::__exit__)
        .def("brain", &BrainContextManager::brain, py::return_value_policy::reference_internal);

    // Enhanced Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("run_steps", [](std::shared_ptr<Brain> self, size_t numSteps, Timestamp time = 0.0) {
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self->step(step, time + step * 0.1); // Assume 0.1ms timestep
            }
        }, py::arg("numSteps"), py::arg("time") = 0.0,
           "Run multiple simulation steps efficiently")
        .def("run_simulation", [](std::shared_ptr<Brain> self, 
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent,
                                 size_t numSteps,
                                 double timestep = 0.1) {
            if (!world || !agent) {
                throw BrainError("World and AgentBrain must not be null");
            }
            for (SimulationStep step = 0; step < numSteps; ++step) {
                world->update(timestep);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                self->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }, py::arg("world"), py::arg("agent"), py::arg("numSteps"), py::arg("timestep") = 0.1,
           "Run a complete simulation with brain, world, and agent")
        .def("reset_and_initialize", [](std::shared_ptr<Brain> self) {
            self->reset();
            if (!self->initialize()) {
                throw BrainError("Failed to initialize brain after reset");
            }
        }, "Reset brain and initialize in one call")
        .def("warm_up", [](std::shared_ptr<Brain> self, size_t numSteps) {
            self->reset();
            self->initialize();
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self->step(step);
            }
        }, py::arg("numSteps"), "Warm up the brain with initial steps")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Brain statistics and convenience methods
        .def("get_brain_stats", [](const std::shared_ptr<Brain>& self) {
            py::dict stats;
            stats["total_neurons"] = self->getTotalNeuronCount();
            stats["total_synapses"] = self->getTotalSynapseCount();
            stats["firing_neurons"] = self->getFiringNeuronCount();
            stats["active_neurons"] = self->getActiveNeuronCount();
            stats["average_firing_rate"] = self->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self->getExcitationInhibitionRatio();
            stats["total_spikes"] = self->getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(self->getDevelopmentalStage());
            stats["region_count"] = self->getRegionCount();
            return stats;
        }, "Get comprehensive brain statistics as a dictionary")
        .def("create_profiler", [](std::shared_ptr<Brain> self) {
            return std::make_shared<BrainProfiler>(self);
        }, "Create a performance profiler for this brain")
        .def("configure_neurons", [](std::shared_ptr<Brain> self, size_t neuron_count, size_t region_count) {
            auto config = self->getConfig();
            config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
            config->set("brain.region_count", static_cast<int64_t>(region_count), ConfigSource::Runtime);
        }, py::arg("neuron_count"), py::arg("region_count"),
           "Configure brain with specific neuron and region counts")
        .def("configure_learning", [](std::shared_ptr<Brain> self, double learning_rate, double stdp_rate) {
            auto config = self->getConfig();
            config->set("plasticity.stdp.learning_rate", learning_rate, ConfigSource::Runtime);
            config->set("plasticity.stdp.tau_plus", stdp_rate, ConfigSource::Runtime);
            config->set("plasticity.stdp.tau_minus", stdp_rate, ConfigSource::Runtime);
        }, py::arg("learning_rate"), py::arg("stdp_rate"),
           "Configure learning parameters")
        .def("configure_neuromodulation", [](std::shared_ptr<Brain> self, bool curiosity, bool novelty) {
            auto config = self->getConfig();
            config->set("neuromod.curiosity.enable", curiosity, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", novelty, ConfigSource::Runtime);
        }, py::arg("curiosity"), py::arg("novelty"),
           "Configure neuromodulation parameters")
        // Context manager
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return std::make_shared<BrainContextManager>(self);
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, int, const char*, int, const char*) {
            self->reset();
        })
        .def("__repr__", [](const std::shared_ptr<Brain>& self) {
            std::stringstream ss;
            ss << "<Brain: " << self->getTotalNeuronCount() << " neurons, "
                << self->getTotalSynapseCount() << " synapses, "
                << static_cast<int>(self->getDevelopmentalStage()) << " stage>";
            return ss.str();
        });

    // ===== Enhanced World Class =====
    
    // Context manager for AgentBrain
    py::class_<AgentBrainContextManager>(m, "AgentBrainContext", R"pbdoc(Context manager for AgentBrain with automatic initialization and cleanup)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("__enter__", &AgentBrainContextManager::__enter__)
        .def("__exit__", &AgentBrainContextManager::__exit__)
        .def("agent", &AgentBrainContextManager::agent, py::return_value_policy::reference_internal);

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        // AgentBrain statistics and convenience methods
        .def("get_agent_stats", [](const std::shared_ptr<AgentBrain>& self) {
            py::dict stats;
            if (self && self->getBrain()) {
                stats["brain"] = self->getBrain();
                stats["reward_modulation_enabled"] = self->isRewardModulationEnabled();
                stats["structural_plasticity_enabled"] = self->isStructuralPlasticityEnabled();
                stats["development_enabled"] = self->isDevelopmentEnabled();
                stats["curiosity_enabled"] = self->isCuriosityEnabled();
                stats["developmental_stage"] = static_cast<int>(self->getDevelopmentalStage());
                stats["neuromodulation_level"] = self->getNeuromodulationLevel();
                stats["curiosity_level"] = self->getCuriosityLevel();
                stats["novelty_level"] = self->getNoveltyLevel();
                stats["prediction_error"] = self->getPredictionError();
            }
            return stats;
        }, "Get comprehensive agent statistics as a dictionary")
        // Context manager
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return std::make_shared<AgentBrainContextManager>(self);
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain> self, int, const char*, int, const char*) {
            self->reset();
        })
        .def("__repr__", [](const std::shared_ptr<AgentBrain>& self) {
            std::stringstream ss;
            if (self && self->getBrain()) {
                ss << "<AgentBrain connected to Brain with " << self->getBrain()->getTotalNeuronCount() << " neurons>";
            } else {
                ss << "<AgentBrain disconnected>";
            }
            return ss.str();
        });

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ===== Enhanced World Class =====
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced functionality)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy,
             py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed,
             py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // World convenience methods
        .def("get_world_stats", [](const std::shared_ptr<SimpleWorld>& self) {
            py::dict stats;
            stats["width"] = self->getWidth();
            stats["height"] = self->getHeight();
            stats["simulation_time"] = self->getSimulationTime();
            stats["max_energy"] = self->getMaxEnergy();
            stats["energy_decay_rate"] = self->getEnergyDecayRate();
            auto body = self->getAgentBody();
            if (body) {
                stats["agent_x"] = body->x;
                stats["agent_y"] = body->y;
                stats["agent_orientation"] = body->orientation;
                stats["agent_energy"] = body->energy;
                stats["agent_health"] = body->health;
            }
            return stats;
        }, "Get world statistics as a dictionary")
        .def("create_episode", [](std::shared_ptr<SimpleWorld> self, double timestep = 0.1) {
            return [self, timestep](size_t numSteps) {
                for (size_t step = 0; step < numSteps; ++step) {
                    self->update(timestep);
                }
            };
        }, py::arg("timestep") = 0.1,
           "Create an episode function for easy simulation loops")
        .def("reset_and_configure", [](std::shared_ptr<SimpleWorld> self, 
                                     size_t width, size_t height,
                                     size_t vision_width = 8,
                                     size_t vision_height = 8) {
            self->reset();
            self->configure(width, height, vision_width, vision_height);
        }, py::arg("width"), py::arg("height"), py::arg("vision_width") = 8, py::arg("vision_height") = 8,
           "Reset and configure world in one call")
        .def("__repr__", [](const std::shared_ptr<SimpleWorld>& self) {
            std::stringstream ss;
            ss << "<SimpleWorld: " << self->getWidth() << "x" << self->getHeight()
                << ", agent at (" << self->getAgentBody()->x << "," << self->getAgentBody()->y << ")>";
            return ss.str();
        });

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ===== Performance Profiler =====
    
    py::class_<BrainProfiler>(m, "BrainProfiler", R"pbdoc(Performance profiler for brain simulations)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("record_step", &BrainProfiler::record_step,
             "Record a simulation step")
        .def("collect", &BrainProfiler::collect,
             "Collect performance statistics")
        .def("report", &BrainProfiler::report,
             "Generate a formatted performance report");

    // ===== Convenience Functions =====
    
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t numSteps,
                                   double timestep = 0.1) {
        if (!brain || !world || !agent) {
            throw BrainError("Brain, world, and agent must not be null");
        }
        brain->initialize();
        world->reset();
        agent->initialize(world);
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       py::arg("timestep") = 0.1,
       "Run a complete agent simulation with brain, world, and agent");

    // ===== Version and Utilities =====
    
    m.attr("__version__") = "2.0.0-enhanced";
    m.def("get_version", []() { return std::string("NLM Python Bindings v2.0.0-enhanced"); },
          "Get version information");
    
    // Error types
    m.attr("NLMError") = py::object(py::cast<NLMError>(NLMError("")));
    m.attr("ConfigError") = py::object(py::cast<ConfigError>(ConfigError("")));
    m.attr("BrainError") = py::object(py::cast<BrainError>(BrainError("")));
    m.attr("WorldError") = py::object(py::cast<WorldError>(WorldError("")));
}

} // namespace nlm