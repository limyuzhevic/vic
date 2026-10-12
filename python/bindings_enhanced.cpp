#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

// NLM includes
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ResearchMetrics.hpp"
#include "../src/experiments/ResearchTools.hpp"
#include "../src/visualization/NLMLiveVisualization.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for enhanced bindings
class BrainManager;
class ExperimentManager;
class ResearchSession;
class PythonConfig;
class MatplotlibVisualizer;
class PlotlyVisualizer;
class PandasDataFrameExporter;
class StatisticalAnalysis;
class LearningExperimentAPI;
class IntegrationTestAPI;
class ParameterTuningAPI;
class AblationStudyAPI;
class CrossValidationAPI;

// BrainManager - High-level brain management
class BrainManager {
public:
    BrainManager() : config_(std::make_shared<Config>()) {}
    
    explicit BrainManager(std::shared_ptr<Config> config) : config_(config) {}
    
    BrainManager(const std::string& configPath) {
        config_ = std::make_shared<Config>();
        if (!configPath.empty()) {
            config_->loadFromFile(configPath);
        }
    }
    
    BrainManager(const std::string& brainType, const std::string& configPath) {
        config_ = std::make_shared<Config>();
        if (!configPath.empty()) {
            config_->loadFromFile(configPath);
        }
        // Set brain type in config
        if (brainType == "standard") {
            config_->setConfigValue("brain.type", "standard");
        } else if (brainType == "development") {
            config_->setConfigValue("brain.type", "developmental");
        }
    }
    
    // Brain lifecycle management
    bool initialize() {
        brain_ = std::make_shared<Brain>(config_);
        return brain_ ? brain_->initialize() : false;
    }
    
    std::shared_ptr<Brain> getBrain() {
        if (!brain_) {
            initialize();
        }
        return brain_;
    }
    
    std::shared_ptr<Brain> runExperiment(size_t steps, 
                                         bool recordMetrics = true,
                                         const std::string& experimentName = "default") {
        if (!brain_) {
            initialize();
        }
        
        // Create experiment if requested
        std::shared_ptr<Experiment> experiment;
        if (recordMetrics) {
            experiment = std::make_shared<Experiment>(experimentName);
            experiment->setStartStep(0);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step);
            
            if (experiment) {
                // Record basic metrics
                auto spikeSys = brain_->getSpikeSystem();
                size_t neuronCount = spikeSys ? spikeSys->getTotalNeuronCount() : 0;
                size_t synapseCount = spikeSys ? spikeSys->getTotalSynapseCount() : 0;
                size_t firingCount = spikeSys ? spikeSys->getFiringNeuronCount() : 0;
                float avgFiringRate = spikeSys ? spikeSys->getAverageFiringRate() : 0.0f;
                
                experiment->recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
            }
        }
        
        if (experiment) {
            experiment->setEndStep(steps - 1);
        }
        
        return brain_;
    }
    
    // State management
    bool saveState(const std::string& path) {
        if (!brain_) return false;
        return brain_->save(path);
    }
    
    bool loadState(const std::string& path) {
        if (!brain_) initialize();
        if (!brain_) return false;
        return brain_->load(path);
    }
    
    // Metrics and analysis
    std::map<std::string, double> getMetrics() {
        std::map<std::string, double> metrics;
        
        if (brain_) {
            auto spikeSys = brain_->getSpikeSystem();
            if (spikeSys) {
                metrics["total_spike_count"] = brain_->getTotalSpikeCount();
                metrics["active_neuron_count"] = brain_->getActiveNeuronCount();
                metrics["firing_neuron_count"] = brain_->getFiringNeuronCount();
                metrics["average_firing_rate"] = brain_->getAverageFiringRate();
                metrics["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            }
            
            // Get developmental stage
            auto stage = brain_->getDevelopmentalStage();
            metrics["developmental_stage"] = static_cast<int>(stage);
        }
        
        return metrics;
    }
    
    // Visualization
    std::string visualize() {
        std::ostringstream oss;
        oss << "Brain Visualization:\n";
        oss << "===================\n";
        oss << "Brain Type: Standard\n";
        oss << "Regions: " << (brain_ ? brain_->getRegionCount() : 0) << "\n";
        oss << "Neurons: " << (brain_ ? brain_->getTotalNeuronCount() : 0) << "\n";
        oss << "Synapses: " << (brain_ ? brain_->getTotalSynapseCount() : 0) << "\n";
        oss << "Active Neurons: " << (brain_ ? brain_->getActiveNeuronCount() : 0) << "\n";
        oss << "Firing Neurons: " << (brain_ ? brain_->getFiringNeuronCount() : 0) << "\n";
        
        return oss.str();
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() {
        return config_;
    }
    
    // Convenience functions for common operations
    void setLearningRate(float rate) {
        config_->setConfigValue("learning_rate", std::to_string(rate));
    }
    
    float getLearningRate() const {
        std::string value = config_->getConfigValue("learning_rate");
        try {
            return std::stof(value);
        } catch (...) {
            return 0.01f;
        }
    }
    
    void setNeuronCount(size_t count) {
        config_->setConfigValue("neuron_count", std::to_string(count));
    }
    
    size_t getNeuronCount() const {
        std::string value = config_->getConfigValue("neuron_count");
        try {
            return std::stoul(value);
        } catch (...) {
            return 1000;
        }
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

// PythonConfig - Python-friendly configuration
class PythonConfig {
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    
    PythonConfig(const std::string& filepath) : config_(std::make_shared<Config>()) {
        if (!filepath.empty()) {
            config_->loadFromFile(filepath);
        }
    }
    
    PythonConfig(const std::string& jsonContent) : config_(std::make_shared<Config>()) {
        // Parse JSON (simplified implementation)
        // For full JSON support, would use a JSON library
        if (!jsonContent.empty()) {
            // Simple key=value parsing for demonstration
            size_t pos = 0;
            while ((pos = jsonContent.find('{', pos)) != std::string::npos) {
                size_t endPos = jsonContent.find('}', pos);
                if (endPos != std::string::npos) {
                    std::string pair = jsonContent.substr(pos + 1, endPos - pos - 2);
                    size_t equalsPos = pair.find('=');
                    if (equalsPos != std::string::npos) {
                        std::string key = pair.substr(0, equalsPos);
                        std::string value = pair.substr(equalsPos + 1);
                        // Trim whitespace
                        key.erase(0, key.find_first_not_of(" \t\n\r"));
                        key.erase(key.find_last_not_of(" \t\n\r") + 1);
                        value.erase(0, value.find_first_not_of(" \t\n\r"));
                        value.erase(value.find_last_not_of(" \t\n\r") + 1);
                        config_->setConfigValue(key, value);
                    }
                }
                pos = endPos + 1;
            }
        }
    }
    
    // Load from various formats
    void load_json(const std::string& filepath) {
        config_->loadFromFile(filepath);
    }
    
    void load_yaml(const std::string& filepath) {
        // YAML parsing would require a YAML library
        // For now, just use JSON as fallback
        config_->loadFromFile(filepath);
    }
    
    void save_json(const std::string& filepath) const {
        config_->saveToFile(filepath);
    }
    
    // Pythonic property access
    float getLearningRate() const {
        std::string value = config_->getConfigValue("learning_rate");
        try {
            return std::stof(value);
        } catch (...) {
            return 0.01f;
        }
    }
    
    void setLearningRate(float value) {
        config_->setConfigValue("learning_rate", std::to_string(value));
    }
    
    size_t getNeuronCount() const {
        std::string value = config_->getConfigValue("neuron_count");
        try {
            return std::stoul(value);
        } catch (...) {
            return 1000;
        }
    }
    
    void setNeuronCount(size_t value) {
        config_->setConfigValue("neuron_count", std::to_string(value));
    }
    
    std::string getString(const std::string& key) const {
        return config_->getConfigValue(key);
    }
    
    void setString(const std::string& key, const std::string& value) {
        config_->setConfigValue(key, value);
    }
    
    int getInt(const std::string& key) const {
        std::string value = config_->getConfigValue(key);
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }
    
    void setInt(const std::string& key, int value) {
        config_->setConfigValue(key, std::to_string(value));
    }
    
    double getFloat(const std::string& key) const {
        std::string value = config_->getConfigValue(key);
        try {
            return std::stod(value);
        } catch (...) {
            return 0.0;
        }
    }
    
    void setFloat(const std::string& key, double value) {
        config_->setConfigValue(key, std::to_string(value));
    }
    
    bool getBool(const std::string& key) const {
        std::string value = config_->getConfigValue(key);
        // Convert string to bool (true/false, yes/no, 1/0)
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == "true" || value == "yes" || value == "1");
    }
    
    void setBool(const std::string& key, bool value) {
        config_->setConfigValue(key, value ? "true" : "false");
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Get underlying C++ config
    std::shared_ptr<Config> getConfig() {
        return config_;
    }
    
    // Factory methods for common configurations
    static PythonConfig createDefault() {
        return PythonConfig();
    }
    
    static PythonConfig createLearningExperiment(float learningRate = 0.01f,
                                                  size_t neuronCount = 1000,
                                                  size_t maxSteps = 1000) {
        PythonConfig config;
        config.setLearningRate(learningRate);
        config.setNeuronCount(neuronCount);
        config.setString("experiment.name", "learning_experiment");
        config.setInt("experiment.max_steps", maxSteps);
        config.setString("brain.type", "standard");
        config.setBool("brain.enable_plasticity", true);
        config.setBool("brain.enable_development", false);
        return config;
    }
    
    static PythonConfig createIntegrationTest(size_t neuronCount = 500,
                                              size_t regionCount = 1) {
        PythonConfig config;
        config.setNeuronCount(neuronCount);
        config.setInt("region.count", regionCount);
        config.setString("test.type", "integration");
        config.setBool("test.enable_checkpointing", true);
        config.setBool("test.enable_replay", true);
        config.setString("brain.type", "development");
        config.setBool("brain.enable_development", true);
        return config;
    }
    
    static PythonConfig createParameterTuning(const std::string& paramSpaceJson) {
        // This would parse a JSON parameter space specification
        return createDefault();
    }
    
private:
    std::shared_ptr<Config> config_;
};

// ResearchSession - Individual experiment session management with context manager support
class ResearchSession {
public:
    ResearchSession() : active_(false), stepCount_(0) {}
    
    ResearchSession(std::shared_ptr<Brain> brain) : brain_(brain), active_(false), stepCount_(0) {}
    
    ResearchSession(std::shared_ptr<BrainManager> brainManager) : brainManager_(brainManager), active_(false), stepCount_(0) {
        if (brainManager_) {
            brain_ = brainManager_->getBrain();
        }
    }
    
    // Context manager support
    ResearchSession& __enter__() {
        active_ = true;
        stepCount_ = 0;
        
        // Initialize if needed
        if (brain_ && !brain_->getConfig()->getKeys().empty()) {
            // Brain is already initialized
        } else if (brainManager_) {
            brainManager_->initialize();
        } else if (!brain_) {
            brain_ = std::make_shared<Brain>(std::make_shared<Config>());
        }
        
        return *this;
    }
    
    void __exit__(int exc_type, int exc_val, const std::string& exc_tb) {
        active_ = false;
        // Collect final metrics
        collectMetrics();
        
        // Save if enabled
        if (saveOnExit_) {
            saveState(exitPath_);
        }
    }
    
    // Session management
    bool start() {
        if (active_) return false;
        active_ = true;
        stepCount_ = 0;
        
        // Initialize if needed
        if (brain_ && !brain_->getConfig()->getKeys().empty()) {
            // Brain is already initialized
        } else if (brainManager_) {
            brainManager_->initialize();
        } else if (!brain_) {
            brain_ = std::make_shared<Brain>(std::make_shared<Config>());
        }
        
        return active_;
    }
    
    void stop() {
        if (!active_) return;
        
        active_ = false;
        collectMetrics();
        
        // Save if enabled
        if (saveOnExit_) {
            saveState(exitPath_);
        }
    }
    
    bool isActive() const {
        return active_;
    }
    
    // Run simulation steps
    std::shared_ptr<Brain> runStep(size_t step) {
        if (!active_ || !brain_) return nullptr;
        
        brain_->step(step);
        stepCount_ = step + 1;
        
        return brain_;
    }
    
    std::shared_ptr<Brain> runSteps(size_t steps) {
        if (!active_ || !brain_) return nullptr;
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step);
            stepCount_ = step + 1;
        }
        
        return brain_;
    }
    
    // Metrics collection
    std::map<std::string, double> collectMetrics() {
        std::map<std::string, double> metrics;
        
        if (brain_) {
            auto spikeSys = brain_->getSpikeSystem();
            if (spikeSys) {
                metrics["total_spike_count"] = brain_->getTotalSpikeCount();
                metrics["active_neuron_count"] = brain_->getActiveNeuronCount();
                metrics["firing_neuron_count"] = brain_->getFiringNeuronCount();
                metrics["average_firing_rate"] = brain_->getAverageFiringRate();
                metrics["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
            }
            
            // Get developmental stage
            auto stage = brain_->getDevelopmentalStage();
            metrics["developmental_stage"] = static_cast<int>(stage);
            
            // Get config metrics
            auto config = brain_->getConfig();
            if (config) {
                std::string lr = config->getConfigValue("learning_rate");
                if (!lr.empty()) {
                    try {
                        metrics["learning_rate"] = std::stof(lr);
                    } catch (...) {}
                }
            }
        }
        
        metrics["session_step_count"] = stepCount_;
        metrics["session_active"] = active_ ? 1.0 : 0.0;
        
        return metrics;
    }
    
    // Get results
    std::map<std::string, py::object> getResults() {
        std::map<std::string, py::object> results;
        
        // Get metrics
        auto metrics = collectMetrics();
        py::dict metricsDict;
        for (const auto& pair : metrics) {
            if (std::holds_alternative<int>(pair.second)) {
                metricsDict[pair.first.c_str()] = std::get<int>(pair.second);
            } else if (std::holds_alternative<double>(pair.second)) {
                metricsDict[pair.first.c_str()] = std::get<double>(pair.second);
            } else if (std::holds_alternative<size_t>(pair.second)) {
                metricsDict[pair.first.c_str()] = std::get<size_t>(pair.second);
            }
        }
        results["metrics"] = metricsDict;
        
        // Get brain state if available
        if (brain_) {
            results["brain"] = brain_;
        }
        
        return results;
    }
    
    // State management
    bool saveState(const std::string& path) {
        if (!brain_) return false;
        return brain_->save(path);
    }
    
    bool loadState(const std::string& path) {
        if (!brain_) return false;
        return brain_->load(path);
    }
    
    // Configuration
    void setSaveOnExit(bool save) {
        saveOnExit_ = save;
    }
    
    void setExitPath(const std::string& path) {
        exitPath_ = path;
    }
    
    std::string getExitPath() const {
        return exitPath_;
    }
    
    // Get brain manager if available
    std::shared_ptr<BrainManager> getBrainManager() const {
        return brainManager_;
    }
    
    // Get brain if available
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainManager> brainManager_;
    bool active_;
    size_t stepCount_;
    bool saveOnExit_ = false;
    std::string exitPath_;
};

// ExperimentManager - Research experiment management
class ExperimentManager {
public:
    ExperimentManager() = default;
    
    ExperimentManager(std::shared_ptr<BrainManager> brainManager) : brainManager_(brainManager) {}
    
    // Learning experiments
    std::shared_ptr<Experiment> conductLearningExperiment(const std::string& experimentName = "learning",
                                                          size_t steps = 1000,
                                                          float learningRate = 0.01f,
                                                          bool recordStepData = true) {
        if (!brainManager_) {
            brainManager_ = std::make_shared<BrainManager>();
        }
        
        // Configure for learning experiment
        auto config = brainManager_->getConfig();
        if (config) {
            config->setConfigValue("learning_rate", std::to_string(learningRate));
            config->setConfigValue("experiment.name", experimentName);
            config->setConfigValue("experiment.type", "learning");
        }
        
        // Run experiment
        auto brain = brainManager_->runExperiment(steps, true, experimentName);
        
        // Create experiment object
        auto experiment = std::make_shared<Experiment>(experimentName);
        experiment->setStartStep(0);
        experiment->setEndStep(steps - 1);
        
        if (recordStepData) {
            // Record step data from brain (simplified)
            for (SimulationStep step = 0; step < steps; ++step) {
                auto spikeSys = brain->getSpikeSystem();
                if (spikeSys) {
                    size_t neuronCount = spikeSys->getTotalNeuronCount();
                    size_t synapseCount = spikeSys->getTotalSynapseCount();
                    size_t firingCount = spikeSys->getFiringNeuronCount();
                    float avgFiringRate = spikeSys->getAverageFiringRate();
                    
                    experiment->recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
                    
                    // Record learning metrics
                    double learningProgress = static_cast<double>(step) / steps;
                    experiment->recordMetric("learning_progress", learningProgress);
                    
                    // Record firing rate
                    experiment->recordMetric("firing_rate", avgFiringRate);
                }
            }
        }
        
        return experiment;
    }
    
    // Integration tests
    std::shared_ptr<Experiment> runIntegrationTest(const std::string& testName = "integration_test",
                                                   size_t steps = 500,
                                                   bool enableCheckpointing = true,
                                                   bool enableReplay = true) {
        if (!brainManager_) {
            brainManager_ = std::make_shared<BrainManager>();
        }
        
        // Configure for integration test
        auto config = brainManager_->getConfig();
        if (config) {
            config->setConfigValue("test.name", testName);
            config->setConfigValue("test.type", "integration");
            config->setConfigValue("test.checkpointing", enableCheckpointing ? "true" : "false");
            config->setConfigValue("test.replay", enableReplay ? "true" : "false");
            config->setConfigValue("brain.type", "development");
            config->setConfigValue("brain.enable_development", "true");
        }
        
        // Run integration test
        auto brain = brainManager_->runExperiment(steps, true, testName);
        
        // Create experiment object
        auto experiment = std::make_shared<Experiment>(testName);
        experiment->setStartStep(0);
        experiment->setEndStep(steps - 1);
        
        // Record integration-specific metrics
        experiment->recordMetric("checkpointing_enabled", enableCheckpointing ? 1.0 : 0.0);
        experiment->recordMetric("replay_enabled", enableReplay ? 1.0 : 0.0);
        
        return experiment;
    }
    
    // Parameter tuning
    std::map<std::string, double> parameterTuning(const std::string& paramSpaceJson,
                                                  size_t nTrials = 10,
                                                  const std::string& metric = "performance") {
        // This is a simplified implementation - in practice, this would use
        // a more sophisticated optimization algorithm
        
        std::map<std::string, double> bestParams;
        double bestScore = 0.0;
        
        for (size_t trial = 0; trial < nTrials; ++trial) {
            // Generate random parameters within the space (simplified)
            std::map<std::string, double> params;
            params["learning_rate"] = 0.001f * (1.0 + static_cast<double>(rand()) / RAND_MAX);
            params["neuron_count"] = 500 + static_cast<size_t>(rand() % 1500);
            params["connection_probability"] = 0.05f + static_cast<float>(rand()) / RAND_MAX * 0.2f;
            
            // Run experiment with these parameters
            if (brainManager_) {
                auto config = brainManager_->getConfig();
                if (config) {
                    config->setConfigValue("learning_rate", std::to_string(params["learning_rate"]));
                    config->setConfigValue("neuron_count", std::to_string(params["neuron_count"]));
                    config->setConfigValue("connection_probability", std::to_string(params["connection_probability"]));
                }
                
                // Run experiment
                auto brain = brainManager_->runExperiment(100, false);
                
                // Evaluate based on the specified metric
                double score = 0.0;
                auto metrics = brainManager_->getMetrics();
                
                if (metric == "performance") {
                    score = metrics["average_firing_rate"] * metrics["total_spike_count"];
                } else if (metric == "learning") {
                    score = metrics["average_firing_rate"] / (metrics["average_firing_rate"] + 0.1);
                } else if (metric == "stability") {
                    // Higher stability = more consistent firing rates
                    score = 1.0 / (1.0 + std::abs(metrics["average_firing_rate"] - 0.1));
                }
                
                // Update best parameters if this trial is better
                if (score > bestScore) {
                    bestScore = score;
                    bestParams = params;
                }
            }
        }
        
        return bestParams;
    }
    
    // Ablation studies
    std::shared_ptr<Experiment> ablationStudy(const std::vector<std::string>& componentsToDisable,
                                             const std::string& studyName = "ablation_study",
                                             size_t steps = 500) {
        if (!brainManager_) {
            brainManager_ = std::make_shared<BrainManager>();
        }
        
        // Configure for ablation study
        auto config = brainManager_->getConfig();
        if (config) {
            config->setConfigValue("study.name", studyName);
            config->setConfigValue("study.type", "ablation");
            
            // Set disabled components
            std::string disabledComponents;
            for (size_t i = 0; i < componentsToDisable.size(); ++i) {
                if (i > 0) disabledComponents += ",";
                disabledComponents += componentsToDisable[i];
            }
            config->setConfigValue("study.disabled_components", disabledComponents);
            
            // Disable components in configuration
            for (const auto& component : componentsToDisable) {
                config->setConfigValue("component." + component + ".enabled", "false");
            }
        }
        
        // Run ablation study
        auto brain = brainManager_->runExperiment(steps, true, studyName);
        
        // Create experiment object
        auto experiment = std::make_shared<Experiment>(studyName);
        experiment->setStartStep(0);
        experiment->setEndStep(steps - 1);
        
        // Record ablation metrics
        experiment->recordMetric("disabled_components_count", static_cast<double>(componentsToDisable.size()));
        
        return experiment;
    }
    
    // Cross-validation
    std::map<std::string, double> crossValidation(const std::vector<std::string>& paramSets,
                                                  size_t nFolds = 5,
                                                  const std::string& metric = "accuracy") {
        std::map<std::string, double> results;
        
        for (const auto& paramSet : paramSets) {
            // Parse parameter set (simplified)
            std::map<std::string, double> params;
            // In practice, this would parse the paramSet properly
            
            double foldScore = 0.0;
            for (size_t fold = 0; fold < nFolds; ++fold) {
                if (brainManager_) {
                    // Apply parameters
                    auto config = brainManager_->getConfig();
                    if (config) {
                        config->setConfigValue("learning_rate", std::to_string(params["learning_rate"]));
                        config->setConfigValue("neuron_count", std::to_string(params["neuron_count"]));
                    }
                    
                    // Run experiment
                    auto brain = brainManager_->runExperiment(100, false);
                    
                    // Calculate fold score
                    auto metrics = brainManager_->getMetrics();
                    if (metric == "accuracy") {
                        foldScore += metrics["average_firing_rate"] / (metrics["average_firing_rate"] + 0.1);
                    } else if (metric == "performance") {
                        foldScore += metrics["average_firing_rate"] * metrics["total_spike_count"];
                    }
                }
            }
            
            results[paramSet] = foldScore / nFolds;
        }
        
        return results;
    }
    
    // Get brain manager
    std::shared_ptr<BrainManager> getBrainManager() const {
        return brainManager_;
    }
    
    // Set brain manager
    void setBrainManager(std::shared_ptr<BrainManager> brainManager) {
        brainManager_ = brainManager;
    }
    
private:
    std::shared_ptr<BrainManager> brainManager_;
};

// Research-specific API classes
class LearningExperimentAPI {
public:
    LearningExperimentAPI() = default;
    
    std::shared_ptr<Experiment> runBasicLearning(float learningRate = 0.01f,
                                                  size_t steps = 1000,
                                                  size_t neuronCount = 1000) {
        PythonConfig config;
        config.setLearningRate(learningRate);
        config.setNeuronCount(neuronCount);
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.conductLearningExperiment("basic_learning", steps, learningRate, true);
    }
    
    std::shared_ptr<Experiment> runPlasticityLearning(float learningRate = 0.01f,
                                                      size_t steps = 2000,
                                                      bool enableSTDP = true,
                                                      bool enableHebbian = true) {
        PythonConfig config;
        config.setLearningRate(learningRate);
        config.setString("plasticity.stdp.enabled", enableSTDP ? "true" : "false");
        config.setString("plasticity.hebbian.enabled", enableHebbian ? "true" : "false");
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.conductLearningExperiment("plasticity_learning", steps, learningRate, true);
    }
    
    std::shared_ptr<Experiment> runDevelopmentLearning(float learningRate = 0.001f,
                                                       size_t steps = 5000,
                                                       bool enableDevelopment = true) {
        PythonConfig config;
        config.setLearningRate(learningRate);
        config.setString("brain.enable_development", enableDevelopment ? "true" : "false");
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.conductLearningExperiment("development_learning", steps, learningRate, true);
    }
};

class IntegrationTestAPI {
public:
    IntegrationTestAPI() = default;
    
    std::shared_ptr<Experiment> runCompleteIntegration(size_t steps = 10000,
                                                       bool enableCheckpointing = true,
                                                       bool enableReplay = true,
                                                       bool enableDevelopment = true) {
        PythonConfig config;
        config.setString("test.checkpointing", enableCheckpointing ? "true" : "false");
        config.setString("test.replay", enableReplay ? "false" : "false");
        config.setString("brain.enable_development", enableDevelopment ? "true" : "false");
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.runIntegrationTest("complete_integration", steps, enableCheckpointing, enableReplay);
    }
    
    std::shared_ptr<Experiment> runMemoryIntegration(size_t steps = 3000) {
        PythonConfig config;
        config.setString("memory.working_memory.enabled", "true");
        config.setString("memory.episodic_memory.enabled", "true");
        config.setString("memory.associative_memory.enabled", "true");
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.runIntegrationTest("memory_integration", steps, false, true);
    }
    
    std::shared_ptr<Experiment> runNeuromodulationIntegration(size_t steps = 2000) {
        PythonConfig config;
        config.setString("neuromodulation.dopamine.enabled", "true");
        config.setString("neuromodulation.curiosity.enabled", "true");
        config.setString("neuromodulation.novelty.enabled", "true");
        config.setString("neuromodulation.prediction_error.enabled", "true");
        
        std::shared_ptr<BrainManager> brainManager = std::make_shared<BrainManager>(config.getConfig());
        brainManager->initialize();
        
        ExperimentManager expManager(brainManager);
        return expManager.runIntegrationTest("neuromodulation_integration", steps, false, true);
    }
};

class ParameterTuningAPI {
public:
    ParameterTuningAPI() = default;
    
    std::map<std::string, double> tuneLearningRate(size_t nTrials = 20,
                                                   const std::vector<float>& learningRates = {0.001f, 0.005f, 0.01f, 0.05f, 0.1f}) {
        std::map<std::string, double> paramSpace;
        for (size_t i = 0; i < learningRates.size(); ++i) {
            paramSpace[std::to_string(i)] = learningRates[i];
        }
        
        // Create parameter space JSON (simplified)
        std::string paramSpaceJson = "{";
        for (size_t i = 0; i < learningRates.size(); ++i) {
            if (i > 0) paramSpaceJson += ",";
            paramSpaceJson += "\"learning_rate_" + std::to_string(i) + "\": " + std::to_string(learningRates[i]);
        }
        paramSpaceJson += "}";
        
        return gridSearch(paramSpaceJson, nTrials, "performance");
    }
    
    std::map<std::string, double> tuneNetworkSize(size_t nTrials = 20,
                                                  const std::vector<size_t>& neuronCounts = {500, 1000, 2000, 5000}) {
        std::map<std::string, double> paramSpace;
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            paramSpace[std::to_string(i)] = static_cast<double>(neuronCounts[i]);
        }
        
        // Create parameter space JSON (simplified)
        std::string paramSpaceJson = "{";
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            if (i > 0) paramSpaceJson += ",";
            paramSpaceJson += "\"neuron_count_" + std::to_string(i) + "\": " + std::to_string(neuronCounts[i]);
        }
        paramSpaceJson += "}";
        
        return gridSearch(paramSpaceJson, nTrials, "performance");
    }
    
    std::map<std::string, double> gridSearch(const std::string& paramSpaceJson,
                                             size_t nTrials,
                                             const std::string& metric = "performance") {
        ExperimentManager expManager;
        return expManager.parameterTuning(paramSpaceJson, nTrials, metric);
    }
    
    std::map<std::string, double> randomSearch(const std::string& paramSpaceJson,
                                               size_t nTrials,
                                               const std::string& metric = "performance") {
        // Similar to grid search but with random sampling
        ExperimentManager expManager;
        return expManager.parameterTuning(paramSpaceJson, nTrials, metric);
    }
};

class AblationStudyAPI {
public:
    AblationStudyAPI() = default;
    
    std::shared_ptr<Experiment> disableMemorySystems(size_t steps = 500) {
        ExperimentManager expManager;
        return expManager.ablationStudy({"working_memory", "episodic_memory", "associative_memory"}, 
                                       "memory_ablation", steps);
    }
    
    std::shared_ptr<Experiment> disableNeuromodulation(size_t steps = 500) {
        ExperimentManager expManager;
        return expManager.ablationStudy({"dopamine", "curiosity", "novelty", "prediction_error"}, 
                                       "neuromodulation_ablation", steps);
    }
    
    std::shared_ptr<Experiment> disablePlasticity(size_t steps = 500) {
        ExperimentManager expManager;
        return expManager.ablationStudy({"stdp", "hebbian", "structural_plasticity"}, 
                                       "plasticity_ablation", steps);
    }
    
    std::shared_ptr<Experiment> disableCognition(size_t steps = 500) {
        ExperimentManager expManager;
        return expManager.ablationStudy({"concept_formation", "neural_planner", "attentional_selection"}, 
                                       "cognition_ablation", steps);
    }
};

class CrossValidationAPI {
public:
    CrossValidationAPI() = default;
    
    std::map<std::string, double> validateLearning(size_t nFolds = 5) {
        std::vector<std::string> paramSets;
        
        // Create different parameter sets for cross-validation
        for (size_t i = 0; i < 10; ++i) {
            paramSets.push_back("params_" + std::to_string(i));
        }
        
        ExperimentManager expManager;
        return expManager.crossValidation(paramSets, nFolds, "performance");
    }
    
    std::map<std::string, double> validateMemory(size_t nFolds = 5) {
        std::vector<std::string> paramSets;
        
        for (size_t i = 0; i < 5; ++i) {
            paramSets.push_back("memory_params_" + std::to_string(i));
        }
        
        ExperimentManager expManager;
        return expManager.crossValidation(paramSets, nFolds, "accuracy");
    }
};