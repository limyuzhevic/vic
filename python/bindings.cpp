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
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        Features:
        - Python-friendly configuration management
        - Convenience functions for common operations
        - Pythonic error handling
        - Comprehensive examples and documentation
        - Real-time monitoring capabilities
        - Advanced brain state analysis
    )pbdoc";

    // Register custom exception for Python-friendly error handling
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Custom exceptions with detailed error messages
    m.attr("BrainCreationError") = py::cast(std::runtime_error("Brain creation failed"));
    m.attr("ConfigLoadError") = py::cast(std::runtime_error("Configuration loading failed"));
    m.attr("InvalidConfigError") = py::cast(std::invalid_argument("Invalid configuration"));
    m.attr("FileOperationError") = py::cast(std::runtime_error("File operation failed"));

    // Configuration class with enhanced Python features
    py::class_<Config>(m, "Config")
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
        .def("getOr", [](const Config& self, const std::string& key, const ConfigValue& defaultValue) {
            return self.getOr<ConfigValue>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with Python-style default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (string type)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (integer type)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float type)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (boolean type)")
        .def("validate", [](Config& self) {
            return validateConfig(self);
        })
        .def("fixDuplicates", [](Config& self) {
            return fixConfigDuplicates(self);
        })
        .def("to_dict", [](const Config& self) {
            std::map<std::string, std::string> result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value.has_value()) {
                    result[key] = valueToString(value.value());
                }
            }
            return result;
        })
        .def("from_dict", [](Config& self, const std::map<std::string, std::string>& config) {
            for (const auto& item : config) {
                // Try to parse value based on key
                if (item.second == "true" || item.second == "false") {
                    bool boolVal = (item.second == "true");
                    self.set(item.first, boolVal, ConfigSource::Runtime);
                } else {
                    try {
                        size_t pos;
                        int intVal = std::stoi(item.second, &pos);
                        if (pos == item.second.length()) {
                            self.set(item.first, intVal, ConfigSource::Runtime);
                            continue;
                        }
                    } catch (...) {}
                    
                    try {
                        size_t pos;
                        double doubleVal = std::stod(item.second, &pos);
                        if (pos == item.second.length()) {
                            self.set(item.first, doubleVal, ConfigSource::Runtime);
                            continue;
                        }
                    } catch (...) {}
                    
                    self.set(item.first, item.second, ConfigSource::Runtime);
                }
            }
        })
        .def("copy", [](const Config& self) {
            auto newConfig = std::make_shared<Config>();
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<ConfigValue>(key);
                if (value.has_value()) {
                    newConfig->set(key, value.value(), ConfigSource::Runtime);
                }
            }
            return newConfig;
        });

    // Enhanced Brain class with Python-friendly methods
    py::class_<Brain>(m, "Brain")
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
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getStatistics", [](Brain& self) {
            auto stats = getBrainStatistics(self);
            return stats;
        }, "Get comprehensive brain statistics")
        .def("getActivityHeatmap", [](Brain& self) {
            return getBrainActivityHeatmap(self);
        }, "Get activity heatmap of the brain")
        .def("getPerformanceMetrics", [](Brain& self) {
            return getBrainPerformanceMetrics(self);
        }, "Get performance metrics")
        .def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
            return compareBrainStates(brain1, brain2);
        }, py::arg("brain1"), py::arg("brain2"), "Compare brain states")
        .def("clone", [](const Brain& brain) {
            return cloneBrain(brain);
        }, "Clone brain state");

    // Enhanced AgentBrain class with Python-friendly methods
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
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
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven behavior")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getAgentStats", [](AgentBrain& agent) {
            return getAgentStatistics(agent);
        }, "Get comprehensive agent statistics")
        .def("trainEpisode", [](AgentBrain& agent, Brain& brain, SimpleWorld& world, int steps) {
            return trainAgentEpisode(agent, brain, world, steps);
        }, py::arg("agent"), py::arg("brain"), py::arg("world"), py::arg("steps"), "Train agent for one episode");

    // Enhanced utility functions
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

    // Convenience functions for common operations
    m.def("quickStart", []() {
        return quickStartExample();
    }, "Quick start example - create and initialize a basic brain");

    m.def("demoMode", [](const std::string& mode) {
        return demoModeRun(mode);
    }, py::arg("mode"), "Run demo mode with predefined scenarios");

    m.def("benchmark", [](const std::string& brainType, int steps) {
        return benchmarkBrain(brainType, steps);
    }, py::arg("brain_type"), py::arg("steps"), "Benchmark brain performance");

    m.def("validateConfig", &validateConfig, py::arg("config"),
          "Validate configuration and return results");

    m.def("fixConfigDuplicates", &fixConfigDuplicates, py::arg("config"),
          "Fix duplicate configuration entries");

    // Configuration utilities
    m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& override) {
        return mergeConfigs(base, override);
    }, py::arg("base"), py::arg("override"), "Merge two configuration objects");

    m.def("exportConfig", [](const std::shared_ptr<Config>& config, const std::string& format) {
        return exportConfigToFormat(config, format);
    }, py::arg("config"), py::arg("format"), "Export configuration to different format");

    m.def("importConfig", [](const std::string& data, const std::string& format) {
        return importConfigFromFormat(data, format);
    }, py::arg("data"), py::arg("format"), "Import configuration from different format");

    // Analysis and monitoring utilities
    m.def("analyzeBrainActivity", &analyzeBrainActivity, py::arg("brain"),
          "Analyze brain activity patterns");

    m.def("compareBrains", &compareBrainStates, py::arg("brain1"), py::arg("brain2"),
          "Compare two brain states");

    m.def("getBrainStatistics", &getBrainStatistics, py::arg("brain"),
          "Get comprehensive brain statistics");

    m.def("getAgentStatistics", &getAgentStatistics, py::arg("agent"),
          "Get comprehensive agent statistics");

    m.def("getBrainActivityHeatmap", &getBrainActivityHeatmap, py::arg("brain"),
          "Get activity heatmap of the brain");

    m.def("getBrainPerformanceMetrics", &getBrainPerformanceMetrics, py::arg("brain"),
          "Get performance metrics");

    m.def("cloneBrain", &cloneBrain, py::arg("brain"), "Clone brain state");

    // Example utilities
    m.def("createTutorialBrain", []() {
        return createTutorialBrain();
    }, "Create a brain configured for tutorial/demo purposes");

    m.def("createBenchmarkBrain", [](int neuronCount) {
        return createBenchmarkBrain(neuronCount);
    }, py::arg("neuron_count"), "Create a brain configured for benchmarking");

    m.def("createResearchBrain", []() {
        return createResearchBrain();
    }, "Create a brain configured for research purposes");

    // Error handling utilities
    m.def("getLastError", []() {
        return getLastErrorMessage();
    }, "Get the last error message");

    m.def("clearErrors", []() {
        clearErrorHistory();
    }, "Clear error history");

    m.def("getErrorCount", []() {
        return getErrorCount();
    }, "Get error count");

    // Performance utilities
    m.def("optimizeBrain", [](std::shared_ptr<Brain>& brain) {
        optimizeBrainForPerformance(brain);
    }, py::arg("brain"), "Optimize brain for performance");

    m.def("profileBrain", [](std::shared_ptr<Brain>& brain, int duration) {
        return profileBrainPerformance(brain, duration);
    }, py::arg("brain"), py::arg("duration"), "Profile brain performance");

    // Monitoring utilities
    m.def("startMonitoring", [](std::shared_ptr<Brain>& brain, int interval) {
        return startBrainMonitoring(brain, interval);
    }, py::arg("brain"), py::arg("interval"), "Start monitoring brain");

    m.def("stopMonitoring", []() {
        stopAllMonitoring();
    }, "Stop all monitoring");

    m.def("getMonitoringData", []() {
        return getCurrentMonitoringData();
    }, "Get current monitoring data");

    // Batch processing utilities
    m.def("batchProcessBrains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        return batchProcessBrainOperations(brains);
    }, py::arg("brains"), "Process multiple brains in batch");

    m.def("compareBrainSets", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        return compareBrainSets(brains);
    }, py::arg("brains"), "Compare multiple brain sets");

    // Advanced features
    m.def("enableAdvancedLearning", [](std::shared_ptr<Brain>& brain) {
        enableAdvancedLearningFeatures(brain);
    }, py::arg("brain"), "Enable advanced learning features");

    m.def("enableNeuromodulation", [](std::shared_ptr<Brain>& brain, bool enable) {
        enableBrainNeuromodulation(brain, enable);
    }, py::arg("brain"), py::arg("enable"), "Enable or disable neuromodulation");

    m.def("enableSelfOrganization", [](std::shared_ptr<Brain>& brain) {
        enableSelfOrganizationFeatures(brain);
    }, py::arg("brain"), "Enable self-organization features");

    // Output definitions for constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "0.2.0"; // Enhanced version
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
}

} // namespace nlm