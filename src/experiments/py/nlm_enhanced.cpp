// Enhanced Python bindings for NLM
// Extends the basic pynlm module with advanced functionality
// Provides enhanced API for advanced users and researchers

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <chrono>
#include <sstream>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/NLMCLI.hpp"

namespace py = pybind11;

// Enhanced configuration management from Python
class EnhancedPythonConfig {
public:
    EnhancedPythonConfig() {
        config_ = std::make_shared<Config>();
    }

    // Set value with automatic type detection
    template<typename T>
    void set(const std::string& key, T value) {
        config_->set(key, value);
    }

    // Set value with source specification
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source) {
        config_->set(key, value, source);
    }

    // Get value with type conversion
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const {
        auto value = config_->get<T>(key);
        return value ? *value : defaultValue;
    }

    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }

    // Load from JSON string
    bool loadFromJSON(const std::string& json) {
        // Simple JSON parsing - in production would use a proper JSON library
        try {
            // Parse simple key=value pairs
            std::stringstream ss(json);
            std::string line;
            while (std::getline(ss, line)) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
                    
                    // Try different types
                    if (value.find('.') != std::string::npos) {
                        try {
                            double doubleValue = std::stod(value);
                            config_->set(key, doubleValue, ConfigSource::File);
                        } catch (...) {
                            config_->set(key, value, ConfigSource::File);
                        }
                    } else {
                        try {
                            int intValue = std::stoi(value);
                            config_->set(key, intValue, ConfigSource::File);
                        } catch (...) {
                            try {
                                int64_t int64Value = std::stoll(value);
                                config_->set(key, int64Value, ConfigSource::File);
                            } catch (...) {
                                config_->set(key, value, ConfigSource::File);
                            }
                        }
                    }
                }
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    // Save to JSON string
    std::string saveToJSON() const {
        std::stringstream ss;
        ss << "{\n";
        auto keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            ss << "  \"" << keys[i] << "\": ";
            auto value = config_->get<std::string>(keys[i]);
            if (value) {
                ss << "\"" << *value << "\"";
            } else {
                auto intValue = config_->get<int64_t>(keys[i]);
                if (intValue) {
                    ss << *intValue;
                } else {
                    auto doubleValue = config_->get<double>(keys[i]);
                    if (doubleValue) {
                        ss << std::fixed << std::setprecision(6) << *doubleValue;
                    } else {
                        auto boolValue = config_->get<bool>(keys[i]);
                        if (boolValue) {
                            ss << (*boolValue ? "true" : "false");
                        }
                    }
                }
            }
            if (i < keys.size() - 1) ss << ",\n";
            else ss << "\n";
        }
        ss << "}\n";
        return ss.str();
    }

    // Load from Python dict
    void loadFromDict(const py::dict& pyDict) {
        for (auto item : pyDict) {
            std::string key = py::cast<std::string>(item.first);
            std::string value = py::cast<std::string>(item.second);
            
            // Try to parse as number
            try {
                if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    config_->set(key, doubleValue, ConfigSource::File);
                } else {
                    int intValue = std::stoi(value);
                    config_->set(key, intValue, ConfigSource::File);
                }
            } catch (...) {
                config_->set(key, value, ConfigSource::File);
            }
        }
    }

    // Convert to Python dict
    py::dict toDict() const {
        py::dict result;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            auto value = config_->get<std::string>(key);
            if (value) {
                result[key] = *value;
            } else {
                auto intValue = config_->get<int64_t>(key);
                if (intValue) {
                    result[key] = *intValue;
                } else {
                    auto doubleValue = config_->get<double>(key);
                    if (doubleValue) {
                        result[key] = *doubleValue;
                    } else {
                        auto boolValue = config_->get<bool>(key);
                        if (boolValue) {
                            result[key] = *boolValue;
                        }
                    }
                }
            }
        }
        return result;
    }

    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }

    // Reset to defaults
    void reset() {
        config_->clear();
    }

    // Create brain with this configuration
    std::shared_ptr<Brain> createBrain() {
        return std::make_shared<Brain>(config_);
    }

    // Get underlying config
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }

private:
    std::shared_ptr<Config> config_;
};

// Enhanced brain interface with additional functionality
class EnhancedPythonBrain {
public:
    EnhancedPythonBrain(std::shared_ptr<Config> config) {
        brain_ = std::make_shared<Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }

    // Step with detailed statistics
    struct StepResult {
        size_t stepNumber;
        float firingRate;
        float eiratio;
        size_t activeNeurons;
        size_t totalSpikes;
        float predictionError;
        float noveltyLevel;
        float curiosityLevel;
        float dopamineLevel;
        float developmentStage;
        bool simulationComplete;
    };

    StepResult step(size_t stepNumber, float time = 0.0f) {
        StepResult result;
        
        // Store state before step
        size_t initialSpikes = brain_->getTotalSpikeCount();
        float initialFiringRate = brain_->getAverageFiringRate();
        float initialEIRatio = brain_->getExcitationInhibitionRatio();
        
        // Perform step
        brain_->step(stepNumber, time);
        
        // Calculate statistics
        result.stepNumber = stepNumber;
        result.firingRate = brain_->getAverageFiringRate();
        result.eiratio = brain_->getExcitationInhibitionRatio();
        result.activeNeurons = brain_->getActiveNeuronCount();
        result.totalSpikes = brain_->getTotalSpikeCount() - initialSpikes;
        
        // Get additional metrics from integrated systems
        if (auto* predictionSystem = brain_->getPredictionSystem()) {
            result.predictionError = predictionSystem->getPredictionError();
        }
        
        if (auto* noveltySystem = brain_->getNovelty()) {
            result.noveltyLevel = noveltySystem->getNoveltyLevel();
        }
        
        if (auto* curiositySystem = brain_->getCuriosity()) {
            result.curiosityLevel = curiositySystem->getCuriosityLevel();
        }
        
        if (auto* dopamineSystem = brain_->getDopamine()) {
            result.dopamineLevel = dopamineSystem->getLevel();
        }
        
        if (auto* developmentSystem = brain_->getDevelopmentSystem()) {
            result.developmentStage = static_cast<float>(developmentSystem->getDevelopmentalStage());
        }
        
        result.simulationComplete = true;
        
        return result;
    }

    // Batch step with performance monitoring
    std::vector<StepResult> batchStep(size_t startStep, size_t endStep, float timeStep = 0.001f) {
        std::vector<StepResult> results;
        for (size_t step = startStep; step <= endStep; ++step) {
            float time = step * timeStep;
            results.push_back(step(step, time));
        }
        return results;
    }

    // Get brain statistics
    py::dict getStatistics() const {
        py::dict stats;
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["firing_rate"] = brain_->getAverageFiringRate();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["region_count"] = brain_->getRegionCount();
        
        // Add memory system statistics
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            stats["working_memory_capacity"] = workingMemory->getCapacity();
            stats["working_memory_size"] = workingMemory->getCurrentSize();
        }
        
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            stats["episodic_memory_count"] = episodicMemory->getEpisodeCount();
        }
        
        return stats;
    }

    // Get brain state as Python dict
    py::dict getBrainState() const {
        py::dict state;
        
        // Basic brain statistics
        state["total_neurons"] = brain_->getTotalNeuronCount();
        state["total_synapses"] = brain_->getTotalSynapseCount();
        state["firing_rate"] = brain_->getAverageFiringRate();
        state["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        state["total_spikes"] = brain_->getTotalSpikeCount();
        
        // Get region information
        auto regionIds = brain_->getRegionIds();
        py::list regions;
        for (const auto& regionId : regionIds) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                py::dict regionInfo;
                regionInfo["id"] = static_cast<size_t>(regionId.value);
                regionInfo["neuron_count"] = region->getAllNeurons().size();
                regionInfo["synapse_count"] = region->getSynapses().size();
                regions.append(regionInfo);
            }
        }
        state["regions"] = regions;
        
        // Get development stage
        if (auto* developmentSystem = brain_->getDevelopmentSystem()) {
            state["developmental_stage"] = static_cast<size_t>(developmentSystem->getDevelopmentalStage());
        }
        
        return state;
    }

    // Run comprehensive experiment
    py::dict runExperiment(const py::dict& config) {
        // Convert Python dict to experiment config
        Phase6Config expConfig;
        
        if (config.contains("neuronCount")) {
            expConfig.neuronCount = config["neuronCount"].cast<size_t>();
        }
        
        if (config.contains("maxSteps")) {
            expConfig.maxSteps = config["maxSteps"].cast<size_t>();
        }
        
        if (config.contains("connectionProbability")) {
            expConfig.connectionProbability = config["connectionProbability"].cast<float>();
        }
        
        if (config.contains("enableCheckpointing")) {
            expConfig.enableCheckpointing = config["enableCheckpointing"].cast<bool>();
        }
        
        if (config.contains("enableReplay")) {
            expConfig.enableReplay = config["enableReplay"].cast<bool>();
        }
        
        if (config.contains("enableDevelopment")) {
            expConfig.enableDevelopment = config["enableDevelopment"].cast<bool>();
        }
        
        // Run experiment
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(expConfig);
        
        // Convert result to Python dict
        py::dict expResult;
        expResult["totalReward"] = result.totalReward;
        expResult["avgRewardPerStep"] = result.avgRewardPerStep;
        expResult["overallLearningEfficiency"] = result.overallLearningEfficiency;
        expResult["finalPerformance"] = result.finalPerformance;
        expResult["memoryCapacity"] = result.memoryCapacity;
        expResult["predictionAccuracy"] = result.predictionAccuracy;
        expResult["generalizationAbility"] = result.generalizationAbility;
        expResult["memoryEpisodesStored"] = result.memoryEpisodesStored;
        expResult["dopamineLevel"] = result.dopamineLevel;
        expResult["curiosityLevel"] = result.curiosityLevel;
        expResult["noveltyLevel"] = result.noveltyLevel;
        expResult["developmentStage"] = static_cast<size_t>(result.developmentStage);
        expResult["memoryWorkingMemoryIntegrated"] = result.memoryWorkingMemoryIntegrated;
        expResult["memoryEpisodicMemoryIntegrated"] = result.memoryEpisodicMemoryIntegrated;
        expResult["memoryAssociativeMemoryIntegrated"] = result.memoryAssociativeMemoryIntegrated;
        expResult["neuromodulationIntegrated"] = result.neuromodulationIntegrated;
        expResult["predictionIntegrated"] = result.predictionIntegrated;
        expResult["developmentIntegrated"] = result.developmentIntegrated;
        expResult["checkpointingWorks"] = result.checkpointingWorks;
        expResult["replayWorks"] = result.replayWorks;
        expResult["totalWallClockTime"] = result.totalWallClockTime;
        
        return expResult;
    }

    // Save brain state to file
    bool saveBrain(const std::string& filepath) {
        return brain_->save(filepath);
    }

    // Load brain state from file
    bool loadBrain(const std::string& filepath) {
        return brain_->load(filepath);
    }

    // Reset brain
    void resetBrain() {
        brain_->reset();
        brain_->initialize();
    }

    // Get underlying brain
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }

private:
    std::shared_ptr<Brain> brain_;
};

// Command line interface wrapper for Python
class PythonNLMCLI {
public:
    PythonNLMCLI(std::shared_ptr<Config> config) {
        cli_ = createNLMCLI(config);
    }

    // Execute command
    bool execute(const std::string& command, const std::vector<std::string>& args = {}) {
        return cli_->executeCommand(command, args);
    }

    // Set brain
    void setBrain(std::shared_ptr<Brain> brain) {
        cli_->setBrain(brain);
    }

private:
    std::unique_ptr<NLMCommandLineInterface> cli_;
};

// Enhanced Python module initialization
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = "Enhanced Python bindings for NLM (Neural Learning Machine) - Phase 6 integration";

    // Version information
    m.attr("__version__") = "0.6.0";
    m.attr("__phase__") = "Phase 6: Final Integration";

    // Enhanced configuration class
    py::class_<EnhancedPythonConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("set", static_cast<void(EnhancedPythonConfig::*)(const std::string&, int)>(&EnhancedPythonConfig::set<int>))
        .def("set", static_cast<void(EnhancedPythonConfig::*)(const std::string&, double)>(&EnhancedPythonConfig::set<double>))
        .def("set", static_cast<void(EnhancedPythonConfig::*)(const std::string&, bool)>(&EnhancedPythonConfig::set<bool>))
        .def("set", static_cast<void(EnhancedPythonConfig::*)(const std::string&, const std::string&)>(&EnhancedPythonConfig::set<std::string>))
        .def("get", static_cast<int(EnhancedPythonConfig::*)(const std::string&, int)>(&EnhancedPythonConfig::get<int>))
        .def("get", static_cast<double(EnhancedPythonConfig::*)(const std::string&, double)>(&EnhancedPythonConfig::get<double>))
        .def("get", static_cast<bool(EnhancedPythonConfig::*)(const std::string&, bool)>(&EnhancedPythonConfig::get<bool>))
        .def("get", static_cast<std::string(EnhancedPythonConfig::*)(const std::string&, const std::string&)>(&EnhancedPythonConfig::get<std::string>))
        .def("getKeys", &EnhancedPythonConfig::getKeys)
        .def("loadFromJSON", &EnhancedPythonConfig::loadFromJSON)
        .def("saveToJSON", &EnhancedPythonConfig::saveToJSON)
        .def("loadFromDict", &EnhancedPythonConfig::loadFromDict)
        .def("toDict", &EnhancedPythonConfig::toDict)
        .def("summary", &EnhancedPythonConfig::summary)
        .def("reset", &EnhancedPythonConfig::reset)
        .def("createBrain", &EnhancedPythonConfig::createBrain)
        .def("__repr__", [](const EnhancedPythonConfig& config) {
            return "EnhancedConfig with " + std::to_string(config.getKeys().size()) + " entries";
        });

    // Enhanced brain class
    py::class_<EnhancedPythonBrain>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("step", static_cast<StepResult(EnhancedPythonBrain::*)(size_t, float)>(&EnhancedPythonBrain::step))
        .def("batchStep", &EnhancedPythonBrain::batchStep)
        .def("getStatistics", &EnhancedPythonBrain::getStatistics)
        .def("getBrainState", &EnhancedPythonBrain::getBrainState)
        .def("runExperiment", &EnhancedPythonBrain::runExperiment)
        .def("saveBrain", &EnhancedPythonBrain::saveBrain)
        .def("loadBrain", &EnhancedPythonBrain::loadBrain)
        .def("resetBrain", &EnhancedPythonBrain::resetBrain)
        .def("__repr__", [](const EnhancedPythonBrain& brain) {
            auto stats = brain.getStatistics();
            return "EnhancedBrain with " + std::to_string(stats["total_neurons"].cast<size_t>()) + " neurons";
        });

    // Command line interface class
    py::class_<PythonNLMCLI>(m, "NLMCLI")
        .def(py::init<std::shared_ptr<Config>>())
        .def("execute", &PythonNLMCLI::execute)
        .def("setBrain", &PythonNLMCLI::setBrain);

    // Enhanced simulation functions
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        return std::make_unique<EnhancedPythonConfig>();
    });

    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) {
        return std::make_unique<EnhancedPythonBrain>(config);
    });

    m.def("runEnhancedExperiment", [](std::shared_ptr<Config> config, const py::dict& experimentConfig) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto enhancedBrain = std::make_unique<EnhancedPythonBrain>(config);
        return enhancedBrain->runExperiment(experimentConfig);
    });

    // Performance monitoring functions
    m.def("profileBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict profileStats;
        profileStats["steps_performed"] = steps;
        profileStats["total_time_ms"] = duration.count();
        profileStats["steps_per_second"] = steps / (duration.count() / 1000.0);
        profileStats["avg_time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        
        return profileStats;
    });

    // Utility functions
    m.def("generateBrainReport", [](std::shared_ptr<Brain> brain) {
        auto config = std::make_shared<Config>();
        auto explorer = createBrainExplorer(brain);
        return explorer->generateReport();
    });

    m.def("createBatchConfig", [](const py::list& keyValuePairs) {
        auto config = std::make_shared<Config>();
        for (auto item : keyValuePairs) {
            auto pair = item.cast<py::tuple>();
            std::string key = pair[0].cast<std::string>();
            std::string value = pair[1].cast<std::string>();
            config->set(key, value, ConfigSource::Runtime);
        }
        return config;
    });

    // Constants
    m.attr("CONFIG_DEFAULT") = "default";
    m.attr("CONFIG_EXPERIMENT") = "experiment";
    m.attr("CONFIG_RESEARCH") = "research";
    m.attr("DEVELOPMENT_STAGES") = py::dict()
        .def("insert", py::arg("Initial"), 0)
        .def("insert", py::arg("CriticalPeriod"), 1)
        .def("insert", py::arg("Maturation"), 2)
        .def("insert", py::arg("Adult"), 3)
        .def("insert", py::arg("Aging"), 4);

    // Error types
    py::register_exception<PyException>(m, "PyException");
}

} // namespace nlm