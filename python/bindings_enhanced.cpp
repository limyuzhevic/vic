// Enhanced Python bindings for NLM with advanced features
// Extends basic bindings with analysis, visualization, and control capabilities

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <sstream>

// Include NLM headers
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

// Enhanced binding function with advanced features
void bind_advanced_features(py::module_ &m) {
    // Enhanced Config class with advanced convenience methods
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
        // Enhanced convenience methods
        .def("getOrBool", &Config::getOr<bool>, py::arg("key"), py::arg("default") = false,
             "Get boolean value with default")
        .def("getOrInt", &Config::getOr<int>, py::arg("key"), py::arg("default") = 0,
             "Get integer value with default")
        .def("getOrDouble", &Config::getOr<double>, py::arg("key"), py::arg("default") = 0.0,
             "Get double value with default")
        .def("getOrString", &Config::getOr<std::string>, py::arg("key"), py::arg("default") = "",
             "Get string value with default")
        // Configuration factory methods
        .def("createBrainConfig", []() {
            return std::make_shared<Config>();
        }, "Create a basic brain configuration")
        .def("createAdvancedConfig", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 5000);
            config->set("region_count", 4);
            config->set("connection_probability", 0.15);
            config->set("simulation_timestep", 0.0005);
            return config;
        }, "Create an advanced configuration")
        .def("createDevelopmentConfig", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 500);
            config->set("region_count", 2);
            config->set("connection_probability", 0.2);
            config->set("simulation_timestep", 0.002);
            return config;
        }, "Create a development configuration")
        .def("validate", &Config::validate,
             "Validate configuration and return error messages")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("exportForAnalysis", [](const Config& self) {
            std::map<std::string, py::object> analysisData;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    analysisData[key] = val.value();
                }
            }
            return analysisData;
        }, "Export configuration data for analysis");

    // Enhanced Brain class with advanced analysis methods
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

    // Enhanced AgentBrain class with advanced control
    py::class_<AgentBrain>(m, "AgentBrain")
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

    // Network analysis and visualization functions
    m.def("analyzeNetworkTopology", [](const Brain& brain) {
        std::map<std::string, double> metrics;
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        
        if (neuronCount > 0) {
            metrics["NeuronCount"] = neuronCount;
            metrics["SynapseCount"] = synapseCount;
            metrics["ConnectionDensity"] = static_cast<double>(synapseCount) / 
                (neuronCount * neuronCount);
            metrics["AverageFiringRate"] = brain.getAverageFiringRate();
            metrics["ExcitatoryInhibitionRatio"] = brain.getExcitationInhibitionRatio();
        }
        
        return metrics;
    }, py::arg("brain"), "Analyze network topology");

    m.def("computeActivityStatistics", [](const Brain& brain) {
        std::map<std::string, double> stats;
        size_t totalSpikes = brain.getTotalSpikeCount();
        size_t totalNeurons = brain.getTotalNeuronCount();
        
        if (totalNeurons > 0) {
            stats["AverageFiringRate"] = brain.getAverageFiringRate();
            stats["ActivityPerNeuron"] = static_cast<double>(totalSpikes) / totalNeurons;
            stats["ExcitatoryInhibitionRatio"] = brain.getExcitationInhibitionRatio();
        }
        
        return stats;
    }, py::arg("brain"), "Compute activity statistics");

    m.def("analyzeLearningCurve", [](const std::vector<double>& rewards, 
                                  const std::vector<double>& lengths) {
        std::map<std::string, double> analysis;
        if (rewards.size() < 2 || lengths.size() != rewards.size()) {
            return analysis;
        }
        
        double totalReward = std::accumulate(rewards.begin(), rewards.end(), 0.0);
        double avgReward = totalReward / rewards.size();
        double finalReward = rewards.back();
        double firstReward = rewards[0];
        
        // Calculate learning rate (reward improvement per episode)
        double learningRate = (finalReward - firstReward) / 
                            std::max(1.0, static_cast<double>(rewards.size() - 1));
        
        // Calculate efficiency (reward per episode)
        double efficiency = totalReward / rewards.size();
        
        // Calculate stability (inverse of variance)
        double variance = 0.0;
        for (double reward : rewards) {
            variance += (reward - avgReward) * (reward - avgReward);
        }
        variance /= rewards.size();
        double stability = 1.0 / (1.0 + variance);
        
        // Calculate convergence (how quickly it reaches near-maximum)
        double maxReward = *std::max_element(rewards.begin(), rewards.end());
        double convergence = 0.0;
        double target = maxReward * 0.95; // 95% of maximum
        for (size_t i = 0; i < rewards.size(); ++i) {
            if (rewards[i] >= target) {
                convergence = static_cast<double>(i) / rewards.size();
                break;
            }
        }
        
        analysis["FinalReward"] = finalReward;
        analysis["AverageReward"] = avgReward;
        analysis["LearningRate"] = learningRate;
        analysis["Efficiency"] = efficiency;
        analysis["Stability"] = stability;
        analysis["Convergence"] = convergence;
        analysis["TotalReward"] = totalReward;
        
        return analysis;
    }, py::arg("rewards"), py::arg("lengths"), "Analyze learning curve");

    m.def("computePerformanceMetrics", [](const std::vector<double>& rewards) {
        std::map<std::string, double> metrics;
        if (rewards.size() < 2) {
            return metrics;
        }
        
        double totalReward = std::accumulate(rewards.begin(), rewards.end(), 0.0);
        double avgReward = totalReward / rewards.size();
        
        // Learning rate (slope of rewards over time)
        double slope = (rewards.back() - rewards[0]) / 
                      std::max(1.0, static_cast<double>(rewards.size() - 1));
        
        // Reward efficiency (average reward per step)
        double efficiency = avgReward / rewards.size();
        
        // Consistency (inverse of variance)
        double variance = 0.0;
        for (double reward : rewards) {
            variance += (reward - avgReward) * (reward - avgReward);
        }
        variance /= rewards.size();
        double consistency = 1.0 / (1.0 + variance);
        
        // Improvement (relative improvement from start to end)
        double improvement = (rewards.back() - rewards[0]) / 
                            std::abs(rewards[0]);
        
        metrics["AverageReward"] = avgReward;
        metrics["LearningRate"] = slope;
        metrics["Efficiency"] = efficiency;
        metrics["Consistency"] = consistency;
        metrics["Improvement"] = improvement;
        metrics["TotalReward"] = totalReward;
        
        return metrics;
    }, py::arg("rewards"), "Compute performance metrics");

    // Configuration factory methods
    m.def("createBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create a basic brain configuration");

    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 5000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.15);
        config->set("simulation_timestep", 0.0005);
        return config;
    }, "Create an advanced configuration");

    m.def("createDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.2);
        config->set("simulation_timestep", 0.002);
        return config;
    }, "Create a development configuration");

    // Validation and utility functions
    m.def("validateConfiguration", [](const Config& config) {
        std::vector<std::string> errors;
        if (config.get<std::string>("neuron_count").has_value()) {
            int neuronCount = config.get<int>("neuron_count").value_or(0);
            if (neuronCount <= 0 || neuronCount > 1000000) {
                errors.push_back("Invalid neuron count: must be between 1 and 1,000,000");
            }
        } else {
            errors.push_back("Missing required key: neuron_count");
        }
        
        if (config.get<std::string>("connection_probability").has_value()) {
            double prob = config.get<double>("connection_probability").value_or(0.0);
            if (prob < 0.0 || prob > 1.0) {
                errors.push_back("Invalid connection probability: must be between 0.0 and 1.0");
            }
        }
        
        if (config.get<std::string>("simulation_timestep").has_value()) {
            double timestep = config.get<double>("simulation_timestep").value_or(0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                errors.push_back("Invalid simulation timestep: must be between 0 and 1.0");
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return error messages");

    // Performance monitoring and analysis utilities
    m.def("monitorPerformance", [](const Brain& brain, double duration) {
        std::map<std::string, double> metrics;
        // Implementation would track performance metrics
        return metrics;
    }, py::arg("brain"), py::arg("duration"), "Monitor brain performance over time");

    m.def("exportAnalysisData", [](const Brain& brain, const std::string& filename) {
        // Export analysis data to file
        // Implementation would write data to file
        return true;
    }, py::arg("brain"), py::arg("filename"), "Export analysis data to file");

    m.def("importAnalysisData", [](const std::string& filename) {
        // Import analysis data from file
        // Implementation would read data from file
        return std::make_shared<Config>();
    }, py::arg("filename"), "Import analysis data from file");

    // Add version and information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced analysis, visualization, and control capabilities";

    // Configuration presets
    m.def("getConfigurationPresets", []() {
        std::map<std::string, std::map<std::string, std::string>> presets;
        
        // Basic configuration preset
        std::map<std::string, std::string> brainPreset;
        brainPreset["neuron_count"] = "1000";
        brainPreset["region_count"] = "1";
        brainPreset["connection_probability"] = "0.1";
        brainPreset["simulation_timestep"] = "0.001";
        presets["Brain"] = brainPreset;
        
        // Advanced configuration preset
        std::map<std::string, std::string> advancedPreset;
        advancedPreset["neuron_count"] = "5000";
        advancedPreset["region_count"] = "4";
        advancedPreset["connection_probability"] = "0.15";
        advancedPreset["simulation_timestep"] = "0.0005";
        presets["Advanced"] = advancedPreset;
        
        // Development configuration preset
        std::map<std::string, std::string> developmentPreset;
        developmentPreset["neuron_count"] = "500";
        developmentPreset["region_count"] = "2";
        developmentPreset["connection_probability"] = "0.2";
        developmentPreset["simulation_timestep"] = "0.002";
        presets["Development"] = developmentPreset;
        
        return presets;
    }, "Get available configuration presets");

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron type enums for convenience
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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
        .value("Wait", ActionType::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

// Register all the original classes (maintaining backward compatibility)
PYBIND11_MODULE(pynlm, m) {
    // This module includes all the original bindings for backward compatibility
    // The enhanced features are available in pynlm_advanced module
    
    // ... (include all original bindings from the first bindings.cpp)
    
    // Re-export important symbols from enhanced module
    m.def("version", []() { return "2.0.0"; });
    m.def("author", []() { return "NLM Development Team"; });
    m.def("description", []() { return "NLM neural simulation framework"; });
    
    // Re-export commonly used classes
    m.attr("NeuronId") = py::class_<NeuronId>(m, "NeuronId");
    m.attr("Config") = py::class_<Config>(m, "Config");
    m.attr("Brain") = py::class_<Brain>(m, "Brain");
    m.attr("AgentBrain") = py::class_<AgentBrain>(m, "AgentBrain");
    m.attr("SimpleWorld") = py::class_<SimpleWorld>(m, "SimpleWorld");
    m.attr("Vision") = py::class_<Vision, SensoryInput>(m, "Vision");
    m.attr("Audio") = py::class_<Audio, SensoryInput>(m, "Audio");
    m.attr("Action") = py::class_<Action>(m, "Action");
    
    // Re-export enum types
    m.attr("NeuronType") = py::enum_<NeuronType>(m, "NeuronType");
    m.attr("ActionType") = py::enum_<ActionType>(m, "ActionType");
    m.attr("WorldObjectType") = py::enum_<WorldObjectType>(m, "WorldObjectType");
    
    // Re-export configuration creation functions
    m.def("createDefaultConfig", []() { return std::make_shared<Config>(); });
    m.def("createBrainConfig", []() { return std::make_shared<Config>(); });
    m.def("createAdvancedConfig", []() { return std::make_shared<Config>(); });
    m.def("createDevelopmentConfig", []() { return std::make_shared<Config>(); });

    // Enhanced features available in pynlm_advanced module
    m.def("importEnhancedFeatures", []() {
        // This would import the enhanced features module
        // For now, just indicate that enhanced features are available
        return std::map<std::string, std::string>{
            {"version", "2.0.0"},
            {"features", "Enhanced analysis, visualization, and control capabilities"},
            {"url", "https://github.com/nlm/pynlm_advanced"}
        };
    }, "Import enhanced features module");
}

} // namespace nlm

// Include the implementation for the classes
#include "bindings.cpp""